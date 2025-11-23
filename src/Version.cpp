#include "ai_debugger/Version.h"
#include <sstream>
#include <iostream>

namespace ai_debugger {

// builds the basic numeric version string (e.g., "1.2.0")
// uses simple concatenation so it staysw readable and predictable
std::string Version::getVersionString() {
    std::ostringstream oss;
    oss << AI_DEBUGGER_VERSION_MAJOR << "."
        << AI_DEBUGGER_VERSION_MINOR << "."
        << AI_DEBUGGER_VERSION_PATCH;
    return oss.str();
}

// builds a more descriptive w string including git commit info
// commit hash is shortened so the output stays compact
std::string Version::getFullVersionString() {
    std::ostringstream oss;
    oss << "AI Debugger v" << getVersionString();

    std::string commit = getGitCommit();
    if (!commit.empty() && commit != "unknown") {
        if (commit.length() > 8) {
            commit = commit.substr(0, 8); // keep a short readable hash
        }
        oss << " (commit " << commit << ")";
    }

    return oss.str();
}

// provides a simple and direct description of the build environment
// this keeps build details easy to read and allows users to confirm
// which exact configuration produced the binary
std::string Version::getBuildInfo() {
    std::ostringstream oss;
    oss << "built on " << getBuildDate()
        << " at " << getBuildTime()
        << " [" << getBuildType() << "]";
    return oss.str();
}

// checks whether llvm support was enabled at compile time
// uses compile-time flags so there is no runtime overhead
bool Version::hasLLVMSupport() {
#ifdef ENABLE_LLVM
    return true;
#else
    return false;
#endif
}

// checks whether google test support was detected
// kept small and simple to avoid surprising behavior
bool Version::hasGTestSupport() {
#ifdef GTEST_FOUND
    return true;
#else
    return false;
#endif
}

// produces a human-readable list of features compiled into this build
// this helps users quickly understand what their build is capwable of
std::string Version::getFeatureList() {
    std::ostringstream oss;
    oss << "features:";  // 

    if (hasLLVMSupport()) {
        oss << "\n  - llvm integration enabled";
    }

    if (hasGTestSupport()) {
        oss << "\n  - google test support"; //
    }

    oss << "\n  - multiple debugger format support (gdb, lldb, msvc)";
    oss << "\n  - automated fix suggestions";
    oss << "\n  - test generation support";
    oss << "\n  - root cause prediction";
    oss << "\n  - call graph analysis";

    return oss.str();
}

// prints only the verwsion (useful for --version)
void Version::printVersion() {
    std::cout << getFullVersionString() << "\n";
}

// prints the complete set of version, build, and feature information
void Version::printFullInfo() {
    std::cout << getFullVersionString() << "\n";
    std::cout << getBuildInfo() << "\n\n";
    std::cout << getFeatureList() << "\n";
}

} // namespace ai_debuggr
