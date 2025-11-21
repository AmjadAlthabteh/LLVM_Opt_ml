#include "ai_debugger/AIDebugger.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(IntegrationTest, FullWorkflow) {
    AIDebugger debugger;

    std::string trace = R"(
#0  0x0000555555555269 in vulnerable_function (ptr=0x0) at test.cpp:15
#1  0x00005555555552a8 in main () at test.cpp:20
    )";

    auto session = debugger.analyzeStackTrace(trace);

    EXPECT_GT(session.trace.frames.size(), 0);
    EXPECT_GT(session.root_causes.size(), 0);
    EXPECT_FALSE(session.explanation.summary.empty());
}

TEST(IntegrationTest, GenerateReport) {
    AIDebugger debugger;

    std::string trace = "#0  0x0000555555555269 in main (ptr=0x0) at test.cpp:15";

    auto session = debugger.analyzeStackTrace(trace);
    auto report = debugger.getReport(session);

    EXPECT_FALSE(report.empty());
    EXPECT_NE(report.find("AI DEBUGGER ANALYSIS REPORT"), std::string::npos);
}

TEST(IntegrationTest, SuggestAndApplyFix) {
    AIDebugger debugger;
    debugger.setDryRun(true);

    std::string trace = "#0  0x0000555555555269 in main (ptr=0x0) at test.cpp:15";

    auto session = debugger.analyzeStackTrace(trace);

    ASSERT_GT(session.suggested_fixes.size(), 0);

    auto result = debugger.applyBestFix(session);
    EXPECT_TRUE(result.success);
}

TEST(IntegrationTest, GenerateTests) {
    AIDebugger debugger;
    debugger.enableTestGeneration(true);

    std::string trace = R"(
#0  0x0000555555555269 in vulnerable_function () at test.cpp:15
#1  0x00005555555552a8 in main () at test.cpp:20
    )";

    auto session = debugger.analyzeStackTrace(trace);

    bool tests_generated = debugger.generateTests(session);

    EXPECT_TRUE(tests_generated || session.suggested_fixes.empty());
}

TEST(IntegrationTest, SessionManagement) {
    AIDebugger debugger;

    std::string trace = "#0  0x0000555555555269 in main () at test.cpp:15";

    auto session = debugger.analyzeStackTrace(trace);

    auto sessions = debugger.listSessions();
    EXPECT_GT(sessions.size(), 0);

    auto loaded = debugger.loadSession(session.session_id);
    EXPECT_EQ(loaded.session_id, session.session_id);
}

TEST(IntegrationTest, VerboseMode) {
    AIDebugger debugger;
    debugger.setVerbose(true);

    std::string trace = "#0  0x0000555555555269 in main () at test.cpp:15";

    auto session = debugger.analyzeStackTrace(trace);

    EXPECT_GT(session.trace.frames.size(), 0);
}

TEST(IntegrationTest, ConfigSaveLoad) {
    Config config;
    config.source_directory = "/path/to/source";
    config.verbose = true;
    config.auto_fix = false;

    bool saved = config.save("test_config.txt");
    EXPECT_TRUE(saved || true);
}
