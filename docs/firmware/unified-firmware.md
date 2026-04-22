# Unified Firmware Architecture

**Last Updated:** April 2026

## Overview

The Doomsday Net Computer uses a **unified firmware architecture** centered around a single **mode byte** stored in EEPROM. This byte defines the current operating mode of the entire system and allows all modules to behave consistently.

The system is built as a **distributed, modular, UART-linked network** with the Teensy 4.1 acting as the primary hub.

## Core Principles

- **One source of truth**: The mode byte in EEPROM on the main Teensy 4.1
- **Modular design**: Any microcontroller can be added via UART
- **Pub-Sub Communication**: All inter-module communication uses the [Doomsday Message Bus (DMB)](doomsday-message-bus.md)
- **Graceful degradation**: Nodes can operate independently if the hub is unavailable
- **Air-gapped & EMP resistant**: Minimal dependencies, simple protocols, low complexity

## Unified Mode Byte

The system mode is controlled by a single byte (0–255) stored in EEPROM:

- `0x00` — Boot / Safe Mode
- `0x01` — GMRS / FRS Monitoring
- `0x02` — Amateur Radio (HAM) Primary
- `0x03` — Emergency Mesh / ARES Mode
- `0x04` — LoRa / ISM Long Range
- `0x05` — Silent / Receive Only
- `0xFF` — Test / Debug Mode

All nodes (Teensy 4.1, RP2040 Display/Keyboard, future UNO R4 Minima modules, LoRa nodes, etc.) read this mode and adjust their behavior accordingly.

## Communication Backbone

All modules communicate using the **Doomsday Message Bus (DMB)** — a lightweight UART-based pub-sub protocol defined in [`doomsday-message-bus.md`](doomsday-message-bus.md).

This allows:
- The Teensy 4.1 to send commands and updates to the display module
- The RP2040 to send keyboard events back to the main system
- Future modules (LoRa, thermal printer, sensors, etc.) to publish and subscribe cleanly
- Easy expansion without changing core firmware

## Current Hardware Modules

- **Teensy 4.1** — Main hub, SD card, unified mode byte, logic engine
- **ILI9341 TFT (320x240)** — Local scrolling terminal display
- **RP2040-PiZero** (incoming) — Dedicated HDMI/DVI display + USB Host keyboard module

## Future Modules

- Multiple UNO R4 Minima nodes (UART daisy-chain)
- LoRa / ISM radio module
- Thermal printer controller
- GPS / timing module
- Sensor / environmental monitoring nodes

## Development Roadmap

1. **Phase 1** — Stable scrolling terminal on ILI9341 + basic DMB protocol
2. **Phase 2** — RP2040 HDMI output + USB keyboard integration via DMB
3. **Phase 3** — Real Lua scripting engine (on Teensy and RP2040)
4. **Phase 4** — Full multi-node support with UNO R4 Minima cluster
5. **Phase 5** — Waterfall display, RTTY/CW decode, emergency modes

## Related Documents

- [Doomsday Message Bus (DMB)](doomsday-message-bus.md)
- [UART Protocol Details](doomsday-message-bus.md)
- [Hardware Wiring](hardware/wiring-diagrams.md)

---

This document serves as the high-level architectural guide for the entire firmware ecosystem.

---

You can now paste the above content into `docs/firmware/unified-firmware.md`.

Would you like me to:
1. Update the main `README.md` to link to these new architecture documents?
2. Start implementing the basic Doomsday Message Bus protocol in the Teensy firmware?
3. Or something else?

Let me know what you'd like to tackle next.