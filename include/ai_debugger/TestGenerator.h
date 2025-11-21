#ifndef AI_DEBUGGER_TEST_GENERATOR_H
#define AI_DEBUGGER_TEST_GENERATOR_H

#include "StackTraceParser.h"
#include "RootCausePredictor.h"
#include "FixSuggester.h"
#include <string>
#include <vector>
#include <memory>

namespace ai_debugger {

enum class TestFramework {
    GTEST,
    CATCH2,
    BOOST_TEST,
    DOCTEST,
    CUSTOM
};

struct TestCase {
    std::string name;
    std::string description;
    std::string test_code;
    std::string setup_code;
    std::string teardown_code;
    std::vector<std::string> dependencies;
    bool is_regression_test;

    TestCase() : is_regression_test(false) {}
};

struct TestSuite {
    std::string suite_name;
    std::vector<TestCase> test_cases;
    std::string file_path;
    TestFramework framework;

    TestSuite() : framework(TestFramework::GTEST) {}

    std::string generateCode() const;
};

class TestGenerator {
public:
    TestGenerator();
    ~TestGenerator();

    TestSuite generateRegressionTests(
        const RootCause& root_cause,
        const CodeFix& fix,
        const StackTrace& original_trace
    );

    std::vector<TestCase> generateEdgeCaseTests(
        const RootCause& root_cause,
        const std::string& function_name
    );

    TestCase generateNegativeTest(const RootCause& root_cause);
    TestCase generatePositiveTest(const CodeFix& fix);

    void setFramework(TestFramework framework);
    void setOutputDirectory(const std::string& dir);

    bool writeTestFile(const TestSuite& suite);
    std::vector<std::string> getGeneratedFiles() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    std::string generateTestName(const RootCause& cause, int index);
    std::string generateAssertions(const RootCause& cause, const CodeFix& fix);
    std::string generateSetup(const RootCause& cause);
    std::string generateTeardown(const RootCause& cause);

    std::string formatForGTest(const TestCase& test);
    std::string formatForCatch2(const TestCase& test);
    std::string formatForBoostTest(const TestCase& test);

    std::vector<std::string> extractTestInputs(const StackTrace& trace);
    std::vector<std::string> generateBoundaryValues(const RootCause& cause);
};

const char* testFrameworkToString(TestFramework framework);

} // namespace ai_debugger

#endif // AI_DEBUGGER_TEST_GENERATOR_H
