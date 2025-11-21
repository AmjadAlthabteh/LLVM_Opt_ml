#include "ai_debugger/StackTraceParser.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cxxabi.h>

namespace ai_debugger {

struct StackTraceParser::Impl {
    bool verbose = false;

    std::regex gdb_frame_regex{
        R"(#(\d+)\s+(?:0x[0-9a-fA-F]+\s+in\s+)?([^\s]+)\s*\(([^\)]*)\)\s*(?:at\s+([^:]+):(\d+))?)"
    };

    std::regex lldb_frame_regex{
        R"(frame\s+#(\d+):\s+0x[0-9a-fA-F]+\s+([^\s`]+)`([^\s+]+)\s*(?:\+\s*\d+)?\s*(?:at\s+([^:]+):(\d+))?)"
    };

    std::regex addr2line_regex{
        R"(([^\s]+)\s+at\s+([^:]+):(\d+))"
    };

    std::regex msvc_regex{
        R"(([^\(]+)\((\d+)\):\s*(.*))"
    };
};

StackTraceParser::StackTraceParser() : impl_(std::make_unique<Impl>()) {}

StackTraceParser::~StackTraceParser() = default;

void StackTraceParser::setVerbose(bool verbose) {
    impl_->verbose = verbose;
}

std::optional<StackTrace> StackTraceParser::parse(const std::string& trace_text) {
    if (trace_text.find("frame #") != std::string::npos) {
        return parseLLDB(trace_text);
    } else if (trace_text.find("#0") != std::string::npos || trace_text.find("#1") != std::string::npos) {
        return parseGDB(trace_text);
    } else if (trace_text.find(".cpp(") != std::string::npos || trace_text.find(".h(") != std::string::npos) {
        return parseMSVC(trace_text);
    } else {
        return parseBacktrace(trace_text);
    }
}

std::optional<StackTrace> StackTraceParser::parseGDB(const std::string& gdb_output) {
    StackTrace trace;
    std::istringstream stream(gdb_output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("signal") != std::string::npos || line.find("SIGSEGV") != std::string::npos) {
            trace.error_message = line;
            if (line.find("SIGSEGV") != std::string::npos) {
                trace.signal_number = 11;
            } else if (line.find("SIGABRT") != std::string::npos) {
                trace.signal_number = 6;
            }
        }

        StackFrame frame;
        if (parseGDBFrame(line, frame)) {
            trace.frames.push_back(frame);
        }
    }

    if (trace.frames.empty()) {
        return std::nullopt;
    }

    return trace;
}

bool StackTraceParser::parseGDBFrame(const std::string& line, StackFrame& frame) {
    std::smatch match;
    if (std::regex_search(line, match, impl_->gdb_frame_regex)) {
        frame.mangled_name = match[2].str();
        frame.function_name = demangle(frame.mangled_name);

        if (match[4].matched) {
            frame.location.file = match[4].str();
            frame.location.line = std::stoi(match[5].str());
        }

        if (match[3].matched) {
            std::string params = match[3].str();
            std::istringstream param_stream(params);
            std::string param;
            while (std::getline(param_stream, param, ',')) {
                frame.parameters.push_back(param);
            }
        }

        return true;
    }
    return false;
}

std::optional<StackTrace> StackTraceParser::parseLLDB(const std::string& lldb_output) {
    StackTrace trace;
    std::istringstream stream(lldb_output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("stop reason") != std::string::npos) {
            trace.error_message = line;
        }

        StackFrame frame;
        if (parseLLDBFrame(line, frame)) {
            trace.frames.push_back(frame);
        }
    }

    if (trace.frames.empty()) {
        return std::nullopt;
    }

    return trace;
}

bool StackTraceParser::parseLLDBFrame(const std::string& line, StackFrame& frame) {
    std::smatch match;
    if (std::regex_search(line, match, impl_->lldb_frame_regex)) {
        frame.module = match[2].str();
        frame.mangled_name = match[3].str();
        frame.function_name = demangle(frame.mangled_name);

        if (match[4].matched) {
            frame.location.file = match[4].str();
            frame.location.line = std::stoi(match[5].str());
        }

        return true;
    }
    return false;
}

std::optional<StackTrace> StackTraceParser::parseBacktrace(const std::string& bt_output) {
    StackTrace trace;
    std::istringstream stream(bt_output);
    std::string line;

    int frame_index = 0;
    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, impl_->addr2line_regex)) {
            StackFrame frame;
            frame.function_name = demangle(match[1].str());
            frame.mangled_name = match[1].str();
            frame.location.file = match[2].str();
            frame.location.line = std::stoi(match[3].str());
            trace.frames.push_back(frame);
            frame_index++;
        }
    }

    if (trace.frames.empty()) {
        return std::nullopt;
    }

    return trace;
}

std::optional<StackTrace> StackTraceParser::parseMSVC(const std::string& msvc_output) {
    StackTrace trace;
    std::istringstream stream(msvc_output);
    std::string line;

    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, impl_->msvc_regex)) {
            StackFrame frame;
            frame.location.file = match[1].str();
            frame.location.line = std::stoi(match[2].str());
            frame.function_name = match[3].str();
            trace.frames.push_back(frame);
        }
    }

    if (trace.frames.empty()) {
        return std::nullopt;
    }

    return trace;
}

std::string StackTraceParser::demangle(const std::string& mangled) {
#ifdef __GNUG__
    int status = 0;
    char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
    if (status == 0 && demangled) {
        std::string result(demangled);
        free(demangled);
        return result;
    }
#endif
    return mangled;
}

std::optional<SourceLocation> StackTraceParser::extractLocation(const std::string& text) {
    std::regex loc_regex(R"(([^:]+):(\d+)(?::(\d+))?)");
    std::smatch match;
    if (std::regex_search(text, match, loc_regex)) {
        SourceLocation loc;
        loc.file = match[1].str();
        loc.line = std::stoi(match[2].str());
        if (match[3].matched) {
            loc.column = std::stoi(match[3].str());
        }
        return loc;
    }
    return std::nullopt;
}

} // namespace ai_debugger
