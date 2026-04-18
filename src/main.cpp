#include <Arduino.h>

// Simple command buffer
const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

void printPrompt() {
    Serial.print("\n> ");
}

void handleCommand(const char* cmd);   // Forward declaration

void setup() {
    Serial.begin(115200);
    
    // Wait for Serial Monitor to connect (very important on Teensy 4.1)
    while (!Serial && millis() < 5000) {
        ; 
    }
    
    delay(100);
    
    Serial.println("\n=== Doomsday Net Computer - Teensy 4.1 Terminal Test ===");
    Serial.println("Basic scripting / command shell ready.");
    Serial.println("Type 'help' for available commands.");
    Serial.println("This is the test bed while waiting for display + keyboard hardware.");
    printPrompt();
}

void loop() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        if (c == '\r' || c == '\n') {
            cmdBuffer[cmdIndex] = '\0';  // null terminate
            
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

void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    command.toLowerCase();
    
    if (command == "help") {
        Serial.println("Available commands:");
        Serial.println("  help     - show this help");
        Serial.println("  info     - system information");
        Serial.println("  uptime   - show time since boot");
        Serial.println("  echo <text> - repeat text back");
        Serial.println("  free     - show approximate free RAM");
        Serial.println("  reset    - software reset the Teensy");
        Serial.println("  mode     - show current firmware mode (placeholder for unified system)");
    }
    else if (command == "info") {
        Serial.print("CPU: Teensy 4.1 @ ");
        Serial.print(F_CPU / 1000000);
        Serial.println(" MHz");
        Serial.println("Board: Teensy 4.1");
        Serial.println("Purpose: Early terminal test for Doomsday Ham Radio Comms Computer");
        Serial.println("Will later control / link to 5× UNO R4 Minima via UART.");
    }
    else if (command.startsWith("echo ")) {
        Serial.println(command.substring(5));
    }
    else if (command == "uptime") {
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
    }
    else if (command == "free") {
        // Simple & safe way to show free stack space on Teensy 4.1
        uint32_t stackTop;
        Serial.print("Approximate free stack RAM: ");
        Serial.print((uint32_t)&stackTop - 0x20000000);  // rough estimate
        Serial.println(" bytes (stack side)");
        Serial.println("Note: Teensy 4.1 has 512KB RAM1 + 512KB RAM2. Full memory reporting is complex.");
    }
    else if (command == "reset") {
        Serial.println("Resetting in 1 second...");
        delay(1000);
        SCB_AIRCR = 0x05FA0004;  // Software reset for Teensy 4.x
    }
    else if (command == "mode") {
        Serial.println("Current mode: TEST_TERMINAL (0x00)");
        Serial.println("In the final unified firmware this will read the mode byte from EEPROM on the UNO R4 cluster.");
    }
    else if (command.length() > 0) {
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        Serial.println("Type 'help' for list of commands.");
    }
}