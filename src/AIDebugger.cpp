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
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_buf);
    session.timestamp = buffer;

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
    std::ifstream file(trace_file);
    if (!file.is_open()) return DebugSession();

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    return content.empty() ? DebugSession() : analyzeStackTrace(content);
}

std::string AIDebugger::getReport(const DebugSession& session) const {
    std::string report;
    report.reserve(512);
    report += "Session: " + session.session_id + "\n";
    report += "Frames: " + std::to_string(session.trace.frames.size()) + "\n";
    report += "Error: " + session.trace.error_message + "\n";

    if (!session.root_causes.empty()) {
        report += "\nRoot Causes:\n";
        for (const auto& cause : session.root_causes) {
            report += "- " + cause.description + "\n";
        }
    }

    if (!session.suggested_fixes.empty()) {
        report += "\nFixes:\n";
        for (const auto& fix : session.suggested_fixes) {
            report += "- " + fix.description + "\n";
        }
    }
    return report;
}

std::string AIDebugger::getReportJSON(const DebugSession& session) const {
    return "{\"session_id\":\"" + session.session_id + "\"," +
           "\"frames\":" + std::to_string(session.trace.frames.size()) + "," +
           "\"error\":\"" + session.trace.error_message + "\"," +
           "\"causes\":" + std::to_string(session.root_causes.size()) + "," +
           "\"fixes\":" + std::to_string(session.suggested_fixes.size()) + "}";
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
