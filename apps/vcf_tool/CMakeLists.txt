add_executable(vcf_tool main.cpp)

target_link_libraries(vcf_tool
    PRIVATE
        vcf_tool_core
        vcf_tool_domain
        vcf_tool_utils
        project_warnings
)

target_compile_features(vcf_tool PRIVATE cxx_std_17)
