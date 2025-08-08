#!/bin/bash
# Quick incremental build script for Little Dawn

echo "Sourcing ESP-IDF environment..."
source /Users/chris/esp/v5.5/esp-idf/export.sh

echo "Reconfiguring build system..."
idf.py reconfigure

echo "Building project..."
idf.py build

echo "Build complete!"