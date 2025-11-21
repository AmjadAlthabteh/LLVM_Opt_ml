#ifndef AI_DEBUGGER_EXPLANATION_GENERATOR_H
#define AI_DEBUGGER_EXPLANATION_GENERATOR_H

#include "StackTraceParser.h"
#include "CallGraphAnalyzer.h"
#include "RootCausePredictor.h"
#include <string>
#include <vector>
#include <memory>

namespace ai_debugger {

struct Explanation {
    std::string summary;
    std::string detailed_analysis;
    std::vector<std::string> step_by_step;
    std::string technical_details;
    std::string simplified_explanation;
    std::vector<std::string> relevant_code_snippets;

    std::string toMarkdown() const;
    std::string toPlainText() const;
    std::string toHTML() const;
};

class ExplanationGenerator {
public:
    ExplanationGenerator();
    ~ExplanationGenerator();

    Explanation generate(
        const StackTrace& trace,
        const RootCause& root_cause,
        const CallGraphAnalyzer& graph
    );

    std::string explainBugCategory(BugCategory category);
    std::string explainCallFlow(const std::vector<StackFrame>& frames);
    std::string suggestPreventionStrategies(const RootCause& cause);

    void setDetailLevel(int level);
    void setIncludeCodeSnippets(bool include);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    std::string generateSummary(const RootCause& cause);
    std::string generateDetailedAnalysis(const StackTrace& trace, const RootCause& cause);
    std::vector<std::string> generateStepByStep(const StackTrace& trace, const RootCause& cause);
    std::string generateSimplified(const RootCause& cause);

    std::string describeMemoryError(const RootCause& cause);
    std::string describeConcurrencyError(const RootCause& cause);
    std::string describeLogicError(const RootCause& cause);

    std::vector<std::string> extractRelevantCode(const std::vector<StackFrame>& frames);
};

} // namespace ai_debugger

#endif // AI_DEBUGGER_EXPLANATION_GENERATOR_H
