# Utilized by OSSFuzz to build the harness(es) for continuous fuzz-testing
# OSSFuzz defines the following environment variables, that this target relies upon:
# CXX, CFLAGS, LIB_FUZZING_ENGINE, OUT
cmake_minimum_required(VERSION 3.14)

project(Fuzzer LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)

add_definitions(-DNDEBUG)  # Do not want assertions

if (DEFINED ENV{CFLAGS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} $ENV{CFLAGS}")
endif ()
if (DEFINED ENV{CXXFLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{CXXFLAGS}")
endif ()

if(PROJECT_IS_TOP_LEVEL)
    find_package(tomlplusplus REQUIRED)
endif()

add_executable(toml_fuzzer toml_fuzzer.cpp)
target_link_libraries(toml_fuzzer PRIVATE tomlplusplus::tomlplusplus $ENV{LIB_FUZZING_ENGINE})
target_compile_features(toml_fuzzer PRIVATE cxx_std_17)

if (DEFINED ENV{OUT})
    install(TARGETS toml_fuzzer DESTINATION $ENV{OUT})
else ()
    message(WARNING "Cannot install if $OUT is not defined!")
endif ()