# RP2040 Display + Keyboard Module

**Role**: Dedicated co-processor for HDMI/DVI output and future USB Host keyboard handling.

## Hardware
- Waveshare RP2040-PiZero (or compatible RP2040 board with Mini HDMI)
- Connected to Teensy 4.1 via UART (3 wires: TX/RX/GND)

## Current Status
- Stable 640×480 DVI output
- Basic UART command receiver from Teensy 4.1
- Retro TempleOS-style splash screen

## Planned Features
- USB Host keyboard support
- Graphical dashboard with status bars and mode byte display
- Real-time waterfall display for radio signals
- Lua scripting support for custom UI behaviors

**Communication Protocol**: See `uart-protocol.md`

Last Updated: April 2026
