// Manual binary pool for Little Dawn
// This creates a minimal ISOBUS object pool directly in binary format

#include <cstdint>
#include <vector>

// Object type codes
#define TYPE_WORKING_SET      0x00
#define TYPE_DATA_MASK        0x01
#define TYPE_SOFT_KEY_MASK    0x04
#define TYPE_BUTTON           0x06
#define TYPE_OUTPUT_STRING    0x0B
#define TYPE_OUTPUT_NUMBER    0x0C
#define TYPE_FONT_ATTRIBUTES  0x17
#define TYPE_LINE_ATTRIBUTES  0x18

// Object IDs
#define ID_WORKING_SET      1
#define ID_DATA_MASK        100
#define ID_STRING_TITLE     200
#define ID_STRING_VALUE     201
#define ID_NUMBER_VALUE     202
#define ID_FONT_8x16        300
#define ID_FONT_6x8         301

// Create binary pool data
std::vector<uint8_t> createLittleDawnPool() {
    std::vector<uint8_t> pool;
    
    // Working Set Object
    pool.push_back(TYPE_WORKING_SET);
    pool.push_back(ID_WORKING_SET & 0xFF);
    pool.push_back(ID_WORKING_SET >> 8);
    pool.push_back(0x01); // selectable
    pool.push_back(ID_DATA_MASK & 0xFF); // active mask ID low
    pool.push_back(ID_DATA_MASK >> 8);   // active mask ID high
    pool.push_back(0x00); // 0 object references
    pool.push_back(0x00);
    pool.push_back(0x00); // 0 macro references
    pool.push_back(0x00);
    pool.push_back(0x00); // 0 language codes
    pool.push_back(0x00);
    
    // Data Mask Object
    pool.push_back(TYPE_DATA_MASK);
    pool.push_back(ID_DATA_MASK & 0xFF);
    pool.push_back(ID_DATA_MASK >> 8);
    pool.push_back(0x01); // white background
    pool.push_back(0xFF); // soft key mask ID (none)
    pool.push_back(0xFF);
    pool.push_back(0x03); // 3 object references
    pool.push_back(0x00);
    // Reference to title string
    pool.push_back(ID_STRING_TITLE & 0xFF);
    pool.push_back(ID_STRING_TITLE >> 8);
    pool.push_back(50);  // x position
    pool.push_back(0);
    pool.push_back(10); // y position
    pool.push_back(0);
    // Reference to value string
    pool.push_back(ID_STRING_VALUE & 0xFF);
    pool.push_back(ID_STRING_VALUE >> 8);
    pool.push_back(50);  // x position
    pool.push_back(0);
    pool.push_back(40); // y position
    pool.push_back(0);
    // Reference to number value
    pool.push_back(ID_NUMBER_VALUE & 0xFF);
    pool.push_back(ID_NUMBER_VALUE >> 8);
    pool.push_back(50);  // x position
    pool.push_back(0);
    pool.push_back(60); // y position
    pool.push_back(0);
    pool.push_back(0x00); // 0 macro references
    pool.push_back(0x00);
    
    // Font Attributes 8x16
    pool.push_back(TYPE_FONT_ATTRIBUTES);
    pool.push_back(ID_FONT_8x16 & 0xFF);
    pool.push_back(ID_FONT_8x16 >> 8);
    pool.push_back(0x00); // black color
    pool.push_back(0x08); // 8x16 size
    pool.push_back(0x00); // latin1
    pool.push_back(0x00); // normal style
    pool.push_back(0x00);
    
    // Font Attributes 6x8
    pool.push_back(TYPE_FONT_ATTRIBUTES);
    pool.push_back(ID_FONT_6x8 & 0xFF);
    pool.push_back(ID_FONT_6x8 >> 8);
    pool.push_back(0x00); // black color
    pool.push_back(0x00); // 6x8 size
    pool.push_back(0x00); // latin1
    pool.push_back(0x00); // normal style
    pool.push_back(0x00);
    
    // Output String - Title
    pool.push_back(TYPE_OUTPUT_STRING);
    pool.push_back(ID_STRING_TITLE & 0xFF);
    pool.push_back(ID_STRING_TITLE >> 8);
    pool.push_back(88);   // width low
    pool.push_back(0);    // width high
    pool.push_back(16);   // height low
    pool.push_back(0);    // height high
    pool.push_back(0x01); // white background
    pool.push_back(ID_FONT_8x16 & 0xFF); // font ID low
    pool.push_back(ID_FONT_8x16 >> 8);   // font ID high
    pool.push_back(0x01); // transparent
    pool.push_back(11);   // string length
    pool.push_back(0);    // reserved
    // String value: "Little Dawn"
    pool.push_back('L');
    pool.push_back('i');
    pool.push_back('t');
    pool.push_back('t');
    pool.push_back('l');
    pool.push_back('e');
    pool.push_back(' ');
    pool.push_back('D');
    pool.push_back('a');
    pool.push_back('w');
    pool.push_back('n');
    pool.push_back(0x00); // null terminator
    
    // Output String - Label
    pool.push_back(TYPE_OUTPUT_STRING);
    pool.push_back(ID_STRING_VALUE & 0xFF);
    pool.push_back(ID_STRING_VALUE >> 8);
    pool.push_back(48);   // width low
    pool.push_back(0);    // width high
    pool.push_back(8);    // height low
    pool.push_back(0);    // height high
    pool.push_back(0x01); // white background
    pool.push_back(ID_FONT_6x8 & 0xFF); // font ID low
    pool.push_back(ID_FONT_6x8 >> 8);   // font ID high
    pool.push_back(0x01); // transparent
    pool.push_back(6);    // string length
    pool.push_back(0);    // reserved
    // String value: "Value:"
    pool.push_back('V');
    pool.push_back('a');
    pool.push_back('l');
    pool.push_back('u');
    pool.push_back('e');
    pool.push_back(':');
    pool.push_back(0x00); // null terminator
    
    // Output Number - Value display
    pool.push_back(TYPE_OUTPUT_NUMBER);
    pool.push_back(ID_NUMBER_VALUE & 0xFF);
    pool.push_back(ID_NUMBER_VALUE >> 8);
    pool.push_back(48);   // width low
    pool.push_back(0);    // width high
    pool.push_back(8);    // height low
    pool.push_back(0);    // height high
    pool.push_back(0x01); // white background
    pool.push_back(ID_FONT_6x8 & 0xFF); // font ID low
    pool.push_back(ID_FONT_6x8 >> 8);   // font ID high
    pool.push_back(0x01); // transparent, right justified
    pool.push_back(0x00); // offset low
    pool.push_back(0x00);
    pool.push_back(0x00);
    pool.push_back(0x00); // offset high
    pool.push_back(0x01); // scale = 1.0
    pool.push_back(0x00);
    pool.push_back(0x00);
    pool.push_back(0x00);
    pool.push_back(0x00); // decimals
    pool.push_back(0x00); // format
    pool.push_back(0x00); // reserved
    pool.push_back(0x00); // initial value
    pool.push_back(0x00);
    pool.push_back(0x00);
    pool.push_back(0x00);
    
    return pool;
}

// Export the pool data
extern "C" {
    const std::vector<uint8_t> manual_pool_data = createLittleDawnPool();
    const uint8_t* manual_pool_start = manual_pool_data.data();
    const uint8_t* manual_pool_end = manual_pool_data.data() + manual_pool_data.size();
}