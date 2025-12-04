# Find third-party dependencies that utils wraps
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE UTIL_SOURCES "src/*.cpp")
add_library(vcf_tool_utils  ${UTIL_SOURCES})

target_include_directories(vcf_tool_utils
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# C++ standard inherited from root CMakeLists.txt

# Link dependencies
# Utils owns all third-party infrastructure libraries
target_link_libraries(vcf_tool_utils
    PUBLIC
        project_warnings
    PRIVATE
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# Add source files when you have implementation files
# file(GLOB_RECURSE UTILS_SOURCES "src/*.cpp")
# target_sources(vcf_tool_utils PRIVATE ${UTILS_SOURCES})
