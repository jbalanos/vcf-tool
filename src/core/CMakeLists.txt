add_library(vcf_tool_core INTERFACE)

target_include_directories(vcf_tool_core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(vcf_tool_core INTERFACE cxx_std_17)

target_link_libraries(vcf_tool_core
    INTERFACE
        project_warnings
)

# Add source files when you have implementation files
# file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
# target_sources(vcf_tool_core PRIVATE ${CORE_SOURCES})
