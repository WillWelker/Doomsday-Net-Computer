// ============================================================
// Teensy 4.1 - USB Host Keyboard Test (Current API)
// ============================================================

#include <USBHost_t36.h>

USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
KeyboardController keyboard1(myusb);

void OnPress(int key) {
    Serial.print("KEY PRESSED: ");
    Serial.print((char)key);
    Serial.print("  (code: ");
    Serial.print(key);
    Serial.println(")");
}

void setup() {
    Serial.begin(115200);
    delay(1500);
    
    Serial.println("\n========================================");
    Serial.println("  Teensy 4.1 - USB Host Keyboard Test");
    Serial.println("========================================");
    Serial.println("D+ = Pin 3, D- = Pin 2");
    Serial.println("");
    
    keyboard1.attachPress(OnPress);  // Attach callback function
    myusb.begin();
}

void loop() {
    myusb.Task();  // Process USB events
}