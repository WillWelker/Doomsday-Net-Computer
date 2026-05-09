// Firmware code for the Teensy 4.1  === VERSION 0.27 ===

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include "config.h"
#include "commands.h"
#include "MeshCoreBus.h"
#include "KeyboardInput.h"
#include "TerminalDisplay.h"
#include "ILI9341Driver.h"
#include "FileManager.h"
#include "DoomsdayMessageBus.h"

// ==================== HARDWARE (using safe names from config.h) ====================
const int SD_CS_PIN = DOOMSDAY_SD_CS_PIN;
const int TFT_CS    = DOOMSDAY_TFT_CS;
const int TFT_DC    = DOOMSDAY_TFT_DC;
const int TFT_RST   = DOOMSDAY_TFT_RST;
const int TFT_LED   = DOOMSDAY_TFT_LED;

ILI9341_t3 tft(TFT_CS, TFT_DC, TFT_RST);
ILI9341Driver displayDriver(tft);
TerminalDisplay terminal(displayDriver, 14, 16);

const int INPUT_LINE_Y = 8 + (14 * 16);
KeyboardInput keyboard(tft, INPUT_LINE_Y);

// ==================== USB HOST + MESHCORE ====================
USBSerial meshSerial(keyboard.myusb);
MeshCoreBus mesh(keyboard.myusb, meshSerial);
// Doomsday Message Bus (UART between boards)
DoomsdayMessageBus dmb(DMB_SERIAL, DMB_BAUD_RATE);

FileManager files;



void onMeshMessage(uint8_t chan, const char* from, const char* msg) {
    terminal.addLine(("Mesh: " + String(from) + ": " + String(msg)).c_str(), 0x07FF);
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(500);
    
    pinMode(DOOMSDAY_ONBOARD_LED, OUTPUT);
    pinMode(DOOMSDAY_EXTERNAL_LED, OUTPUT);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
    
    terminal.begin();
    dmb.begin();
    
    
    terminal.addLine("=== Doomsday Net Computer v0.27 ===", 0x07E0);
    terminal.addLine("USB Hub + Shared USBHost + MeshCore", 0xFFFF);
    
    // Serial debug message (visible in Serial Monitor)
    Serial.println("=== Doomsday Net Computer v0.27 - USB Hub + MeshCore Ready ===");
    
    if (!files.begin(SD_CS_PIN)) {
        terminal.addLine("SD FAILED", 0xF800);
    } else {
        terminal.addLine("SD Ready - FileManager initialized", 0x07E0);
    }
    
    files.setOutputCallback([](const char* line, uint16_t color) {
        terminal.addLine(line, color);
    });
    
    keyboard.setCommandCallback(handleCommand);
    keyboard.setOutputCallback([](const char* line, uint16_t color) {
        terminal.addLine(line, color);
    });
    keyboard.begin();
    
    mesh.begin();
    mesh.onMessage(onMeshMessage);
    terminal.addLine("MeshCore (USB Hub) ready", 0x07E0);
    
    // === Initialize Doomsday Message Bus ===
    dmb.begin();
    Serial.println("[DMB] Doomsday Message Bus initialized on Serial2 (pins 9/10)");
    
    // Example subscription (you can change or remove this later)
    dmb.subscribe("mesh", [](const char* topic, const char* msg) {
        Serial.print("[DMB] Received mesh message: ");
        Serial.println(msg);
        // TODO: Forward this to MeshCoreBus when ready
    });
    
    terminal.addLine("Type 'help' for commands", 0xFFFF);
    keyboard.printPrompt();
}

// ==================== LOOP ====================
void loop() {
    keyboard.update();
    mesh.update();

    // Read commands from Serial Monitor (computer)
    if (Serial.available()) {
        static String serialCmd = "";
        while (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                if (serialCmd.length() > 0) {
                    handleCommand(serialCmd.c_str());
                    serialCmd = "";
                }
            } else {
                serialCmd += c;
            }
        }
    }
}