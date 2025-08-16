#ifndef NEW_DAWN_SERIAL_H
#define NEW_DAWN_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

// Serial configuration for New Dawn communication
// XIAO ESP32C3 in AiO board socket
// Using UART1 for general purpose serial communication
#define NEW_DAWN_UART_NUM     UART_NUM_1
#define NEW_DAWN_TX_PIN       GPIO_NUM_21   // D6 on XIAO - TX to Teensy RX (UART1 default)
#define NEW_DAWN_RX_PIN       GPIO_NUM_20   // D7 on XIAO - RX from Teensy TX (UART1 default)
#define NEW_DAWN_BAUD_RATE    460800        // Match New Dawn's SerialESP32 rate
#define NEW_DAWN_BUF_SIZE     256

// Message IDs
#define MSG_MACHINE_STATUS    0x01
#define MSG_HANDSHAKE_REQUEST 0x10
#define MSG_HANDSHAKE_RESPONSE 0x11

// Machine status structure (must match New Dawn)
typedef struct __attribute__((packed)) {
    int16_t speed;        // Speed in 0.01 km/h
    int16_t heading;      // Heading in 0.1 degrees
    int16_t roll;         // Roll in 0.1 degrees
    int16_t pitch;        // Pitch in 0.1 degrees
    int16_t steerAngle;   // WAS - Steer angle in 0.1 degrees
} MachineStatus;

// New Dawn data structure
typedef struct {
    MachineStatus status;   // Latest machine status
    bool data_valid;        // True if data is valid
    uint32_t timestamp;     // Timestamp of last update
} new_dawn_data_t;

// Function prototypes
void new_dawn_serial_init(void);
bool new_dawn_get_data(new_dawn_data_t *data);
void new_dawn_serial_task(void *arg);

#endif // NEW_DAWN_SERIAL_H