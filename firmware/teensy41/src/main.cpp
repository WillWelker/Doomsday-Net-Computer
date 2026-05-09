// Doomsday Net Computer - Terminal Engine
// Version 0.33 - DoomsdayMessageBus to RP2040 DVI + MeshCoreBus

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include <USBHost_t36.h>
#include "MeshCoreBus.h"
#include "DoomsdayMessageBus.h"

// ==================== HARDWARE ====================
const int SD_CS_PIN   = BUILTIN_SDCARD;
const int TFT_CS      = 10;
const int TFT_DC      = 9;
const int TFT_RST     = 8;
const int TFT_LED     = 5;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

// ==================== USB HOST ====================
USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
KeyboardController keyboard1(myusb);
USBSerial meshUSBSerial(myusb);

// ==================== MESHCORE BUS ====================
MeshCoreBus meshCoreBus(myusb, meshUSBSerial);

// ==================== DOOMSDAY MESSAGE BUS (to RP2040 DVI) ====================
DoomsdayMessageBus dmbToDisplay(Serial1);

// ==================== TERMINAL STATE ====================
const int MAX_LINES = 15;
const int LINE_HEIGHT = 16;

struct TerminalLine {
    String text;
    uint16_t color;
};

TerminalLine terminalLines[MAX_LINES];
int scrollOffset = 0;

const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

String currentPath = "/";

// ==================== TIMERS ====================
uint32_t lastSend = 0;

// ==================== FORWARD DECLARATIONS ====================
void addToTerminal(const char* text, uint16_t color = ILI9341_GREEN);
void handleCommand(const char* cmd);
void OnPress(int key);
void scrollUp();
void printPrompt();
void checkMeshCore();
void sendTestMessage();

// ==================== MESHCORE LISTENER ====================
void checkMeshCore() {
    meshCoreBus.update();
}

// ==================== DOOMSDAY MESSAGE BUS SENDER (to RP2040 DVI) ====================
void sendTestMessage() {
    String msg = "[TEST] Doomsday UART link - " + String(millis() / 1000) + "s uptime";
    
    dmbToDisplay.publish("display", msg.c_str());
    Serial.println("[DMB TX] " + msg);
    
    tft.setTextColor(ILI9341_YELLOW);
    tft.print(msg);
    tft.print("\n");
    tft.setTextColor(ILI9341_WHITE);
}

// ==================== MESHCORE CALLBACK ====================
void onMeshCoreMessage(uint8_t chan, const char* from, const char* msg) {
    char buf[128];
    snprintf(buf, sizeof(buf), "[MeshCore ch%d] %s: %s", chan, from, msg);
    
    addToTerminal(buf, ILI9341_CYAN);           // Show on local TFT
    dmbToDisplay.publish("display", buf);       // Forward to RP2040 DVI display
}

// ==================== TERMINAL FUNCTIONS ====================
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
    if (scrollOffset >= MAX_LINES - 1) scrollUp();
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

void OnPress(int key) {
    if (key == 10 || key == 13) {
        cmdBuffer[cmdIndex] = '\0';
        if (cmdIndex > 0) {
            addToTerminal(cmdBuffer, ILI9341_YELLOW);
            handleCommand(cmdBuffer);
        }
        cmdIndex = 0;
        tft.fillRect(4, 8 + scrollOffset * LINE_HEIGHT, 312, LINE_HEIGHT, ILI9341_BLACK);
        printPrompt();
    }
    else if (key == 8 || key == 127) {
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

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(300);

    Serial1.begin(921600);
    dmbToDisplay.begin();

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);

    addToTerminal("=== Doomsday Net Computer v0.33 ===", ILI9341_GREEN);
    addToTerminal("DoomsdayMessageBus + MeshCoreBus + SD", ILI9341_YELLOW);

    if (!SD.begin(SD_CS_PIN)) {
        addToTerminal("SD FAILED", ILI9341_RED);
    } else {
        addToTerminal("SD Ready", ILI9341_GREEN);
    }

    keyboard1.attachPress(OnPress);
    myusb.begin();

    meshCoreBus.begin();
    meshCoreBus.onMessage(onMeshCoreMessage);
    meshCoreBus.setDebugPrint(&Serial);

    addToTerminal("MeshCoreBus + DMB ready", ILI9341_CYAN);
    printPrompt();
}

// ==================== LOOP ====================
void loop() {
    myusb.Task();
    checkMeshCore();

    if (millis() - lastSend > 10000) {
        sendTestMessage();
        lastSend = millis();
    }

    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\r' || c == '\n') {
            cmdBuffer[cmdIndex] = '\0';
            if (cmdIndex > 0) {
                addToTerminal(cmdBuffer, ILI9341_YELLOW);
                handleCommand(cmdBuffer);
            }
            cmdIndex = 0;
            printPrompt();
        } else if (cmdIndex < MAX_CMD - 1) {
            cmdBuffer[cmdIndex++] = c;
        }
    }
}

void handleCommand(const char* cmd) {
    String c = String(cmd);
    c.trim();
    c.toLowerCase();

    if (c == "help") {
        addToTerminal("Commands: help, status, ls, mesh <msg>, reset", ILI9341_WHITE);
    }
    else if (c.startsWith("mesh ")) {
        String msg = c.substring(5);
        msg.trim();
        if (msg.length() > 0) {
            meshCoreBus.sendChannel(0, msg);
            addToTerminal(("[MeshCore TX] " + msg).c_str(), ILI9341_GREEN);
            
            // Also send to RP2040 DVI display via DMB
            dmbToDisplay.publish("display", ("[MeshCore] " + msg).c_str());
        }
    }
    else if (c == "status") {
        addToTerminal(("MeshCore connected: " + String(meshCoreBus.isConnected() ? "YES" : "NO")).c_str(), ILI9341_WHITE);
    }
    else if (c == "reset") {
        SCB_AIRCR = 0x05FA0004;
    }
    else {
        addToTerminal("Unknown command", ILI9341_RED);
    }
}