# toml++ (tomlplusplus)
[![c++version](https://img.shields.io/badge/c%2B%2B-17%2C%2020-informational)][cpp_compilers]
[![tomlversion](https://img.shields.io/badge/TOML-v0.5.0-informational)][v0.5.0]
[![CircleCI](https://circleci.com/gh/marzer/tomlplusplus.svg?style=shield)](https://circleci.com/gh/marzer/tomlplusplus)
[![GitHub](https://img.shields.io/github/license/marzer/tomlplusplus)](https://github.com/marzer/tomlplusplus/blob/master/LICENSE)  

`toml++` is a header-only toml parser and serializer for C++17, C++20 and whatever comes after.

<br>

# Example

Given a TOML file `configuration.toml` containing the following:
```toml
[library]
name = "toml++"
version = "0.1.0"
authors = ["Mark Gillard <mark@notarealwebsite.com>"]

[dependencies]
cpp = 17
```
Reading it in C++ is easy with `toml++`:
```cpp
auto config = toml::parse_file( "configuration.toml" );

// get key-value pairs
std::string_view library_name = config["library"]["name"].as_string()->get();
std::string_view library_version = config["library"]["version"].as_string()->get();
std::string_view library_author = config["library"]["authors"][0].as_string()->get();
int64_t depends_on_cpp_version = config["dependencies"]["cpp"].as_integer()->get();

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
won't need to mess with these at all, butif you do, set them before including toml++.

| Option                     |      Type      | Default                           | Description                                                                                              |
|----------------------------|:--------------:|-----------------------------------|----------------------------------------------------------------------------------------------------------|
| `TOML_ASSERT(expr)`        | function macro | `assert(expr)`<br>(or undefined)  | Sets the assert function used by the library.                                                            |
| `TOML_CHAR_8_STRINGS`      |     boolean    | `0`                               | Uses C++20 [char8_t]-based strings as the toml string data type.                                         |
| `TOML_CONFIG_HEADER`       | string literal | undefined                         | Includes the given header file before the rest of the library.                                           |
| `TOML_LARGE_FILES`         |     boolean    | `0`                               | Uses 32-bit integers for line and column indices (instead of 16-bit).                                    |
| `TOML_SMALL_FLOAT_TYPE`    |    type name   | undefined                         | If your codebase has an additional 'small' float type (e.g. half-precision), this tells toml++ about it. |
| `TOML_SMALL_INT_TYPE`      |    type name   | undefined                         | If your codebase has an additional 'small' integer type (e.g. 24-bits), this tells toml++ about it.      |
| `TOML_UNDEF_MACROS`        |     boolean    | `1`                               | `#undefs` the library's internal macros at the end of the header.                                        |
| `TOML_UNRELEASED_FEATURES` |     boolean    | `1`                               | Enables support for [unreleased TOML language features] not yet part of a [numbered version].            |

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

_These can be disabled (and thus strict [TOML v0.5.0] compliance enforced) by specifying
`TOML_UNRELEASED_FEATURES = 0` (see [Configuration](#Configuration))._

### **🔹TOML v0.5.0 and earlier:**
- All features supported.

<br>

# Contributing
Contributions are welcome, either by [reporting issues](https://github.com/marzer/tomlplusplus/issues)
or submitting pull requests. If you wish to submit a PR, please be aware that:
- The single-header file `toml.hpp` is generated by a script; make your changes in the files in
    `include`, **not** in `toml.hpp`.
- Your changes should compile warning-free on at least one of gcc 8.3.0, clang 8.0, and MSVC 19.2X
    (Visual Studio 2019). All three is a bonus.
- You should regenerate the single-header file as part of your PR (a CI check will fail if you don't).

### Regenerating toml.hpp
1. Make your changes as necessary
2. If you've added a new header file that isn't going to be transitively included by one of the
    others, add an include directive to `include/toml++/toml.h`
3. Run `python/generate_single_header.py`

### Building and testing
Testing is done using [Catch2], included in the respository as a submodule under `extern/Catch2`.
The first time you want to begin testing you'll need to ensure submodules have been fetched:  
```bash
git submodule update --init --recursive extern/Catch2
```

#### Windows

Install [Visual Studio 2019] and [Test Adapter for Catch2], then open `vs/toml++.sln` and build the
projects in the `tests` solution folder. Visual Studio's Test Explorer should pick these up and
allow you to run the tests directly.

If test discovery fails you can usually fix it by clicking enabling
`Auto Detect runsettings Files` (settings gear icon > `Configure Run Settings`).

#### Linux
Install [meson] and [ninja] if necessary, then test with both gcc and clang:
```bash
CXX=g++ meson build-gcc
CXX=clang++ meson build-clang
cd build-gcc && ninja && ninja test
cd ../build-clang && ninja && ninja test
```

<br>

# License and Attribution

`toml++` is licensed under the terms of the MIT license - see [LICENSE].

UTF-8 decoding is performed using a state machine based on Bjoern Hoehrmann's '[Flexible and Economical UTF-8 Decoder]',
which is also subject to the terms of the MIT license - see [LICENSE-utf8-decoder].

[API documentation]: https://marzer.github.io/tomlplusplus/
[unreleased TOML language features]: https://github.com/marzer/tomlplusplus#unreleased-features
[numbered version]: https://github.com/toml-lang/toml/releases
[char8_t]: https://en.cppreference.com/w/cpp/keyword/char8_t
[TOML master]: https://github.com/toml-lang/toml/blob/master/README.md
[TOML issues list]: https://github.com/toml-lang/toml/issues
[TOML v0.5.0]: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
[v0.5.0]: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
[LICENSE]: https://github.com/marzer/tomlplusplus/blob/master/LICENSE
[Flexible and Economical UTF-8 Decoder]: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
[meson]: https://mesonbuild.com/Getting-meson.html
[ninja]: https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages
[Catch2]: https://github.com/catchorg/Catch2
[Test Adapter for Catch2]: https://marketplace.visualstudio.com/items?itemName=JohnnyHendriks.ext01
[Visual Studio 2019]: https://visualstudio.microsoft.com/vs/
[cpp_compilers]: https://en.cppreference.com/w/cpp/compiler_support
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
[LICENSE-utf8-decoder]: https://github.com/marzer/tomlplusplus/blob/master/LICENSE-utf8-decoder
