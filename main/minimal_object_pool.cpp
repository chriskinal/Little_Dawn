#include "minimal_object_pool.h"
#include "esp_log.h"
#include <vector>
#include <cstring>

static const char* TAG = "MinimalPool";

std::vector<uint8_t> createMinimalObjectPool()
{
    std::vector<uint8_t> objectPool;
    
    // Data Mask Object (ID=1000) - Define this first since it's referenced by Working Set
    objectPool.push_back(0xE8); // Object ID low byte (1000)
    objectPool.push_back(0x03); // Object ID high byte
    objectPool.push_back(0x01); // Type: Data Mask
    objectPool.push_back(0x00); // Background color (black)
    objectPool.push_back(0xFF); // Soft key mask low byte (none)
    objectPool.push_back(0xFF); // Soft key mask high byte
    objectPool.push_back(0x00); // Number of object references (0 for now)
    objectPool.push_back(0x00); // Number of macro references
    
    // Working Set Object (ID=0) - Must be last
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
    
    ESP_LOGI(TAG, "Created minimal object pool: %d bytes", objectPool.size());
    ESP_LOGI(TAG, "Object order: Data Mask (1000), Working Set (0)");
    
    return objectPool;
}