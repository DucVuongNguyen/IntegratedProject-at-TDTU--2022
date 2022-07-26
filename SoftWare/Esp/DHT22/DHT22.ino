#include "DHT.h"
#define DHTPIN 15    // modify to the pin we connected

#define DHTTYPE DHT21   // AM2301 

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read from DHT");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
    delay(2000);
  }
}
