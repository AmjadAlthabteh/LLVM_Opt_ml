# AI Debugger

> Automatically analyze C++ crashes, find root causes, and suggest fixes

## What It Does

Takes a crash stack trace → Identifies the bug → Suggests a fix → Generates tests

```
Input: Segfault stack trace
Output: "NULL POINTER DEREFERENCE - Add null check at line 42"
```

## Tech Stack

- **Language**: C++17
- **Build**: CMake
- **Tests**: Google Test
- **Platforms**: Linux, macOS, Windows

## Features

✅ Parse stack traces (GDB, LLDB, MSVC)
✅ Detect 14 bug types (null pointer, buffer overflow, use-after-free, etc.)
✅ Generate human-readable explanations
✅ Suggest code fixes with confidence scores
✅ Auto-generate regression tests

## Quick Start

**1. Build:**
```bash
./build.sh
```

**2. Test simple demo:**
```bash
g++ demo/simple_test.cpp -o demo_test
./demo_test
```

**3. Use the library:**
```cpp
#include "ai_debugger/AIDebugger.h"

ai_debugger::AIDebugger debugger;
auto session = debugger.analyzeStackTrace(crash_log);
std::cout << debugger.getReport(session);
```

**4. Or use CLI tool:**
```bash
./build/examples/cli_tool crash.log
```

## Example Output

**Input:**
```
#0  0x555555555269 in process (ptr=0x0) at main.cpp:42
```

**Output:**
```
Bug Type: NULL POINTER DEREFERENCE
Confidence: 95%
Location: main.cpp:42

Explanation:
The program tried to access memory through a null pointer.
This happens when a pointer is not initialized or set to nullptr.

Suggested Fix:
if (ptr != nullptr) {
    ptr->process();
}

Prevention:
- Always initialize pointers
- Check before dereferencing
- Use smart pointers (std::unique_ptr, std::shared_ptr)
```

## Project Structure

```
include/          → Headers (StackTraceParser, RootCausePredictor, etc.)
src/              → Implementation
tests/            → Unit tests
examples/         → Working examples
demo/             → Simple standalone demo
docs/             → Full documentation
```

## How It Works

1. **StackTraceParser** → Extracts function names, files, line numbers
2. **CallGraphAnalyzer** → Builds call graph, detects patterns
3. **RootCausePredictor** → Matches error patterns to bug categories
4. **ExplanationGenerator** → Creates human-readable explanations
5. **FixSuggester** → Generates code fixes
6. **TestGenerator** → Creates regression tests

## Bug Categories Detected

- Null Pointer Dereference
- Buffer Overflow/Underflow
- Use After Free
- Double Free
- Memory Leak
- Race Condition
- Deadlock
- Logic Error
- Assertion Failure
- Resource Exhaustion
- Type Error
- Arithmetic Error

## Fix Types Generated

- Null checks
- Bounds checking
- Smart pointer conversion
- Memory cleanup
- Synchronization (mutexes)
- Error handling
- Initialization

## Requirements

**Minimum:**
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+

**Optional:**
- LLVM (for advanced analysis)
- Google Test (for tests)

## Build & Test

```bash
# Quick build
make

# Full build with tests
./build.sh Release ON ON

# Run tests
cd build && ctest

# Install
sudo make install
```

## Usage Examples

**Analyze crash:**
```cpp
AIDebugger debugger;
auto session = debugger.analyzeFromFile("crash.log");
std::cout << debugger.getReport(session);
```

**Auto-fix:**
```cpp
debugger.enableAutoFix(true);
auto result = debugger.applyBestFix(session);
```

**Generate tests:**
```cpp
debugger.enableTestGeneration(true);
debugger.generateTests(session);
```

## Documentation

- `SIMPLE_README.md` ← You are here
- `README.md` → Full documentation
- `docs/QUICKSTART.md` → Getting started guide
- `docs/API.md` → API reference
- `INSTALL.md` → Installation guide

## License

MIT - Use freely in any project

## Performance

- Parse trace: <1ms
- Detect bug: <10ms
- Generate fix: <5ms
- Total: <50ms

---

**TL;DR**: Drop in a crash log, get back the bug type, explanation, and fix suggestion. All automated.
