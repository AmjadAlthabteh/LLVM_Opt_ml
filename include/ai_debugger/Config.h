#ifndef AI_DEBUGGER_CONFIG_H
#define AI_DEBUGGER_CONFIG_H

#include <string>
#include <unordered_map>
#include <vector>

namespace ai_debugger {

enum class TestFramework {
    GTEST,
    CATCH2,
    BOOST_TEST,
    DOCTEST
};

enum class OutputFormat {
    TEXT,
    JSON,
    XML,
    MARKDOWN
};

enum class LogLevel {
    SILENT,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    TRACE
};

struct Config {
    // Paths
    std::string source_directory;
    std::string output_directory;
    std::string test_output_directory;

    // Behavior
    bool verbose = false;
    bool auto_fix = false;
    bool auto_test = false;
    bool interactive = false;
    bool color_output = true;
    bool show_progress = true;

    // Frameworks and formats
    TestFramework test_framework = TestFramework::GTEST;
    OutputFormat output_format = OutputFormat::TEXT;
    LogLevel log_level = LogLevel::INFO;

    // Analysis options
    int max_fixes_to_suggest = 5;
    double confidence_threshold = 0.7;
    bool enable_llvm_analysis = false;
    bool generate_detailed_reports = true;

    // Performance
    int max_parallel_tasks = 4;
    int timeout_seconds = 300;

    // Custom patterns
    std::vector<std::string> ignore_patterns;
    std::vector<std::string> include_patterns;

    // Advanced options
    std::unordered_map<std::string, std::string> custom_options;

    Config();

    // Configuration file methods
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;

    // Parse from command line
    bool parseCommandLine(int argc, char* argv[]);

    // Merge configurations (CLI overrides file config)
    void merge(const Config& other);

    // Validation
    bool validate(std::string& error_message) const;

    // Get default config file paths
    static std::vector<std::string> getDefaultConfigPaths();
    static std::string getDefaultConfigPath();

    // Convert enums to/from strings
    static std::string testFrameworkToString(TestFramework framework);
    static TestFramework stringToTestFramework(const std::string& str);

    static std::string outputFormatToString(OutputFormat format);
    static OutputFormat stringToOutputFormat(const std::string& str);

    static std::string logLevelToString(LogLevel level);
    static LogLevel stringToLogLevel(const std::string& str);

    // Print current configuration
    std::string toString() const;
};

class ConfigLoader {
public:
    static Config loadDefault();
    static Config loadFromFileOrDefault(const std::string& filepath);
    static Config loadWithCommandLine(int argc, char* argv[]);

private:
    static Config tryLoadFromDefaultLocations();
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_CONFIG_H
