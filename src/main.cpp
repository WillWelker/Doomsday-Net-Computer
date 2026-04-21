#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>

// ==================== PIN DEFINITIONS ====================
const int SD_CS_PIN   = BUILTIN_SDCARD;
const int TFT_CS      = 10;
const int TFT_DC      = 9;
const int TFT_RST     = 8;
const int TFT_LED     = 5;        // Backlight
const int ONBOARD_LED = 13;
const int EXTERNAL_LED = 12;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

// Simple scrolling terminal buffer
const int LINES = 15;
String terminalLines[LINES];
int currentLine = 0;

// Forward declarations
void blinkLED(int pin, int times = 1, int onTime = 120, int offTime = 120);
void updateDisplay(const char* line1, const char* line2 = "");
void printPrompt();
void handleCommand(const char* cmd);

void blinkLED(int pin, int times, int onTime, int offTime) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}

void addToTerminal(const char* text) {
    terminalLines[currentLine] = text;
    currentLine = (currentLine + 1) % LINES;

    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(1);

    for (int i = 0; i < LINES; i++) {
        int idx = (currentLine + i) % LINES;
        if (terminalLines[idx].length() > 0) {
            tft.setCursor(4, 8 + i * 16);
            tft.println(terminalLines[idx]);
        }
    }
}

void updateDisplay(const char* line1, const char* line2) {
    addToTerminal(line1);
    if (strlen(line2) > 0) addToTerminal(line2);
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

    // Welcome banner
    addToTerminal("=== Doomsday Net Computer ===");
    addToTerminal("320x240 ILI9341 Scrolling Terminal");

    Serial.print("SD card... ");
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("FAILED");
        addToTerminal("SD card FAILED");
    } else {
        Serial.println("OK");
        addToTerminal("SD card Ready");
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
                addToTerminal(cmdBuffer);     // Echo what you typed
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
        addToTerminal("  help     - this help");
        addToTerminal("  info     - system info");
        addToTerminal("  uptime   - show uptime");
        addToTerminal("  run      - Lua placeholder");
        addToTerminal("  reset    - restart Teensy");
    }
    else if (command == "info") {
        addToTerminal("Teensy 4.1 + ILI9341 Scrolling Terminal");
    }
    else if (command == "uptime") {
        char buf[40];
        snprintf(buf, sizeof(buf), "Uptime: %lu seconds", millis() / 1000);
        addToTerminal(buf);
    }
    else if (command.startsWith("run ")) {
        addToTerminal("Lua script execution coming soon...");
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
        char buf[50];
        snprintf(buf, sizeof(buf), "Unknown: %s", cmd);
        addToTerminal(buf);
    }
}