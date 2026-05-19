cmake_minimum_required(VERSION 3.28)

file(GLOB_RECURSE TOMLPLUSPLUS_MODULES *.cppm)

add_library(tomlplusplus_modules)
target_sources(tomlplusplus_modules
    PUBLIC
    FILE_SET CXX_MODULES FILES
    ${TOMLPLUSPLUS_MODULES}
)

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
