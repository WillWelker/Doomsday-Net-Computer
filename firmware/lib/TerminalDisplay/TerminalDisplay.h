#ifndef TERMINAL_DISPLAY_H
#define TERMINAL_DISPLAY_H

#include "DisplayDriver.h"

class TerminalDisplay {
public:
    TerminalDisplay(DisplayDriver& driver, int maxLines, int lineHeight);
    
    void begin();
    void addLine(const char* text, uint16_t color = 0x07E0);  // Green default
    void clear();
    void redraw();

private:
    DisplayDriver& _driver;
    int _maxLines;
    int _lineHeight;
    int _currentLine;
    
    struct Line {
        char text[64];
        uint16_t color;
    };
    
    Line* _lines;
    
    void drawLine(int index);
};

#endif