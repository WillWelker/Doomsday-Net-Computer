#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>

// ==================== PIN DEFINITIONS ====================
const int SD_CS_PIN   = BUILTIN_SDCARD;
const int TFT_CS      = 10;
const int TFT_DC      = 9;
const int TFT_RST     = 8;
const int TFT_LED     = 5;
const int ONBOARD_LED = 13;
const int EXTERNAL_LED = 12;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

// Scrolling terminal buffer
const int MAX_LINES = 15;
String terminalLines[MAX_LINES];
int currentLine = 0;

String currentPath = "/";

// Forward declarations
void addToTerminal(const char* text);
void printPrompt();
void handleCommand(const char* cmd);

void blinkLED(int pin, int times = 1, int onTime = 120, int offTime = 120) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}

void addToTerminal(const char* text) {
    terminalLines[currentLine] = text;
    currentLine = (currentLine + 1) % MAX_LINES;

    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(1);

    for (int i = 0; i < MAX_LINES; i++) {
        int idx = (currentLine + i) % MAX_LINES;
        if (terminalLines[idx].length() > 0) {
            tft.setCursor(4, 8 + i * 16);
            tft.println(terminalLines[idx]);
        }
    }
}

void printPrompt() {
    Serial.print("\n> ");
    addToTerminal("> ");
}

void setup() {
    pinMode(ONBOARD_LED, OUTPUT);
    pinMode(EXTERNAL_LED, OUTPUT);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, LOW);
    digitalWrite(EXTERNAL_LED, LOW);
    analogWrite(TFT_LED, 200);

    Serial.begin(115200);
    while (!Serial && millis() < 8000) {}
    delay(200);

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);

    addToTerminal("=== Doomsday Net Computer ===");
    addToTerminal("Basic Terminal Engine v0.2");

    Serial.print("SD card... ");
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("FAILED");
        addToTerminal("SD card FAILED");
        blinkLED(ONBOARD_LED, 6, 60, 60);
    } else {
        Serial.println("OK");
        addToTerminal("SD card Ready");
        blinkLED(ONBOARD_LED, 2, 300, 200);
    }

    addToTerminal("Type 'help' for commands.");
    Serial.println("Type 'help' for commands.");
    printPrompt();
}

void loop() {
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\r' || c == '\n') {
            cmdBuffer[cmdIndex] = '\0';
            if (cmdIndex > 0) {
                addToTerminal(cmdBuffer);
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
        addToTerminal("Available commands:");
        addToTerminal("  help          - this help");
        addToTerminal("  ls            - list files");
        addToTerminal("  pwd           - print working directory");
        addToTerminal("  cd <dir>      - change directory");
        addToTerminal("  cat <file>    - display file content");
        addToTerminal("  mkdir <name>  - create directory");
        addToTerminal("  rm <file>     - delete file");
        addToTerminal("  uptime        - show uptime");
        addToTerminal("  info          - system info");
        addToTerminal("  run <script>  - run Lua script");
        addToTerminal("  reset         - restart Teensy");
    }
    else if (command == "ls") {
        File dir = SD.open(currentPath.c_str());
        if (dir) {
            File entry = dir.openNextFile();
            bool hasFiles = false;
            while (entry) {
                hasFiles = true;
                String line = entry.isDirectory() ? "DIR  " : "FILE ";
                line += entry.name();
                if (!entry.isDirectory()) {
                    line += "  (" + String(entry.size()) + " bytes)";
                }
                addToTerminal(line.c_str());
                entry.close();
                entry = dir.openNextFile();
            }
            if (!hasFiles) addToTerminal("(empty directory)");
            dir.close();
        } else {
            addToTerminal("Error opening directory");
        }
    }
    else if (command == "pwd") {
        addToTerminal(currentPath.c_str());
    }
    else if (command.startsWith("cd ")) {
        String newPath = command.substring(3);
        newPath.trim();
        if (newPath == "..") {
            addToTerminal("cd .. not fully implemented yet");
        } else if (SD.exists(newPath.c_str())) {
            currentPath = newPath;
            addToTerminal(("Changed to: " + currentPath).c_str());
        } else {
            addToTerminal("Directory not found");
        }
    }
    else if (command.startsWith("cat ")) {
        String filename = command.substring(4);
        filename.trim();
        String fullPath = (currentPath == "/" ? "" : currentPath) + "/" + filename;
        if (currentPath == "/") fullPath = "/" + filename;

        File f = SD.open(fullPath.c_str());
        if (f) {
            addToTerminal(("--- " + filename + " ---").c_str());
            while (f.available()) {
                Serial.write(f.read());
            }
            f.close();
            addToTerminal("--- End of file ---");
        } else {
            addToTerminal("File not found");
        }
    }
    else if (command.startsWith("mkdir ")) {
        String dirname = command.substring(6);
        dirname.trim();
        if (SD.mkdir(dirname.c_str())) {
            addToTerminal("Directory created");
        } else {
            addToTerminal("Failed to create directory");
        }
    }
    else if (command.startsWith("rm ")) {
        String filename = command.substring(3);
        filename.trim();
        if (SD.remove(filename.c_str())) {
            addToTerminal("File deleted");
        } else {
            addToTerminal("Failed to delete file");
        }
    }
    else if (command == "uptime") {
        char buf[40];
        snprintf(buf, sizeof(buf), "Uptime: %lu seconds", millis() / 1000);
        addToTerminal(buf);
    }
    else if (command == "info") {
        addToTerminal("Teensy 4.1 Basic Terminal Engine");
        addToTerminal("320x240 ILI9341 TFT");
    }
    else if (command.startsWith("run ")) {
        addToTerminal("Lua script runner coming soon...");
        blinkLED(ONBOARD_LED, 3);
        blinkLED(EXTERNAL_LED, 3);
    }
    else if (command == "reset") {
        addToTerminal("Restarting system...");
        tft.fillScreen(ILI9341_RED);
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(70, 100);
        tft.setTextSize(2);
        tft.println("RESTARTING");
        delay(1000);
        SCB_AIRCR = 0x05FA0004;
    }
    else if (command.length() > 0) {
        char buf[60];
        snprintf(buf, sizeof(buf), "Unknown command: %s", cmd);
        addToTerminal(buf);
    }
}