// ==================== INCLUDES ====================
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include <USBHost_t36.h>
#include "config.h"
#include "terminal.h"
#include "commands.h"
#include "MeshCoreBus.h"

// ==================== GLOBALS ====================
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);
// ... your other globals ...

// MeshCore USB Host
USBHost myusb;
USBHub hub1(myusb);
USBSerial meshSerial(myusb);
MeshCoreBus mesh(meshSerial);

void onMeshMessage(uint8_t chan, const char* from, const char* msg) {
    addToTerminal(("Mesh Ch" + String(chan) + " " + from + ": " + msg).c_str(), ILI9341_CYAN);
}

// ==================== SETUP ====================
void setup() {
    // ... your existing setup code ...
    
    // Initialize MeshCore USB Host
    myusb.begin();
    mesh.begin();
    mesh.onMessage(onMeshMessage);
    mesh.setDebugPrint(&Serial);
    addToTerminal("MeshCore USB Host ready", ILI9341_GREEN);
}

// ==================== LOOP ====================
void loop() {
    // ... your existing loop code ...
    
    // Update MeshCore
    myusb.Task();
    mesh.update();
}

// ==================== HANDLE COMMAND ====================
void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    
    if (command == "help") {
        addToTerminal("Available commands:", ILI9341_WHITE);
        addToTerminal("  help - this help", ILI9341_WHITE);
        addToTerminal("  info - system info", ILI9341_WHITE);
        addToTerminal("  mesh <msg> - send to public channel", ILI9341_WHITE);
        addToTerminal("  uptime - show uptime", ILI9341_WHITE);
        addToTerminal("  run <name> - execute Lua script", ILI9341_WHITE);
        addToTerminal("  reset - restart Teensy", ILI9341_WHITE);
    }
    else if (command.startsWith("mesh ")) {
        String msg = command.substring(5);
        msg.trim();
        if (msg.length() > 0) {
            mesh.sendChannel(0, msg.c_str());
            addToTerminal(("Mesh TX: " + msg).c_str(), ILI9341_YELLOW);
        } else {
            addToTerminal("Usage: mesh <message>", ILI9341_RED);
        }
    }
    // ... rest of your command handlers ...
}