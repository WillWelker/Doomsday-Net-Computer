// ============================================================
// RP2040-PiZero - Basic Test Program
// Doomsday Net Computer - Display + Keyboard Module
// ============================================================

#include <Arduino.h>

#define LED_PIN 25          // Onboard LED on Waveshare RP2040-PiZero
#define SERIAL_BAUD 115200

void setup() {
    Serial.begin(SERIAL_BAUD);
    
    // Wait for serial connection (up to 5 seconds)
    while (!Serial && millis() < 5000) {}
    
    pinMode(LED_PIN, OUTPUT);
    
    Serial.println("\n========================================");
    Serial.println("  RP2040-PiZero - Doomsday Net Computer");
    Serial.println("  Display + Keyboard Module");
    Serial.println("========================================");
    Serial.println("Board: Waveshare RP2040-PiZero");
    Serial.println("Status: Basic test program running");
    Serial.println("");
    Serial.println("LED should be blinking...");
    Serial.println("");
}

void loop() {
    // Blink the onboard LED
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
    
    // Print heartbeat message every 5 seconds
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 5000) {
        Serial.println("RP2040-PiZero: Running... (LED blinking)");
        lastHeartbeat = millis();
    }
}