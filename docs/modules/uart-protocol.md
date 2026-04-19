# UART Protocol - Teensy 4.1 ↔ RP2040 Display Module

**Baud Rate**: 921600 (recommended)

## Direction: Teensy → RP2040
Simple text commands terminated with `\n`

### Current Example Commands
- `TEXT:Hello from main brain`
- `CLEAR`
- `STATUS:Mode=0x03,Uptime=3h 12m`

## Direction: RP2040 → Teensy
- Keyboard events (when implemented)
- Status / acknowledgment messages

## Future Enhancements
- Binary packet mode for high-speed data (waterfall columns, graphics)
- Command acknowledgments and error handling

This protocol will grow as we add waterfall, graphics commands, and Lua integration.
