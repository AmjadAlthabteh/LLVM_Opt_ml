#ifndef AI_DEBUGGER_VERSION_H
#define AI_DEBUGGER_VERSION_H

#include <string>

namespace ai_debugger {

// Version information - automatically updated by build system
#define AI_DEBUGGER_VERSION_MAJOR 1
#define AI_DEBUGGER_VERSION_MINOR 0
#define AI_DEBUGGER_VERSION_PATCH 0

// Build metadata
#ifndef AI_DEBUGGER_BUILD_DATE
#define AI_DEBUGGER_BUILD_DATE __DATE__
#endif

#ifndef AI_DEBUGGER_BUILD_TIME
#define AI_DEBUGGER_BUILD_TIME __TIME__
#endif

#ifndef AI_DEBUGGER_GIT_COMMIT
#define AI_DEBUGGER_GIT_COMMIT "unknown"
#endif

#ifndef AI_DEBUGGER_BUILD_TYPE
#define AI_DEBUGGER_BUILD_TYPE "Release"
#endif

class Version {
public:
    static int getMajor() { return AI_DEBUGGER_VERSION_MAJOR; }
    static int getMinor() { return AI_DEBUGGER_VERSION_MINOR; }
    static int getPatch() { return AI_DEBUGGER_VERSION_PATCH; }

    static std::string getVersionString();
    static std::string getFullVersionString();
    static std::string getBuildInfo();
    static std::string getBuildDate() { return AI_DEBUGGER_BUILD_DATE; }
    static std::string getBuildTime() { return AI_DEBUGGER_BUILD_TIME; }
    static std::string getGitCommit() { return AI_DEBUGGER_GIT_COMMIT; }
    static std::string getBuildType() { return AI_DEBUGGER_BUILD_TYPE; }

    // Feature flags
    static bool hasLLVMSupport();
    static bool hasGTestSupport();
    static std::string getFeatureList();

    static void printVersion();
    static void printFullInfo();
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_VERSION_H
