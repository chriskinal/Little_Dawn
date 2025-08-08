#!/bin/bash
# Clean build script for Little Dawn

echo "Cleaning build directory..."
rm -rf build

echo "Sourcing ESP-IDF environment..."
source /Users/chris/esp/v5.5/esp-idf/export.sh

echo "Running fullclean..."
idf.py fullclean

echo "Building project..."
idf.py build

echo "Build complete!"