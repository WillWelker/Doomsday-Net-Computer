#include "ILI9341Driver.h"

ILI9341Driver::ILI9341Driver(ILI9341_t3& tft) : _tft(tft) {}

void ILI9341Driver::begin() {
    _tft.begin();
}

void ILI9341Driver::clear(uint16_t color) {
    _tft.fillScreen(color);
}

void ILI9341Driver::drawText(int x, int y, const char* text, uint16_t color, int size) {
    _tft.setTextColor(color);
    _tft.setTextSize(size);
    _tft.setCursor(x, y);
    _tft.print(text);
}

void ILI9341Driver::fillRect(int x, int y, int w, int h, uint16_t color) {
    _tft.fillRect(x, y, w, h, color);
}

int ILI9341Driver::getWidth() { return _tft.width(); }
int ILI9341Driver::getHeight() { return _tft.height(); }