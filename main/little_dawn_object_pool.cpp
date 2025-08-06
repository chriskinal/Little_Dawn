#include "little_dawn_object_pool.h"
#include "esp_log.h"
#include <vector>
#include <cstring>

static const char* TAG = "LittleDawnPool";

// Object IDs for our custom pool
#define LITTLE_DAWN_WORKING_SET      0
#define LITTLE_DAWN_MAIN_MASK        1000
#define LITTLE_DAWN_TITLE_STRING     3000
#define LITTLE_DAWN_VALUE_STRING     3001
#define LITTLE_DAWN_STATUS_STRING    3002
#define LITTLE_DAWN_VALUE_NUMBER     4000
#define LITTLE_DAWN_FONT_NORMAL      5000
#define LITTLE_DAWN_FONT_LARGE       5001

std::vector<uint8_t> createLittleDawnObjectPool()
{
    std::vector<uint8_t> objectPool;
    
    // Working Set Object (ID=0) - 18 bytes + 2 bytes for language code
    objectPool.push_back(0x00); // Object ID low byte
    objectPool.push_back(0x00); // Object ID high byte  
    objectPool.push_back(0x00); // Type: Working Set
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0x01); // Selectable
    objectPool.push_back(0xE8); // Active mask low byte (1000)
    objectPool.push_back(0x03); // Active mask high byte
    objectPool.push_back(0x00); // Number of object references
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x01); // Number of language codes
    // Working set designator (8 bytes) - "LTLDAWN "
    objectPool.push_back(0x4C); // L
    objectPool.push_back(0x54); // T
    objectPool.push_back(0x4C); // L
    objectPool.push_back(0x44); // D
    objectPool.push_back(0x41); // A
    objectPool.push_back(0x57); // W
    objectPool.push_back(0x4E); // N
    objectPool.push_back(0x20); // space
    // Language code "en" (2 bytes)
    objectPool.push_back(0x65); // 'e'
    objectPool.push_back(0x6E); // 'n'
    
    // Font Attributes Normal (ID=5000) - 7 bytes
    objectPool.push_back(0x88); // Object ID low byte (5000)
    objectPool.push_back(0x13); // Object ID high byte
    objectPool.push_back(0x17); // Type: Font Attributes (23)
    objectPool.push_back(0x0F); // Font color (white)
    objectPool.push_back(0x08); // Font size (8x12)
    objectPool.push_back(0x00); // Font type (Latin Basic)
    objectPool.push_back(0x00); // Font style (Normal)
    
    // Font Attributes Large (ID=5001) - 7 bytes
    objectPool.push_back(0x89); // Object ID low byte (5001)
    objectPool.push_back(0x13); // Object ID high byte
    objectPool.push_back(0x17); // Type: Font Attributes (23)
    objectPool.push_back(0x0F); // Font color (white)
    objectPool.push_back(0x10); // Font size (16x24)
    objectPool.push_back(0x00); // Font type (Latin Basic)
    objectPool.push_back(0x02); // Font style (Bold)
    
    // Data Mask Object (ID=1000) - 8 bytes + (6 bytes * 4 object references)
    objectPool.push_back(0xE8); // Object ID low byte (1000)
    objectPool.push_back(0x03); // Object ID high byte
    objectPool.push_back(0x01); // Type: Data Mask
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0xFF); // Soft key mask low byte (none)
    objectPool.push_back(0xFF); // Soft key mask high byte
    objectPool.push_back(0x04); // Number of object references
    objectPool.push_back(0x00); // Number of macro references
    
    // Object reference: Title String at position (0,10)
    objectPool.push_back(0xB8); // Object ID low byte (3000)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x00); // X position low byte
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x0A); // Y position low byte (10)
    objectPool.push_back(0x00); // Y position high byte
    
    // Object reference: Value String at position (10,60)
    objectPool.push_back(0xB9); // Object ID low byte (3001)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x0A); // X position low byte (10)
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x3C); // Y position low byte (60)
    objectPool.push_back(0x00); // Y position high byte
    
    // Object reference: Value Number at position (110,60)
    objectPool.push_back(0xA0); // Object ID low byte (4000)
    objectPool.push_back(0x0F); // Object ID high byte
    objectPool.push_back(0x6E); // X position low byte (110)
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x3C); // Y position low byte (60)
    objectPool.push_back(0x00); // Y position high byte
    
    // Object reference: Status String at position (0,150)
    objectPool.push_back(0xBA); // Object ID low byte (3002)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x00); // X position low byte
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x96); // Y position low byte (150)
    objectPool.push_back(0x00); // Y position high byte
    
    // Title Output String (ID=3000) - 17 bytes + string length
    const char* title = "Little Dawn Bridge";
    size_t titleLen = strlen(title);
    objectPool.push_back(0xB8); // Object ID low byte (3000)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x0B); // Type: Output String (11)
    objectPool.push_back(0xF0); // Width low byte (240)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x20); // Height low byte (32)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0x89); // Font attributes low byte (5001)
    objectPool.push_back(0x13); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x01); // Justification (center)
    objectPool.push_back(titleLen); // String length low byte
    objectPool.push_back(0x00); // String length high byte
    // Add string
    for (size_t i = 0; i < titleLen; i++) {
        objectPool.push_back(title[i]);
    }
    
    // Value Label Output String (ID=3001) - 17 bytes + string length
    const char* valueLabel = "Value:";
    size_t valueLabelLen = strlen(valueLabel);
    objectPool.push_back(0xB9); // Object ID low byte (3001)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x0B); // Type: Output String (11)
    objectPool.push_back(0x78); // Width low byte (120)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x18); // Height low byte (24)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0x88); // Font attributes low byte (5000)
    objectPool.push_back(0x13); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x00); // Justification (left)
    objectPool.push_back(valueLabelLen); // String length low byte
    objectPool.push_back(0x00); // String length high byte
    // Add string
    for (size_t i = 0; i < valueLabelLen; i++) {
        objectPool.push_back(valueLabel[i]);
    }
    
    // Status Output String (ID=3002) - 17 bytes + string length
    const char* status = "Waiting for New Dawn...";
    size_t statusLen = strlen(status);
    objectPool.push_back(0xBA); // Object ID low byte (3002)
    objectPool.push_back(0x0B); // Object ID high byte
    objectPool.push_back(0x0B); // Type: Output String (11)
    objectPool.push_back(0xF0); // Width low byte (240)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x18); // Height low byte (24)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0x88); // Font attributes low byte (5000)
    objectPool.push_back(0x13); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x01); // Justification (center)
    objectPool.push_back(statusLen); // String length low byte
    objectPool.push_back(0x00); // String length high byte
    // Add string
    for (size_t i = 0; i < statusLen; i++) {
        objectPool.push_back(status[i]);
    }
    
    // Value Output Number (ID=4000) - 29 bytes
    objectPool.push_back(0xA0); // Object ID low byte (4000)
    objectPool.push_back(0x0F); // Object ID high byte
    objectPool.push_back(0x0C); // Type: Output Number (12)
    objectPool.push_back(0x78); // Width low byte (120)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x18); // Height low byte (24)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0x88); // Font attributes low byte (5000)
    objectPool.push_back(0x13); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x00); // Value low byte
    objectPool.push_back(0x00); // Value byte 2
    objectPool.push_back(0x00); // Value byte 3
    objectPool.push_back(0x00); // Value high byte
    objectPool.push_back(0x00); // Offset low byte
    objectPool.push_back(0x00); // Offset byte 2
    objectPool.push_back(0x00); // Offset byte 3
    objectPool.push_back(0x00); // Offset high byte
    objectPool.push_back(0x00); // Scale factor (1.0 as IEEE 754 float)
    objectPool.push_back(0x00); // Scale factor byte 2
    objectPool.push_back(0x80); // Scale factor byte 3
    objectPool.push_back(0x3F); // Scale factor high byte
    objectPool.push_back(0x01); // Number of decimals
    objectPool.push_back(0x00); // Format (not exponential)
    objectPool.push_back(0x02); // Justification (right)
    
    ESP_LOGI(TAG, "Created Little Dawn object pool: %d bytes", objectPool.size());
    
    return objectPool;
}