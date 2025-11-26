# Sample Crash Examples

This directory contains example crash logs that you can use to test AI Debugger.

## Available Examples

### 1. **null_pointer_crash.txt**
   - **Bug Type**: Null Pointer Dereference
   - **Difficulty**: Easy (very common bug)
   - **Try it**:
     ```batch
     .\build\examples\Release\cli_tool.exe demo\null_pointer_crash.txt
     ```
   - **What you'll learn**: How to detect and fix null pointer bugs

### 2. **buffer_overflow_crash.txt**
   - **Bug Type**: Buffer Overflow
   - **Difficulty**: Medium
   - **Try it**:
     ```batch
     .\build\examples\Release\cli_tool.exe demo\buffer_overflow_crash.txt
     ```
   - **What you'll learn**: How to identify buffer overruns and bounds checking issues

### 3. **use_after_free_crash.txt**
   - **Bug Type**: Use-After-Free
   - **Difficulty**: Medium-Hard
   - **Try it**:
     ```batch
     .\build\examples\Release\cli_tool.exe demo\use_after_free_crash.txt
     ```
   - **What you'll learn**: Memory management bugs and dangling pointers

### 4. **sample_crash.log**
   - **Bug Type**: Mixed/General
   - **Difficulty**: Easy
   - **Try it**:
     ```batch
     .\build\examples\Release\cli_tool.exe demo\sample_crash.log
     ```
   - **What you'll learn**: Basic stack trace analysis

## Try Them All!

Run this command to test all examples:

**Windows:**
```batch
for %f in (demo\*.txt demo\*.log) do (
    echo.
    echo ========================================
    echo Testing: %f
    echo ========================================
    .\build\examples\Release\cli_tool.exe %f
    echo.
)
```

**Linux/Mac:**
```bash
for f in demo/*.txt demo/*.log; do
    echo ""
    echo "========================================"
    echo "Testing: $f"
    echo "========================================"
    ./build/examples/cli_tool "$f"
    echo ""
done
```

## Advanced Usage

### Generate Fixes for All Examples

```batch
REM Windows
for %f in (demo\*.txt) do cli_tool.exe -v --generate-tests -o reports\%~nf_report.txt %f

# Linux/Mac
for f in demo/*.txt; do
    name=$(basename "$f" .txt)
    ./cli_tool -v --generate-tests -o "reports/${name}_report.txt" "$f"
done
```

### Compare Different Test Frameworks

```batch
REM GTest
cli_tool.exe --framework gtest --generate-tests demo\null_pointer_crash.txt

REM Catch2
cli_tool.exe --framework catch2 --generate-tests demo\null_pointer_crash.txt

REM Boost.Test
cli_tool.exe --framework boost --generate-tests demo\null_pointer_crash.txt
```

## Creating Your Own Examples

To add your own crash examples:

1. Save your crash log to a `.txt` or `.log` file in this directory
2. Run AI Debugger on it
3. (Optional) Add a description at the bottom of the file

**Tip**: The more complete the stack trace, the better AI Debugger can analyze it!

## Stack Trace Formats Supported

AI Debugger understands these debugger formats:

- **GDB** (GNU Debugger) - Linux/Mac
- **LLDB** (LLVM Debugger) - Mac/Linux
- **MSVC** (Microsoft Visual C++) - Windows
- **Standard C++** exception stack traces
- **AddressSanitizer** (ASAN) reports
- **Valgrind** memory error reports

---

**Pro Tip**: Start with `null_pointer_crash.txt` as it's the simplest example!
