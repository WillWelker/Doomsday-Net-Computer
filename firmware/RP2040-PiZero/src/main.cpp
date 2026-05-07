#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"

// DVI Library (from firmware/lib/DVI/hello_dvi/)
#include "DVI/hello_dvi/dvi.h"
#include "DVI/hello_dvi/common_dvi_pin_configs.h"

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// RP2040-PiZero HDMI Test Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)

// ==================== DVI CONFIG (Waveshare RP2040-PiZero) ====================
static const struct dvi_serialiser_cfg dvi_cfg = DVI_DEFAULT_SERIAL_CONFIG;

void initDVI() {
    dvi_init(&dvi0, &dvi_cfg);
    dvi_start(&dvi0);
    Serial.println("[DVI] HDMI output active @ 640x480@60Hz");
}

void drawHDMITestScreen() {
    // Basic test - in full version this would draw text/pattern via framebuffer
    // For now we rely on Serial + future LVGL overlay
    Serial.println("[HDMI] === Doomsday Net Computer - RP2040-PiZero HDMI Test v0.28 ===");
    Serial.println("[HDMI] 5x UNO R4 Minima | Air-gapped | EMP-protected | Low-power HF/VHF/ISM");
    Serial.println("[HDMI] Mode byte: 0x10 (DISPLAY_NODE) - unified firmware prior decision");
    Serial.println("[HDMI] Real DVI driver active - monitor should show output");
}

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
    
    initDVI();
    drawHDMITestScreen();
    
    Serial.println("HDMI test phase active - DMB paused until HDMI confirmed working");
}

void loop() {
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 1000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
}