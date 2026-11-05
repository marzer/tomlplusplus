set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS sources.txt)
file(STRINGS sources.txt tomlplusplus_test_sources)

# ---- Tests ----

add_executable(tomlplusplus_tests ${tomlplusplus_test_sources})
tomlplusplus_set_compile_options(tomlplusplus_tests)
target_link_libraries(tomlplusplus_tests PRIVATE tomlplusplus::tomlplusplus)
target_include_directories(tomlplusplus_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(
    tomlplusplus_tests PRIVATE
    "SHOULD_HAVE_EXCEPTIONS=$<BOOL:${tomlplusplus_EXCEPTIONS}>"
    "USE_VENDORED_LIBS=$<BOOL:${tomlplusplus_USE_VENDORED_LIBS}>"
    "USE_SINGLE_HEADER=$<BOOL:${tomlplusplus_TESTS_SINGLE_HEADER}>"
    "LEAK_TESTS=$<BOOL:${tomlplusplus_TESTS_LEAK}>"
)
if(NOT tomlplusplus_USE_VENDORED_LIBS)
  find_package(Catch2 3 REQUIRED)
  target_link_libraries(tomlplusplus_tests PRIVATE Catch2::Catch2)
endif()

foreach(
    locale IN ITEMS
    C
    en_US.utf8
    ja_JP.utf8
    it_IT.utf8
    tr_TR.utf8
    fi_FI.utf8
    fr_FR.utf8
    zh_CN.utf8
    de_DE.utf8
)
  set(tomlplusplus_test_environment "LC_ALL=${locale}")
  if(tomlplusplus_COVERAGE)
    list(APPEND tomlplusplus_test_environment "LLVM_PROFILE_FILE=${PROJECT_BINARY_DIR}/profiles/tests-%p.profraw")
  endif()
  add_test(
      NAME "tests.${locale}"
      COMMAND tomlplusplus_tests
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set_tests_properties(
      "tests.${locale}" PROPERTIES
      ENVIRONMENT "${tomlplusplus_test_environment}"
      SKIP_RETURN_CODE 77
  )
endforeach()

# ---- ODR test ----

add_executable(tomlplusplus_odr_test odr_test_1.cpp odr_test_2.cpp)
tomlplusplus_set_compile_options(tomlplusplus_odr_test)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # also checks that toml++ compiles warning-free with -ffinite-math-only (#241)
  target_compile_options(tomlplusplus_odr_test PRIVATE -ffinite-math-only)
endif()
target_link_libraries(tomlplusplus_odr_test PRIVATE tomlplusplus::tomlplusplus)
add_test(NAME odr_test COMMAND tomlplusplus_odr_test)

# ---- check-cmake ----

find_program(check-cmake check-cmake)
if(check-cmake)
  add_test(
      NAME check-cmake
      COMMAND "${check-cmake}" "${PROJECT_SOURCE_DIR}"
  )
endif()

# ---- check-symbols ----

if(tomlplusplus_BUILD_LIB
   AND BUILD_SHARED_LIBS
   AND CMAKE_SYSTEM_NAME STREQUAL "Linux"
   AND CMAKE_BUILD_TYPE STREQUAL "Release"
   AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
   AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15
   AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16
   AND NOT tomlplusplus_ASAN
   AND NOT tomlplusplus_UBSAN
   AND NOT tomlplusplus_COVERAGE)
  find_program(check-symbols check-symbols)
  if(check-symbols)
    add_test(
        NAME check-symbols
        COMMAND
        "${check-symbols}" "$<TARGET_FILE:tomlplusplus_tomlplusplus>"
        --check "${PROJECT_SOURCE_DIR}/symbols.txt"
        --include "^toml::"
    )
  endif()
endif()
