/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_task_wdt.h"

// AgIsoStack includes
#include "isobus/hardware_integration/twai_plugin.hpp"
#include "isobus/hardware_integration/can_hardware_interface.hpp"
#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/isobus_virtual_terminal_client.hpp"

#include "esp32_logger.h"
#include "simple_object_pool.h"
#include "little_dawn_object_pool.h"
#include "minimal_object_pool.h"
#include "working_object_pool.h"
#include "vt_object_ids.h"

static const char *TAG = "LITTLE_DAWN";

// Pin configuration for XIAO ESP32C3
#define TWAI_TX_GPIO  GPIO_NUM_4  // D2
#define TWAI_RX_GPIO  GPIO_NUM_5  // D3
#define LED_GPIO      GPIO_NUM_8  // D8 (AiO board LED)

// ISOBUS standard bitrate
#define TWAI_BITRATE  TWAI_TIMING_CONFIG_250KBITS()

// Global ISOBUS objects
static std::shared_ptr<isobus::CANHardwarePlugin> canDriver = nullptr;
static std::shared_ptr<isobus::InternalControlFunction> internalECU = nullptr;
static std::shared_ptr<isobus::PartneredControlFunction> partnerVT = nullptr;
static std::shared_ptr<isobus::VirtualTerminalClient> vtClient = nullptr;

// Variables for VT display updates
static uint32_t displayValue = 0;
static bool vtConnected = false;
// External symbols for embedded object pool binary
extern "C" const uint8_t object_pool_start[] asm("_binary_object_pool_iop_start");
extern "C" const uint8_t object_pool_end[] asm("_binary_object_pool_iop_end");


void setup_led(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_GPIO, 0);  // LED off initially
}

void blink_led(int times, int duration_ms)
{
    for (int i = 0; i < times; i++) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
    }
}

void can_update_task(void *arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // 10ms update rate
    
    while (1) {
        // Update the CAN hardware interface (required when threads are disabled)
        isobus::CANHardwareInterface::update();
        
        // Update VT client if it exists
        if (vtClient) {
            vtClient->update();
        }
        
        
        // Use vTaskDelayUntil for more consistent timing
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Little Dawn Starting...");
    
    // Initialize LED
    setup_led();
    
    // Setup ESP32 logger for AgIsoStack
    isobus::ESP32Logger::setup();
    
    // Enable debug logging for VT debugging
    isobus::CANStackLogger::set_log_level(isobus::CANStackLogger::LoggingLevel::Debug);
    
    // Configure TWAI
    twai_general_config_t twaiConfig = TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX_GPIO, TWAI_RX_GPIO, TWAI_MODE_NORMAL);
    twaiConfig.rx_queue_len = 20;
    twaiConfig.tx_queue_len = 20;
    
    twai_timing_config_t twaiTiming = TWAI_BITRATE;
    twai_filter_config_t twaiFilter = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    
    // Create TWAI driver instance
    canDriver = std::make_shared<isobus::TWAIPlugin>(&twaiConfig, &twaiTiming, &twaiFilter);
    
    // Configure CAN hardware interface
    isobus::CANHardwareInterface::set_number_of_can_channels(1);
    isobus::CANHardwareInterface::assign_can_channel_frame_handler(0, canDriver);
    
    // Start CAN hardware
    if (!isobus::CANHardwareInterface::start() || !canDriver->get_is_valid()) {
        ESP_LOGE(TAG, "Failed to start CAN hardware interface");
        blink_led(10, 100); // Fast blink to indicate error
        return;
    }
    
    ESP_LOGI(TAG, "CAN hardware started successfully");
    
    // Create ISOBUS NAME for our device
    isobus::NAME deviceNAME(0);
    deviceNAME.set_arbitrary_address_capable(true);
    deviceNAME.set_industry_group(2); // Agricultural
    deviceNAME.set_device_class(0);    // Non-specific
    deviceNAME.set_function_code(130); // Auxiliary control
    deviceNAME.set_identity_number(1); // Unique ID
    deviceNAME.set_ecu_instance(0);
    deviceNAME.set_function_instance(0);
    deviceNAME.set_device_class_instance(0);
    deviceNAME.set_manufacturer_code(1407); // Open-Agriculture
    
    // Create internal control function (our ECU)
    internalECU = isobus::CANNetworkManager::CANNetwork.create_internal_control_function(deviceNAME, 0, 0x80);
    
    if (!internalECU) {
        ESP_LOGE(TAG, "Failed to create internal control function");
        blink_led(10, 100);
        return;
    }
    
    ESP_LOGI(TAG, "ISOBUS ECU created, starting address claim at 0x%02X", 0x80);
    
    // Re-enable VT client with increased pthread stack size
    #if 1
    // Create VT partner control function (any VT on the bus)
    const isobus::NAMEFilter vtFilter(isobus::NAME::NAMEParameters::FunctionCode, 
                                     static_cast<uint8_t>(isobus::NAME::Function::VirtualTerminal));
    partnerVT = isobus::CANNetworkManager::CANNetwork.create_partnered_control_function(0, {vtFilter});
    
    // Create VT client
    vtClient = std::make_shared<isobus::VirtualTerminalClient>(partnerVT, internalECU);
    
    // Use embedded example object pool - the only one that works reliably
    size_t poolSize = object_pool_end - object_pool_start;
    ESP_LOGI(TAG, "Using embedded example object pool: %d bytes", poolSize);
    vtClient->set_object_pool(0, object_pool_start, poolSize, "ais1");
    
    // Initialize VT client with data storage callbacks enabled
    vtClient->initialize(true);
    ESP_LOGI(TAG, "VT client initialized");
    
    // Set up VT state change callback
    vtClient->get_vt_change_active_mask_event_dispatcher().add_listener(
        [](const isobus::VirtualTerminalClient::VTChangeActiveMaskEvent &event) {
            ESP_LOGI(TAG, "VT Active Mask Changed - Mask: %d, Parent: %d, Errors: %s", 
                     event.maskObjectID, event.parentObjectID, 
                     (event.missingObjects || event.maskOrChildHasErrors || event.anyOtherError) ? "Yes" : "No");
        });
    
    vtClient->get_vt_select_input_object_event_dispatcher().add_listener(
        [](const isobus::VirtualTerminalClient::VTSelectInputObjectEvent &event) {
            ESP_LOGI(TAG, "VT Input Selected - Object: %d, Selected: %s", 
                     event.objectID, event.objectSelected ? "Yes" : "No");
        });
    
    // Add button event handler
    vtClient->get_vt_button_event_dispatcher().add_listener(
        [](const isobus::VirtualTerminalClient::VTKeyEvent &event) {
            ESP_LOGI(TAG, "VT Button Event - Object: %d, Key: %d, Event: %d", 
                     event.objectID, event.keyNumber, (int)event.keyEvent);
            
            // Handle button presses (Plus_Button = 6000, Minus_Button = 6001)
            if (event.keyEvent == isobus::VirtualTerminalClient::KeyActivationCode::ButtonUnlatchedOrReleased) {
                switch (event.objectID) {
                    case 6000: // Plus button
                        displayValue += 10;
                        ESP_LOGI(TAG, "Plus button pressed - value now: %lu", displayValue);
                        blink_led(1, 50);
                        break;
                    case 6001: // Minus button
                        if (displayValue >= 10) displayValue -= 10;
                        ESP_LOGI(TAG, "Minus button pressed - value now: %lu", displayValue);
                        blink_led(2, 50);
                        break;
                }
            }
        });
    
    // Add soft key event handler
    vtClient->get_vt_soft_key_event_dispatcher().add_listener(
        [](const isobus::VirtualTerminalClient::VTKeyEvent &event) {
            ESP_LOGI(TAG, "VT Soft Key Event - Object: %d, Key: %d, Event: %d", 
                     event.objectID, event.keyNumber, (int)event.keyEvent);
            
            // Handle soft key presses (alarm_SoftKey = 5000, acknowledgeAlarm_SoftKey = 5001)
            if (event.keyEvent == isobus::VirtualTerminalClient::KeyActivationCode::ButtonUnlatchedOrReleased) {
                switch (event.objectID) {
                    case 5000: // Alarm soft key
                        ESP_LOGI(TAG, "Alarm soft key pressed - resetting counter");
                        displayValue = 0;
                        blink_led(3, 100);
                        break;
                    case 5001: // Acknowledge alarm soft key
                        ESP_LOGI(TAG, "Acknowledge soft key pressed");
                        blink_led(1, 200);
                        break;
                }
            }
        });
    
    #endif
    
    // Create task for updating CAN hardware (since threads are disabled)
    // Lower priority (1) to ensure idle task can run
    // Increased stack size to 16KB for VT message processing
    xTaskCreate(can_update_task, "CAN_update", 16384, NULL, 1, NULL);
    
    // Main loop
    uint32_t lastStatusTime = 0;
    while (1) {
        uint32_t currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        // Print status every 5 seconds
        if (currentTime - lastStatusTime >= 5000) {
            lastStatusTime = currentTime;
            
            // Check address claim status
            if (internalECU->get_address_valid()) {
                ESP_LOGI(TAG, "Address claimed successfully at 0x%02X", internalECU->get_address());
                blink_led(1, 200); // Single blink when operating normally
            } else {
                ESP_LOGW(TAG, "Address claim in progress...");
                blink_led(2, 200); // Double blink during address claim
            }
            
            // Get TWAI status
            twai_status_info_t status;
            twai_get_status_info(&status);
            ESP_LOGI(TAG, "TWAI state: %d, TX errors: %ld, RX errors: %ld, Bus: %s", 
                     status.state, 
                     status.tx_error_counter, 
                     status.rx_error_counter,
                     status.bus_error_count > 0 ? "ERROR" : "OK");
            
            // Report VT client status
            if (vtClient) {
                ESP_LOGI(TAG, "VT State: %s, Connected: %s", 
                         vtClient->get_is_initialized() ? "Initialized" : "Not initialized",
                         vtClient->get_is_connected() ? "Yes" : "No");
                
                // Check if we have a VT partner
                if (partnerVT && partnerVT->get_address_valid()) {
                    ESP_LOGI(TAG, "VT partner at address: 0x%02X", partnerVT->get_address());
                } else {
                    ESP_LOGI(TAG, "No VT partner found");
                }
                
                // Check for VT connection success
                if (vtClient->get_is_connected()) {
                    if (!vtConnected) {
                        ESP_LOGI(TAG, "VT Connected! Object pool uploaded successfully");
                        blink_led(5, 100); // Celebrate with 5 fast blinks
                        vtConnected = true;
                        
                        // Try to activate the main data mask (working set ID 0, mask ID depends on pool)
                        // For the example pool, try common mask IDs
                        if (vtClient->send_change_active_mask(0, 1000)) {
                            ESP_LOGI(TAG, "Sent change active mask command for mask 1000");
                        }
                    }
                    
                    // Update the display value periodically
                    static uint32_t lastUpdateTime = 0;
                    if (currentTime - lastUpdateTime >= 1000) { // Update every second
                        lastUpdateTime = currentTime;
                        displayValue++;
                        
                        // Update the VT number variable (ButtonExampleNumber_VarNum = 21000)
                        // The example pool has an offset of -214748364, so we add that to our value
                        uint32_t vtValue = displayValue + 214748364;
                        if (vtClient->send_change_numeric_value(21000, vtValue)) {
                            ESP_LOGI(TAG, "Updated VT display value to %lu", displayValue);
                        } else {
                            ESP_LOGW(TAG, "Failed to update VT display value");
                        }
                    }
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}