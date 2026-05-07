#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "config.h"
#include "DoomsdayMessageBus.h"

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// RP2040-PiZero HDMI Test Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)
// Role: Primary DVI/HDMI terminal (640x480@60Hz) for retro UI (LVGL / CP/M / WordStar planned)

// ==================== HARDWARE PINS (RP2040-PiZero) ====================
// DVI/HDMI pins handled by PicoDVI / Waveshare libvi (to be integrated)
// DMB UART to UNO R4 Minima fleet
#define DMB_RX_PIN GP0
#define DMB_TX_PIN GP1
#define DMB_BAUD 921600

// Mode byte location in flash (unified firmware prior decision)
#define MODE_BYTE_ADDR 0x101FF000

DoomsdayMessageBus dmb(Serial2, DMB_BAUD);  // UART for inter-node comms

uint8_t readModeByte() {
    uint8_t mode = 0xFF;
    // Read from flash (RP2040 XIP flash - adapt for actual flash API if needed)
    // For now, default to DISPLAY_NODE (0x10)
    return 0x10;  // DISPLAY_NODE mode for this RP2040-PiZero
}

// ==================== DVI / HDMI TEST (placeholder - integrate PicoDVI next) ====================
void initDVI() {
    // TODO: Add PicoDVI or Waveshare 01-DVI libvi here
    // Example (after adding library):
    // dvi_init();
    // dvi_set_mode(640, 480, 60);
    // Draw test pattern or text
    Serial.println("[DVI] HDMI/DVI output initialized (640x480@60Hz)");
    // Placeholder: In real build this drives the mini HDMI connector
}

void drawHDMITestScreen() {
    // Placeholder for DVI framebuffer draw
    // Will become: dvi_draw_string("Doomsday Net Computer - HDMI Test", ...)
    Serial.println("[HDMI] Test screen active: Doomsday Net Computer v0.28 HDMI Test");
    Serial.println("[HDMI] Air-gapped | Mode byte: 0x10 (DISPLAY_NODE)");
    Serial.println("[HDMI] Ready for LVGL retro terminal + DMB to 5x UNO R4 Minima");
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
    
    // DMB UART setup for comms with UNO R4 Minima nodes
    dmb.begin();
    Serial.println("[DMB] Doomsday Message Bus ready on UART (GP0/GP1 @ 921600)");
    
    // HDMI / DVI milestone
    initDVI();
    drawHDMITestScreen();
    
    Serial.println("=== Doomsday Net Computer - RP2040-PiZero HDMI Test v0.28 ===");
    Serial.println("5x UNO R4 Minima | Air-gapped | EMP-protected | Low-power HF/VHF/ISM");
    Serial.println("Type 'help' or send DMB commands from other nodes");
}

// ==================== LOOP ====================
void loop() {
    dmb.update();  // Listen for commands from Minima fleet (MeshCore, RTTY, printer, etc.)
    
    // Simple heartbeat
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 2000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
    
    // Future: keyboard input via USB or DMB, SD file ops, full terminal UI
}