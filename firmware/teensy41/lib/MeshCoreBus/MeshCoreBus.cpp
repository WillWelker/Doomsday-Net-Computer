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
}/*
  MeshCoreBus.cpp - Implementation
*/

#include "MeshCoreBus.h"
#include <USBHost_t36.h>

MeshCoreBus::MeshCoreBus(USBSerial& serial)
    : _usbSerial(&serial), _hwSerial(nullptr), _isUSB(true),
      _connected(false), _lastActivity(0), _debug(nullptr),
      _msgCallback(nullptr), _rxState(WAIT_START), _rxLen(0), _rxIndex(0) {}

MeshCoreBus::MeshCoreBus(HardwareSerial& serial, uint32_t baud)
    : _usbSerial(nullptr), _hwSerial(&serial), _isUSB(false),
      _connected(false), _lastActivity(0), _debug(nullptr),
      _msgCallback(nullptr), _rxState(WAIT_START), _rxLen(0), _rxIndex(0) {
    _hwSerial->begin(baud);
}

void MeshCoreBus::begin() {
    sendAppStart();
    _connected = true;
    _lastActivity = millis();
    if (_debug) _debug->println("[MeshCoreBus] Initialized");
}

void MeshCoreBus::update() {
    if (_isUSB && _usbSerial) {
        while (_usbSerial->available()) {
            handleRxByte(_usbSerial->read());
        }
    } else if (_hwSerial) {
        while (_hwSerial->available()) {
            handleRxByte(_hwSerial->read());
        }
    }
}

void MeshCoreBus::sendChannel(uint8_t chan, const char* msg) {
    uint32_t ts = millis() / 1000;
    uint16_t len = strlen(msg);
    uint8_t pkt[8 + len];
    
    pkt[0] = CMD_SEND_CHAN;
    pkt[1] = 0x00;           // Reserved
    pkt[2] = chan;           // Channel index
    pkt[3] = ts & 0xFF;      // Timestamp (little-endian)
    pkt[4] = (ts >> 8) & 0xFF;
    pkt[5] = (ts >> 16) & 0xFF;
    pkt[6] = (ts >> 24) & 0xFF;
    memcpy(pkt + 7, msg, len);
    
    writeFrame(pkt, 7 + len);
    _lastActivity = millis();
    
    if (_debug) {
        _debug->print("[MeshCoreBus] Sent to ch");
        _debug->print(chan);
        _debug->print(": ");
        _debug->println(msg);
    }
}

void MeshCoreBus::sendChannel(uint8_t chan, const String& msg) {
    sendChannel(chan, msg.c_str());
}

void MeshCoreBus::onMessage(void (*callback)(uint8_t, const char*, const char*)) {
    _msgCallback = callback;
}

void MeshCoreBus::pollMessages() {
    requestNextMessage();
}

void MeshCoreBus::writeFrame(const uint8_t* data, uint16_t len) {
    uint8_t hdr[3] = {FRAME_TX, (uint8_t)(len & 0xFF), (uint8_t)(len >> 8)};
    
    if (_isUSB && _usbSerial) {
        _usbSerial->write(hdr, 3);
        _usbSerial->write(data, len);
    } else if (_hwSerial) {
        _hwSerial->write(hdr, 3);
        _hwSerial->write(data, len);
    }
}

void MeshCoreBus::sendAppStart() {
    // Exact format from meshcore Python package device.py
    uint8_t pkt[17] = {
        CMD_APP_START, 0x03,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,  // 7 reserved bytes
        'm', 'c', 'c', 'l', 'i'                     // App name
    };
    writeFrame(pkt, 17);
}

void MeshCoreBus::requestNextMessage() {
    uint8_t pkt = CMD_GET_MSG;
    writeFrame(&pkt, 1);
}

void MeshCoreBus::parseFrame(const uint8_t* data, uint16_t len) {
    if (len < 1) return;
    uint8_t type = data[0];
    
    if (type == PKT_CHAN_MSG && len >= 6) {
        uint8_t chan = data[1];
        // Timestamp at bytes 2-5 (little-endian), message follows
        const char* msg = (const char*)(data + 6);
        
        if (_msgCallback) {
            _msgCallback(chan, "NONAME", msg);  // TODO: Parse actual sender from message
        }
        
        if (_debug) {
            _debug->print("[MeshCoreBus] RX ch");
            _debug->print(chan);
            _debug->print(": ");
            _debug->println(msg);
        }
    }
    else if (type == PKT_MSG_WAITING) {
        // Device has messages - auto-fetch
        requestNextMessage();
        if (_debug) _debug->println("[MeshCoreBus] Auto-fetching messages...");
    }
}

void MeshCoreBus::handleRxByte(uint8_t b) {
    switch (_rxState) {
        case WAIT_START:
            if (b == FRAME_RX) {
                _rxState = WAIT_LEN_LO;
                _rxIndex = 0;
            }
            break;
            
        case WAIT_LEN_LO:
            _rxLen = b;
            _rxState = WAIT_LEN_HI;
            break;
            
        case WAIT_LEN_HI:
            _rxLen |= (uint16_t)b << 8;
            if (_rxLen > 511) {
                _rxState = WAIT_START;  // Invalid frame
            } else {
                _rxState = WAIT_DATA;
            }
            break;
            
        case WAIT_DATA:
            _rxBuf[_rxIndex++] = b;
            if (_rxIndex >= _rxLen) {
                parseFrame(_rxBuf, _rxLen);
                _rxState = WAIT_START;
            }
            break;
    }
}

void MeshCoreBus::debugPrint(const char* msg) {
    if (_debug) _debug->println(msg);
}

void MeshCoreBus::debugPrintHex(const uint8_t* data, uint16_t len) {
    if (!_debug) return;
    for (uint16_t i = 0; i < len; i++) {
        if (data[i] < 16) _debug->print("0");
        _debug->print(data[i], HEX);
        _debug->print(" ");
    }
    _debug->println();
}