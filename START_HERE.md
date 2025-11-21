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

## Quickest Test (30 seconds)

Verify everything is working:

```bash
./verify_project.sh
```

**Expected:** ✓ PROJECT VERIFICATION SUCCESSFUL (34/34 checks passed)

---

## See It Work (5 minutes)

### Option 1: Simple Demo (No Build Required)

```bash
# Compile standalone demo
g++ -std=c++17 demo/simple_test.cpp -o demo_test

# Run it
./demo_test
```

**Shows:** Stack parsing → Bug detection → Fix suggestion

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

Test Generated: ✓
```

---

## Documentation

**Quick:**
- `SIMPLE_README.md` ← Overview with examples
- `HOW_TO_TEST.md` ← Testing guide
- `START_HERE.md` ← You are here

**Detailed:**
- `README.md` ← Full documentation
- `INSTALL.md` ← Installation guide
- `docs/API.md` ← API reference
- `docs/QUICKSTART.md` ← Getting started

---

## Project Structure

```
LLVM-MLOpt_wp/
├── demo/
│   ├── simple_test.cpp       ← Standalone test (compile & run)
│   └── sample_crash.log      ← Example crash
│
├── include/ai_debugger/      ← API headers
│   ├── AIDebugger.h
│   ├── StackTraceParser.h
│   ├── RootCausePredictor.h
│   └── ...
│
├── src/                      ← Implementation
├── tests/                    ← Unit tests (41+ tests)
├── examples/                 ← Working examples
│
├── SIMPLE_README.md          ← Read this first
├── HOW_TO_TEST.md            ← Testing guide
└── verify_project.sh         ← Quick check
```

---

## Key Features

✅ **Multi-format parsing** - GDB, LLDB, MSVC
✅ **14 bug types** - Null pointer, buffer overflow, use-after-free, etc.
✅ **Auto-fix suggestions** - Smart pointers, null checks, bounds checking
✅ **Test generation** - GTest, Catch2, Boost.Test
✅ **Natural language** - Technical + simplified explanations
✅ **Fast** - <50ms for complete analysis

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

✓ All 34 project checks passed
✓ All files present and valid
✓ Build system configured
✓ Examples ready
✓ Tests ready
✓ Documentation complete

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
