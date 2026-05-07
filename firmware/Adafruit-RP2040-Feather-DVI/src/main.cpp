// Doomsday Ham Radio Comms Computer - Adafruit RP2040 Feather DVI Node v0.05
// 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)
// VERSION: 0.05 - 2026-05-07 - FIXED DVIGFX16 constructor for current Adafruit PicoDVI fork

#include <Arduino.h>
#include <PicoDVI.h>

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// Adafruit RP2040 Feather DVI Node
// Part of 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)

// ==================== MODE BYTE (Flash at 0x101FF000) ====================
uint8_t readModeByte() {
    volatile uint8_t* modeAddr = (volatile uint8_t*)MODE_BYTE_ADDR;
    return *modeAddr;
}

// DVI display - 400x240 internal → crisp 800x480 HDMI (corrected constructor)
DVIGFX16 display(DVI_RES_400x240p60, adafruit_feather_dvi_cfg);

void setup() {
    Serial.begin(115200);
    delay(500);
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    uint8_t mode = readModeByte();
    Serial.print("[BOOT] Mode byte read from flash: 0x");
    Serial.println(mode, HEX);
    Serial.println("Adafruit RP2040 Feather DVI - Doomsday Node v0.05 ONLINE");
    Serial.println("Claiming 800x480 HDMI output for 5x UNO R4 Minima cluster");

    // Initialize DVI - claims display from factory demo
    if (!display.begin()) {
        Serial.println("DVI init FAILED");
        while (1) delay(100);
    }
    display.fillScreen(0x0000);           // black
    display.setTextColor(0xFFFF);         // white
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.print("DOOMSDAY v0.05");
    display.setCursor(20, 60);
    display.print("800x480 HDMI ONLINE");
    display.setCursor(20, 100);
    display.print("5x UNO R4 Minima Cluster");
}

void loop() {
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 1000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
}