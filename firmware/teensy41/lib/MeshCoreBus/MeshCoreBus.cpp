#include "MeshCoreBus.h"

MeshCoreBus::MeshCoreBus(HardwareSerial& serial, uint32_t baud)
    : _serial(serial), _baud(baud), _rxIndex(0), _expectedLen(0), 
      _inFrame(false), _messageCallback(nullptr) {}

void MeshCoreBus::begin() {
    _serial.begin(_baud);
}

void MeshCoreBus::update() {
    while (_serial.available()) {
        uint8_t b = _serial.read();
        
        if (!_inFrame) {
            if (b == 0x3C) {  // Start of frame
                _inFrame = true;
                _rxIndex = 0;
            }
        } else {
            _rxBuffer[_rxIndex++] = b;
            
            if (_rxIndex == 2) {
                // Got length bytes
                _expectedLen = _rxBuffer[0] | (_rxBuffer[1] << 8);
            }
            
            if (_rxIndex >= 2 && _rxIndex == _expectedLen + 2) {
                // Complete frame received
                parseFrame(_rxBuffer + 2, _expectedLen);
                _inFrame = false;
                _rxIndex = 0;
            }
            
            if (_rxIndex >= 256) {
                // Buffer overflow, reset
                _inFrame = false;
                _rxIndex = 0;
            }
        }
    }
}

void MeshCoreBus::send(const char* topic, const char* message) {
    char payload[128];
    snprintf(payload, sizeof(payload), "%s:%s", topic, message);
    
    uint16_t len = strlen(payload);
    uint8_t frame[3 + len];
    
    frame[0] = 0x3C;           // Start byte
    frame[1] = len & 0xFF;     // Length low byte
    frame[2] = (len >> 8) & 0xFF;  // Length high byte
    
    memcpy(frame + 3, payload, len);
    
    _serial.write(frame, 3 + len);
}

void MeshCoreBus::onMessage(void (*callback)(const char* from, const char* message)) {
    _messageCallback = callback;
}

void MeshCoreBus::sendRaw(const uint8_t* data, uint16_t len) {
    uint8_t frame[3 + len];
    
    frame[0] = 0x3C;
    frame[1] = len & 0xFF;
    frame[2] = (len >> 8) & 0xFF;
    
    memcpy(frame + 3, data, len);
    
    _serial.write(frame, 3 + len);
}

void MeshCoreBus::parseFrame(const uint8_t* data, uint16_t len) {
    if (_messageCallback) {
        // Simple parsing: assume "topic:message" format
        char* colon = strchr((char*)data, ':');
        if (colon) {
            *colon = '\0';
            _messageCallback((char*)data, colon + 1);
        } else {
            _messageCallback("unknown", (char*)data);
        }
    }
}

void MeshCoreBus::processPayload(const uint8_t* payload, uint16_t len) {
    // Placeholder for more advanced payload processing
    // Currently handled in parseFrame()
}