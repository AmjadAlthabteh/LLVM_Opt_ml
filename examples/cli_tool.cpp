#include "ai_debugger/AIDebugger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void printUsage(const char* program_name) {
    std::cout << "AI Debugger CLI Tool\n";
    std::cout << "====================\n\n";
    std::cout << "Usage: " << program_name << " [options] <trace_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  -o, --output FILE       Save report to file\n";
    std::cout << "  -s, --source DIR        Set source directory\n";
    std::cout << "  --auto-fix              Automatically apply best fix\n";
    std::cout << "  --generate-tests        Generate regression tests\n";
    std::cout << "  --framework FRAMEWORK   Test framework (gtest, catch2, boost)\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " -v --generate-tests stacktrace.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string trace_file;
    std::string output_file;
    std::string source_dir;
    std::string framework = "gtest";
    bool verbose = false;
    bool auto_fix = false;
    bool generate_tests = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            }
        } else if (arg == "-s" || arg == "--source") {
            if (i + 1 < argc) {
                source_dir = argv[++i];
            }
        } else if (arg == "--auto-fix") {
            auto_fix = true;
        } else if (arg == "--generate-tests") {
            generate_tests = true;
        } else if (arg == "--framework") {
            if (i + 1 < argc) {
                framework = argv[++i];
            }
        } else if (arg[0] != '-') {
            trace_file = arg;
        }
    }

    if (trace_file.empty()) {
        std::cerr << "Error: No trace file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    ai_debugger::AIDebugger debugger;

    debugger.setVerbose(verbose);
    debugger.enableAutoFix(auto_fix);
    debugger.enableTestGeneration(generate_tests);

    if (!source_dir.empty()) {
        debugger.setSourceDirectory(source_dir);
    }

    if (framework == "catch2") {
        debugger.setTestFramework(ai_debugger::TestFramework::CATCH2);
    } else if (framework == "boost") {
        debugger.setTestFramework(ai_debugger::TestFramework::BOOST_TEST);
    } else {
        debugger.setTestFramework(ai_debugger::TestFramework::GTEST);
    }

    std::cout << "Analyzing stack trace from: " << trace_file << "\n";
    std::cout << "============================================\n\n";

    auto session = debugger.analyzeFromFile(trace_file);

    if (session.trace.frames.empty()) {
        std::cerr << "Error: Failed to parse stack trace\n";
        return 1;
    }

    std::string report = debugger.getReport(session);

    if (output_file.empty()) {
        std::cout << report;
    } else {
        if (debugger.saveReport(session, output_file)) {
            std::cout << "Report saved to: " << output_file << "\n";
        } else {
            std::cerr << "Error: Failed to save report\n";
            return 1;
        }
    }

    if (auto_fix && !session.suggested_fixes.empty()) {
        std::cout << "\nApplying best fix...\n";
        auto result = debugger.applyBestFix(session);
        if (result.success) {
            std::cout << "Fix applied successfully!\n";
            std::cout << "Modified files:\n";
            for (const auto& file : result.modified_files) {
                std::cout << "  - " << file << "\n";
            }
        } else {
            std::cerr << "Failed to apply fix: " << result.message << "\n";
        }
    }

    if (generate_tests) {
        std::cout << "\nGenerating regression tests...\n";
        if (debugger.generateTests(session)) {
            std::cout << "Tests generated successfully!\n";
        } else {
            std::cerr << "Failed to generate tests\n";
        }
    }

    return 0;
}
