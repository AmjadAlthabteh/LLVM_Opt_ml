# How to Test AI Debugger

## Quick Verification (No Compiler Needed)

Check that all files are in place:

```bash
# Linux/macOS
./quick_test.sh

# Windows
quick_test.bat
```

This verifies:
- ✓ All source files exist
- ✓ Build configuration is valid
- ✓ Project structure is correct

## Simple Test (Minimal C++ Compiler)

**Fastest way to see it work:**

```bash
# Compile standalone demo (no dependencies)
g++ -std=c++17 demo/simple_test.cpp -o demo_test

# Run it
./demo_test
```

**Expected output:**
```
=================================
AI DEBUGGER - SIMPLE TEST
=================================

TEST 1: Stack Trace Parsing
----------------------------
Parsed 3 frames:
  - process_data (main.cpp:42)
  - handle_request (server.cpp:108)
  - main (main.cpp:15)
✓ PASS: Correctly parsed stack trace

TEST 2: Bug Type Detection
---------------------------
Error: Segmentation fault: null pointer dereference at 0x0
Detected: NULL POINTER DEREFERENCE
✓ PASS: Correctly identified null pointer bug

TEST 3: Fix Suggestion
-----------------------
Suggested Fix:
Add null check:
  if (ptr != nullptr) {
    ptr->method();
  }
✓ PASS: Generated appropriate fix

TEST 4: Complete Workflow
--------------------------
Input: Crash log with segfault

Analysis Results:
  Frames parsed: 2
  Bug type: NULL POINTER DEREFERENCE
  Fix available: Yes

✓ PASS: Complete workflow successful

=================================
SUMMARY: Core functionality verified
=================================
```

This proves:
- ✓ Stack trace parsing works
- ✓ Bug detection works
- ✓ Fix generation works
- ✓ No dependencies needed

## Full Build Test (Requires CMake)

Build the complete library:

```bash
# Linux/macOS
./build.sh

# Windows
build.bat
```

**What this does:**
1. Configures CMake
2. Compiles all source files
3. Builds library (libai_debugger.a)
4. Compiles examples
5. Compiles tests
6. Runs test suite

**Expected result:**
```
================================
AI Debugger Build Script
================================

Configuration:
  Build Type: Release
  Tests: ON
  Examples: ON

Running CMake...
Building...
[100%] Built target ai_debugger

Running tests...
Test #1: StackTraceParser ............... Passed
Test #2: CallGraphAnalyzer ............. Passed
Test #3: RootCausePredictor ............ Passed
Test #4: ExplanationGenerator .......... Passed
Test #5: FixSuggester .................. Passed
Test #6: TestGenerator ................. Passed
Test #7: Integration ................... Passed

100% tests passed

================================
Build completed successfully!
================================
```

## Run Examples

After building, try the examples:

### Example 1: Simple API Usage

```bash
./build/examples/simple_example
```

This demonstrates:
- Parsing a stack trace
- Analyzing bug patterns
- Generating explanations
- Suggesting fixes
- Creating tests

### Example 2: CLI Tool

```bash
./build/examples/cli_tool demo/sample_crash.log
```

**Expected output:**
```
=================================================
AI DEBUGGER ANALYSIS REPORT
=================================================

Session ID: session_1234567890
Timestamp: 2025-11-20 19:53:00

STACK TRACE SUMMARY
-------------------
Frames: 3
Error Message: Segmentation fault

ROOT CAUSE ANALYSIS
-------------------
1. Null Pointer Dereference (Confidence: 95%)
   Null pointer dereference detected from segmentation fault
   Location: main.cpp:42

EXPLANATION
-----------
Bug Type: Null Pointer Dereference
The program tried to access memory through a null pointer...

SUGGESTED FIXES
---------------
1. Null Check (Confidence: 90%)
   Add null pointer check before dereferencing

   Original:
   ptr->member

   Fixed:
   if (ptr != nullptr) {
       ptr->member;
   }
```

## Manual Testing Steps

### Test 1: Parse Different Stack Trace Formats

Create test files:

**GDB format (test_gdb.log):**
```
#0  0x0000555555555269 in main (ptr=0x0) at test.cpp:42
#1  0x00005555555552a8 in process () at test.cpp:20
```

**LLDB format (test_lldb.log):**
```
frame #0: 0x0000555555555269 test`main at test.cpp:42
frame #1: 0x00005555555552a8 test`process at test.cpp:20
```

Test each:
```bash
./build/examples/cli_tool test_gdb.log
./build/examples/cli_tool test_lldb.log
```

Should parse both correctly.

### Test 2: Bug Detection Accuracy

Create crashes with different errors:

**Null pointer:**
```
Segmentation fault: null pointer dereference at 0x0
#0  in function (ptr=0x0) at code.cpp:10
```

**Buffer overflow:**
```
*** stack smashing detected ***: terminated
#0  in vulnerable_function at buffer.cpp:42
```

**Double free:**
```
double free or corruption (fasttop)
#0  in cleanup at memory.cpp:100
```

Each should detect the correct bug type.

### Test 3: Fix Quality

Check that fixes are appropriate:

**For null pointer:** Should suggest null checks
**For buffer overflow:** Should suggest bounds checking or std::vector
**For memory issues:** Should suggest smart pointers

### Test 4: Test Generation

Run with test generation enabled:
```bash
./build/examples/cli_tool --generate-tests crash.log
```

Check that `tests/` directory contains new test files with:
- Regression tests
- Edge case tests
- Proper framework syntax (GTest by default)

## Automated Test Suite

Run all unit tests:

```bash
cd build
ctest --output-on-failure
```

**Tests included:**
- `test_stack_trace_parser` - 6 tests
- `test_call_graph_analyzer` - 5 tests
- `test_root_cause_predictor` - 6 tests
- `test_explanation_generator` - 6 tests
- `test_fix_suggester` - 6 tests
- `test_test_generator` - 6 tests
- `test_integration` - 6 tests

**Total: 41+ test cases**

## Performance Testing

Time the operations:

```bash
time ./build/examples/cli_tool large_crash.log
```

Should complete in <100ms for typical crash logs.

## Troubleshooting

### "No such file or directory"

Make sure you're in the project root:
```bash
cd /path/to/LLVM-MLOpt_wp
```

### "Permission denied"

Make scripts executable:
```bash
chmod +x build.sh quick_test.sh
```

### "CMake not found"

Install CMake:
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows - download from cmake.org
```

### "Compiler not found"

Install build tools:
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows - install Visual Studio Build Tools
```

### Tests fail

Check compiler version:
```bash
g++ --version  # Should be 7.0+
```

Build in debug mode for more info:
```bash
./build.sh Debug
```

## Verification Checklist

After testing, verify:

- [ ] Simple demo compiles and runs
- [ ] Full build completes without errors
- [ ] All tests pass (ctest)
- [ ] Examples run successfully
- [ ] Can parse different stack trace formats
- [ ] Detects various bug types
- [ ] Generates appropriate fixes
- [ ] Creates valid test code
- [ ] Reports are human-readable
- [ ] Performance is acceptable (<100ms)

## Success Criteria

**Minimum (Works):**
- Simple demo runs ✓
- Parses stack traces ✓
- Detects common bugs ✓

**Standard (Production Ready):**
- Full build succeeds ✓
- All tests pass ✓
- Examples work ✓
- Generates fixes ✓

**Complete (Fully Validated):**
- Tested on multiple formats ✓
- Verified bug detection accuracy ✓
- Validated fix quality ✓
- Performance meets specs ✓

---

**Quick Start:** Just run `./quick_test.sh` or compile `demo/simple_test.cpp`
