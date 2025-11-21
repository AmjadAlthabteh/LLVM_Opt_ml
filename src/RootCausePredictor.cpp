#include "ai_debugger/RootCausePredictor.h"
#include <algorithm>
#include <sstream>
#include <map>

namespace ai_debugger {

const char* bugCategoryToString(BugCategory category) {
    switch (category) {
        case BugCategory::MEMORY_ERROR: return "Memory Error";
        case BugCategory::NULL_POINTER: return "Null Pointer Dereference";
        case BugCategory::BUFFER_OVERFLOW: return "Buffer Overflow";
        case BugCategory::USE_AFTER_FREE: return "Use After Free";
        case BugCategory::DOUBLE_FREE: return "Double Free";
        case BugCategory::MEMORY_LEAK: return "Memory Leak";
        case BugCategory::RACE_CONDITION: return "Race Condition";
        case BugCategory::DEADLOCK: return "Deadlock";
        case BugCategory::LOGIC_ERROR: return "Logic Error";
        case BugCategory::TYPE_ERROR: return "Type Error";
        case BugCategory::ARITHMETIC_ERROR: return "Arithmetic Error";
        case BugCategory::RESOURCE_EXHAUSTION: return "Resource Exhaustion";
        case BugCategory::ASSERTION_FAILURE: return "Assertion Failure";
        default: return "Unknown";
    }
}

BugCategory stringToBugCategory(const std::string& str) {
    static std::map<std::string, BugCategory> mapping = {
        {"Memory Error", BugCategory::MEMORY_ERROR},
        {"Null Pointer Dereference", BugCategory::NULL_POINTER},
        {"Buffer Overflow", BugCategory::BUFFER_OVERFLOW},
        {"Use After Free", BugCategory::USE_AFTER_FREE},
        {"Double Free", BugCategory::DOUBLE_FREE},
        {"Memory Leak", BugCategory::MEMORY_LEAK},
        {"Race Condition", BugCategory::RACE_CONDITION},
        {"Deadlock", BugCategory::DEADLOCK},
        {"Logic Error", BugCategory::LOGIC_ERROR},
        {"Type Error", BugCategory::TYPE_ERROR},
        {"Arithmetic Error", BugCategory::ARITHMETIC_ERROR},
        {"Resource Exhaustion", BugCategory::RESOURCE_EXHAUSTION},
        {"Assertion Failure", BugCategory::ASSERTION_FAILURE}
    };

    auto it = mapping.find(str);
    return (it != mapping.end()) ? it->second : BugCategory::UNKNOWN;
}

struct RootCausePredictor::Impl {
    std::map<std::string, std::vector<RootCause>> knowledge_base;
};

RootCausePredictor::RootCausePredictor() : impl_(std::make_unique<Impl>()) {}

RootCausePredictor::~RootCausePredictor() = default;

std::vector<RootCause> RootCausePredictor::predict(
    const StackTrace& trace,
    const CallGraphAnalyzer& graph_analyzer
) {
    PredictionFeatures features = extractFeatures(trace, graph_analyzer);

    std::vector<RootCause> causes;

    auto heuristic_causes = applyHeuristics(features);
    causes.insert(causes.end(), heuristic_causes.begin(), heuristic_causes.end());

    auto pattern_causes = applyPatternMatching(trace);
    causes.insert(causes.end(), pattern_causes.begin(), pattern_causes.end());

    for (auto& cause : causes) {
        cause.confidence = calculateConfidence(cause, features);
    }

    rankCauses(causes);

    return causes;
}

RootCause RootCausePredictor::getMostLikelyCause(
    const StackTrace& trace,
    const CallGraphAnalyzer& graph_analyzer
) {
    auto causes = predict(trace, graph_analyzer);
    if (!causes.empty()) {
        return causes[0];
    }

    RootCause unknown;
    unknown.category = BugCategory::UNKNOWN;
    unknown.description = "Unable to determine root cause";
    unknown.confidence = 0.0;
    return unknown;
}

void RootCausePredictor::loadKnowledgeBase(const std::string& kb_path) {
}

void RootCausePredictor::trainFromExamples(
    const std::vector<std::pair<StackTrace, RootCause>>& examples
) {
    for (const auto& example : examples) {
        const auto& trace = example.first;
        const auto& cause = example.second;

        std::string key = trace.error_message;
        impl_->knowledge_base[key].push_back(cause);
    }
}

PredictionFeatures RootCausePredictor::extractFeatures(
    const StackTrace& trace,
    const CallGraphAnalyzer& graph
) {
    PredictionFeatures features;
    features.error_message = trace.error_message;
    features.stack_depth = static_cast<int>(trace.frames.size());

    for (const auto& frame : trace.frames) {
        features.function_names.push_back(frame.function_name);

        if (frame.function_name.find("malloc") != std::string::npos ||
            frame.function_name.find("new") != std::string::npos ||
            frame.function_name.find("alloc") != std::string::npos) {
            features.has_allocation = true;
        }

        if (frame.function_name.find("free") != std::string::npos ||
            frame.function_name.find("delete") != std::string::npos) {
            features.has_deallocation = true;
        }

        if (frame.function_name.find("thread") != std::string::npos ||
            frame.function_name.find("mutex") != std::string::npos ||
            frame.function_name.find("lock") != std::string::npos) {
            features.has_threading = true;
        }
    }

    return features;
}

std::vector<RootCause> RootCausePredictor::applyHeuristics(const PredictionFeatures& features) {
    std::vector<RootCause> causes;

    std::string error_lower = features.error_message;
    std::transform(error_lower.begin(), error_lower.end(), error_lower.begin(), ::tolower);

    if (error_lower.find("segmentation fault") != std::string::npos ||
        error_lower.find("sigsegv") != std::string::npos) {

        if (error_lower.find("null") != std::string::npos ||
            error_lower.find("0x0") != std::string::npos) {
            RootCause cause;
            cause.category = BugCategory::NULL_POINTER;
            cause.description = "Null pointer dereference detected from segmentation fault";
            cause.confidence = 0.85;
            causes.push_back(cause);
        } else {
            RootCause cause;
            cause.category = BugCategory::MEMORY_ERROR;
            cause.description = "Invalid memory access causing segmentation fault";
            cause.confidence = 0.75;
            causes.push_back(cause);
        }
    }

    if (error_lower.find("double free") != std::string::npos) {
        RootCause cause;
        cause.category = BugCategory::DOUBLE_FREE;
        cause.description = "Double free corruption detected";
        cause.confidence = 0.95;
        causes.push_back(cause);
    }

    if (error_lower.find("heap") != std::string::npos &&
        error_lower.find("corruption") != std::string::npos) {
        RootCause cause;
        cause.category = BugCategory::BUFFER_OVERFLOW;
        cause.description = "Heap corruption likely due to buffer overflow";
        cause.confidence = 0.80;
        causes.push_back(cause);
    }

    if (features.has_threading) {
        RootCause cause;
        cause.category = BugCategory::RACE_CONDITION;
        cause.description = "Possible race condition in multithreaded code";
        cause.confidence = 0.60;
        causes.push_back(cause);
    }

    if (error_lower.find("assertion") != std::string::npos ||
        error_lower.find("assert") != std::string::npos) {
        RootCause cause;
        cause.category = BugCategory::ASSERTION_FAILURE;
        cause.description = "Assertion failed indicating logic error";
        cause.confidence = 0.90;
        causes.push_back(cause);
    }

    return causes;
}

std::vector<RootCause> RootCausePredictor::applyPatternMatching(const StackTrace& trace) {
    std::vector<RootCause> causes;

    bool has_dealloc_in_trace = false;
    int dealloc_index = -1;

    for (size_t i = 0; i < trace.frames.size(); ++i) {
        const auto& frame = trace.frames[i];
        if (frame.function_name.find("free") != std::string::npos ||
            frame.function_name.find("delete") != std::string::npos) {
            has_dealloc_in_trace = true;
            dealloc_index = static_cast<int>(i);
            break;
        }
    }

    if (has_dealloc_in_trace && dealloc_index >= 0) {
        RootCause cause;
        cause.category = BugCategory::USE_AFTER_FREE;
        cause.description = "Potential use-after-free: memory accessed after deallocation";
        cause.confidence = 0.70;
        if (dealloc_index < static_cast<int>(trace.frames.size())) {
            cause.location = trace.frames[dealloc_index].location;
        }
        causes.push_back(cause);
    }

    return causes;
}

std::vector<RootCause> RootCausePredictor::applyMLModel(const PredictionFeatures& features) {
    return {};
}

double RootCausePredictor::calculateConfidence(
    const RootCause& cause,
    const PredictionFeatures& features
) {
    double confidence = cause.confidence;

    if (features.stack_depth > 10) {
        confidence *= 0.9;
    }

    if (!features.error_message.empty()) {
        confidence *= 1.1;
    }

    return std::min(confidence, 1.0);
}

void RootCausePredictor::rankCauses(std::vector<RootCause>& causes) {
    std::sort(causes.begin(), causes.end(), [](const RootCause& a, const RootCause& b) {
        return a.confidence > b.confidence;
    });
}

} // namespace ai_debugger
