# Getting Started with AI Debugger - Complete Guide for Beginners

> **New to this tool?** This guide will walk you through everything step-by-step!

## What is AI Debugger?

AI Debugger is a tool that helps you fix C++ crashes and bugs automatically. When your program crashes, it:

1. **Reads** the crash information (stack trace)
2. **Finds** what caused the bug
3. **Explains** it in simple English
4. **Suggests** how to fix it
5. **Creates** tests to prevent it from happening again

**Think of it as having an AI assistant that helps debug your compiler errors and crashes!**

---

## Quick 2-Minute Test (Windows)

If you're on Windows and just want to see it work right now:

```batch
REM Make sure you're in the project folder
cd C:\Users\17346\OneDrive\Documents\LLVM-MLOpt_wp

REM Run the pre-built tool on a sample crash
.\build\examples\Release\cli_tool.exe demo\sample_crash.log
```

**That's it!** You should see the AI analyze the crash and suggest a fix.

---

## Installation (First Time Setup)

### Windows Users

1. **Make sure you have Visual Studio installed** (with C++ tools)
   - Download from: https://visualstudio.microsoft.com/downloads/
   - During install, select "Desktop development with C++"

2. **Open "Developer Command Prompt for VS 2022"**
   - Search for it in Windows Start Menu

3. **Navigate to the project folder**
   ```batch
   cd C:\Users\17346\OneDrive\Documents\LLVM-MLOpt_wp
   ```

4. **Build the project**
   ```batch
   build.bat Release ON ON
   ```

   This will take 2-5 minutes. You'll see lots of text flying by - that's normal!

5. **Test it worked**
   ```batch
   .\build\examples\Release\cli_tool.exe --version
   ```

### Linux/Mac Users

1. **Make sure you have the tools**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake git

   # macOS
   brew install cmake
   ```

2. **Build the project**
   ```bash
   cd /path/to/LLVM-MLOpt_wp
   ./build.sh Release ON ON
   ```

3. **Test it worked**
   ```bash
   ./build/examples/cli_tool --version
   ```

---

## How to Use It - Step by Step

### Method 1: Analyze a Crash File (Easiest)

When your program crashes, save the error message to a file, then run:

**Windows:**
```batch
.\build\examples\Release\cli_tool.exe my_crash.txt
```

**Linux/Mac:**
```bash
./build/examples/cli_tool my_crash.txt
```

### Method 2: Try the Built-in Examples

We've included sample crashes for you to try:

```batch
REM Windows
.\build\examples\Release\cli_tool.exe demo\sample_crash.log

# Linux/Mac
./build/examples/cli_tool demo/sample_crash.log
```

### Method 3: Paste a Crash Directly

You can paste crash info directly (use `-` for stdin):

**Windows:**
```batch
type crash_info.txt | .\build\examples\Release\cli_tool.exe -
```

**Linux/Mac:**
```bash
cat crash_info.txt | ./build/examples/cli_tool -
```

---

## Common Use Cases

### 🔧 Just Tell Me What's Wrong

```batch
cli_tool.exe crash.log
```

Shows: Bug type, location, explanation

### 🔧 Give Me Detailed Info

```batch
cli_tool.exe -v crash.log
```

The `-v` flag means "verbose" - gives you more details

### 🔧 Fix It Automatically

```batch
cli_tool.exe --auto-fix crash.log
```

**⚠️ Warning:** This modifies your source code! Make sure you have backups or use git.

### 🔧 Generate Tests So It Won't Happen Again

```batch
cli_tool.exe --generate-tests crash.log
```

Creates test files in your project that check for this bug

### 🔧 Save the Report to a File

```batch
cli_tool.exe -o report.txt crash.log
```

Good for sharing with your team or keeping records

### 🔧 Do Everything at Once

```batch
cli_tool.exe -v --auto-fix --generate-tests -o report.txt crash.log
```

Analyzes, fixes, generates tests, and saves a report

---

## Understanding the Output

When you run the tool, you'll see something like:

```
Analyzing stack trace from: crash.log
============================================

Stack Trace Analysis
--------------------
Frames: 5
Crash Type: SIGSEGV (Segmentation fault)

Root Cause Prediction
---------------------
Bug Category: NULL_POINTER_DEREFERENCE
Confidence: 95%
Location: main.cpp:42

Explanation
-----------
The program attempted to access memory through a null pointer.
This occurs when a pointer is not initialized or has been set to nullptr.

Suggested Fix #1 (Confidence: 90%)
----------------------------------
Type: ADD_NULL_CHECK
File: main.cpp
Line: 42

Before:
    ptr->process();

After:
    if (ptr != nullptr) {
        ptr->process();
    }

Prevention Tips
---------------
- Always initialize pointers
- Check pointers before dereferencing
- Use smart pointers (std::unique_ptr, std::shared_ptr)
```

### What Each Section Means

- **Stack Trace Analysis**: Basic info about the crash
- **Root Cause Prediction**: What the AI thinks caused it + how confident it is
- **Explanation**: Human-readable description of the problem
- **Suggested Fix**: The code change that would prevent this
- **Prevention Tips**: How to avoid this bug in the future

---

## Working with Your Own Crashes

### Getting a Stack Trace

When your program crashes, you need to capture the "stack trace" (the error message showing where it crashed).

**Using GDB (Linux/Mac):**
```bash
gdb ./your_program
run
# Program crashes
bt  # This shows the stack trace
```

**Using Visual Studio (Windows):**
1. When the program crashes, Visual Studio shows the stack trace automatically
2. Copy the entire "Call Stack" window
3. Paste into a text file

**Using LLDB (Mac):**
```bash
lldb ./your_program
run
# Program crashes
bt  # This shows the stack trace
```

Save this information to a file, then run AI Debugger on it!

---

## Configuration File (Advanced)

You can create a `.aidebuggerrc` file to set your preferences:

```json
{
  "source_directory": "./src",
  "test_output_directory": "./tests",
  "verbose": true,
  "auto_fix": false,
  "auto_test": true,
  "test_framework": "gtest"
}
```

Then run with:
```batch
cli_tool.exe --config .aidebuggerrc crash.log
```

This saves you from typing the same options every time!

---

## Troubleshooting

### "cli_tool.exe not found"

Make sure you built the project first:
```batch
build.bat Release ON ON
```

The tool will be at: `.\build\examples\Release\cli_tool.exe`

### "Failed to parse stack trace"

Your crash file might not be in a format the tool recognizes. Supported formats:
- GDB output
- LLDB output
- MSVC/Visual Studio crashes
- Standard C++ stack traces

Try running with `-v` to see more details about what went wrong.

### "Error: File not found"

Check that your crash file path is correct. Use full paths if needed:
```batch
cli_tool.exe C:\full\path\to\crash.log
```

### The tool suggests a fix but it seems wrong

The AI isn't perfect! The "confidence" score tells you how sure it is. If confidence is low (<70%), double-check the suggestion before applying it.

**Always review auto-fixes before committing them to your code!**

---

## Next Steps

1. **Try the examples**: Run the tool on `demo\sample_crash.log`
2. **Read the docs**: Check out `SIMPLE_README.md` for more info
3. **Use it on real crashes**: Next time your program crashes, try AI Debugger!
4. **Integrate with your workflow**: Add it to your build scripts or CI/CD

---

## Tips for Best Results

✅ **DO:**
- Use debug builds (they have better error information)
- Include full stack traces (not just the top line)
- Set your source directory with `-s /path/to/src`
- Review AI suggestions before applying them
- Keep backups before using `--auto-fix`

❌ **DON'T:**
- Use on release builds (less debugging info)
- Apply fixes blindly without understanding them
- Expect 100% accuracy (it's AI, not magic!)
- Skip reading the explanation

---

## Real-World Example Workflow

Here's how a typical developer uses AI Debugger:

1. **Program crashes during testing**
   ```
   Segmentation fault (core dumped)
   ```

2. **Get the stack trace**
   ```bash
   gdb ./myapp core
   bt > crash.log
   ```

3. **Analyze it**
   ```bash
   ./cli_tool -v crash.log
   ```

4. **Read the explanation and fix suggestion**
   ```
   Bug: NULL_POINTER_DEREFERENCE
   Fix: Add null check before ptr->process()
   ```

5. **Apply the fix manually** (or use `--auto-fix` if confident)

6. **Generate tests**
   ```bash
   ./cli_tool --generate-tests crash.log
   ```

7. **Run tests to verify**
   ```bash
   cd tests && make test
   ```

8. **Commit the fix**
   ```bash
   git add .
   git commit -m "Fix null pointer crash in process() - suggested by AI Debugger"
   ```

---

## Help & Support

- **Full documentation**: See `README.md`
- **API reference**: See `docs/API.md`
- **How to run tests**: See `HOW_TO_TEST.md`
- **Installation issues**: See `INSTALL.md`

---

## Quick Reference Card

```
# Show help
cli_tool --help

# Analyze crash
cli_tool crash.log

# Verbose output
cli_tool -v crash.log

# Auto-fix
cli_tool --auto-fix crash.log

# Generate tests
cli_tool --generate-tests crash.log

# Save report
cli_tool -o report.txt crash.log

# From stdin
cat crash.log | cli_tool -

# JSON output
cli_tool --format json crash.log

# With config
cli_tool --config .aidebuggerrc crash.log
```

---

**You're all set!** Start with the simple example, then try it on your own crashes. Happy debugging! 🚀
