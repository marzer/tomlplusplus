# Utilized by OSSFuzz to build the harness(es) for continuous fuzz-testing
# OSSFuzz defines the following environment variables, that this target relies upon:
# CXX, CXXFLAGS, LIB_FUZZING_ENGINE, OUT
cmake_minimum_required(VERSION 3.14...4.4)

project(Fuzzer VERSION 1.0.0 LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)

if(PROJECT_IS_TOP_LEVEL)
    find_package(tomlplusplus REQUIRED)
endif()

add_executable(toml_fuzzer toml_fuzzer.cpp)
target_compile_definitions(toml_fuzzer PRIVATE NDEBUG) # Do not want assertions

add_executable(toml_float_fuzzer toml_float_fuzzer.cpp)
target_compile_definitions(toml_float_fuzzer PRIVATE NDEBUG)

set(fuzzers toml_fuzzer toml_float_fuzzer)

# a compiled toml++ has its own exception and feature settings
get_target_property(tomlplusplus_type tomlplusplus::tomlplusplus TYPE)
if(tomlplusplus_type STREQUAL "INTERFACE_LIBRARY")
    add_executable(toml_no_exceptions_fuzzer toml_fuzzer.cpp)
    target_compile_options(toml_no_exceptions_fuzzer PRIVATE -fno-exceptions -UNDEBUG)
    target_compile_definitions(toml_no_exceptions_fuzzer PRIVATE TOML_ENABLE_UNRELEASED_FEATURES=1)
    list(APPEND fuzzers toml_no_exceptions_fuzzer)
endif()

foreach(fuzzer IN LISTS fuzzers)
    target_link_libraries(${fuzzer} PRIVATE tomlplusplus::tomlplusplus $ENV{LIB_FUZZING_ENGINE})
    target_compile_features(${fuzzer} PRIVATE cxx_std_17)
    if(DEFINED ENV{OUT})
        install(TARGETS ${fuzzer} DESTINATION $ENV{OUT})
        install(FILES toml.dict DESTINATION $ENV{OUT} RENAME ${fuzzer}.dict)
        install(FILES toml.options DESTINATION $ENV{OUT} RENAME ${fuzzer}.options)
    endif()
endforeach()

if(NOT DEFINED ENV{OUT})
    message(WARNING "Cannot install if $OUT is not defined!")
endif()
