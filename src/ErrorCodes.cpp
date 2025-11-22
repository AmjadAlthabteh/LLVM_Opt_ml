#include "ai_debugger/ErrorCodes.h"
#include <sstream>

namespace ai_debugger {

std::string ErrorInfo::toString() const {
    if (!isError()) return "";

    std::ostringstream oss;
    oss << "Error [" << ErrorHandler::getCategoryName(category) << "]: " << message;

    if (!location.empty()) {
        oss << "\n  Location: " << location;
    }

    if (!hint.empty()) {
        oss << "\n  Hint: " << hint;
    }

    oss << "\n  Exit code: " << static_cast<int>(code);

    return oss.str();
}

std::string ErrorInfo::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"error\": true,\n";
    oss << "  \"code\": " << static_cast<int>(code) << ",\n";
    oss << "  \"category\": \"" << ErrorHandler::getCategoryName(category) << "\",\n";
    oss << "  \"message\": \"" << message << "\"";

    if (!location.empty()) {
        oss << ",\n  \"location\": \"" << location << "\"";
    }

    if (!hint.empty()) {
        oss << ",\n  \"hint\": \"" << hint << "\"";
    }

    oss << "\n}";
    return oss.str();
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
