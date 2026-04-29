#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include <SD.h>
#include <USBHost_t36.h>
#include "config.h"
#include "terminal.h"

extern KeyboardController keyboard1;

// ==================== COMMAND IMPLEMENTATIONS ====================

void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();
    command.toLowerCase();

    if (command == "help") {
    addToTerminal("Available commands:", ILI9341_WHITE);
    addToTerminal("  help    - this help", ILI9341_WHITE);
    addToTerminal("  status  - show system status", ILI9341_WHITE);
    addToTerminal("  ls      - list files", ILI9341_WHITE);
    addToTerminal("  pwd     - print working directory", ILI9341_WHITE);
    addToTerminal("  cd <dir> - change directory", ILI9341_WHITE);
    addToTerminal("  cat <file> - display file", ILI9341_WHITE);
    addToTerminal("  echo \"text\" > file - write to file", ILI9341_WHITE);
    addToTerminal("  mkdir <name> - create directory", ILI9341_WHITE);
    addToTerminal("  rm <file> - delete file", ILI9341_WHITE);
    addToTerminal("  uptime  - show uptime", ILI9341_WHITE);
    addToTerminal("  info    - system info", ILI9341_WHITE);
    addToTerminal("  reset   - restart system", ILI9341_WHITE);
}
    else if (command == "status") {
        addToTerminal("=== Status ===", ILI9341_YELLOW);
        addToTerminal(("Keyboard: " + String(keyboard1 ? "CONNECTED" : "NOT CONNECTED")).c_str(), 
                      keyboard1 ? ILI9341_GREEN : ILI9341_RED);
        addToTerminal(("SD Card: " + String(SD.exists("/") ? "CONNECTED" : "NOT CONNECTED")).c_str(),
                      SD.exists("/") ? ILI9341_GREEN : ILI9341_RED);
        addToTerminal(("Uptime: " + String(millis() / 1000) + "s").c_str(), ILI9341_WHITE);
    }
    else if (command == "ls") {
        File dir = SD.open(currentPath.c_str());
        if (dir) {
            File entry = dir.openNextFile();
            while (entry) {
                String line = entry.isDirectory() ? "DIR  " : "FILE ";
                line += entry.name();
                addToTerminal(line.c_str(), ILI9341_WHITE);
                entry.close();
                entry = dir.openNextFile();
            }
            dir.close();
        }
    }
    else if (command == "pwd") {
        addToTerminal(currentPath.c_str(), ILI9341_WHITE);
    }
    else if (command.startsWith("cd ")) {
        String newPath = command.substring(3);
        newPath.trim();
        if (SD.exists(newPath.c_str())) {
            currentPath = newPath;
            addToTerminal(("Changed to: " + currentPath).c_str(), ILI9341_YELLOW);
        } else {
            addToTerminal("Directory not found", ILI9341_RED);
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
            addToTerminal("File not found", ILI9341_RED);
        }
    }
    else if (command == "uptime") {
        addToTerminal(("Uptime: " + String(millis() / 1000) + " seconds").c_str(), ILI9341_WHITE);
    }
    else if (command == "info") {
        addToTerminal("Teensy 4.1 Terminal v0.20", ILI9341_YELLOW);
    }
    else if (command == "reset") {
        SCB_AIRCR = 0x05FA0004;
    }
    else if (command.length() > 0) {
        addToTerminal(("Unknown: " + command).c_str(), ILI9341_RED);
    }
}

void cmd_help() { handleCommand("help"); }
void cmd_status() { handleCommand("status"); }
void cmd_ls() { handleCommand("ls"); }
void cmd_pwd() { handleCommand("pwd"); }
void cmd_cd(const char* args) { handleCommand(("cd " + String(args)).c_str()); }
void cmd_cat(const char* args) { handleCommand(("cat " + String(args)).c_str()); }
void cmd_echo(const char* args) { handleCommand(("echo " + String(args)).c_str()); }
void cmd_mkdir(const char* args) { handleCommand(("mkdir " + String(args)).c_str()); }
void cmd_rm(const char* args) { handleCommand(("rm " + String(args)).c_str()); }
void cmd_uptime() { handleCommand("uptime"); }
void cmd_info() { handleCommand("info"); }
void cmd_reset() { handleCommand("reset"); }

#endif