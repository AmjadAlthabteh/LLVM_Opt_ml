#ifndef AI_DEBUGGER_AI_DEBUGGER_H
#define AI_DEBUGGER_AI_DEBUGGER_H

#include "StackTraceParser.h"
#include "CallGraphAnalyzer.h"
#include "RootCausePredictor.h"
#include "ExplanationGenerator.h"
#include "FixSuggester.h"
#include "TestGenerator.h"
#include "Config.h"

#include <string>
#include <memory>
#include <vector>

namespace ai_debugger {

struct DebugSession {
    StackTrace trace;
    std::vector<RootCause> root_causes;
    Explanation explanation;
    std::vector<CodeFix> suggested_fixes;
    TestSuite regression_tests;

    std::string session_id;
    std::string timestamp;
};

class AIDebugger {
public:
    AIDebugger();
    ~AIDebugger();

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

    std::vector<std::string> listSessions() const;
    DebugSession loadSession(const std::string& session_id) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    std::string generateSessionId() const;
    void saveSession(const DebugSession& session);
};

// Config is defined in Config.h

} // namespace ai_debugger

#endif // AI_DEBUGGER_AI_DEBUGGER_H
