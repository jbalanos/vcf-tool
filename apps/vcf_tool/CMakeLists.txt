# Find CLI11 for command-line parsing
find_package(CLI11 CONFIG REQUIRED)

add_executable(vcf_tool main.cpp)

target_link_libraries(vcf_tool
    PRIVATE
        vcf_tool_core
        vcf_tool_domain
        vcf_tool_utils
        project_warnings
        CLI11::CLI11
)

# C++ standard inherited from root CMakeLists.txt
