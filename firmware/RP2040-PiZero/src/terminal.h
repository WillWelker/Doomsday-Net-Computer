#ifndef TERMINAL_H
#define TERMINAL_H

#include <Arduino.h>
#include <ILI9341_t3.h>
#include "config.h"

extern ILI9341_t3 tft;
extern String currentPath;
extern char cmdBuffer[MAX_CMD];
extern int cmdIndex;

struct TerminalLine {
    String text;
    uint16_t color;
};

extern TerminalLine terminalLines[MAX_LINES];
extern int scrollOffset;

// ==================== FUNCTION IMPLEMENTATIONS ====================

void blinkLED(int pin, int times = 1, int onTime = 120, int offTime = 120) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}

void scrollUp() {
    for (int i = 0; i < MAX_LINES - 1; i++) {
        terminalLines[i] = terminalLines[i + 1];
    }
    terminalLines[MAX_LINES - 1].text = "";
    terminalLines[MAX_LINES - 1].color = ILI9341_GREEN;
    
    tft.fillScreen(ILI9341_BLACK);
    for (int i = 0; i < MAX_LINES - 1; i++) {
        if (terminalLines[i].text.length() > 0) {
            tft.setCursor(4, 8 + i * LINE_HEIGHT);
            tft.setTextColor(terminalLines[i].color);
            tft.setTextSize(1);
            tft.print(terminalLines[i].text);
        }
    }
}

void addToTerminal(const char* text, uint16_t color = ILI9341_GREEN) {
    if (scrollOffset >= MAX_LINES - 1) {
        scrollUp();
        scrollOffset = MAX_LINES - 2;
    }
    
    terminalLines[scrollOffset].text = text;
    terminalLines[scrollOffset].color = color;
    
    tft.setCursor(4, 8 + scrollOffset * LINE_HEIGHT);
    tft.setTextColor(color);
    tft.setTextSize(1);
    tft.print(text);
    
    scrollOffset++;
    Serial.println(text);
}

void printPrompt() {
    Serial.print("\n> ");
    addToTerminal("> ", ILI9341_YELLOW);
}

#endif