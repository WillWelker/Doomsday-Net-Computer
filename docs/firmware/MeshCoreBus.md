# MeshCoreBus Library

USB Host / UART library for MeshCore companion devices.

## Protocol

Based on the official `meshcore` Python package (meshcore-cli).

**Frame Format:**
TX: 0x3C [len_lo] [len_hi] [payload...]
RX: 0x3E [len_lo] [len_hi] [payload...]


**Key Commands:**
| Hex | Name | Direction |
|-----|------|-----------|
| 0x01 | APP_START | TX (handshake) |
| 0x03 | SEND_CHANNEL_TXT_MSG | TX |
| 0x0A | GET_MESSAGE | TX (poll) |
| 0x08 | CHANNEL_MSG_RECV | RX |
| 0x83 | MESSAGES_WAITING | RX (push notification) |

## Usage

### USB Host Mode (Teensy 4.1)

```cpp
#include <USBHost_t36.h>
#include "MeshCoreBus.h"

USBHost myusb;
USBHub hub1(myusb);
USBSerial meshSerial(myusb);
MeshCoreBus mesh(meshSerial);

void onMessage(uint8_t chan, const char* from, const char* msg) {
    Serial.print("Ch"); Serial.print(chan);
    Serial.print(" "); Serial.print(from);
    Serial.print(": "); Serial.println(msg);
}

void setup() {
    myusb.begin();
    mesh.begin();
    mesh.onMessage(onMessage);
    mesh.setDebugPrint(&Serial);
}

void loop() {
    myusb.Task();
    mesh.update();
    
    // Send a message
    if (millis() % 10000 < 100) {
        mesh.sendChannel(0, "Hello from Teensy!");
    }
}

## UART Mode (Other Boards)
#include "MeshCoreBus.h"

MeshCoreBus mesh(Serial1, 115200);  // RX=0, TX=1 on most boards

void setup() {
    mesh.begin();
    mesh.onMessage(onMessage);
}

void loop() {
    mesh.update();
}

## License
GPL-3.0 - Part of Doomsday Net Computer project
---

### `keywords.txt`
MeshCoreBus	KEYWORD1
sendChannel	KEYWORD2
onMessage	KEYWORD2
pollMessages	KEYWORD2
isConnected	KEYWORD2

---

## Install Instructions

1. **Copy the `MeshCoreBus` folder** to:

firmware/teensy41/lib/MeshCoreBus/ (or your project lib directory)
2. **Add to your `platformio.ini`:**
```ini
lib_deps =
    USBHost_t36

    Include in your code:

3.   #include "MeshCoreBus.h"

