#ifndef AI_DEBUGGER_FRAMEWORK_H
#define AI_DEBUGGER_FRAMEWORK_H

namespace ai_debugger {

// Unified test framework enum used across the project
enum class TestFramework {
    GTEST,
    CATCH2,
    BOOST_TEST,
    DOCTEST,
    CUSTOM
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_FRAMEWORK_H

