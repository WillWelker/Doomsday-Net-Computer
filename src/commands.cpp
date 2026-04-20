#include <Arduino.h>
#include <SD.h>
#include "config.h"
#include "led_control.h"
#include "commands.h"

extern File currentDir;

void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    command.toLowerCase();

    if (command == "help") {
        Serial.println("Available commands:");
        Serial.println("  help              - this help");
        Serial.println("  info              - system info");
        Serial.println("  uptime            - show uptime");
        Serial.println("  run blink [n]     - blink LEDs n times (default 3)");
        Serial.println("  run led on        - turn LEDs on");
        Serial.println("  run led off       - turn LEDs off");
        Serial.println("  reset             - restart Teensy");
    }
    else if (command == "info") {
        Serial.println("Teensy 4.1 with simple scriptable LED commands");
    }
    else if (command == "uptime") {
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
    }
    else if (command.startsWith("run ")) {
        String args = command.substring(4);
        args.trim();

        if (args.startsWith("blink")) {
            String numStr = args.substring(5);
            numStr.trim();
            int times = numStr.length() > 0 ? numStr.toInt() : 3;
            if (times < 1) times = 1;
            if (times > 20) times = 20;   // safety limit

            Serial.print("Blinking LEDs ");
            Serial.print(times);
            Serial.println(" times...");

            blinkLED(ONBOARD_LED, times, 120, 120);
            blinkLED(EXTERNAL_LED, times, 120, 120);

            Serial.println("Blink completed.");
        }
        else if (args == "led on") {
            Serial.println("Turning LEDs ON");
            digitalWrite(ONBOARD_LED, HIGH);
            digitalWrite(EXTERNAL_LED, HIGH);
        }
        else if (args == "led off") {
            Serial.println("Turning LEDs OFF");
            digitalWrite(ONBOARD_LED, LOW);
            digitalWrite(EXTERNAL_LED, LOW);
        }
        else {
            Serial.println("Unknown run command. Try: run blink 5, run led on, run led off");
        }
    }
    else if (command == "reset") {
        Serial.println("Resetting...");
        delay(500);
        SCB_AIRCR = 0x05FA0004;
    }
    else if (command.length() > 0) {
        Serial.println("Unknown command. Type 'help'");
    }
}