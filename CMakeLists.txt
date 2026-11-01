cmake_minimum_required(VERSION 3.14)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS VERSION.txt)
file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION.txt" tomlplusplus_VERSION)
string(STRIP "${tomlplusplus_VERSION}" tomlplusplus_VERSION)

project(
    tomlplusplus
    VERSION "${tomlplusplus_VERSION}"
    DESCRIPTION "Header-only TOML config file parser and serializer for C++17"
    HOMEPAGE_URL "https://marzer.github.io/tomlplusplus/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Generated files ----

set(tomlplusplus_regenerate_default OFF)
if(PROJECT_IS_TOP_LEVEL AND EXISTS "${PROJECT_SOURCE_DIR}/.git" AND NOT BUILD_FUZZER)
  set(tomlplusplus_regenerate_default ON)
endif()
option(
    TOMLPLUSPLUS_REGENERATE
    "Keep toml.hpp and other derived files up to date when building from a git checkout"
    ${tomlplusplus_regenerate_default}
)

if(TOMLPLUSPLUS_REGENERATE)
  find_package(Python3 COMPONENTS Interpreter)
  if(Python3_Interpreter_FOUND)
    set(tomlplusplus_regenerate_command "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tools/regenerate.py")
    execute_process(COMMAND ${tomlplusplus_regenerate_command} RESULT_VARIABLE tomlplusplus_regenerate_result)
    if(tomlplusplus_regenerate_result)
      message(FATAL_ERROR "tools/regenerate.py failed: ${tomlplusplus_regenerate_result}")
    endif()
    add_custom_target(tomlplusplus_regenerate ALL COMMAND ${tomlplusplus_regenerate_command} VERBATIM)
  else()
    message(STATUS "Python 3 not found; toml.hpp and other derived files won't be kept up to date")
  endif()
endif()

# ---- Declare library ----

add_library(tomlplusplus_tomlplusplus INTERFACE)
add_library(tomlplusplus::tomlplusplus ALIAS tomlplusplus_tomlplusplus)

set_property(
    TARGET tomlplusplus_tomlplusplus PROPERTY
    EXPORT_NAME tomlplusplus
)

target_include_directories(
  tomlplusplus_tomlplusplus
  ${tomlplusplus_warning_guard} # unquoted for list expansion
  INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(tomlplusplus_tomlplusplus INTERFACE cxx_std_17)

# ---- Install rules ----
if (tomlplusplus_INSTALL)
  include(cmake/install-rules.cmake)
endif()

# ---- C++ Modules Support (optional) ----
option(TOMLPLUSPLUS_BUILD_MODULES "Build C++ modules support" OFF)

if(TOMLPLUSPLUS_BUILD_MODULES)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
    message(STATUS "Building C++ modules (CMake ${CMAKE_VERSION} supports modules)")
    add_subdirectory(src/modules)
  else()
    message(WARNING "Skipping C++ modules (requires CMake 3.28+, found ${CMAKE_VERSION})")
  endif()
else()
  message(STATUS "C++ modules support is disabled. Enable with -DTOMLPLUSPLUS_BUILD_MODULES=ON")
endif()

# ---- Examples and fuzzing ----
if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." OFF)
  option(BUILD_FUZZER "Build fuzzer." OFF)
  if(BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
  if(BUILD_FUZZER AND DEFINED ENV{LIB_FUZZING_ENGINE})
    add_subdirectory(fuzzing)
  endif()
endif()
