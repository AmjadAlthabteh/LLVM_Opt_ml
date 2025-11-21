#include "ai_debugger/StackTraceParser.h"
#include <gtest/gtest.h>

using namespace ai_debugger;

TEST(StackTraceParserTest, ParseGDBTrace) {
    StackTraceParser parser;

    std::string gdb_trace = R"(
#0  0x00007ffff7a3d428 in __GI_raise (sig=sig@entry=6) at ../sysdeps/unix/sysv/linux/raise.c:54
#1  0x00007ffff7a3f02a in __GI_abort () at abort.c:89
#2  0x0000555555555269 in vulnerable_function (input=0x7fffffffe0a0) at test.cpp:15
    )";

    auto result = parser.parseGDB(gdb_trace);

    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->frames.size(), 0);
    EXPECT_EQ(result->frames[0].function_name, "__GI_raise");
}

TEST(StackTraceParserTest, ParseLLDBTrace) {
    StackTraceParser parser;

    std::string lldb_trace = R"(
frame #0: 0x00007fff20394a16 libsystem_kernel.dylib`__pthread_kill + 10
frame #1: 0x00007fff203c3487 libsystem_pthread.dylib`pthread_kill + 285
frame #2: 0x0000000100000f5a test`main at test.cpp:42
    )";

    auto result = parser.parseLLDB(lldb_trace);

    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->frames.size(), 0);
}

TEST(StackTraceParserTest, AutoDetectFormat) {
    StackTraceParser parser;

    std::string gdb_trace = "#0  0x0000555555555269 in main () at test.cpp:15";

    auto result = parser.parse(gdb_trace);

    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->frames.size(), 0);
}

TEST(StackTraceParserTest, ExtractLocation) {
    StackTraceParser parser;

    auto loc = parser.extractLocation("test.cpp:42:10");

    ASSERT_TRUE(loc.has_value());
    EXPECT_EQ(loc->file, "test.cpp");
    EXPECT_EQ(loc->line, 42);
    EXPECT_EQ(loc->column, 10);
}

TEST(StackTraceParserTest, EmptyTrace) {
    StackTraceParser parser;

    auto result = parser.parse("");

    EXPECT_FALSE(result.has_value());
}

TEST(StackTraceParserTest, InvalidTrace) {
    StackTraceParser parser;

    auto result = parser.parse("This is not a stack trace");

    EXPECT_FALSE(result.has_value());
}
