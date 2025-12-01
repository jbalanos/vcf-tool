# Add source files
file(GLOB_RECURSE DOMAIN_SOURCES "src/*.cpp")
add_library(vcf_tool_domain ${DOMAIN_SOURCES})

target_include_directories(vcf_tool_domain
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(vcf_tool_domain PUBLIC cxx_std_17)

# Link dependencies
target_link_libraries(vcf_tool_domain
    PUBLIC
        vcf_tool_core
        project_warnings
)
