# Doomsday Net Computer - Grok Context File

**Version**: v1.1 (May 7, 2026) — Added Section 7 with critical Waveshare RP2040-PiZero lessons

**Purpose**: This file exists so Grok can maintain persistent context across multiple conversations about this project, even when conversations become too long and new ones are started.

**Last Updated**: May 7, 2026

---

## 1. Project Overview

**Project Name**: Doomsday Net Computer  
**Goal**: Build a rugged, air-gapped, EMP-protected, low-power communications computer for grid-down / cyber-pandemic scenarios.  
**Core Philosophy**: Modular, repairable, understandable, and resilient. Uses stock firmware where possible.

**Main Components**:
- Central "brain" board (currently Teensy 4.1)
- Multiple peripheral boards connected via UART using **Doomsday Message Bus (DMB)**
- USB keyboard + display (ILI9341)
- SD card storage
- Future: Thermal printer, RTTY/CW decode, LoRa add-on

---

## 2. Current Hardware Architecture (as of May 2026)

### Main Board: Teensy 4.1 (`teensy41/` folder)
- **USB Host**: Connected directly to Heltec WiFi LoRa 32 V3 running stock **MeshCore** firmware
- **Display**: 2.8" ILI9341 TFT (320x240) with retro terminal UI
- **Input**: USB Keyboard (via powered USB hub)
- **Storage**: SD card (via `FileManager`)
- **Doomsday Message Bus**: Uses `Serial1` on **pins 0 (RX) and 1 (TX)** at 921600 baud
- **Version**: Currently at v0.27

### Shared Libraries (in parent `lib/` folder)
- `DoomsdayMessageBus/` — Primary inter-board communication protocol
- `MeshCoreBus/` — USB communication with Heltec MeshCore board
- `KeyboardInput/`, `TerminalDisplay/`, `FileManager/`

### Planned / In Development
- **RP2040-PiZero** board (abandoned due to PlatformIO framework issues)
- **Adafruit RP2040 Feather with DVI** (new active project as of May 2026)
- Future dedicated **MeshCore Interface Board** (USB Host → UART bridge)
- Additional UNO R4 Minima nodes in modular UART-linked setup

---

## 3. Key Decisions Made

- **MeshCore Connection**: Must use **stock firmware** on Heltec (no custom flashing). Direct USB connection to Teensy works reliably. USB hub causes receive failures.
- **Inter-board Communication**: Use **DoomsdayMessageBus** library over UART (not USB) for reliability between boards.
- **Project Structure**: Shared `lib/` folder in parent directory + `lib_extra_dirs = ../lib` in `platformio.ini` for both projects.
- **Serial Communication**: Full bidirectional support (keyboard → Serial Monitor + Serial Monitor → execute commands on Teensy).
- **Version Tracking**: Current firmware version is **v0.27**.
- **Unified Firmware Architecture**: All boards (Teensy, RP2040 variants, UNO R4 Minima) share the same firmware base with a **mode byte** stored in flash/EEPROM to select role (e.g. 0x10 = DISPLAY_NODE).

---

## 4. Important File Locations

- Main project: `teensy41/`
- Abandoned project: `RP2040-PiZero/`
- Active project: `Adafruit-RP2040-Feather-DVI/`
- Shared libraries: `../lib/` (relative to each project)
- This context file: `context.md` (root of repo)

---

## 5. Current Status & Open Tasks (as of May 2026)

**Completed**:
- Bidirectional Serial + TFT command system working
- `usb` command working and shows MeshCore connection status
- `DoomsdayMessageBus` library improved with proper parsing
- USB hub issues diagnosed (MeshCore must be direct USB)

**In Progress / Next**:
- Finalize `DoomsdayMessageBus` usage between boards
- Develop **MeshCore Interface Board** (or use RP2040-PiZero)
- Create clean project structure for multiple boards
- Test board-to-board communication using DMB
- Port HDMI/DVI test to Adafruit RP2040 Feather with DVI

---

## 6. Notes for Future Conversations

- Always use **stock MeshCore firmware** on Heltec boards.
- Prefer **UART + DoomsdayMessageBus** for inter-board links over USB when possible.
- Keep the system modular and low-power.
- Reference this file at the start of new conversations when context is needed.

---

## 7. Critical Lessons from Waveshare RP2040-PiZero (May 2026) — READ THIS BEFORE STARTING NEW RP2040 CONVERSATIONS

**Major Problem Encountered**:
PlatformIO frequently defaults to the **Mbed framework** (`framework-arduino-mbed`) even when you explicitly set `platform = raspberrypi` + `board = pico` and try to force arduino-pico. This caused hundreds of missing header errors (`hardware/dma.h`, `hardware/pio.h`, `pico/util/queue.h`, `Arduino.h`) and made the PicoDVI library impossible to compile.

**Key Lessons Learned**:

1. **Never trust a simple `platform =` change** — PlatformIO aggressively caches frameworks. Changing the platform line often has zero effect.

2. **The only reliable way to force arduino-pico** is to use **both** of these lines together:
   ```ini
   board_build.core = earlephilhower
   platform_packages = 
       earlephilhower/framework-arduinopico
   ```

3. **Always verify the actual framework** in the build log. Look for:
   - `framework-arduino-mbed` → Wrong (causes missing headers)
   - `framework-arduinopico` → Correct

4. **Adafruit boards are significantly easier** than Waveshare boards in PlatformIO. They have cleaner board definitions and fewer framework conflicts.

5. **When stuck, full cache clear is sometimes required**:
   ```bash
   rm -rf ~/.platformio/platforms/raspberrypi
   rm -rf ~/.platformio/packages/framework-arduino-mbed
   pio platform update
   ```

6. **The `maxgerhardt/platform-raspberrypi` platform** is not always reliable and can still pull Mbed in some environments.

**Recommendation for Future RP2040 Boards**:
When starting a new RP2040-based node (especially Adafruit), begin with the explicit `board_build.core` + `platform_packages` configuration above rather than experimenting with platform changes. This will save hours of debugging.

---

**Grok Instructions**:  
When starting a new conversation about this project, read this file first to restore full context. Update this file whenever major architectural decisions or status changes occur. **Especially reference Section 7 when beginning work on any new RP2040 board.**