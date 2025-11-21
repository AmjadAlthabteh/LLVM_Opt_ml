#ifndef AI_DEBUGGER_CALL_GRAPH_ANALYZER_H
#define AI_DEBUGGER_CALL_GRAPH_ANALYZER_H

#include "StackTraceParser.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace ai_debugger {

struct CallGraphNode {
    std::string function_name;
    SourceLocation location;
    std::vector<std::string> callers;
    std::vector<std::string> callees;
    int depth;
    bool is_library_function;

    CallGraphNode() : depth(0), is_library_function(false) {}
};

struct CallPattern {
    std::string pattern_type;
    std::vector<std::string> functions;
    double confidence;
    std::string description;

    CallPattern() : confidence(0.0) {}
};

class CallGraphAnalyzer {
public:
    CallGraphAnalyzer();
    ~CallGraphAnalyzer();

    void buildFromStackTrace(const StackTrace& trace);
    void buildFromSource(const std::string& source_path);

    std::vector<CallGraphNode> getNodes() const;
    std::optional<CallGraphNode> getNode(const std::string& function) const;

    std::vector<CallPattern> detectPatterns() const;
    std::vector<std::string> findCriticalPath() const;

    int getCallDepth(const std::string& function) const;
    bool isRecursive(const std::string& function) const;
    std::vector<std::string> getRecursionChain(const std::string& function) const;

    std::string getIntentSummary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    void analyzeRecursion();
    void detectCommonPatterns();
    void classifyFunctions();
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_CALL_GRAPH_ANALYZER_H
