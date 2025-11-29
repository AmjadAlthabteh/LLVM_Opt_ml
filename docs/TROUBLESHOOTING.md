# AI Debugger - Troubleshooting Guide

This guide covers common issues and their solutions when using the AI Debugger.

## Table of Contents
1. [Installation Issues](#installation-issues)
2. [Parse Errors](#parse-errors)
3. [Analysis Issues](#analysis-issues)
4. [Fix Application Problems](#fix-application-problems)
5. [Test Generation Issues](#test-generation-issues)
6. [Configuration Problems](#configuration-problems)
7. [Performance Issues](#performance-issues)
8. [Platform-Specific Issues](#platform-specific-issues)

---

## Installation Issues

### Build Fails with "CMake not found"

**Problem**: CMake is not installed or not in PATH

**Solution**:
```bash
# Linux (Ubuntu/Debian)
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
# Or use: choco install cmake
```

### Build Fails with "C++17 required"

**Problem**: Compiler doesn't support C++17

**Solution**:
```bash
# Check your compiler version
g++ --version      # Need GCC 7+ or Clang 5+
clang++ --version

# Update compiler:
# Linux
sudo apt-get install g++-9

# macOS
brew install gcc@11

# Windows: Install Visual Studio 2017 or newer
```

### Tests Don't Build

**Problem**: Test framework (GTest) not found

**Solution**:
```bash
# Option 1: Install GTest system-wide
# Linux
sudo apt-get install libgtest-dev

# macOS
brew install googletest

# Option 2: Disable tests during build
cmake -DBUILD_TESTS=OFF ..

# Option 3: Use bundled GTest (automatic)
# CMake will download it automatically if not found
```

### LLVM Integration Fails

**Problem**: LLVM libraries not found

**Solution**:
```bash
# Only needed if you want LLVM integration
# Otherwise, build without it:
cmake -DUSE_LLVM=OFF ..

# To install LLVM:
# Linux
sudo apt-get install llvm-dev

# macOS
brew install llvm

# Windows: Download from https://releases.llvm.org/
```

---

## Parse Errors

### "Failed to parse stack trace"

**Problem**: Stack trace format not recognized

**Solution**:
1. Check that your trace includes function names and line numbers
2. Supported formats:
   - GDB: `#0 0xaddr in func() at file.cpp:42`
   - LLDB: `frame #0: 0xaddr module\`func() at file.cpp:42`
   - MSVC: `func() Line 42 C++`
   - Generic: Function names with file:line references

3. If using custom format, add more context:
```bash
# BAD: Incomplete trace
func1
func2

# GOOD: With file and line info
func1() at main.cpp:42
func2() at utils.cpp:15
```

4. Check file encoding (must be UTF-8 or ASCII)

### "No stack frames found"

**Problem**: Input doesn't contain recognizable stack frames

**Solution**:
```bash
# Verify your crash log has stack frames:
cat crash.log | grep -E "#[0-9]+|frame #|at .+:[0-9]+"

# If no output, your file might be:
# 1. A plain error message (not a stack trace)
# 2. In an unsupported format
# 3. Missing debug symbols

# Generate a proper stack trace:
# GDB
gdb ./program core
(gdb) bt full
(gdb) quit

# LLDB
lldb ./program -c core
(lldb) bt all
(lldb) quit
```

### "Symbol demangling failed"

**Problem**: C++ symbols couldn't be demangled

**Solution**:
This is a warning, not an error. Analysis continues with mangled names.

To get better results:
1. Build with debug symbols: `g++ -g`
2. Use `c++filt` to demangle before analysis:
```bash
c++filt < crash.log > crash_demangled.log
./cli_tool crash_demangled.log
```

### "Error at line X: Unexpected format"

**Problem**: Mixed format or corruption in trace

**Solution**:
```bash
# Check line X in your file:
sed -n 'Xp' crash.log

# Common issues:
# 1. Mixed GDB and LLDB output - use one debugger
# 2. Non-ASCII characters - convert to UTF-8
# 3. Binary data in file - extract text only

# Clean the trace:
grep -E "^#[0-9]+|^frame #" crash.log > clean_trace.log
```

---

## Analysis Issues

### "Low confidence prediction (<70%)"

**Problem**: AI Debugger isn't confident about the bug type

**Solution**:
1. **This is normal** for unusual crashes. Manual review recommended.

2. Check what hints were found:
```bash
./cli_tool --verbose crash.log
```

3. Improve confidence by adding context:
   - Include full stack trace (not truncated)
   - Add error messages (SIGSEGV, exception type)
   - Include function parameters if available

4. Look at alternative predictions:
```bash
./cli_tool --json crash.log | jq '.alternatives'
```

5. For recurring similar crashes, consider creating custom heuristics

### "Root cause: Unknown"

**Problem**: No matching patterns found

**Solution**:
1. Check if this is really a crash or just normal program output
2. Ensure stack trace shows the actual crash point (not just exit handlers)
3. Try analyzing with verbose mode:
```bash
./cli_tool --verbose crash.log
```

4. Common reasons:
   - **Exit handlers**: Trace shows `exit()`, `atexit()` - not the real crash
   - **Signal handlers**: Trace shows signal handling code - need original crash
   - **Custom crashes**: Application uses custom error handling

5. Get the real crash:
```bash
# Disable signal handlers temporarily
# Or use core dump directly
gdb ./program core
(gdb) bt   # Get backtrace before signal handler
```

### "Multiple possible bug types"

**Problem**: Several bug types have similar confidence

**Solution**:
This indicates ambiguous crash. Check all suggestions:
```bash
./cli_tool --verbose --json crash.log
```

Look at the top 2-3 predictions and investigate each manually.

### "No crash detected in trace"

**Problem**: Stack trace looks normal (no crash indicators)

**Solution**:
1. Make sure you're providing a crash log, not normal program output
2. Check that the trace includes error information:
   - Signal numbers (SIGSEGV, SIGABRT, etc.)
   - Exception types (std::bad_alloc, etc.)
   - Error messages

3. If it's a hang (not crash), use different tools:
   - For deadlock: `gdb -p <pid>` then `thread apply all bt`
   - For infinite loop: Profiling tools (perf, gprof)

---

## Fix Application Problems

### "Source file not found"

**Problem**: Can't find source file to apply fix

**Solution**:
1. Ensure source file path in stack trace is correct:
```bash
# Check if file exists
ls -la path/from/trace.cpp
```

2. If path is relative, specify source directory:
```bash
./cli_tool --source-dir /path/to/project crash.log
```

3. Or use config file:
```ini
# .aidebuggerrc
source_directory=/path/to/project
```

4. If source path in trace is wrong, create symlink:
```bash
ln -s /actual/path /path/in/trace
```

### "Fix application failed: Permission denied"

**Problem**: No write access to source file

**Solution**:
```bash
# Option 1: Fix permissions
chmod u+w source_file.cpp

# Option 2: Use preview mode instead
./cli_tool --preview crash.log
# Then manually apply the fix

# Option 3: Output fix to different file
./cli_tool --apply-fix --output fixed_code.cpp crash.log
```

### "Applied fix but program still crashes"

**Problem**: Fix didn't solve the issue

**Solution**:
1. Check confidence score - low confidence fixes might not be correct
2. The crash might have multiple causes - fix one at a time
3. Rebuild after applying fix:
```bash
make clean
make
./program
```

4. Check if fix was applied correctly:
```bash
git diff source_file.cpp
```

5. Try alternative fixes:
```bash
./cli_tool --all-fixes crash.log
```

6. Manual debugging still recommended for low-confidence cases

### "Backup file collision"

**Problem**: `.bak` file already exists

**Solution**:
```bash
# Option 1: Remove old backup
rm source_file.cpp.bak

# Option 2: Specify different backup suffix
./cli_tool --backup-suffix .bak2 --apply-fix crash.log

# Option 3: Disable backups (not recommended)
./cli_tool --no-backup --apply-fix crash.log
```

---

## Test Generation Issues

### "Generated test doesn't compile"

**Problem**: Test code has syntax errors

**Solution**:
1. Check that test framework is installed:
```bash
# For GTest
find /usr -name "gtest.h"

# Install if missing
sudo apt-get install libgtest-dev
```

2. Verify framework version compatibility:
```bash
# AI Debugger supports:
# - Google Test 1.8+
# - Catch2 2.0+
# - Boost.Test 1.59+
# - doctest 2.0+
```

3. Manually adjust generated test:
```bash
# Generate to file
./cli_tool --gen-tests --test-framework gtest crash.log > test.cpp

# Edit as needed
vim test.cpp

# Compile
g++ -std=c++17 test.cpp -lgtest -lgtest_main -pthread
```

4. Some fixes require manual setup code - check comments in generated test

### "Test framework not supported"

**Problem**: Using unsupported test framework

**Solution**:
Supported frameworks:
- Google Test (gtest)
- Catch2 (catch2)
- Boost.Test (boost)
- doctest (doctest)

If using different framework:
```bash
# Generate for closest equivalent
./cli_tool --test-framework gtest crash.log > test_template.cpp

# Then manually convert to your framework
```

### "Generated test passes but bug still exists"

**Problem**: Test doesn't reproduce the crash

**Solution**:
1. Generated tests are templates - you may need to adjust:
   - Setup code (create objects, initialize state)
   - Input data (use actual crashing input)
   - Environment (threads, files, network)

2. For complex bugs, capture full program state:
```bash
# Use a debugger to understand exact crash conditions
gdb ./program core
(gdb) info locals
(gdb) print varname
```

3. Add that state to test setup

---

## Configuration Problems

### "Config file not found"

**Problem**: `.aidebuggerrc` not loaded

**Solution**:
1. Create config file:
```bash
# Copy sample
cp .aidebuggerrc.sample .aidebuggerrc

# Or create from scratch
cat > .aidebuggerrc << 'EOF'
source_directory=.
test_framework=gtest
confidence_threshold=70
output_format=markdown
auto_apply_fixes=false
EOF
```

2. Check search locations:
   - Current directory: `./.aidebuggerrc`
   - Home directory: `~/.aidebuggerrc`
   - Specified path: `./cli_tool --config /path/to/config`

3. Verify format:
```bash
# Must be key=value format, no spaces around =
# Comments start with #
```

### "Invalid configuration value"

**Problem**: Config option has invalid value

**Solution**:
Check valid values:
```ini
# source_directory: Any valid path
source_directory=/path/to/src

# test_framework: gtest, catch2, boost, doctest
test_framework=gtest

# confidence_threshold: 0-100
confidence_threshold=70

# output_format: markdown, html, plain, json
output_format=markdown

# auto_apply_fixes: true or false
auto_apply_fixes=false
```

### "Config option ignored"

**Problem**: CLI flags override config file

**Solution**:
CLI flags take precedence:
```bash
# This uses gtest (from CLI), not catch2 (from config)
./cli_tool --test-framework gtest crash.log
```

To use config file values, omit CLI flags.

---

## Performance Issues

### "Analysis takes too long"

**Problem**: Slow analysis on large crash logs

**Solution**:
1. Large trace files (>1000 lines) can be slow. Truncate if needed:
```bash
# Keep last 500 lines (usually sufficient)
tail -500 huge_crash.log > crash.log
```

2. Use faster output format:
```bash
# JSON is faster than markdown/HTML
./cli_tool --json crash.log
```

3. Disable verbose mode:
```bash
# Don't use --verbose for large files
./cli_tool crash.log
```

4. For batch processing, use multiple cores:
```bash
# Parallel analysis
find crashes/ -name "*.log" | xargs -P4 -I{} ./cli_tool {}
```

### "High memory usage"

**Problem**: AI Debugger uses too much RAM

**Solution**:
1. Typical usage: <50MB. If higher, check:
   - File size (huge logs use more memory)
   - Memory leaks (file a bug report)

2. For very large files, process in chunks:
```bash
# Split large crash log
split -l 100 huge_crash.log crash_part_

# Analyze each part
for f in crash_part_*; do ./cli_tool $f; done
```

---

## Platform-Specific Issues

### Windows: "Command not found"

**Problem**: Path not set correctly

**Solution**:
```cmd
REM Add to PATH
set PATH=%PATH%;C:\path\to\LLVM-MLOpt_wp\build\examples

REM Or use full path
C:\path\to\LLVM-MLOpt_wp\build\examples\cli_tool.exe crash.log
```

### Windows: "VCRUNTIME140.dll not found"

**Problem**: Visual C++ runtime missing

**Solution**:
Install Visual C++ Redistributable:
https://aka.ms/vs/17/release/vc_redist.x64.exe

### macOS: "Library not loaded"

**Problem**: Dynamic library path issue

**Solution**:
```bash
# Set library path
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# Or build static
cmake -DBUILD_SHARED_LIBS=OFF ..
make
```

### Linux: "Permission denied" on scripts

**Problem**: Script not executable

**Solution**:
```bash
chmod +x TRY_NOW.sh
chmod +x ANALYZE_MY_CRASH.sh
chmod +x *.sh
```

### Cross-Platform: Line Ending Issues

**Problem**: CRLF vs LF line endings

**Solution**:
```bash
# Convert to Unix format (LF)
dos2unix crash.log

# Or use Python
python -c "import sys; sys.stdout.write(open('crash.log').read().replace('\r\n', '\n'))" > fixed.log

# Configure git
git config core.autocrlf true  # Windows
git config core.autocrlf input # Linux/macOS
```

---

## Common Error Messages

### "ERROR_INVALID_INPUT (1)"

**Cause**: Invalid command-line arguments or input file

**Solution**:
```bash
# Check syntax
./cli_tool --help

# Common mistakes:
# - Missing input file
# - Typo in flag name
# - Invalid option value
```

### "ERROR_PARSE_ERROR (2)"

**Cause**: Could not parse stack trace format

**Solution**: See [Parse Errors](#parse-errors) section

### "ERROR_FILE_NOT_FOUND (3)"

**Cause**: Input file doesn't exist

**Solution**:
```bash
# Check file path
ls -la crash.log

# Use absolute path if relative path fails
./cli_tool /full/path/to/crash.log
```

### "ERROR_CONFIG_ERROR (5)"

**Cause**: Invalid configuration file

**Solution**:
```bash
# Validate config syntax
cat .aidebuggerrc

# Check for:
# - Typos in option names
# - Invalid values
# - Missing = sign
# - Spaces around = (not allowed)
```

### "ERROR_ANALYSIS_FAILED (6)"

**Cause**: Internal analysis error

**Solution**:
1. Try verbose mode for more details:
```bash
./cli_tool --verbose crash.log
```

2. If persistent, file a bug report with:
   - Crash log (if not sensitive)
   - Error message
   - Platform and version info

---

## Getting Help

If this guide doesn't solve your problem:

1. **Check documentation**:
   - `README.md` - Overview
   - `GETTING_STARTED.md` - Beginner guide
   - `docs/API.md` - API reference
   - `CONFIG_GUIDE.md` - Configuration options

2. **Run diagnostics**:
```bash
./cli_tool --version
./cli_tool --help
./quick_test.sh  # Verify installation
```

3. **Search existing issues**:
   - GitHub: https://github.com/your-repo/issues
   - Look for similar problems

4. **File a bug report**:
   - Include platform (OS, compiler version)
   - Include error messages (use `--verbose`)
   - Include minimal reproducing example
   - Include AI Debugger version

5. **Community support**:
   - Stack Overflow tag: `ai-debugger`
   - Discussions: GitHub Discussions tab

---

## Quick Reference

| Problem | Quick Solution |
|---------|----------------|
| Build fails | Install CMake, update compiler to C++17 |
| Parse error | Check trace format, add more context |
| Low confidence | Use `--verbose`, manual review recommended |
| File not found | Use `--source-dir` flag or config |
| Test won't compile | Install test framework, check version |
| Config ignored | CLI flags override config |
| Slow analysis | Truncate large traces, use JSON output |
| Permission denied | `chmod +x` on Linux/macOS |
| Missing DLL (Windows) | Install VC++ Redistributable |

---

## Best Practices to Avoid Issues

1. **Always use debug builds** for crash analysis (`-g` flag)
2. **Include full stack traces** (not truncated)
3. **Test configuration** with sample crashes first
4. **Preview fixes** before applying (`--preview` flag)
5. **Keep backups** (automatic, but verify)
6. **Start simple** (use TRY_NOW.sh first)
7. **Read error messages carefully** (they contain hints)
8. **Update regularly** to get bug fixes

---

For more information, see:
- [Getting Started Guide](../GETTING_STARTED.md)
- [Architecture Documentation](ARCHITECTURE.md)
- [API Reference](API.md)
- [Configuration Guide](../CONFIG_GUIDE.md)
