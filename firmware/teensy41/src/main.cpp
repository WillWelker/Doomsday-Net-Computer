// Doomsday Net Computer - Terminal Engine
// Version 0.14 - Proper Scrolling + Live Input

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include <USBHost_t36.h>

// ==================== PIN DEFINITIONS ====================
const int SD_CS_PIN   = BUILTIN_SDCARD;
const int TFT_CS      = 10;
const int TFT_DC      = 9;
const int TFT_RST     = 8;
const int TFT_LED     = 5;
const int ONBOARD_LED = 13;
const int EXTERNAL_LED = 12;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

// ==================== USB HOST ====================
USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
KeyboardController keyboard1(myusb);

// ==================== TERMINAL STATE ====================
const int MAX_LINES = 15;
const int LINE_HEIGHT = 16;

struct TerminalLine {
    String text;
    uint16_t color;
};

TerminalLine terminalLines[MAX_LINES];
int scrollOffset = 0;  // Current scroll position

const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

String currentPath = "/";

// Forward declarations
void addToTerminal(const char* text, uint16_t color = ILI9341_GREEN);
void handleCommand(const char* cmd);
void OnPress(int key);
void scrollUp();

void blinkLED(int pin, int times = 1, int onTime = 120, int offTime = 120) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}

void scrollUp() {
    for (int i = 0; i < MAX_LINES - 1; i++) {
        terminalLines[i] = terminalLines[i + 1];
    }
    terminalLines[MAX_LINES - 1].text = "";
    terminalLines[MAX_LINES - 1].color = ILI9341_GREEN;
    
    tft.fillScreen(ILI9341_BLACK);
    for (int i = 0; i < MAX_LINES - 1; i++) {
        if (terminalLines[i].text.length() > 0) {
            tft.setCursor(4, 8 + i * LINE_HEIGHT);
            tft.setTextColor(terminalLines[i].color);
            tft.setTextSize(1);
            tft.print(terminalLines[i].text);
        }
    }
}

void addToTerminal(const char* text, uint16_t color) {
    if (scrollOffset >= MAX_LINES - 1) {
        scrollUp();
        scrollOffset = MAX_LINES - 2;
    }
    
    terminalLines[scrollOffset].text = text;
    terminalLines[scrollOffset].color = color;
    
    tft.setCursor(4, 8 + scrollOffset * LINE_HEIGHT);
    tft.setTextColor(color);
    tft.setTextSize(1);
    tft.print(text);
    
    scrollOffset++;
    Serial.println(text);
}

void printPrompt() {
    Serial.print("\n> ");
    addToTerminal("> ", ILI9341_YELLOW);
}

// USB Keyboard callback - LIVE INPUT WITH PROPER SCROLLING
void OnPress(int key) {
    if (key == 10 || key == 13) {  // Enter
        cmdBuffer[cmdIndex] = '\0';
        if (cmdIndex > 0) {
            addToTerminal(cmdBuffer, ILI9341_YELLOW);
            handleCommand(cmdBuffer);
        }
        cmdIndex = 0;
        tft.fillRect(4, 8 + scrollOffset * LINE_HEIGHT, 312, LINE_HEIGHT, ILI9341_BLACK);
        printPrompt();
    }
    else if (key == 8 || key == 127) {  // Backspace
        if (cmdIndex > 0) {
            cmdIndex--;
            tft.fillRect(4, 8 + scrollOffset * LINE_HEIGHT, 312, LINE_HEIGHT, ILI9341_BLACK);
            tft.setCursor(4, 8 + scrollOffset * LINE_HEIGHT);
            tft.setTextColor(ILI9341_YELLOW);
            tft.setTextSize(1);
            tft.print("> ");
            for (int i = 0; i < cmdIndex; i++) {
                tft.print(cmdBuffer[i]);
            }
        }
    }
    else if (cmdIndex < MAX_CMD - 1 && key >= 32 && key <= 126) {
        cmdBuffer[cmdIndex++] = (char)key;
        tft.fillRect(4, 8 + scrollOffset * LINE_HEIGHT, 312, LINE_HEIGHT, ILI9341_BLACK);
        tft.setCursor(4, 8 + scrollOffset * LINE_HEIGHT);
        tft.setTextColor(ILI9341_YELLOW);
        tft.setTextSize(1);
        tft.print("> ");
        for (int i = 0; i < cmdIndex; i++) {
            tft.print(cmdBuffer[i]);
        }
    }
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

    addToTerminal("=== Doomsday Net Computer ===", ILI9341_GREEN);
    addToTerminal("Terminal Engine v0.14 + USB Host", ILI9341_YELLOW);
    addToTerminal("Type 'help' for commands.", ILI9341_WHITE);

    Serial.print("SD card... ");
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("FAILED");
        addToTerminal("SD card FAILED", ILI9341_RED);
        blinkLED(ONBOARD_LED, 6, 60, 60);
    } else {
        Serial.println("OK");
        addToTerminal("SD card Ready", ILI9341_GREEN);
        blinkLED(ONBOARD_LED, 2, 300, 200);
    }

    Serial.println("USB Host initializing...");
    keyboard1.attachPress(OnPress);
    myusb.begin();
    Serial.println("USB Host ready - connect keyboard");

    printPrompt();
}

void loop() {
    myusb.Task();
    
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\r' || c == '\n') {
            cmdBuffer[cmdIndex] = '\0';
            if (cmdIndex > 0) {
                addToTerminal(cmdBuffer, ILI9341_YELLOW);
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
        addToTerminal("Available commands:", ILI9341_WHITE);
        addToTerminal("  help             - this help", ILI9341_WHITE);
        addToTerminal("  upload <file>    - upload file over USB (end with EOF)", ILI9341_WHITE);
        addToTerminal("  ls               - list files", ILI9341_WHITE);
        addToTerminal("  pwd              - print working directory", ILI9341_WHITE);
        addToTerminal("  cd <dir>         - change directory", ILI9341_WHITE);
        addToTerminal("  cat <file>       - display file content", ILI9341_WHITE);
        addToTerminal("  echo \"text\" > file - create/write file", ILI9341_WHITE);
        addToTerminal("  mkdir <name>     - create directory", ILI9341_WHITE);
        addToTerminal("  rm <file>        - delete file", ILI9341_WHITE);
        addToTerminal("  uptime           - show uptime", ILI9341_WHITE);
        addToTerminal("  info             - system info", ILI9341_WHITE);
        addToTerminal("  reset            - restart Teensy", ILI9341_WHITE);
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
                addToTerminal(line.c_str(), ILI9341_WHITE);
                entry.close();
                entry = dir.openNextFile();
            }
            if (!hasFiles) addToTerminal("(empty directory)", ILI9341_WHITE);
            dir.close();
        } else {
            addToTerminal("Error opening directory", ILI9341_RED);
        }
    }
    else if (command == "pwd") {
        addToTerminal(currentPath.c_str(), ILI9341_WHITE);
    }
    else if (command.startsWith("cd ")) {
        String newPath = command.substring(3);
        newPath.trim();
        if (newPath == "..") {
            addToTerminal("cd .. not fully implemented yet", ILI9341_YELLOW);
        } else if (SD.exists(newPath.c_str())) {
            currentPath = newPath;
            addToTerminal(("Changed to: " + currentPath).c_str(), ILI9341_YELLOW);
        } else {
            addToTerminal("Directory not found", ILI9341_RED);
        }
    }
    else if (command.startsWith("cat ")) {
        String filename = command.substring(4);
        filename.trim();
        String fullPath = (currentPath == "/" ? "" : currentPath) + "/" + filename;
        if (currentPath == "/") fullPath = "/" + filename;

        File f = SD.open(fullPath.c_str());
        if (f) {
            addToTerminal(("--- " + filename + " ---").c_str(), ILI9341_YELLOW);
            while (f.available()) {
                Serial.write(f.read());
            }
            f.close();
            addToTerminal("--- End of file ---", ILI9341_YELLOW);
        } else {
            addToTerminal("File not found", ILI9341_RED);
        }
    }
    else if (command.startsWith("echo ")) {
        int arrowPos = command.indexOf(">");
        if (arrowPos != -1) {
            String text = command.substring(5, arrowPos);
            text.trim();
            String filename = command.substring(arrowPos + 1);
            filename.trim();

            String fullPath = (currentPath == "/" ? "" : currentPath) + "/" + filename;
            if (currentPath == "/") fullPath = "/" + filename;

            File f = SD.open(fullPath.c_str(), FILE_WRITE);
            if (f) {
                f.println(text);
                f.close();
                addToTerminal(("Written to " + filename).c_str(), ILI9341_YELLOW);
            } else {
                addToTerminal("Failed to write file", ILI9341_RED);
            }
        } else {
            addToTerminal(command.substring(5).c_str(), ILI9341_WHITE);
        }
    }
    else if (command.startsWith("mkdir ")) {
        String dirname = command.substring(6);
        dirname.trim();
        if (SD.mkdir(dirname.c_str())) {
            addToTerminal("Directory created", ILI9341_YELLOW);
        } else {
            addToTerminal("Failed to create directory", ILI9341_RED);
        }
    }
    else if (command.startsWith("rm ")) {
        String filename = command.substring(3);
        filename.trim();
        if (SD.remove(filename.c_str())) {
            addToTerminal("File deleted", ILI9341_YELLOW);
        } else {
            addToTerminal("Failed to delete file", ILI9341_RED);
        }
    }
    else if (command == "uptime") {
        char buf[40];
        snprintf(buf, sizeof(buf), "Uptime: %lu seconds", millis() / 1000);
        addToTerminal(buf, ILI9341_WHITE);
    }
    else if (command == "info") {
        addToTerminal("Teensy 4.1 Terminal Engine v0.14", ILI9341_YELLOW);
        addToTerminal("USB Host Keyboard + SD Card", ILI9341_WHITE);
    }
    else if (command == "reset") {
        addToTerminal("Restarting system...", ILI9341_YELLOW);
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
        addToTerminal(buf, ILI9341_RED);
    }
}