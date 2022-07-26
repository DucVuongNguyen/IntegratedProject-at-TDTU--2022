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
#include <esp_wifi.h>
const char* ssid = "509_2.4 GHz";

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xB8, 0xF0, 0x09, 0xB3, 0x52, 0x80};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  bool ping;
} struct_message;

// Create a struct_message called myData
struct_message SenderPing;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(23, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(ssid);

  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  //   Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Set values to send
  SenderPing.ping = true;

  // Send message via ESP-NOW

  if (digitalRead(23) == 0) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &SenderPing, sizeof(SenderPing));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    delay(200);
  }



}
