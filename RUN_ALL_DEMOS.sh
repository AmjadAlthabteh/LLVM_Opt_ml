#!/bin/bash
# ============================================
#   AI Debugger - Run All Example Crashes
#   See how it handles different bug types!
# ============================================

echo ""
echo "========================================"
echo "   AI Debugger - All Demos"
echo "========================================"
echo ""
echo "This will run AI Debugger on multiple"
echo "crash examples to show different bug types:"
echo "  - Null pointer dereference"
echo "  - Buffer overflow"
echo "  - Use-after-free"
echo "  - And more..."
echo ""
echo "Press Enter to start, or Ctrl+C to cancel"
read

for f in demo/*.txt demo/*.log; do
    if [ -f "$f" ]; then
        echo ""
        echo "========================================"
        echo "Demo: $(basename "$f")"
        echo "========================================"
        echo ""
        ./build/examples/cli_tool "$f"
        echo ""
        echo "Press Enter for next demo..."
        read
    fi
done

echo ""
echo "========================================"
echo "   All Demos Complete!"
echo "========================================"
echo ""
echo "You've seen how AI Debugger handles:"
echo "  - Different crash types"
echo "  - Various debugger formats"
echo "  - Root cause identification"
echo "  - Fix suggestions"
echo ""
echo "Ready to try your own crashes?"
echo "  Run: ./ANALYZE_MY_CRASH.sh"
echo ""
