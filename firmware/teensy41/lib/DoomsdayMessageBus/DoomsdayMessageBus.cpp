#include "DoomsdayMessageBus.h"

DoomsdayMessageBus::DoomsdayMessageBus(HardwareSerial& serial, uint32_t baud)
    : _serial(serial), _baud(baud), _bufIndex(0), _subCount(0) {}

void DoomsdayMessageBus::begin() {
    _serial.begin(_baud);
}

void DoomsdayMessageBus::update() {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            _buffer[_bufIndex] = '\0';
            parseMessage(_buffer);
            _bufIndex = 0;
        } else if (_bufIndex < 255) {
            _buffer[_bufIndex++] = c;
        }
    }
}

void DoomsdayMessageBus::publish(const char* topic, const char* message) {
    _serial.print("PUB:");
    _serial.print(topic);
    _serial.print(":");
    _serial.println(message);
}

void DoomsdayMessageBus::subscribe(const char* topic, void (*callback)(const char* topic, const char* message)) {
    if (_subCount < MAX_SUBSCRIPTIONS) {
        strncpy(_subscriptions[_subCount].topic, topic, 31);
        _subscriptions[_subCount].callback = callback;
        _subCount++;
    }
}

void DoomsdayMessageBus::sendRaw(const char* cmd) {
    _serial.println(cmd);
}

void DoomsdayMessageBus::parseMessage(const char* msg) {
    // TODO: Full PUB:topic:message parsing
    // For now, just print received messages
    Serial.print("Doomsday Bus RX: ");
    Serial.println(msg);
}
