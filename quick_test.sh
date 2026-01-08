#!/bin/bash

echo "================================="
echo "AI Debugger - Quick Test"
echo "================================="
echo ""

echo "Test 0: Compiler check..."
echo "--------------------------"
if [ -f "./compiler_check.sh" ]; then
    chmod +x compiler_check.sh 2>/dev/null
    if ! ./compiler_check.sh; then
        echo "[WARN] Compiler check failed. Demo build may fail."
    fi
else
    echo "[WARN] compiler_check.sh not found. Skipping compiler check."
fi
echo ""

echo "Test 1: Building simple demo..."
echo "--------------------------------"

g++ -std=c++17 demo/simple_test.cpp -o demo_test 2>&1

if [ $? -eq 0 ]; then
    echo "[OK] Compilation successful"
    echo ""
    echo "Running demo..."
    echo ""
    ./demo_test
    echo ""
else
    echo "[FAIL] Compilation failed"
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

if command -v cmake > /dev/null 2>&1; then
    echo "[OK] CMake found: $(cmake --version | head -1)"
    echo ""
    echo "Checking CMakeLists.txt..."
    if [ -f "CMakeLists.txt" ]; then
        echo "[OK] Main CMakeLists.txt exists"
        grep "project(AIDebugger" CMakeLists.txt > /dev/null && echo "[OK] Project configured"
        grep "add_library(ai_debugger" CMakeLists.txt > /dev/null && echo "[OK] Library target defined"
    fi
else
    echo "[WARN] CMake not found (needed for full build)"
fi

echo ""
echo "================================="
echo "Test 4: Build system check"
echo "================================="
echo ""

if [ -f "Makefile" ]; then
    echo "[OK] Makefile exists"
fi

if [ -f "build.sh" ]; then
    echo "[OK] Build script exists"
    chmod +x build.sh 2>/dev/null
fi

if [ -f "package.json" ]; then
    echo "[OK] Package configuration exists"
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
