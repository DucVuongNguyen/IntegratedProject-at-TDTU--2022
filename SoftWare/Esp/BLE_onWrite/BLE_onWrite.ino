#include "ThingSpeak.h" // khai báo thư viện sử dụng cho ThingSpeak
unsigned long myChannelID  = 1686633;         // Channel ID
const char * myReadAPIKey  = "ZI9TTAWMYC7T5V2U";   // Read API Key
const char * myWriteAPIKey = "S1W6S04XVNH0DP86";  // Write API Key
// Các Field cần dùng
const int FieldNumber1        = 1;
const int FieldNumber2        = 2;
unsigned long time_send = 0; // biến lưu thời điểm sửa sự kiện xảy ra
// chương trình gửi dữ liệu lên các Field 1, 2
void sendThingSpeak()
{
  if ((millis() - time_send) > 20000  ) // nếu thời gian cách thời điểm xảy ra sự kiện là 20s
  {
    ThingSpeak.setField(FieldNumber1, 10); // cài đặt giá trị gửi lên Field
    ThingSpeak.setField(FieldNumber2, 11);
    int x = ThingSpeak.writeFields(myChannelID, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    time_send = millis();
  }
}
//------------- LIB BLE--------------------------------------------
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false, oldDeviceConnected = false;
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
//-------------LIB WIFI--------------------------------------------
#include <WiFi.h> //thư viện wifi
WiFiClient  client; // khởi tạo class wifi
String ssid, password, key = "ducvuong";
unsigned long previousMillis = 0, previousMillis_notifi = 0, interval = 10000;


//-------------WIFI--------------------------------------------
bool ConnectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str()); // cho phép kết nối wifi
  unsigned long startMillis = millis();
  unsigned long timeout = 10000;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...");
    delay(100);
    if (millis() - startMillis >= timeout) {
      Serial.println("\r\nWiFi connected Fail");
      goto exit_;
    }
  }
  Serial.println(WiFi.localIP());
  Serial.println("\r\nWiFi connected");
exit_:
  {
  }
}

void ReconnectWifi() {
  //if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (millis() - previousMillis >= interval)) {
    Serial.println(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = millis();
  }
}
void NotifiWifi_BLE() {
  // disconnecting
  if ((millis() -  previousMillis_notifi >= 500)) {
    if (WiFi.status() != WL_CONNECTED) {
      if (deviceConnected) {
        pTxCharacteristic->setValue("Wifi Connect Fail");
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
      }

    }
    else {
      if (deviceConnected) {
        pTxCharacteristic->setValue("Wifi Connect Success");
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
      }

    }
    if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
    previousMillis_notifi = millis();

  }



}

//-------------BLE--------------------------------------------
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        String data_ = value.c_str();
        Serial.println("Received Value: " + data_);
        ssid = data_.substring(data_.indexOf("SSID") + 4, data_.indexOf("PASS"));
        password = data_.substring(data_.indexOf("PASS") + 4, data_.indexOf("KEY"));
        String key_ = data_.substring(data_.indexOf("KEY") + 3);
        Serial.println("ssid:" + ssid);
        Serial.println("password:" + password);
        Serial.println("key:" + key_);
        if (key_ == key) {
          ConnectWifi();
        }
      }
    }
};
void SetupBLE()
{
  // Create the BLE Device
  BLEDevice::init("V-Home");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
//---------MAIN------------------------------------------
void setup() {
  Serial.begin(115200);
  SetupBLE();
  ConnectWifi();
  ThingSpeak.begin(client); //  cho phép kết nối Thingspeak
}

void loop() {
  NotifiWifi_BLE();
  ReconnectWifi();
  sendThingSpeak();

}
