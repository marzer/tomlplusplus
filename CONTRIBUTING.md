# Contributing to toml++

Contributions are very welcome! Either by [reporting issues] or submitting pull requests.
If you wish to submit a PR, please be aware that:

- The single-header `toml.hpp` at the repository root is generated, so don't change it directly; make your changes in
  the files in `include`. You can leave it out of your PR too (see [Generated files](#generated-files)).
- Your changes should compile warning-free on at least one of:
    - GCC 8 or higher
    - Clang 8 or higher
    - MSVC 19.2X (Visual Studio 2019) or higher

If you've added a new header file that isn't going to be transitively included by one of the others, add an include
directive to `include/toml++/toml.hpp`.

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

Testing is done using [Catch2].

### Testing on Windows with Visual Studio

Install [Visual Studio] and [Test Adapter for Catch2], then open `toml++.sln` and build the
projects in the `tests` solution folder. Visual Studio's Test Explorer should pick these up and
allow you to run the tests directly.

If test discovery fails you can usually fix it by enabling
`Auto Detect runsettings Files` (settings gear icon > `Configure Run Settings`).

### Testing on Linux (and WSL)

```bash
# install ninja, meson, locales (first time only)
sudo apt update && sudo apt install -y locales python3 python3-pip ninja-build
sudo pip3 install meson
sudo locale-gen 'en_US.utf8' \
                'ja_JP.utf8' \
                'de_DE.utf8' \
                'it_IT.utf8' \
                'tr_TR.utf8' \
                'fi_FI.utf8' \
                'fr_FR.utf8' \
                'zh_CN.utf8'

# create the build configs (first time only)
meson setup build-debug   --buildtype=debug   -Ddevel=true
meson setup build-release --buildtype=release   -Ddevel=true

# run the tests
cd build-debug && ninja && ninja test               \
    && cd ../build-release && ninja && ninja test   \
    && cd ..
```

> ℹ&#xFE0F; Pass `-Duse_vendored_libs=false` to Meson if you wish to use the system-installed version
> of Catch2 (v3 or newer) rather than the vendored one.

<br>

## Testing with the [toml-test] suite

As an optional extra you may wish to test against the official test TOML test suite, [toml-test]. See the
instructions at [toml-test/README](./toml-test/README.md). Note that the toml++ tests already consume tests from the
offical suite via a C++ code-generation script so you are not expected to take this extra step as part of contributing
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

[visual studio]: https://visualstudio.microsoft.com/vs/
[test adapter for catch2]: https://marketplace.visualstudio.com/items?itemName=JohnnyHendriks.ext01
[reporting issues]: https://github.com/marzer/tomlplusplus/issues
[catch2]: https://github.com/catchorg/Catch2
[toml-test]: https://github.com/toml-lang/toml-test
