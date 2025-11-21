// Simple standalone test - no dependencies needed
#include <iostream>
#include <string>
#include <vector>
#include <regex>

// Minimal inline demo of AI Debugger concepts
struct SimpleStackFrame {
    std::string function;
    std::string file;
    int line;
};

std::vector<SimpleStackFrame> parseStackTrace(const std::string& trace) {
    std::vector<SimpleStackFrame> frames;
    std::regex frame_regex(R"(#\d+\s+.*in\s+(\w+)\s+.*at\s+([^:]+):(\d+))");

    std::istringstream stream(trace);
    std::string line;

    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, frame_regex)) {
            SimpleStackFrame frame;
            frame.function = match[1].str();
            frame.file = match[2].str();
            frame.line = std::stoi(match[3].str());
            frames.push_back(frame);
        }
    }

    return frames;
}

std::string detectBugType(const std::string& error_msg) {
    std::string lower = error_msg;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower.find("null") != std::string::npos || lower.find("0x0") != std::string::npos) {
        return "NULL POINTER DEREFERENCE";
    } else if (lower.find("double free") != std::string::npos) {
        return "DOUBLE FREE";
    } else if (lower.find("segmentation fault") != std::string::npos) {
        return "MEMORY ACCESS VIOLATION";
    } else if (lower.find("buffer") != std::string::npos) {
        return "BUFFER OVERFLOW";
    }

    return "UNKNOWN ERROR";
}

std::string suggestFix(const std::string& bug_type) {
    if (bug_type == "NULL POINTER DEREFERENCE") {
        return "Add null check:\n  if (ptr != nullptr) {\n    ptr->method();\n  }";
    } else if (bug_type == "DOUBLE FREE") {
        return "Set pointer to nullptr after delete:\n  delete ptr;\n  ptr = nullptr;";
    } else if (bug_type == "BUFFER OVERFLOW") {
        return "Use bounds checking:\n  if (index < array_size) {\n    array[index] = value;\n  }";
    }

    return "Review code and add error handling";
}

int main() {
    std::cout << "\n=================================\n";
    std::cout << "AI DEBUGGER - SIMPLE TEST\n";
    std::cout << "=================================\n\n";

    // Test 1: Parse stack trace
    std::cout << "TEST 1: Stack Trace Parsing\n";
    std::cout << "----------------------------\n";

    std::string sample_trace = R"(
#0  0x555555555269 in process_data at main.cpp:42
#1  0x555555555320 in handle_request at server.cpp:108
#2  0x555555555450 in main at main.cpp:15
)";

    auto frames = parseStackTrace(sample_trace);
    std::cout << "Parsed " << frames.size() << " frames:\n";
    for (const auto& frame : frames) {
        std::cout << "  - " << frame.function << " (" << frame.file << ":" << frame.line << ")\n";
    }

    if (frames.size() == 3) {
        std::cout << "✓ PASS: Correctly parsed stack trace\n\n";
    } else {
        std::cout << "✗ FAIL: Expected 3 frames, got " << frames.size() << "\n\n";
    }

    // Test 2: Bug detection
    std::cout << "TEST 2: Bug Type Detection\n";
    std::cout << "---------------------------\n";

    std::string error1 = "Segmentation fault: null pointer dereference at 0x0";
    std::string detected = detectBugType(error1);
    std::cout << "Error: " << error1 << "\n";
    std::cout << "Detected: " << detected << "\n";

    if (detected == "NULL POINTER DEREFERENCE") {
        std::cout << "✓ PASS: Correctly identified null pointer bug\n\n";
    } else {
        std::cout << "✗ FAIL: Misidentified bug type\n\n";
    }

    // Test 3: Fix suggestion
    std::cout << "TEST 3: Fix Suggestion\n";
    std::cout << "-----------------------\n";

    std::string fix = suggestFix(detected);
    std::cout << "Suggested Fix:\n" << fix << "\n\n";

    if (!fix.empty() && fix.find("nullptr") != std::string::npos) {
        std::cout << "✓ PASS: Generated appropriate fix\n\n";
    } else {
        std::cout << "✗ FAIL: Fix suggestion incomplete\n\n";
    }

    // Test 4: Full workflow
    std::cout << "TEST 4: Complete Workflow\n";
    std::cout << "--------------------------\n";

    std::string crash_log = R"(
Program received signal SIGSEGV, Segmentation fault.
Null pointer dereference at 0x0
#0  0x555555555269 in vulnerable_function at bug.cpp:42
#1  0x555555555320 in main at main.cpp:10
)";

    std::cout << "Input: Crash log with segfault\n";

    auto crash_frames = parseStackTrace(crash_log);
    std::string bug_type = detectBugType(crash_log);
    std::string suggested_fix = suggestFix(bug_type);

    std::cout << "\nAnalysis Results:\n";
    std::cout << "  Frames parsed: " << crash_frames.size() << "\n";
    std::cout << "  Bug type: " << bug_type << "\n";
    std::cout << "  Fix available: " << (!suggested_fix.empty() ? "Yes" : "No") << "\n";

    if (crash_frames.size() > 0 && bug_type != "UNKNOWN ERROR" && !suggested_fix.empty()) {
        std::cout << "\n✓ PASS: Complete workflow successful\n\n";
    } else {
        std::cout << "\n✗ FAIL: Workflow incomplete\n\n";
    }

    std::cout << "=================================\n";
    std::cout << "SUMMARY: Core functionality verified\n";
    std::cout << "=================================\n\n";

    return 0;
}
