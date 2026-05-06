# Doomsday Net Computer - Grok Context File

**Purpose**: This file exists so Grok can maintain persistent context across multiple conversations about this project, even when conversations become too long and new ones are started.

**Last Updated**: May 5, 2026

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
- **RP2040-PiZero** board (new project folder created)
- Future dedicated **MeshCore Interface Board** (USB Host → UART bridge)
- Additional UNO R4 Minima nodes in modular UART-linked setup

---

## 3. Key Decisions Made

- **MeshCore Connection**: Must use **stock firmware** on Heltec (no custom flashing). Direct USB connection to Teensy works reliably. USB hub causes receive failures.
- **Inter-board Communication**: Use **DoomsdayMessageBus** library over UART (not USB) for reliability between boards.
- **Project Structure**: Shared `lib/` folder in parent directory + `lib_extra_dirs = ../lib` in `platformio.ini` for both projects.
- **Serial Communication**: Full bidirectional support (keyboard → Serial Monitor + Serial Monitor → execute commands on Teensy).
- **Version Tracking**: Current firmware version is **v0.27**.

---

## 4. Important File Locations

- Main project: `teensy41/`
- New project: `RP2040-PiZero/`
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

---

## 6. Notes for Future Conversations

- Always use **stock MeshCore firmware** on Heltec boards.
- Prefer **UART + DoomsdayMessageBus** for inter-board links over USB when possible.
- Keep the system modular and low-power.
- Reference this file at the start of new conversations when context is needed.

---

**Grok Instructions**:  
When starting a new conversation about this project, read this file first to restore full context. Update this file whenever major architectural decisions or status changes occur.