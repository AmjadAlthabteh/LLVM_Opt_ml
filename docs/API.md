# AI Debugger API Reference

## Core Classes

### AIDebugger

Main interface for the debugging system.

```cpp
namespace ai_debugger {
    class AIDebugger {
    public:
        AIDebugger();

        DebugSession analyzeStackTrace(const std::string& trace_text);
        DebugSession analyzeFromFile(const std::string& trace_file);

        void setSourceDirectory(const std::string& src_dir);
        void setTestFramework(TestFramework framework);
        void setVerbose(bool verbose);
        void enableAutoFix(bool enable);
        void enableTestGeneration(bool enable);

        std::string getReport(const DebugSession& session) const;
        bool saveReport(const DebugSession& session, const std::string& output_path) const;

        FixApplication applyBestFix(const DebugSession& session);
        std::vector<FixApplication> applyAllFixes(const DebugSession& session);

        bool generateTests(const DebugSession& session);
    };
}
```

### StackTraceParser

Parses stack traces from various debugger formats.

```cpp
class StackTraceParser {
public:
    StackTraceParser();

    std::optional<StackTrace> parse(const std::string& trace_text);
    std::optional<StackTrace> parseGDB(const std::string& gdb_output);
    std::optional<StackTrace> parseLLDB(const std::string& lldb_output);
    std::optional<StackTrace> parseMSVC(const std::string& msvc_output);

    void setVerbose(bool verbose);
};
```

### CallGraphAnalyzer

Analyzes function call relationships.

```cpp
class CallGraphAnalyzer {
public:
    CallGraphAnalyzer();

    void buildFromStackTrace(const StackTrace& trace);

    std::vector<CallGraphNode> getNodes() const;
    std::optional<CallGraphNode> getNode(const std::string& function) const;

    std::vector<CallPattern> detectPatterns() const;
    std::vector<std::string> findCriticalPath() const;

    bool isRecursive(const std::string& function) const;
    std::vector<std::string> getRecursionChain(const std::string& function) const;
};
```

### RootCausePredictor

Predicts root causes of bugs.

```cpp
class RootCausePredictor {
public:
    RootCausePredictor();

    std::vector<RootCause> predict(
        const StackTrace& trace,
        const CallGraphAnalyzer& graph_analyzer
    );

    RootCause getMostLikelyCause(
        const StackTrace& trace,
        const CallGraphAnalyzer& graph_analyzer
    );

    void loadKnowledgeBase(const std::string& kb_path);
};
```

### ExplanationGenerator

Generates human-readable explanations.

```cpp
class ExplanationGenerator {
public:
    ExplanationGenerator();

    Explanation generate(
        const StackTrace& trace,
        const RootCause& root_cause,
        const CallGraphAnalyzer& graph
    );

    std::string explainBugCategory(BugCategory category);
    std::string explainCallFlow(const std::vector<StackFrame>& frames);
    std::string suggestPreventionStrategies(const RootCause& cause);

    void setDetailLevel(int level);
};
```

### FixSuggester

Suggests and applies code fixes.

```cpp
class FixSuggester {
public:
    FixSuggester();

    std::vector<CodeFix> suggestFixes(
        const RootCause& root_cause,
        const StackTrace& trace
    );

    FixApplication applyFix(const CodeFix& fix, bool create_backup = true);
    void rollbackFix(const FixApplication& application);

    void setSourceRoot(const std::string& root_path);
    void setDryRun(bool dry_run);
};
```

### TestGenerator

Generates regression tests.

```cpp
class TestGenerator {
public:
    TestGenerator();

    TestSuite generateRegressionTests(
        const RootCause& root_cause,
        const CodeFix& fix,
        const StackTrace& original_trace
    );

    std::vector<TestCase> generateEdgeCaseTests(
        const RootCause& root_cause,
        const std::string& function_name
    );

    void setFramework(TestFramework framework);
    bool writeTestFile(const TestSuite& suite);
};
```

## Data Structures

### StackTrace

```cpp
struct StackTrace {
    std::vector<StackFrame> frames;
    std::string error_message;
    std::string exception_type;
    int signal_number;
};
```

### StackFrame

```cpp
struct StackFrame {
    std::string function_name;
    std::string mangled_name;
    SourceLocation location;
    std::string module;
    uintptr_t address;
    std::vector<std::string> parameters;
};
```

### RootCause

```cpp
struct RootCause {
    BugCategory category;
    std::string description;
    SourceLocation location;
    double confidence;
    std::vector<std::string> contributing_factors;
    std::vector<StackFrame> relevant_frames;
};
```

### CodeFix

```cpp
struct CodeFix {
    FixType type;
    std::string description;
    SourceLocation location;
    std::string original_code;
    std::string fixed_code;
    double confidence;
};
```

### TestCase

```cpp
struct TestCase {
    std::string name;
    std::string description;
    std::string test_code;
    std::string setup_code;
    std::string teardown_code;
    bool is_regression_test;
};
```

## Enumerations

### BugCategory

```cpp
enum class BugCategory {
    MEMORY_ERROR,
    NULL_POINTER,
    BUFFER_OVERFLOW,
    USE_AFTER_FREE,
    DOUBLE_FREE,
    MEMORY_LEAK,
    RACE_CONDITION,
    DEADLOCK,
    LOGIC_ERROR,
    TYPE_ERROR,
    ARITHMETIC_ERROR,
    RESOURCE_EXHAUSTION,
    ASSERTION_FAILURE,
    UNKNOWN
};
```

### FixType

```cpp
enum class FixType {
    NULL_CHECK,
    BOUNDS_CHECK,
    INITIALIZATION,
    MEMORY_CLEANUP,
    SYNCHRONIZATION,
    REFACTORING,
    ASSERTION,
    ERROR_HANDLING,
    CUSTOM
};
```

### TestFramework

```cpp
enum class TestFramework {
    GTEST,
    CATCH2,
    BOOST_TEST,
    DOCTEST,
    CUSTOM
};
```

## Usage Examples

### Basic Workflow

```cpp
#include "ai_debugger/AIDebugger.h"

ai_debugger::AIDebugger debugger;
auto session = debugger.analyzeStackTrace(trace_text);
std::cout << debugger.getReport(session);
```

### Advanced Configuration

```cpp
ai_debugger::Config config;
config.source_directory = "/path/to/source";
config.verbose = true;
config.auto_fix = true;

AIDebugger debugger;
debugger.setSourceDirectory(config.source_directory);
debugger.setVerbose(config.verbose);
debugger.enableAutoFix(config.auto_fix);
```

### Custom Processing

```cpp
StackTraceParser parser;
auto trace = parser.parse(input);

CallGraphAnalyzer analyzer;
analyzer.buildFromStackTrace(*trace);

RootCausePredictor predictor;
auto causes = predictor.predict(*trace, analyzer);

ExplanationGenerator explainer;
auto explanation = explainer.generate(*trace, causes[0], analyzer);
```

## Return Values

Most methods return:
- `std::optional<T>` for operations that may fail
- `bool` for success/failure operations
- Structs containing detailed results

Always check return values:

```cpp
auto trace_opt = parser.parse(input);
if (trace_opt) {
    // Use trace_opt.value()
} else {
    // Handle parse failure
}
```

## Thread Safety

- All classes are **not** thread-safe by default
- Use separate instances per thread
- Or protect with mutexes for shared access

## Error Handling

```cpp
try {
    auto session = debugger.analyzeStackTrace(trace);
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```
