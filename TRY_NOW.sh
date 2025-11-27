#!/bin/bash
# ============================================
#   AI Debugger - INSTANT DEMO
#   Run this to see it in action!
# ============================================

echo ""
echo "========================================"
echo "   AI Debugger - LIVE DEMO"
echo "========================================"
echo ""
echo "Running AI analysis on a crash..."
echo ""
sleep 2

./build/examples/cli_tool demo/null_pointer_crash.txt

echo ""
echo "========================================"
echo "   Demo Complete!"
echo "========================================"
echo ""
echo "What just happened?"
echo "- AI Debugger analyzed a null pointer crash"
echo "- Identified the bug type and location"
echo "- Suggested how to fix it"
echo "- Gave prevention tips"
echo ""
echo "Want to try more?"
echo "  Run: ./quick_start.sh"
echo ""
echo "Have your own crash to analyze?"
echo "  Run: ./build/examples/cli_tool your_crash.log"
echo ""
