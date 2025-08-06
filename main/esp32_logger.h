#ifndef ESP32_LOGGER_H
#define ESP32_LOGGER_H

#include "isobus/isobus/can_stack_logger.hpp"
#include "esp_log.h"

namespace isobus
{
    class ESP32Logger : public CANStackLogger
    {
    public:
        static void setup()
        {
            static ESP32Logger instance;
            CANStackLogger::set_can_stack_logger_sink(&instance);
            CANStackLogger::set_log_level(LoggingLevel::Debug);
        }

        void sink_CAN_stack_log(LoggingLevel level, const std::string &text) override
        {
            const char *TAG = "AgIsoStack";
            
            switch (level)
            {
                case LoggingLevel::Debug:
                    ESP_LOGD(TAG, "%s", text.c_str());
                    break;
                case LoggingLevel::Info:
                    ESP_LOGI(TAG, "%s", text.c_str());
                    break;
                case LoggingLevel::Warning:
                    ESP_LOGW(TAG, "%s", text.c_str());
                    break;
                case LoggingLevel::Error:
                    ESP_LOGE(TAG, "%s", text.c_str());
                    break;
                case LoggingLevel::Critical:
                    ESP_LOGE(TAG, "[CRITICAL] %s", text.c_str());
                    break;
            }
        }
    };
}

#endif // ESP32_LOGGER_H