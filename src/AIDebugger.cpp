#include "ai_debugger/AIDebugger.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

namespace ai_debugger {

struct AIDebugger::Impl {
    StackTraceParser parser;
    CallGraphAnalyzer graph_analyzer;
    RootCausePredictor predictor;
    ExplanationGenerator explanation_gen;
    FixSuggester fix_suggester;
    TestGenerator test_gen;

    Config config;
    std::vector<DebugSession> sessions;
};

AIDebugger::AIDebugger() : impl_(std::make_unique<Impl>()) {}

AIDebugger::~AIDebugger() = default;

void AIDebugger::setSourceDirectory(const std::string& src_dir) {
    impl_->config.source_directory = src_dir;
    impl_->fix_suggester.setSourceRoot(src_dir);
}

void AIDebugger::setTestFramework(TestFramework framework) {
    impl_->config.test_framework = framework;
    impl_->test_gen.setFramework(framework);
}

void AIDebugger::setTestOutputDirectory(const std::string& dir) {
    impl_->config.test_output_directory = dir;
    impl_->test_gen.setOutputDirectory(dir);
}

void AIDebugger::setVerbose(bool verbose) {
    impl_->config.verbose = verbose;
    impl_->parser.setVerbose(verbose);
}

void AIDebugger::enableAutoFix(bool enable) {
    impl_->config.auto_fix = enable;
}

void AIDebugger::enableTestGeneration(bool enable) {
    impl_->config.auto_test = enable;
}

DebugSession AIDebugger::analyzeStackTrace(const std::string& trace_text) {
    DebugSession session;
    session.session_id = generateSessionId();

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    {
#if defined(_WIN32)
        std::tm tm_buf{};
        localtime_s(&tm_buf, &time);
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
#else
        std::tm tm_buf{};
        localtime_r(&time, &tm_buf);
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
#endif
    }
    session.timestamp = oss.str();

    auto trace_opt = impl_->parser.parse(trace_text);
    if (!trace_opt) {
        return session;
    }

    session.trace = *trace_opt;

    impl_->graph_analyzer.buildFromStackTrace(session.trace);

    auto root_causes = impl_->predictor.predict(session.trace, impl_->graph_analyzer);
    session.root_causes = root_causes;

    if (!root_causes.empty()) {
        session.explanation = impl_->explanation_gen.generate(
            session.trace,
            root_causes[0],
            impl_->graph_analyzer
        );

        session.suggested_fixes = impl_->fix_suggester.suggestFixes(
            root_causes[0],
            session.trace
        );

        if (impl_->config.auto_test && !session.suggested_fixes.empty()) {
            session.regression_tests = impl_->test_gen.generateRegressionTests(
                root_causes[0],
                session.suggested_fixes[0],
                session.trace
            );
        }
    }

    saveSession(session);

    return session;
}

DebugSession AIDebugger::analyzeFromFile(const std::string& trace_file) {
    if (trace_file.empty()) {
        return DebugSession();
    }

    std::ifstream file(trace_file);
    if (!file.is_open()) {
        return DebugSession();
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();

    if (content.empty()) {
        return DebugSession();
    }

    return analyzeStackTrace(content);
}

std::string AIDebugger::getReport(const DebugSession& session) const {
    std::ostringstream oss;

    oss << "=================================================\n";
    oss << "AI DEBUGGER ANALYSIS REPORT\n";
    oss << "=================================================\n\n";

    oss << "Session ID: " << session.session_id << "\n";
    oss << "Timestamp: " << session.timestamp << "\n\n";

    oss << "STACK TRACE SUMMARY\n";
    oss << "-------------------\n";
    oss << "Frames: " << session.trace.frames.size() << "\n";
    oss << "Error Message: " << session.trace.error_message << "\n\n";

    if (!session.root_causes.empty()) {
        oss << "ROOT CAUSE ANALYSIS\n";
        oss << "-------------------\n";
        for (size_t i = 0; i < session.root_causes.size(); ++i) {
            const auto& cause = session.root_causes[i];
            oss << (i + 1) << ". " << bugCategoryToString(cause.category)
                << " (Confidence: " << (cause.confidence * 100) << "%)\n";
            oss << "   " << cause.description << "\n";
            if (!cause.location.file.empty()) {
                oss << "   Location: " << cause.location.file << ":"
                    << cause.location.line << "\n";
            }
            oss << "\n";
        }
    }

    oss << "\n" << session.explanation.toPlainText() << "\n";

    if (!session.suggested_fixes.empty()) {
        oss << "\nSUGGESTED FIXES\n";
        oss << "---------------\n";
        for (size_t i = 0; i < session.suggested_fixes.size(); ++i) {
            const auto& fix = session.suggested_fixes[i];
            oss << (i + 1) << ". " << fixTypeToString(fix.type)
                << " (Confidence: " << (fix.confidence * 100) << "%)\n";
            oss << "   " << fix.description << "\n\n";
            oss << "   Original:\n   " << fix.original_code << "\n\n";
            oss << "   Fixed:\n   " << fix.fixed_code << "\n\n";
        }
    }

    oss << "=================================================\n";

    return oss.str();
}

std::string AIDebugger::getReportJSON(const DebugSession& session) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"session_id\": \"" << session.session_id << "\",\n";
    oss << "  \"timestamp\": \"" << session.timestamp << "\",\n";
    oss << "  \"stack\": {\n";
    oss << "    \"frames\": " << session.trace.frames.size() << ",\n";
    oss << "    \"error_message\": \"" << session.trace.error_message << "\"\n";
    oss << "  },\n";
    oss << "  \"root_causes\": [\n";
    for (size_t i = 0; i < session.root_causes.size(); ++i) {
        const auto& cause = session.root_causes[i];
        oss << "    {\n";
        oss << "      \"category\": \"" << bugCategoryToString(cause.category) << "\",\n";
        oss << "      \"confidence\": " << cause.confidence << ",\n";
        oss << "      \"description\": \"" << cause.description << "\",\n";
        oss << "      \"location\": { \"file\": \"" << cause.location.file << "\", \"line\": " << cause.location.line << " }\n";
        oss << "    }" << (i + 1 < session.root_causes.size() ? "," : "") << "\n";
    }
    oss << "  ],\n";
    oss << "  \"explanation\": {\n";
    oss << "    \"text\": \"" << session.explanation.toPlainText() << "\"\n";
    oss << "  },\n";
    oss << "  \"suggested_fixes\": [\n";
    for (size_t i = 0; i < session.suggested_fixes.size(); ++i) {
        const auto& fix = session.suggested_fixes[i];
        oss << "    {\n";
        oss << "      \"type\": \"" << fixTypeToString(fix.type) << "\",\n";
        oss << "      \"confidence\": " << fix.confidence << ",\n";
        oss << "      \"description\": \"" << fix.description << "\"\n";
        oss << "    }" << (i + 1 < session.suggested_fixes.size() ? "," : "") << "\n";
    }
    oss << "  ]\n";
    oss << "}\n";
    return oss.str();
}

bool AIDebugger::saveReport(const DebugSession& session, const std::string& output_path) const {
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }

    file << getReport(session);
    return true;
}

FixApplication AIDebugger::applyBestFix(const DebugSession& session) {
    if (session.suggested_fixes.empty()) {
        FixApplication app;
        app.success = false;
        app.message = "No fixes available";
        return app;
    }

    return impl_->fix_suggester.applyFix(session.suggested_fixes[0], true);
}

std::vector<FixApplication> AIDebugger::applyAllFixes(const DebugSession& session) {
    return impl_->fix_suggester.applyAllFixes(session.suggested_fixes, true);
}

bool AIDebugger::generateTests(const DebugSession& session) {
    if (!session.regression_tests.test_cases.empty()) {
        return impl_->test_gen.writeTestFile(session.regression_tests);
    }

    if (!session.root_causes.empty() && !session.suggested_fixes.empty()) {
        auto test_suite = impl_->test_gen.generateRegressionTests(
            session.root_causes[0],
            session.suggested_fixes[0],
            session.trace
        );
        return impl_->test_gen.writeTestFile(test_suite);
    }

    return false;
}

std::vector<std::string> AIDebugger::listSessions() const {
    std::vector<std::string> session_ids;
    for (const auto& session : impl_->sessions) {
        session_ids.push_back(session.session_id);
    }
    return session_ids;
}

DebugSession AIDebugger::loadSession(const std::string& session_id) const {
    for (const auto& session : impl_->sessions) {
        if (session.session_id == session_id) {
            return session;
        }
    }
    return DebugSession();
}

std::string AIDebugger::generateSessionId() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return "session_" + std::to_string(millis);
}

void AIDebugger::saveSession(const DebugSession& session) {
    impl_->sessions.push_back(session);

    std::filesystem::create_directories(".ai_debugger/sessions");
    std::string path = ".ai_debugger/sessions/" + session.session_id + ".txt";
    saveReport(session, path);
}

// Config methods are implemented in src/Config.cpp

} // namespace ai_debugger
