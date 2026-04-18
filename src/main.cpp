#include <Arduino.h>

void setup() {
  // Initialize the built-in LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Teensy 4.1 Blink Test Started");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on
  Serial.println("LED ON");
  delay(1000);                       // Wait 1 second

  digitalWrite(LED_BUILTIN, LOW);    // Turn the LED off
  Serial.println("LED OFF");
  delay(1000);                       // Wait 1 second
}