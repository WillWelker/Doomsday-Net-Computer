// ============================================================
// RP2350 - HDMI Text Output Test
// ============================================================

#include <Arduino.h>
#include <SparkFun_PicoDVI.h>

// DVI output - 640x480 @ 60Hz
PicoDVI display(640, 480, 60);

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {}
    
    Serial.println("\n========================================");
    Serial.println("  RP2350 - HDMI Text Output Test");
    Serial.println("========================================");
    
    if (!display.begin()) {
        Serial.println("ERROR: DVI/HDMI init failed!");
        while (1) delay(1000);
    }
    
    Serial.println("HDMI/DVI initialized successfully!");
    
    display.setTextColor(0xFFFF);
    display.setTextSize(2);
    display.fillScreen(0x0000);
    
    display.setCursor(40, 40);
    display.println("DOOMSDAY NET COMPUTER");
    
    display.setCursor(40, 90);
    display.println("RP2350 + HDMI Test");
    
    display.setCursor(40, 140);
    display.println("Text output working!");
    
    Serial.println("Text should be visible on HDMI monitor.");
}

void loop() {
    delay(1000);
}