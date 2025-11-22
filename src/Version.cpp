#include "ai_debugger/Version.h"
#include <sstream>
#include <iostream>

namespace ai_debugger {

std::string Version::getVersionString() {
    std::ostringstream oss;
    oss << AI_DEBUGGER_VERSION_MAJOR << "."
        << AI_DEBUGGER_VERSION_MINOR << "."
        << AI_DEBUGGER_VERSION_PATCH;
    return oss.str();
}

std::string Version::getFullVersionString() {
    std::ostringstream oss;
    oss << "AI Debugger v" << getVersionString();

    std::string commit = getGitCommit();
    if (commit != "unknown" && !commit.empty()) {
        if (commit.length() > 8) {
            commit = commit.substr(0, 8);
        }
        oss << " (commit " << commit << ")";
    }

    return oss.str();
}

std::string Version::getBuildInfo() {
    std::ostringstream oss;
    oss << "Built on " << getBuildDate() << " at " << getBuildTime();
    oss << " [" << getBuildType() << "]";
    return oss.str();
}

bool Version::hasLLVMSupport() {
#ifdef ENABLE_LLVM
    return true;
#else
    return false;
#endif
}

bool Version::hasGTestSupport() {
#ifdef GTEST_FOUND
    return true;
#else
    return false;
#endif
}

std::string Version::getFeatureList() {
    std::ostringstream oss;
    oss << "Features:";

    if (hasLLVMSupport()) {
        oss << "\n  - LLVM integration enabled";
    }

    if (hasGTestSupport()) {
        oss << "\n  - Google Test support";
    }

    oss << "\n  - Multi-debugger format support (GDB, LLDB, MSVC)";
    oss << "\n  - Automated fix suggestions";
    oss << "\n  - Test generation (GTest, Catch2, Boost.Test)";
    oss << "\n  - Root cause prediction";
    oss << "\n  - Call graph analysis";

    return oss.str();
}

void Version::printVersion() {
    std::cout << getFullVersionString() << "\n";
}

void Version::printFullInfo() {
    std::cout << getFullVersionString() << "\n";
    std::cout << getBuildInfo() << "\n\n";
    std::cout << getFeatureList() << "\n";
}

} // namespace ai_debugger
