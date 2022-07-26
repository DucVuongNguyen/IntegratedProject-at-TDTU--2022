#include <Arduino.h>
#include "WiFi.h"
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#define FLASH_MEMORY_SIZE 10
#include <ArduinoJson.h>
DynamicJsonDocument JsonData(256);
#define MODE_SW 25
#define STATUS_LED 27
#define CTRL_BTN 33
#define BTN_LED 22
#define RELAY 32

boolean RelayStatus = false;
byte RelayAddress = 0;
String NameDevice = "V001", KeySecurity = "73029";
String url = "http://192.168.0.102:8000/api";
boolean isPost = false;
boolean check_WifiStatus();
boolean isReset(int btn);
void Reset();
String post_HTTP(String url_, String Data_, String print_);
String get_Value(String Res, String Obj, String item);
void sync_Device();
boolean save_EEPROM(byte address, byte value);
byte read_EEPROM(byte address);
boolean isState(byte sw);
boolean isPress(byte btn);
boolean WifiSmartConfig();
boolean check_WifiStatus();
boolean reconnect_Wifi();

boolean isReset(int btn)
{
  unsigned long time_ = millis();
  unsigned long holdTime = 5000;
  boolean is_Reset = false;
  while (digitalRead(btn) == 0)
  {
    if ((millis() - time_) >= holdTime)
    {
      is_Reset = true;
      break;
    }
  }

  //  Serial.print("is_Reset: ");
  //  Serial.println(is_Reset);
  return is_Reset;
}

void Reset()
{
  boolean ResetLed = LOW;
  if (isReset(CTRL_BTN))
  {
    if (check_WifiStatus())
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        post_HTTP(url + "/resetPassword", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "Reset Password");
        while (!isPost)
        {
          post_HTTP(url + "/resetPassword", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "Reset Password");
        }
      }
      if (WiFi.status() == WL_CONNECTED)
      {
        post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String(RelayStatus), "New Data Added");
        while (!isPost)
        {
          post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String(RelayStatus), "New Data Added");
        }
      }
      RelayStatus = LOW;
      save_EEPROM(RelayAddress, RelayStatus);
      for (byte i = 0; i < 20; i++)
      {
        digitalWrite(STATUS_LED, ResetLed);
        ResetLed = !ResetLed;
        Serial.println(ResetLed);
        delay(1000);
      }
      ESP.restart();
    }
  }
}

String post_HTTP(String url_, String Data_, String print_)
{
  Serial.println(print_);
  String response_ = "null";
  HTTPClient http;
  http.begin(url_);
  http.setTimeout(5000);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(Data_);
  if (httpResponseCode == 200)
  {
    response_ = http.getString();
    Serial.println(httpResponseCode);
    response_.replace("[", "");
    response_.replace("]", "");
    isPost = true;
    //    Serial.println(response);   //Print return code
  }
  else
  {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    isPost = false;
  }
  http.end(); // Free resources
  return response_;
}

String get_Value(String Res, String Obj, String item)
{
  deserializeJson(JsonData, Res);
  String DataResult_ = JsonData["DataResult"];
  //  Serial.println(DataResult_);
  deserializeJson(JsonData, DataResult_);
  String Obj_ = JsonData[Obj];
  //  Serial.println(Obj_);
  deserializeJson(JsonData, Obj_);
  String item_ = JsonData[item].as<String>();
  //  Serial.println(item_);
  String value_ = item_;
  //  Serial.println("Value: " + (String)value_);
  return value_;
}

void sync_Device()
{
  String value_ = "null";
  if (WiFi.status() == WL_CONNECTED)
  {
    String res_ = post_HTTP(url + "/readData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "New Data Readed");
    value_ = get_Value(res_, "Data", "Status");
    Serial.println("Value: " + value_);
    if (value_ == "1")
    {
      RelayStatus = true;
      digitalWrite(RELAY, RelayStatus);
    }
    if (value_ == "0")
    {
      RelayStatus = false;
      digitalWrite(RELAY, RelayStatus);
    }
  }
}

boolean save_EEPROM(byte address, byte value)
{
  boolean isSave = false;
  EEPROM.write(address, value);
  EEPROM.commit();
  Serial.println("**WRITE EEPROM**");
  Serial.println("Address: " + (String)address + " - Value: " + (String)EEPROM.read(address));
  Serial.println("*******");
  isSave = true;
}

byte read_EEPROM(byte address)
{
  byte value;
  value = EEPROM.read(address);
  Serial.println("**READ EEPROM**");
  Serial.println("Address: " + (String)address + " - Value: " + (String)value);
  Serial.println("*******");
  return value;
}

boolean isState(byte sw)
{
  unsigned long time_ = millis();
  unsigned long switcTime = 50;
  boolean is_State = false;
  while (true)
  {
    while (digitalRead(sw) == 1)
    {
      if ((millis() - time_) >= switcTime)
      {
        is_State = false;
        digitalWrite(STATUS_LED, LOW);
        //        Serial.println("Normal Mode");
        goto exit_;
      }
    }
    while (digitalRead(sw) == 0)
    {
      if ((millis() - time_) >= switcTime)
      {
        is_State = true;
        digitalWrite(STATUS_LED, HIGH);
        //        Serial.println("WifiSmartConfig Mode");
        goto exit_;
      }
    }
  }
exit_:
{
  return is_State;
}
}

boolean isPress(byte btn)
{
  unsigned long time_ = millis();
  unsigned long pressTime = 50, releasesTime = 2000;
  boolean is_Press = false;
  while (digitalRead(btn) == 0)
  {
    if ((millis() - time_) >= pressTime && (millis() - time_) < releasesTime)
    {
      is_Press = true;
      Serial.println("Button was pressed...");
      digitalWrite(BTN_LED, HIGH);
    }
  }
  digitalWrite(BTN_LED, LOW);
  return is_Press;
}

boolean WifiSmartConfig()
{
  boolean isDone = false;
  unsigned long time_ = millis(), time_blink = 200;
  unsigned long time_ConfigDone, time_ConfigAgain = 10000;
  boolean signal_ = LOW;
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone())
  {
    if ((millis() - time_) >= time_blink)
    {
      signal_ = !signal_;
      digitalWrite(STATUS_LED, signal_);
      time_ = millis();
    }
    if (!isState(MODE_SW))
    {
      goto exit_;
    }
    time_ConfigDone = millis();
  }
  Serial.println("SmartConfig done.");
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    if ((millis() - time_) >= time_blink)
    {
      signal_ = !signal_;
      digitalWrite(STATUS_LED, signal_);
      time_ = millis();
    }
    if ((millis() - time_ConfigDone) >= time_ConfigAgain)
    {
      save_EEPROM(RelayAddress, RelayStatus);
      ESP.restart();
    }
    if (!isState(MODE_SW))
    {
      goto exit_;
    }
  }
  isDone = true;
  post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String(RelayStatus), "New Data Added");
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  while (isState(MODE_SW))
  {
  }
exit_:
{
  WiFi.begin();
  return isDone;
}
}

boolean check_WifiStatus()
{
  boolean isWifiConnect = false;
  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(STATUS_LED, LOW);
    isWifiConnect = false;
  }
  else
  {
    digitalWrite(STATUS_LED, HIGH);
    isWifiConnect = true;
  }
  //  Serial.println(WiFi.RSSI());
  return isWifiConnect;
}
unsigned long previousMillis_ReconnectWifi = 0;
unsigned long interval_ReconnectWifi = 10000;
boolean reconnect_Wifi()
{
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (millis() - previousMillis_ReconnectWifi >= interval_ReconnectWifi))
  {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000);
    previousMillis_ReconnectWifi = millis();
  }
}

void TaskControl(void *pvParameters);
void TaskControl(void *pvParameters) // This is a task.
{
  delay(100);
  unsigned long previousMillis_Alive = 0;
  unsigned long interval_Alive = 60000;
  for (;;)
  {
    if (isPress(CTRL_BTN))
    {
      RelayStatus = !RelayStatus;
      digitalWrite(RELAY, RelayStatus);
      if (WiFi.status() == WL_CONNECTED)
      {
        post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String(RelayStatus), "New Data Added");
        while (!isPost)
        {
          post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String(RelayStatus), "New Data Added");
        }
      }
    }
    sync_Device();
    if (millis() - previousMillis_Alive >= interval_Alive)
    {
      post_HTTP(url + "/keepAlive", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&RSSI=" + (String)WiFi.RSSI(), "Keep Alive");
      while (!isPost)
      {
        post_HTTP(url + "/keepAlive", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&RSSI=" + (String)WiFi.RSSI(), "Keep Alive");
      }
      previousMillis_Alive = millis();
    }
  }
}
void setup()
{
  Serial.begin(115200);
  EEPROM.begin(FLASH_MEMORY_SIZE);
  Serial.println("Waitting setting... ");
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  pinMode(BTN_LED, OUTPUT);
  digitalWrite(BTN_LED, LOW);
  pinMode(RELAY, OUTPUT);
  RelayStatus = read_EEPROM(RelayAddress);
  digitalWrite(RELAY, RelayStatus);
  pinMode(MODE_SW, INPUT_PULLUP);
  pinMode(CTRL_BTN, INPUT_PULLUP);
  xTaskCreatePinnedToCore(
      TaskControl, "TaskControl" // A name just for humans
      ,
      10000 // This stack size can be checked & adjusted by reading the Stack Highwater
      ,
      NULL, 1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      NULL, 1);
  WiFi.begin();
}

void loop()
{

  while (isState(MODE_SW))
  {
    WifiSmartConfig();
  }
  while (!isState(MODE_SW))
  {
    reconnect_Wifi();
    check_WifiStatus();
    Reset();
  }
}