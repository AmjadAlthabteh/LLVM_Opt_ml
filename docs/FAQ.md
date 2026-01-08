# AI Debugger - Frequently Asked Questions (FAQ)

Quick answers to common questions about the AI Debugger.

## Table of Contents
- [General Questions](#general-questions)
- [Capabilities](#capabilities)
- [Accuracy and Trust](#accuracy-and-trust)
- [Usage and Integration](#usage-and-integration)
- [Comparison with Other Tools](#comparison-with-other-tools)
- [Advanced Usage](#advanced-usage)

---

## General Questions

### What is AI Debugger?

AI Debugger is an automated crash analysis tool that:
1. Takes a stack trace as input
2. Identifies the bug type (null pointer, buffer overflow, etc.)
3. Explains what went wrong in plain English
4. Suggests code fixes
5. Generates regression tests

It's designed to speed up debugging by automating root cause analysis.

### Is this really AI? Does it use machine learning?

The current version uses **heuristic pattern matching**, not machine learning. "AI" refers to the intelligent analysis and decision-making:
- Pattern recognition across 14 bug categories
- Confidence scoring based on multiple signals
- Natural language explanation generation
- Context-aware code fix suggestions

The architecture is **ML-ready** for future enhancements.

### Is it free and open source?

Yes, AI Debugger is released under the MIT License. You can:
- Use it commercially
- Modify the source code
- Distribute it
- Integrate it into your products

No attribution required (but appreciated).

### What platforms are supported?

- **Linux** (Ubuntu, Debian, Fedora, CentOS, etc.)
- **macOS** (10.12+)
- **Windows** (Windows 7+, MSVC 2017+)

Built with portable C++17 code.

### What programming languages does it support?

Currently: **C and C++**

The stack trace parser works with any C/C++ program compiled with debug symbols. Support for Rust, Go, and other languages is planned.

### Do I need LLVM?

**No.** LLVM integration is optional:
- **Without LLVM**: Full functionality using heuristics
- **With LLVM**: Enhanced analysis using LLVM IR (future feature)

Most users don't need LLVM.

---

## Capabilities

### What types of bugs can it detect?

AI Debugger recognizes 14 bug categories:

**Memory Errors** (most common):
- Null pointer dereference
- Buffer overflow / out-of-bounds access
- Use-after-free
- Double free
- Memory leak

**Concurrency Issues**:
- Race conditions
- Deadlocks

**Logic Errors**:
- Logic errors (incorrect algorithm)
- Assertion failures

**Type & Arithmetic**:
- Type errors (cast failures, type mismatch)
- Arithmetic errors (division by zero, overflow)

**Resource Issues**:
- Resource exhaustion (out of memory, file descriptors)

**Unknown**: Fallback for unrecognized patterns

### What debugger formats are supported?

- **GDB** (GNU Debugger) - Linux/Unix standard
- **LLDB** (LLVM Debugger) - macOS default, also Linux
- **MSVC** (Visual Studio Debugger) - Windows
- **Generic** (best-effort pattern matching)

Auto-detects the format automatically.

### Can it analyze core dumps directly?

Not directly. You need to:
1. Extract the stack trace from the core dump first
2. Feed the stack trace to AI Debugger

```bash
# Extract trace from core dump
gdb ./program core -batch -ex "bt full" > crash.log

# Analyze
./cli_tool crash.log
```

### Can it automatically fix my code?

Yes, with the `--apply-fix` flag, but:
- **Automatic backups** are created (`.bak` files)
- **Preview mode** available (`--preview`) to see changes first
- **Manual review recommended** for low-confidence fixes (<70%)
- **Not all bugs can be auto-fixed** (complex logic errors need human judgment)

Best practice: Use `--preview` first, then apply manually.

### Does it generate working tests?

It generates **test templates** that often work out-of-the-box, but:
- **Simple cases**: Usually compile and run immediately
- **Complex cases**: May need minor adjustments (setup code, mocking)
- **Supports 4 frameworks**: GTest, Catch2, Boost.Test, doctest

The tests are designed to prevent regression of the fixed bug.

### Can it handle multiple crashes at once?

Currently, one crash at a time. For batch processing:
```bash
# Analyze all crash logs in a directory
for file in crashes/*.log; do
    ./cli_tool "$file" > "analysis_$(basename $file)"
done
```

Batch mode with aggregated reporting is planned for future versions.

---

## Accuracy and Trust

### How accurate is it?

Accuracy depends on confidence score:
- **High confidence (85-100%)**: ~90% accurate
- **Medium confidence (70-84%)**: ~70% accurate
- **Low confidence (<70%)**: ~50% accurate, manual review needed

Accuracy improves with:
- Complete stack traces (not truncated)
- Debug symbols enabled (`-g` compiler flag)
- Clear error signals (SIGSEGV, exception types)

### When should I trust the predictions?

Trust guidelines:
- **Always trust**: Confidence >90% with clear error signal
- **Probably trust**: Confidence 80-90% with supporting evidence
- **Verify first**: Confidence 70-80%, review suggested fix
- **Manual review**: Confidence <70%, use as starting point only

Run with `--verbose` to see reasoning.

### What if it's wrong?

If the prediction is incorrect:
1. **Don't apply the fix** - low confidence indicates uncertainty
2. **Check alternative predictions** - use `--json` to see all possibilities
3. **Add more context** - include full stack trace, error messages
4. **Manual debugging** - AI Debugger is a tool, not a replacement for debugging

The tool provides **assistance**, not guaranteed solutions.

### Can it detect all bugs?

**No.** AI Debugger focuses on **crash analysis**. It cannot detect:
- Bugs that don't crash (logic errors producing wrong results)
- Performance issues (slowness, memory leaks that don't crash)
- Security vulnerabilities (unless they cause crashes)
- Data corruption (silent errors)
- Race conditions that don't crash during testing

For comprehensive testing, combine with:
- Static analyzers (clang-tidy, cppcheck)
- Dynamic analyzers (AddressSanitizer, ThreadSanitizer)
- Fuzzing (AFL, libFuzzer)
- Code review

### Why is confidence so low for my crash?

Low confidence (<70%) occurs when:
1. **Unusual crash pattern** - doesn't match known signatures
2. **Ambiguous signals** - multiple possible causes
3. **Incomplete stack trace** - missing frames or context
4. **Generic function names** - can't determine purpose
5. **Custom code patterns** - company-specific idioms

To improve confidence:
- Provide complete stack traces
- Include error messages (signals, exceptions)
- Add function parameters to trace (use `bt full` in GDB)
- Consider custom heuristics for your codebase

---

## Usage and Integration

### How do I get started quickly?

Run the instant demo:
```bash
# Windows
TRY_NOW.bat

# Linux/macOS
./TRY_NOW.sh
```

This analyzes a sample crash in <5 seconds, no setup required.

### How do I analyze my own crash?

```bash
# Step 1: Get stack trace
gdb ./your_program core -batch -ex "bt" > crash.log

# Step 2: Analyze
./cli_tool crash.log

# Step 3: (Optional) Apply fix
./cli_tool --preview crash.log  # Preview first
./cli_tool --apply-fix crash.log  # Apply if confident
```

See `GETTING_STARTED.md` for detailed walkthrough.

### Can I integrate this into my CI/CD pipeline?

Yes! Example for GitHub Actions:
```yaml
- name: Analyze test crashes
  if: failure()
  run: |
    find . -name "core.*" -exec gdb ./test_binary {} -batch -ex "bt" \; > crash.log
    ./cli_tool --json crash.log > analysis.json

- name: Upload analysis
  uses: actions/upload-artifact@v2
  with:
    name: crash-analysis
    path: analysis.json
```

See `docs/CI_CD_INTEGRATION.md` (planned) for more examples.

### Can I use it as a library in my C++ code?

Yes, AI Debugger is designed as a library:
```cpp
#include "ai_debugger/AIDebugger.h"

AIDebugger debugger;
debugger.loadConfig(".aidebuggerrc");

std::string traceContent = /* read crash log */;
auto trace = debugger.parseTrace(traceContent);
auto cause = debugger.predictRootCause(trace);

if (cause.confidence > 85) {
    auto explanation = debugger.generateExplanation(cause);
    std::cout << explanation.getMarkdown() << std::endl;
}
```

See `docs/API.md` for full API reference.

### How do I configure it for my project?

Create `.aidebuggerrc` in your project root:
```ini
# Source code location
source_directory=./src

# Test framework (gtest, catch2, boost, doctest)
test_framework=gtest

# Minimum confidence for auto-fix (0-100)
confidence_threshold=85

# Output format (markdown, html, plain, json)
output_format=markdown

# Auto-apply fixes (true/false) - USE WITH CAUTION
auto_apply_fixes=false
```

See `CONFIG_GUIDE.md` for all options.

### Does it work with sanitizers (ASan, TSan)?

Yes! In fact, sanitizer output often improves accuracy:
```bash
# Compile with AddressSanitizer
g++ -fsanitize=address -g program.cpp -o program

# Run and capture crash
./program 2>&1 | tee crash_asan.log

# Analyze
./cli_tool crash_asan.log
```

Sanitizer output includes extra context (allocation history, thread IDs) that helps prediction.

---

## Comparison with Other Tools

### How is this different from GDB/LLDB?

| Feature | GDB/LLDB | AI Debugger |
|---------|----------|-------------|
| **Interactive debugging** | Yes | No |
| **Set breakpoints** | Yes | No |
| **Examine variables** | Yes | No |
| **Automated root cause** | No | Yes |
| **Explain bugs** | No | Yes |
| **Suggest fixes** | No | Yes |
| **Generate tests** | No | Yes |
| **Batch processing** | Difficult | Easy |

**Use case**: AI Debugger is for **post-mortem analysis** of crashes, not interactive debugging. It complements debuggers, doesn't replace them.

### How is this different from static analyzers (clang-tidy, cppcheck)?

| Feature | Static Analyzers | AI Debugger |
|---------|------------------|-------------|
| **Analyze source code** | Yes | No |
| **Find potential bugs** | Yes | No |
| **Analyze crashes** | No | Yes |
| **Requires crash** | No | Yes |
| **False positives** | Many | Few |
| **Runtime info** | No | Yes |

**Use case**: Static analyzers find potential bugs before runtime. AI Debugger analyzes actual crashes that occurred.

### How is this different from crash reporting tools (Sentry, Bugsnag)?

| Feature | Crash Reporters | AI Debugger |
|---------|-----------------|-------------|
| **Collect crashes** | Yes | No |
| **Aggregate reports** | Yes | No |
| **Root cause analysis** | Basic | Advanced |
| **Fix suggestions** | No | Yes |
| **Test generation** | No | Yes |
| **Privacy** | Cloud-based | Local-only |

**Use case**: Crash reporters collect crashes from users. AI Debugger analyzes them locally with deeper technical analysis.

### Can I use it with valgrind?

Yes! Valgrind output works well:
```bash
valgrind --leak-check=full ./program 2>&1 | tee valgrind.log
./cli_tool valgrind.log
```

Valgrind provides detailed memory error info that improves accuracy.

---

## Advanced Usage

### Can I add custom bug patterns?

Currently, custom heuristics require code modifications:
1. Edit `src/RootCausePredictor.cpp`
2. Add pattern matching logic
3. Recompile

**Planned feature**: Plugin system for loadable custom heuristics (`.so`/`.dll`).

### Can it learn from feedback?

Not yet. Current version doesn't store feedback or improve over time.

**Planned feature**: Feedback system to:
- Mark predictions as correct/incorrect
- Build knowledge base of project-specific patterns
- Improve accuracy over time

### How do I analyze optimized binaries?

Optimized binaries (`-O2`, `-O3`) make analysis harder:
- Inlined functions don't appear in stack trace
- Optimized-away code loses debug info
- Reordered code confuses line numbers

**Best practice**: For crash analysis, use debug builds (`-g -O0`).

If you must analyze optimized binaries:
```bash
# Compile with some debug info
g++ -O2 -g -fno-omit-frame-pointer program.cpp
```

### Can I use it for embedded systems?

Yes, if you can extract a stack trace:
1. Capture crash dump from device (UART, JTAG, etc.)
2. Post-process to standard format (function names, addresses)
3. Analyze with AI Debugger

Some embedded debuggers (OpenOCD, J-Link) can output GDB-compatible traces.

### How do I analyze multi-threaded crashes?

Get traces for **all threads**:
```bash
# GDB
gdb ./program core
(gdb) thread apply all bt full
(gdb) quit

# Save to file
gdb ./program core -batch -ex "thread apply all bt full" > crash.log

# Analyze
./cli_tool crash.log
```

AI Debugger will analyze all threads and identify potential race conditions or deadlocks.

### Can I customize the fix suggestions?

Not through configuration. To customize:
1. Use `--preview` to see suggested fixes
2. Manually modify the fix
3. Apply manually

**Planned feature**: Fix templates and customizable fix strategies.

### How do I handle false positives?

If AI Debugger identifies the wrong bug type:
1. **Check confidence** - low confidence indicates uncertainty
2. **Use --verbose** - see what patterns were matched
3. **Try --json** - review alternative predictions
4. **Provide more context** - full trace, error messages
5. **Manual review** - trust your judgment over the tool

If false positives are common, consider filing an issue with examples.

---

## Performance and Scalability

### How fast is it?

Typical performance:
- **Small crash** (50 lines): <10ms
- **Medium crash** (200 lines): <50ms
- **Large crash** (1000+ lines): <500ms

Measured on modern CPU (Intel i7). Actual performance varies by system.

### Can it handle huge crash logs?

Yes, but performance degrades:
- **<1000 lines**: Fast (<100ms)
- **1000-10000 lines**: Moderate (~1 second)
- **>10000 lines**: Slow (>5 seconds)

For huge logs, truncate to relevant portion:
```bash
tail -500 huge_crash.log > crash.log
```

### Does it use multiple CPU cores?

Current version is single-threaded per analysis.

For batch processing, parallelize manually:
```bash
find crashes/ -name "*.log" | xargs -P4 -I{} ./cli_tool {}
```

**Planned feature**: Multi-threaded batch processing.

---

## Privacy and Security

### Does it send data to external servers?

**No.** AI Debugger is 100% local:
- No network connections
- No telemetry
- No data collection
- No external API calls

Your crash data never leaves your machine.

### Is it safe to use on proprietary code?

Yes. The tool:
- Runs entirely locally
- Doesn't log or store your code
- Doesn't transmit data
- Doesn't require internet

Safe for closed-source, proprietary, or confidential projects.

### Can auto-fix introduce security vulnerabilities?

Potentially, yes:
- **Null checks** might hide bugs instead of fixing root cause
- **Error handling** might silently ignore errors
- **Bounds checks** might break assumptions

**Best practice**:
1. Review all auto-generated fixes
2. Test thoroughly after applying
3. Use static analysis to verify safety
4. Never auto-apply to production without testing

---

## Troubleshooting

### It says "parse error" on my crash log

See the [Troubleshooting Guide](TROUBLESHOOTING.md#parse-errors) for detailed solutions.

Quick checks:
1. Verify trace has function names and line numbers
2. Check file encoding (UTF-8 or ASCII)
3. Try with verbose mode: `--verbose`

### The suggested fix doesn't work

This is normal for:
- Low confidence predictions (<70%)
- Complex bugs requiring deep understanding
- Bugs with multiple root causes

AI Debugger provides **suggestions**, not guaranteed solutions. Manual review always recommended.

### Where can I get help?

1. Check documentation:
   - `GETTING_STARTED.md` - Beginner guide
   - `docs/TROUBLESHOOTING.md` - Common issues
   - `docs/API.md` - API reference

2. Run diagnostics:
   ```bash
   ./cli_tool --version
   ./quick_test.sh
   ```

3. File an issue:
   - GitHub Issues (include verbose output, crash log if possible)

---

## Future Plans

### What features are planned?

Upcoming features:
1. **Machine learning integration** - Improve accuracy with trained models
2. **Batch processing** - Analyze multiple crashes with aggregated reports
3. **Interactive mode** - Menu-driven analysis with real-time feedback
4. **Custom heuristics plugin system** - Load custom patterns without recompiling
5. **Knowledge base** - Learn project-specific patterns
6. **More languages** - Rust, Go, Python support
7. **CI/CD templates** - Pre-built GitHub Actions, GitLab CI configs
8. **Web UI** - Browser-based interface for teams
9. **Real-time monitoring** - Watch mode for continuous analysis

See GitHub project board for roadmap.

### How can I contribute?

Contributions welcome! See `CONTRIBUTING.md` (planned) for:
- Code style guide
- Development setup
- Testing requirements
- Pull request process

Quick ways to help:
- Report bugs with crash examples
- Suggest improvements
- Add test cases for edge cases
- Improve documentation
- Share use cases and feedback

---

## Quick Reference

| Question | Answer |
|----------|--------|
| **Is it free?** | Yes, MIT licensed |
| **Does it work on Windows?** | Yes, Windows/macOS/Linux |
| **Can it fix bugs automatically?** | Yes, with `--apply-fix`, but review first |
| **How accurate is it?** | ~90% for high-confidence cases |
| **Does it require internet?** | No, 100% local |
| **Can I use it commercially?** | Yes, no restrictions |
| **What languages are supported?** | C and C++ (Rust/Go planned) |
| **Does it replace debuggers?** | No, it complements them |
| **Is my code private?** | Yes, no data transmitted |
| **Can I customize patterns?** | Not yet (plugin system planned) |

---

For more information:
- [Getting Started](../GETTING_STARTED.md)
- [Architecture](ARCHITECTURE.md)
- [API Reference](API.md)
- [Troubleshooting](TROUBLESHOOTING.md)
- [Configuration Guide](../CONFIG_GUIDE.md)
