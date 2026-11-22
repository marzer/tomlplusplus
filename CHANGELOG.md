# Changelog

## v3.5.1

No code changes. Patch bump only to address a git tag snafu.

## v3.5.0

**Highlights**

- Support for TOML v1.1.0 🎉&#xFE0F;
- Support for C++20 `std::chrono` types
- C++20 module support with `import tomlplusplus;`
- Faster parsing and formatting
- Faster compiles and smaller binaries
- Floats print in their shortest round-trip form with every compiler
- Many fixes for crashes and undefined behaviour

> [!WARNING]
> ⚠&#xFE0F; Meson users: a default Meson build is now header-only. This is a bugfix for a
> regression that was accidentally introduced in v3.4.0 where the default became
> a shared library build, despite the documentation stating otherwise. If you want
> to keep shared library builds, you must specify `-Dbuild_lib=true`.

Full changelog below.

**Fixes**

- Fixed assertion failure when parsing some incomplete date-times (#313) (@allenflux)
- Fixed `is_homogeneous()` overloads with `first_nonmatch` outparam being broken in optimized builds (#231) (@Forbinn)
- Fixed unclear error message when parsing integers that would overflow (#224) (@chrimbo)
- Fixed CMake `install` target installing `meson.build` files (#236) (@JWCS)
- Fixed "unresolved symbol" error with nvc++ (#220) (@Tomcat-42)
- Fixed stack overflow when parsing deeply nested dotted keys (#242) (@tyler92)
- Fixed assertion failure when parsing some malformed documents (#321) (@emincan2000)
- Fixed a number of bugs when parsing long values with underscores
- Fixed assertion failure when parsing table headers starting with `[[[` (#301) (@gistrec)
- Fixed redefinition errors showing the wrong text for quoted keys (#300) (@SAY-5)
- Fixed undefined behaviour when parsing some non-ASCII characters (#295) (@94xhn)
- Fixed undefined behaviour in `toml_formatter` with fractional floats (#276) (@mikael-s-persson)
- Fixed downcasts in the node hierarchy that used `reinterpret_cast` (#299) (@ShereeMorphett)
- Fixed `parse_error` not being exported from shared libraries (#316) (@ac1982)
- Fixed build error with MSYS2 (#264) (@Toni500github)
- Fixed missing `#include` in `date_time.hpp` (#298) (@dwithun-dev)
- Fixed Clang 20 deprecation warning about literal operators (#263) (@eddelbuettel)
- Fixed C++26 deprecation warning about `std::is_trivial` (#270) (@nmpassthf)
- Fixed compiler warnings with `TOML_FLOAT_CHARCONV` on some toolchains
- Fixed MSVC warning C4702 (unreachable code) from the parser (#307) (@soniafergusonship)
- Fixed out-of-bounds read in `path::subpath()` (#319) (@aouxwoux)
- Fixed undefined behaviour when adding a `toml::path` to itself (@arfa-21)
- Fixed some multi-line strings not round-tripping (#323) (@arfa-21)
- Fixed some parse errors reporting the wrong position
- Fixed some misleading parse error messages
- Fixed times with very long fractional seconds failing to parse
- Fixed leap seconds failing to parse
- Fixed line-ending backslashes trimming non-ASCII spaces
- Fixed very small floats failing to parse with MSVC or libc++
- Fixed floats printing differently on different compilers (#179) (@aborisovich)
- Fixed `yaml_formatter` writing numbers that YAML 1.1 loaders read as strings
- Fixed floats changing value with a non-default rounding mode
- Fixed `SIGFPE` when parsing out-of-range floats with FP traps enabled
- Fixed out-of-memory handling when parsing or printing floats
- Fixed some hexadecimal floats parsing to the wrong value (`TOML_ENABLE_UNRELEASED_FEATURES`)
- Fixed `toml::parse()` returning an empty table for non-seekable streams
- Fixed `toml::parse()` crashing or succeeding on a failed stream
- Fixed invalid UTF-8 or a failed read hiding an earlier parse error
- Fixed `toml::parse()` failing on streams that throw on failbit or eofbit
- Fixed `toml::parse()` not compiling with a string literal source path
- Fixed `toml::parse_file()` succeeding on a directory or a failed read
- Fixed slow parsing of documents with many tables
- Fixed a default-constructed `toml::key` leaving its source region uninitialised
- Fixed use-after-free when assigning between nested tables or arrays
- Fixed null-pointer dereference when inserting a `toml::array` into itself
- Fixed `toml::table` asserting when built from an empty initialiser list
- Fixed `toml::array::insert()` not compiling for some integer arguments
- Fixed comparing a `toml::array` with a `std::vector<bool>` not compiling
- Fixed `node_view<node> == node_view<const node>` not compiling on GCC 11
- Fixed exception-safety bugs in `toml::path_component`
- Fixed some explicit conversions of `toml::value` rvalues not compiling
- Fixed the `toml::value(val, flags)` constructor not compiling
- Fixed undefined behaviour when converting out-of-range floats to integers
- Fixed conversions to `__float128` and `_Float16` rejecting most numbers
- Fixed formatters asserting on a failed `parse_result` without exceptions
- Fixed formatters dropping a trailing incomplete UTF-8 sequence
- Fixed undefined behaviour in `toml_formatter` with some numeric arrays
- Fixed `toml_formatter` emitting leading blank lines when reused
- Fixed keys containing U+1680 or U+FEFF formatting as bare keys (`TOML_ENABLE_UNRELEASED_FEATURES`)
- Fixed bare keys starting with U+180E or U+FEFF failing to parse (`TOML_ENABLE_UNRELEASED_FEATURES`)
- Fixed `json_formatter` emitting invalid JSON with some format flags
- Fixed `yaml_formatter` emitting incorrect YAML in some cases
- Fixed `for_each()` not compiling with MSVC in C++17 mode (#219) (@KevDi)
- Fixed warnings when compiling with Clang and `-ffinite-math-only` (#241) (@theakman2)
- Fixed spurious nesting depth errors with MSVC and the C++20 module (#288) (@DhirenTheHeadlights)
- Fixed linker errors with the JSON and YAML formatters in a MinGW DLL
- Fixed `TOML_INCLUDE_WINDOWS_H` failing with MinGW on case-sensitive filesystems
- Fixed a Clang `-Wabsolute-value` warning with MinGW
- Fixed the formatters not escaping some control characters
- Fixed crashes when parsing with mixed `TOML_EXCEPTIONS` settings
- Fixed stack overflow when destroying deeply nested tables
- Fixed use-after-free when assigning or appending a `toml::path`'s own key to it
- Fixed `insert_or_assign()` leaving `toml::table` iterators dangling
- Fixed inserting a `toml::inserter` of a null `node_view` adding a null node
- Fixed undefined behaviour when parsing a file with an empty UTF-8 path
- Fixed a `toml::value` losing its source region when assigned to itself
- Fixed `value()` and `value_or()` not compiling for some scoped enums
- Fixed `emplace()` and `emplace_back()` not compiling with string literals
- Fixed wrong results when comparing an integer `toml::value` with a float
- Fixed long parse errors ending in an incomplete UTF-8 sequence
- Fixed some parse errors quoting a control character without escaping it
- Fixed some table headers silently redefining a dotted-key table
- Fixed `toml::parse_file()` failing to open pipes and FIFOs
- Fixed `using namespace toml;` leaking into the `TOML_IMPLEMENTATION` file
- Fixed `toml_formatter` wrapping some arrays at the wrong width
- Fixed some user-defined strings being cut off at the first NUL

**Additions**

- Added support for TOML v1.1.0 (#289)
- Added `toml::get_line()` (#255, #256, #258) (@N-Dekker)
- Added `toml::format_flags::force_multi_line_arrays` (#282) (@rafal-c)
- Added support for user-defined string types (#306) (@mikomikotaishi)
- Added support for C++20 `std::chrono` types (#291, #322) (@mikomikotaishi)
- Added C++20 module support with `import tomlplusplus;` (#266, #267, #273, #303) (@mikomikotaishi, @N-Dekker, @davidstone)
- Added `TOML_ENABLE_CHRONO` config
- Added `TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA` config (#247) (@N-Dekker)
- Added `TOML_DISABLE_NOEXCEPT_NOEXCEPT` config (#271) (@N-Dekker)
- Added IWYU pragmas to `toml++/toml.hpp` (#277) (@fnuecke)

**Changes**

- Printing a `source_region` also shows its end position (#244) (@N-Dekker)
- Improvements to parsing and formatting speed
- Improvements to float parsing
- Improvements to compile speed and binary size
- Improvements to `array::flatten()` speed on large arrays
- Improvements to `array::prune()` speed on large arrays
- Lowered the default of `TOML_MAX_NESTED_VALUES` from 256 to 128 (#293) (@danielbodorin)
- `TOML_MAX_NESTED_VALUES` now limits the nesting depth of tables too
- `TOML_INT_CHARCONV` no longer has any effect
- Changed the wording of some parse error messages

**Removals**

- Removed the Visual Studio solution and project files
- Removed the `parse_benchmark` example

**Build system**

- Meson: Fixed a default build compiling the library instead of being header-only (#296) (@jfsimoneau)
- Meson: Tests built with `-Duse_vendored_libs=false` now need Catch2 v3
- Meson: Installs the examples when they're built in a top-level build (#233) (@Scrumplex)
- Meson: Fixed the installed CMake package failing on CMake 3.14 and 3.15
- Cmake: Added `tomlplusplus_INSTALL` (#240) (@DapengFeng)
- Cmake: Added `tomlplusplus_BUILD_LIB` to compile the library
- Cmake: Added `tomlplusplus_UNRELEASED_FEATURES`
- Cmake: Added a pkg-config file, `tomlplusplus.pc` (#274) (@enessmr)
- Cmake: Added the install components `tomlplusplus_Runtime` and `tomlplusplus_Development`
- Cmake: Fixed `TOMLPLUSPLUS_BUILD_MODULES` never installing the module
- Cmake: Added developer options, such as `tomlplusplus_BUILD_TESTS`
- Cmake, Meson: Builds from a git checkout regenerate `toml.hpp`
- Fixed the tests failing to build with libc++ in C++23 mode (#315) (@peppergrayxyz)
- Added OSS-Fuzz integration (#214) (@capuanob)

<br><br>

## v3.4.0

**Fixes**

- Fixed `value_flags` not being preserved correctly when inserting into tables and arrays (#108) (@LebJe)
- Fixed `toml::value::flags()` not being cleared when `std::move`-ing a value
- Fixed error in README (#195) (@andrewkcorcoran)
- Fixed compiler error when using NVCC (#198) (@thompsonnoahe)
- Fixed `noexcept(...)` sometimes being incorrectly derived on `for_each()`
- Fixed `for_each()` compilation error on GCC &lt;= 7 (#197) (@sagi-ottopia, @damirbarr)
- Fixed `FLT_RADIX` check getting broken by Intel MKL headers (#202) (@iago-lito)
- Fixed keys containing `\t` incorrectly formatting as bare keys (@jasmine-zhu, @arp242)
- Fixed keys containing `\t` and `\n` not round-tripping correctly (@arp242)

**Additions**

- Added support for using enums with `value_or()`

**Changes**

- Renamed header files to have `.hpp` extension (`toml.h` is still present for backwards-compatibility)

**Build system**

- Fixed meson builds with `-Ddefault_library=static` having hidden symbols on GNU compilers (#201) (@vlad0x00)

<br><br>

## v3.3.0

**Fixes**

- Fixed null pointer dereference in parser when exceptions are disabled (#169) (@ncaklovic)
- Fixed spurious warnings in MSVC 19.34
- Fixed `toml::parse_file()` on windows for non-ASCII paths
- Fixed a spurious table redefinition error (#187) (@jorisvr)
- Fixed UB edge-case in integer parsing (#188) (@jorisvr)
- Fixed some build issues with Apple-flavoured Clang (#189) (@eddelbuettel)

**Additions**

- Added `toml::format_flags::terse_key_value_pairs`
- Added `TOML_ENABLE_FLOAT16` config (#178) (@Scrumplex)

**Removals**

- Removed automatic detection of `_Float16` (you must explicitly set `TOML_ENABLE_FLOAT16` to enable it) (#186) (@benthetechguy)

**Build system**

- Re-wrote the meson scripts to fix a number of issues (#185, #186) (@Tachi107, @benthetechguy)
- Increased the minimum required meson version to `0.61.0`

<br><br>

## v3.2.0

**Fixes**

- Fixed `[dotted.table]` source columns sometimes being off by one (#152) (@vaartis)
- Fixed spurious `Wnull-dereference` warning on GCC (#164) (@zaporozhets)
- Fixed `print_to_stream` ambiguity for `size_t` (#167) (@acronce)

**Additions**

- Added value type deduction to `emplace()` methods
- Added `toml::path` utility type (#153, #156, #168) (@jonestristand, @kcsaul)
- Added config option `TOML_CALLCONV`
- Added missing relational operators for `source_position`

**Changes**

- Relaxed cvref requirements of `is_homogeneous()`, `emplace()`, `emplace_back()`, `emplace_hint()`
- Relaxed mantissa and digits10 requirements of extended float support

<br><br>

## v3.1.0

**Fixes**

- Fixed potential segfault when calling `at_path()` with an empty string
- Fixed UB in internal unicode machinery (#144) (@kchalmer)
- Fixed a number of spurious warnings with Clang 10 (#145, #146) (@chronoxor)

**Additions**

- Added `toml::array::for_each()`
- Added `toml::table::for_each()`
- Added config options `TOML_EXPORTED_CLASS`, `TOML_EXPORTED_MEMBER_FUNCTION`, `TOML_EXPORTED_STATIC_FUNCTION` &amp; `TOML_EXPORTED_FREE_FUNCTION`
- Added support for escape sequence `\e` when using `TOML_ENABLE_UNRELEASED_FEATURES` ([toml/790](https://github.com/toml-lang/toml/pull/790))
- Added support for more unicode in bare keys when using `TOML_ENABLE_UNRELEASED_FEATURES` ([toml/891](https://github.com/toml-lang/toml/pull/891))

**Removals/Deprecations**

- Deprecated old `TOML_API` option in favour new `TOML_EXPORTED_X` options
  (it will continue to work as it did before if none of the new function export options are defined)

**Build system**

- Meson: added `compile_library` option (@Tachi107)
- Meson: added `ubsan_tests` and `ubsan_examples` options
- Meson: use system dependencies where available when building tests (@Tachi107)

<br><br>

## v3.0.1

This is a single-bugfix release to fix an ODR issue for people using header-only mode in multiple
translation units. If you aren't seeing linker errors because of `toml::array::insert_at()`,
this release holds nothing of value over v3.0.0.

**Fixes**

- Fixed erroneous use of `TOML_API` causing ODR issue (#136) (@Azarael)

<br><br>

## v3.0.0

This release will be a major version bump, so it's ABI breaks all around.
Any changes that are likely to cause migration issues (API changes, build system breakage, etc.) are indicated with ⚠&#xFE0F;.

**Fixes**

- ⚠&#xFE0F; Fixed `toml::table` init-list constructor requiring double-brackets
- ⚠&#xFE0F; Fixed `TOML_API` + extern templates causing linker errors in some circumstances
- ⚠&#xFE0F; Fixed incorrect `noexcept` specifications on many functions
- ⚠&#xFE0F; Fixed missing `TOML_API` on some interfaces
- Fixed `toml::json_formatter` not formatting inf and nan incorrectly
- Fixed a number of spec conformance issues (#127, #128, #129, #130, #131, #132, #135) (@moorereason)
- Fixed an illegal table redefinition edge case (#112) (@python36)
- Fixed documentation issues
- Fixed GCC bug causing memory leak during parse failures (#123, #124) (@rsmmr, @ronalabraham)
- Fixed incorrect handling of vertical whitespace in keys when printing TOML to streams
- Fixed incorrect source position in redefinition error messages
- Fixed missing includes `<initializer_list>`, `<utility>`
- Fixed parser not correctly round-tripping the format of binary and octal integers in some cases
- Fixed some incorrect unicode scalar sequence transformations (#125)
- Fixed strong exception guarantee edge-cases in `toml::table` and `toml::array`

**Additions**

- Added value flags to array + table insert methods (#44) (@levicki)
- Added support for Unicode 14.0
- Added support for ref categories and cv-qualifiers in `toml::node::ref()`
- Added magic `toml::value_flags` constant `toml::preserve_source_value_flags`
- Added clang's enum annotation attributes to all enums
- Added `TOML_ENABLE_FORMATTERS` option
- Added `toml::yaml_formatter`
- Added `toml::value` copy+move constructor overloads with flags override
- Added `toml::table::prune()`
- Added `toml::table::lower_bound()` (same semantics as `std::map::lower_bound()`)
- Added `toml::table::emplace_hint()` (same semantics as `std::map::emplace_hint()`)
- Added `toml::table::at()` (same semantics as `std::map::at()`)
- Added `toml::node_view::operator==`
- Added `toml::key` - provides a facility to access the source_regions of parsed keys (#82) (@vaartis)
- Added `toml::is_key<>` and `toml::is_key_or_convertible<>` metafunctions
- Added `toml::format_flags::relaxed_float_precision` (#89) (@vaartis)
- Added `toml::format_flags::quote_infinities_and_nans`
- Added `toml::format_flags::indent_sub_tables` (#120) (@W4RH4WK)
- Added `toml::format_flags::indent_array_elements` (#120) (@W4RH4WK)
- Added `toml::format_flags::allow_unicode_strings`
- Added `toml::format_flags::allow_real_tabs_in_strings`
- Added `toml::format_flags::allow_octal_integers`
- Added `toml::format_flags::allow_hexadecimal_integers`
- Added `toml::format_flags::allow_binary_integers`
- Added `toml::date_time` converting constructors from `toml::date` and `toml::time`
- Added `toml::at_path()`, `toml::node::at_path()` and `toml::node_view::at_path()` for qualified path-based lookups (#118) (@ben-crowhurst)
- Added `toml::array::resize()` param `default_init_flags`
- Added `toml::array::replace()` (#109) (@LebJe)
- Added `toml::array::prune()`
- Added `toml::array::at()` (same semantics as `std::vector::at()`)
- Added `parse_benchmark` example
- Added `operator->` to `toml::value` for class types

**Changes**

- ⚠&#xFE0F; `toml::format_flags` is now backed by `uint64_t` (was previously `uint8_t`)
- ⚠&#xFE0F; `toml::source_index` is now an alias for `uint32_t` unconditionally (was previously dependent on `TOML_LARGE_FILES`)
- ⚠&#xFE0F; `toml::table` now uses `toml::key` as the key type (was previously `std::string`)
- ⚠&#xFE0F; `toml::value_flags` is now backed by `uint16_t` (was previously `uint8_t`)
- ⚠&#xFE0F; made all overloaded operators 'hidden friends' where possible
- ⚠&#xFE0F; renamed `toml::default_formatter` to `toml::toml_formatter` (`toml::default_formatter` is now an alias)
- ⚠&#xFE0F; renamed `TOML_PARSER` option to `TOML_ENABLE_PARSER` (`TOML_PARSER` will continue to work but is deprecated)
- ⚠&#xFE0F; renamed `TOML_UNRELEASED_FEATURES` to `TOML_ENABLE_UNRELEASED_FEATURES` (`TOML_UNRELEASED_FEATURES` will continue to work but is deprecated)
- ⚠&#xFE0F; renamed `TOML_WINDOWS_COMPAT` to `TOML_ENABLE_WINDOWS_COMPAT` (`TOML_WINDOWS_COMPAT` will continue to work but is deprecated)
- applied clang-format to all the things 🎉&#xFE0F;
- exposed `TOML_NAMESPACE_START` and `TOML_NAMESPACE_END` macros to help with ADL specialization scenarios
- Improved performance of parser
- made date/time constructors accept any integral types
- moved all implementation headers to `/impl`
- Renamed all implementation headers to `.h` and 'source' headers to `.inl`
- Updated conformance tests

**Removals**

- ⚠&#xFE0F; Removed `toml::format_flags::allow_value_format_flags`
- ⚠&#xFE0F; Removed `TOML_LARGE_FILES` (it is now default - explicitly setting `TOML_LARGE_FILES` to `0` will invoke an `#error`)
- ⚠&#xFE0F; Removed unnecessary template machinery (esp. where ostreams were involved)
- Removed unnecessary uses of `final`

**Build system**

- ⚠&#xFE0F; increased minimum required meson version to `0.54.0`
- Disabled 'install' path when being used as a meson subproject (#114) (@Tachi107)
- Fixed builds failing with meson 0.6.0 (#117) (@Tachi107)
- General meson improvements and fixes (#115) (@Tachi107)
- Used `override_dependency` where supported (#116) (@Tachi107)

<br><br>

## v2.5.0

**Fixes**

- Fixed linkage error with windows compat mode
- Fixed `TOML_CONSTEVAL` broken in MSVC (again)
- Fixed minor documentation bugs
- Fixed cmake project version being incorrect (#110) (@GiulioRomualdi)

**Additions**

- Added support for lowercase 't' and 'z' in datetimes (per spec)
- Added natvis file to cmake install (#106) (@Ryan-rsm-McKenzie)
- Added VS cpp.hint file to cmake install
- Added metafunctions `is_container`, `is_chronological`, `is_value`, `is_node`, `inserted_type_of`

**Changes**

- Improved debug code size by removing unnecessary std::forwards and std::moves
- Modernized the CMake build files (#102, #103, #105) (@friendlyanon)
- Updated conformance tests

<br><br>

## v2.4.0

**Fixes**

- Fixed `node::value()` not retrieving inf and nan correctly
- Fixed dotted kvps being unable to add subtables (#61) (@Validark)
- Fixed linker error on linux ICC (#83) (@blackwer)
- Fixed segfault JSON-formatting a failed `parse_result` (#96) (@proydakov)
- Fixed spurious newline after JSON formatting a table
- Fixed VS intellisense not detecting `TOML_COMPILER_EXCEPTIONS` correctly
- Fixed crash with pathologically-nested inputs (#100) (@geeknik)
- Fixed `parse_result` natvis
- Fixed false-positive `char8_t` support detection on older compilers
- Fixed unnecessary `#include <Windows.h>` Windows builds (@BeastLe9enD)
- Fixed `TOML_CONSTEVAL` breaking on VS 16.10.0pre2
- Fixed spurious warnings with MSVC /Wall
- Fixed missing blank lines between consecutive empty tables/A-o-T
- Fixed unnecessary `TOML_API` declarations
- Fixed many small documentation issues

**Additions**

- Added proper cmake support (#85) (@ClausKlein)
- Added cmake FetchContent information to documentation (#101) (@proydakov)

**Removals**

- Removed explicit `#include <fstream>` requirement for `parse_file()`

<br><br>

## v2.3.0

**Fixes**

- Fixed compiler errors caused by `<charconv>` with Apple-flavoured clang
- Fixed array and table iterators missing `iterator_category` (#77) (@HazardyKnusperkeks)
- Fixed `Wuseless-cast` warnings on GCC 10 (#75) (@HazardyKnusperkeks)
- Fixed formatter not correctly line wrapping in some rare circumstances (#73) (@89z)
- Fixed an unnecessary global compiler flag breaking builds when used as a meson subproject (#72) (@jamabr)
- Fixed link error caused by `<charconv>` on emscripten (#71) (@suy)
- Fixed ambiguity with the `toml::literals` inline namespace (#69) (@std-any-emplace)
- Fixed formatter emitting superfluous newlines after printing tables (#68) (@std-any-emplace)
- Fixed array and table iterators not converting between const and non-const versions of themselves (#67) (@std-any-emplace)
- Fixed some parser crashes when given pathologically-malformed UTF-8 (#65) (@sneves)

<br><br>

## v2.2.0

**Fixes**

- Fixed some issues building with VS2017 (#55) (@sobczyk)
- Fixed `_Float16` erroneously detected as supported on g++ (#57) (@sobczyk)
- Fixed `<Windows.h>` causing compilation failure on mingw (#63) (@rezahousseini)
- Fixed CMake and pkg-config files not being installed into architecture-agnostic directories (#59) (@tambry)
- Fixed memory leak during parsing (#64) (@sneves)
- Fixed ambiguous `operator==` error on MSVC (#56) (@HellsingDarge)

**Additions**

- Added additional node_view constructors
- Added ability to specify serialization format of integer values
- Added integer value serialization format round trip (e.g. hex in, hex out)

**Changes**

- Updated conformance tests
- TOML version bump to v1.0.0-rc.3
- Refactors and cleanups based on feedback given [here](https://medium.com/@julienjorge/code-review-of-toml-f816a6071120)

**Build system**

- Renamed build options to `snake_case`
- Tests, examples and cmake config now explicitly disabled when used as a subproject
- Removed small_binaries (it's now implicit when building as release)
- Bumped minimum meson version to 0.53

<br><br>

## v2.1.0

**Fixes**

- Fixed inconsistent emission of leading/trailing newlines when writing a table to an ostream (#48) (@levicki)
- Fixed `Wcast-align` warning spam on ARM
- Fixed `array::insert` not working correctly in some cases
- Fixed `node::value_or()` not having the same semantics as `node::value()` (#50) (@whiterabbit963)
- Fixed 'misleading assignment' of rvalue node_views (#52) (@Reedbeta)
- Fixed some issues handling infinities and NaNs (#51) (@Reedbeta)
- Fixed some minor documentation issues

**Additions**

- Added support for `__fp16`, `_Float16`, `__float128`, `__int128_t` and `__uint128_t`
- Added copy construction/assignment for arrays, tables and values
- Added insert, emplace, push_back etc. compatibility with node_views
- Added `node::is_homogenous`
- Added `table::is_homogenous`
- Added `value::is_homogenous` (just for generic code's sake)
- Added `is_homogenous` overload for identifying failure-causing element
- Added implicit conversion operator from `node` to `node_view` (#52) (@Reedbeta)

**Changes**

- Renamed `TOML_ALL_INLINE` to `TOML_HEADER_ONLY` (the old name will still work, but is no longer documented)
- General cleanup

<br><br>

## v2.0.0

This release contains a fairly significant number of 'quality of life' improvements, yay! But also necessitates an ABI
break (hence the version number bump). Changes that might block a migration are annotated with ⚠&#xFE0F;.

**Fixes**

- Fixed infinity and NaN-related code breaking when using `-ffast-math` and friends
- Fixed narrowing conversion warnings when constructing int values from unsigned
- Fixed Visual Studio debugger native visualizations for `date`, `time`, `time_offset`, `date_time`
- Fixed some static assert messages being badly formatted on clang
- Fixed internal macro `assert_or_assume` leaking out of `toml_parser.hpp`

**Additions**

- Added additional types allowed in `node::value()` and `node::value_or()` ([see `value()` dox for examples](https://marzer.github.io/tomlplusplus/classtoml_1_1node.html#ab144c1ae90338b6b03f6af0574c87993))
- Added additional types allowed in `node_view::value()` and `node_view::value_or()`
- Added `node::value_exact()` and `node_view::value_exact()`
- Added support for interop with wide strings on Windows:
  - wide-string path arg overloads of `parse()` and `parse_file()`
  - wide-string support to all relevant `table` and `array` ops
  - wide-string support to `node::value(), node::value_or()`
  - wide-string support to `node_view::value(), node_view::value_or()`
  - wide-string support to `value<string>` constructor
  - wide-string overloads of `node_view::operator[]`
  - `source_region::wide_path()`
  - `TOML_WINDOWS_COMPAT` switch for explicitly enabling/disabling this stuff
- Added emission of 'literal' strings to the TOML serializer
- Added lots of minor documentation fixes and improvements
- Added Visual Studio debugger native visualizations for `table`, `array`, `parse_result`, and `parse_error` (#46) (@Reedbeta)
- Added non-template version of `array::is_homogeneous()`
- Added explicit instantiations of more template types when `!TOML_ALL_INLINE`

**Changes**

- ⚠&#xFE0F; deprecated `parse_result::get()` in favour of `parse_result::table()`
- ⚠&#xFE0F; deprecated `node_view::get()` in favour of `node_view::node()`
- ⚠&#xFE0F; simplified internal ABI namespaces
- Improved the quality of many static_assert error messages

**Removals**

- ⚠&#xFE0F; renamed `date_time::time_offset` to just 'offset'
- ⚠&#xFE0F; Removed `TOML_CHAR_8_STRINGS` since it no longer makes sense

<br><br>

## v1.3.3

**Fixes**

- Fixed some minor TOML spec conformance bugs
- Fixed BOM check causing EOF on very short iostream inputs
- Fixed `std::numeric_limits::max()` getting broken by macros in some environments
- Fixed 'unknown pragma' warning spam in older versions of GCC
- Fixed a few minor documentation issues

**Additions**

- Added rvalue overload of `array::flatten`
- Added conformance tests from `BurntSushi/toml-test` and `iarna/toml-spec-tests`
- Added `toml::inserter` as a workaround for nested construction of single-element `toml::arrays` performing move-construction instead
- Added license boilerplate to test files

**Changes**

- Refactored the parser to reduce binary size

<br><br>

## v1.3.2

**Fixes**

- Fixed single-digit negative integers parsing as positive
- Fixed parse failure when parsing an empty file
- Fixed multi-line strings being allowed in keys
- Fixed overflow for very long binary integer literals

**Changes**

- Improved the performance of toml::parse_file
- Improved the performance of printing to streams for deepy-nested TOML data

<br><br>

## v1.3.0

**Fixes**

- Fixed `formatter::print_inline()` causing compilation failures in DLL builds
- Fixed BOMs occasionally causing overflow/crash in char8 mode
- Fixed some spurious warnings in GCC 10
- Fixed clang static analyzer warning in BOM handling code

**Additions**

- Added `table_iterator::operator ->`
- Added `array::resize()` and `array::truncate()`
- Added `array::capacity()`, `array::shrink_to_fit()`, `array::max_size()`
- Added non-const -> const conversion for table and array iterators

**Changes**

- Renamed table iterator proxy pair members to `first` and `second` to match STL

<br><br>

## v1.2.5

**Fixes**

- Fixed some multi-line string parsing issues
- Fixed pedantic warnings on gcc 10 and clang 11
- Fixed `is_unicode_XXXXXX` functions being wrong in some cases
- Fixed `TOML_LIKELY` not being correct on older versions of gcc and clang
- Fixed minor documentation issues (#26, #38) (@prince-chrismc)

**Additions**

- Added additional error message cases to the parser
- Added `error_printer` example
- Added `toml_generator` example

**Changes**

- Improved unicode-related codegen

<br><br>

## v1.2.3

**Fixes**

- Fixed printing of inf and nan
- Fixed parser not handling floats with leading '.' characters
- Fixed pedantic vtable warnings on clang with -Weverything
- Fixed a number of documentation bugs
- Fixed `TOML_UNRELEASED_FEATURES` default being 1 (it should have been 0)

**Additions**

- Added `TOML_PARSER` configuration option
- Added `TOML_LIB_SINGLE_HEADER` indicator
- Added doxygen page for the configuration options
- Added SPDX-License-Identifiers around the place

**Changes**

- Split some header files up to make future maintenance easier
- Refactored and greatly simplified parser

<br><br>

## v1.2.0

**Fixes**

- Fixed some parsing and printing ops being locale-dependent
- Fixed some parsing errors at EOF when `TOML_EXCEPTIONS = 0`
- Fixed some unreferenced variable warnings on older compilers
- Fixed some 'maybe-uninitialized' false-positives on GCC9
- Fixed pkgconfig subdir being wrong

**Additions**

- Added support for implementations without `<charconv>`
- Added cmake package config generator (#22) (@GiulioRomualdi)
- Added build config feature option `GENERATE_CMAKE_CONFIG`
- Added many new tests

<br><br>

## v1.1.0

**Fixes**

- Fixed some parser error paths not returning early enough `TOML_EXCEPTIONS=0`
- Fixed a number of minor documentation issues

**Additions**

- Added support for [TOML 1.0.0-rc.1](https://github.com/toml-lang/toml/releases/tag/v1.0.0-rc.1) 🎉
- Added `operator[]`, `begin()`, `end()` to `toml::parse_result` for `TOML_EXCEPTIONS=0`
- Added additional compilation speed improvements for `TOML_ALL_INLINE=0`
- Added more specific error messages for parsing errors relating to prohibited codepoints
- Added a large number of additional tests
- Added support for installation with meson (#16) (@ximion)
- Added the array and table iterators to the `toml` namespace

<br><br>

## v1.0.0

**Fixes**

- Fixed minor documentation issues

**Changes**

- Refactoring of ABI-based inline namespaces

<br><br>

## v0.6.0

**Fixes**

- Fixed minor preprocessor/macro issues
- Fixed minor documentation issues

**Additions**

- Added `<cassert>` include directly in 'debug' builds when `TOML_ASSERT` isn't defined
- Added Clang's `[[trivial_abi]]` attribute to `date`, `time`, `time_offset`

<br><br>

## v0.5.0

**Fixes**

- Fixed crash when reaching EOF while parsing a string when exceptions are disabled
- Fixed some attribute warnings in GCC
- Fixed build with GCC 8.2.0 (#15) (@shdnx)
- Fixed exception mode detection sometimes being incorrect on MSVC
- Fixed compilation on older implementations without `std::launder`
- Fixed `json_formatter` type deduction on older compilers

**Additions**

- Added support for Unicode 13.0
- Added support for `\xHH` escape sequences ([toml/pull/796](https://github.com/toml-lang/toml/pull/796))
- Added short-form license preamble to all source files
- Added build configuration option for compiling examples

<br><br>

## v0.4.3

**Fixes**

- Fixed ICE in VS2019 when using `/std:c++17` instead of `/std:c++latest`

**Additions**

- Added `#error` when `TOML_EXCEPTIONS` is set to `1` but compiler exceptions were disabled

**Changes**

- Parsing performance improvements

<br><br>

## v0.4.0

**Fixes**

- Fixed `parse_file()` failing to compile with plain string literals
- Fixed tests being built when used as a meson subproject (#14) (@shdnx)

**Additions**

- Added support for compiling into DLLs on windows (`TOML_API`)
- Added support for explicitly setting the `TOML_EXCEPTION` mode
- Added `TOML_OPTIONAL_TYPE` customization point
- Added `node::ref()` and `node_view::ref()`

<br><br>

## v0.3.0

**Fixes**

- Fixed some pedantic clang warnings
- Fixed some minor documentation errors

**Additions**

- Added `node::value()` and `node::value_or()`
- Added `node_view::value()`
- Added relops for the date/time classes
- Added `TOML_ALL_INLINE` and `TOML_IMPLEMENTATION` options
- Added preliminary support for ICC

**Removals**

- Removed `<cmath>` dependency

<br><br>

## v0.2.1

**Fixes**

- Fixed minor printing bug in `operator<<(ostream, source_position)`
- Fixed minor documentation issues

**Additions**

- Added `operator<<(ostream&, parse_error)`

**Changes**

- Improved quality of error messages for boolean and inf/nan parsing

<br><br>

## v0.2.0

**Fixes**

- Fixed truncation of floating-point values when using ostreams
- Fixed missing value deduction guides for dates and times
- Fixed potential ODR issues relating to exception mode handling etc.
- Fixed some documentation issues

**Additions**

- Added serialization round-trip tests
- Added `node::is_number()`
- Added `node_view::is_number()`
- Added `node_view::value_or()`
- Added hexfloat parsing support for all implementations (not just `<charconv>` ones)

<br><br>

## v0.1.0

- First public release, yay! 🎉&#xFE0F;
