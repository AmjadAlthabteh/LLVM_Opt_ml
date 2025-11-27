#!/bin/bash
# ============================================
#   AI Debugger - Analyze Your Crash
#   Usage: ./ANALYZE_MY_CRASH.sh crash_file.log
# ============================================

if [ -z "$1" ]; then
    echo ""
    echo "========================================"
    echo "   AI Debugger - Crash Analysis"
    echo "========================================"
    echo ""
    echo "HOW TO USE:"
    echo ""
    echo "  ./ANALYZE_MY_CRASH.sh crash_file.log"
    echo ""
    echo "Or enter the path now:"
    echo ""
    read -p "Enter path to crash file: " CRASH_FILE
    if [ -z "$CRASH_FILE" ]; then
        echo "No file specified. Exiting."
        exit 1
    fi
else
    CRASH_FILE="$1"
fi

if [ ! -f "$CRASH_FILE" ]; then
    echo ""
    echo "[ERROR] File not found: $CRASH_FILE"
    echo ""
    exit 1
fi

echo ""
echo "========================================"
echo "   Analyzing: $CRASH_FILE"
echo "========================================"
echo ""

./build/examples/cli_tool -v "$CRASH_FILE"

echo ""
echo "========================================"
echo "   Analysis Complete!"
echo "========================================"
echo ""
echo "Want to generate tests?"
echo "  Run: ./build/examples/cli_tool --generate-tests \"$CRASH_FILE\""
echo ""
echo "Want to save the report?"
echo "  Run: ./build/examples/cli_tool -o report.txt \"$CRASH_FILE\""
echo ""
