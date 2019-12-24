# toml++ (tomlplusplus)
Header-only TOML parsing lib for C++17 and later.

# Usage
Single-header libraries are the hip thing at the moment but are pretty unergonomic to develop. Multi-header libraries are easier to develop but can't always be dropped in your source tree with ease. `toml++` provides both flavours.

#### 'Single-header' mode
1. Drop `toml.hpp` somewhere in your source tree
2. There is no step two

#### Regular mode
1. Add the `include` directory to your include paths
2. Include `toml++/toml.h`

# Configuration
A number of configurable options are exposed in the form of preprocessor macros. Most likely you won't need to mess
with these at all, but in the event you do, set your overrides prior to including toml++.

| Option                         |      Type      | Default                                    | Description                                                                                                                                                                                     |
|--------------------------------|:--------------:|--------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `TOML_CONFIG_HEADER`           | string literal | undefined                                  | Includes the given header file before the rest of toml++.                                                                                                                                       |
| `TOML_ASSERT(expr)`            | function macro | `assert(expr)` (or undefined)              | Sets the assert function used by the library.                                                                                                                                                   |
| `TOML_STRICT`                  |     boolean    | `0`                                        | Disables support for language features not yet released in a [numbered version](https://github.com/toml-lang/toml/releases).                                                                    |
| `TOML_USE_CHAR_8_IF_AVAILABLE` |     boolean    | `0`                                        | Uses [char8_t](https://en.cppreference.com/w/cpp/keyword/char8_t)-based strings if supported by your compiler and standard library. `TOML_CHAR_8` will be set to `1` if support is detected. |

# Contributing
TBA.

# TOML Language Support
At any given time `toml++` aims to implement whatever the [latest numbered TOML version](https://github.com/toml-lang/toml/releases) is, with the addition
of unreleased features from the [TOML master](https://github.com/toml-lang/toml/blob/master/README.md) and some sane cherry-picks from their
[issues list](https://github.com/toml-lang/toml/issues) where the discussion strongly indicates inclusion in a near-future release.

The library advertises the most recent numbered language version it fully supports via the preprocessor defines `TOML_LANG_MAJOR`, `TOML_LANG_MINOR` and `TOML_LANG_REVISION`.

### **🔸Unreleased features:**
- [#356](https://github.com/toml-lang/toml/issues/356): Allow leading zeros in the exponent part of a float
- [#516](https://github.com/toml-lang/toml/issues/516): Allow newlines and trailing commas in inline tables
- [#562](https://github.com/toml-lang/toml/issues/562): Allow hex floatingpoint values
- [#567](https://github.com/toml-lang/toml/issues/567): Clarify that control characters are not permitted in comments
- [#571](https://github.com/toml-lang/toml/issues/571): Allow raw tabs inside strings
- [#622](https://github.com/toml-lang/toml/issues/622): Add short escaping alias `\s` for space (`\u0020`)
- [#665](https://github.com/toml-lang/toml/issues/665): Make arrays heterogeneous
- [#671](https://github.com/toml-lang/toml/issues/671): Local time of day format should support `09:30` as opposed to `09:30:00`
- [#687](https://github.com/toml-lang/toml/issues/687): Relax bare key restrictions to allow additional unicode characters

_These can be disabled (and thus strict TOML [v0.5.0](https://github.com/toml-lang/toml/tree/v0.5.0) compliance enforced) by specifying `TOML_STRICT = 1` (see [Configuration](#Configuration))._

### **🔹[v0.5.0](https://github.com/toml-lang/toml/releases/tag/v0.5.0) and earlier:**
- All features as of `<< release date >>`.

