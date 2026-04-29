#ifndef DOOMSDAY_MESSAGE_BUS_H
#define DOOMSDAY_MESSAGE_BUS_H

#include <Arduino.h>

class DoomsdayMessageBus {
public:
    DoomsdayMessageBus(HardwareSerial& serial, uint32_t baud = 921600);
    void begin();
    void update();
    void publish(const char* topic, const char* message);
    void subscribe(const char* topic, void (*callback)(const char* topic, const char* message));
    void sendRaw(const char* cmd);

private:
    HardwareSerial& _serial;
    uint32_t _baud;
    char _buffer[256];
    int _bufIndex;
    
    static const int MAX_SUBSCRIPTIONS = 10;
    struct Subscription {
        char topic[32];
        void (*callback)(const char* topic, const char* message);
    };
    Subscription _subscriptions[MAX_SUBSCRIPTIONS];
    int _subCount;
    
    void parseMessage(const char* msg);
};

#endif
