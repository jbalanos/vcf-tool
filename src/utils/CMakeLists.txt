add_library(vcf_tool_utils INTERFACE)

target_include_directories(vcf_tool_utils
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(vcf_tool_utils INTERFACE cxx_std_17)

# Link dependencies
target_link_libraries(vcf_tool_utils
    INTERFACE
        vcf_tool_core
        project_warnings
)

# Add source files when you have implementation files
# file(GLOB_RECURSE UTILS_SOURCES "src/*.cpp")
# target_sources(vcf_tool_utils PRIVATE ${UTILS_SOURCES})
