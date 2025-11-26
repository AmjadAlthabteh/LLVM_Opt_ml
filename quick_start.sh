#!/bin/bash
# AI Debugger - Quick Start Script for Linux/Mac
# This script helps you get started with AI Debugger quickly

set -e

echo "========================================"
echo "  AI Debugger - Quick Start"
echo "========================================"
echo ""

# Check if already built
if [ -f "build/examples/cli_tool" ]; then
    echo "[OK] AI Debugger is already built!"
    echo ""
else
    echo "[INFO] AI Debugger needs to be built first."
    echo "[INFO] This will take 2-5 minutes..."
    echo ""
    read -p "Would you like to build it now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo ""
        echo "Building AI Debugger..."
        echo ""
        chmod +x build.sh
        ./build.sh Release ON ON
        echo ""
        echo "[OK] Build completed successfully!"
        echo ""
    else
        echo "Build cancelled. Run ./build.sh when ready."
        exit 0
    fi
fi

# Main menu function
show_menu() {
    echo "What would you like to do?"
    echo ""
    echo "  1. Try a simple example (null pointer crash)"
    echo "  2. Try all examples"
    echo "  3. Analyze your own crash file"
    echo "  4. Show version info"
    echo "  5. Show help"
    echo "  6. Exit"
    echo ""
    read -p "Enter your choice (1-6): " choice

    case $choice in
        1) example1;;
        2) allexamples;;
        3) custom;;
        4) version;;
        5) help;;
        6) exit 0;;
        *) echo "Invalid choice"; show_menu;;
    esac
}

# Example 1
example1() {
    echo ""
    echo "========================================"
    echo " Running: Null Pointer Crash Example"
    echo "========================================"
    echo ""
    ./build/examples/cli_tool demo/null_pointer_crash.txt
    echo ""
    echo "========================================"
    echo " Example Complete!"
    echo "========================================"
    echo ""
    read -p "Press enter to continue..."
    show_menu
}

# All examples
allexamples() {
    echo ""
    echo "========================================"
    echo " Running All Examples"
    echo "========================================"
    echo ""

    for f in demo/*.txt demo/*.log; do
        if [ -f "$f" ]; then
            echo ""
            echo "----------------------------------------"
            echo "Testing: $f"
            echo "----------------------------------------"
            ./build/examples/cli_tool "$f"
            echo ""
            sleep 1
        fi
    done

    echo ""
    echo "========================================"
    echo " All Examples Complete!"
    echo "========================================"
    echo ""
    read -p "Press enter to continue..."
    show_menu
}

# Custom file
custom() {
    echo ""
    echo "========================================"
    echo " Analyze Your Own Crash File"
    echo "========================================"
    echo ""
    read -p "Enter path to your crash file: " crashfile

    if [ ! -f "$crashfile" ]; then
        echo "[ERROR] File not found: $crashfile"
        echo ""
        read -p "Press enter to continue..."
        show_menu
        return
    fi

    echo ""
    echo "Analyzing: $crashfile"
    echo ""
    ./build/examples/cli_tool -v "$crashfile"
    echo ""
    echo "========================================"
    echo " Analysis Complete!"
    echo "========================================"
    echo ""
    echo "Would you like to:"
    echo " 1. Generate tests for this crash"
    echo " 2. Save report to file"
    echo " 3. Return to menu"
    echo ""
    read -p "Enter choice (1-3): " subchoice

    case $subchoice in
        1)
            echo ""
            echo "Generating tests..."
            ./build/examples/cli_tool --generate-tests "$crashfile"
            echo ""
            read -p "Press enter to continue..."
            ;;
        2)
            read -p "Enter report filename (e.g., report.txt): " reportfile
            echo ""
            echo "Saving report..."
            ./build/examples/cli_tool -o "$reportfile" "$crashfile"
            echo ""
            echo "Report saved to: $reportfile"
            read -p "Press enter to continue..."
            ;;
        3)
            ;;
    esac

    show_menu
}

# Version
version() {
    echo ""
    ./build/examples/cli_tool --version
    echo ""
    read -p "Press enter to continue..."
    show_menu
}

# Help
help() {
    echo ""
    ./build/examples/cli_tool --help
    echo ""
    echo "========================================"
    echo " Additional Resources"
    echo "========================================"
    echo ""
    echo " - GETTING_STARTED.md : Complete beginner guide"
    echo " - SIMPLE_README.md   : Quick overview"
    echo " - README.md          : Full documentation"
    echo " - demo/              : Sample crash files"
    echo ""
    read -p "Press enter to continue..."
    show_menu
}

# Start
show_menu
