#include <WiFi.h>
#include <HTTPClient.h>
  
const char* ssid = "509_2.4 GHz";
const char* password =  "ducvuong";
  
void setup() {
  
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin
  
  WiFi.begin(ssid, password); 
  
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
}
  
void loop() {
  delay(10);  //Send a request every 10 seconds
  
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  
   HTTPClient http;   
   
  
   http.begin("http://192.168.0.101:8000/api/readData_KeySecurity");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");             //Specify content-type header
  
   int httpResponseCode = http.POST("NameDevice=V001&KeySecurity=73029");   //Send the actual POST request
  
   if(httpResponseCode>0){
  
    String response = http.getString();                       //Get the response to the request
  
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
  
   }else{
  
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  
   }
  
   http.end();  //Free resources
  
 }else{
  
    Serial.println("Error in WiFi connection");   
  
 }
  
  
  
}
