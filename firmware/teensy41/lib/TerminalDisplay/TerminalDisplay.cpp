#include "TerminalDisplay.h"
#include <string.h>

TerminalDisplay::TerminalDisplay(DisplayDriver& driver, int maxLines, int lineHeight)
    : _driver(driver), _maxLines(maxLines), _lineHeight(lineHeight), 
      _currentLine(0), _lines(new Line[maxLines]) {
    
    for (int i = 0; i < maxLines; i++) {
        _lines[i].text[0] = '\0';
        _lines[i].color = 0x07E0;  // Green
    }
}

void TerminalDisplay::begin() {
    _driver.begin();
    clear();
}

void TerminalDisplay::addLine(const char* text, uint16_t color) {
    strncpy(_lines[_currentLine].text, text, 63);
    _lines[_currentLine].text[63] = '\0';
    _lines[_currentLine].color = color;
    
    _currentLine = (_currentLine + 1) % _maxLines;
    redraw();
}

void TerminalDisplay::clear() {
    _driver.clear(0x0000);  // Black
    for (int i = 0; i < _maxLines; i++) {
        _lines[i].text[0] = '\0';
    }
    _currentLine = 0;
}

void TerminalDisplay::redraw() {
    _driver.clear(0x0000);
    
    for (int i = 0; i < _maxLines; i++) {
        int idx = (_currentLine + i) % _maxLines;
        if (_lines[idx].text[0] != '\0') {
            _driver.drawText(4, 8 + i * _lineHeight, _lines[idx].text, _lines[idx].color);
        }
    }
}

void TerminalDisplay::drawLine(int index) {
    int y = 8 + index * _lineHeight;
    _driver.fillRect(4, y, _driver.getWidth() - 8, _lineHeight, 0x0000);
    if (_lines[index].text[0] != '\0') {
        _driver.drawText(4, y, _lines[index].text, _lines[index].color);
    }
}