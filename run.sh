#!/bin/bash

# Exit immediately if any command fails
set -e

echo "🔨 Building Qt Monopoly..."

# 1. Create a dedicated build directory to keep the source tree clean
mkdir -p build
cd build

# 2. Configure the CMake project (points to the parent directory where CMakeLists.txt is)
cmake ..

# 3. Compile the project using all available CPU cores
cmake --build . -j$(nproc)

echo "✅ Build successful!"
echo "🚀 Launching game..."

# 4. Execute the compiled binary
./QtMonopoly