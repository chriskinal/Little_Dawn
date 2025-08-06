#include "working_object_pool.h"
#include "esp_log.h"
#include <vector>
#include <cstring>

static const char* TAG = "WorkingPool";

std::vector<uint8_t> createWorkingObjectPool()
{
    std::vector<uint8_t> objectPool;
    
    // Font Attributes (ID=23000) - 7 bytes (define first since it's referenced)
    objectPool.push_back(0xD8); // Object ID low byte (23000 = 0x59D8)
    objectPool.push_back(0x59); // Object ID high byte
    objectPool.push_back(0x17); // Type: Font Attributes (23)
    objectPool.push_back(0x00); // Font color (black)
    objectPool.push_back(0x08); // Font size (8x12)
    objectPool.push_back(0x00); // Font type (Latin Basic)
    objectPool.push_back(0x00); // Font style (Normal)
    
    // Output String (ID=11000) - 17 bytes + string length
    const char* text = "Little Dawn OK!";
    size_t textLen = strlen(text);
    objectPool.push_back(0xF8); // Object ID low byte (11000 = 0x2AF8)
    objectPool.push_back(0x2A); // Object ID high byte
    objectPool.push_back(0x0B); // Type: Output String (11)
    objectPool.push_back(0xF0); // Width low byte (240)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x20); // Height low byte (32)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0xFF); // Background color (white)
    objectPool.push_back(0xD8); // Font attributes low byte (23000)
    objectPool.push_back(0x59); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x01); // Justification (center)
    objectPool.push_back(textLen); // String length low byte
    objectPool.push_back(0x00); // String length high byte
    // Add string
    for (size_t i = 0; i < textLen; i++) {
        objectPool.push_back(text[i]);
    }
    
    // Data Mask Object (ID=1000) - 8 bytes + (6 bytes * 1 object reference)
    objectPool.push_back(0xE8); // Object ID low byte (1000)
    objectPool.push_back(0x03); // Object ID high byte
    objectPool.push_back(0x01); // Type: Data Mask
    objectPool.push_back(0xFF); // Background color (white)
    objectPool.push_back(0xFF); // Soft key mask low byte (none)
    objectPool.push_back(0xFF); // Soft key mask high byte
    objectPool.push_back(0x01); // Number of object references (1)
    objectPool.push_back(0x00); // Number of macro references
    // Object reference: Output String at position (0,100)
    objectPool.push_back(0xF8); // Object ID low byte (11000)
    objectPool.push_back(0x2A); // Object ID high byte
    objectPool.push_back(0x00); // X position low byte
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x64); // Y position low byte (100)
    objectPool.push_back(0x00); // Y position high byte
    
    // Working Set Object (ID=0) - Must be last
    objectPool.push_back(0x00); // Object ID low byte
    objectPool.push_back(0x00); // Object ID high byte  
    objectPool.push_back(0x00); // Type: Working Set
    objectPool.push_back(0xFF); // Background color (white)
    objectPool.push_back(0x01); // Selectable
    objectPool.push_back(0xE8); // Active mask low byte (1000)
    objectPool.push_back(0x03); // Active mask high byte
    objectPool.push_back(0x00); // Number of object references
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x01); // Number of language codes
    // Working set designator (8 bytes) - must be exactly 8 bytes
    objectPool.push_back('L');
    objectPool.push_back('T');
    objectPool.push_back('L');
    objectPool.push_back('D');
    objectPool.push_back('A');
    objectPool.push_back('W');
    objectPool.push_back('N');
    objectPool.push_back(' ');
    // Language code "en" (2 bytes)
    objectPool.push_back('e');
    objectPool.push_back('n');
    
    ESP_LOGI(TAG, "Created working object pool: %d bytes", objectPool.size());
    ESP_LOGI(TAG, "Objects: Font(23000), String(11000), Mask(1000), WorkingSet(0)");
    
    return objectPool;
}