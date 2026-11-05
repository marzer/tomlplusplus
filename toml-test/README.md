# Encoder and Decoder for [toml-test]

> ℹ&#xFE0F; _The contents of this directory relate to testing with the [toml-test] suite, **not** anything to do with running the library's unit-tests.
> For information about running the unit-tests see [CONTRIBUTING](../CONTRIBUTING.md)._
>
> ℹ&#xFE0F; _All command snippets in this document assume the working directory is the toml++ repository root._

<br>

## Prerequisites

Install the `toml-test` runner, either from a release binary or with Go, as the [toml-test] README describes:

```bash
go install github.com/toml-lang/toml-test/v2/cmd/toml-test@v2.2.0
```

CI runs the same version against the TOML 1.1 tests, which is what `-toml 1.1` below selects.

<br>

## Building and Testing the Encoder and Decoder

### CMake

```bash
cmake -S . -B build-tt -DCMAKE_BUILD_TYPE=Release -Dtomlplusplus_BUILD_TOML_TEST=ON
cmake --build build-tt
toml-test test -toml 1.1 -decoder=build-tt/toml-test/tt_decoder -encoder=build-tt/toml-test/tt_encoder
```

With Visual Studio or another multi-config generator, build with `--config Release`; the executables are then in
`build-tt/toml-test/Release`.

### Meson

```bash
meson setup build-tt --buildtype=release -Dbuild_tt=true
meson compile -C build-tt
toml-test test -toml 1.1 -decoder=build-tt/toml-test/tt_decoder -encoder=build-tt/toml-test/tt_encoder
```

> ℹ&#xFE0F; Pass `-Dtomlplusplus_USE_VENDORED_LIBS=OFF` to CMake or `-Duse_vendored_libs=false` to Meson if you wish to
> use the system-installed version of nlohmann/json rather than the vendored one.
>
> ℹ&#xFE0F; With unreleased TOML features enabled, the encoder writes keys such as `€` without quotes, which TOML 1.1
> doesn't allow, so toml-test reports failures.

[toml-test]: https://github.com/toml-lang/toml-test
