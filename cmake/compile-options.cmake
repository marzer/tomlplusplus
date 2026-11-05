if(tomlplusplus_COVERAGE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "tomlplusplus_COVERAGE requires Clang")
endif()
if(MSVC AND (tomlplusplus_ASAN OR tomlplusplus_UBSAN))
  message(FATAL_ERROR "tomlplusplus_ASAN and tomlplusplus_UBSAN are not supported with MSVC")
endif()

function(tomlplusplus_set_compile_options target)
  target_compile_features("${target}" PRIVATE cxx_std_17)
  set_target_properties("${target}" PROPERTIES CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
  get_target_property(target_type "${target}" TYPE)
  if(WIN32 AND tomlplusplus_BUILD_LIB AND BUILD_SHARED_LIBS AND target_type STREQUAL "EXECUTABLE"
     AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    add_custom_command(
        TARGET "${target}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${target}>>,copy_if_different,true>
            "$<TARGET_RUNTIME_DLLS:${target}>" "$<TARGET_FILE_DIR:${target}>"
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
  endif()

  if(MSVC)
    target_compile_options(
        "${target}" PRIVATE
        /bigobj /Gy /GF /openmp- /utf-8 /volatile:iso
        /Zc:__cplusplus /Zc:inline /Zc:externConstexpr /Zc:preprocessor
    )
    if(tomlplusplus_PERMISSIVE)
      target_compile_options("${target}" PRIVATE /permissive)
    else()
      target_compile_options("${target}" PRIVATE /permissive-)
    endif()
    if(tomlplusplus_EXCEPTIONS)
      target_compile_options("${target}" PRIVATE /Zc:throwingNew)
      target_compile_definitions("${target}" PRIVATE _HAS_EXCEPTIONS=1)
    else()
      target_compile_options("${target}" PRIVATE /EHs-c-)
      target_compile_definitions("${target}" PRIVATE _HAS_EXCEPTIONS=0)
    endif()
    if(tomlplusplus_PEDANTIC)
      target_compile_options("${target}" PRIVATE /W4)
    endif()
    if(tomlplusplus_WERROR)
      target_compile_options("${target}" PRIVATE /WX)
    endif()
    return()
  endif()

  if(tomlplusplus_IS_TOP_LEVEL)
    target_compile_definitions(
        "${target}" PRIVATE
        "$<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS=1;_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST>"
    )
  endif()
  if(NOT tomlplusplus_EXCEPTIONS)
    target_compile_options("${target}" PRIVATE -fno-exceptions)
  endif()
  if(tomlplusplus_WERROR)
    target_compile_options("${target}" PRIVATE -Werror)
  endif()
  if(tomlplusplus_ASAN)
    target_compile_options("${target}" PRIVATE -fsanitize=address -fno-sanitize-recover=all -fno-omit-frame-pointer)
    target_link_options("${target}" PRIVATE -fsanitize=address)
  endif()
  if(tomlplusplus_UBSAN)
    target_compile_options("${target}" PRIVATE -fsanitize=undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    target_link_options("${target}" PRIVATE -fsanitize=undefined)
  endif()
  if(tomlplusplus_COVERAGE)
    target_compile_options("${target}" PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_compile_definitions("${target}" PRIVATE TOML_ALWAYS_INLINE=inline)
    target_link_options("${target}" PRIVATE -fprofile-instr-generate)
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(
        "${target}" PRIVATE
        -ferror-limit=5
        -fchar8_t
        -Wno-unused-command-line-argument
        -Wno-reserved-macro-identifier
    )
    if(tomlplusplus_PEDANTIC)
      target_compile_options(
          "${target}" PRIVATE
          -Wno-unknown-warning-option
          -Wall
          -Wextra
          -Wpedantic
          -Weverything
          -Wno-c++98-compat
          -Wno-c++98-compat-pedantic
          -Wno-switch-enum
          -Wno-switch-default
          -Wno-covered-switch-default
          -Wno-padded
          -Wno-float-equal
          -Wno-c2y-extensions
          -Wno-unsafe-buffer-usage
          -Wno-nrvo
          -Wno-poison-system-directories
      )
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        "${target}" PRIVATE
        -fmax-errors=5
        -fchar8_t
        -Wno-init-list-lifetime
    )
    if(tomlplusplus_PEDANTIC)
      target_compile_options(
          "${target}" PRIVATE
          -Wall
          -Wextra
          -Wpedantic
          -Wcast-align
          -Wcast-qual
          -Wctor-dtor-privacy
          -Wdisabled-optimization
          -Wimport
          -Winit-self
          -Wlogical-op
          -Wmissing-declarations
          -Wmissing-field-initializers
          -Wmissing-format-attribute
          -Wmissing-include-dirs
          -Wmissing-noreturn
          -Wold-style-cast
          -Woverloaded-virtual
          -Wpacked
          -Wpointer-arith
          -Wredundant-decls
          -Wshadow
          -Wsign-conversion
          -Wsign-promo
          -Wstack-protector
          -Wstrict-null-sentinel
          -Wswitch-default
          -Wundef
          -Wunreachable-code
          -Wunused
          -Wunused-parameter
          -Wuseless-cast
          -Wvariadic-macros
          -Wwrite-strings
      )
    endif()
  endif()
endfunction()
