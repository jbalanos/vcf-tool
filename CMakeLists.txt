cmake_minimum_required(VERSION 3.15)
project(vcf_tool VERSION 0.1.0 LANGUAGES CXX)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include standard project settings
include(StandardProjectSettings)

# Include compiler warnings module
include(CompilerWarnings)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create interface library for project warnings
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Add subdirectories for libraries
add_subdirectory(src/core)
add_subdirectory(src/domain)
add_subdirectory(src/utils)

# Add applications
add_subdirectory(apps/vcf_tool)

# Add tests if enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()