#include "KeyboardInput.h"

KeyboardInput* KeyboardInput::_instance = nullptr;

KeyboardInput::KeyboardInput(ILI9341_t3& tft, int inputLineY)
    : _tft(tft), _inputY(inputLineY), _cmdIndex(0), 
      hub1(myusb), hid1(myusb), keyboard1(myusb), 
      _commandCallback(nullptr), _outputCallback(nullptr) {
    _instance = this;
    _cmdBuffer[0] = '\0';
}

void KeyboardInput::begin() {
    keyboard1.attachPress(keyPressHandler);
    myusb.begin();
}

void KeyboardInput::update() {
    myusb.Task();
}

void KeyboardInput::setCommandCallback(void (*callback)(const char* cmd)) {
    _commandCallback = callback;
}

void KeyboardInput::setOutputCallback(void (*callback)(const char* line, uint16_t color)) {
    _outputCallback = callback;
}

void KeyboardInput::clearInput() {
    clearInputLine();
}

void KeyboardInput::printPrompt() {
    _tft.setTextColor(ILI9341_YELLOW);
    _tft.setCursor(4, _inputY);
    _tft.print("> ");
}

void KeyboardInput::updateInputLine() {
    _tft.fillRect(20, _inputY, 296, 16, ILI9341_BLACK);
    _tft.setTextColor(ILI9341_YELLOW);
    _tft.setCursor(20, _inputY);
    _tft.print(_cmdBuffer);
}

void KeyboardInput::clearInputLine() {
    _tft.fillRect(4, _inputY, 312, 16, ILI9341_BLACK);
    _cmdIndex = 0;
    _cmdBuffer[0] = '\0';
}

void KeyboardInput::onKeyPress(int key) {
    if (key == 10 || key == 13) {
        // Enter pressed
        _cmdBuffer[_cmdIndex] = '\0';
        if (_cmdIndex > 0) {
            // ECHO command to terminal (yellow)
            if (_outputCallback) {
                _outputCallback(_cmdBuffer, 0xFFE0);
            }
            // Execute command
            if (_commandCallback) {
                _commandCallback(_cmdBuffer);
            }
        }
        clearInputLine();
        printPrompt();
    }
    else if (key == 8 || key == 127) {
        // Backspace
        if (_cmdIndex > 0) {
            _cmdIndex--;
            _cmdBuffer[_cmdIndex] = '\0';
            updateInputLine();
        }
    }
    else if (_cmdIndex < MAX_CMD - 1) {
        // Regular character
        _cmdBuffer[_cmdIndex++] = key;
        _cmdBuffer[_cmdIndex] = '\0';
        updateInputLine();
    }
}

void KeyboardInput::keyPressHandler(int key) {
    if (_instance) {
        _instance->onKeyPress(key);
    }
}