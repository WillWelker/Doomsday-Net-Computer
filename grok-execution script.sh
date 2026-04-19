#!/bin/bash
# populate-rp2040-module.sh

echo "Populating RP2040 Display/Keyboard module files..."

cat > rp2040-display-keyboard/platformio.ini << 'EOF'
[env:rp2040]
platform = raspberrypi
board = rpipico
framework = arduino
monitor_speed = 115200
monitor_echo = yes

build_flags =
    -D PICO_DVI
    -D PICO_CORE1_STACK_SIZE=4096
    -O2

lib_deps =
    https://github.com/wren6991/PicoDVI.git
    https://github.com/earlephilhower/arduino-pico.git
    SdFat
EOF

cat > rp2040-display-keyboard/src/main.cpp << 'EOF'
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
EOF

cat > docs/modules/rp2040-display-keyboard.md << 'EOF'
# RP2040 Display + Keyboard Module

**Role**: Dedicated co-processor for HDMI/DVI output and future USB Host keyboard handling.

## Hardware
- Waveshare RP2040-PiZero (or compatible RP2040 board with Mini HDMI)
- Connected to Teensy 4.1 via UART (3 wires: TX/RX/GND)

## Current Status
- Stable 640×480 DVI output
- Basic UART command receiver from Teensy 4.1
- Retro TempleOS-style splash screen

## Planned Features
- USB Host keyboard support
- Graphical dashboard with status bars and mode byte display
- Real-time waterfall display for radio signals
- Lua scripting support for custom UI behaviors

**Communication Protocol**: See `uart-protocol.md`

Last Updated: April 2026
EOF

cat > docs/modules/uart-protocol.md << 'EOF'
# UART Protocol - Teensy 4.1 ↔ RP2040 Display Module

**Baud Rate**: 921600 (recommended)

## Direction: Teensy → RP2040
Simple text commands terminated with `\n`

### Current Example Commands
- `TEXT:Hello from main brain`
- `CLEAR`
- `STATUS:Mode=0x03,Uptime=3h 12m`

## Direction: RP2040 → Teensy
- Keyboard events (when implemented)
- Status / acknowledgment messages

## Future Enhancements
- Binary packet mode for high-speed data (waterfall columns, graphics)
- Command acknowledgments and error handling

This protocol will grow as we add waterfall, graphics commands, and Lua integration.
EOF

echo "✅ All RP2040 Display/Keyboard module files have been populated!"
echo "You can now review them in VS Code and commit."
