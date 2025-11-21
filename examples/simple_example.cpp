#include "ai_debugger/AIDebugger.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "AI Debugger - Simple Example\n";
    std::cout << "============================\n\n";

    std::string sample_trace = R"(
#0  0x00007ffff7a3d428 in __GI_raise (sig=sig@entry=6) at ../sysdeps/unix/sysv/linux/raise.c:54
#1  0x00007ffff7a3f02a in __GI_abort () at abort.c:89
#2  0x00007ffff7a867ea in __libc_message (action=action@entry=do_abort, fmt=fmt@entry=0x7ffff7b9b1f8 "*** %s ***: terminated\n") at ../sysdeps/posix/libc_fatal.c:155
#3  0x00007ffff7a8f15c in __GI___fortify_fail (msg=msg@entry=0x7ffff7b9b14a "stack smashing detected") at fortify_fail.c:26
#4  0x00007ffff7a8f120 in __stack_chk_fail () at stack_chk_fail.c:24
#5  0x0000555555555269 in vulnerable_function (input=0x7fffffffe0a0 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") at test.cpp:15
#6  0x00005555555552a8 in main () at test.cpp:20
)";

    ai_debugger::AIDebugger debugger;

    debugger.setVerbose(true);
    debugger.enableAutoFix(false);
    debugger.enableTestGeneration(true);

    std::cout << "Analyzing stack trace...\n\n";
    auto session = debugger.analyzeStackTrace(sample_trace);

    std::cout << debugger.getReport(session);

    if (!session.root_causes.empty()) {
        std::cout << "\n\nDetailed Explanation:\n";
        std::cout << "====================\n";
        std::cout << session.explanation.toMarkdown() << "\n";
    }

    if (!session.suggested_fixes.empty()) {
        std::cout << "\nNumber of suggested fixes: " << session.suggested_fixes.size() << "\n";
        std::cout << "Best fix: " << session.suggested_fixes[0].description << "\n";
        std::cout << "Confidence: " << (session.suggested_fixes[0].confidence * 100) << "%\n";
    }

    if (debugger.generateTests(session)) {
        std::cout << "\nRegression tests generated successfully!\n";
        auto test_files = debugger.listSessions();
        std::cout << "Generated " << test_files.size() << " test sessions\n";
    }

    return 0;
}
