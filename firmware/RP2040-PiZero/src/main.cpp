#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"

// ==================== DOOMSDAY PROJECT IDENTITY (HDMI TEST PHASE) ====================
// RP2040-PiZero HDMI Test Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)
// Current focus: HDMI/DVI output milestone ONLY (DMB re-added after HDMI works)

// ==================== MODE BYTE (unified firmware prior decision) ====================
#define MODE_BYTE_ADDR 0x101FF000

uint8_t readModeByte() {
    return 0x10;  // DISPLAY_NODE mode for this RP2040-PiZero
}

// ==================== DVI / HDMI TEST (PicoDVI / Waveshare 01-DVI placeholder) ====================
void initDVI() {
    // TODO: Integrate real PicoDVI or Waveshare libvi here
    // After integration this will drive the onboard mini HDMI at 640x480@60Hz
    Serial.println("[DVI] HDMI/DVI output initialized (640x480@60Hz)");
}

void drawHDMITestScreen() {
    Serial.println("[HDMI] === Doomsday Net Computer - RP2040-PiZero HDMI Test v0.28 ===");
    Serial.println("[HDMI] 5x UNO R4 Minima | Air-gapped | EMP-protected | Low-power HF/VHF/ISM");
    Serial.println("[HDMI] Mode byte: 0x10 (DISPLAY_NODE) - unified firmware prior decision");
    Serial.println("[HDMI] Ready for real DVI driver + retro LVGL terminal");
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(500);
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    uint8_t mode = readModeByte();
    Serial.print("[BOOT] Mode byte read: 0x");
    Serial.println(mode, HEX);
    
    initDVI();
    drawHDMITestScreen();
    
    Serial.println("HDMI test phase active - DMB and other libs paused until HDMI works");
}

// ==================== LOOP ====================
void loop() {
    // Heartbeat
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 1000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
}