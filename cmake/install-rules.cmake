include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    FILES "${PROJECT_SOURCE_DIR}/toml++.natvis" "${PROJECT_SOURCE_DIR}/cpp.hint"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/tomlplusplus"
    COMPONENT tomlplusplus_Development
)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT tomlplusplus_Development
    FILES_MATCHING REGEX "^.*[.](h|hpp|inl)$"
)

install(
    TARGETS tomlplusplus_tomlplusplus
    EXPORT tomlplusplusTargets
    RUNTIME COMPONENT tomlplusplus_Runtime
    LIBRARY COMPONENT tomlplusplus_Runtime NAMELINK_COMPONENT tomlplusplus_Development
    ARCHIVE COMPONENT tomlplusplus_Development
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# ---- Package files ----

set(tomlplusplus_pc_cflags "")
set(tomlplusplus_pc_libs "")

if(tomlplusplus_BUILD_LIB)
  set(tomlplusplus_pc_install_dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
  set(tomlplusplus_pc_description "TOML config file parser and serializer for C++")
  set(tomlplusplus_pc_libs "-L\${libdir} -ltomlplusplus")
  string(APPEND tomlplusplus_pc_cflags " -DTOML_HEADER_ONLY=0")
  if(BUILD_SHARED_LIBS)
    string(APPEND tomlplusplus_pc_cflags " -DTOML_SHARED_LIB=1")
  endif()
  write_basic_package_version_file(
      tomlplusplusConfigVersion.cmake
      COMPATIBILITY SameMajorVersion
  )
else()
  set(tomlplusplus_pc_install_dir "${CMAKE_INSTALL_DATADIR}/pkgconfig")
  set(tomlplusplus_pc_description "Header-only TOML config file parser and serializer for C++")
  write_basic_package_version_file(
      tomlplusplusConfigVersion.cmake
      COMPATIBILITY SameMajorVersion
      ARCH_INDEPENDENT
  )
endif()
if(tomlplusplus_UNRELEASED_FEATURES)
  string(APPEND tomlplusplus_pc_cflags " -DTOML_ENABLE_UNRELEASED_FEATURES=1")
endif()

set(
    tomlplusplus_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/tomlplusplus"
    CACHE STRING "CMake package config location relative to the install prefix"
)

mark_as_advanced(tomlplusplus_INSTALL_CMAKEDIR)

install(
    FILES
    "${PROJECT_SOURCE_DIR}/cmake/tomlplusplusConfig.cmake"
    "${PROJECT_BINARY_DIR}/tomlplusplusConfigVersion.cmake"
    DESTINATION "${tomlplusplus_INSTALL_CMAKEDIR}"
    COMPONENT tomlplusplus_Development
)

set(tomlplusplus_export_args "")
if(TOMLPLUSPLUS_BUILD_MODULES AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
  set(tomlplusplus_export_args CXX_MODULES_DIRECTORY modules)
endif()

install(
    EXPORT tomlplusplusTargets
    NAMESPACE tomlplusplus::
    DESTINATION "${tomlplusplus_INSTALL_CMAKEDIR}"
    COMPONENT tomlplusplus_Development
    ${tomlplusplus_export_args}
)

# ---- pkg-config ----

if(IS_ABSOLUTE "${tomlplusplus_pc_install_dir}")
  set(tomlplusplus_pc_prefix "${CMAKE_INSTALL_PREFIX}")
else()
  file(
      RELATIVE_PATH tomlplusplus_pc_prefix
      "${PROJECT_BINARY_DIR}/${tomlplusplus_pc_install_dir}" "${PROJECT_BINARY_DIR}"
  )
  string(REGEX REPLACE "/$" "" tomlplusplus_pc_prefix "${tomlplusplus_pc_prefix}")
  string(PREPEND tomlplusplus_pc_prefix "\${pcfiledir}/")
endif()
foreach(dir IN ITEMS INCLUDEDIR LIBDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${dir}}")
    set("tomlplusplus_pc_${dir}" "${CMAKE_INSTALL_${dir}}")
  else()
    set("tomlplusplus_pc_${dir}" "\${prefix}/${CMAKE_INSTALL_${dir}}")
  endif()
endforeach()

configure_file(cmake/tomlplusplus.pc.in tomlplusplus.pc @ONLY)

install(
    FILES "${PROJECT_BINARY_DIR}/tomlplusplus.pc"
    DESTINATION "${tomlplusplus_pc_install_dir}"
    COMPONENT tomlplusplus_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
