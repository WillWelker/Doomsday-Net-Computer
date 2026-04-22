# Firmware vs Scripting Layer

**Last Updated:** April 2026

This document defines the clear boundary between what belongs in **firmware** (C++ on the microcontrollers) and what should be implemented in **scripting** (Lua on the SD card).

## Core Philosophy

- **Firmware** = Small, fast, auditable, security-critical code that changes rarely.
- **Scripting** = Flexible, user-modifiable behavior that can be changed without recompiling or reflashing.

The goal is to keep the firmware minimal (reducing attack surface for cyberpandemic immunity) while giving users maximum flexibility through editable scripts.

## 1. Firmware Layer (C++)

These functions must live in the core firmware because they are performance-critical, security-sensitive, or provide foundational services:

### Must Be in Firmware
- **Doomsday Message Bus (DMB)** engine
  - Full message parsing, routing, publishing and subscribing
  - UART drivers and low-level protocol handling
- **Unified Mode Byte** management
  - Reading/writing EEPROM
  - Mode validation and change broadcasting
- **SD Card core access**
  - Safe initialization, file I/O primitives, and basic file system operations
- **Hardware abstraction & safety**
  - GPIO control, LED drivers, watchdog timer, power monitoring
- **Display drivers**
  - Low-level ILI9341 (and future DVI) driver code
- **Script Engine Host**
  - Lua interpreter (or lightweight scripting runner)
  - Safe binding layer between Lua and firmware functions
- **Security boundaries**
  - Input validation for all external data (UART messages, SD card scripts, etc.)
- **Boot sequence & system integrity checks**

### Should Be in Firmware (Strongly Recommended)
- Basic timing and scheduling primitives
- Error logging to SD card
- Emergency shutdown / safe mode logic

## 2. Scripting Layer (Lua on SD card)

Everything higher-level should be done in Lua scripts stored on the SD card (especially in `/boot/` and subdirectories).

### Should Be Done in Lua
- **User Interface & Display Logic**
  - All text formatting, menus, status screens, dashboards
  - Waterfall rendering logic
  - Custom screen layouts

- **Applications & Features**
  - BBS (Bulletin Board System)
  - RTTY/CW/JS8CALL decode post-processing
  - Contact lists, repeater databases, net procedures
  - Logging and reporting systems

- **Automation & Behavior**
  - Boot-time scripts (`autorun.lua`)
  - Mode-specific behaviors
  - Scheduled tasks
  - Emergency protocols and checklists

- **Input Handling**
  - Keyboard mapping and macros
  - Custom key combinations → DMB commands

- **Integration Logic**
  - How different modules interact at the application level
  - Custom net check-in scripts
  - User-defined modes and workflows

- **Extensibility**
  - Any feature a user might want to add or modify later

## Examples

**Firmware provides:**
- `sendDMB("display.text", "Hello World")`
- `setMode(0x03)`
- `runLuaScript("emergency_net.lua")`
- `readSDFile("/boot/config.txt")`

**Lua scripts use those primitives to build:**
- Full dashboard rendering
- BBS message handling
- Custom emergency procedures
- User-defined automation

## Benefits of This Split

- **Cyberpandemic Immunity**: The firmware stays small, simple, and rarely changed → minimal attack surface.
- **Flexibility**: Users can modify behavior by editing files on the SD card without touching firmware.
- **Maintainability**: Core system remains auditable.
- **Repairability**: Individual modules can be replaced or reprogrammed independently.

## Related Documents

- [Unified Firmware Architecture](unified-firmware.md)
- [Doomsday Message Bus (DMB)](doomsday-message-bus.md)

---

This gives us a clear, defensible boundary that we can reference as we continue building the system.

Would you like me to make any adjustments to this document before you commit it? Or shall we now start implementing the basic DMB engine in the Teensy firmware? 

Let me know how you'd like to proceed.