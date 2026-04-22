# Doomsday Message Bus (DMB)

**Version:** 0.1 (Draft)  
**Date:** April 2026  
**Purpose:** Lightweight, UART-based pub-sub communication protocol for the Doomsday Net Computer.

## Design Goals

- Simple enough to implement on any microcontroller (Teensy 4.1, RP2040, UNO R4 Minima, etc.)
- Inspired by ROS/ROS 2 and MQTT, but optimized for direct UART links
- No central broker required — soft preference for Teensy 4.1 as the hub
- Nodes can operate independently if the hub is offline
- Human-readable for easy debugging
- Low overhead and robust for air-gapped, low-power, EMP-hardened systems

## Core Philosophy

The system uses a **star topology** with the Teensy 4.1 as the preferred central hub, but any node can publish and subscribe to topics. Communication is strictly over UART (3 wires: TX, RX, GND).

All nodes follow the same simple text-based protocol.

## Message Format

Every message is a single line terminated by `\n`:


### Fields

- **TYPE** (single character)
  - `P` = Publish — a node is announcing an event
  - `C` = Command — hub sending a command to a specific node or group
  - `R` = Response — reply/acknowledgment from a node
  - `S` = Subscribe — a node wants to receive messages on a topic

- **TOPIC** — descriptive name using dot notation (e.g. `display.text`, `keyboard.key`, `system.mode`, `radio.waterfall`, `lua.run`)

- **PAYLOAD** — the actual data (can be plain text, comma-separated values, or simple structured data)

## Examples

### From Teensy (Hub) to RP2040 (Display)

### From RP2040 (or any module) to Teensy

## Reserved Topics

- `system.mode` — Current unified mode byte
- `system.status` — Node status (ready, error, low-battery, etc.)
- `display.text` — Text to show on display
- `display.clear` — Clear display
- `keyboard.key` — Key press events
- `lua.run` — Request to run a Lua script
- `radio.waterfall` — Spectrum/waterfall data column

## Implementation Guidelines

1. All nodes should parse incoming messages and ignore unknown topics gracefully.
2. The Teensy 4.1 acts as the primary router but is not required for basic node operation.
3. Nodes should be able to operate in a degraded mode if the hub is unavailable.
4. Use high baud rates (921600 or higher) where possible for smooth waterfall/display updates.
5. Keep payload simple — avoid heavy JSON unless necessary.

## Future Extensions

- Binary packet mode for high-bandwidth data (waterfall, images)
- Node discovery / heartbeat
- Topic wildcards (`system.*`)
- Message priority / QoS levels

---

This document will serve as the living specification for all inter-module communication in the Doomsday Net Computer.

**Next Steps:**
- Define specific topics for keyboard, display, Lua, radio, etc.
- Implement the protocol on the Teensy 4.1 side
- Create RP2040 receiver when the board arrives

Would you like me to expand any section, or shall we start implementing the protocol on the Teensy side next?