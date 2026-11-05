cmake_minimum_required(VERSION 3.14...4.4)

project(consumer VERSION 1.0.0 LANGUAGES CXX)

if(ADD_SUBDIRECTORY)
  add_subdirectory("${ADD_SUBDIRECTORY}" tomlplusplus)
else()
  find_package(tomlplusplus REQUIRED)
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE tomlplusplus::tomlplusplus)

if(TARGET tomlplusplus::modules)
  add_executable(consumer_import import.cpp)
  target_link_libraries(consumer_import PRIVATE tomlplusplus::modules)
endif()
