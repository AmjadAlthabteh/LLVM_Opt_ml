#include "ai_debugger/CallGraphAnalyzer.h"
#include <algorithm>
#include <queue>
#include <set>
#include <sstream>

namespace ai_debugger {

struct CallGraphAnalyzer::Impl {
    std::map<std::string, CallGraphNode> nodes;
    std::vector<CallPattern> patterns;
    std::string intent_summary;

    void addEdge(const std::string& from, const std::string& to) {
        if (nodes.find(from) == nodes.end()) {
            nodes[from] = CallGraphNode();
            nodes[from].function_name = from;
        }
        if (nodes.find(to) == nodes.end()) {
            nodes[to] = CallGraphNode();
            nodes[to].function_name = to;
        }

        nodes[from].callees.push_back(to);
        nodes[to].callers.push_back(from);
    }

    bool hasPath(const std::string& from, const std::string& to, std::set<std::string>& visited) {
        if (from == to) return true;
        if (visited.count(from)) return false;

        visited.insert(from);

        auto it = nodes.find(from);
        if (it == nodes.end()) return false;

        for (const auto& callee : it->second.callees) {
            if (hasPath(callee, to, visited)) {
                return true;
            }
        }

        return false;
    }
};

CallGraphAnalyzer::CallGraphAnalyzer() : impl_(std::make_unique<Impl>()) {}

CallGraphAnalyzer::~CallGraphAnalyzer() = default;

void CallGraphAnalyzer::buildFromStackTrace(const StackTrace& trace) {
    for (size_t i = 0; i < trace.frames.size(); ++i) {
        const auto& frame = trace.frames[i];

        CallGraphNode node;
        node.function_name = frame.function_name;
        node.location = frame.location;
        node.depth = static_cast<int>(i);

        node.is_library_function =
            frame.function_name.find("std::") == 0 ||
            frame.function_name.find("__") == 0 ||
            frame.location.file.find("/usr/") == 0 ||
            frame.location.file.find("C:\\Program Files") == 0;

        impl_->nodes[frame.function_name] = node;

        if (i + 1 < trace.frames.size()) {
            const auto& caller_frame = trace.frames[i + 1];
            impl_->addEdge(caller_frame.function_name, frame.function_name);
        }
    }

    analyzeRecursion();
    detectCommonPatterns();
    classifyFunctions();
}

void CallGraphAnalyzer::buildFromSource(const std::string& source_path) {
    impl_->intent_summary = "Source analysis requires LLVM/Clang integration";
}

std::vector<CallGraphNode> CallGraphAnalyzer::getNodes() const {
    std::vector<CallGraphNode> result;
    for (const auto& pair : impl_->nodes) {
        result.push_back(pair.second);
    }
    return result;
}

std::optional<CallGraphNode> CallGraphAnalyzer::getNode(const std::string& function) const {
    auto it = impl_->nodes.find(function);
    if (it != impl_->nodes.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<CallPattern> CallGraphAnalyzer::detectPatterns() const {
    return impl_->patterns;
}

std::vector<std::string> CallGraphAnalyzer::findCriticalPath() const {
    std::vector<std::string> path;
    for (const auto& pair : impl_->nodes) {
        if (!pair.second.is_library_function) {
            path.push_back(pair.first);
        }
    }
    return path;
}

int CallGraphAnalyzer::getCallDepth(const std::string& function) const {
    auto it = impl_->nodes.find(function);
    if (it != impl_->nodes.end()) {
        return it->second.depth;
    }
    return -1;
}

bool CallGraphAnalyzer::isRecursive(const std::string& function) const {
    std::set<std::string> visited;
    auto it = impl_->nodes.find(function);
    if (it == impl_->nodes.end()) return false;

    for (const auto& callee : it->second.callees) {
        visited.clear();
        if (impl_->hasPath(callee, function, visited)) {
            return true;
        }
    }

    return false;
}

std::vector<std::string> CallGraphAnalyzer::getRecursionChain(const std::string& function) const {
    std::vector<std::string> chain;

    if (!isRecursive(function)) {
        return chain;
    }

    std::set<std::string> visited;
    std::queue<std::vector<std::string>> paths;
    paths.push({function});

    while (!paths.empty()) {
        auto current_path = paths.front();
        paths.pop();

        std::string current = current_path.back();

        if (current_path.size() > 1 && current == function) {
            return current_path;
        }

        if (visited.count(current)) continue;
        visited.insert(current);

        auto it = impl_->nodes.find(current);
        if (it != impl_->nodes.end()) {
            for (const auto& callee : it->second.callees) {
                auto new_path = current_path;
                new_path.push_back(callee);
                paths.push(new_path);
            }
        }
    }

    return chain;
}

std::string CallGraphAnalyzer::getIntentSummary() const {
    if (!impl_->intent_summary.empty()) {
        return impl_->intent_summary;
    }

    std::ostringstream oss;
    oss << "Call graph contains " << impl_->nodes.size() << " functions. ";

    int user_functions = 0;
    for (const auto& pair : impl_->nodes) {
        if (!pair.second.is_library_function) {
            user_functions++;
        }
    }

    oss << user_functions << " user-defined functions involved in the error path.";

    return oss.str();
}

void CallGraphAnalyzer::analyzeRecursion() {
    for (const auto& pair : impl_->nodes) {
        if (isRecursive(pair.first)) {
            CallPattern pattern;
            pattern.pattern_type = "RECURSION";
            pattern.functions = getRecursionChain(pair.first);
            pattern.confidence = 0.9;
            pattern.description = "Recursive call pattern detected in " + pair.first;
            impl_->patterns.push_back(pattern);
        }
    }
}

void CallGraphAnalyzer::detectCommonPatterns() {
    std::set<std::string> alloc_functions = {"malloc", "calloc", "new", "new[]"};
    std::set<std::string> dealloc_functions = {"free", "delete", "delete[]"};

    bool has_alloc = false;
    bool has_dealloc = false;

    for (const auto& pair : impl_->nodes) {
        if (alloc_functions.count(pair.first)) has_alloc = true;
        if (dealloc_functions.count(pair.first)) has_dealloc = true;
    }

    if (has_alloc && !has_dealloc) {
        CallPattern pattern;
        pattern.pattern_type = "MEMORY_LEAK";
        pattern.confidence = 0.7;
        pattern.description = "Memory allocation without corresponding deallocation";
        impl_->patterns.push_back(pattern);
    }
}

void CallGraphAnalyzer::classifyFunctions() {
    for (auto& pair : impl_->nodes) {
        auto& node = pair.second;

        if (node.function_name.find("lock") != std::string::npos ||
            node.function_name.find("mutex") != std::string::npos) {
            CallPattern pattern;
            pattern.pattern_type = "SYNCHRONIZATION";
            pattern.functions.push_back(node.function_name);
            pattern.confidence = 0.8;
            pattern.description = "Synchronization primitive usage detected";
            impl_->patterns.push_back(pattern);
        }
    }
}

} // namespace ai_debugger
