find_package(GTest REQUIRED)

include(GoogleTest)

set(TEST_SOURCES
    test_stack_trace_parser.cpp
    test_call_graph_analyzer.cpp
    test_root_cause_predictor.cpp
    test_explanation_generator.cpp
    test_fix_suggester.cpp
    test_test_generator.cpp
    test_integration.cpp
)

add_executable(ai_debugger_tests ${TEST_SOURCES})

target_link_libraries(ai_debugger_tests
    PRIVATE
        ai_debugger
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests(ai_debugger_tests)

add_test(NAME ai_debugger_tests COMMAND ai_debugger_tests)
