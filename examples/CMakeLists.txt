add_executable(simple_example simple_example.cpp)
target_link_libraries(simple_example PRIVATE ai_debugger)

add_executable(cli_tool cli_tool.cpp)
target_link_libraries(cli_tool PRIVATE ai_debugger)

install(TARGETS simple_example cli_tool
    RUNTIME DESTINATION bin/examples
)
