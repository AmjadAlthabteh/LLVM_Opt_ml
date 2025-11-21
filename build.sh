#!/bin/bash

set -e

echo "================================"
echo "AI Debugger Build Script"
echo "================================"
echo ""

BUILD_TYPE=${1:-Release}
BUILD_DIR="build"
ENABLE_TESTS=${2:-ON}
ENABLE_EXAMPLES=${3:-ON}

echo "Configuration:"
echo "  Build Type: $BUILD_TYPE"
echo "  Tests: $ENABLE_TESTS"
echo "  Examples: $ENABLE_EXAMPLES"
echo ""

if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Running CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS="$ENABLE_TESTS" \
    -DBUILD_EXAMPLES="$ENABLE_EXAMPLES"

echo ""
echo "Building..."
cmake --build . --config "$BUILD_TYPE" -j$(nproc 2>/dev/null || echo 4)

if [ "$ENABLE_TESTS" = "ON" ]; then
    echo ""
    echo "Running tests..."
    ctest --output-on-failure
fi

echo ""
echo "================================"
echo "Build completed successfully!"
echo "================================"
echo ""
echo "Binaries location:"
echo "  Library: $BUILD_DIR/libai_debugger.a"
if [ "$ENABLE_EXAMPLES" = "ON" ]; then
    echo "  Examples: $BUILD_DIR/examples/"
fi
echo ""
echo "To install system-wide:"
echo "  cd $BUILD_DIR && sudo cmake --install ."
echo ""
