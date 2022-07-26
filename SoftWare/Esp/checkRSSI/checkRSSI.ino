#include "WiFi.h"

const char* ssid = "509_2.4 GHz";
const char* password =  "ducvuong";

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

}

void loop() {
  Serial.println(WiFi.RSSI());
}
