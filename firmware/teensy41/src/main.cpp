// Doomsday Net Computer - With DoomsdayMessageBus + MeshCoreBus
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include <USBHost_t36.h>
#include "config.h"
#include "terminal.h"
#include "commands.h"
#include <DoomsdayMessageBus.h>
#include <MeshCoreBus.h>

// ==================== GLOBALS ====================
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);
TerminalLine terminalLines[MAX_LINES];
int scrollOffset = 0;

USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
KeyboardController keyboard1(myusb);

// Doomsday Message Bus on Serial1 (pins 0/1)
DoomsdayMessageBus doomsdayBus(Serial1);

// MeshCore Bus on Serial2 (pins 7/8) - DIFFERENT UART
MeshCoreBus meshCore(Serial2);

char cmdBuffer[MAX_CMD];
int cmdIndex = 0;
String currentPath = "/";

// ==================== USB KEYBOARD ====================
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

    addToTerminal("=== Doomsday Net Computer v0.22 ===", ILI9341_GREEN);
    addToTerminal("Libraries: Terminal, Commands, DoomsdayBus, MeshCoreBus", ILI9341_YELLOW);

    if (!SD.begin(SD_CS_PIN)) {
        addToTerminal("SD card FAILED", ILI9341_RED);
    } else {
        addToTerminal("SD card Ready", ILI9341_GREEN);
    }

    keyboard1.attachPress(OnPress);
    myusb.begin();

    // Initialize Doomsday Message Bus (Serial1)
    doomsdayBus.begin();
    doomsdayBus.subscribe("status", [](const char* topic, const char* msg) {
        addToTerminal(("Doomsday: " + String(msg)).c_str(), ILI9341_CYAN);
    });

    // Initialize MeshCore Bus (Serial2)
    meshCore.begin();
    meshCore.onMessage([](const char* from, const char* msg) {
        addToTerminal(("Mesh: " + String(from) + ": " + String(msg)).c_str(), ILI9341_CYAN);
    });

    printPrompt();
}

void loop() {
    myusb.Task();
    doomsdayBus.update();
    meshCore.update();
    
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