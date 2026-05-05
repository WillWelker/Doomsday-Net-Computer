#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <Arduino.h>

class DisplayDriver {
public:
    virtual void begin() = 0;
    virtual void clear(uint16_t color) = 0;
    virtual void drawText(int x, int y, const char* text, uint16_t color, int size = 1) = 0;
    virtual void fillRect(int x, int y, int w, int h, uint16_t color) = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual ~DisplayDriver() {}
};

#endif