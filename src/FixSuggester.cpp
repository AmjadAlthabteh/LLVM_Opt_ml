#include "ai_debugger/FixSuggester.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>

namespace ai_debugger {

const char* fixTypeToString(FixType type) {
    switch (type) {
        case FixType::NULL_CHECK: return "Null Check";
        case FixType::BOUNDS_CHECK: return "Bounds Check";
        case FixType::INITIALIZATION: return "Initialization";
        case FixType::MEMORY_CLEANUP: return "Memory Cleanup";
        case FixType::SYNCHRONIZATION: return "Synchronization";
        case FixType::REFACTORING: return "Refactoring";
        case FixType::ASSERTION: return "Assertion";
        case FixType::ERROR_HANDLING: return "Error Handling";
        default: return "Custom";
    }
}

struct FixSuggester::Impl {
    std::string source_root;
    bool dry_run = false;
};

FixSuggester::FixSuggester() : impl_(std::make_unique<Impl>()) {}

FixSuggester::~FixSuggester() = default;

void FixSuggester::setSourceRoot(const std::string& root_path) {
    impl_->source_root = root_path;
}

void FixSuggester::setDryRun(bool dry_run) {
    impl_->dry_run = dry_run;
}

std::vector<CodeFix> FixSuggester::suggestFixes(
    const RootCause& root_cause,
    const StackTrace& trace
) {
    std::vector<CodeFix> fixes;

    switch (root_cause.category) {
        case BugCategory::NULL_POINTER:
            fixes = generateNullCheckFixes(root_cause);
            break;

        case BugCategory::BUFFER_OVERFLOW:
            fixes = generateBoundsCheckFixes(root_cause);
            break;

        case BugCategory::USE_AFTER_FREE:
        case BugCategory::DOUBLE_FREE:
        case BugCategory::MEMORY_LEAK:
            fixes = generateMemoryFixes(root_cause);
            break;

        case BugCategory::RACE_CONDITION:
        case BugCategory::DEADLOCK:
            fixes = generateConcurrencyFixes(root_cause);
            break;

        default:
            break;
    }

    for (auto& fix : fixes) {
        fix.location = root_cause.location;
    }

    std::sort(fixes.begin(), fixes.end(), [](const CodeFix& a, const CodeFix& b) {
        return a.confidence > b.confidence;
    });

    return fixes;
}

CodeFix FixSuggester::getBestFix(
    const RootCause& root_cause,
    const StackTrace& trace
) {
    auto fixes = suggestFixes(root_cause, trace);
    if (!fixes.empty()) {
        return fixes[0];
    }
    return CodeFix();
}

FixApplication FixSuggester::applyFix(const CodeFix& fix, bool create_backup) {
    FixApplication application;

    if (impl_->dry_run) {
        application.success = true;
        application.message = "[DRY RUN] Would apply fix to " + fix.location.file;
        return application;
    }

    if (!validateFix(fix)) {
        application.success = false;
        application.message = "Fix validation failed";
        return application;
    }

    std::string full_path = impl_->source_root.empty() ?
        fix.location.file :
        impl_->source_root + "/" + fix.location.file;

    try {
        if (create_backup) {
            std::string backup_path = createBackup(full_path);
            application.backup_files.push_back(backup_path);
        }

        std::string content = readSourceFile(full_path);
        if (content.empty()) {
            application.success = false;
            application.message = "Failed to read source file";
            return application;
        }

        std::string fixed_code = generateFixedCode(fix);

        if (writeSourceFile(full_path, fixed_code)) {
            application.success = true;
            application.message = "Successfully applied fix";
            application.modified_files.push_back(full_path);
        } else {
            application.success = false;
            application.message = "Failed to write fixed code";
        }

    } catch (const std::exception& e) {
        application.success = false;
        application.message = std::string("Exception: ") + e.what();
    }

    return application;
}

std::vector<FixApplication> FixSuggester::applyAllFixes(
    const std::vector<CodeFix>& fixes,
    bool create_backup
) {
    std::vector<FixApplication> applications;

    for (const auto& fix : fixes) {
        applications.push_back(applyFix(fix, create_backup));
    }

    return applications;
}

void FixSuggester::rollbackFix(const FixApplication& application) {
    if (!application.backup_files.empty()) {
        for (size_t i = 0; i < application.modified_files.size(); ++i) {
            if (i < application.backup_files.size()) {
                std::filesystem::copy_file(
                    application.backup_files[i],
                    application.modified_files[i],
                    std::filesystem::copy_options::overwrite_existing
                );
            }
        }
    }
}

std::vector<CodeFix> FixSuggester::generateNullCheckFixes(const RootCause& cause) {
    std::vector<CodeFix> fixes;

    CodeFix fix1;
    fix1.type = FixType::NULL_CHECK;
    fix1.description = "Add null pointer check before dereferencing";
    fix1.confidence = 0.90;
    fix1.original_code = "ptr->member";
    fix1.fixed_code = "if (ptr != nullptr) {\n    ptr->member;\n} else {\n    // Handle null case\n}";
    fixes.push_back(fix1);

    CodeFix fix2;
    fix2.type = FixType::INITIALIZATION;
    fix2.description = "Initialize pointer to nullptr";
    fix2.confidence = 0.85;
    fix2.original_code = "Type* ptr;";
    fix2.fixed_code = "Type* ptr = nullptr;";
    fixes.push_back(fix2);

    CodeFix fix3;
    fix3.type = FixType::ASSERTION;
    fix3.description = "Add assertion to catch null early";
    fix3.confidence = 0.75;
    fix3.original_code = "ptr->member";
    fix3.fixed_code = "assert(ptr != nullptr);\nptr->member;";
    fixes.push_back(fix3);

    return fixes;
}

std::vector<CodeFix> FixSuggester::generateBoundsCheckFixes(const RootCause& cause) {
    std::vector<CodeFix> fixes;

    CodeFix fix1;
    fix1.type = FixType::BOUNDS_CHECK;
    fix1.description = "Add bounds checking before array access";
    fix1.confidence = 0.88;
    fix1.original_code = "array[index]";
    fix1.fixed_code = "if (index >= 0 && index < array_size) {\n    array[index];\n}";
    fixes.push_back(fix1);

    CodeFix fix2;
    fix2.type = FixType::REFACTORING;
    fix2.description = "Replace raw array with std::vector and use at()";
    fix2.confidence = 0.92;
    fix2.original_code = "int array[100];\narray[index] = value;";
    fix2.fixed_code = "std::vector<int> array(100);\narray.at(index) = value;";
    fixes.push_back(fix2);

    return fixes;
}

std::vector<CodeFix> FixSuggester::generateMemoryFixes(const RootCause& cause) {
    std::vector<CodeFix> fixes;

    if (cause.category == BugCategory::USE_AFTER_FREE) {
        CodeFix fix1;
        fix1.type = FixType::MEMORY_CLEANUP;
        fix1.description = "Set pointer to nullptr after delete";
        fix1.confidence = 0.87;
        fix1.original_code = "delete ptr;";
        fix1.fixed_code = "delete ptr;\nptr = nullptr;";
        fixes.push_back(fix1);

        CodeFix fix2;
        fix2.type = FixType::REFACTORING;
        fix2.description = "Replace raw pointer with std::unique_ptr";
        fix2.confidence = 0.95;
        fix2.original_code = "Type* ptr = new Type();\ndelete ptr;";
        fix2.fixed_code = "std::unique_ptr<Type> ptr = std::make_unique<Type>();\n// Automatic cleanup";
        fixes.push_back(fix2);
    }

    if (cause.category == BugCategory::DOUBLE_FREE) {
        CodeFix fix;
        fix.type = FixType::NULL_CHECK;
        fix.description = "Check pointer before deleting";
        fix.confidence = 0.85;
        fix.original_code = "delete ptr;";
        fix.fixed_code = "if (ptr != nullptr) {\n    delete ptr;\n    ptr = nullptr;\n}";
        fixes.push_back(fix);
    }

    if (cause.category == BugCategory::MEMORY_LEAK) {
        CodeFix fix;
        fix.type = FixType::MEMORY_CLEANUP;
        fix.description = "Add missing delete statement";
        fix.confidence = 0.80;
        fix.original_code = "Type* ptr = new Type();";
        fix.fixed_code = "Type* ptr = new Type();\n// ... use ptr ...\ndelete ptr;";
        fixes.push_back(fix);
    }

    return fixes;
}

std::vector<CodeFix> FixSuggester::generateConcurrencyFixes(const RootCause& cause) {
    std::vector<CodeFix> fixes;

    CodeFix fix1;
    fix1.type = FixType::SYNCHRONIZATION;
    fix1.description = "Protect shared data with mutex";
    fix1.confidence = 0.85;
    fix1.original_code = "shared_data++;";
    fix1.fixed_code = "{\n    std::lock_guard<std::mutex> lock(mtx);\n    shared_data++;\n}";
    fixes.push_back(fix1);

    CodeFix fix2;
    fix2.type = FixType::REFACTORING;
    fix2.description = "Use atomic variable for simple counter";
    fix2.confidence = 0.90;
    fix2.original_code = "int counter;";
    fix2.fixed_code = "std::atomic<int> counter;";
    fixes.push_back(fix2);

    return fixes;
}

std::string FixSuggester::readSourceFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool FixSuggester::writeSourceFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    file << content;
    return true;
}

std::string FixSuggester::createBackup(const std::string& path) {
    std::time_t now = std::time(nullptr);
    std::string backup_path = path + ".backup." + std::to_string(now);

    std::filesystem::copy_file(path, backup_path);
    return backup_path;
}

std::string FixSuggester::generateFixedCode(const CodeFix& fix) {
    return fix.fixed_code;
}

bool FixSuggester::validateFix(const CodeFix& fix) {
    if (fix.location.file.empty()) {
        return false;
    }

    if (fix.fixed_code.empty()) {
        return false;
    }

    return true;
}

} // namespace ai_debugger
