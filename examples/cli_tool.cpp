#include "ai_debugger/AIDebugger.h"
#include "ai_debugger/Config.h"
#include "ai_debugger/Version.h"
#include "ai_debugger/ErrorCodes.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void printUsage(const char* program_name) {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              AI Debugger - Crash Analysis Tool                ║\n";
    std::cout << "║         Automatically analyze crashes and suggest fixes       ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "Usage: " << program_name << " [options] <trace_file|->\n\n";

    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  --version               Show version information\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  -o, --output FILE       Save report to file\n";
    std::cout << "  -s, --source DIR        Set source directory\n";
    std::cout << "  --auto-fix              Automatically apply best fix (⚠ modifies code!)\n";
    std::cout << "  --apply-all             Apply all suggested fixes\n";
    std::cout << "  --generate-tests        Generate regression tests\n";
    std::cout << "  --framework FRAMEWORK   Test framework (gtest, catch2, boost, doctest)\n";
    std::cout << "  --format FORMAT         Output format (text, json)\n";
    std::cout << "  --config FILE           Load configuration file (.aidebuggerrc)\n";

    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "Quick Start Examples:\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    std::cout << "  📋 Basic Analysis:\n";
    std::cout << "     " << program_name << " crash.log\n\n";

    std::cout << "  📝 Detailed Analysis:\n";
    std::cout << "     " << program_name << " -v crash.log\n\n";

    std::cout << "  🔧 Auto-Fix (⚠ Be careful!):\n";
    std::cout << "     " << program_name << " --auto-fix crash.log\n\n";

    std::cout << "  🧪 Generate Tests:\n";
    std::cout << "     " << program_name << " --generate-tests --framework gtest crash.log\n\n";

    std::cout << "  💾 Save Report:\n";
    std::cout << "     " << program_name << " -o report.txt crash.log\n\n";

    std::cout << "  📊 JSON Output:\n";
    std::cout << "     " << program_name << " --format json crash.log\n\n";

    std::cout << "  🚀 Full Analysis with Tests:\n";
    std::cout << "     " << program_name << " -v --generate-tests -o report.txt crash.log\n\n";

    std::cout << "  📥 Read from stdin:\n";
    std::cout << "     cat crash.log | " << program_name << " -\n\n";

    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "New to AI Debugger?\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    std::cout << "  👉 Try sample crashes:    " << program_name << " demo/null_pointer_crash.txt\n";
    std::cout << "  📖 Read the guide:        GETTING_STARTED.md\n";
    std::cout << "  🎯 Quick start script:    quick_start.bat (Windows) or quick_start.sh (Linux/Mac)\n\n";

    std::cout << "For detailed documentation, see README.md or visit the docs/ folder.\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return static_cast<int>(ai_debugger::ExitCode::USAGE_ERROR);
    }

    std::string trace_file;
    std::string output_file;
    std::string source_dir;
    std::string framework = "gtest";
    bool verbose = false;
    bool auto_fix = false;
    bool generate_tests = false;
    bool apply_all = false;
    std::string format = "text";
    std::string config_file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return static_cast<int>(ai_debugger::ExitCode::SUCCESS);
        } else if (arg == "--version") {
            ai_debugger::Version::printFullInfo();
            return static_cast<int>(ai_debugger::ExitCode::SUCCESS);
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
        } else if (arg == "--apply-all") {
            apply_all = true;
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                config_file = argv[++i];
            }
        } else if (arg == "-") {
            trace_file = arg;
        } else if (arg[0] != '-') {
            trace_file = arg;
        }
    }

    if (trace_file.empty()) {
        std::cerr << "Error: No trace file specified\n";
        printUsage(argv[0]);
        return static_cast<int>(ai_debugger::ExitCode::USAGE_ERROR);
    }

    ai_debugger::AIDebugger debugger;

    if (!config_file.empty()) {
        auto cfg = ai_debugger::ConfigLoader::loadFromFileOrDefault(config_file);
        debugger.setVerbose(cfg.verbose);
        debugger.enableAutoFix(cfg.auto_fix);
        debugger.enableTestGeneration(cfg.auto_test);
        if (!cfg.source_directory.empty()) {
            debugger.setSourceDirectory(cfg.source_directory);
        }
        if (!cfg.test_output_directory.empty()) {
            debugger.setTestOutputDirectory(cfg.test_output_directory);
        }
        if (cfg.test_framework == ai_debugger::TestFramework::CATCH2) {
            debugger.setTestFramework(ai_debugger::TestFramework::CATCH2);
        } else if (cfg.test_framework == ai_debugger::TestFramework::BOOST_TEST) {
            debugger.setTestFramework(ai_debugger::TestFramework::BOOST_TEST);
        } else if (cfg.test_framework == ai_debugger::TestFramework::DOCTEST) {
            debugger.setTestFramework(ai_debugger::TestFramework::DOCTEST);
        } else {
            debugger.setTestFramework(ai_debugger::TestFramework::GTEST);
        }
    }

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

    std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   AI DEBUGGER ANALYSIS                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    std::cout << "📁 Input File: " << trace_file << "\n";
    if (verbose) {
        std::cout << "🔍 Mode: Verbose\n";
    }
    if (auto_fix) {
        std::cout << "⚠️  Auto-fix enabled (will modify source code)\n";
    }
    if (generate_tests) {
        std::cout << "🧪 Test generation enabled (" << framework << ")\n";
    }
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    ai_debugger::DebugSession session;
    if (trace_file == "-") {
        std::ostringstream buf;
        buf << std::cin.rdbuf();
        session = debugger.analyzeStackTrace(buf.str());
    } else {
        std::ifstream fcheck(trace_file);
        if (!fcheck.is_open()) {
            std::cerr << "Error: File not found: " << trace_file << "\n";
            return static_cast<int>(ai_debugger::ExitCode::FILE_NOT_FOUND);
        }
        session = debugger.analyzeFromFile(trace_file);
    }

    if (session.trace.frames.empty()) {
        std::cerr << "Error: Failed to parse stack trace\n";
        return static_cast<int>(ai_debugger::ExitCode::PARSE_ERROR);
    }

    if (format == "json") {
        std::string report_json = debugger.getReportJSON(session);
        if (output_file.empty()) {
            std::cout << report_json;
        } else {
            std::ofstream out(output_file);
            if (!out.is_open()) {
                std::cerr << "Error: Failed to save report\n";
                return 1;
            }
            out << report_json;
            std::cout << "Report saved to: " << output_file << "\n";
        }
    } else {
        std::string report = debugger.getReport(session);
        if (output_file.empty()) {
            std::cout << report;
        } else {
            if (debugger.saveReport(session, output_file)) {
                std::cout << "Report saved to: " << output_file << "\n";
            } else {
                std::cerr << "Error: Failed to save report\n";
            return static_cast<int>(ai_debugger::ExitCode::GENERAL_ERROR);
        }
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
            return static_cast<int>(ai_debugger::ExitCode::FIX_APPLICATION_FAILED);
        }
    }

    if (apply_all && !session.suggested_fixes.empty()) {
        std::cout << "\nApplying all suggested fixes...\n";
        auto results = debugger.applyAllFixes(session);
        bool any_success = false;
        for (const auto& r : results) {
            if (r.success) {
                any_success = true;
                for (const auto& f : r.modified_files) {
                    std::cout << "  - Modified: " << f << "\n";
                }
            }
        }
        if (!any_success) {
            std::cerr << "No fixes could be applied." << "\n";
        }
    }

    if (generate_tests) {
        std::cout << "\nGenerating regression tests...\n";
        if (debugger.generateTests(session)) {
            std::cout << "Tests generated successfully!\n";
        } else {
            std::cerr << "Failed to generate tests\n";
            return static_cast<int>(ai_debugger::ExitCode::TEST_GENERATION_FAILED);
        }
    }

    return static_cast<int>(ai_debugger::ExitCode::SUCCESS);
}
