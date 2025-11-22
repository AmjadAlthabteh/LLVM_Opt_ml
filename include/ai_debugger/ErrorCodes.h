#ifndef AI_DEBUGGER_ERROR_CODES_H
#define AI_DEBUGGER_ERROR_CODES_H

#include <string>
#include <unordered_map>

namespace ai_debugger {

// Standard exit codes following POSIX conventions
enum class ExitCode {
    SUCCESS = 0,
    GENERAL_ERROR = 1,
    USAGE_ERROR = 2,
    FILE_NOT_FOUND = 3,
    PERMISSION_DENIED = 4,
    INVALID_INPUT = 5,
    PARSE_ERROR = 6,
    NO_ROOT_CAUSE_FOUND = 7,
    FIX_APPLICATION_FAILED = 8,
    TEST_GENERATION_FAILED = 9,
    CONFIGURATION_ERROR = 10,
    DEPENDENCY_ERROR = 11,
    INTERNAL_ERROR = 99
};

// Error categories for detailed diagnostics
enum class ErrorCategory {
    NONE,
    FILE_IO,
    PARSING,
    ANALYSIS,
    GENERATION,
    CONFIGURATION,
    SYSTEM
};

struct ErrorInfo {
    ExitCode code;
    ErrorCategory category;
    std::string message;
    std::string hint;
    std::string location;

    ErrorInfo(ExitCode c = ExitCode::SUCCESS,
              ErrorCategory cat = ErrorCategory::NONE,
              const std::string& msg = "",
              const std::string& h = "",
              const std::string& loc = "")
        : code(c), category(cat), message(msg), hint(h), location(loc) {}

    bool isError() const { return code != ExitCode::SUCCESS; }
    std::string toString() const;
    std::string toJSON() const;
};

class ErrorHandler {
public:
    static ErrorInfo createFileNotFound(const std::string& filepath);
    static ErrorInfo createParseError(const std::string& details, int line = -1);
    static ErrorInfo createInvalidInput(const std::string& details);
    static ErrorInfo createConfigError(const std::string& details);
    static ErrorInfo createInternalError(const std::string& details);

    static std::string getErrorMessage(ExitCode code);
    static std::string getCategoryName(ErrorCategory category);
};

// Global error code descriptions
static const std::unordered_map<ExitCode, std::string> ERROR_DESCRIPTIONS = {
    {ExitCode::SUCCESS, "Operation completed successfully"},
    {ExitCode::GENERAL_ERROR, "General error occurred"},
    {ExitCode::USAGE_ERROR, "Invalid command-line usage"},
    {ExitCode::FILE_NOT_FOUND, "Required file not found"},
    {ExitCode::PERMISSION_DENIED, "Permission denied"},
    {ExitCode::INVALID_INPUT, "Invalid input provided"},
    {ExitCode::PARSE_ERROR, "Failed to parse input"},
    {ExitCode::NO_ROOT_CAUSE_FOUND, "Could not determine root cause"},
    {ExitCode::FIX_APPLICATION_FAILED, "Failed to apply suggested fix"},
    {ExitCode::TEST_GENERATION_FAILED, "Failed to generate tests"},
    {ExitCode::CONFIGURATION_ERROR, "Configuration error"},
    {ExitCode::DEPENDENCY_ERROR, "Missing or incompatible dependency"},
    {ExitCode::INTERNAL_ERROR, "Internal error"}
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_ERROR_CODES_H
