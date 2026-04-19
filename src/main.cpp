#include <Arduino.h>
#include <USBHost_t36.h>

USBHost myusb;
KeyboardController keyboard1(myusb);

void OnKeyPress(int key) {
    digitalWrite(13, HIGH);   // onboard LED flash
    delay(30);
    digitalWrite(13, LOW);

    Serial.print("Key: ");
    if (key >= 32 && key <= 126) Serial.print((char)key);
    else Serial.printf("[0x%02X]", key);
    Serial.println();
}

void setup() {
    pinMode(13, OUTPUT);
    Serial.begin(115200);
    while (!Serial && millis() < 8000);
    delay(200);

    Serial.println("\n=== Doomsday Net Computer - Minimal USB Host Keyboard Test ===");
    myusb.begin();
    keyboard1.attachPress(OnKeyPress);
    Serial.println("USB Host started - plug keyboard into soldered port");
}

void loop() {
    myusb.Task();   // MUST run frequently
}