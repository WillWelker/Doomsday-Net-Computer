/*
  MeshCoreBus.h - MeshCore Companion Protocol Library
  For Teensy 4.1 with USB Host or UART connection to MeshCore devices
  
  Protocol based on meshcore Python package (meshcore-cli)
  Frame format: 0x3C [len_lo] [len_hi] [payload] for TX
                0x3E [len_lo] [len_hi] [payload] for RX
*/

#ifndef MESHCOREBUS_H
#define MESHCOREBUS_H

#include <Arduino.h>

// Forward declarations for optional USB Host support
class USBSerial;
class HardwareSerial;

class MeshCoreBus {
public:
    // Constructor for USB Host mode (Teensy 4.1)
    MeshCoreBus(USBSerial& serial);
    
    // Constructor for UART/Serial mode (other boards)
    MeshCoreBus(HardwareSerial& serial, uint32_t baud = 115200);
    
    void begin();
    void update();  // Call frequently in loop()
    
    // === SENDING ===
    void sendChannel(uint8_t chan, const char* msg);
    void sendChannel(uint8_t chan, const String& msg);
    
    // === RECEIVING ===
    // Set callback for incoming messages: void callback(uint8_t chan, const char* from, const char* msg)
    void onMessage(void (*callback)(uint8_t chan, const char* from, const char* msg));
    
    // Manual poll (usually not needed - auto-fetches on push notification)
    void pollMessages();
    
    // === STATUS ===
    bool isConnected() const { return _connected; }
    uint32_t getLastActivity() const { return _lastActivity; }
    
    // === DEBUG ===
    void setDebugPrint(Print* printer) { _debug = printer; }
    
private:
    // Connection (either USB or UART)
    USBSerial* _usbSerial;
    HardwareSerial* _hwSerial;
    bool _isUSB;
    
    bool _connected;
    uint32_t _lastActivity;
    Print* _debug;
    
    // Message callback
    void (*_msgCallback)(uint8_t, const char*, const char*);
    
    // RX state machine
    enum RxState { WAIT_START, WAIT_LEN_LO, WAIT_LEN_HI, WAIT_DATA };
    RxState _rxState;
    uint16_t _rxLen;
    uint16_t _rxIndex;
    uint8_t _rxBuf[512];
    
    // Protocol constants
    static const uint8_t FRAME_TX = 0x3C;
    static const uint8_t FRAME_RX = 0x3E;
    static const uint8_t CMD_APP_START = 0x01;
    static const uint8_t CMD_SEND_CHAN = 0x03;
    static const uint8_t CMD_GET_MSG = 0x0A;
    static const uint8_t PKT_CHAN_MSG = 0x08;
    static const uint8_t PKT_MSG_WAITING = 0x83;
    
    // Internal methods
    void writeFrame(const uint8_t* data, uint16_t len);
    void sendAppStart();
    void requestNextMessage();
    void parseFrame(const uint8_t* data, uint16_t len);
    void handleRxByte(uint8_t b);
    void debugPrint(const char* msg);
    void debugPrintHex(const uint8_t* data, uint16_t len);
};

#endif