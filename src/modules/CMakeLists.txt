add_library(tomlplusplus_modules STATIC)
add_library(tomlplusplus::modules ALIAS tomlplusplus_modules)

set_target_properties(tomlplusplus_modules PROPERTIES EXPORT_NAME modules POSITION_INDEPENDENT_CODE ON)

target_sources(
    tomlplusplus_modules
    PUBLIC
    FILE_SET CXX_MODULES FILES
    tomlplusplus.cppm
)

target_link_libraries(tomlplusplus_modules PUBLIC tomlplusplus::tomlplusplus)

target_compile_features(tomlplusplus_modules PUBLIC cxx_std_20)

if(tomlplusplus_INSTALL)
  install(
      TARGETS tomlplusplus_modules
      EXPORT tomlplusplusTargets
      ARCHIVE COMPONENT tomlplusplus_Development
      FILE_SET CXX_MODULES
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tomlplusplus/modules"
      COMPONENT tomlplusplus_Development
  )
endif()
