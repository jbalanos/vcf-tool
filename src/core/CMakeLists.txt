# Core depends on utils for generic infrastructure
# Core adds app-specific abstractions on top of utils

# Find MongoDB C++ driver
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

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
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
        project_warnings
)
