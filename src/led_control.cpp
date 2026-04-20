#include <Arduino.h>
#include "led_control.h"

void blinkLED(int pin, int times, int onTime, int offTime) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(onTime);
        digitalWrite(pin, LOW);
        delay(offTime);
    }
}
