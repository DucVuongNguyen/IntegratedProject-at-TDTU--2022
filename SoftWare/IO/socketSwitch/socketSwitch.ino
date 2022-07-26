#include <Arduino.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
SocketIOclient socketIO;
#include <EEPROM.h>
#define FLASH_MEMORY_SIZE 10


//khai báo chân
// không được dùng 34 35 vì 2 chân này không có điện trở kéo lên/xuống
#define MODE_WIFI 23
#define NON_WIFI 22
#define MODE_LED 33 // led báo Status Wifi
#define RELAY 27
#define SW 25
#define RESET 32
#define RELAY_LED 26 //led báo trạng thái Relay/BTN

byte RelayAddress = 0;
boolean pre_Status = false; // biến lưu trạng thái nút nhấn
boolean isSocketConnect = false;

String NameDevice = "V000", KeySecurity = "73029", Key = "null";
String url = "https://iot-server-demo.herokuapp.com";
int port = 80;

String HTTP_getKeyDevice(String url_) {
  String response_ = "null";
  HTTPClient http;
  http.begin(url_);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String Data_ = "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity;
  int httpResponseCode = http.POST(Data_);
  if (httpResponseCode == 200) {
    response_ = http.getString();
    //Serial.println(httpResponseCode);
    //Print return code
    Serial.println(response_);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return response_;
}

String HTTP_ressetKeyDevice(String url_) {
  String response_ = "null";
  HTTPClient http;
  http.begin(url_);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String Data_ = "NameDevice=" + NameDevice + "&KeySecurity=" + KeySecurity;
  int httpResponseCode = http.POST(Data_);
  if (httpResponseCode == 200) {
    response_ = http.getString();
    //Serial.println(httpResponseCode);
    //Print return code
    Serial.println(response_);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return response_;
}

String getKey_() {

  boolean isError = true;
  String Key;
  String response_ = "null";
begin_: {
    while (response_ == "null" && !isNoneWifi()) {
      response_ = HTTP_getKeyDevice(url + "/api/getKey");
    }
    DynamicJsonDocument obj(256);
    deserializeJson(obj, response_);
    isError = obj["isError"];
    if (isError) {
      //      Serial.println("isError: " + (String)isError);
      goto begin_;
    }
    else {
      Key = obj["Key"].as<String>();
      //      Serial.println("Key: " + (String)Key);
    }
  }
  return Key;
}

boolean isNoneWifi() {
  unsigned long time_ = millis();
  unsigned long time_out = 20;
  boolean is_NoneWifi = false;
  while (digitalRead(NON_WIFI) == 0) {
    if ((millis() - time_) >= time_out) {
      is_NoneWifi = true;
      digitalWrite(MODE_LED, LOW);
      //      Serial.println("NoneWifi Mode");
      WiFi.disconnect();
      break;
    }
  }
  return is_NoneWifi;
  //is_NoneWifi : 1 is_NoneWifi turn on
  //is_NoneWifi : 0 is_NoneWifi turn off
}

boolean isModeWifi() {
  unsigned long time_ = millis();
  unsigned long time_out = 10;
  boolean isModeWifi = false;
  while (true) {
    while (digitalRead(MODE_WIFI) == 1) {
      if ((millis() - time_) >= time_out) {
        isModeWifi = true;
        //        Serial.println("Normal Mode");
        goto exit_;
      }
    }
    while (digitalRead(MODE_WIFI) == 0) {
      if ((millis() - time_) >= time_out) {
        isModeWifi = false;
        //        Serial.println("WifiSmartConfig Mode");
        goto exit_;
      }
    }
  }
exit_: {
    return isModeWifi;
    //isModeWifi : 1 Wifi Mode
    //isModeWifi : 0 WifiSmartConfig Mode

  }

}

boolean initStatusSwitch() {
  unsigned long time_ = millis();
  unsigned long time_out = 10;
  boolean Status_ = false;
  while ((millis() - time_) <= time_out) {
    Status_ = digitalRead(SW);
    Serial.println("initStatusSwitch is: " + (String)Status_);
  }
  return Status_;
}

boolean handleSwitch() {
  unsigned long time_ = millis();
  unsigned long time_out = 20;
  boolean handleSwitch = false;
  boolean Status;
  while (digitalRead(SW) != pre_Status) {
    if ((millis() - time_) >= time_out) {
      handleSwitch = true;
      pre_Status = digitalRead(SW);
      Status = !read_EEPROM(RelayAddress);
      save_EEPROM(RelayAddress, Status);
      digitalWrite(RELAY, Status);
      DeviceToApp();
      Serial.println("Switch was changed...");
      break;
    }
  }
  return handleSwitch;
}

boolean handleSwitch_NoneWifi() {
  unsigned long time_ = millis();
  unsigned long time_out = 50;
  boolean handleSwitch = false;
  boolean Status;
  while (digitalRead(SW) != pre_Status) {
    //    Serial.println("pre_Status: " + (String)pre_Status);
    //    Serial.println("digitalRead(SW): " + (String)digitalRead(SW));
    if ((millis() - time_) >= time_out) {
      Serial.println("Switch was ...");
      handleSwitch = true;
      pre_Status = digitalRead(SW);
      Status = !read_EEPROM(RelayAddress);
      save_EEPROM(RelayAddress, Status);
      digitalWrite(RELAY, Status);
      Serial.println("Switch was changed...");
      break;
    }
  }
  return handleSwitch;
}

boolean handleReset() {
  unsigned long time_ = millis();
  unsigned long time_out = 500;
  boolean handleReset = false;
  String response_ = "null";
  if (isModeWifi() && !isNoneWifi()) {
    while (digitalRead(RESET) == 0) {
      if ((millis() - time_) >= time_out) {
        Serial.println("RESSET ...");
        while (response_ == "null" && !isNoneWifi() && check_Wifi())
        {
          response_ = HTTP_ressetKeyDevice(url + "/api/resetKey");
          for (int i = 0; i < 5; i++) {
            digitalWrite(MODE_LED, HIGH);
            delay(500);
            digitalWrite(MODE_LED, LOW);
            delay(500);
          }
          ESP.restart();
        }

      }
    }

  }
  return handleReset;
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
      ESP.restart();
    }
    if (isModeWifi() || isNoneWifi()) {
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
      ESP.restart();
    }
    if (isModeWifi() || isNoneWifi()) {
      goto exit_;
    }
  }
  isDone = true;
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  while (!isModeWifi() && !isNoneWifi()) {
    Serial.println("Wait Wifi Mode");
    digitalWrite(MODE_LED, HIGH);
  }
exit_: {
    ESP.restart();
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
    if (isSocketConnect) {
      digitalWrite(MODE_LED, HIGH);
    }
    else {
      digitalWrite(MODE_LED, LOW);
    }
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

void joinroom () {
  // creat JSON message for Socket.IO (event)
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  // add evnet name
  array.add("Switch");

  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["room"] = NameDevice + Key;
  param1["function"] = "join_room";
  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);
  // Send event
  socketIO.sendEVENT(output);
  //   Print JSON for debugging
  //  USE_SERIAL.println(output);
}

void SyncStatus(String payloadEvent) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payloadEvent);
  int Status = doc["DataResult"];
  save_EEPROM(RelayAddress, Status);
  digitalWrite(RELAY, Status);

}
void getKey_socket() {
  Serial.println("getKey_socket");
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  // add evnet name
  array.add("Switch");

  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["room"] = NameDevice + Key;
  param1["NameDevice"] = NameDevice;
  param1["KeySecurity"] = KeySecurity;
  param1["function"] = "getKey_socket";
  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);
  // Send event
  socketIO.sendEVENT(output);
  //   Print JSON for debugging
  Serial.println(output);
}
String Key_socket(String payloadEvent) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payloadEvent);
  String Key_ = doc["Key"];
  return Key_;
}


void DeviceToApp() {
  Serial.println("DeviceToApp");
  boolean RelayEEPROM = read_EEPROM(RelayAddress);
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  // add evnet name
  array.add("Switch");

  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["room"] = NameDevice + Key;
  param1["NameDevice"] = NameDevice;
  param1["Key"] = Key;
  param1["Status"] = (String)RelayEEPROM;
  param1["function"] = "DeviceToApp";
  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);
  // Send event
  socketIO.sendEVENT(output);
  //   Print JSON for debugging
  Serial.println(output);
}

void InitStatusValue() {
  Serial.println("InitStatusValue");
  boolean RelayEEPROM = read_EEPROM(RelayAddress);
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  // add evnet name
  array.add("Switch");

  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["room"] = NameDevice + Key;
  param1["Status"] = (String)RelayEEPROM;
  param1["function"] = "InitStatusValue";
  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);
  // Send event
  socketIO.sendEVENT(output);
  //   Print JSON for debugging
  Serial.println(output);
}

void keepAlive() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.to<JsonArray>();
  // add evnet name
  array.add("Switch");
  // add payload (parameters) for the event
  JsonObject param1 = array.createNestedObject();
  param1["room"] = NameDevice + Key;
  param1["RSSI"] = (String)WiFi.RSSI();;
  param1["function"] = "keepAlive";
  // JSON to String (serializion)
  String output;
  serializeJson(doc, output);
  // Send event
  socketIO.sendEVENT(output);
  //   Print JSON for debugging
  Serial.println(output);
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  String eventName, payloadEvent;
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      isSocketConnect = false;
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);
      // join default namespace (no auto join in Socket.IO V3)
      socketIO.send(sIOtype_CONNECT, "/");
      joinroom ();
      InitStatusValue();
      isSocketConnect = true;
      break;
    case sIOtype_EVENT:
      {
        //        Serial.printf("[IOc] get event: %s\n", payload);
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        String eventName = doc[0];
        //        Serial.printf("[IOc] event name: %s\n", eventName.c_str());
        String payloadEvent = doc[1];
        if (eventName.equals("SyncStatus")) {
          Serial.println("SyncStatus");
          SyncStatus(payloadEvent);
        }
        if (eventName.equals("GetInitStatus")) {
          Serial.println("GetInitStatus");
          InitStatusValue();
        }
        if (eventName.equals("Key_socket")) {
          //          String Key_ = Key;
          Key = Key_socket(payloadEvent);
          Serial.println("Key:" + (String)Key);
          joinroom ();
          //          if (Key != Key_) {
          //
          //          }
        }

      }
      break;
    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}

void TaskControl(void *pvParameters);
void TaskControl(void *pvParameters) // This is a task.
{ unsigned long time_getkey = millis(), timeout_getKey = 1000, time_alive = millis(), timeout_alive = 2000;;
  for (;;) {
    if (isModeWifi() && !isNoneWifi()) {
      socketIO.loop();
      if (millis() - time_alive > timeout_alive) {
        keepAlive();
        time_alive = millis();
      }
      if ((millis() - time_getkey > timeout_getKey) && check_Wifi()) {
        getKey_socket();
        time_getkey = millis();
      }
    }

  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  EEPROM.begin(FLASH_MEMORY_SIZE);
  pinMode(MODE_WIFI, INPUT_PULLUP);
  pinMode(NON_WIFI, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(MODE_LED, OUTPUT);
  pinMode(RELAY_LED, OUTPUT);
  digitalWrite(MODE_LED, LOW);
  pinMode(RELAY, OUTPUT);
  boolean RelayEEPROM = read_EEPROM(RelayAddress);
  digitalWrite(RELAY, RelayEEPROM);
  digitalWrite(RELAY_LED, digitalRead(RELAY));
  pinMode(SW, INPUT_PULLUP);
  for (int i = 0; i < 3; i++) {
    digitalWrite(MODE_LED, HIGH);
    delay(500);
    digitalWrite(MODE_LED, LOW);
    delay(500);
  }
  delay(3000);
  if (isModeWifi() && !isNoneWifi()) {
    WiFi.begin();
    while (!check_Wifi()) {
      Serial.println("....");
      if (!isModeWifi() || isNoneWifi()) {
        ESP.restart();
      }
    }
    while (Key == "null" && !isNoneWifi()) {
      Key = getKey_();
      Serial.println("Key: " + (String)Key);
    }
    // server address, port and URL
    socketIO.begin("iot-server-demo.herokuapp.com", port, "/socket.io/?EIO=4");

    // event handler
    socketIO.onEvent(socketIOEvent);
    xTaskCreatePinnedToCore(
      TaskControl, "TaskControl" // A name just for humans
      ,
      10000 // This stack size can be checked & adjusted by reading the Stack Highwater
      ,
      NULL, 1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      NULL, 1);
  }
  pre_Status = initStatusSwitch();

}
void loop() {
  // put your main code here, to run repeatedly:
  while (!isModeWifi() && !isNoneWifi()) {
    //    Serial.println("WifiSmartConfig Mode");
    Key = "null";
    WifiSmartConfig();
  }

  while (isModeWifi() && !isNoneWifi()) {
    //    Serial.println("Wifi Mode");
    reconnect_Wifi();

    if (check_Wifi()) {
      handleSwitch();
      digitalWrite(RELAY_LED, digitalRead(RELAY));
      handleReset();

    }
    else {
      pre_Status = initStatusSwitch();
    }


  }

  while (isNoneWifi()) {
    handleSwitch_NoneWifi();
    digitalWrite(RELAY_LED, digitalRead(RELAY));
  }
  ESP.restart();

}
