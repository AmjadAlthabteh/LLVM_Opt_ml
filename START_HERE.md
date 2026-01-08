# AI Debugger - START HERE

## What This Is

An AI-powered C++ debugger that automatically:
- Reads crash stack traces
- Identifies what went wrong
- Explains the bug in plain English
- Suggests code fixes
- Generates regression tests

**Tech Stack:** C++17, CMake, GTest

---

## Try It RIGHT NOW (10 seconds)

**Already built?** Double-click to run an instant demo:

**Windows:** `TRY_NOW.bat`
**Linux/Mac:** `./TRY_NOW.sh`

**Have a crash?** Drag your crash file onto `ANALYZE_MY_CRASH.bat` or run:
```bash
./ANALYZE_MY_CRASH.sh your_crash.log
```

**Want to see all examples?** `RUN_ALL_DEMOS.bat` / `./RUN_ALL_DEMOS.sh`

**Need to build first?** Run `build.bat` (Windows) or `./build.sh` (Linux/Mac)

---

## Quickest Test (30 seconds)

Verify everything is working:

```bash
./compiler_check.sh
./verify_project.sh
```

**Expected:** âœ“ PROJECT VERIFICATION SUCCESSFUL (34/34 checks passed)

---

## See It Work (5 minutes)

### Option 1: Simple Demo (No Build Required)

```bash
# Compile standalone demo
g++ -std=c++17 demo/simple_test.cpp -o demo_test

# Run it
./demo_test
```

**Shows:** Stack parsing â†’ Bug detection â†’ Fix suggestion

### Option 2: Full System

```bash
# Build everything
./build.sh

# Run example
./build/examples/simple_example

# Try CLI tool
./build/examples/cli_tool demo/sample_crash.log
```

---

## How It Works

**Input:** Crash log
```
Segmentation fault
#0 in process_data (ptr=0x0) at main.cpp:42
```

**Output:**
```
Bug: NULL POINTER DEREFERENCE
Confidence: 95%
Location: main.cpp:42

Fix:
  if (ptr != nullptr) {
      ptr->process();
  }

Test Generated: âœ“
```

---

## Documentation

**Quick:**
- `SIMPLE_README.md` â† Overview with examples
- `HOW_TO_TEST.md` â† Testing guide
- `START_HERE.md` â† You are here

**Detailed:**
- `README.md` â† Full documentation
- `INSTALL.md` â† Installation guide
- `docs/API.md` â† API reference
- `docs/QUICKSTART.md` â† Getting started

---

## Project Structure

```
LLVM-MLOpt_wp/
â”œâ”€â”€ demo/
â”‚   â”œâ”€â”€ simple_test.cpp       â† Standalone test (compile & run)
â”‚   â””â”€â”€ sample_crash.log      â† Example crash
â”‚
â”œâ”€â”€ include/ai_debugger/      â† API headers
â”‚   â”œâ”€â”€ AIDebugger.h
â”‚   â”œâ”€â”€ StackTraceParser.h
â”‚   â”œâ”€â”€ RootCausePredictor.h
â”‚   â””â”€â”€ ...
â”‚
â”œâ”€â”€ src/                      â† Implementation
â”œâ”€â”€ tests/                    â† Unit tests (41+ tests)
â”œâ”€â”€ examples/                 â† Working examples
â”‚
â”œâ”€â”€ SIMPLE_README.md          â† Read this first
â”œâ”€â”€ HOW_TO_TEST.md            â† Testing guide
â””â”€â”€ verify_project.sh         â† Quick check
```

---

## Key Features

âœ… **Multi-format parsing** - GDB, LLDB, MSVC
âœ… **14 bug types** - Null pointer, buffer overflow, use-after-free, etc.
âœ… **Auto-fix suggestions** - Smart pointers, null checks, bounds checking
âœ… **Test generation** - GTest, Catch2, Boost.Test
âœ… **Natural language** - Technical + simplified explanations
âœ… **Fast** - <50ms for complete analysis

---

## Usage

### Library

```cpp
#include "ai_debugger/AIDebugger.h"

ai_debugger::AIDebugger debugger;
auto session = debugger.analyzeStackTrace(crash_log);
std::cout << debugger.getReport(session);
```

### Command Line

```bash
./cli_tool --verbose --generate-tests crash.log
```

---

## Build Requirements

**Minimum:**
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+

**Optional:**
- LLVM (advanced analysis)
- GTest (testing)

---

## Verification Status

âœ“ All 34 project checks passed
âœ“ All files present and valid
âœ“ Build system configured
âœ“ Examples ready
âœ“ Tests ready
âœ“ Documentation complete

---

## Next Steps

1. **Quick look:** Read `SIMPLE_README.md`
2. **Try it:** Run `./verify_project.sh`
3. **Build:** Run `./build.sh`
4. **Test:** Compile `demo/simple_test.cpp`
5. **Use:** Integrate with your project

---

## Get Help

- **Testing:** See `HOW_TO_TEST.md`
- **Installing:** See `INSTALL.md`
- **API:** See `docs/API.md`
- **Examples:** Check `examples/` directory

---

**TL;DR**: Run `./verify_project.sh` to verify, then read `SIMPLE_README.md` for overview.
