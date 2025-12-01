# This will define the `set_project_warnings` function
# that takes the interface name.
function(set_project_warnings project_name)
  # I always treat warnings as errors, making my life easier
  # in the long run.
  option(
    WARNINGS_AS_ERRORS
    "Treat compiler warnings as errors" TRUE)

  set(MSVC_WARNINGS
    # Baseline reasonable warnings
    /W4
    # Warn on shadowed variables
    /w14242
    # Warn on useless casts
    /w14263
    /w14265
    # Warn on possible performance issues
    /w14287
    /w14296
    /w14311
    /w14545
    /w14546
    /w14547
    /w14549
    /w14555
    /w14619
    /w14640
    /w14826
    /w14905
    /w14906
    /w14928
  )

  set(CLANG_WARNINGS
    # Reasonable and standard
    -Wall
    -Wextra
    -Wpedantic
    # Warn on shadowed variables
    -Wshadow
    # Warn on implicit conversions that may lose data
    -Wconversion
    -Wsign-conversion
    # Warn on unused values
    -Wunused
    # Warn on casts that discard qualifiers
    -Wcast-qual
    # Warn on old-style casts
    -Wold-style-cast
    # Warn on non-virtual destructors
    -Wnon-virtual-dtor
    # Warn on overloaded virtual functions
    -Woverloaded-virtual
  )

  # Stop the build if there are any warnings
  if(WARNINGS_AS_ERRORS)
    message(STATUS "Warnings are treated as errors")
    set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
    set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
  endif()

  set(GCC_WARNINGS
    ${CLANG_WARNINGS}
    # Additional GCC-specific warnings
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wuseless-cast
  )

  # Enable the right set of warnings depending on
  # the used compiler.
  if(MSVC)
    set(PROJECT_WARNINGS ${MSVC_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})
  else()
    message(
      AUTHOR_WARNING
      "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
  endif()

  # This will "link" the warnings to the defined
  # project name, in my case "project_warnings".
  target_compile_options(
    ${project_name} INTERFACE ${PROJECT_WARNINGS})
endfunction()
