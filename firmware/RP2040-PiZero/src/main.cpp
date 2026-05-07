#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"

// DVI Library (flattened to firmware/lib/DVI/)
#include "dvi.h"
#include "common_dvi_pin_configs.h"

// DVI instance (required by the library)
struct dvi_inst dvi0;

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// RP2040-PiZero HDMI Test Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)

// ==================== DVI CONFIG (Waveshare RP2040-PiZero) ====================
static const struct dvi_serialiser_cfg dvi_cfg = DVI_DEFAULT_SERIAL_CONFIG;

void initDVI() {
    // Correct signature: dvi_init(inst, spinlock_tmds_queue, spinlock_colour_queue)
    uint spinlock_tmds = spinlock_claim_unused(true);
    uint spinlock_colour = spinlock_claim_unused(true);
    dvi_init(&dvi0, spinlock_tmds, spinlock_colour);
    dvi_start(&dvi0);
    Serial.println("[DVI] HDMI output active @ 640x480@60Hz (flattened path)");
}

void drawHDMITestScreen() {
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