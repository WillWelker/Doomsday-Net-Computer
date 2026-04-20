#include <Arduino.h>
#include <SD.h>
#include "config.h"
#include "led_control.h"
#include "sd_manager.h"
#include "commands.h"

File currentDir;

void printPrompt() {
    Serial.print("\n> ");
}

void setup() {
    pinMode(ONBOARD_LED, OUTPUT);
    pinMode(EXTERNAL_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, LOW);
    digitalWrite(EXTERNAL_LED, LOW);

    Serial.begin(115200);
    while (!Serial && millis() < 8000) {}
    delay(200);

    Serial.println("\n=== Doomsday Net Computer - Teensy 4.1 Terminal + Lua Scripting ===");

    blinkLED(ONBOARD_LED, 3, 80, 80);
    blinkLED(EXTERNAL_LED, 3, 80, 80);

    Serial.print("Initializing SD card... ");
    if (!initSDCard()) {
        Serial.println("FAILED!");
        blinkLED(ONBOARD_LED, 6, 60, 60);
        blinkLED(EXTERNAL_LED, 6, 60, 60);
    } else {
        Serial.println("OK");
        blinkLED(ONBOARD_LED, 2, 300, 200);
        blinkLED(EXTERNAL_LED, 2, 300, 200);

        ensureDefaultLuaScript();

        currentDir = SD.open("/");
    }

    Serial.println("Available commands: help, info, uptime, run <script>, reset");
    printPrompt();
}

void loop() {
    static char cmdBuffer[MAX_CMD];
    static int cmdIndex = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\r' || c == '\n') {
            cmdBuffer[cmdIndex] = '\0';
            if (cmdIndex > 0) {
                handleCommand(cmdBuffer);
            }
            cmdIndex = 0;
            printPrompt();
        }
        else if (cmdIndex < MAX_CMD - 1) {
            cmdBuffer[cmdIndex++] = c;
        }
    }
}
