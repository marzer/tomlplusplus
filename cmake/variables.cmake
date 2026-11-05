include(CMakeDependentOption)

# ---- Options ----

option(tomlplusplus_INSTALL "Enable generation of tomlplusplus install targets" ${PROJECT_IS_TOP_LEVEL})
option(tomlplusplus_BUILD_LIB "Compile the library, rather than consuming it in header-only mode" OFF)
option(TOMLPLUSPLUS_BUILD_MODULES "Build C++ modules support" OFF)
option(tomlplusplus_UNRELEASED_FEATURES "Enable TOML_ENABLE_UNRELEASED_FEATURES" OFF)

# ---- Developer options ----

macro(tomlplusplus_developer_option name description default)
  cmake_dependent_option(${name} "${description}" ${default} PROJECT_IS_TOP_LEVEL ${default})
endmacro()

tomlplusplus_developer_option(tomlplusplus_BUILD_TESTS "Build the tests" OFF)
tomlplusplus_developer_option(tomlplusplus_BUILD_TOML_TEST "Build the toml-test encoder and decoder" OFF)
tomlplusplus_developer_option(tomlplusplus_PEDANTIC "Enable as many compiler warnings as possible" OFF)
tomlplusplus_developer_option(tomlplusplus_WERROR "Treat compiler warnings as errors" OFF)
tomlplusplus_developer_option(tomlplusplus_ASAN "Build with AddressSanitizer" OFF)
tomlplusplus_developer_option(tomlplusplus_UBSAN "Build with UndefinedBehaviorSanitizer" OFF)
tomlplusplus_developer_option(tomlplusplus_COVERAGE "Build with Clang source-based coverage" OFF)
tomlplusplus_developer_option(tomlplusplus_EXCEPTIONS "Build with exceptions enabled" ON)
tomlplusplus_developer_option(tomlplusplus_PERMISSIVE "Build with /permissive instead of /permissive- (MSVC only)" OFF)
tomlplusplus_developer_option(tomlplusplus_USE_VENDORED_LIBS "Use the libraries in vendor/, rather than installed ones" ON)
tomlplusplus_developer_option(tomlplusplus_TESTS_SINGLE_HEADER "Test toml.hpp, rather than the multi-file headers" OFF)
tomlplusplus_developer_option(tomlplusplus_TESTS_LEAK "Check for leaks in the tests (a shared library hides most objects from the count)" OFF)

# ---- Warning guard ----

# target_include_directories with the SYSTEM modifier will request the compiler
# to omit warnings from the provided paths, if the compiler supports that
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(tomlplusplus_warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      tomlplusplus_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for tomlplusplus's includes, disabling warnings"
      ON
  )
  mark_as_advanced(tomlplusplus_INCLUDES_WITH_SYSTEM)
  if(tomlplusplus_INCLUDES_WITH_SYSTEM)
    set(tomlplusplus_warning_guard SYSTEM)
  endif()
endif()
