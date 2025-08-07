# Little Dawn Serial Integration for AiO_New_Dawn

## Context
I have an ESP32 co-processor called "Little Dawn" that will handle ISOBUS communication. It's connected via serial to the Teensy 4.1. We need to add serial communication to send WAS (Wheel Angle Sensor) data from New Dawn to Little Dawn.

## Hardware Connection
- Use SerialESP32 on Teensy 4.1 (Serial2, pins 7/8)
- Baud rate: 460800 (matches SerialManager configuration)

## Code to Add

Please add the following to AiO_New_Dawn:

### 1. Add these definitions near the top of the main file:

```cpp
// Little Dawn Serial Communication
#define LITTLE_DAWN_SERIAL SerialESP32  // Serial2
#define LITTLE_DAWN_BAUD 460800

// Message IDs
#define MSG_MACHINE_STATUS 0x01

// Machine status structure (must match Little Dawn)
struct MachineStatus {
  int16_t speed;        // Speed in 0.01 km/h
  int16_t heading;      // Heading in 0.1 degrees
  int16_t roll;         // Roll in 0.1 degrees
  int16_t pitch;        // Pitch in 0.1 degrees
  int16_t steerAngle;   // WAS - Steer angle in 0.1 degrees
} __attribute__((packed));
```

### 2. Add these functions:

```cpp
// Calculate simple checksum
uint8_t calculateChecksum(const uint8_t* data, size_t len) {
  uint8_t sum = 0;
  for (size_t i = 0; i < len; i++) {
    sum += data[i];
  }
  return ~sum;  // One's complement
}

// Send message to Little Dawn
void sendToLittleDawn(uint8_t id, const uint8_t* data, uint8_t length) {
  uint8_t buffer[68];  // id + length + data + checksum
  buffer[0] = id;
  buffer[1] = length;
  memcpy(&buffer[2], data, length);
  buffer[2 + length] = calculateChecksum(buffer, 2 + length);
  
  LITTLE_DAWN_SERIAL.write(buffer, 3 + length);
}

// Send machine status with WAS data
void sendMachineStatus() {
  MachineStatus status;
  
  // Use existing New Dawn variables
  status.speed = (int16_t)(speedKph * 100);           // Convert to 0.01 km/h units
  status.heading = (int16_t)(heading * 10);           // If available, in 0.1 degree units
  status.roll = (int16_t)(roll * 10);                 // If available, in 0.1 degree units
  status.pitch = (int16_t)(pitch * 10);               // If available, in 0.1 degree units
  status.steerAngle = (int16_t)(steerAngle * 10);     // WAS in 0.1 degree units
  
  sendToLittleDawn(MSG_MACHINE_STATUS, (uint8_t*)&status, sizeof(status));
}
```

### 3. In setup(), add:

```cpp
// Initialize Little Dawn communication
// Note: SerialESP32 is already initialized by SerialManager at 460800 baud
Serial.println("Little Dawn serial initialized on Serial2 at 460800 baud");
```

### 4. In the main loop, add periodic sending:

```cpp
// Send data to Little Dawn every 100ms
static unsigned long lastLittleDawnUpdate = 0;
if (millis() - lastLittleDawnUpdate > 100) {
  sendMachineStatus();
  lastLittleDawnUpdate = millis();
}
```

## Variables to Check/Adjust

Please check what the actual variable names are in New Dawn for:
- `speedKph` - The current speed in km/h
- `steerAngle` - The WAS reading (might be called `wasAngle` or similar)
- `heading` - If available from GPS/IMU
- `roll` - If available from IMU
- `pitch` - If available from IMU

Set any unavailable values to 0 for now.

## Testing

1. The ESP32 will print received WAS values to its serial monitor
2. The ESP32's LED will be solid when receiving data
3. Every 5 seconds, the ESP32 prints a status report showing connection state

## What This Enables

Once working, Little Dawn will receive real-time machine data and can:
- Display it on ISOBUS Virtual Terminals
- Forward it to implements
- Handle ISOBUS task control based on machine state