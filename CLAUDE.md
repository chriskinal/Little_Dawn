# Little Dawn - ESP32 ISOBUS Bridge Project

## Project Overview
Little Dawn is an ESP32-based ISOBUS bridge that communicates with agricultural equipment using the ISO 11783 (ISOBUS) protocol. It acts as a bridge between "New Dawn" (another device) and ISOBUS Virtual Terminals (VT).

## Hardware Configuration
- **MCU**: ESP32C3 (XIAO ESP32C3)
- **CAN Transceiver**: SN65HVD230
- **Pin Connections**:
  - TWAI_TX: GPIO4 (D2 on XIAO)
  - TWAI_RX: GPIO5 (D3 on XIAO)
  - LED: GPIO8 (D8 - AiO board LED)
  - Serial to New Dawn: GPIO21/20 (D6/D7) - planned
  - Available: GPIO6 (D4), GPIO9/10 (D9/D10)

## Software Stack
- **Framework**: ESP-IDF v5.5
- **ISOBUS Library**: AgIsoStack++ (with CONFIG_CAN_STACK_DISABLE_THREADS=1)
- **CAN Controller**: TWAI (ESP32's CAN peripheral)
- **Protocol**: ISO 11783 at 250kbps

## Key Components
1. **main.cpp**: Main application with ISOBUS initialization and CAN communication
2. **vt_client.cpp/h**: Virtual Terminal client implementation with object pool
3. **esp32_logger.h**: Custom logger for AgIsoStack integration
4. **vt_object_ids.h**: Defines all VT object IDs used in the display

## Current Status
- ✅ CAN communication working (TX/RX verified)
- ✅ Address claiming successful at 0x80
- ✅ VT client initialization and state machine updates
- ✅ Object pool upload to VT (166 bytes)
- ❌ VT object pool parsing failing with "Unsupported Object"
- 🔧 Plan: Rebuild from scratch with incremental approach

## Common Issues and Solutions
1. **No CAN transmission**: Ensure manual `isobus::CANHardwareInterface::update()` calls since threads are disabled
2. **VT not connecting**: Add `vtClient->update()` call in VTClient::update() method
3. **Address conflicts**: Use 0x80 instead of 0x26 (VT's address)
4. **Object pool parsing errors**: Check object ID byte order and ensure all referenced objects exist

## Build and Flash Commands
```bash
# Source ESP-IDF
source /Users/chris/esp/v5.5/esp-idf/export.sh

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/cu.usbmodem* flash monitor
```

## Debugging Tips
- Enable debug logging: `isobus::CANStackLogger::set_log_level(isobus::CANStackLogger::LoggingLevel::Debug)`
- Check TWAI status for hardware issues
- Monitor VT logs for object pool parsing errors
- Use CAN sniffer to verify message transmission

## Next Steps (Tomorrow's Plan)
1. Start fresh with minimal ESP32 + TWAI setup
2. Create simplest possible VT object pool (just "Hello VT" text)
3. Verify basic VT connection and display
4. Incrementally add one object at a time:
   - First: Add a variable
   - Second: Add soft keys
   - Third: Add more complex display elements
5. Once stable, add serial communication with New Dawn
6. Display real New Dawn data on the VT

## Important Notes
- VT parser is extremely strict about object pool format
- Build incrementally from known-working examples
- Test thoroughly after each addition
- See VT_DEBUG_SUMMARY.md for detailed learnings