# AI Debugger Quick Start Guide

## Installation

### Option 1: Build from Source

```bash
git clone https://github.com/yourusername/ai-debugger.git
cd ai-debugger
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

### Option 2: Using Package Manager

```bash
# Ubuntu/Debian
sudo apt-get install libai-debugger-dev

# macOS
brew install ai-debugger

# Windows (vcpkg)
vcpkg install ai-debugger
```

## First Steps

### 1. Capture Stack Trace

When your program crashes, capture the stack trace:

```bash
# Using GDB
gdb ./your_program
run
# After crash:
bt
# Save output to file
```

### 2. Analyze with AI Debugger

```cpp
#include "ai_debugger/AIDebugger.h"
#include <iostream>
#include <fstream>

int main() {
    ai_debugger::AIDebugger debugger;

    // Load stack trace from file
    auto session = debugger.analyzeFromFile("crash.log");

    // Print analysis
    std::cout << debugger.getReport(session);

    return 0;
}
```

### 3. Apply Suggested Fixes

```cpp
// Review suggested fixes
for (const auto& fix : session.suggested_fixes) {
    std::cout << "Fix: " << fix.description << "\n";
    std::cout << "Confidence: " << (fix.confidence * 100) << "%\n";
    std::cout << "Original: " << fix.original_code << "\n";
    std::cout << "Fixed: " << fix.fixed_code << "\n\n";
}

// Apply best fix
auto result = debugger.applyBestFix(session);
if (result.success) {
    std::cout << "Fix applied successfully!\n";
}
```

### 4. Generate Tests

```cpp
// Generate regression tests
debugger.setTestFramework(ai_debugger::TestFramework::GTEST);
if (debugger.generateTests(session)) {
    std::cout << "Tests generated in ./tests/\n";
}
```

## Command-Line Usage

### Basic Analysis

```bash
./cli_tool crash.log
```

### Full Workflow

```bash
./cli_tool -v \
    --source /path/to/source \
    --output report.txt \
    --generate-tests \
    --framework gtest \
    crash.log
```

### Auto-Fix Mode

```bash
./cli_tool --auto-fix crash.log
```

## Common Scenarios

### Null Pointer Crash

**Input:**
```
#0  0x555555555269 in process(ptr=0x0) at main.cpp:42
```

**AI Debugger Output:**
- Category: Null Pointer Dereference
- Confidence: 95%
- Suggested Fix: Add null check
- Generated Test: Verify null handling

### Buffer Overflow

**Input:**
```
*** stack smashing detected ***: terminated
#0  vulnerable_function at buffer.cpp:15
```

**AI Debugger Output:**
- Category: Buffer Overflow
- Confidence: 88%
- Suggested Fix: Use bounds checking
- Generated Test: Boundary value tests

### Use After Free

**Input:**
```
Segmentation fault
#0  free(ptr=0x7fff...) at memory.cpp:100
```

**AI Debugger Output:**
- Category: Use After Free
- Confidence: 82%
- Suggested Fix: Use smart pointers
- Generated Test: Lifetime management test

## Integration Examples

### CMake Integration

```cmake
find_package(AIDebugger REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp AIDebugger::ai_debugger)
```

### CI/CD Integration

```yaml
# .github/workflows/ci.yml
- name: Analyze Crashes
  if: failure()
  run: |
    ./cli_tool --generate-tests crash_dumps/*.log
    git add tests/
    git commit -m "Add regression tests for crashes"
```

### GDB Integration

```gdb
# .gdbinit
define analyze_crash
  set logging file crash.log
  set logging on
  bt
  set logging off
  shell ./cli_tool crash.log
end
```

## Tips and Best Practices

1. **Always use debug symbols** (`-g` flag) for accurate analysis
2. **Provide source directory** for better fix suggestions
3. **Review fixes** before auto-applying them
4. **Run generated tests** to verify fixes
5. **Keep session history** for tracking patterns

## Next Steps

- Read [Architecture Documentation](ARCHITECTURE.md)
- Explore [Advanced Usage](ADVANCED.md)
- Check [API Reference](API.md)
- See [Examples Directory](../examples/)

## Troubleshooting

### Parser Errors

```cpp
debugger.setVerbose(true);  // Enable detailed logging
```

### Fix Application Fails

```cpp
debugger.setDryRun(true);   // Test without modifying files
```

### No Root Cause Found

- Ensure complete stack trace
- Verify debug symbols present
- Check error message clarity

## Support

- GitHub Issues: Report bugs and request features
- Documentation: Full API reference in docs/
- Examples: Working code in examples/
