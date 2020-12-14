#include <Arduino.h>

#define RELAY_PIN D2
#define DELAY 1000

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  delay(DELAY);

  digitalWrite(RELAY_PIN, HIGH);

  delay(DELAY);
 
  digitalWrite(RELAY_PIN, LOW);
}