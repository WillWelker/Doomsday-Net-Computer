#include <Arduino.h>
#include <PicoDVI.h>

// ==================== DOOMSDAY PROJECT IDENTITY ====================
// Adafruit RP2040 Feather DVI Node — DISPLAY ADAPTER v0.11
// 5× UNO R4 Minima modular UART-linked setup
// Air-gapped, EMP-protected, low-power HF/VHF/ISM comms

uint8_t readModeByte() { return 0x10; }

DVItext1 display(DVI_RES_640x240p60, adafruit_feather_dvi_cfg);

void setup() {
    Serial.begin(115200);
    delay(200);

    uint8_t mode = readModeByte();
    Serial1.begin(921600);   // DMB speed

    if (!display.begin()) {
        pinMode(LED_BUILTIN, OUTPUT);
        for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
    }

    display.println("========================================");
    display.println("  DOOMSDAY NET COMPUTER - DISPLAY NODE");
    display.println("========================================");
    display.print("Firmware: v0.11   Mode: 0x");
    display.print(mode, HEX);
    display.println(" (DISPLAY_NODE)");
    display.println("Status: Listening on UART (921600 baud)...");
    display.println("========================================");
    display.println();
}

void loop() {
    // Simple UART listener (we'll wrap with proper DMB class later)
    if (Serial1.available()) {
        String msg = Serial1.readStringUntil('\n');
        if (msg.length() > 0) {
            display.println(msg);
        }
    }
}