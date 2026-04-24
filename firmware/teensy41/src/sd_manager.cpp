#include <Arduino.h>
#include <SD.h>
#include "config.h"
#include "sd_manager.h"

// Definitions
const int ONBOARD_LED    = 13;
const int EXTERNAL_LED   = 12;
const char* DEFAULT_LUA_PATH = "/boot/autorun.lua";

bool initSDCard() {
    const int SD_CS_PIN = BUILTIN_SDCARD;   // Local definition
    return SD.begin(SD_CS_PIN);
}

bool ensureDefaultLuaScript() {
    if (!SD.exists(DEFAULT_LUA_PATH)) {
        File f = SD.open(DEFAULT_LUA_PATH, FILE_WRITE);
        if (f) {
            f.println("-- Doomsday Net Computer - autorun.lua");
            f.println("-- Created on first boot. Edit freely.");
            f.println("");
            f.println("print('Lua engine started')");
            f.println("print('Try: run blink')");
            f.close();
            return true;
        }
        return false;
    }
    return true;
}
