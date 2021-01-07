#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <ESPAsyncWebServer.h>

#include "AsyncJson.h"
#include "ArduinoJson.h"

AsyncWebServer server(80);

#define RELAY_PIN D2
#define DELAY 1000
#define WIFI_SSID ""
#define WIFI_PASS ""

const char indexHtml[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center;}
      h2 {font-size: 3.0rem;}
      p {font-size: 3.0rem;}
      body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
      .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
      .switch input {display: none}
      .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
      .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
      input:checked+.slider {background-color: #2196F3}
      input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    </style>
  </head>
  <body onload="isChecked()">
    <h2>Home</h2>
    <h4 id="relay_title">Relay</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="2"><span class="slider"></span></label>
  <script>
  function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    xhr.open("PUT","/relay/1");
    xhr.setRequestHeader("Content-Type", "application/json");
    if(element.checked){ 
      xhr.send(JSON.stringify({"state":"on"}));
    } else { 
      xhr.send(JSON.stringify({"state":"off"}));
    }
  }
  var getJSON = function(url, callback) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'json';
    xhr.onload = function() {
      var status = xhr.status;
      if (status === 200) {
        callback(null, xhr.response);
      } else {
        callback(status, xhr.response);
      }
    };
    xhr.send();
  };
  function isChecked() {
    getJSON('relay/1',
    function(err, data) {
      if (err !== null) {
        alert('Error consultando estado: ' + err);
      } else {
        document.getElementById("relay_title").innerHTML=data.title;
        if (data.state === 'on'){
          document.getElementById("2").checked=true;
        }
      }
    });
  }
  </script>
  </body>
  </html>
)rawliteral";

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
   // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al wifi...");
  }

  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/relay/1", HTTP_GET, [](AsyncWebServerRequest *request){
    int relayState = digitalRead(RELAY_PIN);
    if(relayState == LOW) {
      request->send(200, "application/json", "{\"state\":\"on\",\"title\":\"Christmas lights\"}");
    }else {
      request->send(200, "application/json", "{\"state\":\"off\",\"title\":\"Christmas lights\"}");
    }      
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", indexHtml);
  });

  AsyncCallbackJsonWebHandler* modifyRelay = new AsyncCallbackJsonWebHandler("/relay/1", [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject& jsonObj = json.as<JsonObject>();
    Serial.println("Recibida llamada")  ;
    String state = jsonObj["state"];
    Serial.println(state);
    if ( state == "on" ){
      digitalWrite(RELAY_PIN, LOW);
    } else if ( state == "off" ) {
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      request->send(400);
    } 
    request->send(200);
  });
  modifyRelay->setMethod(HTTP_PUT);
  server.addHandler(modifyRelay);

  server.begin();
}

void loop() {}