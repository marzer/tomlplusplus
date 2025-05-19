file(GLOB_RECURSE TOMLPLUSPLUS_MODULES *.cppm)

add_library(tomlplusplus_modules)
target_sources(tomlplusplus_modules
    PUBLIC
    FILE_SET CXX_MODULES FILES
    ${TOMLPLUSPLUS_MODULES}
)

cmake_minimum_required(VERSION 3.28)

if(NOT COMMAND configure_cpp_module_target)
  function(configure_cpp_module_target target)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
      target_sources(${target} PUBLIC FILE_SET CXX_MODULES FILES ${TOMLPLUSPLUS_MODULES})
    else()
      message(WARNING "C++ modules require CMake 3.28+. Using standard compilation.")
      target_sources(${target} PRIVATE ${TOMLPLUSPLUS_MODULES})
    endif()
  endfunction()
endif()

configure_cpp_module_target(tomlplusplus_modules)

target_link_libraries(tomlplusplus_modules
  PUBLIC
    tomlplusplus::tomlplusplus
)

target_include_directories(tomlplusplus_modules
  PRIVATE
    ${tomlplusplus_SOURCE_DIR}/include
)

target_compile_features(tomlplusplus_modules PUBLIC cxx_std_20)

if(TOMLPLUSPLUS_ENABLE_INSTALL)
  install(TARGETS tomlplusplus_modules
    EXPORT tomlplusplus-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tomlplusplus/modules
  )
endif()
