#include "ai_debugger/RootCausePredictor.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(RootCausePredictorTest, PredictNullPointer) {
    RootCausePredictor predictor;

    StackTrace trace;
    trace.error_message = "Segmentation fault: null pointer dereference at 0x0";

    CallGraphAnalyzer analyzer;

    auto causes = predictor.predict(trace, analyzer);

    ASSERT_GT(causes.size(), 0);
    EXPECT_EQ(causes[0].category, BugCategory::NULL_POINTER);
    EXPECT_GT(causes[0].confidence, 0.0);
}

TEST(RootCausePredictorTest, PredictDoubleFree) {
    RootCausePredictor predictor;

    StackTrace trace;
    trace.error_message = "double free or corruption";

    CallGraphAnalyzer analyzer;

    auto causes = predictor.predict(trace, analyzer);

    ASSERT_GT(causes.size(), 0);
    EXPECT_EQ(causes[0].category, BugCategory::DOUBLE_FREE);
    EXPECT_GT(causes[0].confidence, 0.9);
}

TEST(RootCausePredictorTest, GetMostLikelyCause) {
    RootCausePredictor predictor;

    StackTrace trace;
    trace.error_message = "SIGSEGV";

    CallGraphAnalyzer analyzer;

    auto cause = predictor.getMostLikelyCause(trace, analyzer);

    EXPECT_NE(cause.category, BugCategory::UNKNOWN);
    EXPECT_GT(cause.confidence, 0.0);
}

TEST(RootCausePredictorTest, BugCategoryToString) {
    EXPECT_STREQ(bugCategoryToString(BugCategory::NULL_POINTER), "Null Pointer Dereference");
    EXPECT_STREQ(bugCategoryToString(BugCategory::BUFFER_OVERFLOW), "Buffer Overflow");
    EXPECT_STREQ(bugCategoryToString(BugCategory::MEMORY_LEAK), "Memory Leak");
}

TEST(RootCausePredictorTest, StringToBugCategory) {
    EXPECT_EQ(stringToBugCategory("Null Pointer Dereference"), BugCategory::NULL_POINTER);
    EXPECT_EQ(stringToBugCategory("Buffer Overflow"), BugCategory::BUFFER_OVERFLOW);
    EXPECT_EQ(stringToBugCategory("Unknown"), BugCategory::UNKNOWN);
}

TEST(RootCausePredictorTest, ConfidenceRanking) {
    RootCausePredictor predictor;

    StackTrace trace;
    trace.error_message = "heap corruption detected";

    StackFrame frame;
    frame.function_name = "malloc";
    trace.frames.push_back(frame);

    CallGraphAnalyzer analyzer;
    analyzer.buildFromStackTrace(trace);

    auto causes = predictor.predict(trace, analyzer);

    ASSERT_GT(causes.size(), 0);

    for (size_t i = 1; i < causes.size(); ++i) {
        EXPECT_GE(causes[i - 1].confidence, causes[i].confidence);
    }
}
