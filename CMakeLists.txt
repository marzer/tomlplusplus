cmake_minimum_required(VERSION 3.13...3.19)

project(tomlplusplus LANGUAGES CXX VERSION 2.3.1)

# Determine if this project is built as a subproject (using
# add_subdirectory) or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
endif()


add_library(tomlplusplus INTERFACE)
add_library(tomlplusplus::tomlplusplus ALIAS tomlplusplus)
target_compile_features(tomlplusplus INTERFACE cxx_std_17)
target_include_directories(tomlplusplus INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


option(TOMLPP_BUILD_EXAMPLES "Build examples." ${MASTER_PROJECT})
if(TOMLPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()


option(TOMLPP_INSTALL "Generate the install target" ${MASTER_PROJECT})
if(TOMLPP_INSTALL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)
    install(TARGETS tomlplusplus EXPORT ${PROJECT_NAME}Targets)

    install(DIRECTORY include/toml++ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    write_basic_package_version_file(
        ${PROJECT_NAME}ConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES cmake/tomlplusplusConfig.cmake # ---------->
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        FILE ${PROJECT_NAME}Targets.cmake   # <----------
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endif()
