#include <Arduino.h>

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// Adafruit RP2040 Feather DVI Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)

// ==================== MODE BYTE ====================
uint8_t readModeByte() {
    return 0x10;  // DISPLAY_NODE
}

void setup() {
    Serial.begin(115200);
    delay(500);
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    uint8_t mode = readModeByte();
    Serial.print("[BOOT] Mode byte read: 0x");
    Serial.println(mode, HEX);
    
    Serial.println("Adafruit RP2040 Feather DVI - Placeholder v0.01");
}

void loop() {
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 1000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
}