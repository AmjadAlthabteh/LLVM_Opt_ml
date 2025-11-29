# AI Debugger - System Architecture

## Table of Contents
1. [Overview](#overview)
2. [Component Architecture](#component-architecture)
3. [Data Flow](#data-flow)
4. [Core Components](#core-components)
5. [Heuristic Engine](#heuristic-engine)
6. [Extensibility Points](#extensibility-points)
7. [Performance Characteristics](#performance-characteristics)

---

## Overview

The AI Debugger is a modular crash analysis system that transforms raw stack traces into actionable intelligence. The system follows a pipeline architecture where each component performs a specific analysis stage, building upon the results of previous stages.

### Design Philosophy
- **Modularity**: Each component is independent and testable
- **Composability**: Components can be used standalone or in pipeline
- **Performance**: Sub-second analysis for typical crashes
- **Extensibility**: Plugin-ready architecture for custom heuristics
- **Clarity**: Natural language explanations for technical users

---

## Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    AIDebugger (Orchestrator)                 │
│  • Session Management  • Configuration  • Report Generation   │
└────────────┬────────────────────────────────────────────────┘
             │
             │  Coordinates all components
             │
    ┌────────┴────────┬────────────┬──────────────┬──────────┐
    │                 │            │              │          │
    v                 v            v              v          v
┌─────────┐    ┌──────────┐  ┌─────────┐  ┌──────────┐  ┌──────────┐
│ Stack   │    │ Call     │  │  Root   │  │Explanation│ │   Fix    │
│ Trace   │───>│ Graph    │─>│ Cause   │─>│Generator │─>│Suggester │
│ Parser  │    │ Analyzer │  │Predictor│  │          │  │          │
└─────────┘    └──────────┘  └─────────┘  └──────────┘  └─────┬────┘
     │                             │                            │
     │                             │                            v
     │                             │                      ┌──────────┐
     │                             └─────────────────────>│   Test   │
     │                                                     │Generator │
     │                                                     └──────────┘
     │
     v
Input: Raw stack trace (GDB/LLDB/MSVC/Generic format)
Output: Structured StackTrace object with parsed frames
```

### Component Dependencies
```
AIDebugger
├── StackTraceParser (no dependencies)
├── CallGraphAnalyzer (depends on: StackTraceParser)
├── RootCausePredictor (depends on: StackTraceParser, CallGraphAnalyzer)
├── ExplanationGenerator (depends on: RootCausePredictor)
├── FixSuggester (depends on: RootCausePredictor)
└── TestGenerator (depends on: RootCausePredictor)
```

---

## Data Flow

### 1. Parse Phase
```
Raw Stack Trace
      │
      v
[Format Detection]
      │
      ├─> GDB format     → GDB-specific parser
      ├─> LLDB format    → LLDB-specific parser
      ├─> MSVC format    → MSVC-specific parser
      └─> Generic format → Regex-based parser
      │
      v
[Symbol Demangling]
      │
      v
StackTrace{frames, errorInfo, metadata}
```

### 2. Analysis Phase
```
StackTrace
      │
      v
[Call Graph Construction]
      │
      ├─> Build directed graph
      ├─> Detect recursion
      └─> Identify critical path
      │
      v
CallGraph{nodes, edges, patterns}
      │
      v
[Pattern Matching Engine]
      │
      ├─> Memory error patterns
      ├─> Concurrency patterns
      ├─> Logic error patterns
      └─> Resource patterns
      │
      v
RootCause{category, confidence, location, factors}
```

### 3. Generation Phase
```
RootCause
      │
      ├────────────────┬──────────────┬──────────────┐
      │                │              │              │
      v                v              v              v
[Explanation]    [Fix Suggestions] [Test Cases]  [Report]
      │                │              │              │
      v                v              v              v
  Markdown          Code Diffs     GTest/Catch2    JSON/Text
```

---

## Core Components

### 1. StackTraceParser

**Purpose**: Convert raw crash dumps into structured data

**Input**: String containing stack trace (any format)
**Output**: `StackTrace` object with parsed frames

**Algorithm**:
```
1. Detect format (GDB/LLDB/MSVC/Generic)
2. Apply format-specific regex patterns
3. Extract per frame:
   - Function name (with C++ demangling)
   - File path and line number
   - Memory address
   - Function parameters (if available)
4. Extract error information (signal, exception type)
5. Return structured StackTrace object
```

**Performance**: O(n) where n = number of lines in trace (typical: <1ms)

**Supported Formats**:
- **GDB**: `#0 0x... in function() at file.cpp:42`
- **LLDB**: `frame #0: 0x... module\`function() at file.cpp:42`
- **MSVC**: `function() Line 42 C++`
- **Generic**: Best-effort pattern matching

**Key Features**:
- Auto-format detection
- C++ symbol demangling (platform-specific)
- Robust error handling for malformed traces
- Preserves original trace for reference

---

### 2. CallGraphAnalyzer

**Purpose**: Build function call relationships and detect patterns

**Input**: `StackTrace` object
**Output**: `CallGraph` with nodes, edges, and detected patterns

**Algorithm**:
```
1. Create nodes for each unique function
2. Build edges: caller → callee (bottom-up from stack)
3. Detect recursion:
   - Direct recursion: f → f
   - Indirect recursion: f → g → f
4. Identify critical path:
   - Functions on path from error to main()
5. Compute metrics:
   - Depth of call stack
   - Number of unique functions
   - Recursion count
```

**Performance**: O(n²) worst case, O(n) typical (n = stack depth)

**Output Patterns**:
- Recursive calls (potential stack overflow)
- Deep call chains (complexity indicator)
- Function call frequency
- Critical execution path

---

### 3. RootCausePredictor

**Purpose**: Identify bug category using pattern matching

**Input**: `StackTrace` and `CallGraph`
**Output**: `RootCause` with category, confidence, location

**Heuristic Engine** (see detailed section below):
```
For each bug category:
  1. Check function name patterns
  2. Check error signal/exception
  3. Check call graph patterns
  4. Check stack frame context
  5. Compute confidence score (0-100)

Return highest confidence match
```

**Supported Bug Categories** (14 types):
1. **Memory Errors**:
   - Null Pointer Dereference
   - Buffer Overflow
   - Use After Free
   - Double Free
   - Memory Leak

2. **Concurrency Errors**:
   - Race Condition
   - Deadlock

3. **Logic Errors**:
   - Logic Error
   - Assertion Failure

4. **Type & Arithmetic**:
   - Type Error
   - Arithmetic Error (div by zero, overflow)

5. **Resource Errors**:
   - Resource Exhaustion
   - Unknown (fallback)

**Confidence Scoring**:
- **High (85-100)**: Multiple strong indicators, well-known pattern
- **Medium (70-84)**: Some indicators, likely correct
- **Low (<70)**: Weak indicators, manual review recommended

---

### 4. ExplanationGenerator

**Purpose**: Create human-readable explanations

**Input**: `RootCause`, `StackTrace`
**Output**: Multi-format explanation (Markdown/HTML/Plain)

**Generated Sections**:
1. **Bug Summary**: Category, location, confidence
2. **Technical Explanation**: What went wrong (detailed)
3. **Simplified Explanation**: For non-experts
4. **Call Flow**: How execution reached the error
5. **Prevention Strategy**: How to avoid in future
6. **Related Patterns**: Similar bugs to watch for

**Output Formats**:
- **Markdown**: Rich formatting for documentation
- **HTML**: Web display with syntax highlighting
- **Plain Text**: Terminal/log file output

**Example Output**:
```markdown
## Root Cause Analysis

**Bug Type**: Null Pointer Dereference
**Confidence**: 92%
**Location**: `processData()` at main.cpp:42

### What Happened
The program attempted to dereference a null pointer, causing a segmentation fault.

### Why It Happened
The pointer `data` was not validated before use...

### Prevention
1. Always check pointers before dereferencing
2. Use smart pointers (std::unique_ptr, std::shared_ptr)
3. Enable compiler warnings (-Wall -Wextra)
```

---

### 5. FixSuggester

**Purpose**: Generate code fixes for identified bugs

**Input**: `RootCause`, `StackTrace`, source files
**Output**: `CodeFix` objects with original/fixed code

**Fix Types** (8 categories):
1. **Null Check**: Add `if (ptr != nullptr)`
2. **Bounds Check**: Add array bounds validation
3. **Initialization**: Initialize variables
4. **Memory Cleanup**: Add `delete`/`free()` calls
5. **Synchronization**: Add mutex locks
6. **Refactoring**: Structural improvements
7. **Assertions**: Add runtime checks
8. **Error Handling**: Add try-catch, return code checks

**Fix Generation Algorithm**:
```
1. Load source file at error location
2. Identify fix insertion point (line number)
3. Apply transformation based on bug type:
   - Null pointer → Insert null check
   - Buffer overflow → Insert bounds check
   - Use-after-free → Remove delete or clone object
4. Generate before/after code snippets
5. Compute confidence score
6. Return CodeFix object
```

**Safety Features**:
- Dry-run mode (preview only)
- Automatic file backup before applying
- Confidence threshold enforcement
- Rollback support

**Example Fix**:
```cpp
// Original (line 42)
int value = *ptr;

// Fixed
if (ptr != nullptr) {
    int value = *ptr;
} else {
    // Handle null pointer
    std::cerr << "Error: null pointer detected\n";
    return -1;
}
```

---

### 6. TestGenerator

**Purpose**: Create regression tests to prevent future bugs

**Input**: `RootCause`, source files
**Output**: `TestCase` objects with test code

**Supported Frameworks**:
1. **Google Test** (GTest)
2. **Catch2**
3. **Boost.Test**
4. **doctest**

**Test Generation Strategy**:
```
1. Identify test scenario from bug type
2. Generate setup code (create test fixture)
3. Generate test cases:
   - Positive test: Normal operation
   - Negative test: Trigger the bug condition
   - Edge cases: Boundary conditions
4. Generate teardown code (cleanup)
5. Format according to selected framework
```

**Example Generated Test** (GTest):
```cpp
TEST(ProcessDataTest, HandlesNullPointer) {
    // Setup
    Data* nullData = nullptr;

    // Test: Should not crash on null pointer
    EXPECT_NO_THROW({
        int result = processData(nullData);
        EXPECT_EQ(result, -1); // Error code
    });

    // Teardown
    // (none needed)
}
```

**Test Types Generated**:
- Crash reproduction test
- Null/invalid input tests
- Boundary value tests
- Resource exhaustion tests
- Concurrency stress tests (for race conditions)

---

## Heuristic Engine

The heart of the Root Cause Predictor is a multi-stage pattern matching engine.

### Stage 1: Signal/Exception Analysis
```
SIGSEGV/SIGABRT/Access Violation → Memory error (high confidence)
SIGFPE → Arithmetic error (high confidence)
std::bad_alloc → Memory exhaustion (high confidence)
pthread_mutex_lock → Potential deadlock (medium confidence)
```

### Stage 2: Function Name Pattern Matching
```
Pattern                           → Bug Category
────────────────────────────────────────────────────────
free/delete/~                     → Use-after-free or Double-free
malloc/new/alloc                  → Memory leak or allocation failure
strcpy/sprintf/strcat             → Buffer overflow
pthread_/mutex_/lock_             → Race condition or deadlock
[]/std::vector::operator[]        → Buffer overflow
assert/ASSERT                     → Assertion failure
```

### Stage 3: Call Graph Pattern Matching
```
Deep recursion (>100 frames)      → Stack overflow
Duplicate function in stack       → Infinite recursion
Lock operations without unlock    → Deadlock
Malloc without corresponding free → Memory leak
```

### Stage 4: Context Analysis
```
Check nearby function calls:
  - Memory allocation before error → Likely memory issue
  - Lock acquisition before error → Likely deadlock
  - Array access before error → Likely bounds issue

Check function parameters:
  - Null pointers passed → Likely null deref
  - Negative indices → Likely bounds issue
```

### Confidence Scoring Formula
```
confidence = base_score
           + signal_match * 30
           + function_pattern * 25
           + call_graph_pattern * 25
           + context_match * 20

Capped at 100
```

### Adding Custom Heuristics
```cpp
// Extensibility point: Custom pattern matcher
class CustomHeuristicPlugin : public HeuristicPlugin {
public:
    int matchPattern(const StackTrace& trace) override {
        // Return confidence score 0-100
        if (detectMyPattern(trace)) {
            return 85; // High confidence
        }
        return 0;
    }
};

// Register custom heuristic
predictor.registerHeuristic(new CustomHeuristicPlugin());
```

---

## Extensibility Points

### 1. Custom Bug Categories
Add new bug types by extending `BugCategory` enum:
```cpp
// In RootCausePredictor.h
enum class BugCategory {
    // ... existing categories
    CUSTOM_BUG_TYPE  // Add your custom type
};
```

### 2. Custom Heuristics
Implement `HeuristicPlugin` interface for custom pattern matching.

### 3. Custom Output Formats
Extend `ExplanationGenerator` to support new formats:
```cpp
std::string generateCustomFormat(const RootCause& cause);
```

### 4. Custom Fix Strategies
Add new fix types in `FixSuggester`:
```cpp
CodeFix generateCustomFix(const RootCause& cause);
```

### 5. Custom Test Frameworks
Support additional test frameworks in `TestGenerator`:
```cpp
TestCase generateCustomFrameworkTest(const RootCause& cause);
```

---

## Performance Characteristics

### Time Complexity
| Component | Typical Case | Worst Case | Notes |
|-----------|--------------|------------|-------|
| StackTraceParser | O(n) | O(n) | n = lines in trace |
| CallGraphAnalyzer | O(n) | O(n²) | n = stack depth |
| RootCausePredictor | O(m×n) | O(m×n) | m = categories, n = frames |
| ExplanationGenerator | O(1) | O(n) | Depends on trace size |
| FixSuggester | O(k) | O(k×n) | k = file size, n = fixes |
| TestGenerator | O(1) | O(1) | Template-based |

### Space Complexity
| Component | Space Usage |
|-----------|-------------|
| StackTrace | O(n) - proportional to stack depth |
| CallGraph | O(n²) - worst case for edges |
| RootCause | O(1) - fixed size |
| Generated fixes | O(k) - proportional to source file size |

### Real-World Performance
```
Typical crash analysis (50-line trace): <10ms
Large crash analysis (500-line trace): <100ms
Batch analysis (100 crashes): <5 seconds

Memory usage: <50MB for typical workloads
```

### Optimization Strategies
1. **Lazy Loading**: Source files loaded only when needed
2. **Caching**: Parsed traces cached during session
3. **Early Termination**: Stop pattern matching once high confidence reached
4. **Parallel Processing**: Batch analysis uses thread pool

---

## Integration Patterns

### 1. CLI Integration (examples/cli_tool.cpp)
```cpp
AIDebugger debugger;
debugger.loadConfig(".aidebuggerrc");
debugger.analyzeFile("crash.log");
auto report = debugger.generateReport();
std::cout << report;
```

### 2. API Integration (Programmatic)
```cpp
#include "ai_debugger/AIDebugger.h"

AIDebugger debugger;
auto trace = debugger.parseTrace(crashDump);
auto cause = debugger.predictRootCause(trace);

if (cause.confidence > 85) {
    auto fixes = debugger.suggestFixes(cause);
    debugger.applyFix(fixes[0]);
}
```

### 3. CI/CD Integration
```bash
# In CI pipeline
./cli_tool --json crash.log > analysis.json
if [ $(jq '.confidence' analysis.json) -gt 85 ]; then
    echo "High-confidence bug detected!"
    exit 1
fi
```

### 4. IDE Plugin Integration
```cpp
// VSCode/IntelliJ plugin can call
std::string json = debugger.getReportJSON();
// Parse and display in IDE UI
```

---

## Error Handling Strategy

### Error Categories
1. **Input Errors**: Malformed traces, invalid files
2. **Parse Errors**: Unrecognized format, corrupt data
3. **Analysis Errors**: Insufficient data for prediction
4. **File Errors**: Can't read source files for fixes
5. **Config Errors**: Invalid configuration

### Error Reporting
All components return standardized error codes (see `ErrorCodes.h`):
```cpp
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_INPUT = 1,
    PARSE_ERROR = 2,
    FILE_NOT_FOUND = 3,
    // ... see ErrorCodes.h for full list
};
```

### Recovery Strategies
- **Graceful Degradation**: Partial results on parse errors
- **Fallback Modes**: Generic patterns when specific ones fail
- **User Feedback**: Clear error messages with actionable hints

---

## Security Considerations

### Input Validation
- All user input sanitized before processing
- File paths validated to prevent directory traversal
- Regex patterns bounded to prevent ReDoS attacks
- Memory limits enforced to prevent DoS

### Code Fix Safety
- Never auto-apply fixes without user confirmation
- Backup files before modification
- Validate fix syntax before applying
- Rollback capability for failed fixes

### Data Privacy
- No telemetry or data collection
- All processing local-only
- No external API calls
- User data never transmitted

---

## Testing Strategy

### Unit Tests (tests/)
Each component has dedicated test file:
- `test_stack_trace_parser.cpp`: 20+ test cases
- `test_call_graph_analyzer.cpp`: Graph construction tests
- `test_root_cause_predictor.cpp`: Pattern matching tests
- `test_explanation_generator.cpp`: Output format tests
- `test_fix_suggester.cpp`: Fix generation tests
- `test_test_generator.cpp`: Test code generation tests

### Integration Tests
- `test_integration.cpp`: End-to-end pipeline tests
- Real crash scenarios from `demo/` directory
- Multi-component interaction tests

### Test Coverage Goals
- Line coverage: >80%
- Branch coverage: >70%
- Critical path coverage: 100%

---

## Future Architecture Enhancements

### Planned Improvements
1. **Machine Learning Integration**
   - Train neural network on crash patterns
   - Improve confidence scoring accuracy
   - Learn from user feedback

2. **Knowledge Base System**
   - Store project-specific crash patterns
   - Team-shared bug knowledge
   - Version-controlled pattern database

3. **Real-Time Analysis**
   - Watch mode for continuous monitoring
   - Live crash analysis during debugging
   - Integration with debugger protocols (GDB/LLDB)

4. **Distributed Analysis**
   - Batch processing across multiple cores
   - Cloud-based analysis for large projects
   - Aggregate insights across teams

5. **Advanced Heuristics**
   - Data flow analysis for deeper insights
   - Control flow graph analysis
   - Taint analysis for security bugs

---

## Conclusion

The AI Debugger architecture prioritizes:
- **Modularity**: Easy to extend and maintain
- **Performance**: Sub-second analysis for most crashes
- **Accuracy**: High-confidence predictions with clear scoring
- **Usability**: Natural language explanations and actionable fixes
- **Safety**: Never auto-modify code without user approval

For specific implementation details, see:
- API Reference: `docs/API.md`
- Configuration: `CONFIG_GUIDE.md`
- Contributing: `CONTRIBUTING.md`
- Examples: `examples/` directory
