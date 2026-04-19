#include <Arduino.h>
#include "PicoDVI.h"

// ==================== CONFIG ====================
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

// DVI Output (640x480 @ 60Hz)
DVI::DVI display(DVI::MODE_640x480p60, DVI::COLOUR_RGB565);

void setup() {
    Serial.begin(115200);        // USB debug output
    Serial1.begin(921600);       // High-speed UART to Teensy 4.1 (GPIO 0 TX, GPIO 1 RX)

    Serial.println("\n=== Doomsday Net Computer - RP2040 Display Module ===");
    Serial.println("640x480 DVI Output + UART Receiver");
    Serial.println("Waiting for commands from Teensy 4.1...\n");

    if (!display.begin()) {
        Serial.println("DVI init FAILED!");
        while (1) delay(10);
    }

    // Classic TempleOS / retro terminal splash screen
    display.setTextColor(0x07E0);  // Green
    display.setCursor(90, 140);
    display.setTextSize(2);
    display.println("DOOMSDAY NET COMPUTER");

    display.setTextColor(0xFFFF);  // White
    display.setTextSize(1);
    display.setCursor(130, 200);
    display.println("RP2040 Display + Keyboard Module");
    display.setCursor(170, 220);
    display.println("640×480 DVI Output");

    display.setCursor(100, 280);
    display.setTextColor(0xFFE0);  // Yellow
    display.println("Connected via UART to Teensy 4.1");
    display.setCursor(140, 300);
    display.println("Waiting for commands...");
}

void loop() {
    // Read commands from Teensy over UART
    if (Serial1.available()) {
        String cmd = Serial1.readStringUntil('\n');
        cmd.trim();

        if (cmd.length() > 0) {
            Serial.print("Received from Teensy: ");
            Serial.println(cmd);

            // Simple echo to bottom of screen
            display.fillRect(0, 400, 640, 80, 0x0000);
            display.setCursor(20, 420);
            display.setTextColor(0x07E0);
            display.print("Teensy: ");
            display.setTextColor(0xFFFF);
            display.println(cmd);
        }
    }

    display.swap();
}
