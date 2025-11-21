#!/bin/bash

echo "================================="
echo "AI Debugger - Quick Test"
echo "================================="
echo ""

# Test 1: Simple standalone demo
echo "Test 1: Building simple demo..."
echo "--------------------------------"

g++ -std=c++17 demo/simple_test.cpp -o demo_test 2>&1

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful"
    echo ""
    echo "Running demo..."
    echo ""
    ./demo_test
    echo ""
else
    echo "✗ Compilation failed"
    echo "Trying with explicit includes..."
    g++ -std=c++17 -I./include demo/simple_test.cpp -o demo_test 2>&1
    if [ $? -eq 0 ]; then
        ./demo_test
    fi
fi

echo ""
echo "================================="
echo "Test 2: Checking project files"
echo "================================="
echo ""

echo "Header files:"
find include -name "*.h" -type f | wc -l
echo ""

echo "Source files:"
find src -name "*.cpp" -type f | wc -l
echo ""

echo "Test files:"
find tests -name "*.cpp" -type f | wc -l
echo ""

echo "Example files:"
find examples -name "*.cpp" -type f | wc -l
echo ""

echo "================================="
echo "Test 3: CMake configuration check"
echo "================================="
echo ""

if command -v cmake &> /dev/null; then
    echo "✓ CMake found: $(cmake --version | head -1)"
    echo ""
    echo "Checking CMakeLists.txt..."
    if [ -f "CMakeLists.txt" ]; then
        echo "✓ Main CMakeLists.txt exists"
        grep "project(AIDebugger" CMakeLists.txt > /dev/null && echo "✓ Project configured"
        grep "add_library(ai_debugger" CMakeLists.txt > /dev/null && echo "✓ Library target defined"
    fi
else
    echo "⚠ CMake not found (needed for full build)"
fi

echo ""
echo "================================="
echo "Test 4: Build system check"
echo "================================="
echo ""

if [ -f "Makefile" ]; then
    echo "✓ Makefile exists"
fi

if [ -f "build.sh" ]; then
    echo "✓ Build script exists"
    chmod +x build.sh 2>/dev/null
fi

if [ -f "package.json" ]; then
    echo "✓ Package configuration exists"
fi

echo ""
echo "================================="
echo "Quick Test Complete"
echo "================================="
echo ""
echo "Next steps:"
echo "  1. Run full build: ./build.sh"
echo "  2. Run tests: cd build && ctest"
echo "  3. Try examples: ./build/examples/simple_example"
echo ""
