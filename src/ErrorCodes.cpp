#include "ai_debugger/ErrorCodes.h"
#include <sstream>

namespace ai_debugger {

std::string ErrorInfo::toString() const {
    if (!isError()) return "";
    return "Error: " + message;
}

std::string ErrorInfo::toJSON() const {
    return "{\"error\":\"" + message + "\"}";
}

ErrorInfo ErrorHandler::createFileNotFound(const std::string& filepath) {
    return ErrorInfo(
        ExitCode::FILE_NOT_FOUND,
        ErrorCategory::FILE_IO,
        "File not found: " + filepath,
        "Check that the file path is correct and the file exists",
        filepath
    );
}

ErrorInfo ErrorHandler::createParseError(const std::string& details, int line) {
    std::string location = line >= 0 ? "line " + std::to_string(line) : "";
    return ErrorInfo(
        ExitCode::PARSE_ERROR,
        ErrorCategory::PARSING,
        "Parse error: " + details,
        "Ensure the input format is correct and matches expected debugger output",
        location
    );
}

ErrorInfo ErrorHandler::createInvalidInput(const std::string& details) {
    return ErrorInfo(
        ExitCode::INVALID_INPUT,
        ErrorCategory::PARSING,
        "Invalid input: " + details,
        "Check the input format and try again"
    );
}

ErrorInfo ErrorHandler::createConfigError(const std::string& details) {
    return ErrorInfo(
        ExitCode::CONFIGURATION_ERROR,
        ErrorCategory::CONFIGURATION,
        "Configuration error: " + details,
        "Check your configuration file or command-line options"
    );
}

ErrorInfo ErrorHandler::createInternalError(const std::string& details) {
    return ErrorInfo(
        ExitCode::INTERNAL_ERROR,
        ErrorCategory::SYSTEM,
        "Internal error: " + details,
        "This is likely a bug. Please report it to the developers"
    );
}

std::string ErrorHandler::getErrorMessage(ExitCode code) {
    auto it = ERROR_DESCRIPTIONS.find(code);
    if (it != ERROR_DESCRIPTIONS.end()) {
        return it->second;
    }
    return "Unknown error";
}

std::string ErrorHandler::getCategoryName(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::NONE: return "NONE";
        case ErrorCategory::FILE_IO: return "FILE_IO";
        case ErrorCategory::PARSING: return "PARSING";
        case ErrorCategory::ANALYSIS: return "ANALYSIS";
        case ErrorCategory::GENERATION: return "GENERATION";
        case ErrorCategory::CONFIGURATION: return "CONFIGURATION";
        case ErrorCategory::SYSTEM: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

} // namespace ai_debugger
