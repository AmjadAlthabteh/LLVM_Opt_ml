#include "ai_debugger/FixSuggester.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(FixSuggesterTest, SuggestNullCheckFixes) {
    FixSuggester suggester;

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    StackTrace trace;

    auto fixes = suggester.suggestFixes(cause, trace);

    ASSERT_GT(fixes.size(), 0);
    EXPECT_EQ(fixes[0].type, FixType::NULL_CHECK);
    EXPECT_FALSE(fixes[0].description.empty());
}

TEST(FixSuggesterTest, SuggestBufferOverflowFixes) {
    FixSuggester suggester;

    RootCause cause;
    cause.category = BugCategory::BUFFER_OVERFLOW;

    StackTrace trace;

    auto fixes = suggester.suggestFixes(cause, trace);

    ASSERT_GT(fixes.size(), 0);
    EXPECT_TRUE(fixes[0].type == FixType::BOUNDS_CHECK || fixes[0].type == FixType::REFACTORING);
}

TEST(FixSuggesterTest, SuggestMemoryFixes) {
    FixSuggester suggester;

    RootCause cause;
    cause.category = BugCategory::USE_AFTER_FREE;

    StackTrace trace;

    auto fixes = suggester.suggestFixes(cause, trace);

    ASSERT_GT(fixes.size(), 0);
    EXPECT_TRUE(fixes[0].type == FixType::MEMORY_CLEANUP || fixes[0].type == FixType::REFACTORING);
}

TEST(FixSuggesterTest, GetBestFix) {
    FixSuggester suggester;

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    StackTrace trace;

    auto best_fix = suggester.getBestFix(cause, trace);

    EXPECT_FALSE(best_fix.description.empty());
    EXPECT_GT(best_fix.confidence, 0.0);
}

TEST(FixSuggesterTest, DryRun) {
    FixSuggester suggester;
    suggester.setDryRun(true);

    CodeFix fix;
    fix.location.file = "test.cpp";
    fix.fixed_code = "fixed";

    auto result = suggester.applyFix(fix);

    EXPECT_TRUE(result.success);
    EXPECT_NE(result.message.find("DRY RUN"), std::string::npos);
}

TEST(FixSuggesterTest, FixTypeToString) {
    EXPECT_STREQ(fixTypeToString(FixType::NULL_CHECK), "Null Check");
    EXPECT_STREQ(fixTypeToString(FixType::BOUNDS_CHECK), "Bounds Check");
    EXPECT_STREQ(fixTypeToString(FixType::SYNCHRONIZATION), "Synchronization");
}
