// Doomsday Ham Radio Comms Computer - Adafruit RP2040 Feather DVI Node v0.06
// 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms
// Unified firmware with mode byte in flash (prior decision referenced)
// VERSION: 0.06 - 2026-05-07 - Heavy persistent serial debugging + stable 400x240p30 DVI init

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

// DVI display - 400x240 internal → crisp 800x480 HDMI (stable p30 timing)
DVIGFX16 display(DVI_RES_400x240p30, adafruit_feather_dvi_cfg);

void setup() {
    // EARLY DEBUG - start serial as fast as possible
    Serial.begin(115200);
    delay(300);
    Serial.println("\n=== DOOMSDAY v0.06 BOOT START ===");
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("[1] LED pin configured");
    
    uint8_t mode = readModeByte();
    Serial.print("[2] Mode byte read from flash: 0x");
    Serial.println(mode, HEX);
    
    Serial.println("[3] Attempting DVI init (400x240p30 → 800x480 HDMI)...");
    
    bool dvi_ok = display.begin();
    Serial.print("[4] display.begin() returned: ");
    Serial.println(dvi_ok ? "SUCCESS" : "FAILED");
    
    if (!dvi_ok) {
        Serial.println("[ERROR] DVI init failed - entering error blink loop");
        while (true) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(200);
        }
    }
    
    Serial.println("[5] DVI init OK - drawing test pattern");
    display.fillScreen(0x0000);
    display.setTextColor(0xFFFF);
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.print("DOOMSDAY v0.06");
    display.setCursor(20, 60);
    display.print("800x480 HDMI LIVE");
    display.setCursor(20, 100);
    display.print("5x UNO R4 Minima Cluster");
    
    Serial.println("[6] Test pattern drawn - setup complete");
    Serial.println("=== DOOMSDAY v0.06 BOOT COMPLETE ===\n");
}

void loop() {
    static uint32_t lastBeat = 0;
    if (millis() - lastBeat > 1000) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        lastBeat = millis();
    }
}