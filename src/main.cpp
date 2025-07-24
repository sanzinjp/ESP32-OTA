#include <Arduino.h>

#define LED_PIN 48 // Onboard LED pin, Change to your LED pin if needed

void setup() { pinMode(LED_PIN, OUTPUT); }

void loop() {
  digitalWrite(LED_PIN, HIGH); // Turn LED on
  delay(1000);                 // Wait 500ms
  digitalWrite(LED_PIN, LOW);  // Turn LED off
  delay(1000);                 // Wait 500ms
}
