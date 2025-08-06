#include "simple_object_pool.h"
#include "vt_object_ids.h"
#include <cstring>
#include "esp_log.h"

static const char* TAG = "ObjectPool";

std::vector<uint8_t> createSimpleObjectPool()
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
    // Working set designator (8 bytes) - "DAWN    "
    objectPool.push_back(0x44); // D
    objectPool.push_back(0x41); // A
    objectPool.push_back(0x57); // W
    objectPool.push_back(0x4E); // N
    objectPool.push_back(0x20); // space
    objectPool.push_back(0x20); // space
    objectPool.push_back(0x20); // space
    objectPool.push_back(0x20); // space
    // Language code "en" (2 bytes)
    objectPool.push_back(0x65); // 'e'
    objectPool.push_back(0x6E); // 'n'
    
    // Data Mask Object (ID=1000) - 8 bytes + (6 bytes * 1 object reference)
    objectPool.push_back(0xE8); // Object ID low byte (1000)
    objectPool.push_back(0x03); // Object ID high byte
    objectPool.push_back(0x01); // Type: Data Mask
    objectPool.push_back(0xFF); // Background color (white)
    objectPool.push_back(0xFF); // Soft key mask low byte (none)
    objectPool.push_back(0xFF); // Soft key mask high byte
    objectPool.push_back(0x01); // Number of object references
    objectPool.push_back(0x00); // Number of macro references
    // Object reference: Output String at position (10,10)
    objectPool.push_back(0xD0); // Object ID low byte (2000)
    objectPool.push_back(0x07); // Object ID high byte
    objectPool.push_back(0x0A); // X position low byte (10)
    objectPool.push_back(0x00); // X position high byte
    objectPool.push_back(0x0A); // Y position low byte (10)
    objectPool.push_back(0x00); // Y position high byte
    
    // Output String - Title (ID=2000) - 17 bytes + string length
    objectPool.push_back(0xD0); // Object ID low byte (2000)
    objectPool.push_back(0x07); // Object ID high byte
    objectPool.push_back(0x0B); // Type: Output String (11)
    objectPool.push_back(0xDC); // Width low byte (220)
    objectPool.push_back(0x00); // Width high byte
    objectPool.push_back(0x28); // Height low byte (40)
    objectPool.push_back(0x00); // Height high byte
    objectPool.push_back(0xFF); // Background color (white)
    objectPool.push_back(0xE8); // Font attributes low byte (1000)
    objectPool.push_back(0x03); // Font attributes high byte
    objectPool.push_back(0x00); // Number of macro references
    objectPool.push_back(0x00); // Options
    objectPool.push_back(0xFF); // Variable reference low byte (none)
    objectPool.push_back(0xFF); // Variable reference high byte
    objectPool.push_back(0x01); // Justification (center)
    objectPool.push_back(0x09); // String length low byte (9 chars)
    objectPool.push_back(0x00); // String length high byte
    // "Hello VT!" (9 bytes)
    objectPool.push_back(0x48); // H
    objectPool.push_back(0x65); // e
    objectPool.push_back(0x6C); // l
    objectPool.push_back(0x6C); // l
    objectPool.push_back(0x6F); // o
    objectPool.push_back(0x20); // space
    objectPool.push_back(0x56); // V
    objectPool.push_back(0x54); // T
    objectPool.push_back(0x21); // !
    
    // Font Attributes (ID=1000) - 7 bytes
    objectPool.push_back(0xE8); // Object ID low byte (1000)
    objectPool.push_back(0x03); // Object ID high byte
    objectPool.push_back(0x17); // Type: Font Attributes (23)
    objectPool.push_back(0x00); // Font color (black)
    objectPool.push_back(0x08); // Font size (8)
    objectPool.push_back(0x00); // Font type (Latin Basic)
    objectPool.push_back(0x00); // Font style (Normal)
    
    ESP_LOGI(TAG, "Created simple object pool: %d bytes", objectPool.size());
    
    return objectPool;
}