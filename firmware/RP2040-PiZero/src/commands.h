#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "TerminalDisplay.h"
#include "FileManager.h"
#include "MeshCoreBus.h"

extern TerminalDisplay terminal;
extern FileManager files;
extern MeshCoreBus mesh;
extern String currentPath;

void handleCommand(const char* cmd) {
    String command = String(cmd);
    command.trim();

    // Mirror to Serial Monitor
    Serial.print("> ");
    Serial.println(command);

    if (command == "help") {
        terminal.addLine("Available commands:", 0xFFFF);
        Serial.println("Available commands:");
        terminal.addLine("  help     - show this help", 0xFFFF);
        Serial.println("  help     - show this help");
        terminal.addLine("  usb      - show USB host & connected devices status", 0x07FF);
        Serial.println("  usb      - show USB host & connected devices status");
        terminal.addLine("  info     - system information", 0xFFFF);
        Serial.println("  info     - system information");
        terminal.addLine("  mesh <msg> - send message to public channel", 0xFFFF);
        Serial.println("  mesh <msg> - send message to public channel");
        terminal.addLine("  uptime   - show system uptime", 0xFFFF);
        Serial.println("  uptime   - show system uptime");
        terminal.addLine("  ls       - list files in current directory", 0x07E0);
        Serial.println("  ls       - list files in current directory");
        terminal.addLine("  cd <dir> - change directory", 0x07E0);
        Serial.println("  cd <dir> - change directory");
        terminal.addLine("  cat <file> - display file contents", 0x07E0);
        Serial.println("  cat <file> - display file contents");
        terminal.addLine("  mkdir <dir> - create directory", 0x07E0);
        Serial.println("  mkdir <dir> - create directory");
        terminal.addLine("  rm <file> - delete file", 0x07E0);
        Serial.println("  rm <file> - delete file");
        terminal.addLine("  echo <text> - print text", 0xFFFF);
        Serial.println("  echo <text> - print text");
        terminal.addLine("  reset    - restart system", 0xF800);
        Serial.println("  reset    - restart system");
    }
    else if (command == "usb") {
        terminal.addLine("=== USB Status ===", 0x07FF);
        Serial.println("=== USB Status ===");
        terminal.addLine("USB Host: Active (shared stack)", 0x07E0);
        Serial.println("USB Host: Active (shared stack)");
        terminal.addLine("Keyboard: Connected via powered USB hub ✓", 0x07E0);
        Serial.println("Keyboard: Connected via powered USB hub ✓");
        terminal.addLine(("MeshCore: " + String(mesh.isConnected() ? "Connected (USB)" : "Not detected")).c_str(), 
                         mesh.isConnected() ? 0x07E0 : 0xF800);
        Serial.print("MeshCore: ");
        Serial.println(mesh.isConnected() ? "Connected (USB)" : "Not detected");
        terminal.addLine("Powered Hub: In use (recommended for stability)", 0xFFFF);
        Serial.println("Powered Hub: In use (recommended for stability)");
    }
    else if (command.startsWith("mesh ")) {
        String msg = command.substring(5);
        msg.trim();
        if (msg.length() > 0) {
            mesh.sendChannel(0, msg.c_str());
            terminal.addLine(("Mesh TX: " + msg).c_str(), 0xFFE0);
            Serial.println("Mesh TX: " + msg);
        }
    }
    else if (command == "info") {
        terminal.addLine("Doomsday Net Computer v0.27 - USB Hub + MeshCore Ready", 0x07E0);
        Serial.println("Doomsday Net Computer v0.27 - USB Hub + MeshCore Ready");
    }
    else if (command == "uptime") {
        char buf[40];
        snprintf(buf, sizeof(buf), "Uptime: %lu seconds", millis() / 1000);
        terminal.addLine(buf, 0xFFFF);
        Serial.println(buf);
    }
    else if (command == "ls") {
        files.listDir();
    }
    else if (command.startsWith("cd ")) {
        String path = command.substring(3);
        path.trim();
        if (files.changeDir(path.c_str())) {
            terminal.addLine(("Now in: " + files.getCurrentPath()).c_str(), 0x07E0);
            Serial.println("Now in: " + files.getCurrentPath());
        } else {
            terminal.addLine("Directory not found", 0xF800);
            Serial.println("Directory not found");
        }
    }
    else if (command.startsWith("cat ")) {
        String filename = command.substring(4);
        filename.trim();
        if (files.catFile(filename.c_str())) {
            terminal.addLine("File displayed", 0x07E0);
            Serial.println("File displayed");
        } else {
            terminal.addLine("File not found", 0xF800);
            Serial.println("File not found");
        }
    }
    else if (command.startsWith("mkdir ")) {
        String dirname = command.substring(6);
        dirname.trim();
        if (files.makeDir(dirname.c_str())) {
            terminal.addLine("Directory created", 0x07E0);
            Serial.println("Directory created");
        } else {
            terminal.addLine("Failed", 0xF800);
            Serial.println("Failed");
        }
    }
    else if (command.startsWith("rm ")) {
        String filename = command.substring(3);
        filename.trim();
        if (files.removeFile(filename.c_str())) {
            terminal.addLine("File deleted", 0x07E0);
            Serial.println("File deleted");
        } else {
            terminal.addLine("Failed", 0xF800);
            Serial.println("Failed");
        }
    }
    else if (command.startsWith("echo ")) {
        String text = command.substring(5);
        terminal.addLine(text.c_str(), 0xFFFF);
        Serial.println(text);
    }
    else if (command == "reset") {
        Serial.println("Restarting system...");
        SCB_AIRCR = 0x05FA0004;
    }
    else {
        terminal.addLine(("Unknown: " + command).c_str(), 0xF800);
        Serial.println("Unknown command: " + command);
    }
}
#endif