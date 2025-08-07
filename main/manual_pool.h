// Manual binary pool for Little Dawn
#ifndef MANUAL_POOL_H
#define MANUAL_POOL_H

// This is a manually created minimal ISOBUS object pool
// Structure: Working Set -> Data Mask -> 2 Output Strings + 1 Output Number

static const unsigned char manual_pool[] = {
    // Working Set (ID=1)
    0x00,                   // Type: Working Set
    0x01, 0x00,            // ID: 1
    0x01,                   // Selectable: yes
    0x64, 0x00,            // Active mask ID: 100
    0x00, 0x00,            // 0 object references
    0x00, 0x00,            // 0 macro references
    0x00, 0x00,            // 0 language codes
    
    // Data Mask (ID=100)
    0x01,                   // Type: Data Mask
    0x64, 0x00,            // ID: 100
    0x01,                   // Background: white
    0xFF, 0xFF,            // No soft key mask
    0x03, 0x00,            // 3 object references
    // Reference 1: Title string at (50,10)
    0xC8, 0x00,            // Object ID: 200
    0x32, 0x00,            // X: 50
    0x0A, 0x00,            // Y: 10
    // Reference 2: Label string at (50,40)
    0xC9, 0x00,            // Object ID: 201
    0x32, 0x00,            // X: 50
    0x28, 0x00,            // Y: 40
    // Reference 3: Number at (80,40)
    0xCA, 0x00,            // Object ID: 202
    0x50, 0x00,            // X: 80
    0x28, 0x00,            // Y: 40
    0x00, 0x00,            // 0 macro references
    
    // Font Attributes 8x16 (ID=300)
    0x17,                   // Type: Font Attributes
    0x2C, 0x01,            // ID: 300
    0x00,                   // Color: black
    0x08,                   // Size: 8x16
    0x00,                   // Type: Latin1
    0x00,                   // Style: Normal
    0x00,                   // Reserved
    
    // Font Attributes 6x8 (ID=301)
    0x17,                   // Type: Font Attributes
    0x2D, 0x01,            // ID: 301
    0x00,                   // Color: black
    0x00,                   // Size: 6x8
    0x00,                   // Type: Latin1
    0x00,                   // Style: Normal
    0x00,                   // Reserved
    
    // Output String - Title (ID=200)
    0x0B,                   // Type: Output String
    0xC8, 0x00,            // ID: 200
    0x58, 0x00,            // Width: 88
    0x10, 0x00,            // Height: 16
    0x01,                   // Background: white
    0x2C, 0x01,            // Font ID: 300
    0x01,                   // Options: transparent
    0x0B, 0x00,            // Length: 11
    // String: "Little Dawn"
    'L', 'i', 't', 't', 'l', 'e', ' ', 'D', 'a', 'w', 'n', 0x00,
    
    // Output String - Label (ID=201)
    0x0B,                   // Type: Output String
    0xC9, 0x00,            // ID: 201
    0x30, 0x00,            // Width: 48
    0x08, 0x00,            // Height: 8
    0x01,                   // Background: white
    0x2D, 0x01,            // Font ID: 301
    0x01,                   // Options: transparent
    0x06, 0x00,            // Length: 6
    // String: "Value:"
    'V', 'a', 'l', 'u', 'e', ':', 0x00,
    
    // Output Number (ID=202)
    0x0C,                   // Type: Output Number
    0xCA, 0x00,            // ID: 202
    0x30, 0x00,            // Width: 48
    0x08, 0x00,            // Height: 8
    0x01,                   // Background: white
    0x2D, 0x01,            // Font ID: 301
    0x01,                   // Options: transparent, right justified
    0x00, 0x00, 0x00, 0x00, // Offset: 0
    0x01, 0x00, 0x00, 0x00, // Scale: 1.0
    0x00,                   // Decimals: 0
    0x00,                   // Format: fixed
    0x00,                   // Reserved
    0x00, 0x00, 0x00, 0x00  // Initial value: 0
};

#define MANUAL_POOL_SIZE sizeof(manual_pool)

#endif // MANUAL_POOL_H