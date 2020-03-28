![banner](docs/tomlplusplus-banner-small.png)  
[![C++](https://img.shields.io/badge/c%2B%2B-17%2C%2020-informational)][cpp_compilers]
[![TOML](https://img.shields.io/badge/TOML-v0.5.0-informational)][v0.5.0]
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)
[![Releases](https://img.shields.io/github/release/marzer/tomlplusplus.svg)](https://github.com/marzer/tomlplusplus/releases)
[![Mentioned in Awesome C++](https://awesome.re/mentioned-badge.svg)](https://github.com/fffaraz/awesome-cpp)
[![CircleCI](https://circleci.com/gh/marzer/tomlplusplus.svg?style=shield)](https://circleci.com/gh/marzer/tomlplusplus)
 ====
 - Header-only
 - [TOML v0.5.0](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md), plus optional support for some [unreleased TOML language features]
 - C++17 (plus some C++20 features where available, e.g. experimental support for char8_t strings)
 - Proper UTF-8 handling (incl. BOM)
 - Works with or without exceptions
 - Doesn't require RTTI
 - First-class support for serializing to JSON
 - Tested on Clang, GCC and MSVC (VS2019)
 - Tested on x64, x86 and ARM

<br>

# Example

Given a TOML file `configuration.toml` containing the following:
```toml
[library]
name = "toml++"
authors = ["Mark Gillard <mark@notarealwebsite.com>"]

[dependencies]
cpp = 17
```
Reading it in C++ is easy with `toml++`:
```cpp
auto config = toml::parse_file( "configuration.toml" );

// get key-value pairs
std::string_view library_name = config["library"]["name"].value_or(""sv);
std::string_view library_author = config["library"]["authors"][0].value_or(""sv);
int64_t depends_on_cpp_version = config["dependencies"]["cpp"].value_or(0);

// modify the data
config.insert_or_assign("alternatives", toml::array{
    "cpptoml",
    "toml11",
    "Boost.TOML"
});

// iterate & visit over the data
for (auto [k, v] : config)
{
    v.visit([](auto& node) noexcept
    {
        std::cout << node << std:endl;
        if constexpr (toml::is_string<decltype(node)>)
            do_something_with_string_values(node);
    });
}

// re-serialize as TOML
std::cout << config << std::endl;

// re-serialize as JSON
std::cout << toml::json_formatter{ config } << std::endl;


```
You'll find some more code examples in the `examples` directory, and plenty more as part of the [API documentation].

<br>

# Adding toml++ to your project
`toml++` comes in two flavours: Regular and Single-header.

### Regular mode
1. Add `tomlplusplus/include` to your include paths
2. `#include <toml++/toml.h>`

### Single-header mode
1. Drop `toml.hpp` wherever you like in your source tree
2. There is no step two

The API is the same regardless of how you consume the library. 

### Configuration
A number of configurable options are exposed in the form of preprocessor `#defines`. Most likely you
won't need to mess with these at all, but if you do, set them before including toml++.

| Option                     |      Type      | Default                           | Description                                                                                                |
|----------------------------|:--------------:|-----------------------------------|------------------------------------------------------------------------------------------------------------|
| `TOML_ALL_INLINE`          |     boolean    | `1`                               | Disable this to explicitly control where toml++'s implementation is compiled (e.g. as part of a library).  |
| `TOML_API`                 |     define     | undefined                         | API annotation to add to public symbols (e.g. `__declspec(dllexport)` on Windows).                         |
| `TOML_ASSERT(expr)`        | function macro | `assert(expr)`<br>(or undefined)  | Sets the assert function used by the library.                                                              |
| `TOML_CHAR_8_STRINGS`      |     boolean    | `0`                               | Uses C++20 [char8_t]-based strings as the toml string data type. **_Experimental!_**                       |
| `TOML_CONFIG_HEADER`       | string literal | undefined                         | Includes the given header file before the rest of the library.                                             |
| `TOML_EXCEPTIONS`          |     boolean    | per your compiler's settings      | Sets whether the library uses exceptions.                                                                  |
| `TOML_IMPLEMENTATION`      |     define     | undefined                         | Define this to enable compilation of the library's implementation. Meaningless if `TOML_ALL_INLINE` is `1`.|
| `TOML_LARGE_FILES`         |     boolean    | `0`                               | Uses 32-bit integers for line and column indices (instead of 16-bit).                                      |
| `TOML_OPTIONAL_TYPE`       |    type name   | undefined                         | Overrides the `optional<T>` type used by the library if you need [something better than std::optional].    |
| `TOML_SMALL_FLOAT_TYPE`    |    type name   | undefined                         | If your codebase has an additional 'small' float type (e.g. half-precision), this tells toml++ about it.   |
| `TOML_SMALL_INT_TYPE`      |    type name   | undefined                         | If your codebase has an additional 'small' integer type (e.g. 24-bits), this tells toml++ about it.        |
| `TOML_UNDEF_MACROS`        |     boolean    | `1`                               | `#undefs` the library's internal macros at the end of the header.                                          |
| `TOML_UNRELEASED_FEATURES` |     boolean    | `1`                               | Enables support for [unreleased TOML language features] not yet part of a [numbered version].              |

_A number of these have ABI implications; the library uses inline namespaces to prevent you from accidentally linking incompatible combinations together._

<br>

# TOML Language Support
At any given time `toml++` aims to implement whatever the [numbered version] of TOML is, with the
addition of unreleased features from the [TOML master] and some sane cherry-picks from the
[TOML issues list] where the discussion strongly indicates inclusion in a near-future release.

The library advertises the most recent numbered language version it fully supports via the preprocessor
defines `TOML_LANG_MAJOR`, `TOML_LANG_MINOR` and `TOML_LANG_PATCH`.

### **🔸Unreleased TOML features:**
- [#356]: Allow leading zeros in the exponent part of a float
- [#516]: Allow newlines and trailing commas in inline tables
- [#562]: Allow hex floatingpoint values
- [#567]: Clarify that control characters are not permitted in comments
- [#571]: Allow raw tabs inside strings
- [#644]: Support `+` in key names
- [#665]: Make arrays heterogeneous
- [#671]: Local time of day format should support `09:30` as opposed to `09:30:00`
- [#687]: Relax bare key restrictions to allow additional unicode characters
- [#709]: Include an \xHH escape code sequence

_These can be disabled (and thus strict [TOML v0.5.0] compliance enforced) by specifying
`TOML_UNRELEASED_FEATURES = 0` (see [Configuration](#Configuration))._

### **🔹TOML v0.5.0 and earlier:**
- All features supported.

<br>

# Contributing
Contributions are very welcome! Either by [reporting issues] or submitting pull requests.
If you wish to submit a pull request, please see [CONTRIBUTING] for all the details you need to get going.

<br>

# License and Attribution

`toml++` is licensed under the terms of the MIT license - see [LICENSE].

UTF-8 decoding is performed using a state machine based on Bjoern Hoehrmann's '[Flexible and Economical UTF-8 Decoder]'.

[API documentation]: https://marzer.github.io/tomlplusplus/
[unreleased TOML language features]: #unreleased-toml-features
[numbered version]: https://github.com/toml-lang/toml/releases
[char8_t]: https://en.cppreference.com/w/cpp/keyword/char8_t
[TOML master]: https://github.com/toml-lang/toml/blob/master/README.md
[TOML issues list]: https://github.com/toml-lang/toml/issues
[TOML v0.5.0]: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
[v0.5.0]: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
[CONTRIBUTING]: ./CONTRIBUTING.md
[LICENSE]: ./LICENSE
[Flexible and Economical UTF-8 Decoder]: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
[cpp_compilers]: https://en.cppreference.com/w/cpp/compiler_support
[reporting issues]: https://github.com/marzer/tomlplusplus/issues
[#356]: https://github.com/toml-lang/toml/issues/356
[#516]: https://github.com/toml-lang/toml/issues/516
[#562]: https://github.com/toml-lang/toml/issues/562
[#567]: https://github.com/toml-lang/toml/issues/567
[#571]: https://github.com/toml-lang/toml/issues/571
[#622]: https://github.com/toml-lang/toml/issues/622
[#644]: https://github.com/toml-lang/toml/issues/644
[#665]: https://github.com/toml-lang/toml/issues/665
[#671]: https://github.com/toml-lang/toml/issues/671
[#687]: https://github.com/toml-lang/toml/issues/687
[#709]: https://github.com/toml-lang/toml/pull/709
[LICENSE-utf8-decoder]: ./LICENSE-utf8-decoder
[something better than std::optional]: https://github.com/TartanLlama/optional
