# Continue Little Dawn ESP-IDF Development

## Context
I'm working on "Little Dawn" - an ESP32C3-based ISOBUS Virtual Terminal bridge for the AiO_New_Dawn Teensy 4.1 project. 

## Current Status
- Created fresh ESP-IDF project structure in `/Users/chris/Documents/Code/Little_Dawn/`
- Implemented basic AgIsoStack integration with TWAI (ESP32's built-in CAN)
- Using SN65HVD230 CAN transceiver instead of MCP2515
- Basic address claiming working

## Hardware Setup
- **MCU**: Seeed XIAO ESP32C3  
- **CAN**: TWAI on GPIO21/22 with SN65HVD230 transceiver
- **Serial**: Will use GPIO2/3 (D0/D1) for New Dawn communication
- **LED**: GPIO8 (D8) for status

## What's Working
1. ESP-IDF project structure with AgIsoStack
2. TWAI configuration for 250kbps ISOBUS
3. Basic ISOBUS address claiming
4. LED status indicator

## Next Tasks
1. **Build and test** the current code - verify address claiming works
2. **Port serial protocol** from Arduino version to ESP-IDF
3. **Add Virtual Terminal client** with basic object pool
4. **Integrate WAS data** from New Dawn to display on VT

## Key Files
- `main/main.cpp` - Main application code
- `sdkconfig.defaults` - ESP-IDF configuration
- Serial protocol needs porting from the PlatformIO project

## Build Commands
```bash
idf.py build
idf.py -p /dev/cu.usbserial-XXXX flash monitor
```

Please help me continue development, starting with building and testing the current code.