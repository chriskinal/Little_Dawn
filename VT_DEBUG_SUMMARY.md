# Little Dawn VT Debugging Summary

## Project Overview
Little Dawn is an ESP32-based ISOBUS bridge that communicates between "New Dawn" (agricultural equipment) and ISOBUS Virtual Terminals using the ISO 11783 protocol.

## What We Learned

### 1. Hardware Configuration (Working)
- **MCU**: ESP32C3 (XIAO ESP32C3)
- **CAN Transceiver**: SN65HVD230
- **Pin Connections**:
  - TWAI_TX: GPIO4 (D2 on XIAO)
  - TWAI_RX: GPIO5 (D3 on XIAO)
  - LED: GPIO8 (D8)
- **CAN Communication**: Hardware verified working (TX/RX confirmed before VT implementation)

### 2. Software Environment (Working)
- **ESP-IDF**: v5.5 at `/Users/chris/esp/v5.5/esp-idf`
- **Build System**: Working correctly
- **AgIsoStack**: Using managed component from GitHub
- **Key Configuration**: `CONFIG_CAN_STACK_DISABLE_THREADS=1` (requires manual update() calls)

### 3. Critical Fixes Discovered

#### A. CAN Communication Fix
**Problem**: No CAN transmission despite TWAI showing as running
**Root Cause**: Thread/update conflict with `CONFIG_CAN_STACK_DISABLE_THREADS=1`
**Solution**: Remove periodic update interval, use manual updates:
```cpp
// Don't set periodic update interval since threads are disabled
while (true) {
    isobus::CANHardwareInterface::update();  // Manual update required!
    // ... rest of loop
}
```

#### B. VT State Machine Fix
**Problem**: VT client stuck in initialization, never progressing
**Root Cause**: Missing VT client state machine update
**Solution**: Add `vtClient->update()` call in VTClient::update():
```cpp
void VTClient::update()
{
    if (vtClient) {
        vtClient->update();  // THIS WAS CRITICAL - updates state machine!
    }
}
```

#### C. Address Conflict Fix
**Problem**: Address conflict with VT
**Solution**: Change Little Dawn preferred address from 0x26 to 0x80

### 4. VT Object Pool Issues Encountered

#### Current Status
- Object pool uploads successfully (166 bytes)
- VT reports "Unsupported Object" and fails parsing
- Variable updates fail with "invalid object ID"

#### Object Type Corrections Made
1. **Output String**: 0x0B (11) - not 0x0C
2. **Output Number**: 0x0C (12) - not 0x0D
3. **Data Mask reference**: 0x01 - not 0x11

#### Structural Fixes Attempted
1. Working Set: Made number of references 2-byte field
2. Soft Key Mask: Made counts 2-byte fields
3. Output String: String length is 1 byte (not 2)

#### VT Configuration
- VT Version: 4 (good balance of features/compatibility)
- Data Mask Size: 480x480 pixels
- Soft Keys: 6 physical keys, 60x60 pixel designators

### 5. Lessons Learned

1. **Start Simple**: Complex object pools are hard to debug. Start with minimal "Hello World"
2. **VT Parser is Strict**: Even small format errors cause complete parsing failure
3. **Debugging is Limited**: VT only reports "Unsupported Object" without details
4. **Manual Updates Critical**: With threads disabled, every component needs manual updating
5. **Object Pool Structure**: Very specific byte layouts required for ISO 11783

### 6. Recommended Approach Going Forward

#### Phase 1: Minimal VT Display
1. Create simplest possible object pool (Working Set + Data Mask + one Output String)
2. Display static "Hello VT" text
3. Verify parsing succeeds and text displays

#### Phase 2: Add Complexity Gradually
1. Add one variable at a time
2. Test each addition thoroughly
3. Add soft keys
4. Add more display elements

#### Phase 3: Dynamic Updates
1. Implement variable updates
2. Add serial communication with New Dawn
3. Display real data from New Dawn

### 7. Build Commands Reference
```bash
# Source ESP-IDF
source /Users/chris/esp/v5.5/esp-idf/export.sh

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/cu.usbmodem* flash monitor

# Clean build
rm -rf build && idf.py build
```

### 8. Common Error Messages
- "Unsupported Object" - Object type or structure incorrect
- "Invalid object ID" - Referenced object doesn't exist or ID mismatch
- "File system error or corruption" - VT trying to load stored pool but failing
- "Get End of Object Pool Response Timeout" - Pool parsing failed

### 9. Next Steps
1. Create new minimal main.cpp with just TWAI setup
2. Build minimal VT object pool step by step
3. Add AgIsoStack components one at a time
4. Test thoroughly at each step
5. Document working object pool structures

## Key Takeaway
The VT parser is extremely strict about object pool format. Building incrementally from a known-working minimal example is the most reliable approach.