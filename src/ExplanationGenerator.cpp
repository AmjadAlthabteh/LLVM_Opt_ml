#include "ai_debugger/ExplanationGenerator.h"
#include <sstream>
#include <algorithm>

namespace ai_debugger {

std::string Explanation::toMarkdown() const {
    std::ostringstream oss;

    oss << "# Bug Analysis Report\n\n";
    oss << "## Summary\n" << summary << "\n\n";
    oss << "## Detailed Analysis\n" << detailed_analysis << "\n\n";

    if (!step_by_step.empty()) {
        oss << "## Step-by-Step Breakdown\n";
        for (size_t i = 0; i < step_by_step.size(); ++i) {
            oss << (i + 1) << ". " << step_by_step[i] << "\n";
        }
        oss << "\n";
    }

    if (!technical_details.empty()) {
        oss << "## Technical Details\n" << technical_details << "\n\n";
    }

    if (!simplified_explanation.empty()) {
        oss << "## Simplified Explanation\n" << simplified_explanation << "\n\n";
    }

    if (!relevant_code_snippets.empty()) {
        oss << "## Relevant Code\n";
        for (const auto& snippet : relevant_code_snippets) {
            oss << "```cpp\n" << snippet << "\n```\n\n";
        }
    }

    return oss.str();
}

std::string Explanation::toPlainText() const {
    std::ostringstream oss;
    oss << "BUG ANALYSIS REPORT\n";
    oss << "==================\n\n";
    oss << "SUMMARY:\n" << summary << "\n\n";
    oss << "DETAILED ANALYSIS:\n" << detailed_analysis << "\n\n";

    if (!step_by_step.empty()) {
        oss << "STEP-BY-STEP:\n";
        for (size_t i = 0; i < step_by_step.size(); ++i) {
            oss << "  " << (i + 1) << ". " << step_by_step[i] << "\n";
        }
        oss << "\n";
    }

    return oss.str();
}

std::string Explanation::toHTML() const {
    std::ostringstream oss;
    oss << "<html><body>\n";
    oss << "<h1>Bug Analysis Report</h1>\n";
    oss << "<h2>Summary</h2>\n<p>" << summary << "</p>\n";
    oss << "<h2>Detailed Analysis</h2>\n<p>" << detailed_analysis << "</p>\n";

    if (!step_by_step.empty()) {
        oss << "<h2>Step-by-Step</h2>\n<ol>\n";
        for (const auto& step : step_by_step) {
            oss << "<li>" << step << "</li>\n";
        }
        oss << "</ol>\n";
    }

    oss << "</body></html>\n";
    return oss.str();
}

struct ExplanationGenerator::Impl {
    int detail_level = 2;
    bool include_code_snippets = true;
};

ExplanationGenerator::ExplanationGenerator() : impl_(std::make_unique<Impl>()) {}

ExplanationGenerator::~ExplanationGenerator() = default;

void ExplanationGenerator::setDetailLevel(int level) {
    impl_->detail_level = level;
}

void ExplanationGenerator::setIncludeCodeSnippets(bool include) {
    impl_->include_code_snippets = include;
}

Explanation ExplanationGenerator::generate(
    const StackTrace& trace,
    const RootCause& root_cause,
    const CallGraphAnalyzer& graph
) {
    Explanation explanation;

    explanation.summary = generateSummary(root_cause);
    explanation.detailed_analysis = generateDetailedAnalysis(trace, root_cause);
    explanation.step_by_step = generateStepByStep(trace, root_cause);
    explanation.simplified_explanation = generateSimplified(root_cause);

    if (impl_->include_code_snippets) {
        explanation.relevant_code_snippets = extractRelevantCode(trace.frames);
    }

    explanation.technical_details = "Error occurred at: " +
        root_cause.location.file + ":" +
        std::to_string(root_cause.location.line);

    return explanation;
}

std::string ExplanationGenerator::explainBugCategory(BugCategory category) {
    switch (category) {
        case BugCategory::NULL_POINTER:
            return "A null pointer dereference occurs when code attempts to access memory "
                   "through a pointer that is NULL (pointing to address 0x0). This causes "
                   "a segmentation fault because the operating system protects the zero page.";

        case BugCategory::BUFFER_OVERFLOW:
            return "A buffer overflow happens when data is written beyond the allocated "
                   "boundaries of a buffer, corrupting adjacent memory. This can overwrite "
                   "important data structures and lead to crashes or security vulnerabilities.";

        case BugCategory::USE_AFTER_FREE:
            return "Use-after-free occurs when code accesses memory after it has been freed. "
                   "The freed memory may be reallocated for other purposes, leading to "
                   "unpredictable behavior, data corruption, or crashes.";

        case BugCategory::DOUBLE_FREE:
            return "Double free happens when the same memory is freed twice. This corrupts "
                   "the memory allocator's internal data structures and typically causes "
                   "immediate crashes.";

        case BugCategory::MEMORY_LEAK:
            return "Memory leaks occur when allocated memory is never freed, causing gradual "
                   "memory exhaustion. While not immediately fatal, leaks can eventually "
                   "cause the program to run out of memory.";

        case BugCategory::RACE_CONDITION:
            return "Race conditions occur in multithreaded programs when the outcome depends "
                   "on the timing or interleaving of operations. This leads to non-deterministic "
                   "bugs that are difficult to reproduce.";

        case BugCategory::DEADLOCK:
            return "Deadlock happens when two or more threads wait for each other to release "
                   "resources, causing all of them to block indefinitely.";

        default:
            return "Unknown bug category.";
    }
}

std::string ExplanationGenerator::explainCallFlow(const std::vector<StackFrame>& frames) {
    std::ostringstream oss;
    oss << "Call flow analysis:\n";

    for (size_t i = frames.size(); i > 0; --i) {
        const auto& frame = frames[i - 1];
        std::string indent(frames.size() - i, ' ');
        oss << indent << "→ " << frame.function_name;

        if (!frame.location.file.empty()) {
            oss << " (" << frame.location.file << ":" << frame.location.line << ")";
        }
        oss << "\n";
    }

    return oss.str();
}

std::string ExplanationGenerator::suggestPreventionStrategies(const RootCause& cause) {
    std::ostringstream oss;
    oss << "Prevention strategies:\n";

    switch (cause.category) {
        case BugCategory::NULL_POINTER:
            oss << "- Always check pointers for NULL before dereferencing\n";
            oss << "- Use smart pointers (std::unique_ptr, std::shared_ptr)\n";
            oss << "- Initialize pointers to nullptr\n";
            oss << "- Consider using optional types or references when possible\n";
            break;

        case BugCategory::BUFFER_OVERFLOW:
            oss << "- Use bounds-checking array access (at() instead of [])\n";
            oss << "- Prefer std::vector and std::string over raw arrays\n";
            oss << "- Enable compiler warnings and address sanitizer\n";
            oss << "- Validate array indices before access\n";
            break;

        case BugCategory::USE_AFTER_FREE:
            oss << "- Use RAII (Resource Acquisition Is Initialization)\n";
            oss << "- Prefer stack allocation over heap when possible\n";
            oss << "- Use smart pointers to manage lifetime automatically\n";
            oss << "- Set pointers to nullptr after freeing\n";
            break;

        case BugCategory::RACE_CONDITION:
            oss << "- Protect shared data with mutexes or locks\n";
            oss << "- Use atomic operations for simple shared variables\n";
            oss << "- Minimize shared state between threads\n";
            oss << "- Consider lock-free data structures\n";
            break;

        default:
            oss << "- Enable all compiler warnings\n";
            oss << "- Use static analysis tools\n";
            oss << "- Write comprehensive unit tests\n";
            oss << "- Perform code reviews\n";
            break;
    }

    return oss.str();
}

std::string ExplanationGenerator::generateSummary(const RootCause& cause) {
    std::ostringstream oss;
    oss << "Bug Type: " << bugCategoryToString(cause.category) << "\n";
    oss << "Confidence: " << (cause.confidence * 100) << "%\n";
    oss << "Description: " << cause.description;
    return oss.str();
}

std::string ExplanationGenerator::generateDetailedAnalysis(
    const StackTrace& trace,
    const RootCause& cause
) {
    std::ostringstream oss;

    oss << "The program encountered a " << bugCategoryToString(cause.category);
    oss << " (" << cause.description << "). ";

    if (cause.category == BugCategory::NULL_POINTER) {
        oss << describeMemoryError(cause);
    } else if (cause.category == BugCategory::RACE_CONDITION || cause.category == BugCategory::DEADLOCK) {
        oss << describeConcurrencyError(cause);
    } else {
        oss << describeLogicError(cause);
    }

    oss << "\n\n";
    oss << "Stack trace analysis shows " << trace.frames.size() << " function calls leading to the error. ";

    if (!cause.relevant_frames.empty()) {
        oss << "The most relevant function is '" << cause.relevant_frames[0].function_name << "'.";
    }

    return oss.str();
}

std::vector<std::string> ExplanationGenerator::generateStepByStep(
    const StackTrace& trace,
    const RootCause& cause
) {
    std::vector<std::string> steps;

    steps.push_back("Program execution begins normally");

    if (trace.frames.size() > 2) {
        steps.push_back("Function '" + trace.frames.back().function_name + "' is called");
    }

    if (trace.frames.size() > 1) {
        steps.push_back("Execution proceeds through " + std::to_string(trace.frames.size()) + " function calls");
    }

    steps.push_back("Error condition triggers in '" + trace.frames[0].function_name + "'");
    steps.push_back(bugCategoryToString(cause.category) + " occurs");
    steps.push_back("Operating system terminates program with signal " + std::to_string(trace.signal_number));

    return steps;
}

std::string ExplanationGenerator::generateSimplified(const RootCause& cause) {
    std::ostringstream oss;

    oss << "In simple terms: ";

    switch (cause.category) {
        case BugCategory::NULL_POINTER:
            oss << "Your program tried to use a pointer that wasn't pointing to valid memory. "
                << "It's like trying to read a book when you don't have one in your hands.";
            break;

        case BugCategory::BUFFER_OVERFLOW:
            oss << "Your program wrote data beyond the end of an array or buffer. "
                << "It's like writing outside the margins of a page.";
            break;

        case BugCategory::USE_AFTER_FREE:
            oss << "Your program tried to use memory that was already given back to the system. "
                << "It's like trying to use a library book after you've returned it.";
            break;

        default:
            oss << "Your program encountered an error condition it couldn't handle properly.";
            break;
    }

    return oss.str();
}

std::string ExplanationGenerator::describeMemoryError(const RootCause& cause) {
    return "This is a memory access violation. The program attempted to read or write to "
           "an invalid memory address, which the operating system prevented, resulting in a crash.";
}

std::string ExplanationGenerator::describeConcurrencyError(const RootCause& cause) {
    return "This is a concurrency issue related to multithreading. Multiple threads "
           "are accessing shared resources without proper synchronization.";
}

std::string ExplanationGenerator::describeLogicError(const RootCause& cause) {
    return "This appears to be a logic error where the program's state or data "
           "doesn't match expected conditions.";
}

std::vector<std::string> ExplanationGenerator::extractRelevantCode(
    const std::vector<StackFrame>& frames
) {
    std::vector<std::string> snippets;

    for (const auto& frame : frames) {
        if (!frame.location.file.empty() && frame.location.line > 0) {
            std::ostringstream oss;
            oss << "// " << frame.location.file << ":" << frame.location.line << "\n";
            oss << "// Function: " << frame.function_name << "\n";
            oss << "// (Source code would be loaded here)";
            snippets.push_back(oss.str());

            if (snippets.size() >= 3) break;
        }
    }

    return snippets;
}

} // namespace ai_debugger
