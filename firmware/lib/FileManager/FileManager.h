#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <Arduino.h>
#include <SD.h>

class FileManager {
public:
    FileManager();
    
    bool begin(int csPin = BUILTIN_SDCARD);
    void setOutputCallback(void (*callback)(const char* line, uint16_t color));
    
    bool listDir(const char* path = nullptr);
    bool changeDir(const char* path);
    String getCurrentPath();
    
    bool catFile(const char* filename);
    bool makeDir(const char* dirname);
    bool removeFile(const char* filename);
    bool writeFile(const char* filename, const char* content);
    bool appendFile(const char* filename, const char* content);
    bool fileExists(const char* filename);

private:
    String _currentPath;
    bool _initialized;
    void (*_outputCallback)(const char* line, uint16_t color);
    
    String resolvePath(const char* path);
    void printLine(const char* line, uint16_t color = 0x07E0);
};

#endif