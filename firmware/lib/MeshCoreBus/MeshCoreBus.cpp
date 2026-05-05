/*
  MeshCoreBus.cpp - Implementation
  Updated for USB Hub + aggressive receive debugging
*/

#include "MeshCoreBus.h"

MeshCoreBus::MeshCoreBus(USBHost& usbHost, USBSerial& serial)
    : _usbHost(&usbHost), _usbSerial(&serial), _hwSerial(nullptr), _isUSB(true),
      _connected(false), _lastActivity(0), _debug(nullptr),
      _msgCallback(nullptr), _rxState(WAIT_START), _rxLen(0), _rxIndex(0) {}

MeshCoreBus::MeshCoreBus(HardwareSerial& serial, uint32_t baud)
    : _usbHost(nullptr), _usbSerial(nullptr), _hwSerial(&serial), _isUSB(false),
      _connected(false), _lastActivity(0), _debug(nullptr),
      _msgCallback(nullptr), _rxState(WAIT_START), _rxLen(0), _rxIndex(0) {
    _hwSerial->begin(baud);
}

void MeshCoreBus::begin() {
    Serial.println("[MeshCoreBus] === Initializing MeshCoreBus (USB Hub mode - NO AppStart) ===");
    
    // Temporarily disabled - testing without sendAppStart
    // sendAppStart();
    
    _connected = true;
    _lastActivity = millis();
    Serial.println("[MeshCoreBus] Initialized - Connected flag set to TRUE (no AppStart sent)");
}

void MeshCoreBus::update() {
    if (_isUSB && _usbSerial) {
        int available = _usbSerial->available();
        if (available > 0) {
            Serial.print("[MeshCoreBus] === RX available: ");
            Serial.print(available);
            Serial.println(" bytes ===");
        }
        while (_usbSerial->available()) {
            uint8_t b = _usbSerial->read();
            Serial.print("[MeshCoreBus] RAW RX BYTE: 0x");
            if (b < 0x10) Serial.print("0");
            Serial.print(b, HEX);
            Serial.print(" (");
            Serial.print(b);
            Serial.println(")");
            handleRxByte(b);
        }
    } else if (_hwSerial) {
        while (_hwSerial->available()) {
            handleRxByte(_hwSerial->read());
        }
    }
}

void MeshCoreBus::sendChannel(uint8_t chan, const char* msg) {
    Serial.print("[MeshCoreBus] >>> Sending on channel ");
    Serial.print(chan);
    Serial.print(": ");
    Serial.println(msg);

    uint32_t ts = millis() / 1000;
    uint16_t len = strlen(msg);
    uint8_t pkt[8 + len];
    pkt[0] = CMD_SEND_CHAN; pkt[1] = 0x00; pkt[2] = chan;
    pkt[3] = ts & 0xFF; pkt[4] = (ts >> 8) & 0xFF;
    pkt[5] = (ts >> 16) & 0xFF; pkt[6] = (ts >> 24) & 0xFF;
    memcpy(pkt + 7, msg, len);
    writeFrame(pkt, 7 + len);
    _lastActivity = millis();
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
    uint8_t fullPacket[3 + len];
    fullPacket[0] = FRAME_TX;
    fullPacket[1] = (uint8_t)(len & 0xFF);
    fullPacket[2] = (uint8_t)(len >> 8);
    memcpy(fullPacket + 3, data, len);

    if (_isUSB && _usbSerial) {
        Serial.print("[MeshCoreBus] >>> Writing ");
        Serial.print(3 + len);
        Serial.println(" bytes (single write + flush + delay)");

        _usbSerial->write(fullPacket, 3 + len);
        _usbSerial->flush();
        delay(2);
        Serial.println("[MeshCoreBus] >>> USB write complete + flushed");
    } 
    else if (_hwSerial) {
        _hwSerial->write(fullPacket, 3 + len);
    }
}

void MeshCoreBus::sendAppStart() {
    uint8_t pkt[17] = {CMD_APP_START, 0x03, 0x20,0x20,0x20,0x20,0x20,0x20,0x20, 'm','c','c','l','i'};
    writeFrame(pkt, 17);
    Serial.println("[MeshCoreBus] AppStart packet sent");
}

void MeshCoreBus::requestNextMessage() {
    uint8_t pkt = CMD_GET_MSG;
    writeFrame(&pkt, 1);
    Serial.println("[MeshCoreBus] Requested next message");
}

void MeshCoreBus::parseFrame(const uint8_t* data, uint16_t len) {
    Serial.print("[MeshCoreBus] <<< PARSED FRAME type=0x");
    Serial.print(data[0], HEX);
    Serial.print(" len=");
    Serial.println(len);

    if (len < 1) return;
    uint8_t type = data[0];
    
    if (type == PKT_CHAN_MSG && len >= 6) {
        uint8_t chan = data[1];
        const char* msg = (const char*)(data + 6);
        Serial.print("[MeshCoreBus] <<< RECEIVED on ch");
        Serial.print(chan);
        Serial.print(": ");
        Serial.println(msg);
        
        if (_msgCallback) _msgCallback(chan, "MeshCore", msg);
    }
    else if (type == PKT_MSG_WAITING) {
        Serial.println("[MeshCoreBus] <<< Message waiting - auto fetching...");
        requestNextMessage();
    }
    else {
        Serial.println("[MeshCoreBus] <<< Unknown frame type received");
    }
}

void MeshCoreBus::handleRxByte(uint8_t b) {
    Serial.print("[MeshCoreBus] State=");
    Serial.print(_rxState);
    Serial.print(" byte=0x");
    Serial.println(b, HEX);

    switch (_rxState) {
        case WAIT_START: 
            if (b == FRAME_RX) { 
                _rxState = WAIT_LEN_LO; 
                _rxIndex = 0; 
                Serial.println("[MeshCoreBus] RX: Found FRAME_RX start");
            } 
            break;
        case WAIT_LEN_LO: 
            _rxLen = b; 
            _rxState = WAIT_LEN_HI; 
            break;
        case WAIT_LEN_HI: 
            _rxLen |= (uint16_t)b << 8; 
            _rxState = (_rxLen > 511) ? WAIT_START : WAIT_DATA; 
            Serial.print("[MeshCoreBus] RX: Frame length = ");
            Serial.println(_rxLen);
            break;
        case WAIT_DATA:
            _rxBuf[_rxIndex++] = b;
            if (_rxIndex >= _rxLen) { 
                Serial.println("[MeshCoreBus] RX: Full frame received, parsing...");
                parseFrame(_rxBuf, _rxLen); 
                _rxState = WAIT_START; 
            }
            break;
    }
}