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
target_link_libraries(toml_fuzzer PRIVATE tomlplusplus::tomlplusplus $ENV{LIB_FUZZING_ENGINE})
target_compile_features(toml_fuzzer PRIVATE cxx_std_17)
target_compile_definitions(toml_fuzzer PRIVATE NDEBUG) # Do not want assertions

if (DEFINED ENV{OUT})
    install(TARGETS toml_fuzzer DESTINATION $ENV{OUT})
else ()
    message(WARNING "Cannot install if $OUT is not defined!")
endif ()
