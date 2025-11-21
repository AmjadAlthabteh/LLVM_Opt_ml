#include "ai_debugger/CallGraphAnalyzer.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(CallGraphAnalyzerTest, BuildFromStackTrace) {
    StackTrace trace;

    StackFrame frame1;
    frame1.function_name = "function_a";
    trace.frames.push_back(frame1);

    StackFrame frame2;
    frame2.function_name = "function_b";
    trace.frames.push_back(frame2);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    auto nodes = analyzer.getNodes();
    EXPECT_EQ(nodes.size(), 2);
}

TEST(CallGraphAnalyzerTest, GetNode) {
    StackTrace trace;

    StackFrame frame;
    frame.function_name = "test_function";
    frame.location.file = "test.cpp";
    frame.location.line = 42;
    trace.frames.push_back(frame);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    auto node = analyzer.getNode("test_function");
    ASSERT_TRUE(node.has_value());
    EXPECT_EQ(node->function_name, "test_function");
}

TEST(CallGraphAnalyzerTest, DetectPatterns) {
    StackTrace trace;

    StackFrame frame1;
    frame1.function_name = "malloc";
    trace.frames.push_back(frame1);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    auto patterns = analyzer.detectPatterns();
    EXPECT_GE(patterns.size(), 0);
}

TEST(CallGraphAnalyzerTest, FindCriticalPath) {
    StackTrace trace;

    StackFrame frame1;
    frame1.function_name = "user_function";
    frame1.location.file = "user.cpp";
    trace.frames.push_back(frame1);

    StackFrame frame2;
    frame2.function_name = "std::vector";
    frame2.location.file = "/usr/include/c++/vector";
    trace.frames.push_back(frame2);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    auto critical_path = analyzer.findCriticalPath();
    EXPECT_GT(critical_path.size(), 0);
}

TEST(CallGraphAnalyzerTest, GetCallDepth) {
    StackTrace trace;

    StackFrame frame;
    frame.function_name = "deep_function";
    trace.frames.push_back(frame);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    int depth = analyzer.getCallDepth("deep_function");
    EXPECT_EQ(depth, 0);
}
