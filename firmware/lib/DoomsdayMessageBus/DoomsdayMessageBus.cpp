#include "DoomsdayMessageBus.h"

DoomsdayMessageBus::DoomsdayMessageBus(HardwareSerial& serial, uint32_t baud)
    : _serial(serial), _baud(baud), _bufIndex(0), _subCount(0) {}

void DoomsdayMessageBus::begin() {
    _serial.begin(_baud);
    Serial.println("[DMB] Doomsday Message Bus initialized");
}

void DoomsdayMessageBus::update() {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            _buffer[_bufIndex] = '\0';
            parseMessage(_buffer);
            _bufIndex = 0;
        } else if (_bufIndex < 511) {
            _buffer[_bufIndex++] = c;
        }
    }
}

void DoomsdayMessageBus::publish(const char* topic, const char* message) {
    char escapedMsg[256];
    escapeString(message, escapedMsg, sizeof(escapedMsg));

    _serial.print("PUB:");
    _serial.print(topic);
    _serial.print(":");
    _serial.println(escapedMsg);

    Serial.print("[DMB] Published -> ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(message);
}

void DoomsdayMessageBus::subscribe(const char* topic, void (*callback)(const char* topic, const char* message)) {
    if (_subCount < MAX_SUBSCRIPTIONS) {
        strncpy(_subscriptions[_subCount].topic, topic, 31);
        _subscriptions[_subCount].callback = callback;
        _subCount++;
        Serial.print("[DMB] Subscribed to topic: ");
        Serial.println(topic);
    }
}

void DoomsdayMessageBus::sendRaw(const char* cmd) {
    _serial.println(cmd);
}

void DoomsdayMessageBus::parseMessage(const char* msg) {
    // Expected format: PUB:topic:message
    if (strncmp(msg, "PUB:", 4) != 0) {
        Serial.print("[DMB] Unknown message: ");
        Serial.println(msg);
        return;
    }

    const char* topicStart = msg + 4;
    const char* colon = strchr(topicStart, ':');
    if (!colon) return;

    char topic[32];
    int topicLen = colon - topicStart;
    if (topicLen > 31) topicLen = 31;
    strncpy(topic, topicStart, topicLen);
    topic[topicLen] = '\0';

    const char* messageStart = colon + 1;
    char unescaped[256];
    unescapeString(messageStart, unescaped, sizeof(unescaped));

    // Call matching subscriber
    for (int i = 0; i < _subCount; i++) {
        if (strcmp(_subscriptions[i].topic, topic) == 0) {
            if (_subscriptions[i].callback) {
                _subscriptions[i].callback(topic, unescaped);
            }
            return;
        }
    }

    // No subscriber found - just print for now
    Serial.print("[DMB] RX -> ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(unescaped);
}

void DoomsdayMessageBus::escapeString(const char* input, char* output, int maxLen) {
    int j = 0;
    for (int i = 0; input[i] != '\0' && j < maxLen - 1; i++) {
        if (input[i] == ':' || input[i] == '\n' || input[i] == '\\') {
            output[j++] = '\\';
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
}

void DoomsdayMessageBus::unescapeString(const char* input, char* output, int maxLen) {
    int j = 0;
    for (int i = 0; input[i] != '\0' && j < maxLen - 1; i++) {
        if (input[i] == '\\' && input[i+1] != '\0') {
            i++;
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
}