#!/bin/bash
# Quick build script for SQLsmith using vcpkg

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🚀 SQLsmith Build Script with vcpkg${NC}\n"

# Check if vcpkg exists
VCPKG_ROOT="${VCPKG_ROOT:-$HOME/vcpkg}"

if [ ! -f "$VCPKG_ROOT/vcpkg" ]; then
    echo -e "${RED}❌ vcpkg not found at $VCPKG_ROOT${NC}"
    echo -e "\n${YELLOW}To install vcpkg:${NC}"
    echo "  git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT"
    echo "  $VCPKG_ROOT/bootstrap-vcpkg.sh"
    exit 1
fi

echo -e "${GREEN}✅ Found vcpkg at: $VCPKG_ROOT${NC}\n"

# Toolchain file path
TOOLCHAIN="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

# Clean build directory if requested
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}🧹 Cleaning build directory...${NC}"
    rm -rf build
    echo -e "${GREEN}✅ Clean complete${NC}\n"
fi

# Configure
echo -e "${YELLOW}⚙️  Configuring with CMake...${NC}"
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo -e "\n${GREEN}✅ Configuration complete${NC}\n"

# Build
echo -e "${YELLOW}🔨 Building SQLsmith...${NC}"
cmake --build build -j$(nproc)

echo -e "\n${GREEN}✅ Build complete!${NC}\n"

# Show result
if [ -f "build/sqlsmith" ]; then
    echo -e "${GREEN}🎉 SQLsmith built successfully!${NC}"
    echo -e "\nTo run:"
    echo -e "  ${YELLOW}./build/sqlsmith --help${NC}\n"
else
    echo -e "${RED}❌ Build failed - executable not found${NC}"
    exit 1
fi
