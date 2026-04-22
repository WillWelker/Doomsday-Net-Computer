# Doomsday Net Computer

A rugged, air-gapped, EMP-resistant communications and computing platform designed to operate **completely outside the internet superhighway**.

Built for resilience in the face of grid-down scenarios, cyber events, or any situation where modern networked systems may fail.

![Doomsday Net Computer](image.png)

## Project Goals

- Ham Radio BBS Network, get in losers, we are going back to 1985!
- Fully modular, repairable hardware using simple UART-linked microcontrollers for distributed computing that is immune to a "Cyber Pandemic" event
- Unified firmware architecture with a single **mode byte** in EEPROM controlling system behavior
- Low-power, air-gapped design suitable for HAM,  HF/VHF/ISM emergency communications
- Support for keyboard input, local display (TFT + future HDMI), SD card storage, and scripting
- Designed to survive "Cyber Pandemic" or major infrastructure collapse

## Core Architecture

- **Main Hub**: Teensy 4.1 (high performance, SD card, unified mode byte)
- **Display Module**: 320x240 ILI9341 TFT with scrolling retro terminal (live)
- **Future Display**: RP2040-PiZero with Mini HDMI output + USB Host keyboard
- **Communication**: Doomsday Message Bus (DMB) — a lightweight UART pub-sub protocol
- **Scripting**: Persistent Lua scripts on SD card (`/boot/autorun.lua` and others)

## Current Capabilities

- Scrolling 80s-style terminal on 2.8" ILI9341 TFT
- Persistent Lua script support (`/boot/autorun.lua` and `run <script>` command)
- LED feedback on both onboard and external LEDs
- SD card logging and script storage
- Modular design ready for expansion (LoRa, thermal printer, multiple UNO R4 Minima nodes, etc.)

## Documentation

- [Unified Firmware Architecture](docs/firmware/unified-firmware.md)
- [Doomsday Message Bus (DMB)](docs/firmware/doomsday-message-bus.md)
- [RP2040 Display + Keyboard Module](docs/modules/rp2040-display-keyboard.md)
- [Hardware Wiring Diagrams](docs/hardware/wiring-diagrams.md)

## Hardware Stack (In Progress)

- Teensy 4.1 (main controller)
- 2.8" ILI9341 TFT (320x240)
- RP2040-PiZero (incoming — HDMI + USB Host keyboard)
- Planned: Multiple UNO R4 Minima nodes via UART daisy-chain

## Philosophy

This is not just another embedded project.

It is an attempt to build a **resilient, understandable, repairable** computing and communications platform that can continue to function when modern infrastructure (internet, cellular, cloud services) is unavailable or compromised.

"Who will you call when the phones go dark?"

---

## Getting Started

1. Clone the repository
2. Open in PlatformIO (VS Code)
3. Build and upload the Teensy 4.1 firmware
4. Insert SD card with `/boot/` directory for Lua scripting

More detailed build and setup instructions coming soon.

---

**Status**: Early development — actively building toward a full air-gapped HF/VHF/ISM communications computer.

Contributions, ideas, and discussion are welcome.

---

