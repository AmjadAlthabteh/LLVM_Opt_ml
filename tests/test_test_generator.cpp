#include "ai_debugger/TestGenerator.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(TestGeneratorTest, GenerateRegressionTests) {
    TestGenerator generator;

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    CodeFix fix;
    fix.type = FixType::NULL_CHECK;

    StackTrace trace;

    auto suite = generator.generateRegressionTests(cause, fix, trace);

    EXPECT_FALSE(suite.suite_name.empty());
    EXPECT_GT(suite.test_cases.size(), 0);
}

TEST(TestGeneratorTest, GenerateEdgeCaseTests) {
    TestGenerator generator;

    RootCause cause;
    cause.category = BugCategory::BUFFER_OVERFLOW;

    auto tests = generator.generateEdgeCaseTests(cause, "test_function");

    EXPECT_GT(tests.size(), 0);
}

TEST(TestGeneratorTest, GenerateNegativeTest) {
    TestGenerator generator;

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    auto test = generator.generateNegativeTest(cause);

    EXPECT_FALSE(test.name.empty());
    EXPECT_FALSE(test.test_code.empty());
}

TEST(TestGeneratorTest, GeneratePositiveTest) {
    TestGenerator generator;

    CodeFix fix;
    fix.type = FixType::NULL_CHECK;

    auto test = generator.generatePositiveTest(fix);

    EXPECT_FALSE(test.name.empty());
    EXPECT_FALSE(test.test_code.empty());
}

TEST(TestGeneratorTest, TestSuiteGenerateCode) {
    TestSuite suite;
    suite.suite_name = "TestSuite";
    suite.framework = TestFramework::GTEST;

    TestCase test;
    test.name = "TestCase1";
    test.test_code = "EXPECT_TRUE(true);";
    suite.test_cases.push_back(test);

    auto code = suite.generateCode();

    EXPECT_NE(code.find("TEST("), std::string::npos);
    EXPECT_NE(code.find("TestSuite"), std::string::npos);
}

TEST(TestGeneratorTest, SetFramework) {
    TestGenerator generator;

    generator.setFramework(TestFramework::CATCH2);

    RootCause cause;
    cause.category = BugCategory::NULL_POINTER;

    CodeFix fix;
    fix.type = FixType::NULL_CHECK;

    StackTrace trace;

    auto suite = generator.generateRegressionTests(cause, fix, trace);

    EXPECT_EQ(suite.framework, TestFramework::CATCH2);
}

TEST(TestGeneratorTest, TestFrameworkToString) {
    EXPECT_STREQ(testFrameworkToString(TestFramework::GTEST), "Google Test");
    EXPECT_STREQ(testFrameworkToString(TestFramework::CATCH2), "Catch2");
    EXPECT_STREQ(testFrameworkToString(TestFramework::BOOST_TEST), "Boost.Test");
}
