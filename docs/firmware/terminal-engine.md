# Terminal Engine

**Last Updated:** April 2026  
**Status:** Planning / Early Implementation

This document defines the **Basic Terminal Engine** for the Doomsday Net Computer. It will serve as the primary user interface when the system boots — a lightweight, Linux-like shell running directly in firmware.

## Goals

- Provide a familiar, responsive command-line interface similar to 80s/90s terminals or a minimal Linux shell.
- Work reliably even if the Lua scripting layer fails or is disabled.
- Serve as the foundation for higher-level Lua scripting.
- Keep the implementation small, auditable, and cyberpandemic-resistant.

## Layer Position

- **Layer 1**: Firmware (C++) — Core Terminal Engine (this document)
- **Layer 2**: Lua Scripting Engine (runs on top of the terminal)

The terminal engine is part of the core firmware and should remain relatively stable.

## Core Features

### 1. Basic Navigation & File System Commands
- `help` — list available commands
- `ls` — list files in current directory
- `pwd` — print working directory
- `cd <dir>` — change directory (`cd ..` supported)
- `cat <file>` — display file contents
- `mkdir <name>` — create directory
- `rm <file>` — delete file
- `tree` — show directory tree (nice to have)

### 2. Output & System Commands
- `echo <text>` — print text (with basic redirection support later)
- `uptime` — show system uptime
- `info` — show system information (CPU, mode byte, memory usage, etc.)
- `mode` — show or set current unified mode byte
- `clear` — clear the terminal screen

### 3. Scripting & Execution
- `run <script.lua>` — execute a Lua script from `/boot/` or current directory
- `autorun.lua` — automatically executed on boot if present

### 4. Future / Nice-to-Have Commands
- `edit <file>` — simple text editor (nano-like)
- `cp <src> <dest>` — copy files
- `mv <src> <dest>` — rename/move files
- Command history (up/down arrows)
- Tab completion

## Display Behavior

- Scrolling terminal on the 320x240 ILI9341 TFT (green text on black, retro style)
- Commands typed in USB Serial Monitor are echoed to the TFT
- Output from commands and scripts appears on both Serial and TFT
- Prompt (`> `) shown on both interfaces

## File System Layout
/boot/
autorun.lua          ← Auto-executed on boot
blink.lua
emergency_net.lua
/scripts/                ← User scripts
/config/                 ← Configuration files


## Security & Cyberpandemic Immunity

- All input is validated and sanitized
- No network stack involved
- Scripts run in a sandboxed environment (future improvement)
- Core terminal engine remains small and auditable

## Implementation Roadmap

1. **Phase 1** — Basic shell with `help`, `ls`, `cd`, `pwd`, `cat`, `mkdir`, `rm`, `echo`, `uptime`, `info`, `run`
2. **Phase 2** — Scrolling TFT display + command echoing
3. **Phase 3** — Simple text editor like nano (`edit`)
4. **Phase 4** — Command history and tab completion
5. **Phase 5** — Lua integration and safe bindings

## Related Documents

- [Unified Firmware Architecture](../unified-firmware.md)
- [Doomsday Message Bus (DMB)](doomsday-message-bus.md)
- [Firmware vs Scripting Layer](firmware-vs-scripting.md)

---

This document will act as our roadmap as we build the terminal engine.

