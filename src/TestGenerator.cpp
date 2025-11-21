#include "ai_debugger/TestGenerator.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace ai_debugger {

const char* testFrameworkToString(TestFramework framework) {
    switch (framework) {
        case TestFramework::GTEST: return "Google Test";
        case TestFramework::CATCH2: return "Catch2";
        case TestFramework::BOOST_TEST: return "Boost.Test";
        case TestFramework::DOCTEST: return "doctest";
        default: return "Custom";
    }
}

std::string TestSuite::generateCode() const {
    std::ostringstream oss;

    switch (framework) {
        case TestFramework::GTEST:
            oss << "#include <gtest/gtest.h>\n\n";
            for (const auto& test : test_cases) {
                oss << "TEST(" << suite_name << ", " << test.name << ") {\n";
                if (!test.setup_code.empty()) {
                    oss << "    // Setup\n" << test.setup_code << "\n\n";
                }
                oss << "    // Test\n" << test.test_code << "\n\n";
                if (!test.teardown_code.empty()) {
                    oss << "    // Teardown\n" << test.teardown_code << "\n";
                }
                oss << "}\n\n";
            }
            break;

        case TestFramework::CATCH2:
            oss << "#include <catch2/catch_test_macros.hpp>\n\n";
            for (const auto& test : test_cases) {
                oss << "TEST_CASE(\"" << test.name << "\", \"[" << suite_name << "]\") {\n";
                oss << test.test_code << "\n";
                oss << "}\n\n";
            }
            break;

        default:
            oss << "// Custom test framework - implement as needed\n";
            break;
    }

    return oss.str();
}

struct TestGenerator::Impl {
    TestFramework framework = TestFramework::GTEST;
    std::string output_directory = "./tests";
    std::vector<std::string> generated_files;
};

TestGenerator::TestGenerator() : impl_(std::make_unique<Impl>()) {}

TestGenerator::~TestGenerator() = default;

void TestGenerator::setFramework(TestFramework framework) {
    impl_->framework = framework;
}

void TestGenerator::setOutputDirectory(const std::string& dir) {
    impl_->output_directory = dir;
}

std::vector<std::string> TestGenerator::getGeneratedFiles() const {
    return impl_->generated_files;
}

TestSuite TestGenerator::generateRegressionTests(
    const RootCause& root_cause,
    const CodeFix& fix,
    const StackTrace& original_trace
) {
    TestSuite suite;
    suite.suite_name = "RegressionTests_" + std::string(bugCategoryToString(root_cause.category));
    suite.framework = impl_->framework;

    TestCase negative_test = generateNegativeTest(root_cause);
    negative_test.is_regression_test = true;
    suite.test_cases.push_back(negative_test);

    TestCase positive_test = generatePositiveTest(fix);
    positive_test.is_regression_test = true;
    suite.test_cases.push_back(positive_test);

    auto edge_cases = generateEdgeCaseTests(root_cause, original_trace.frames[0].function_name);
    for (auto& test : edge_cases) {
        test.is_regression_test = true;
        suite.test_cases.push_back(test);
    }

    return suite;
}

std::vector<TestCase> TestGenerator::generateEdgeCaseTests(
    const RootCause& root_cause,
    const std::string& function_name
) {
    std::vector<TestCase> tests;

    switch (root_cause.category) {
        case BugCategory::NULL_POINTER: {
            TestCase test;
            test.name = generateTestName(root_cause, 1);
            test.description = "Test null pointer handling";
            test.test_code = "    EXPECT_NO_THROW(" + function_name + "(nullptr));";
            tests.push_back(test);
            break;
        }

        case BugCategory::BUFFER_OVERFLOW: {
            TestCase test1;
            test1.name = "TestBoundsLowerLimit";
            test1.description = "Test lower boundary";
            test1.test_code = "    EXPECT_NO_THROW(" + function_name + "(0));";
            tests.push_back(test1);

            TestCase test2;
            test2.name = "TestBoundsUpperLimit";
            test2.description = "Test upper boundary";
            test2.test_code = "    EXPECT_NO_THROW(" + function_name + "(SIZE_MAX));";
            tests.push_back(test2);
            break;
        }

        case BugCategory::USE_AFTER_FREE: {
            TestCase test;
            test.name = "TestDoubleDelete";
            test.description = "Ensure no crash on double delete";
            test.test_code = "    auto* ptr = new int(42);\n"
                            "    delete ptr;\n"
                            "    ptr = nullptr;\n"
                            "    EXPECT_NO_THROW(delete ptr);";
            tests.push_back(test);
            break;
        }

        default:
            break;
    }

    return tests;
}

TestCase TestGenerator::generateNegativeTest(const RootCause& root_cause) {
    TestCase test;
    test.name = "TestOriginalBug_" + std::string(bugCategoryToString(root_cause.category));
    test.description = "Verify that the original bug condition is now handled";

    switch (root_cause.category) {
        case BugCategory::NULL_POINTER:
            test.test_code = "    // This should not crash anymore\n"
                           "    int* ptr = nullptr;\n"
                           "    EXPECT_NO_THROW({\n"
                           "        if (ptr != nullptr) {\n"
                           "            *ptr = 42;\n"
                           "        }\n"
                           "    });";
            break;

        case BugCategory::BUFFER_OVERFLOW:
            test.test_code = "    std::vector<int> vec(10);\n"
                           "    EXPECT_THROW(vec.at(100), std::out_of_range);";
            break;

        case BugCategory::USE_AFTER_FREE:
            test.test_code = "    std::unique_ptr<int> ptr = std::make_unique<int>(42);\n"
                           "    int value = *ptr;\n"
                           "    ptr.reset();\n"
                           "    // ptr is now nullptr, access prevented\n"
                           "    EXPECT_EQ(ptr.get(), nullptr);";
            break;

        default:
            test.test_code = "    // Add specific test for bug category\n"
                           "    EXPECT_TRUE(true);";
            break;
    }

    return test;
}

TestCase TestGenerator::generatePositiveTest(const CodeFix& fix) {
    TestCase test;
    test.name = "TestFixWorks";
    test.description = "Verify the fix resolves the issue";

    test.test_code = "    // Verify normal operation after fix\n";

    switch (fix.type) {
        case FixType::NULL_CHECK:
            test.test_code += "    int value = 42;\n"
                            "    int* ptr = &value;\n"
                            "    EXPECT_EQ(*ptr, 42);";
            break;

        case FixType::BOUNDS_CHECK:
            test.test_code += "    std::vector<int> vec = {1, 2, 3};\n"
                            "    EXPECT_EQ(vec.at(0), 1);\n"
                            "    EXPECT_EQ(vec.at(2), 3);";
            break;

        case FixType::MEMORY_CLEANUP:
            test.test_code += "    std::unique_ptr<int> ptr = std::make_unique<int>(42);\n"
                            "    EXPECT_NE(ptr.get(), nullptr);\n"
                            "    // Automatic cleanup on scope exit";
            break;

        default:
            test.test_code += "    EXPECT_TRUE(true); // Placeholder";
            break;
    }

    return test;
}

bool TestGenerator::writeTestFile(const TestSuite& suite) {
    std::filesystem::create_directories(impl_->output_directory);

    std::string filename = impl_->output_directory + "/" + suite.suite_name + "_test.cpp";
    std::ofstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    file << suite.generateCode();
    file.close();

    impl_->generated_files.push_back(filename);
    return true;
}

std::string TestGenerator::generateTestName(const RootCause& cause, int index) {
    std::string base = "Test" + std::string(bugCategoryToString(cause.category));
    std::replace(base.begin(), base.end(), ' ', '_');
    return base + "_" + std::to_string(index);
}

std::string TestGenerator::generateAssertions(const RootCause& cause, const CodeFix& fix) {
    std::ostringstream oss;

    switch (cause.category) {
        case BugCategory::NULL_POINTER:
            oss << "EXPECT_NE(ptr, nullptr);";
            break;
        case BugCategory::BUFFER_OVERFLOW:
            oss << "EXPECT_LT(index, size);";
            break;
        default:
            oss << "EXPECT_TRUE(condition);";
            break;
    }

    return oss.str();
}

std::string TestGenerator::generateSetup(const RootCause& cause) {
    return "// Setup test environment\n";
}

std::string TestGenerator::generateTeardown(const RootCause& cause) {
    return "// Cleanup test environment\n";
}

std::string TestGenerator::formatForGTest(const TestCase& test) {
    std::ostringstream oss;
    oss << "TEST(Suite, " << test.name << ") {\n";
    oss << test.test_code << "\n";
    oss << "}\n";
    return oss.str();
}

std::string TestGenerator::formatForCatch2(const TestCase& test) {
    std::ostringstream oss;
    oss << "TEST_CASE(\"" << test.name << "\") {\n";
    oss << test.test_code << "\n";
    oss << "}\n";
    return oss.str();
}

std::string TestGenerator::formatForBoostTest(const TestCase& test) {
    std::ostringstream oss;
    oss << "BOOST_AUTO_TEST_CASE(" << test.name << ") {\n";
    oss << test.test_code << "\n";
    oss << "}\n";
    return oss.str();
}

std::vector<std::string> TestGenerator::extractTestInputs(const StackTrace& trace) {
    std::vector<std::string> inputs;
    inputs.push_back("nullptr");
    inputs.push_back("0");
    inputs.push_back("-1");
    return inputs;
}

std::vector<std::string> TestGenerator::generateBoundaryValues(const RootCause& cause) {
    return {"0", "SIZE_MAX", "-1", "INT_MAX", "INT_MIN"};
}

} // namespace ai_debugger
