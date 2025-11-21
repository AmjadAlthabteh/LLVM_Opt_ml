#ifndef AI_DEBUGGER_ROOT_CAUSE_PREDICTOR_H
#define AI_DEBUGGER_ROOT_CAUSE_PREDICTOR_H

#include "StackTraceParser.h"
#include "CallGraphAnalyzer.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ai_debugger {

enum class BugCategory {
    MEMORY_ERROR,
    NULL_POINTER,
    BUFFER_OVERFLOW,
    USE_AFTER_FREE,
    DOUBLE_FREE,
    MEMORY_LEAK,
    RACE_CONDITION,
    DEADLOCK,
    LOGIC_ERROR,
    TYPE_ERROR,
    ARITHMETIC_ERROR,
    RESOURCE_EXHAUSTION,
    ASSERTION_FAILURE,
    UNKNOWN
};

struct RootCause {
    BugCategory category;
    std::string description;
    SourceLocation location;
    double confidence;
    std::vector<std::string> contributing_factors;
    std::vector<StackFrame> relevant_frames;

    RootCause() : category(BugCategory::UNKNOWN), confidence(0.0) {}
};

struct PredictionFeatures {
    std::string error_message;
    std::vector<std::string> function_names;
    std::vector<std::string> variable_patterns;
    bool has_allocation;
    bool has_deallocation;
    bool has_pointer_arithmetic;
    bool has_threading;
    int stack_depth;

    PredictionFeatures()
        : has_allocation(false)
        , has_deallocation(false)
        , has_pointer_arithmetic(false)
        , has_threading(false)
        , stack_depth(0) {}
};

class RootCausePredictor {
public:
    RootCausePredictor();
    ~RootCausePredictor();

    std::vector<RootCause> predict(
        const StackTrace& trace,
        const CallGraphAnalyzer& graph_analyzer
    );

    RootCause getMostLikelyCause(
        const StackTrace& trace,
        const CallGraphAnalyzer& graph_analyzer
    );

    void loadKnowledgeBase(const std::string& kb_path);
    void trainFromExamples(const std::vector<std::pair<StackTrace, RootCause>>& examples);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    PredictionFeatures extractFeatures(const StackTrace& trace, const CallGraphAnalyzer& graph);

    std::vector<RootCause> applyHeuristics(const PredictionFeatures& features);
    std::vector<RootCause> applyPatternMatching(const StackTrace& trace);
    std::vector<RootCause> applyMLModel(const PredictionFeatures& features);

    double calculateConfidence(const RootCause& cause, const PredictionFeatures& features);
    void rankCauses(std::vector<RootCause>& causes);
};

const char* bugCategoryToString(BugCategory category);
BugCategory stringToBugCategory(const std::string& str);

} // namespace ai_debugger

#endif // AI_DEBUGGER_ROOT_CAUSE_PREDICTOR_H
