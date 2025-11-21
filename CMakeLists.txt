cmake_minimum_required(VERSION 3.15)
project(AIDebugger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_LLVM "Enable LLVM integration for advanced analysis" OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(AI_DEBUGGER_HEADERS
    include/ai_debugger/StackTraceParser.h
    include/ai_debugger/CallGraphAnalyzer.h
    include/ai_debugger/RootCausePredictor.h
    include/ai_debugger/ExplanationGenerator.h
    include/ai_debugger/FixSuggester.h
    include/ai_debugger/TestGenerator.h
    include/ai_debugger/AIDebugger.h
)

set(AI_DEBUGGER_SOURCES
    src/StackTraceParser.cpp
    src/CallGraphAnalyzer.cpp
    src/RootCausePredictor.cpp
    src/ExplanationGenerator.cpp
    src/FixSuggester.cpp
    src/TestGenerator.cpp
    src/AIDebugger.cpp
)

add_library(ai_debugger STATIC
    ${AI_DEBUGGER_SOURCES}
    ${AI_DEBUGGER_HEADERS}
)

target_include_directories(ai_debugger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ai_debugger PUBLIC cxx_std_17)

if(ENABLE_LLVM)
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})

    llvm_map_components_to_libnames(llvm_libs support core irreader)
    target_link_libraries(ai_debugger PUBLIC ${llvm_libs})
    target_compile_definitions(ai_debugger PUBLIC ENABLE_LLVM)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(GTest_FOUND)
        add_subdirectory(tests)
    else()
        message(WARNING "GTest not found, tests will not be built")
    endif()
endif()

install(TARGETS ai_debugger
    EXPORT AIDebuggerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ai_debugger
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT AIDebuggerTargets
    FILE AIDebuggerTargets.cmake
    NAMESPACE AIDebugger::
    DESTINATION lib/cmake/AIDebugger
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AIDebuggerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AIDebuggerConfigVersion.cmake"
    DESTINATION lib/cmake/AIDebugger
)

add_custom_target(format
    COMMAND clang-format -i ${AI_DEBUGGER_SOURCES} ${AI_DEBUGGER_HEADERS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format"
)

message(STATUS "AI Debugger Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Enable LLVM: ${ENABLE_LLVM}")
