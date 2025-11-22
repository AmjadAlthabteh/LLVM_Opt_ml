#include "ai_debugger/Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

namespace ai_debugger {

Config::Config() {
    // Set sensible defaults
    source_directory = ".";
    output_directory = ".";
    test_output_directory = "./tests/generated";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        line_number++;

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Remove quotes if present
        if (value.size() >= 2 && value[0] == '"' && value[value.size()-1] == '"') {
            value = value.substr(1, value.size() - 2);
        }

        // Set configuration values
        if (key == "source_directory") {
            source_directory = value;
        } else if (key == "output_directory") {
            output_directory = value;
        } else if (key == "test_output_directory") {
            test_output_directory = value;
        } else if (key == "verbose") {
            verbose = (value == "true" || value == "1" || value == "yes");
        } else if (key == "auto_fix") {
            auto_fix = (value == "true" || value == "1" || value == "yes");
        } else if (key == "auto_test") {
            auto_test = (value == "true" || value == "1" || value == "yes");
        } else if (key == "interactive") {
            interactive = (value == "true" || value == "1" || value == "yes");
        } else if (key == "color_output") {
            color_output = (value == "true" || value == "1" || value == "yes");
        } else if (key == "show_progress") {
            show_progress = (value == "true" || value == "1" || value == "yes");
        } else if (key == "test_framework") {
            test_framework = stringToTestFramework(value);
        } else if (key == "output_format") {
            output_format = stringToOutputFormat(value);
        } else if (key == "log_level") {
            log_level = stringToLogLevel(value);
        } else if (key == "max_fixes_to_suggest") {
            max_fixes_to_suggest = std::stoi(value);
        } else if (key == "confidence_threshold") {
            confidence_threshold = std::stod(value);
        } else if (key == "enable_llvm_analysis") {
            enable_llvm_analysis = (value == "true" || value == "1" || value == "yes");
        } else if (key == "max_parallel_tasks") {
            max_parallel_tasks = std::stoi(value);
        } else if (key == "timeout_seconds") {
            timeout_seconds = std::stoi(value);
        } else {
            // Store custom options
            custom_options[key] = value;
        }
    }

    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << "# AI Debugger Configuration File\n";
    file << "# Generated automatically - edit as needed\n\n";

    file << "# Paths\n";
    file << "source_directory = \"" << source_directory << "\"\n";
    file << "output_directory = \"" << output_directory << "\"\n";
    file << "test_output_directory = \"" << test_output_directory << "\"\n\n";

    file << "# Behavior\n";
    file << "verbose = " << (verbose ? "true" : "false") << "\n";
    file << "auto_fix = " << (auto_fix ? "true" : "false") << "\n";
    file << "auto_test = " << (auto_test ? "true" : "false") << "\n";
    file << "interactive = " << (interactive ? "true" : "false") << "\n";
    file << "color_output = " << (color_output ? "true" : "false") << "\n";
    file << "show_progress = " << (show_progress ? "true" : "false") << "\n\n";

    file << "# Frameworks and formats\n";
    file << "test_framework = " << testFrameworkToString(test_framework) << "\n";
    file << "output_format = " << outputFormatToString(output_format) << "\n";
    file << "log_level = " << logLevelToString(log_level) << "\n\n";

    file << "# Analysis options\n";
    file << "max_fixes_to_suggest = " << max_fixes_to_suggest << "\n";
    file << "confidence_threshold = " << confidence_threshold << "\n";
    file << "enable_llvm_analysis = " << (enable_llvm_analysis ? "true" : "false") << "\n\n";

    file << "# Performance\n";
    file << "max_parallel_tasks = " << max_parallel_tasks << "\n";
    file << "timeout_seconds = " << timeout_seconds << "\n\n";

    if (!custom_options.empty()) {
        file << "# Custom options\n";
        for (const auto& [key, value] : custom_options) {
            file << key << " = \"" << value << "\"\n";
        }
    }

    return true;
}

bool Config::validate(std::string& error_message) const {
    if (max_fixes_to_suggest < 1 || max_fixes_to_suggest > 100) {
        error_message = "max_fixes_to_suggest must be between 1 and 100";
        return false;
    }

    if (confidence_threshold < 0.0 || confidence_threshold > 1.0) {
        error_message = "confidence_threshold must be between 0.0 and 1.0";
        return false;
    }

    if (max_parallel_tasks < 1 || max_parallel_tasks > 128) {
        error_message = "max_parallel_tasks must be between 1 and 128";
        return false;
    }

    if (timeout_seconds < 1) {
        error_message = "timeout_seconds must be positive";
        return false;
    }

    return true;
}

std::vector<std::string> Config::getDefaultConfigPaths() {
    std::vector<std::string> paths;

    // Current directory
    paths.push_back(".aidebuggerrc");
    paths.push_back(".aidebugger.conf");

#ifdef _WIN32
    // Windows: %APPDATA%\aidebugger\config
    char* appdata = std::getenv("APPDATA");
    if (appdata) {
        paths.push_back(std::string(appdata) + "\\aidebugger\\config");
    }
#else
    // Linux/Mac: ~/.config/aidebugger/config and ~/.aidebuggerrc
    const char* home = std::getenv("HOME");
    if (home) {
        paths.push_back(std::string(home) + "/.config/aidebugger/config");
        paths.push_back(std::string(home) + "/.aidebuggerrc");
    }
#endif

    return paths;
}

std::string Config::getDefaultConfigPath() {
    auto paths = getDefaultConfigPaths();
    for (const auto& path : paths) {
        std::ifstream file(path);
        if (file.good()) {
            return path;
        }
    }
    return paths[0]; // Return first option if none exist
}

std::string Config::testFrameworkToString(TestFramework framework) {
    switch (framework) {
        case TestFramework::GTEST: return "gtest";
        case TestFramework::CATCH2: return "catch2";
        case TestFramework::BOOST_TEST: return "boost";
        case TestFramework::DOCTEST: return "doctest";
        default: return "gtest";
    }
}

TestFramework Config::stringToTestFramework(const std::string& str) {
    if (str == "catch2") return TestFramework::CATCH2;
    if (str == "boost") return TestFramework::BOOST_TEST;
    if (str == "doctest") return TestFramework::DOCTEST;
    return TestFramework::GTEST;
}

std::string Config::outputFormatToString(OutputFormat format) {
    switch (format) {
        case OutputFormat::TEXT: return "text";
        case OutputFormat::JSON: return "json";
        case OutputFormat::XML: return "xml";
        case OutputFormat::MARKDOWN: return "markdown";
        default: return "text";
    }
}

OutputFormat Config::stringToOutputFormat(const std::string& str) {
    if (str == "json") return OutputFormat::JSON;
    if (str == "xml") return OutputFormat::XML;
    if (str == "markdown") return OutputFormat::MARKDOWN;
    return OutputFormat::TEXT;
}

std::string Config::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::SILENT: return "silent";
        case LogLevel::ERROR: return "error";
        case LogLevel::WARNING: return "warning";
        case LogLevel::INFO: return "info";
        case LogLevel::DEBUG: return "debug";
        case LogLevel::TRACE: return "trace";
        default: return "info";
    }
}

LogLevel Config::stringToLogLevel(const std::string& str) {
    if (str == "silent") return LogLevel::SILENT;
    if (str == "error") return LogLevel::ERROR;
    if (str == "warning") return LogLevel::WARNING;
    if (str == "debug") return LogLevel::DEBUG;
    if (str == "trace") return LogLevel::TRACE;
    return LogLevel::INFO;
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Configuration:\n";
    oss << "  Source directory: " << source_directory << "\n";
    oss << "  Output directory: " << output_directory << "\n";
    oss << "  Test framework: " << testFrameworkToString(test_framework) << "\n";
    oss << "  Output format: " << outputFormatToString(output_format) << "\n";
    oss << "  Log level: " << logLevelToString(log_level) << "\n";
    oss << "  Verbose: " << (verbose ? "yes" : "no") << "\n";
    oss << "  Auto-fix: " << (auto_fix ? "yes" : "no") << "\n";
    oss << "  Auto-test: " << (auto_test ? "yes" : "no") << "\n";
    return oss.str();
}

Config ConfigLoader::loadDefault() {
    return Config();
}

Config ConfigLoader::loadFromFileOrDefault(const std::string& filepath) {
    Config config;
    if (!config.loadFromFile(filepath)) {
        std::cerr << "Warning: Could not load config from " << filepath << "\n";
    }
    return config;
}

Config ConfigLoader::tryLoadFromDefaultLocations() {
    Config config;
    for (const auto& path : Config::getDefaultConfigPaths()) {
        if (config.loadFromFile(path)) {
            return config;
        }
    }
    return config; // Return default if none found
}

Config ConfigLoader::loadWithCommandLine(int argc, char* argv[]) {
    Config config = tryLoadFromDefaultLocations();
    // Command line parsing would go here
    return config;
}

} // namespace ai_debugger
