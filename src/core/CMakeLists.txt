# Core depends on utils for generic infrastructure
# Core adds app-specific abstractions on top of utils

# Add source files (ThreadPool has implementation)
file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
add_library(vcf_tool_core ${CORE_SOURCES})

target_include_directories(vcf_tool_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# C++ standard inherited from root CMakeLists.txt

target_link_libraries(vcf_tool_core
    PRIVATE
        vcf_tool_utils
    PUBLIC
        project_warnings
)
