# Contributing to toml++

Contributions are very welcome! Either by [reporting issues] or submitting pull requests.
If you wish to submit a PR, please be aware that:

- The single-header `toml.hpp` at the repository root is generated, so don't change it directly; make your changes in
  the files in `include`. You can leave it out of your PR too (see [Generated files](#generated-files)).
- Your changes should compile warning-free with a recent GCC, Clang or MSVC. CI builds every PR with all three, and
  with Apple Clang.

If you've added a new header file that isn't going to be transitively included by one of the others, add an include
directive to `include/toml++/toml.hpp`.

<br>

## Code style

For the most part the library tries to look like idiomatic standard-library-style C++, so between that and using `pre-commit`/`clang-format`, there's not much to think about.

One thing to be very aware of is compile times - the library is primarly a header-only project, so
many choices are made to avoid including certain expensive standard-library headers (e.g. `<filesystem>`, `<algorithm>`).

<br>

## Generated files

`toml.hpp` and a few other files are derived from `include` and `VERSION.txt`. You shouldn't need to do anything to keep
them up to date:

- Building from a git checkout with CMake or Meson regenerates them. Only files that actually changed get written, so
  this doesn't cause rebuilds on its own.
- `pre-commit install` adds a hook that regenerates them when you commit.
- After a merge, CI commits anything still out of date to `master`.

If you need it, `python3 tools/regenerate.py` does the same by hand, and `--check` reports what's out of date without
changing anything. It only needs Python.

<br>

## Building and running the tests

Testing is done using [Catch2]. It's vendored, as is everything else the tests use, so a compiler and CMake or Meson
are all you need.

### CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -Dtomlplusplus_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

With Visual Studio or another multi-config generator, pass `--config Debug` to the build and `-C Debug` to `ctest`.

Set the language standard with `CMAKE_CXX_STANDARD`. The other options (examples, sanitizers, the compiled library,
unreleased TOML features and so on) are in `cmake/variables.cmake`, and `.github/workflows/ci.yaml` shows the
combinations CI tests.

### Meson

```bash
meson setup build --buildtype=debug -Ddevel=true
meson test -C build
```

`-Ddevel=true` builds the tests, the examples and the toml-test encoder and decoder, with all warnings on. Meson 0.61
or later works; `pipx install meson` gets the latest.

> ℹ&#xFE0F; Pass `-Duse_vendored_libs=false` to Meson if you wish to use the system-installed version
> of Catch2 (v3 or newer) rather than the vendored one.

### Locales

The tests run once per locale, and a run whose locale isn't installed is reported as skipped. On Debian and Ubuntu,
`sudo apt install locales-all` installs all of them.

### Writing tests

A test for a document that must fail to parse states the error it expects, as `line:column: description`:

```cpp
parsing_should_fail(FILE_LINE_ARGS, "int = 0o8"sv, "1:9: Error while parsing octal integer: expected octal digit, saw '8'"sv);
```

When the expectation is wrong, the test prints the error it saw, so a first run can supply one. Read it against the
input before keeping it. Where a build option changes the error, put each expectation under the option, as the
`TOML_LANG_UNRELEASED` blocks in the parsing tests do.

The conformance tests generated from toml-test call `parsing_should_fail` without an expectation. Hand-written tests
always give one.

### Coverage

Coverage needs Clang, plus the `llvm-profdata` and `llvm-cov` of the same version on `PATH`:

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug \
    -Dtomlplusplus_BUILD_TESTS=ON -Dtomlplusplus_COVERAGE=ON
cmake --build build
ctest --test-dir build
python3 tools/coverage.py build
```

The HTML report goes in `build/coverage/html`. The report for `master` is on the
[homepage](https://marzer.github.io/tomlplusplus/coverage/).

CI also builds the tests with `-Dtomlplusplus_EXCEPTIONS=OFF -Dtomlplusplus_UNRELEASED_FEATURES=ON`, and measures the two
builds together, so that code compiled out of one build still counts. Give `coverage.py` several build directories to
merge them the same way.

<br>

## Fuzzing

[OSS-Fuzz] fuzzes toml++ continuously, and CIFuzz runs the same fuzzer on every PR. Both build
`fuzzing/toml_fuzzer.cpp` with `fuzzing/build.sh`. OSS-Fuzz takes `build.sh` from `master`, so a change to it only takes
effect after merge.

To run the fuzzer locally with Clang and libFuzzer:

```bash
CXX=clang++ CXXFLAGS="-fsanitize=address,fuzzer-no-link" LIB_FUZZING_ENGINE=-fsanitize=fuzzer \
    cmake -S . -B build-fuzz -DCMAKE_BUILD_TYPE=Release -DBUILD_FUZZER=ON
cmake --build build-fuzz --target toml_fuzzer
build-fuzz/fuzzing/toml_fuzzer
```

CMake warns that it can't install the fuzzer without `$OUT`. That only matters to OSS-Fuzz.

<br>

## Testing with the [toml-test] suite

As an optional extra you may wish to test against the official TOML test suite, [toml-test]. See the
instructions at [toml-test/README](./toml-test/README.md). Note that the toml++ tests already consume tests from the
official suite via a C++ code-generation script so you are not expected to take this extra step as part of contributing
to the library.

<br>

## Versioning and releases

The version is stored in one place: `VERSION.txt` at the repository root, a bare `major.minor.patch` triplet. Everything
that restates it (`version.hpp`, `toml.hpp`, the natvis file and the install snippets in the docs) is one of the
generated files above. Changing the version also renames the Unreleased heading in `CHANGELOG.md` and starts a new one.

Releases are automated and maintainer-only. Bumping `VERSION.txt` on `master` cuts a release: once CI passes, it creates
the `v<version>` tag and a GitHub release with `toml.hpp` attached.

The practical upshot for a PR: **don't touch `VERSION.txt`**. I bump it myself when cutting a release, so a PR that also
bumps it just invites a merge conflict or an accidental release. User-visible changes go under the Unreleased heading at
the top of `CHANGELOG.md`.

[reporting issues]: https://github.com/marzer/tomlplusplus/issues
[catch2]: https://github.com/catchorg/Catch2
[toml-test]: https://github.com/toml-lang/toml-test
[oss-fuzz]: https://github.com/google/oss-fuzz
