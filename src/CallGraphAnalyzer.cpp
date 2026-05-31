#include "ai_debugger/CallGraphAnalyzer.h"
#include <algorithm>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace ai_debugger {

struct CallGraphAnalyzer::Impl {
    std::unordered_map<std::string, CallGraphNode> nodes;
    std::vector<CallPattern> patterns;
    std::string intent_summary;
    std::vector<std::string> node_order;
    std::unordered_map<std::string, int> first_depth;

    void addEdge(const std::string& from, const std::string& to) {
        auto [from_it, from_inserted] = nodes.try_emplace(from, CallGraphNode{});
        if (from_inserted) {
            from_it->second.function_name = from;
        }

        auto [to_it, to_inserted] = nodes.try_emplace(to, CallGraphNode{});
        if (to_inserted) {
            to_it->second.function_name = to;
        }

        from_it->second.callees.push_back(to);
        to_it->second.callers.push_back(from);
    }

    bool hasPath(const std::string& from, const std::string& to, std::unordered_set<std::string>& visited) {
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
    impl_->nodes.clear();
    impl_->patterns.clear();
    impl_->intent_summary.clear();
    impl_->node_order.clear();
    impl_->node_order.reserve(trace.frames.size());
    impl_->first_depth.clear();
    impl_->first_depth.reserve(trace.frames.size());

    std::unordered_set<std::string> seen_in_order;
    seen_in_order.reserve(trace.frames.size());

    for (size_t i = 0; i < trace.frames.size(); ++i) {
        const auto& frame = trace.frames[i];

        auto [node_it, inserted] = impl_->nodes.try_emplace(frame.function_name, CallGraphNode{});
        CallGraphNode& node = node_it->second;

        if (inserted) {
            node.function_name = frame.function_name;
        }

        auto [depth_it, depth_inserted] =
            impl_->first_depth.try_emplace(frame.function_name, static_cast<int>(i));
        (void)depth_inserted;
        node.depth = depth_it->second;

        if (node.location.file.empty() && !frame.location.file.empty()) {
            node.location = frame.location;
        }

        node.is_library_function =
            frame.function_name.find("std::") == 0 ||
            frame.function_name.find("__") == 0 ||
            frame.location.file.find("/usr/") == 0 ||
            frame.location.file.find("C:\\Program Files") == 0;

        if (seen_in_order.insert(frame.function_name).second) {
            impl_->node_order.push_back(frame.function_name);
        }

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
    result.reserve(impl_->nodes.size());

    std::unordered_set<std::string> emitted;
    emitted.reserve(impl_->nodes.size());

    for (const auto& name : impl_->node_order) {
        auto it = impl_->nodes.find(name);
        if (it != impl_->nodes.end() && emitted.insert(name).second) {
            result.push_back(it->second);
        }
    }

    for (const auto& pair : impl_->nodes) {
        if (emitted.insert(pair.first).second) {
            result.push_back(pair.second);
        }
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
    std::unordered_set<std::string> visited;
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

    std::unordered_set<std::string> visited;
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
    const std::unordered_set<std::string> alloc_functions = {"malloc", "calloc", "new", "new[]"};
    const std::unordered_set<std::string> dealloc_functions = {"free", "delete", "delete[]"};

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
