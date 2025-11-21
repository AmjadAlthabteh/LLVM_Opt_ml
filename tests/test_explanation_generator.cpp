#include "ai_debugger/ExplanationGenerator.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(ExplanationGeneratorTest, GenerateExplanation) {
    ExplanationGenerator generator;

    StackTrace trace;
    trace.error_message = "SIGSEGV";

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;
    cause.description = "Null pointer dereference";
    cause.confidence = 0.9;

    CallGraphAnalyzer analyzer;

    auto explanation = generator.generate(trace, cause, analyzer);

    EXPECT_FALSE(explanation.summary.empty());
    EXPECT_FALSE(explanation.detailed_analysis.empty());
    EXPECT_GT(explanation.step_by_step.size(), 0);
}

TEST(ExplanationGeneratorTest, ExplainBugCategory) {
    ExplanationGenerator generator;

    auto explanation = generator.explainBugCategory(BugCategory::NULL_POINTER);
    EXPECT_FALSE(explanation.empty());
    EXPECT_NE(explanation.find("null"), std::string::npos);

    explanation = generator.explainBugCategory(BugCategory::BUFFER_OVERFLOW);
    EXPECT_FALSE(explanation.empty());
    EXPECT_NE(explanation.find("buffer"), std::string::npos);
}

TEST(ExplanationGeneratorTest, ToMarkdown) {
    Explanation explanation;
    explanation.summary = "Test Summary";
    explanation.detailed_analysis = "Test Analysis";

    auto markdown = explanation.toMarkdown();

    EXPECT_NE(markdown.find("# Bug Analysis Report"), std::string::npos);
    EXPECT_NE(markdown.find("Test Summary"), std::string::npos);
}

TEST(ExplanationGeneratorTest, ToPlainText) {
    Explanation explanation;
    explanation.summary = "Test Summary";

    auto text = explanation.toPlainText();

    EXPECT_NE(text.find("BUG ANALYSIS REPORT"), std::string::npos);
    EXPECT_NE(text.find("Test Summary"), std::string::npos);
}

TEST(ExplanationGeneratorTest, ExplainCallFlow) {
    ExplanationGenerator generator;

    std::vector<StackFrame> frames;
    StackFrame frame1;
    frame1.function_name = "main";
    frame1.location.file = "main.cpp";
    frame1.location.line = 10;
    frames.push_back(frame1);

    StackFrame frame2;
    frame2.function_name = "process";
    frame2.location.file = "process.cpp";
    frame2.location.line = 42;
    frames.push_back(frame2);

    auto flow = generator.explainCallFlow(frames);

    EXPECT_FALSE(flow.empty());
    EXPECT_NE(flow.find("main"), std::string::npos);
    EXPECT_NE(flow.find("process"), std::string::npos);
}

TEST(ExplanationGeneratorTest, SuggestPreventionStrategies) {
    ExplanationGenerator generator;

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    auto strategies = generator.suggestPreventionStrategies(cause);

    EXPECT_FALSE(strategies.empty());
    EXPECT_NE(strategies.find("check"), std::string::npos);
}
