#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// Command buffer
const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

// SD card
const int SD_CS_PIN = BUILTIN_SDCARD;   // Teensy 4.1 built-in SD slot
File currentDir;

void printPrompt() {
    Serial.print("\n> ");
}

void handleCommand(const char* cmd);   // Forward declaration

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000) {}
    delay(100);

    Serial.println("\n=== Doomsday Net Computer - Teensy 4.1 Terminal Test ===");
    Serial.println("Basic scripting shell with SD card support");

    // Initialize SD card
    Serial.print("Initializing SD card... ");
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("FAILED! Check card is inserted properly.");
    } else {
        Serial.println("OK");

        // Auto-create boot directory
        if (!SD.exists("/boot")) {
            Serial.print("Creating /boot directory... ");
            if (SD.mkdir("/boot")) {
                Serial.println("OK");
            } else {
                Serial.println("FAILED");
            }
        } else {
            Serial.println("/boot directory already exists");
        }
        
        currentDir = SD.open("/");   // Start at root
    }

    Serial.println("Type 'help' for commands.");
    printPrompt();
}

void loop() {
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

void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    command.toLowerCase();

    if (command == "help") {
        Serial.println("Available commands:");
        Serial.println("  help          - show this help");
        Serial.println("  info          - system information");
        Serial.println("  uptime        - show uptime");
        Serial.println("  free          - approximate free RAM");
        Serial.println("  ls            - list files in current directory");
        Serial.println("  pwd           - print working directory");
        Serial.println("  cd <dir>      - change directory");
        Serial.println("  mkdir <name>  - create new directory");
        Serial.println("  cat <file>    - display text file content");
        Serial.println("  echo <text>   - repeat text");
        Serial.println("  reset         - restart Teensy");
        Serial.println("  mode          - show current mode");
    }
    else if (command == "info") {
        Serial.print("CPU: Teensy 4.1 @ ");
        Serial.print(F_CPU / 1000000);
        Serial.println(" MHz");
        Serial.println("SD card initialized successfully (size reporting not available in basic SD library)");
    }
    else if (command == "uptime") {
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
    }
    else if (command == "free") {
        uint32_t stackTop;
        Serial.print("Approx free stack RAM: ");
        Serial.print((uint32_t)&stackTop - 0x20000000);
        Serial.println(" bytes");
    }
    else if (command == "ls") {
        File dir = currentDir;
        if (!dir) dir = SD.open("/");
        File entry = dir.openNextFile();
        bool hasFiles = false;
        while (entry) {
            hasFiles = true;
            Serial.print(entry.isDirectory() ? "DIR  " : "FILE ");
            Serial.print(entry.name());
            if (!entry.isDirectory()) {
                Serial.print("  (");
                Serial.print(entry.size());
                Serial.print(" bytes)");
            }
            Serial.println();
            entry.close();
            entry = dir.openNextFile();
        }
        if (!hasFiles) Serial.println("(empty directory)");
    }
    else if (command == "pwd") {
        Serial.println("/");   // We'll improve this later with full path tracking
    }
    else if (command.startsWith("cd ")) {
        String path = command.substring(3);
        path.trim();
        if (SD.exists(path.c_str())) {
            currentDir.close();
            currentDir = SD.open(path.c_str());
            Serial.println("Directory changed");
        } else {
            Serial.println("Directory not found");
        }
    }
    else if (command.startsWith("mkdir ")) {
        String dirname = command.substring(6);
        dirname.trim();
        if (SD.mkdir(dirname.c_str())) {
            Serial.println("Directory created");
        } else {
            Serial.println("Failed to create directory");
        }
    }
    else if (command.startsWith("cat ")) {
        String filename = command.substring(4);
        filename.trim();
        File f = SD.open(filename.c_str());
        if (f) {
            while (f.available()) {
                Serial.write(f.read());
            }
            f.close();
        } else {
            Serial.println("File not found or cannot open");
        }
    }
    else if (command.startsWith("echo ")) {
        Serial.println(command.substring(5));
    }
    else if (command == "reset") {
        Serial.println("Resetting in 1 second...");
        delay(1000);
        SCB_AIRCR = 0x05FA0004;
    }
    else if (command == "mode") {
        Serial.println("Current mode: TEST_TERMINAL (0x00)");
        Serial.println("Future: Will read unified mode byte from EEPROM on UNO R4 cluster.");
    }
    else if (command.length() > 0) {
        Serial.print("Unknown command: ");
        Serial.println(cmd);
        Serial.println("Type 'help' for list of commands.");
    }
}