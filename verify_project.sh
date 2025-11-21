#!/bin/bash

echo "========================================"
echo "AI Debugger - Project Verification"
echo "========================================"
echo ""

PASS=0
FAIL=0

check() {
    if [ $? -eq 0 ]; then
        echo "  ✓ $1"
        ((PASS++))
    else
        echo "  ✗ $1"
        ((FAIL++))
    fi
}

echo "1. Project Structure"
echo "--------------------"

[ -d "include/ai_debugger" ]
check "Headers directory exists"

[ -d "src" ]
check "Source directory exists"

[ -d "tests" ]
check "Tests directory exists"

[ -d "examples" ]
check "Examples directory exists"

[ -d "demo" ]
check "Demo directory exists"

echo ""
echo "2. Core Headers"
echo "---------------"

[ -f "include/ai_debugger/AIDebugger.h" ]
check "AIDebugger.h"

[ -f "include/ai_debugger/StackTraceParser.h" ]
check "StackTraceParser.h"

[ -f "include/ai_debugger/RootCausePredictor.h" ]
check "RootCausePredictor.h"

[ -f "include/ai_debugger/FixSuggester.h" ]
check "FixSuggester.h"

echo ""
echo "3. Implementation Files"
echo "-----------------------"

[ -f "src/AIDebugger.cpp" ]
check "AIDebugger.cpp"

[ -f "src/StackTraceParser.cpp" ]
check "StackTraceParser.cpp"

[ -f "src/RootCausePredictor.cpp" ]
check "RootCausePredictor.cpp"

echo ""
echo "4. Build System"
echo "---------------"

[ -f "CMakeLists.txt" ]
check "Main CMakeLists.txt"

[ -f "Makefile" ]
check "Makefile"

[ -f "build.sh" ]
check "Build script (Linux)"

[ -f "build.bat" ]
check "Build script (Windows)"

echo ""
echo "5. Documentation"
echo "----------------"

[ -f "README.md" ]
check "README.md"

[ -f "SIMPLE_README.md" ]
check "SIMPLE_README.md"

[ -f "INSTALL.md" ]
check "INSTALL.md"

[ -f "HOW_TO_TEST.md" ]
check "HOW_TO_TEST.md"

echo ""
echo "6. Examples & Demos"
echo "-------------------"

[ -f "examples/simple_example.cpp" ]
check "Simple example"

[ -f "examples/cli_tool.cpp" ]
check "CLI tool"

[ -f "demo/simple_test.cpp" ]
check "Standalone demo"

[ -f "demo/sample_crash.log" ]
check "Sample crash log"

echo ""
echo "7. Test Suite"
echo "-------------"

[ -f "tests/test_stack_trace_parser.cpp" ]
check "Parser tests"

[ -f "tests/test_root_cause_predictor.cpp" ]
check "Predictor tests"

[ -f "tests/test_integration.cpp" ]
check "Integration tests"

echo ""
echo "8. File Counts"
echo "--------------"

HEADERS=$(find include -name "*.h" 2>/dev/null | wc -l)
SOURCES=$(find src -name "*.cpp" 2>/dev/null | wc -l)
TESTS=$(find tests -name "*.cpp" 2>/dev/null | wc -l)

echo "  Headers: $HEADERS"
echo "  Sources: $SOURCES"
echo "  Tests: $TESTS"

[ $HEADERS -ge 7 ]
check "At least 7 headers"

[ $SOURCES -ge 7 ]
check "At least 7 source files"

[ $TESTS -ge 7 ]
check "At least 7 test files"

echo ""
echo "9. Content Validation"
echo "---------------------"

grep -q "class AIDebugger" include/ai_debugger/AIDebugger.h
check "AIDebugger class defined"

grep -q "class StackTraceParser" include/ai_debugger/StackTraceParser.h
check "StackTraceParser class defined"

grep -q "enum class BugCategory" include/ai_debugger/RootCausePredictor.h
check "BugCategory enum defined"

grep -q "project(AIDebugger" CMakeLists.txt
check "CMake project configured"

echo ""
echo "========================================"
echo "Summary"
echo "========================================"
echo "  Passed: $PASS"
echo "  Failed: $FAIL"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "✓ PROJECT VERIFICATION SUCCESSFUL"
    echo ""
    echo "Next steps:"
    echo "  1. Review SIMPLE_README.md for overview"
    echo "  2. Read HOW_TO_TEST.md for testing instructions"
    echo "  3. Run: ./build.sh (requires CMake + C++ compiler)"
    echo "  4. Or compile demo: g++ demo/simple_test.cpp -o demo_test"
    echo ""
    exit 0
else
    echo "✗ VERIFICATION FAILED - $FAIL check(s) failed"
    echo ""
    echo "Please check missing files and try again"
    echo ""
    exit 1
fi
