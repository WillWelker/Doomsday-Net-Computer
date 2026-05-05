#ifndef ILI9341_DRIVER_H
#define ILI9341_DRIVER_H

#include "DisplayDriver.h"
#include <ILI9341_t3.h>

class ILI9341Driver : public DisplayDriver {
public:
    ILI9341Driver(ILI9341_t3& tft);
    
    void begin() override;
    void clear(uint16_t color) override;
    void drawText(int x, int y, const char* text, uint16_t color, int size = 1) override;
    void fillRect(int x, int y, int w, int h, uint16_t color) override;
    int getWidth() override;
    int getHeight() override;

private:
    ILI9341_t3& _tft;
};

#endif