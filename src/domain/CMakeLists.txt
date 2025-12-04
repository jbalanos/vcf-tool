# Add source files
file(GLOB_RECURSE DOMAIN_SOURCES "src/*.cpp")
add_library(vcf_tool_domain ${DOMAIN_SOURCES})

# Find required packages
find_package(concurrentqueue CONFIG REQUIRED)

target_include_directories(vcf_tool_domain
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# C++ standard inherited from root CMakeLists.txt

# Link dependencies
target_link_libraries(vcf_tool_domain
    PRIVATE
        vcf_tool_core
        concurrentqueue::concurrentqueue
    PUBLIC
        project_warnings
)
