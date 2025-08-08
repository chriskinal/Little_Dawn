| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

# Little Dawn - ESP32 ISOBUS Bridge

Little Dawn is an ESP32-based ISOBUS bridge that enables communication between agricultural equipment using the ISO 11783 (ISOBUS) protocol. It acts as a bridge between "New Dawn" (a proprietary agricultural device) and ISOBUS Virtual Terminals (VT), providing real-time display of machine data on standard agricultural displays.

## Features

- **Real-time Data Display**: Shows wheel angle sensor (WAS) data and vehicle speed on ISOBUS VT displays
- **High-Speed Serial Communication**: 460800 baud serial link to New Dawn device
- **CAN Bus Communication**: Full ISO 11783 compliance at 250kbps
- **Low Latency**: Minimal lag (~50-100ms) between data updates
- **Button Support**: Soft key functionality for user interaction

## Hardware Requirements

- **MCU**: ESP32-C3 (XIAO ESP32C3 in AiO board socket)
- **CAN Transceiver**: SN65HVD230
- **Pin Configuration**:
  - TWAI TX: GPIO4 (D2)
  - TWAI RX: GPIO5 (D3)
  - Status LED: GPIO8 (D8)
  - Serial to New Dawn: GPIO21/20 (D6/D7)

## Software Architecture

The project uses:
- **ESP-IDF v5.5**: Espressif's official development framework
- **AgIsoStack++**: Open-source ISOBUS protocol stack
- **Custom Components**:
  - VT object pool display (LD12.iop)
  - Serial protocol handler for New Dawn communication
  - ESP32-specific logger integration

## Building and Flashing

### Option 1: Using ESP-IDF (for development)

1. Set up ESP-IDF environment:
```bash
source /path/to/esp-idf/export.sh
```

2. Build the project:
```bash
idf.py build
```

3. Flash and monitor:
```bash
idf.py -p /dev/cu.usbmodem* flash monitor
```

### Option 2: Using pre-built binaries (no ESP-IDF required)

If you have pre-built binaries from the `build` directory, you can flash them directly using `esptool.py`:

```bash
esptool.py -p /dev/cu.usbmodem* -b 460800 --chip esp32c3 write_flash \
  --flash_mode dio --flash_freq 80m --flash_size 2MB \
  0x0 bootloader/bootloader.bin \
  0x8000 partition_table/partition-table.bin \
  0x10000 Little_Dawn.bin
```

Required files (found in the `build` directory after compilation):
- `bootloader/bootloader.bin` - Bootloader (flashed at 0x0)
- `partition_table/partition-table.bin` - Partition table (flashed at 0x8000)
- `Little_Dawn.bin` - Main application (flashed at 0x10000)

## Serial Protocol

Communication with New Dawn uses a binary protocol:
- **Format**: `[ID][LENGTH][DATA...][CHECKSUM]`
- **Machine Status Message (0x01)** contains:
  - Speed (0.01 km/h units)
  - Heading (0.1 degree units)
  - Roll/Pitch (0.1 degree units)
  - Steering angle (0.1 degree units)

## Project Status

The project is fully functional with:
- ✅ Bidirectional CAN communication
- ✅ ISOBUS address claiming
- ✅ VT object pool display
- ✅ Real-time data updates
- ✅ Serial communication with New Dawn

## Configuration

Key settings in `sdkconfig`:
- ISOBUS threading disabled for manual update control
- TWAI (CAN) configured for 250kbps
- Custom partition table for larger application size

## Troubleshooting

- **No CAN communication**: Check TWAI pin connections and transceiver power
- **VT not displaying**: Verify object pool format and VT compatibility
- **Serial data issues**: Confirm baud rate (460800) and checksum calculation

## Technical Support

For issues or questions:
- Review the [CLAUDE.md](CLAUDE.md) file for detailed technical notes
- Check debug logs with monitor command
- Use a CAN analyzer to verify message transmission

## License

This project uses open-source components including AgIsoStack++ under their respective licenses.