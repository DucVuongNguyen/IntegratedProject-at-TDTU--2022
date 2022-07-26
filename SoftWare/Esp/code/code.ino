#include <Arduino.h>
#include "WiFi.h"
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#define FLASH_MEMORY_SIZE 10
#include <ArduinoJson.h>
DynamicJsonDocument JsonData(256);
#define MODE_SW 23// không được dùng 34 35 vì 2 chân này không có điện trở kéo lên/xuống
#define NON_WIFI 22
#define RESET 32
#define MODE_LED 33 // led báo Status Wifi
#define SW 25
#define RELAY_LED 26 //led báo trạng thái Relay/BTN
#define RELAY 27


boolean RelayStatus = false;
byte RelayAddress = 0;
String NameDevice = "V000", KeySecurity = "73029";
String url = "http://192.168.0.104:8000/api";
boolean isPost = false;
boolean pre_Status = false; // biến lưu trạng thái nút nhấn


boolean isReset(int sw) {
  unsigned long time_ = millis();
  unsigned long time_out = 100;
  boolean is_Reset = false;
  while (digitalRead(sw) == 0) {
    if ((millis() - time_) >= time_out) {
      is_Reset = true;
      break;
    }
  }

  //  Serial.print("is_Reset: ");
  //  Serial.println(is_Reset);
  return is_Reset;
}

void Reset () {
  boolean ResetLed = LOW;
  if (isReset(RESET) && !isNoneWifi(NON_WIFI)) {
    if (check_Wifi()) {
      post_HTTP(url + "/resetPassword", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "Reset Password");
      while (!isPost && !isNoneWifi(NON_WIFI))
      {
        post_HTTP(url + "/resetPassword", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "Reset Password");
      }
      while (isReset(RESET) && !isNoneWifi(NON_WIFI)) {
        digitalWrite(MODE_LED, ResetLed);
        ResetLed = !ResetLed;
        Serial.println(ResetLed);
        delay (500);
      }
    }
  }
}

String post_HTTP (String url_, String Data_, String print_) {
  String response_ = "null";
  HTTPClient http;
  http.begin(url_);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(Data_);
  if (httpResponseCode == 200) {
    response_ = http.getString();
    //    Serial.println(httpResponseCode);
    response_.replace("[", "");
    response_.replace("]", "");
    isPost = true;
    //    Serial.println(response);   //Print return code
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    isPost = false;
  }
  http.end();  //Free resources
  Serial.println(print_);
  return response_;
}

String get_Value(String Res) {
  deserializeJson(JsonData, Res);
  String DataResult_ = JsonData["DataResult"];
  Serial.println(DataResult_);
  //  deserializeJson(JsonData, DataResult_);
  //  String Obj_ = JsonData[Obj];
  //  //  Serial.println(Obj_);
  //  deserializeJson(JsonData, Obj_);
  //  String item_ = JsonData[item].as<String>();
  //  //  Serial.println(item_);
  String value_ = DataResult_;
  //  Serial.println("Value: " + (String)value_);
  return value_;
}

void sync_Device ()
{
  String value_ = "null";
  String res_;
  if (WiFi.status() == WL_CONNECTED && !isNoneWifi(NON_WIFI)) {
    res_ = post_HTTP(url + "/readData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity, "New Data Readed");
    value_ = get_Value(res_);
    Serial.println("Value: " + value_);

    if (value_ == "1") {
      RelayStatus = true;
      digitalWrite(RELAY, RelayStatus);
      digitalWrite(RELAY_LED, RelayStatus);
    }
    if (value_ == "0") {
      RelayStatus = false;
      digitalWrite(RELAY, RelayStatus);
      digitalWrite(RELAY_LED, RelayStatus);
    }
    if (value_ == "null") {
      RelayStatus = RelayStatus;
      digitalWrite(RELAY, RelayStatus);
      digitalWrite(RELAY_LED, RelayStatus);
    }
  }
}


boolean save_EEPROM (byte address, byte value ) {
  boolean isSave = false;
  EEPROM.write(address, value);
  EEPROM.commit();
  Serial.println("**WRITE EEPROM**");
  Serial.println("Address: " + (String) address + " - Value: " + (String) EEPROM.read(address) );
  Serial.println("*******");
  isSave = true;
}

byte read_EEPROM(byte address) {
  byte value;
  value = EEPROM.read(address);
  Serial.println("**READ EEPROM**");
  Serial.println("Address: " + (String) address + " - Value: " + (String) value );
  Serial.println("*******");
  return value;
}


boolean changeMode(byte sw) {
  unsigned long time_ = millis();
  unsigned long changeTime = 10;
  boolean isChange = false;
  while (true) {
    while (digitalRead(sw) == 1) {
      if ((millis() - time_) >= changeTime) {
        isChange = true;
        //        digitalWrite(MODE_LED, LOW);
        //        Serial.println("Normal Mode");
        goto exit_;
      }
    }
    while (digitalRead(sw) == 0) {
      if ((millis() - time_) >= changeTime) {
        isChange = false;
        //        digitalWrite(MODE_LED, HIGH);
        //        Serial.println("WifiSmartConfig Mode");
        goto exit_;
      }
    }
  }
exit_: {
    return isChange;

  }

}

boolean isSwitch(byte sw) {
  unsigned long time_ = millis();
  unsigned long time_out = 10;
  boolean is_Switch = false;
  while (digitalRead(sw) != pre_Status) {
    if ((millis() - time_) >= time_out) {
      is_Switch = true;
      pre_Status = digitalRead(sw);
      break;
      Serial.println("Switch was changed...");
    }
  }
  return is_Switch;
}

boolean isNoneWifi(byte sw) {
  unsigned long time_ = millis();
  unsigned long time_out = 10;
  boolean is_NoneWifi = false;
  while (digitalRead(sw) == 0) {
    if ((millis() - time_) >= time_out) {
      is_NoneWifi = true;
      break;
      //      Serial.println("NoneWifi Mode");
    }
  }
  return is_NoneWifi;
}

boolean initStatusSwitch(byte sw) {
  unsigned long time_ = millis();
  unsigned long time_out = 10;
  boolean Status_ = false;
  while ((millis() - time_) <= time_out) {
    Status_ = digitalRead(sw);
    Serial.println("initStatusSwitch is: " + (String)Status_);
  }
  return Status_;
}

boolean WifiSmartConfig() {
  boolean isDone = false;
  unsigned long time_ = millis(), time_blink = 200;
  unsigned long time_ConfigDone, time_ConfigStart, time_ConfigAgain_Wifi = 10000, time_ConfigAgain_Timeout = 30000;
  boolean signal_ = LOW;
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  time_ConfigStart =  millis();
  while (!WiFi.smartConfigDone()) {
    //    Serial.println("!WiFi.smartConfigDone()" + String(!WiFi.smartConfigDone()));
    if ((millis() - time_) >= time_blink) {
      signal_ = !signal_;
      digitalWrite(MODE_LED, signal_);
      time_ = millis();
    }
    if ((millis() - time_ConfigStart) >= time_ConfigAgain_Timeout) {
      Serial.println("SmartConfig Timeout.");
      save_EEPROM(RelayAddress, RelayStatus);
      ESP.restart();
    }
    if (changeMode(MODE_SW) || isNoneWifi(NON_WIFI)) {
      goto exit_;
    }
  }
  time_ConfigDone = millis();
  Serial.println("SmartConfig done.");
  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    if ((millis() - time_ ) >= time_blink) {
      signal_ = !signal_;
      digitalWrite(MODE_LED, signal_);
      time_ = millis();
    }
    if ((millis() - time_ConfigDone) >= time_ConfigAgain_Wifi) {
      Serial.println("Wifi Timeout.");
      save_EEPROM(RelayAddress, RelayStatus);
      ESP.restart();
    }
    if (changeMode(MODE_SW) || isNoneWifi(NON_WIFI)) {
      goto exit_;
    }
  }
  isDone = true;
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  while (!changeMode(MODE_SW) && !isNoneWifi(NON_WIFI)) {
    check_Wifi();
  }
exit_: {
    WiFi.begin();
    return isDone;
  }
}


boolean check_Wifi() {
  boolean isWifiConnect = false;
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(MODE_LED, LOW);
    //    Serial.println("LED MODE: " + (String)  digitalRead(MODE_LED));
    isWifiConnect = false;
  }
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(MODE_LED, HIGH);
    //    Serial.println("LED MODE: " + (String)  digitalRead(MODE_LED));
    isWifiConnect = true;
  }
  //  Serial.println("Status Wifi " + (String) isWifiConnect);
  //  Serial.println(WiFi.RSSI());
  return isWifiConnect;
}
unsigned long previousMillis_ReconnectWifi = 0;
unsigned long interval_ReconnectWifi = 1000;
boolean reconnect_Wifi() {
  // if WiFi is down, try reconnecting
  if ((!check_Wifi()) && (millis() - previousMillis_ReconnectWifi >= interval_ReconnectWifi)) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis_ReconnectWifi = millis();
  }


}

void TaskControl(void *pvParameters);
void TaskControl(void *pvParameters) // This is a task.
{
  delay(100);
  unsigned long previousMillis_Alive = 0;
  unsigned long interval_Alive = 10000;
  for (;;) {
    while (isNoneWifi(NON_WIFI)) {
      if (isSwitch(SW) && isNoneWifi(NON_WIFI)) {
        RelayStatus = !RelayStatus;
        digitalWrite(RELAY, RelayStatus);
        digitalWrite(RELAY_LED, RelayStatus);
        save_EEPROM(RelayAddress, RelayStatus);
        Serial.println("RelayStatus: " + (String)RelayStatus);
      }
    }
    //-------------------------
    if (WiFi.status() == WL_CONNECTED && !isNoneWifi(NON_WIFI)) {
      post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String (RelayStatus), "New Data Added");
      while (!isPost && !isNoneWifi(NON_WIFI))
      {
        post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String (RelayStatus), "New Data Added");
      }
    }
    //-------------------------------
    while (!isNoneWifi(NON_WIFI)) {
      if (isSwitch(SW) && !isNoneWifi(NON_WIFI)) {
        RelayStatus = !RelayStatus;
        digitalWrite(RELAY, RelayStatus);
        digitalWrite(RELAY_LED, RelayStatus);
        save_EEPROM(RelayAddress, RelayStatus);
        Serial.println("RelayStatus: " + (String)RelayStatus);
        if (WiFi.status() == WL_CONNECTED && !isNoneWifi(NON_WIFI)) {
          post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String (RelayStatus), "New Data Added");
          while (!isPost && !isNoneWifi(NON_WIFI))
          {
            post_HTTP(url + "/sendData_KeySecurity", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&Status=" + String (RelayStatus), "New Data Added");
          }
        }

      }

      sync_Device();

      if (WiFi.status() == WL_CONNECTED && !isNoneWifi(NON_WIFI)) {
        if (millis() - previousMillis_Alive >= interval_Alive) {
          post_HTTP(url + "/keepAlive", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&RSSI=" + (String)WiFi.RSSI() , "Keep Alive");
          while (!isPost && !isNoneWifi(NON_WIFI))
          {
            post_HTTP(url + "/keepAlive", "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity + "&RSSI=" + (String)WiFi.RSSI() , "Keep Alive");
          }
          previousMillis_Alive = millis();
        }
      }
    }
    //---------------------------------------


  }
}
void setup() {
  Serial.begin(115200);
  EEPROM.begin(FLASH_MEMORY_SIZE);
  Serial.println("Waitting setting... ");
  pinMode(MODE_LED, OUTPUT);
  digitalWrite(MODE_LED, LOW);
  pinMode(RELAY, OUTPUT);
  RelayStatus = read_EEPROM(RelayAddress);
  digitalWrite(RELAY, RelayStatus);
  pinMode(RELAY_LED, OUTPUT);
  digitalWrite(RELAY_LED, RelayStatus);
  pinMode(MODE_SW, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  pre_Status = initStatusSwitch(SW);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(NON_WIFI, INPUT_PULLUP);

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

void loop() {

  while (!changeMode(MODE_SW) && !isNoneWifi(NON_WIFI)) {
    WifiSmartConfig();
    //    Serial.println(digitalRead(MODE_SW));
    //    Serial.println("________________");
  }
  while (changeMode(MODE_SW) && !isNoneWifi(NON_WIFI)) {
    //    Serial.println(digitalRead(MODE_SW));
    //        Serial.println("################");

    reconnect_Wifi();
    check_Wifi();
    Reset ();
  }
  digitalWrite(MODE_LED, LOW);

}
