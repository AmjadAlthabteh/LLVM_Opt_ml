#ifndef AI_DEBUGGER_FIX_SUGGESTER_H
#define AI_DEBUGGER_FIX_SUGGESTER_H

#include "StackTraceParser.h"
#include "RootCausePredictor.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ai_debugger {

enum class FixType {
    NULL_CHECK,
    BOUNDS_CHECK,
    INITIALIZATION,
    MEMORY_CLEANUP,
    SYNCHRONIZATION,
    REFACTORING,
    ASSERTION,
    ERROR_HANDLING,
    CUSTOM
};

struct CodeFix {
    FixType type;
    std::string description;
    SourceLocation location;
    std::string original_code;
    std::string fixed_code;
    double confidence;
    std::vector<std::string> affected_files;

    CodeFix() : type(FixType::CUSTOM), confidence(0.0) {}
};

struct FixApplication {
    bool success;
    std::string message;
    std::vector<std::string> modified_files;
    std::vector<std::string> backup_files;

    FixApplication() : success(false) {}
};

class FixSuggester {
public:
    FixSuggester();
    ~FixSuggester();

    std::vector<CodeFix> suggestFixes(
        const RootCause& root_cause,
        const StackTrace& trace
    );

    CodeFix getBestFix(
        const RootCause& root_cause,
        const StackTrace& trace
    );

    FixApplication applyFix(const CodeFix& fix, bool create_backup = true);
    std::vector<FixApplication> applyAllFixes(
        const std::vector<CodeFix>& fixes,
        bool create_backup = true
    );

    void rollbackFix(const FixApplication& application);

    void setSourceRoot(const std::string& root_path);
    void setDryRun(bool dry_run);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    std::vector<CodeFix> generateNullCheckFixes(const RootCause& cause);
    std::vector<CodeFix> generateBoundsCheckFixes(const RootCause& cause);
    std::vector<CodeFix> generateMemoryFixes(const RootCause& cause);
    std::vector<CodeFix> generateConcurrencyFixes(const RootCause& cause);

    std::string readSourceFile(const std::string& path);
    bool writeSourceFile(const std::string& path, const std::string& content);
    std::string createBackup(const std::string& path);

    std::string generateFixedCode(const CodeFix& fix);
    bool validateFix(const CodeFix& fix);
};

const char* fixTypeToString(FixType type);

} // namespace ai_debugger

#endif // AI_DEBUGGER_FIX_SUGGESTER_H
