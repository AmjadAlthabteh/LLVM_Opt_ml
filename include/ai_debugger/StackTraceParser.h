#ifndef AI_DEBUGGER_STACK_TRACE_PARSER_H
#define AI_DEBUGGER_STACK_TRACE_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <regex>

namespace ai_debugger {

struct SourceLocation {
    std::string file;
    int line;
    int column;

    SourceLocation() : line(0), column(0) {}
    SourceLocation(const std::string& f, int l, int c = 0)
        : file(f), line(l), column(c) {}
};

struct StackFrame {
    std::string function_name;
    std::string mangled_name;
    SourceLocation location;
    std::string module;
    uintptr_t address;
    std::vector<std::string> parameters;

    StackFrame() : address(0) {}
};

struct StackTrace {
    std::vector<StackFrame> frames;
    std::string error_message;
    std::string exception_type;
    int signal_number;

    StackTrace() : signal_number(0) {}
};

class StackTraceParser {
public:
    StackTraceParser();
    ~StackTraceParser();

    std::optional<StackTrace> parse(const std::string& trace_text);

    std::optional<StackTrace> parseGDB(const std::string& gdb_output);
    std::optional<StackTrace> parseLLDB(const std::string& lldb_output);
    std::optional<StackTrace> parseBacktrace(const std::string& bt_output);
    std::optional<StackTrace> parseMSVC(const std::string& msvc_output);

    void setVerbose(bool verbose);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    bool parseGDBFrame(const std::string& line, StackFrame& frame);
    bool parseLLDBFrame(const std::string& line, StackFrame& frame);
    std::string demangle(const std::string& mangled);
    std::optional<SourceLocation> extractLocation(const std::string& text);
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_STACK_TRACE_PARSER_H
