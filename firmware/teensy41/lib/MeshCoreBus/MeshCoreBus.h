#ifndef MESHCOREBUS_H
#define MESHCOREBUS_H

#include <Arduino.h>

class MeshCoreBus {
public:
    MeshCoreBus(HardwareSerial& serial, uint32_t baud = 921600);
    
    void begin();
    void update();
    
    // Send a message to the mesh
    void send(const char* topic, const char* message);
    
    // Callback for received messages
    void onMessage(void (*callback)(const char* from, const char* message));
    
    // Send raw frame (for advanced use)
    void sendRaw(const uint8_t* data, uint16_t len);

private:
    HardwareSerial& _serial;
    uint32_t _baud;
    
    uint8_t _rxBuffer[256];
    uint16_t _rxIndex;
    uint16_t _expectedLen;
    bool _inFrame;
    
    void (*_messageCallback)(const char* from, const char* message);
    
    void parseFrame(const uint8_t* data, uint16_t len);
    void processPayload(const uint8_t* payload, uint16_t len);
};

#endif