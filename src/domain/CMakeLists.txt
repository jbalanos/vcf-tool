# Find required packages
find_package(concurrentqueue CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

# Add source files
file(GLOB_RECURSE DOMAIN_SOURCES "src/*.cpp")
add_library(vcf_tool_domain ${DOMAIN_SOURCES})

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
        vcf_tool_utils
        concurrentqueue::concurrentqueue
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
    PUBLIC
        project_warnings
)
