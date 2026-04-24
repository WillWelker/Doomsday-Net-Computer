// config.h - Declarations only

#pragma once

#include <Arduino.h>

// Hardware pins (defined in sd_manager.cpp)
extern const int ONBOARD_LED;
extern const int EXTERNAL_LED;

// Command buffer
constexpr int MAX_CMD = 128;

// Lua script path
extern const char* DEFAULT_LUA_PATH;

