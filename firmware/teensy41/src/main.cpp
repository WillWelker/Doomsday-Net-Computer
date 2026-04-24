// Doomsday Net Computer - Terminal Engine
// Version 0.10 - First working Lua integration

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ILI9341_t3.h>
#include <lua.hpp>

// ==================== PIN DEFINITIONS ====================
const int SD_CS_PIN   = BUILTIN_SDCARD;
const int TFT_CS      = 10;
const int TFT_DC      = 9;
const int TFT_RST     = 8;
const int TFT_LED     = 5;
const int ONBOARD_LED = 13;
const int EXTERNAL_LED = 12;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);

const int MAX_LINES = 15;
const int LINE_HEIGHT = 16;

struct TerminalLine {
    String text;
    uint16_t color;
};

TerminalLine terminalLines[MAX_LINES];
int currentLine = 0;
int displayRow = 0;

const int MAX_CMD = 128;
char cmdBuffer[MAX_CMD];
int cmdIndex = 0;

String currentPath = "/";

lua_State* L = NULL;

// Forward declarations
void addToTerminal(const char* text, uint16_t color = ILI9341_GREEN);
void handleCommand(const char* cmd);

void blinkLED(int pin, int times = 1, int onTime = 120, int offTime = 120) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}

void redrawScreen() {
    tft.fillScreen(ILI9341_BLACK);
    for (int i = 0; i < MAX_LINES; i++) {
        int idx = (currentLine + i) % MAX_LINES;
        if (terminalLines[idx].text.length() > 0) {
            tft.setCursor(4, 8 + i * LINE_HEIGHT);
            tft.setTextColor(terminalLines[idx].color);
            tft.setTextSize(1);
            tft.print(terminalLines[idx].text);
        }
    }
}

void addToTerminal(const char* text, uint16_t color) {
    terminalLines[currentLine].text = text;
    terminalLines[currentLine].color = color;
    currentLine = (currentLine + 1) % MAX_LINES;
    
    if (displayRow < MAX_LINES) {
        tft.setCursor(4, 8 + displayRow * LINE_HEIGHT);
        tft.setTextColor(color);
        tft.setTextSize(1);
        tft.print(text);
        displayRow++;
    } else {
        redrawScreen();
    }
    
    Serial.println(text);
}

void printPrompt() {
    Serial.print("\n> ");
    addToTerminal("> ", ILI9341_YELLOW);
}

// Lua print function binding
static int lua_print(lua_State* L) {
    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; i++) {
        const char* str = lua_tostring(L, i);
        if (str) {
            addToTerminal(str, ILI9341_WHITE);
        }
    }
    return 0;
}

void initLua() {
    L = luaL_newstate();
    luaL_openlibs(L);
    
    // Register our custom print function
    lua_register(L, "print", lua_print);
    
    Serial.println("Lua interpreter initialized");
}

void runLuaScript(const char* filename) {
    String fullPath = (currentPath == "/" ? "" : currentPath) + "/" + filename;
    if (currentPath == "/") fullPath = "/" + filename;

    File f = SD.open(fullPath.c_str());
    if (!f) {
        addToTerminal(("Lua file not found: " + String(filename)).c_str(), ILI9341_RED);
        return;
    }

    String script = "";
    while (f.available()) {
        script += (char)f.read();
    }
    f.close();

    if (luaL_dostring(L, script.c_str()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        addToTerminal(("Lua error: " + String(err)).c_str(), ILI9341_RED);
        lua_pop(L, 1);
    } else {
        addToTerminal(("Lua script executed: " + String(filename)).c_str(), ILI9341_GREEN);
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
    addToTerminal("Basic Terminal Engine v0.10", ILI9341_YELLOW);
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

    initLua();
    printPrompt();
}

void loop() {
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

    if (command.startsWith("run ")) {
        String script = command.substring(4);
        script.trim();
        runLuaScript(script.c_str());
        return;
    }

    if (command == "help") {
        addToTerminal("Available commands:", ILI9341_WHITE);
        addToTerminal("  help             - this help", ILI9341_WHITE);
        addToTerminal("  upload <file>    - upload file over USB (end with EOF)", ILI9341_WHITE);
        addToTerminal("  run <script>     - run Lua script from SD", ILI9341_WHITE);
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
        addToTerminal("Teensy 4.1 Basic Terminal Engine v0.10", ILI9341_YELLOW);
        addToTerminal("320x240 ILI9341 TFT + Lua 5.3", ILI9341_WHITE);
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