#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include "MeshCoreBus.h"
#include "KeyboardInput.h"
#include "TerminalDisplay.h"
#include "ILI9341Driver.h"
#include "FileManager.h"

// ==================== HARDWARE ====================
const int SD_CS_PIN = BUILTIN_SDCARD;
const int TFT_CS = 10, TFT_DC = 9, TFT_RST = 8, TFT_LED = 5;

ILI9341_t3 tft(TFT_CS, TFT_DC, TFT_RST);
ILI9341Driver displayDriver(tft);
TerminalDisplay terminal(displayDriver, 14, 16);

const int INPUT_LINE_Y = 8 + (14 * 16);
KeyboardInput keyboard(tft, INPUT_LINE_Y);

MeshCoreBus mesh(Serial1, 115200);
FileManager files;

// ==================== COMMAND HANDLER ====================
void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    
    if (command == "help") {
        terminal.addLine("Commands: help, info, mesh, ls, cd, cat, mkdir, rm, echo, reset", 0xFFFF);
    }
    else if (command.startsWith("mesh ")) {
        String msg = command.substring(5);
        msg.trim();
        if (msg.length() > 0) {
            mesh.sendChannel(0, msg.c_str());
            terminal.addLine(("Mesh TX: " + msg).c_str(), 0xFFE0);
        }
    }
    else if (command == "info") {
        terminal.addLine("Doomsday Net Computer v0.26 - FileManager Ready", 0x07E0);
    }
    else if (command == "ls") {
        files.listDir();
    }
    else if (command.startsWith("cd ")) {
        String path = command.substring(3);
        path.trim();
        files.changeDir(path.c_str()) ? 
            terminal.addLine(("Now in: " + files.getCurrentPath()).c_str(), 0x07E0) :
            terminal.addLine("Directory not found", 0xF800);
    }
    else if (command.startsWith("cat ")) {
        String filename = command.substring(4);
        filename.trim();
        files.catFile(filename.c_str()) ? 
            terminal.addLine("File displayed", 0x07E0) :
            terminal.addLine("File not found", 0xF800);
    }
    else if (command.startsWith("mkdir ")) {
        String dirname = command.substring(6);
        dirname.trim();
        files.makeDir(dirname.c_str()) ? 
            terminal.addLine("Directory created", 0x07E0) :
            terminal.addLine("Failed", 0xF800);
    }
    else if (command.startsWith("rm ")) {
        String filename = command.substring(3);
        filename.trim();
        files.removeFile(filename.c_str()) ? 
            terminal.addLine("File deleted", 0x07E0) :
            terminal.addLine("Failed", 0xF800);
    }
    else if (command.startsWith("echo ")) {
        String text = command.substring(5);
        terminal.addLine(text.c_str(), 0xFFFF);
    }
    else if (command == "reset") {
        SCB_AIRCR = 0x05FA0004;
    }
    else {
        terminal.addLine(("Unknown: " + command).c_str(), 0xF800);
    }
}

void onMeshMessage(uint8_t chan, const char* from, const char* msg) {
    terminal.addLine(("Mesh: " + String(from) + ": " + String(msg)).c_str(), 0x07FF);
}

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    delay(500);
    
    pinMode(ONBOARD_LED, OUTPUT);
    pinMode(EXTERNAL_LED, OUTPUT);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
    
    terminal.begin();
    terminal.addLine("=== Doomsday Net Computer v0.26 ===", 0x07E0);
    terminal.addLine("FileManager + TerminalDisplay + KeyboardInput", 0xFFFF);
    
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
    terminal.addLine("MeshCore ready", 0x07E0);
    
    terminal.addLine("Type 'help' for commands", 0xFFFF);
    keyboard.printPrompt();
}

// ==================== LOOP ====================
void loop() {
    keyboard.update();
    mesh.update();
}