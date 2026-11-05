# toml++

A TOML parser and serializer for C++17. It is header-only by default, and can also be built as a static or shared
library or imported as a C++20 module. It has a lot of users and ships in many package managers, so a change that
breaks a build or a behaviour reaches people who never read the changelog.

See @README.md for what the library does, and @CONTRIBUTING.md for building, testing, fuzzing and releases. This file
adds what an agent needs on top of them.

An explicit instruction from the person you are working for overrides this file. Where neither says anything, match
the code around you.

## Compatibility

v3 stays compatible until v4. An idea that would break someone goes on `docs/pages/v4.md`, for the maintainer to weigh.

- The public API, its behaviour, and the CMake and Meson interfaces don't break. When it isn't clear whether something
  is a user interface, ask.
- The shared library's SONAME carries only the major version, so the ABI holds as well: no layout changes, and no
  exported function removed or changed. A new exported function is fine. `symbols.txt` lists the exported names, and
  the `check-symbols` test compares a Linux build against it.
- `toml::path` keeps the semantics it had in v3.4.0, quirks included, and `tests/path.cpp` pins them. Fixes and
  cosmetic changes are fine.
- Every `#ifndef`-guarded config macro stays, and so do the ABI inline namespaces.
- C++17 is the minimum, with or without exceptions and RTTI. The library builds warning-free on GCC, Clang, Apple Clang
  and MSVC, and with CMake 3.14 and Meson 0.61.
- Header-only users pay for every include, so keep expensive standard headers such as `<filesystem>` and `<algorithm>`
  out of the library.

Packagers depend on details that look internal:

- CMake keeps the target `tomlplusplus::tomlplusplus` and its options. Meson keeps its option names, deprecated ones
  included.
- vcpkg builds with Meson and regex-patches the `IMPORTED_LOCATION` line in `cmake/tomlplusplusConfig.cmake.meson.in`,
  so that line keeps its shape.
- Conan copies the headers in `include/`, the root `toml.hpp` and `LICENSE`. The root `toml.hpp` stays committed.
- OSS-Fuzz builds with `fuzzing/build.sh` from `master`, so a change to it takes effect only after merge.

Releases are rare, because each one makes work for every packager. Leave `VERSION.txt` alone: changing it on `master`
publishes a release.

## Generated files

`toml.hpp` at the repository root is generated from the headers in `include/`, and the version numbers in
`include/toml++/impl/version.hpp`, the natvis file and the docs come from `VERSION.txt`:

- Change `include/`, never `toml.hpp`. CMake and Meson builds from a git checkout regenerate it, and so does the
  pre-commit hook. `python3 tools/regenerate.py` does it directly, and `--check` reports what is stale.
- The `#undef` block in `include/toml++/toml.hpp` is generated too. It undefines every macro the headers define,
  except the ones in `PUBLIC_MACROS` in `tools/regenerate.py`.
- `tests/conformance_*.cpp` come from `tools/generate_conformance_tests.py`. Don't edit them by hand.
- A new derived file goes through `tools/regenerate.py`, which stays standard-library-only. Nobody should have to
  remember to run a script.

## Code

clang-format decides formatting, through pre-commit. Never mix a reformat with a change in behaviour.

The library reads like the standard library: `snake_case` names, `PascalCase` template parameters, `TOML_` macros.
Return early and keep the happy path left-aligned. Member state goes at the top of a class. Zero-initialise with
`= {}`. No Hungarian notation.

- A new macro goes in `include/toml++/impl/preprocessor.hpp`. One that users may set also needs an `#ifndef` guard, a
  `\def` doc comment, a row in the README's configuration table and an entry in `PUBLIC_MACROS`.
- The public API carries Doxygen markup, including poxy's commands (`\cpp`, `\out`, `\detail` and so on).
- C library functions are called through `std::`, as in `std::memcpy` and `std::log10`. The `<c...>` headers only
  promise the `std::` names.
- Before writing a helper, look for one in `impl::`.
- The library has no dependencies, and the tests vendor theirs. Don't add one.

## Comments

Comments in code should be terse and to-the-point, documenting _why_, not _what_:

- Primarily lowercase (language snippets excepted)
- At most 1-2 lines, no paragraphs unless absolutely necessary
- If a comment adds no value, DO NOT LEAVE ONE.
- No LLM voice slop
- No over-embellishing
- No re-explaining things that are plainly obvious from the code itself
- No using things like backticks - code isn't markdown
- No using unicode (no em dashes, en dashes, arrows, or anything else non-ASCII)
- No excessive punctuation (language snippets excepted)
- Comments in unit-tests can often be folded into TEST_CASE or SECTION names
- Never comment the diff. A comment that only makes sense to somebody reading the pull request belongs in the pull
  request
- NO LLM VOICE SLOP

ANY COMMENTS THAT DO NOT MEET THE ABOVE CRITERIA SHOULD BE CONSIDERED A BUG AND AGGRESSIVELY FLAGGED DURING REVIEW.
Obviously-human comments like in-jokes, banners/headers and slang are fine.

Write the conclusion, never the investigation. The urge to comment peaks just after working something out the hard
way, and what comes out then is the reasoning. A comment earns its place only by stopping someone breaking the code, so
what was tried, the measurement that convinced you, an alternative nobody proposed and the bug just fixed all go. When
you clean up comments, delete first and tighten what is left; rephrasing alone is a failed pass.

## Tests

Catch2 v3, vendored. A new test file goes in `tests/sources.txt`, which CMake and Meson both read.

- A test checks behaviour that the public API, the TOML spec or a documented guarantee promises. Calling code is not
  testing it.
- A test that cannot fail is worse than none. Break the code it covers and watch it fail before trusting it.
- Negative parse tests state the error's position and description, as `CONTRIBUTING.md` shows. Parsing tests check
  values, types and, where the parser sets them, source regions. Formatter tests compare the whole output and parse it
  back.
- `impl::` details get tested only when no public path reaches them.
- Every fix gets a regression test.
- The suite runs once per locale, and CI also builds it with exceptions off and unreleased TOML features on. A test
  holds in all of them, with `TOML_LANG_UNRELEASED` blocks where an expectation differs.
- CI's coverage floors only go up.

## Review priorities

The parser reads untrusted input, and OSS-Fuzz fuzzes it continuously. Look hardest for:

- undefined behaviour, out-of-bounds reads and unbounded recursion
- conversions that overflow or lose range, in integers and floats alike
- behaviour that differs between libstdc++, libc++ and the MSVC STL, between compilers, or with exceptions off
- anything that depends on the global locale
- exception safety in the node types

## Verification

CI is the full check, and it runs on every push: GCC, Clang, Apple Clang and MSVC, sanitizers, libc++, coverage,
clang-tidy, the oldest supported CMake and Meson, and the installed package. Don't rebuild that matrix locally. Hand the
change over once the quick checks pass, work from what CI reports, and call CI green only when the passing run is for
HEAD.

The quick checks, scaled to what the change can break (a comment or docs edit needs none):

- a build with `-Dtomlplusplus_PEDANTIC=ON -Dtomlplusplus_WERROR=ON` and the tests, with the compiler at hand
- `python3 tools/regenerate.py --check`
- `pre-commit run --files` over the files you touched, since a run over every file can have clang-format rewrite one
  that someone else is editing

Run something slower only for what CI doesn't cover, such as MinGW or a packager's build, and only when the change
touches it.

A green `ctest` after a failed build is the previous binary passing. Read the build's exit status first.

Keep build directories and scratch files outside the repository. An instrumented Meson setup writes `default.profraw`
to the current directory. Run heavy work under `nice -n 19`, in bounded steps, and report progress while it runs.

Measure performance; never guess. A measurement goes in the report, and the conclusion goes in the code or the docs.

## CI

The job matrix only shrinks. New coverage goes into a job that already exists, and an untested combination is something
to report. Jobs run on the `-latest` runner images. A job's name is a word or two saying what sets it apart, and the
workflow holds the rest of its configuration. Actions stay on version tags, which dependabot updates. No CodeQL.

## Changelog

A user-visible change gets one line under `## Unreleased` in `CHANGELOG.md`. A fix reads "fixed X when doing Y", as the
user meets the bug, without how it worked or what the code does now. Credit the issue and the person as
`(#123) (@name)`, and merge fixes of the same kind into one line. A change nobody using or packaging the library can see
gets no entry.

## Source control

- Commit or push only when the person you are working for asks.
- History stays linear: no merge commits.
- A commit message is one short line.
- No AI attribution: no `Co-Authored-By` trailer, no "Generated with" line, no credit in the README.
- A contributor's pull request landed by hand keeps the contributor as author and committer. The message is the pull
  request's title with ` (#123)`, plus `Fixes #456` when it fixes an issue.

## Writing

This covers docs, comments, changelog entries, commit messages and pull requests.

Neutral and terse, in British English and ASCII. Keep paragraphs short and inline backticks few, with one exception.
Prettier formats the Markdown outside `docs/`, `CHANGELOG.md` and the issue templates, and it turns an asterisk, or an
underscore at the start or end of a word, into emphasis. Backtick any token like that, such as `count * 8` or
`__init__`.

LLM voice has named shapes. Cut them:

- the contrast reflex in descriptive prose: "not X but Y", "X, not Y", "X rather than Y". A rule that says "do X, not
  Y" is fine.
- bold-lead bullets, trailing participle clauses ("..., ensuring that"), "serves as" for "is", and nominalisations
  ("perform an analysis")
- metaphor standing in for a plain verb: "what X buys", "where X lives", "earns its keep"
- the tidy closer ("In summary") and a heading that summarises an argument. A heading names a place.

Documents describe the present. No dates, no "used to" or "now does", and no done-marks: a finished item is deleted.
Fix a stale sentence in place, and never append a correction below it. Don't list what the tree can list, and check
every figure you write. Name a role, such as the maintainer or the user, and never a person.

## Working

- Do what was asked. Point out fragile code nearby, and leave it for someone to decide.
- A question gets an answer. It doesn't start the work behind it.
- Ask nothing the work already answers: take the defensible default and say which.
- Edit files with an editing tool. Shell redirection overwrites a file without seeing it.
- After a bug hunt, add whatever would have found it faster: an assert, a test, a clearer error message.
- After a batch of work, review your own changes for LLM voice, comment spam, needless helpers and throwaway code.
