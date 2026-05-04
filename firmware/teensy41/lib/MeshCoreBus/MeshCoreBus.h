/*
  MeshCoreBus.h - MeshCore Companion Protocol Library
  For Teensy 4.1 USB Host or UART connection
*/

#ifndef MESHCOREBUS_H
#define MESHCOREBUS_H

#include <Arduino.h>

class USBSerial;
class HardwareSerial;

class MeshCoreBus {
public:
    MeshCoreBus(USBSerial& serial);
    MeshCoreBus(HardwareSerial& serial, uint32_t baud = 115200);
    
    void begin();
    void update();
    
    void sendChannel(uint8_t chan, const char* msg);
    void sendChannel(uint8_t chan, const String& msg);
    
    void onMessage(void (*callback)(uint8_t chan, const char* from, const char* msg));
    void pollMessages();
    
    bool isConnected() const { return _connected; }
    uint32_t getLastActivity() const { return _lastActivity; }
    
    void setDebugPrint(Print* printer) { _debug = printer; }
    
private:
    USBSerial* _usbSerial;
    HardwareSerial* _hwSerial;
    bool _isUSB;
    
    bool _connected;
    uint32_t _lastActivity;
    Print* _debug;
    
    void (*_msgCallback)(uint8_t, const char*, const char*);
    
    enum RxState { WAIT_START, WAIT_LEN_LO, WAIT_LEN_HI, WAIT_DATA };
    RxState _rxState;
    uint16_t _rxLen;
    uint16_t _rxIndex;
    uint8_t _rxBuf[512];
    
    static const uint8_t FRAME_TX = 0x3C;
    static const uint8_t FRAME_RX = 0x3E;
    static const uint8_t CMD_APP_START = 0x01;
    static const uint8_t CMD_SEND_CHAN = 0x03;
    static const uint8_t CMD_GET_MSG = 0x0A;
    static const uint8_t PKT_CHAN_MSG = 0x08;
    static const uint8_t PKT_MSG_WAITING = 0x83;
    
    void writeFrame(const uint8_t* data, uint16_t len);
    void sendAppStart();
    void requestNextMessage();
    void parseFrame(const uint8_t* data, uint16_t len);
    void handleRxByte(uint8_t b);
    void debugPrint(const char* msg);
};

#endif