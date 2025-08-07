#include "new_dawn_serial.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "NEW_DAWN";

// Global data storage
static new_dawn_data_t current_data = {0, 0, false, 0};
static SemaphoreHandle_t data_mutex = NULL;

void new_dawn_serial_init(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = NEW_DAWN_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(NEW_DAWN_UART_NUM, NEW_DAWN_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(NEW_DAWN_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(NEW_DAWN_UART_NUM, NEW_DAWN_TX_PIN, NEW_DAWN_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // Create mutex for thread-safe data access
    data_mutex = xSemaphoreCreateMutex();
    
    ESP_LOGI(TAG, "Serial interface initialized on UART%d (TX: GPIO%d, RX: GPIO%d)", 
             NEW_DAWN_UART_NUM, NEW_DAWN_TX_PIN, NEW_DAWN_RX_PIN);
}

bool new_dawn_get_data(new_dawn_data_t *data)
{
    if (!data || !data_mutex) {
        return false;
    }
    
    bool result = false;
    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        if (current_data.data_valid) {
            memcpy(data, &current_data, sizeof(new_dawn_data_t));
            result = true;
        }
        xSemaphoreGive(data_mutex);
    }
    
    return result;
}

// Calculate simple checksum (one's complement)
static uint8_t calculateChecksum(const uint8_t* data, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return ~sum;  // One's complement
}

// Parse incoming data from New Dawn
// Expected format: [ID][LENGTH][DATA...][CHECKSUM]
static bool parse_new_dawn_message(const uint8_t *buffer, size_t len, new_dawn_data_t *data)
{
    if (len < 3) return false;  // Minimum: ID + LENGTH + CHECKSUM
    
    uint8_t msg_id = buffer[0];
    uint8_t msg_len = buffer[1];
    
    // Verify we have complete message
    if (len < (size_t)(3 + msg_len)) return false;
    
    // Verify checksum
    uint8_t calc_checksum = calculateChecksum(buffer, 2 + msg_len);
    uint8_t recv_checksum = buffer[2 + msg_len];
    
    if (calc_checksum != recv_checksum) {
        ESP_LOGW(TAG, "Checksum mismatch: calc=0x%02X, recv=0x%02X", calc_checksum, recv_checksum);
        return false;
    }
    
    // Process message based on ID
    if (msg_id == MSG_MACHINE_STATUS && msg_len == sizeof(MachineStatus)) {
        memcpy(&data->status, &buffer[2], sizeof(MachineStatus));
        data->data_valid = true;
        data->timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        ESP_LOGD(TAG, "Machine Status - Speed: %.2f km/h, WAS: %.1f deg", 
                 data->status.speed / 100.0f, data->status.steerAngle / 10.0f);
        return true;
    }
    
    ESP_LOGW(TAG, "Unknown message ID: 0x%02X", msg_id);
    return false;
}

void new_dawn_serial_task(void *arg)
{
    uint8_t buffer[NEW_DAWN_BUF_SIZE];
    int buffer_pos = 0;
    
    ESP_LOGI(TAG, "Serial task started");
    
    // Log initial message
    ESP_LOGI(TAG, "Waiting for data on UART%d (TX: GPIO%d, RX: GPIO%d)", 
             NEW_DAWN_UART_NUM, NEW_DAWN_TX_PIN, NEW_DAWN_RX_PIN);
    
    while (1) {
        // Read data from UART
        int len = uart_read_bytes(NEW_DAWN_UART_NUM, 
                                  buffer + buffer_pos, 
                                  NEW_DAWN_BUF_SIZE - buffer_pos - 1, 
                                  pdMS_TO_TICKS(100));
        
        if (len > 0) {
            ESP_LOGD(TAG, "Received %d bytes", len);
            buffer_pos += len;
            
            // Process messages in buffer
            int processed = 0;
            while (processed < buffer_pos) {
                // Need at least 2 bytes for ID and LENGTH
                if (buffer_pos - processed < 2) break;
                
                uint8_t msg_id = buffer[processed];
                uint8_t msg_len = buffer[processed + 1];
                uint8_t total_len = 3 + msg_len; // ID + LENGTH + DATA + CHECKSUM
                
                // Check if we have complete message
                if (buffer_pos - processed < total_len) break;
                
                // Log binary data for debugging
                ESP_LOG_BUFFER_HEX_LEVEL(TAG, &buffer[processed], total_len, ESP_LOG_DEBUG);
                
                // Parse the message
                new_dawn_data_t temp_data;
                if (parse_new_dawn_message(&buffer[processed], total_len, &temp_data)) {
                    // Update global data with mutex
                    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                        memcpy(&current_data, &temp_data, sizeof(new_dawn_data_t));
                        xSemaphoreGive(data_mutex);
                        
                        ESP_LOGI(TAG, "Updated - Speed: %.2f km/h, WAS: %.1f deg", 
                                 temp_data.status.speed / 100.0f, 
                                 temp_data.status.steerAngle / 10.0f);
                    }
                }
                
                processed += total_len;
            }
            
            // Move unprocessed data to beginning of buffer
            if (processed > 0 && processed < buffer_pos) {
                memmove(buffer, &buffer[processed], buffer_pos - processed);
                buffer_pos -= processed;
            } else if (processed >= buffer_pos) {
                buffer_pos = 0;
            }
            
            // Prevent buffer overflow
            if (buffer_pos >= NEW_DAWN_BUF_SIZE - 1) {
                ESP_LOGW(TAG, "Buffer overflow, resetting");
                buffer_pos = 0;
            }
        }
        
        // Report status every 5 seconds
        static uint32_t last_status = 0;
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now - last_status > 5000) {
            if (current_data.data_valid) {
                uint32_t age = now - current_data.timestamp;
                ESP_LOGI(TAG, "Receiving data - Age: %lu ms, Speed: %.2f km/h, WAS: %.1f deg", 
                         age, current_data.status.speed / 100.0f, 
                         current_data.status.steerAngle / 10.0f);
            } else {
                ESP_LOGI(TAG, "No data received from New Dawn yet");
            }
            last_status = now;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}