# Find Catch2 for unit testing
find_package(Catch2 CONFIG REQUIRED)

# Core tests
add_executable(test_core
    test_logger.cpp
    test_format.cpp
    test_json.cpp
    test_config.cpp
)

target_link_libraries(test_core
    PRIVATE
        vcf_tool_core
        Catch2::Catch2WithMain
        project_warnings
)

add_test(NAME test_core COMMAND test_core)

# Domain tests
add_executable(test_domain
    test_greeting.cpp
)

target_link_libraries(test_domain
    PRIVATE
        vcf_tool_core
        vcf_tool_domain
        Catch2::Catch2WithMain
        project_warnings
)

add_test(NAME test_domain COMMAND test_domain)
