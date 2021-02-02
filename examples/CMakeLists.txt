cmake_minimum_required(VERSION 3.13...3.19)

project(Examples LANGUAGES CXX)

if(NOT TARGET tomlplusplus::tomlplusplus)
    find_package(tomlplusplus REQUIRED)
endif()

add_executable(toml_to_json_transcoder toml_to_json_transcoder.cpp)
target_link_libraries(toml_to_json_transcoder PRIVATE tomlplusplus::tomlplusplus)

enable_testing()
add_test(NAME toml_to_json_transcoder
    COMMAND toml_to_json_transcoder ${CMAKE_CURRENT_LIST_DIR}/example.toml
)
