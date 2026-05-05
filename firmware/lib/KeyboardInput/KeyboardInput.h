#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <Arduino.h>
#include <ILI9341_t3.h>
#include <USBHost_t36.h>

class KeyboardInput {
public:
    KeyboardInput(ILI9341_t3& tft, int inputLineY);
    
    void begin();
    void update();
    void setCommandCallback(void (*callback)(const char* cmd));
    void setOutputCallback(void (*callback)(const char* line, uint16_t color));
    void clearInput();
    void printPrompt();
    
    // USB objects
    USBHost myusb;
    USBHub hub1;
    USBHIDParser hid1;
    KeyboardController keyboard1;

private:
    ILI9341_t3& _tft;
    int _inputY;
    static const int MAX_CMD = 128;
    char _cmdBuffer[MAX_CMD];
    int _cmdIndex;
    
    void (*_commandCallback)(const char* cmd);
    void (*_outputCallback)(const char* line, uint16_t color);
    
    void updateInputLine();
    void clearInputLine();
    void onKeyPress(int key);
    
    static KeyboardInput* _instance;
    static void keyPressHandler(int key);
};

#endif