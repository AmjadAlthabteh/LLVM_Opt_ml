# AI Debugger

Advanced C++ debugging tool powered by AI techniques for automated root cause analysis, fix suggestion, and regression test generation.

## Features

- **Stack Trace Parsing**: Parses stack traces from GDB, LLDB, MSVC, and other debuggers
- **Call Graph Analysis**: Builds and analyzes call graphs to understand code flow
- **Root Cause Prediction**: Identifies bug categories with confidence scores
  - Null pointer dereferences
  - Buffer overflows
  - Use-after-free
  - Double free
  - Memory leaks
  - Race conditions
  - Deadlocks
  - And more
- **Natural Language Explanations**: Generates human-readable bug explanations
- **Automated Fix Suggestions**: Proposes code fixes with one-click application
- **Test Generation**: Creates regression tests to prevent future occurrences

## Architecture

```
ai_debugger/
├── include/ai_debugger/     # Public headers
│   ├── StackTraceParser.h
│   ├── CallGraphAnalyzer.h
│   ├── RootCausePredictor.h
│   ├── ExplanationGenerator.h
│   ├── FixSuggester.h
│   ├── TestGenerator.h
│   └── AIDebugger.h
├── src/                     # Implementation files
├── examples/                # Usage examples
├── tests/                   # Unit tests
└── docs/                    # Documentation
```

## Building

### Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- Optional: LLVM (for advanced analysis)
- Optional: Google Test (for tests)

### Build Instructions

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Build Options

- `BUILD_TESTS=ON` - Build test suite (default: ON)
- `BUILD_EXAMPLES=ON` - Build examples (default: ON)
- `ENABLE_LLVM=ON` - Enable LLVM integration (default: OFF)

```bash
cmake -DENABLE_LLVM=ON -DBUILD_TESTS=ON ..
```

## Usage

### Simple API Example

```cpp
#include "ai_debugger/AIDebugger.h"

int main() {
    ai_debugger::AIDebugger debugger;

    std::string trace = /* stack trace from crash */;

    auto session = debugger.analyzeStackTrace(trace);

    std::cout << debugger.getReport(session);

    if (!session.suggested_fixes.empty()) {
        auto result = debugger.applyBestFix(session);
    }

    debugger.generateTests(session);

    return 0;
}
```

### Command-Line Tool

```bash
# Analyze stack trace
./cli_tool stacktrace.txt

# Generate report with verbose output
./cli_tool -v --output report.txt stacktrace.txt

# Auto-apply fixes and generate tests
./cli_tool --auto-fix --generate-tests stacktrace.txt

# Specify test framework
./cli_tool --framework catch2 --generate-tests stacktrace.txt
```

## Components

### StackTraceParser

Parses stack traces from multiple debugger formats:

```cpp
ai_debugger::StackTraceParser parser;
auto trace = parser.parse(trace_text);
```

### CallGraphAnalyzer

Analyzes function call relationships:

```cpp
ai_debugger::CallGraphAnalyzer analyzer;
analyzer.buildFromStackTrace(trace);
auto patterns = analyzer.detectPatterns();
```

### RootCausePredictor

Predicts bug root causes with confidence scores:

```cpp
ai_debugger::RootCausePredictor predictor;
auto causes = predictor.predict(trace, analyzer);
auto best_cause = causes[0];
```

### ExplanationGenerator

Generates human-readable explanations:

```cpp
ai_debugger::ExplanationGenerator generator;
auto explanation = generator.generate(trace, cause, graph);
std::cout << explanation.toMarkdown();
```

### FixSuggester

Suggests code fixes:

```cpp
ai_debugger::FixSuggester suggester;
auto fixes = suggester.suggestFixes(root_cause, trace);
auto result = suggester.applyFix(fixes[0]);
```

### TestGenerator

Generates regression tests:

```cpp
ai_debugger::TestGenerator generator;
generator.setFramework(TestFramework::GTEST);
auto suite = generator.generateRegressionTests(cause, fix, trace);
generator.writeTestFile(suite);
```

## Supported Bug Categories

- Memory Errors
- Null Pointer Dereference
- Buffer Overflow/Underflow
- Use After Free
- Double Free
- Memory Leak
- Race Condition
- Deadlock
- Logic Error
- Type Error
- Arithmetic Error
- Resource Exhaustion
- Assertion Failure

## Test Frameworks Supported

- Google Test (gtest)
- Catch2
- Boost.Test
- doctest

## Advanced Configuration

```cpp
ai_debugger::Config config;
config.source_directory = "/path/to/source";
config.test_framework = TestFramework::GTEST;
config.verbose = true;
config.auto_fix = false;
config.auto_test = true;

AIDebugger debugger;
debugger.setSourceDirectory(config.source_directory);
debugger.setVerbose(config.verbose);
```

## Integration Examples

### CI/CD Pipeline

```bash
# In your CI script
if [ $? -ne 0 ]; then
    ./cli_tool --generate-tests crash.log
    cat ai_debugger_report.txt >> $CI_COMMENT
fi
```

### IDE Integration

Integrate with VSCode, CLion, or Visual Studio for inline debugging assistance.

### Custom Workflows

```cpp
auto debugger = std::make_unique<AIDebugger>();
debugger->enableAutoFix(true);
debugger->setTestFramework(TestFramework::CATCH2);

for (auto& crash_file : crash_reports) {
    auto session = debugger->analyzeFromFile(crash_file);
    process_results(session);
}
```

## Performance

- Stack trace parsing: <1ms for typical traces
- Root cause prediction: <10ms
- Fix generation: <5ms
- Test generation: <20ms per test suite

## Limitations

- Source code access required for fix application
- Pattern-based prediction (ML models optional)
- Best results with complete debug symbols

## Future Enhancements

- Deep learning models for improved prediction
- Static analysis integration
- Dynamic analysis support
- Multi-language support (Python, Java, Rust)
- IDE plugins
- Web interface

## Contributing

Contributions welcome. Areas of interest:

- Additional debugger format parsers
- More bug pattern heuristics
- ML model integration
- Performance optimizations
- Documentation improvements

## License

MIT License

## Support

For issues and questions:
- GitHub Issues
- Documentation: docs/
- Examples: examples/
