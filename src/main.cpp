#include <Arduino.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2   // Most ESP32 boards use GPIO2 for onboard LED
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("ESP32 Blink Test: setup done");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED OFF");
  delay(500);
}
