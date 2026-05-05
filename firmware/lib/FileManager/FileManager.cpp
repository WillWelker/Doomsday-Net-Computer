#include "FileManager.h"

FileManager::FileManager() : _currentPath("/"), _initialized(false), _outputCallback(nullptr) {}

bool FileManager::begin(int csPin) {
    if (!SD.begin(csPin)) return false;
    _initialized = true;
    return true;
}

void FileManager::setOutputCallback(void (*callback)(const char* line, uint16_t color)) {
    _outputCallback = callback;
}

void FileManager::printLine(const char* line, uint16_t color) {
    if (_outputCallback) {
        _outputCallback(line, color);
    } else {
        Serial.println(line);
    }
}

String FileManager::resolvePath(const char* path) {
    if (!path || strlen(path) == 0) return _currentPath;
    if (path[0] == '/') return String(path);
    return _currentPath + "/" + path;
}

bool FileManager::listDir(const char* path) {
    String targetPath = path ? resolvePath(path) : _currentPath;
    File dir = SD.open(targetPath.c_str());
    if (!dir || !dir.isDirectory()) return false;
    
    printLine(("Contents of " + targetPath + ":").c_str(), 0xFFFF);
    
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;
        
        String line = entry.isDirectory() ? "[DIR]  " : "[FILE] ";
        line += entry.name();
        if (!entry.isDirectory()) {
            line += " (" + String(entry.size()) + " bytes)";
        }
        printLine(line.c_str(), entry.isDirectory() ? 0x07E0 : 0xFFFF);
        entry.close();
    }
    dir.close();
    return true;
}

bool FileManager::changeDir(const char* path) {
    String newPath = resolvePath(path);
    File dir = SD.open(newPath.c_str());
    if (!dir || !dir.isDirectory()) return false;
    dir.close();
    _currentPath = newPath;
    return true;
}

String FileManager::getCurrentPath() { return _currentPath; }

bool FileManager::catFile(const char* filename) {
    String fullPath = resolvePath(filename);
    File file = SD.open(fullPath.c_str());
    if (!file) return false;
    while (file.available()) {
        printLine(file.readStringUntil('\n').c_str(), 0x07E0);
    }
    file.close();
    return true;
}

bool FileManager::makeDir(const char* dirname) {
    return SD.mkdir(resolvePath(dirname).c_str());
}

bool FileManager::removeFile(const char* filename) {
    return SD.remove(resolvePath(filename).c_str());
}

bool FileManager::writeFile(const char* filename, const char* content) {
    File file = SD.open(resolvePath(filename).c_str(), FILE_WRITE);
    if (!file) return false;
    file.print(content);
    file.close();
    return true;
}

bool FileManager::appendFile(const char* filename, const char* content) {
    File file = SD.open(resolvePath(filename).c_str(), FILE_WRITE);
    if (!file) return false;
    file.seek(file.size());
    file.print(content);
    file.close();
    return true;
}

bool FileManager::fileExists(const char* filename) {
    return SD.exists(resolvePath(filename).c_str());
}