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
    
    if (command == "help") {
        terminal.addLine("Available commands:", 0xFFFF);
        terminal.addLine("  help    - show this help", 0xFFFF);
        terminal.addLine("  info    - system information", 0xFFFF);
        terminal.addLine("  mesh <msg> - send message to public channel", 0xFFFF);
        terminal.addLine("  uptime  - show system uptime", 0xFFFF);
        terminal.addLine("  ls      - list files in current directory", 0x07E0);
        terminal.addLine("  cd <dir> - change directory", 0x07E0);
        terminal.addLine("  cat <file> - display file contents", 0x07E0);
        terminal.addLine("  mkdir <dir> - create directory", 0x07E0);
        terminal.addLine("  rm <file> - delete file", 0x07E0);
        terminal.addLine("  echo <text> - print text", 0xFFFF);
        terminal.addLine("  reset   - restart system", 0xF800);
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
    else if (command == "uptime") {
        char buf[40];
        snprintf(buf, sizeof(buf), "Uptime: %lu seconds", millis() / 1000);
        terminal.addLine(buf, 0xFFFF);
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

#endif