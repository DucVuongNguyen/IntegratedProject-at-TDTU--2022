/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>
const char* ssid = "509_2.4 GHz";
const char* password = "ducvuong";
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  bool ping;
} struct_message;

// Create a struct_message called myData
struct_message ReceiverPing;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&ReceiverPing, incomingData, sizeof(ReceiverPing));
  Serial.println("ReceiverPing.ping: " + (String)ReceiverPing.ping);
}
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  // Init ESP-NOW

  //
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  WiFi.setSleep(WIFI_PS_NONE);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);



}

void loop() {

}
