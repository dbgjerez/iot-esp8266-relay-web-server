#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

#define RELAY_PIN D2
#define DELAY 1000
#define WIFI_SSID ""
#define WIFI_PASS ""

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
   // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String mode = request -> getParam("mode")->value();
    if ( mode == "on" ){
      digitalWrite(RELAY_PIN, LOW);
      request->send(200, "text/plain", "ON");
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      request->send(200, "text/plain", "OFF");
    }
        
    });
  
  server.begin();

}

void loop() {}