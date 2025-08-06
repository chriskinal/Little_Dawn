#ifndef LITTLE_DAWN_OBJECT_POOL_H
#define LITTLE_DAWN_OBJECT_POOL_H

#include <vector>
#include <cstdint>

// Object IDs that can be referenced from main code
#define LITTLE_DAWN_VALUE_NUMBER     4000
#define LITTLE_DAWN_STATUS_STRING    3002

// Create the Little Dawn custom object pool
std::vector<uint8_t> createLittleDawnObjectPool();

#endif // LITTLE_DAWN_OBJECT_POOL_H