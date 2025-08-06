#ifndef SIMPLE_OBJECT_POOL_H
#define SIMPLE_OBJECT_POOL_H

#include <vector>
#include <cstdint>

// Create a simple VT object pool with "Hello VT!" text
std::vector<uint8_t> createSimpleObjectPool();

#endif // SIMPLE_OBJECT_POOL_H