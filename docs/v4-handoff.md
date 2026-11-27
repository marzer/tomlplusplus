# toml++ v4: plan and handoff

This file carries the v4 plan, and everything behind it, from one machine to another. A session that has read this
file and `AGENTS.md` needs nothing else to continue. It is a working document: delete it once the plan has a
permanent home.

Line references are to the tree at `efd80e0` (v3.5.1) and will drift.

## Start here

State:

- v3.5.1 is released. No v4 code exists.
- The plan below was built from the earlier plan, `docs/pages/v4.md`, the code, three design studies and an
  interview with the maintainer. A cold review of the plan was folded in.
- The maintainer gave every decision under "Decisions". The plan as a whole is not approved: the batch order and
  the proposals under "Ground rules" have not been put to them.
- `docs/pages/v4.md` still shows the 39 undecided ideas.

Next steps, in order:

1. Read `AGENTS.md`. Its Compatibility section describes v3. For v4 work this file wins where the two differ, until
   batch 0 updates `AGENTS.md`.
2. Tell the maintainer two things the interview did not spell out:
   - CI builds only one of the four compiler floors (batch 1).
   - Real key-node pairs make `for (auto [k, v] : tbl)` copy every node, where v3 gives references (batch 5).
3. Ask the maintainer, one question at a time: the proposals under "Proposed for v4"; the batch order; where the
   plan is kept from now on. Until they say otherwise, this file is the plan of record.
4. Rewrite `docs/pages/v4.md` as "The v4 page" describes.
5. List batch 0's stages and wait for the word to start it.

If the session has a memory, save the rules under "Working with the maintainer" to it.

## What v4 is

- A node is a value. `toml::node` holds a union, and tables store real key-node pairs in insertion order.
- One library serves code built with and without exceptions.
- Output goes through a sink, so it needs no iostreams and works with `std::format` and `fmt::format`.
- Paths are written in TOML's own key syntax.
- New: an event interface (#107) and conversions for user types (#175).
- Gone: the virtual node hierarchy, the old-compiler workarounds, every v2 and v3 compatibility name.

## Decisions

Direction:

| | Decision |
|---|---|
| D1 | C++17 stays the minimum. |
| D2 | Compilers: GCC 11, Clang 14, VS 2022, Xcode 15. Older ones get no workarounds and no fixes. |
| D3 | Non-ASCII bare keys go. Hex floats and `+` in bare keys stay behind `TOML_ENABLE_UNRELEASED_FEATURES`. |
| D4 | Hard break for old names: a renamed or removed name stops working in 4.0.0, with an `#error` or a build error that names the new one where that is possible. No deprecated aliases, so nothing is left to clean up in v5. |
| D5 | `toml::node` becomes one value type holding a union. `table`, `array` and `value<T>` stop deriving from it. |
| D6 | v3 code keeps compiling where the union allows: `table`, `array`, `value<T>`, `node_view` and `key` keep their names and members. Only what the union forces changes. |
| D7 | Tables iterate and print in insertion order. Sorted output is a format flag. `lower_bound()` loses its meaning. |
| D8 | Exceptions are unified: a never-throwing entry point (working name `try_parse`) is exported and returns `parse_result` for every caller. `parse()` is a header-inline wrapper that returns a table and throws, and exists only where the compiler has exceptions. Without exceptions, callers rename `parse()` to `try_parse()`. |
| D9 | 4.0.0 includes custom types (#175), `std::format` (#325) and events (#107). Reflection (#324) is not in 4.0.0. |
| D10 | One formatting implementation serves `std::format` and `fmt::format`, if possible. It builds on the discussion in #327, and uses forward declarations wherever they are ODR-safe, so nobody gets `<format>` or fmt included for them. |
| D11 | Paths use TOML's key syntax plus `[n]`. |

Library entries:

| | Decision |
|---|---|
| L1 | `path::subpath(const_iterator, const_iterator)` becomes static (#309). |
| L2 | Offset date-times compare as instants and order by instant. A local date-time never equals an offset one, and sorts before it. |
| L3 | Assigning a float to an integer value is a compile error. |
| L4 | `toml::value{ x }` rejects what insertion rejects: `uint64_t`, the 128-bit integers, `long double`. |
| L5 | `value_flags` gets one bit per integer format. Only the enumerators' values change. |
| L6 | A table's source region spans its contents, and an array of tables spans its elements. Both still begin at the header. |
| L7 | The `TOML_DISABLE_` macros become `TOML_ENABLE_` with inverted defaults; `TOML_INT_CHARCONV` and `force_multiline_arrays` go. The legacy-lambda workaround is deleted if it proves unneeded. |
| L8 | `TOML_ASSERT` stays off under `NDEBUG`. The config macros move from the README table to a docs page, with the missing ones added. `TOML_CALLCONV` covers `impl::narrow()` and `impl::widen()`. |
| L9 | `noexcept` is dropped from the constructors of `table`, `array`, `parse_error` and `value<std::string>`'s copy, whatever the new storage does. |
| L10 | `_Float16` is detected on GCC 13+ and Clang; `TOML_ENABLE_FLOAT16` still overrides. |
| L11 | Template machinery: `TOML_CONSTRAINED_TEMPLATE` and friends stay as they are. New node code uses them. |
| L12 | Internal macros get a full pass: delete, apply, align with the maintainer's building-blocks library. |
| L13 | Lifetime annotations are added, under that library's macro names. |
| L14 | The five old macro spellings become `#error`s. The traps from 2.0 and 3.0 are deleted. |
| L15 | `toml++/toml.h` becomes an `#error` that names `toml.hpp`. The stub goes in v5. |

Build and repository:

| | Decision |
|---|---|
| B1 | CMake's minimum stays 3.14. |
| B2 | Every CMake option becomes `TOMLPLUSPLUS_*`; an old name is a fatal error that names the new one. `BUILD_FUZZER` keeps working until master's `fuzzing/build.sh` stops passing it. |
| B3 | Header-only CMake installs put the package files in `share/cmake/tomlplusplus`. Compiled installs stay in `lib/`. |
| B4 | CPack goes. |
| B5 | Meson's `ConfigVersion` file accepts the same major version only. |
| B6 | Meson stops installing the examples. |
| B7 | Meson's `default_library=shared` line goes. |
| B8 | Meson's `compile_library` and `float16` options go. |
| B9 | Tipi.build stays. |

Four answers came with the maintainer's own words:

- On D9 and D10: "note that there is an open pr for std::format support, #325, with some relevant discussion. I
  also want to have it _also_ transparently support fmt::format, the one solution should work in both formatting
  libraries if possible. _aaaaand_ care must be taken during this endeavour to make good use of forward
  declarations (assuming ODR safety) to avoid #including things for people that do not want it!" The pull request
  is #327; #325 is its issue.
- On L7: "rename/remove, but also the conditional lambda bug is a workaround for an old msvc that may actually be
  below our new compiler floor". Batch 1 says what was found.
- On L8: "a) asserts are unconditionally disabled during NDEBUG because of the function purity macros (which are
  only applied during NDEBUG, no-ops otherwise). purity and assertions don't mix. b) yes the macros need to be
  documented better. the big table in the README can go, i think, probably to a dedicated docs page, and missing
  ones added. c) ok sure"
- On L13: "add them; cppbb has good macros for this with names that i would like to bring over".

Where the maintainer chose against the recommendation, which says something about their taste: D3 (the
recommendation was to remove all four unreleased features), D4 (deprecated aliases), D5 (drop the virtual functions
and add in-place storage only if measured), L8's `TOML_ASSERT` (honour it under `NDEBUG`), L9 (keep `noexcept`
where the new storage makes it true), L11 (rewrite with C++17 idioms), B1 (CMake 3.22) and B9 (remove Tipi).

## Ground rules

### Kept from v3

`AGENTS.md` has most of them: C++17 with or without exceptions and RTTI, warning-free on four compilers, no
dependencies, no expensive standard headers, the CI matrix only shrinks, the coverage floors only go up, every fix
gets a regression test, CI is the full check. These were only in the earlier plan:

- New tests get a cold review by a fresh agent before handover.
- A line no public API can reach is a finding to report. Dead code goes; defensive code stays and gets listed.
- Fuzz runs inside a batch are short checks of a few minutes. A batch ends with one thorough check.
- A combination CI doesn't build is a risk to name in the handover, for the maintainer to decide. It is no reason
  to build it locally.
- Containers and packaging checks run only when the maintainer asks for one by name, or to diagnose a CI failure.
- The docs build with poxy and Doxygen 1.14.0.

### Proposed for v4

None of these was put to the maintainer. They need a yes before they bind.

- `master` is v3. It takes fixes, and one docs change: the v4 page as rewritten from this plan. v4 work is on a
  `v4` branch, which is rebased onto `master` when `master` moves. At release `master` fast-forwards to `v4`, and a
  `v3` branch marks the last v3 commit.
- 4.0.0 carries every breaking change. Additive work that is late moves to 4.x; nothing breaking does.
- Every break that compiles and changes behaviour is named in the upgrade guide. Known so far: insertion order,
  loops that bind a table's pair by value, date-time comparison, table regions, references into containers, path
  strings, the `value_flags` bit test, documents with non-ASCII bare keys, and a stream's exception arriving as a
  parse error.
- Each breaking change gets its changelog line and its upgrade-guide entry, with before and after code, in the
  change that makes it. On `v4`, the v4 page entry is deleted in the same change.
- `symbols.txt` changes freely until 4.0.0, and each batch updates it. After 4.0.0 the v3 rules apply to v4: the ABI
  holds for the whole major version.
- A test that pinned v3 behaviour changes in the same stage as the behaviour.
- A new public name joins the module's export list (`src/modules/tomlplusplus.cppm`) in the change that adds it.
- A baseline from v3.5.1 is recorded once, in batch 0. A benchmark gate compares the same rows, built the same way
  and run as `CONTRIBUTING.md` says, with the machine's load read before and after. It passes within 1%. A larger
  loss goes to the maintainer with the numbers, and nobody chases a difference of 1 to 2%.
- OSS-Fuzz and CIFuzz do not see pushes to `v4`. So each batch that touches the parser, the nodes, the paths or the
  output ends with a long local fuzz run, as well as the cold review of its diff.

### Contracts with packagers

v4 keeps these:

- the target `tomlplusplus::tomlplusplus`
- the committed root `toml.hpp`, the root `CHANGELOG.md` (poxy reads it), the branch `master`, tags `vX.Y.Z`, the
  raw `toml.hpp` URL and the docs URL
- the shape of the `IMPORTED_LOCATION` line in `cmake/tomlplusplusConfig.cmake.meson.in`, which vcpkg regex-patches
- what Conan copies: the headers under `include/`, the root `toml.hpp` and `LICENSE`
- CMake 3.14 and Meson 0.61
- the Meson option names, except the two B8 removes
- OSS-Fuzz builds every pull request with master's `fuzzing/build.sh`, so a change to that script takes effect
  only once it is on `master`

v4 drops these v3 contracts: the CMake option names (B2), the ABI and `symbols.txt` until 4.0.0, the inline
namespaces for the exception setting (D8), and "every config macro stays" (L7, L14).

## Order

| | Batch | Size | Needs |
|---|---|---|---|
| 0 | Groundwork | S | |
| 1 | Floors and dead weight | M | 0 |
| 2 | Build systems | M | 0 |
| 3 | Exceptions | L | 0 |
| 4 | Parser seam | M | 3 |
| 5 | Node model | XL | 4 |
| 6 | Output | L | 5 |
| 7 | Paths | M | 5 |
| 8 | Events | M | 4, 5 |
| 9 | Conversions | M | 5 |
| 10 | Release | M | all |

The build batch is early so that packagers can try the final build interface long before the release. The parser
seam precedes the node model so that the storage change touches one builder class and no parsing code. Batches 6
to 9 can run in any order.

Each batch opens with a short interview on its "To ask" list, and gets a plan of its own when it starts.

## 0. Groundwork (S)

1. Tooling for the version change: `regenerate.py` derives the namespace in `symbols.txt`, as it does for the
   natvis file. The changelog gets its `## Unreleased` heading back, with a group for breaking changes.
2. `AGENTS.md` on `v4`: the Compatibility section says what v4 may break and which contracts remain.
3. Upgrade guide: a new docs page, "Upgrading from v3", linked from the main page. It starts empty.
4. Benchmark rows the later batches need: lookup, iteration, copy and erase; a 100,000-key table in random order
   and one built to collide; strings and non-ASCII text in all three output formats; an allocation byte counter
   (the benchmark counts allocations only, `benchmarks/benchmarks.cpp:38-47`).
5. Baseline, taken with those rows at v3.5.1's library code and kept, with the script that made it, in a scratch
   directory that survives a reboot: every benchmark row, allocation counts and bytes, binary size header-only and
   compiled, compile time and debug-info size of an empty TU, with GCC and Clang.
6. A profile of the `parse/` rows: the share of time in allocation, `std::map`, node destruction and `shared_ptr`
   counts. It bounds what the node model can gain in speed. A rough estimate from the allocation count puts
   allocation at 5 to 15% of parse time.
7. Drafts for the maintainer to post: a v4 tracking issue, and notes on #324 and #327 about where they fit.

To ask: when `VERSION.txt` becomes 4.0.0. In the release commit is what the tooling assumes, and development builds
then keep the `v3` namespace and SONAME. Changing it at the start gives `toml::v4` from the first break, and needs
`regenerate.py` to stop stamping the changelog with a release date on that day. Also: whether a tracking issue is
wanted.

`benchmarks.cpp` does not build header-only with GCC 13 at `-O3 -Werror`: its counting `operator new` and `delete`
trip `-Wmismatched-new-delete`. Stage 4 touches that code.

## 1. Floors and dead weight (M)

Mechanical work. A change that alters no behaviour is accepted on identical Clang output.

- D2: delete the GCC 7 `for_each` workaround and its macros (`TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN` and two
  companions), the classic Intel branches, the Clang 13 pragma in `include/toml++/toml.hpp`, and the version tests
  in `std_new.hpp`. Chrono's guard for a libstdc++ older than 11 stays, because Clang can be paired with one.
- D2, in the README: CI builds GCC 11 and none of the other three floors; it has Clang 21, with 22 for clang-tidy,
  MSVC 19.51 and Apple Clang 21. The README names the tested versions and the stated minimums separately. Whether
  a job should change compiler to cover a floor is the maintainer's call.
- L7, the legacy-lambda workaround. It is keyed on the language mode: MSVC uses the legacy lambda processor in
  C++17 mode unless `/permissive-` or `/Zc:lambda` is given, in VS 2022 and VS 2026 alike, so the floor alone does
  not retire it. Set the default to 0 and read the `windows-msvc-x64-permissive` job, which builds C++17 with
  `/permissive`. That job has the newest MSVC, so a pass there does not cover VS 2022. If it goes, its four sites
  go with it (`table.hpp:365,391`, `array.hpp:940,972`).
- L7, the rest: `TOML_DISABLE_NOEXCEPT_NOEXCEPT` is needed up to VS 2022 17.13 and stays, renamed. `#error` traps
  on the old names. Remove `format_flags::force_multiline_arrays` and the `static_assert` that pins it
  (`tests/formatters.cpp:1520`).
- L14, L15: traps for `TOML_PARSER`, `TOML_API`, `TOML_ALL_INLINE`, `TOML_UNRELEASED_FEATURES`,
  `TOML_WINDOWS_COMPAT` and `TOML_INT_CHARCONV`; the traps for `TOML_CHAR_8_STRINGS` and `TOML_LARGE_FILES`
  deleted; `toml.h` reduced to its `#error`.
- D3: remove non-ASCII bare keys (`unicode.hpp:133-159`, their tests, the README list). `+` in bare keys
  (`unicode.hpp:127`) and hex floats (`parser.inl:1441-1514`, `2573-2712`) stay.
- L10 and L8's `TOML_CALLCONV`.
- L12: delete the macros nothing reads (the `TOML_ARCH_` family, `TOML_UNIX`, `TOML_LINUX`, `TOML_ICC_CL`,
  `TOML_COMPILER_HAS_RTTI`, `TOML_HAS_FEATURE`, `TOML_INLINE_GETTER`, `TOML_LIKELY_CASE`, `TOML_LANG_AT_LEAST`),
  and the fallbacks at `preprocessor.hpp:1486-1491` if a poxy build does not need them. Apply `TOML_EMPTY_BASES`
  where it belongs, and drop the argument of `TOML_MAKE_FLAGS_1` that its one caller leaves empty. Then compare the
  rest with the building-blocks library's `bb/preprocessor/*.hpp`. Ask the maintainer where its checkout is.
- L13, macros only. Batch 5 applies them. The definitions to bring over:

  | Macro | Expands to |
  |---|---|
  | `TOML_LIFETIME_BOUND` | `[[clang::lifetimebound]]` from Clang 7, `[[msvc::lifetimebound]]` from MSVC 19.41 |
  | `TOML_OWNER` | `[[gsl::Owner]]` from Clang 9 |
  | `TOML_VIEW` | `[[gsl::Pointer]]` from Clang 9 |
  | `TOML_RESETTER` | `[[clang::reinitializes]]` from Clang 7 |

- L8: a configuration page in `docs/pages/` replaces the README table and lists every macro a user may set, the
  undocumented ones included. Each has a `\def` and a `PUBLIC_MACROS` entry. The page says why `TOML_ASSERT` is off
  under `NDEBUG`: `TOML_PURE` and `TOML_CONST` are applied only then (`preprocessor.hpp:455-466`). `AGENTS.md`'s
  rule for a new macro names the page where it names the README table today.

To ask:

- whether any undocumented macro is internal and stays off the page: `TOML_SHARED_LIB`, `TOML_ENABLE_SIMD`,
  `TOML_INCLUDE_WINDOWS_H`, `TOML_UNDEF_MACROS`, `TOML_MAX_NESTED_VALUES`, `TOML_LIFETIME_HOOKS`,
  `TOML_FLOAT_CHARCONV`, `TOML_ABI_NAMESPACES`, `TOML_BREAK_AT_PARSE_ERRORS`. A macro that a failing
  `static_assert` names, such as `TOML_DISABLE_ENVIRONMENT_CHECKS`, is documented by that message and gets no entry.
- the old include guards (`INCLUDE_TOMLPLUSPLUS_H`, `TOMLPLUSPLUS_H`), and `toml::default_formatter`, an alias
  kept since v3.0.0 (`forward_declarations.hpp:407`): D4 suggests both go, and nobody asked
- the wording of the traps
- what evidence is enough to delete the legacy-lambda workaround for VS 2022

## 2. Build systems (M)

- B2: rename the 21 CMake options that lack the prefix, with a fatal error for each old name. `BUILD_FUZZER` is
  the exception. Master's script can change only after the release, so that alias outlives 4.0.0; it is
  undocumented, and goes with the first 4.x release after OSS-Fuzz builds with the new name.
- B3, B4, and Meson's B5 to B8.
- Every file that names an option follows: the CI workflow, `CONTRIBUTING.md`, the README, `AGENTS.md` (its quick
  check passes `-Dtomlplusplus_PEDANTIC=ON`), `toml-test/README.md` and `tools/coverage.py`.
- A check of what packagers will see change between v3.5.1 and the tree: recipes, installed files, exported
  symbols. It needs the maintainer's go-ahead. "Not on this machine" says where the earlier scripts are.
- A first draft of the packagers' heads-up, to post well before the release this time. #328 is the last one.

"Who the build decisions reach", under Reference, lists the recipes each decision breaks.

## 3. Exceptions (L)

Removes the cause of #285, which is closed: a library built with exceptions could not serve a caller that sets
`TOML_EXCEPTIONS=0`.

### Design

From the design study, adjusted for D4. Nothing was built.

- `try_parse()` and `try_parse_file()` are exported and identical for every caller. They return `parse_result`.
- `parse()`, `parse_file()` and `_toml` are header-inline, so the throw is in the caller's translation unit. One
  library, built either way, then serves both. D4 means no forwarders exist where exceptions are off.
- `parse_result` is one class in every build. The alias for `table` goes (`forward_declarations.hpp:113-119`) and
  the conversions get ref-qualifiers. Its implicit constructor from a table (`parse_result.hpp:102`) keeps
  `toml::parse_result r = toml::parse(doc);` compiling.
- `parse_error` has one layout: a string and a source region, today's exceptions-off layout
  (`parse_error.hpp:37-40`), with a non-virtual `what()`.
- The study proposes a separate thrown type, `parse_exception`, derived from `parse_error` and
  `std::runtime_error` and declared only with compiler exceptions. Its reason: with the MSVC STL,
  `_HAS_EXCEPTIONS=0` gives `std::exception` a different definition, so a `std::runtime_error` base cannot have one
  layout, and this project's no-exception builds set that macro (`cmake/compile-options.cmake:44`). The claim about
  the MSVC STL is from memory and unverified.
- The inline namespaces `ex`, `noex`, `lit_ex`, `lit_noex`, `impl_ex` and `impl_noex` go
  (`forward_declarations.hpp:127`, `parser.hpp:417`, `parser.inl:1637`, `preprocessor.hpp:1565`). Header-only
  builds keep one implementation namespace, keyed on the compiler's exception setting, around the reader, the
  parser and the `try_parse` definitions, because every TU compiles their try/catch (`parser.inl:85-111`).
- The rule against ODR trouble: a body that depends on the compiler's setting is compiled once in the library, or
  sits in that namespace, or is declared only with exceptions.
- A stream's exception becomes a `parse_error` carrying `what()` wherever the implementation has exceptions, which
  is today's exceptions-on behaviour (`parser.inl:1682-1694`). The pass-through at `1696-1698` goes.
  `std::bad_alloc` still propagates (`tests/mixed_exceptions.cpp:361-398`).
- `at()` is inline over the exported `get()` and declared only with compiler exceptions. The exported
  `table::at` and `array::at` go. The try/catch in `table::emplace_hint` (`table.hpp:1509-1520`) becomes a scope
  guard.
- The formatters lose `result_`, `dump_failed_parse_result()` and their `parse_result` constructors
  (`formatter.hpp:49-51`, `264`, `268`). Declare those constructors deleted: otherwise `json_formatter{ result }`
  compiles through the conversion to `const table&`, and a failed result is undefined behaviour where v3 printed
  the error.
- `TOML_EXCEPTIONS` stays and only decides whether the inline `parse()` exists. The parser's `optional` goes, which
  ends the `TOML_OPTIONAL_TYPE` leak.

The parser's internals are a separate question. With exceptions on, `set_error` throws and the check macros
compile to nothing (`parser.inl:1538-1543`); with them off, an error member is tested after every step
(`1545-1631`). Unifying the API does not force one control flow: a library built with exceptions could throw
inside and catch in `try_parse()`. One flow is simpler, and its cost is that `is_error()` stops being constant
false at 88 return-if-error sites and 57 advance-and-check sites, and the error functions lose `[[noreturn]]`.

What breaks, for code that uses exceptions:

```cpp
toml::parse_result r = toml::parse(doc); r.size(); // v3: parse_result is table
toml::table r = toml::parse(doc); r.size();        // v4

catch (const toml::parse_error& e) { log(e); }     // v3: log() takes const std::exception&
catch (const toml::parse_exception& e) { log(e); } // v4, if the thrown type is separate
```

For code that doesn't:

```cpp
auto r = toml::parse_file(path);     // v3
auto r = toml::try_parse_file(path); // v4

toml::table t = static_cast<toml::table&&>(result); // v3, from an lvalue result
toml::table t = std::move(result).table();          // v4

std::cout << toml::json_formatter{ result };                     // v3: prints the error
if (result) std::cout << toml::json_formatter{ result.table() }; // v4

auto& n = tbl.at("key");  // v3: asserts, then undefined
auto* n = tbl.get("key"); // v4: at() is not declared
```

Unchanged with exceptions: `auto tbl = toml::parse_file(path);`, `_toml`, catching `toml::parse_error` and reading
`e.description()`, catching `std::exception`, `tbl.at("k")`. Unchanged without: `if (!result)`, `result.error()`,
`result["a"].value_or(0)`, `std::move(result).table()`.

The alternative the study weighed and set aside: keep both modes and have one library export both sets of symbols,
from a second TU that compiles the parser with exceptions off. Its cost is permanent: two control flows, two APIs
in the docs, tests, fuzzers and benchmarks, and the parser twice in the binary.

### Stages

0. Measure. Build `benchmarks/benchmarks.cpp` twice at one commit, header-only, same compiler, once with
   `-DCMAKE_CXX_FLAGS=-DTOML_EXCEPTIONS=0`. The CMake exceptions option would add `-fno-exceptions` and change more
   than the parser. Compare the `parse/` rows. The difference is what one control flow would cost exception users.
1. One `parse_error`, with L9 applied to its constructors. Files: `parse_error.hpp`, `std_except.hpp`,
   `parser.inl:1152-1165`, `1681-1694`, `4334-4340`. Guards: `tests/parse_errors.cpp`,
   `tests/mixed_exceptions.cpp`.
2. `parse_result` everywhere; `parse()` declared as returning `table`. Files: `parse_result.hpp`,
   `forward_declarations.hpp:113-119`, `parser.hpp`, `parser.inl:4385-4476`. Guards:
   `tests/manipulating_parse_result.cpp`, `tests/consumer/`.
3. One control flow in the parser, if stage 0 shows the two builds within 1% on every `parse/` row. Otherwise both
   stay, as an internal detail. Files: `parser.inl:868-872`, `1538-1564`, `1660-1754`, `4156-4169`. Guards: every
   `parsing_should_fail` expectation, the fuzzers.
4. `try_parse` exported, `parse` inline, the namespaces removed, a stream's exception reported as a parse error.
   Files: `parser.hpp`, `parser.inl:4320-4476`, `preprocessor.hpp:1562-1566`, `forward_declarations.hpp:127-129`
   and `194-202`, the module file, `symbols.txt`, `tests/tests.cpp`, `fuzzing/`, the benchmark. Guards:
   `check-symbols`, `tests/parsing_streams_and_files.cpp`.
5. Formatters drop `parse_result`. Files: `formatter.hpp`, `formatter.inl:386-401` and `656-678`, the three
   formatters. Guard: `tests/formatters.cpp:2351-2378`.
6. `at()`, and the scope guard. Files: `table.hpp:924-959` and `1509-1520`, `table.inl:166-187`,
   `array.hpp:743-752`, `array.inl:181-195`. Guards: `tests/manipulating_tables.cpp:548-556` and `1073-1080`,
   `tests/manipulating_arrays.cpp:616-624`.
7. One build of the tests covers both APIs: the two-branch helpers in `tests/tests.cpp:33-103` and `136-158`
   become `try_parse()` plus a check that `parse()` throws the same error. CI's exceptions-off build stays, as a
   compiler mode. The "Test the installed package" step also compiles the consumer with `-fno-exceptions` against
   the shared library. The "rethrown" expectations in `tests/mixed_exceptions.cpp:418-473` become parse errors. If
   both control flows stay, the no-exceptions fuzzers keep covering the second one.

To ask:

- the name `try_parse`
- whether a library built with exceptions must serve MSVC callers built with `_HAS_EXCEPTIONS=0`. If not,
  `parse_error` can stay a `std::runtime_error` and no second type is needed
- what `TOML_EXCEPTIONS` still means
- whether any loss of speed is acceptable for one control flow
- whether the twelve `parse()` overloads shrink now that their symbols change anyway. Merging the `string_view`
  and `std::string&&` source-path overloads was measured earlier as a saving of about 1 KB and left for ABI reasons

## 4. Parser seam (M)

The parser builds the tree itself and uses it as its symbol table: four lookups (`parser.inl:3811`, `3888`,
`3961`, `4037`) and three sets of node addresses (`1651-1653`). The tree-building code is `3793-4060` and
`4172-4291`. This batch moves it behind an internal builder. It is all in `parser.inl`.

### Design

The interface between the syntax layer and the builder, from the study:

- table header: key path, header begin and end, the `[[ ]]` flag; returns the table and its depth
- key: table and key path; returns a slot
- scalar: a string view, `int64_t`, `double`, `bool`, `date`, `time` or `date_time`, with its `value_flags`
- begin array, append, begin inline table
- region: value, begin, end. Insert: slot and value
- begin document, with the source path, once; end document, with the end position

Values are builder-typed and returned up the recursion, as `node_ptr` is today, so the tree builder needs no stack.

A key path is its segments (text, begin, end) plus the key as written, which four messages quote (`3824`, `3915`,
`3922`, `4043`). Nested keys reuse both buffers (`4046-4047`), so a path is valid during the call only.

Error precedence fixes the order of calls: the header call follows the rest of its line (`3787`, `3876`), and the
key call comes between the "expected value" check and the value (`4028`, `4054`).

The syntax layer holds no document state. It enforces encoding, lexical rules, the key and value grammar, value
ranges, brackets and commas, the rest of a line, and depth inside one statement (`3593-3598`, `3688-3692`).

The builder enforces what needs each open table's keys and origin:

- duplicate keys (`4036-4044`)
- dotted keys extend only dotted-key or implicit tables (`3964-3982`)
- a header defines a table once, and may promote an implicit one (`3894-3925`)
- `[[x]]` appends only to an array that `[[x]]` made (`3897`)
- nothing enters an inline table or a static array (`3820-3845`)
- a path through an array of tables takes its last element (`3830-3838`)
- header depth, where an array of tables counts two (`3837`; `tests/parsing_key_value_pairs.cpp:666-681`)

The parser rejects `[a.b.c]`, `[a]`, `b.x = 1`, `[a.b]`, and that stays: TOML 1.1 says a dotted key creates and
defines each table before its last part.

Depth: the syntax layer owns the counter. The builder computes a header's depth, raises the depth error where it
is raised today (`3880-3885`), and returns it as that table's base.

Errors: the builder calls an error function on the syntax layer, which owns the scope text (`1658`), the source
path and the error state. All seven builder errors already pass explicit positions.

The parser assumes today's storage in six ways, and after the split all six are in the builder:

- node addresses as identities: the three sets, and `current_table` (`4068`)
- a `std::map` hint that survives a value parse (`4037` to `4058`)
- one heap node per value: 27 `new value` sites (`3048-3578`)
- a `shared_ptr` copy of the path per key, value and header (`3633`, `3715`, `3790`)
- keys and strings copied from parser buffers (`3052`, `3715`)
- friend access (`node.hpp:33`, `value.hpp:390`) and `node_ptr` back doors (`table.hpp:1488-1506`,
  `array.hpp:1594-1614`)

### Stages

1. A `tree_builder` class takes the tree state and the rules: `1649-1653`, `3710-3716`, `3793-4000`,
   `4032-4058`, `4105-4153`. Guards: `tests/parsing_tables.cpp`, `tests/parsing_key_value_pairs.cpp`,
   `tests/source_regions.cpp`, the conformance tests, toml-test, the fuzzers.
2. Values, arrays, inline tables and regions go through it, and the friends name it: `3025-3070`, `3266-3635`,
   `4172-4291`. The syntax layer then names no node type. Guards: `tests/parsing_*.cpp`.
3. The source path is passed once, and the parser becomes a template over its builder, so the tree builder inlines
   to today's code.
4. L6: the builder closes the region of a table, or of an array of tables, at its last key-value pair. Today
   `update_region_ends` (`4108`) leaves a table at its header and widens an array to its last child.

Each stage passes the benchmark gate on the `parse/` rows, against the stage before. Nothing but L6 is
user-visible. The review nit about an assert in `node_set::insert` ends here: the sets move into the builder.

## 5. Node model (XL)

D5, D6, D7, and L2 to L5, L9, L13. Also the v4 page's entries on node storage, the node interface, reverse
iteration and table iteration by value (#31).

### What exists

- `node` (`node.hpp:28`) is an exported abstract class with 35 virtual functions: the destructor, `type()`, twelve
  `is_*()`, eighteen `as_*()` and three `is_homogeneous()`. Its one data member is the source region: two
  positions and a `shared_ptr` to the path.
- `value<T>` (`value.hpp:382`) holds the value and a 16-bit `value_flags`.
- `table` (`table.hpp:209`) holds a `std::map` from `key` to `unique_ptr<node>` and an inline flag. `key` is a
  string plus a source region. `array` (`array.hpp:265`) holds a vector of `unique_ptr<node>`.
- A table iterator stores a proxy pair of references inside itself and rebuilds it on every dereference, so
  `std::reverse_iterator` over a table dangles. An array iterator yields `node&`.
- `node_view` holds one `node*`.
- A parsed 8 MB document holds about 455,000 allocations. By the value mix in the benchmark, nodes are about half
  and table entries about 30%.
- With GCC, a header-only TU carries about ten times the debug information of a compiled-mode one, because of the
  polymorphic node classes: an empty TU has 260 KB against 24 KB. In a model, node classes without virtual
  functions wrote 13 KB, against 193 KB when the base only has a virtual destructor.

### What the union breaks

By design: `const toml::node& n = tbl;`, `toml::toml_formatter fmt{ tbl }` unless it gains an overload,
`dynamic_cast` on nodes, references held across an insertion into the parent, and sorted iteration.

One break is silent, and the interview did not spell it out. With real pairs, `for (auto [k, v] : tbl)` copies each
pair, node included, so it gets slow and a change made through `v` is lost. In v3 the proxy holds references, so
the same loop edits the table. The tree's own code binds by value three times, all in
`tests/manipulating_parse_result.cpp`, and by reference 33 times. The other way to end the proxy's lifetime
problem is the one `std::flat_map` takes: return a pair of references by value. Then `auto [k, v]` keeps its
meaning and `auto& [k, v]` stops compiling.

Fixed by D6: the union holds `value<T>` objects, so `as_integer()` still returns `value<int64_t>*`, and `table`,
`array` and `value<T>` keep every member they inherit from `node` today.

### Findings of the design study

The study compared three designs. The maintainer chose its third, and the study had recommended the first two in
stages, so its stage plan does not apply. Its findings do:

- The union costs about three times the library work of in-place storage under the old hierarchy: 67
  node-to-derived casts in seven files, three formatter constructors (`toml_formatter.hpp:114`), `is_node`
  (`forward_declarations.hpp:1247`), `node_view{ tbl }` (`fuzzing/toml_node_api_fuzzer.cpp:264`), most of
  `tests/nodes.cpp`.
- Aliasing is tested and must keep working: inserting copies of the array itself, replacing with a descendant,
  assigning from a descendant (`tests/manipulating_arrays.cpp:990-1021`, `1218`, `1483`;
  `tests/manipulating_tables.cpp:201-241`, `943`). The new node must be complete before anything grows or dies.
- Today's code parks null pointers during an insertion (`table.hpp:1499-1522`, `array.inl:269-274`). In-place
  storage needs construct-then-commit; the strong-guarantee test at `tests/manipulating_tables.cpp:1074` depends on
  it.
- Relocation must be typed and must not throw: libstdc++ strings point into themselves.
- Incomplete types: `array` is defined before `key` and `table` (`include/toml++/toml.hpp:49-51`).
- In a DLL, the node's size and layout are compiled into clients and fixed for all of 4.x. No `extern template`:
  an earlier attempt broke shared-library consumers and needed about 186 new exports.
- An index is built on mutation, never lazily inside a const lookup.
- Tests that pin what changes: reference stability (`tests/manipulating_arrays.cpp:663`, `1368`, `1419`), sorted
  order (`tests/using_iterators.cpp:77`, `tests/manipulating_tables.cpp:582`, `792`, the `\out` block at
  `table.hpp:439`), the proxy (`tests/manipulating_parse_result.cpp:60`, `tests/using_iterators.cpp:115`).

On table storage:

- A sorted flat vector is quadratic for a large table built in random order. The parser looks a key up before
  every insert (`parser.inl:4037`), so it cannot sort once at the end.
- Insertion-ordered entries need an index once a table is large. A sorted array of positions is quadratic too. A
  hash without a key is linear on random keys and can be flooded with crafted ones; a keyed hash needs a seed. A
  balanced tree over entry positions is logarithmic in the worst case, as `std::map` is today, and gives the
  sorted traversal that the sorted-output flag and a linear equality need.
- Erase is linear in each, and `prune()` (`table.inl:234`) must compact in one pass.
- Small tables can scan. The node API fuzzer caps tables at 12 entries (`fuzzing/toml_node_api_fuzzer.cpp:17`), so
  its cap must rise past the threshold.
- One idea keeps addresses stable on insertion: chunks of geometric size that never move. It costs a second load
  per access and is unmeasured.

Expected numbers, from layout arithmetic for x86-64 libstdc++, all to be measured: a node with today's source
region and a string is 72 bytes, against 72 for an array element today and 136 against 176 for a table entry. One
path per document saves 16 bytes per node and per key. Peak memory can rise, because a growing container holds its
old and new buffers at once.

### Stages

1. Design document and interview, with prototypes measured on the benchmark documents. The batch asks first where
   the document is kept. The stage ends with the maintainer's go-ahead on the measured design. The questions:
   - how the three classes keep the inherited members now that they inherit none: one shared definition
   - how a table is passed where v3 code took `const node&`: a wider `node_view`, or nothing
   - real pairs, or a pair of references returned by value, given what each does to `for (auto [k, v] : tbl)`
   - table storage: contiguous pairs, chunks that never move, or one allocation per pair; and with it, whether an
     insertion may invalidate references to other elements
   - the index for large tables. A 100,000-key table must not be quadratic, because the parser reads untrusted
     input
   - what happens to `lower_bound()` and `emplace_hint()`. The study's default is to remove both; `emplace()`
     already inserts with one lookup
   - the size of a node, and whether tables and arrays sit in it or behind a pointer
   - one source path per document, which changes `source_region::path`
   - whether a container may be limited to 2^32 - 1 elements
2. Devirtualise within the current hierarchy: a type tag set through a protected constructor, `type()` and
   `is_*()` inline in `node.hpp`, each `as_*()` defined after its class, `is_homogeneous()` dispatched in
   `node.inl`, and a deleter that switches on the tag. About 730 lines of overrides go (`value.hpp:556-816`,
   `array.hpp:398-632`, `table.hpp:465-699`). It breaks loudly: `dynamic_cast`, and owning a node through
   `unique_ptr<node>`. `typeid(n)` changes silently. Guards: `tests/nodes.cpp:153-285`, `tests/visit.cpp`,
   `tests/node_view.cpp`, `tests/for_each.cpp`, the ODR and vtable tests at `tests/CMakeLists.txt:52-70`.
3. Define the member API shared by `node`, `table`, `array` and `value<T>` once, so stage 4 changes no call site.
4. The flip. `node` becomes the union, arrays hold nodes, tables hold pairs, iterators return real references, and
   `make_node` and `node_ptr` go. The builder from batch 4 is the only parser code that changes. `std::map` can
   stay for this one stage, so that the flip and the storage change are separate.
5. Table storage: insertion order and the index. Equality stays independent of order.
6. Size and allocations: the source path, the key layout. Each step is measured and each is optional.
7. The fixes on top: L2, L3, L4, L5, L9, the lifetime annotations, the natvis file
   (`toml++.natvis:105-126` describes the containers).

For stage 1's prototypes the study suggests one throwaway program and no library change: parse each benchmark
document with v3, recover insertion order from each key's source position, and rebuild the tree in a model of each
candidate. Measure build, destroy, copy, lookup hit and miss, and iteration; count allocations, bytes and peak
memory. `parse/tables` and the 100,000-key table settle the index, `parse/numbers` settles arrays, `parse/nested`
shows the overhead per node, and `parse/strings` is the control that should not move.

Stages 4, 5 and 6 each pass the benchmark gate against the baseline. The leak tests (`tests/main.cpp:86-100`) and
the sanitizer jobs guard every stage.

Declined here: keys and strings that refer into one copy of the document. `value<std::string>::get()` returns
`std::string&`, and D6 keeps it.

Also ends here: the nvc++ workaround for pure virtuals (`node.hpp:13`), the race on a shared table iterator's
proxy, and `TOML_UNREACHABLE` after the visit in `table.hpp`.

## 6. Output (L)

D10, D7's sorted flag, and the v4 page's entries on formatter internals and `std::format`. The rows from batch 0
are the benchmark gate for every step.

### What exists

- `impl::formatter` (`formatter.hpp:45`) is an exported base class that holds the node, the constants, the config,
  the indent and a `std::ostream*`. A formatter is attached to a stream, prints, and is detached.
- `toml_formatter` adds a key path and a pending-separator flag. The JSON and YAML formatters add no members. All
  four classes are exported, so on Windows every member function is in the DLL's ABI.
- `formatter_constants` cannot say how strings are written, so JSON passes a dialect argument and YAML writes
  block scalars, `? ` keys and floats itself.
- `toml_formatter` uses an indent of -1 for "at the root" and compensates in six places.
- `impl::print_to_stream()` is a family of exported overloads on `std::ostream&`.

### Design

From the design study. Nothing was built.

- The sink is two words and has no vtable: a function pointer taking a context, a `const char*` and a length, plus
  the context. The core formats into a 4 KiB stack buffer (the one at `formatter.inl:44`) and calls the sink when
  it fills.
- Set aside: a formatter templated on its output (nothing left for the compiled library), an abstract sink,
  writing to a `std::string` only (the whole document in memory before a stream sees a byte), output iterators
  (templates, or an indirect call per character), and `std::streambuf*` (still iostreams).
- On top, all hidden friends, for a formatter or a node: a function that writes to any callable taking a
  `std::string_view`, a to-string function, `operator<<`, and the formatter specialisations.
- `operator<<` becomes a template over the stream's traits, one write per flush. A template needs `<iosfwd>` only,
  as the `node_type` operator shows (`forward_declarations.hpp:268`). It must not name `std::streamsize`, which
  libc++ defines in `<ios>`.
- A failing stream still gets an exact prefix, and its exception still propagates (`tests/formatters.cpp:1695`,
  `tests/mixed_exceptions.cpp:279`).
- The `print_to_stream` overloads go. They are shims over helpers that already write to `char*`
  (`print_to_stream.inl:104-180`, `417`). Exported in their place: one print function taking the sink, the format
  and the flags, and sink printers for the date, time and source-position types.
- The formatter classes keep their names, constructors and `default_flags`. They become plain values: no base
  class, no export, no out-of-line members, a node pointer and the flags. Nothing is attached. The indent, the key
  path and the pending separator move to a writer local to the print call, and printing takes a const formatter.
- `formatter_constants` gains the indent string, the string dialect, a multi-line style (none, triple-quoted,
  block scalar), a bare-string rule (none, TOML key, YAML plain scalar) and a flag for the point before a float's
  exponent, all as enums. JSON loses `print_json_string`; YAML loses its three printers
  (`yaml_formatter.inl:129-177`). `? ` keys are layout and stay in the YAML table printer.
- The indent becomes unsigned. The writer tracks how many enclosing table headers were printed, and a sub-table
  indents only when that is non-zero. That deletes `toml_formatter.inl:498`, `:328` and `:335-336`, the `<= 0` at
  `formatter.inl:441`, and `yaml_formatter.inl:275`.
- Sorted output: the printers see table order through eight range-for loops (`toml_formatter.inl:73`, `226`,
  `298`, `382`, `406`, `416`; `json_formatter.inl:57`; `yaml_formatter.inl:206`). They become one internal helper,
  the only part of this batch that depends on table storage. TOML still prints values before sub-tables, so the
  order applies within each group. Bit 14 of `format_flags` is free.

How far iostreams can go: `<ostream>` and `<streambuf>` leave `formatter.inl` and `print_to_stream.inl`, and
`<sstream>` leaves `path.inl`. The saving is smaller than it looks. A header-only build with the parser includes
`<istream>` and `<fstream>` (`parser.inl:26`). From C++20 the chrono support is on by default, and `<chrono>`
includes `<sstream>` in libstdc++ 15 and in libc++. So the study advises against a switch or a separate header
for output.

For `std::format` the study proposed specialisations that accept an empty format spec only, with flags passed by
formatting the formatter object, as in `std::print("{}", toml::json_formatter{ tbl, flags })`. That matches the
maintainer's remark on #327. It gated them on whether `<format>` was already included. D10 replaces the gate and
adds fmt. Points the study made that still hold:

- `__cpp_lib_format` cannot tell "included" from "available": `<version>` defines it, and the library includes
  `<version>` (`preprocessor.hpp:990`).
- Specialisations declared in the global fragment of the module may be discarded, so `tests/consumer/import.cpp`
  should format a table.
- In C++23 a `key` is a range of characters, so `std::format("{}", key)` may print a list of characters. Not run.

### Stages

1. The sink and the writer under the current classes. Files: `formatter.hpp`, `formatter.inl`,
   `print_to_stream.inl`. Guards: `tests/formatters.cpp:1589`, `1632`, `1695`,
   `tests/mixed_exceptions.cpp:279-358`, `tests/printing.cpp`.
2. Dialects in `formatter_constants`. Guards: `tests/formatters.cpp:1783-2349`, `tests/round_trips.cpp:185`, the
   fuzzers.
3. The unsigned indent with an explicit root. Guards: `tests/formatters.cpp:1080-1587`, `2313`.
4. Formatters become plain values; one exported print function. `impl::formatter` goes, `stream()` included.
5. The templated `operator<<`; the `print_to_stream` files and three includes go. The float overload's only
   callers are `tests/printing.cpp:41` and `fuzzing/toml_float_fuzzer.cpp:74`. Add a compile-only check of what is
   included.
6. The to-string function and the format-to-callable function.
7. `std::formatter` and `fmt::formatter` from one implementation. No format-spec flags.
8. A format flag for sorted keys.

The design stage for step 7 checks, on libstdc++, libc++, the MSVC STL and fmt:

- where a forward declaration of the library's `formatter` template is ODR-safe. libc++ puts its names in an
  inline namespace, and so does fmt
- whether fmt's `format_as` hook serves fmt with no declaration at all. Unverified
- that C++23 range formatting does not claim `table`, `array` or `key` first

Also here, left alone in v3: the YAML trailing space after a key whose value is a table or array; column counts
for strings with escapes or non-ASCII text, which make an array wrap a few columns early or late; the YAML
key-length bound; one named constant for the indent string; the trailing `TOML_UNREACHABLE` in
`toml_formatter_count_inline_columns`.

To ask:

- the names of the two new functions
- whether a templated `operator<<` is acceptable, since it drops streams reached only by conversion
- whether a root YAML sequence loses its indent. It is indented today only because root tables alone get the -1
- whether #304's request to set float precision stays declined, as the maintainer's reply on that issue says

## 7. Paths (M)

D11 and L1.

### What exists

`impl::parse_path` (`at_path.inl:63`) parses, and `path::print_to` (`path.inl:201`) prints. As coded:

- An empty string gives one empty key. A dot at the start, after another dot, or at the end adds an empty key.
- Otherwise a dot only separates, so `a.[0]` equals `a[0]`.
- A key is the raw text up to the next dot or bracket. No quoting, no escaping, whitespace kept.
- `[n]` allows spaces and tabs around the digits. A missing `]` is accepted before a dot, a bracket or the end.
- On failure the path is left empty, and `at_path` returns an empty view.
- `str()` loses the dot between an empty key and an index.

`tests/path.cpp` pins all of it as v3.4.0 had it.

### Stages

1. Write the grammar down: TOML dotted keys (bare, basic-quoted and literal-quoted segments) plus `[n]`. An empty
   string is the empty path. `str()` prints a key bare when it can and quoted otherwise, so every path round-trips.
2. Interview: what a string that fails to parse gives (`path` has no error state today), and what an empty path
   selects.
3. Implement, sharing the key scanner with the parser in a way that still works with `TOML_ENABLE_PARSER=0`. L1
   goes in here, and `path_component::store_key()` and `store_index()` become ordinary members.
4. Rewrite `tests/path.cpp` against the grammar, and give the `at_path` fuzzer a print-and-reparse check.

The upgrade guide gets a table of v3 path strings that change meaning. Some change silently: `a . b` is the keys
`"a "` and `" b"` today, and `a` and `b` under TOML's rules.

## 8. Events (M)

The public half of batch 4, for #107. The maintainer wrote on that issue that the parser scans a document once, so
events could be raised where nodes are created. That is half the job: the parser also uses the tree to validate.

1. Interview on the handler's shape. D5 takes the virtual functions out of the public API, so the starting proposal
   adds none: a table of function pointers with a context, plus a template that fills it from any object. One
   event type with a kind lets later kinds arrive without changing that table.
2. A shadow structure does the tree's validating job when no tree is built: a trie of key names, each with its
   origin and node type, because the error messages name the type. The event entry point then accepts exactly what
   `try_parse()` accepts and reports the same errors.
3. Tests replay every conformance document through events and compare with `try_parse()`, errors included; a
   differential fuzzer.
4. A new `events/` benchmark row with a counting handler, reported next to `parse/` for the same documents.

The promises: events arrive in document order, key paths arrive whole, views are valid during the call, and
scalars arrive converted and range-checked. Events raised before an error are not retracted.

A compiled library cannot export a parser templated on a user's type, so the public entry point is non-template
and the binary holds the syntax layer twice. Report the size.

To ask: the handler's shape; whether 4.0.0 has a mode that skips validation. Adding one later breaks nobody.

## 9. Conversions (M)

For #175. The maintainer's position there: TOML is a config language, so arbitrary struct conversion is not a
goal, and the common cases such as points and vectors should be easy.

### Design

From the design study:

```cpp
struct point { int x, y; };

template <>
struct toml::converter<point>
{
	static toml::table to_node(const point& p)
	{
		return toml::table{ { "x", p.x }, { "y", p.y } };
	}

	static toml::optional<point> from_node(toml::node_view<const toml::node> n) noexcept
	{
		const auto x = n["x"].value<int>();
		const auto y = n["y"].value<int>();
		if (x && y)
			return point{ *x, *y };
		return {};
	}
};

tbl.insert("origin", point{ 1, 2 });
auto p = tbl["origin"].value<point>(); // optional<point>
```

- A specialised class template was preferred to ADL functions. ADL needs the functions in the type's namespace,
  which excludes `std` and third-party types, and the node-to-T direction has no argument of type T. A private
  member is reached with a friend declaration. The library's own `value_traits` already works this way
  (`forward_declarations.hpp:1009`).
- `to_node` returns anything insertion already takes: a table for a struct, a string for an enum.
- Failure is an empty optional, which is the contract `value<T>()` already has, in both exception modes.
  `value_or()` returns its default.
- `value_exact<T>()` rejects converted types with a `static_assert`. Allowing it later breaks nobody.
- A type with its own `value_traits` never reaches the hook. Class and enum types reach it ahead of string-like
  detection and enum-as-integer (`forward_declarations.hpp:618-631`); otherwise an enum stored as a string could
  not be written.
- Cost for code that never uses it: one declaration, and one more trait instantiation per class or enum type.
- Left out: comparison with nodes, `toml::value{ T }` deduction, containers of T.
- An empty optional does not say which key failed. If that matters, add an out-parameter carrying a path.

What could move to the hook: `TOML_SMALL_INT_TYPE` and `TOML_SMALL_FLOAT_TYPE`, which lose comparison with nodes
and deduction, and neither has a test. Retiring them also ends the `_Float16` clash on the v4 page. What stays
built in: `_Float16` and the 128-bit types, chrono, string-like types and wide strings. `TOML_OPTIONAL_TYPE` stays.

### Stages

1. Interview on the hook.
2. `value<T>()` and `value_or()` consult it. Files: `forward_declarations.hpp`, `value.hpp:1341-1502`. Guards: a
   new test file, `tests/nodes.cpp:287-750`, `tests/manipulating_values.cpp:378-1124`, `tests/custom_string.cpp`.
3. Insertion goes through it. Guards: `tests/manipulating_tables.cpp:788-1080`,
   `tests/manipulating_arrays.cpp:911-1230`, the leak build.
4. Docs with the point example and a single-value example.

To ask: the names; whether a converter outranks enum-as-integer and string-like detection; whether
`TOML_SMALL_INT_TYPE` and `TOML_SMALL_FLOAT_TYPE` are retired in favour of the hook.

## 10. Release (M)

1. Docs: the upgrade guide read once as a user, the API reference, the examples, the README.
2. The packagers' heads-up, posted by the maintainer with time to react, and the packaging check against v3.5.1.
3. Long fuzzing, and a cold review of the whole v4 diff in parts.
4. The release commit: `VERSION.txt`, unless batch 0 moved it, and the changelog's release line. `master`
   fast-forwards to `v4`, and CI tags `v4.0.0`. Then `fuzzing/build.sh` passes the new option name.
5. After: close #31, #107, #175, #309 and #325, and #304 once its float-precision half is answered; a `v3` branch
   for fixes.

A pushed tag is never moved. The 3.5.0 tag was recreated on another commit, which broke packagers who had already
taken it, and 3.5.1 exists only to repair that.

To ask: how long v3 gets fixes; whether packagers get a release candidate tag.

## After 4.0.0

Reflection (#324) is not in 4.0.0, and nothing here schedules it. If it is taken up, the batch 9 hook is its base:
a feature-gated default implementation for types that opt in. Only GCC 16 has C++26 reflection, and no CI job has
GCC 16.

The study's view of that pull request, for whoever takes it up. Keep: the member walk, an enabling macro that
defaults to off because it includes `<meta>`, the `rename` and `transient` annotations, and `reflectable` as the
per-type opt-in. Drop: `serialize<T>()` and `table::deserialize<T>()`, which duplicate insertion and `value<T>()`;
`serde_result<T>`, which needs `<variant>` and changes with the exception mode; `flatten`; and
`TOML_SERDE_ALLOW_PRIVATE`, a global macro that changes which members a type has across translation units.

## The v4 page

`docs/pages/v4.md` is the page users see. Rewrite it once the plan is approved:

- The introduction stops saying that nothing is decided.
- A planned entry says what changes and what it breaks, as the entries do today.
- A declined entry is deleted.
- The docs deploy from `master`, so the rewritten page goes there as well as to `v4`.

Where each entry goes. "Gone" means deleted from the page because the idea was declined.

| Entry | Verdict | Batch |
|---|---|---|
| Static path::subpath() overload | L1 | 7 |
| Paths and empty keys | D11 | 7 |
| Reverse iteration over tables | follows from D5 | 5 |
| parse_result conversions; Mixing exception settings; at() | follow from D8 | 3 |
| Comparing date-times as instants | L2 | 5 |
| Assigning a float to an integer value | L3 | 5 |
| Deducing a value's type | L4 | 5 |
| Integer formats in value_flags | L5 | 5 |
| Source regions of tables | L6 | 4 |
| Names kept for compatibility | L7 | 1 |
| Config macros | L8; the `TOML_ASSERT` paragraph is gone | 1 |
| Node storage; A smaller node interface | D5, D7 | 5 |
| Constructors that allocate under noexcept | L9 | 3, 5 |
| Detecting _Float16 | L10 | 1 |
| Minimum compiler versions | D2, L7 | 1 |
| Template machinery | gone (L11) | |
| Internal macros | L12 | 1 |
| Formatter internals | follows from D10 | 6 |
| Private members of exported classes | the classes are rewritten | 6, 7 |
| Unicode bare keys | D3 | 1 |
| Lifetime annotations | L13 | 1, 5 |
| Event interface; Custom types | D9 | 8, 9 |
| Reflection | not in 4.0.0; stays on the page as possible | |
| Output order and float precision | D7; float precision is asked in batch 6 | 5, 6 |
| std::format support | D9, D10 | 6 |
| Minimum CMake version | gone (B1) | |
| Option names; Package files; CPack | B2, B3, B4 | 2 |
| Meson: version check; examples; library type | B5, B6, B7 | 2 |
| Tipi.build | gone (B9) | |
| Appendix: preserving comments | still not planned | |
| Appendix: table iteration by value | follows from D5 | 5 |

New entries the page needs: nodes as values (what stops compiling, insertion order, loops that bind by value);
`try_parse`; the compiler floor; `toml.h` and the old macro spellings; Meson's two options; the `print_to_stream`
family; path syntax.

## Left as they are

Known and left alone. A batch that touches one of these reads the entry first.

Parser and input:

- `TOML_MAX_NESTED_VALUES` limits the depth of every node, however it is nested: arrays, inline tables, dotted
  keys and table headers, with an array of tables counting as two levels. The default of 128 comes from measured
  stack use: a nested inline table costs the parser up to 3.6 KB per level (Clang 22 at `-O0`), which is 456 KB of
  a 1 MiB stack at 128 levels.
- `parse_file()` sends files over 2 MiB through the stream reader. Reading them whole measured within 2%, costs
  the file size in peak memory, needs a size cap because a directory reports `INT64_MAX` on ext4, and turns a
  read failure partway into "File could not be read" at 0:0. The maintainer decided against it.
- `parse_file()` on a directory in procfs or sysfs returns an empty table: the size reads as 0 and a zero-byte
  read succeeds. v3.4.0 does the same. Directories elsewhere give an error.
- vcpkg patches `parser.inl` to map `ftello` and `fseeko` to `ftell` and `fseek` on 32-bit Android below API level
  24. The maintainer declined that in toml++ as a build-system problem (#204, #205). The parser work in v4 may
  stop the patch applying, which is a line for the packagers' heads-up.
- Nothing tests the stream reader's rethrow of glibc's forced-unwind exception.
- #241: with `-ffinite-math-only`, a parsed `inf` or `nan` is undefined behaviour. Only the warning was silenced.
- The float fallback relies on a correctly rounded `snprintf` and `strtod`, and the charconv parser still sends
  out-of-range values to `strtod`. CI checks glibc and Apple's libc; MinGW and UCRTs older than Windows 10 2004
  are untested.
- Denormals-are-zero mode (programs linked with `-ffast-math`): with charconv on, libstdc++'s `to_chars` prints a
  subnormal as `0e+00`, a float subnormal prints as `0.0`, and a parsed subnormal becomes 0. The fallback prints
  double subnormals correctly.
- The hex-float parser's "expected '0'" and "expected 'x' or 'X'" checks can't fail: value detection only gets
  there after a sign and `0x`. They stay as defensive code, with no test.
- A backslash followed by U+2028 in a string gives an "unknown escape sequence" message that looks like a doubled
  backslash, since the escape follows the backslash of the message.
- A parse-error description that cuts a long key to fit its buffer appends the rest of the message after the cut.
- "numeric value too long to identify type - cannot exceed N characters" keeps its wording, and the two `ref()`
  asserts still say "didn't match" and "did not reference". The maintainer chooses message wordings by question.
- The `to_chars` fast path for doubles stays: without it the numbers document formats 13% slower.

Nodes and values:

- `duration_cast_checked` refuses a result above `INTMAX_MAX` for a 64-bit unsigned target, so a `uint64_t`
  nanosecond `to_chrono()` fails for dates after 2262. It is tested directly, because no public call reaches
  sources other than nanoseconds and days.
- `node_view::visit()`'s `static_assert` refuses a return type that `{}` can initialise but that is not
  default-constructible.

Tools and CI:

- The fuzzers ignore documents over 4 KiB. A long table header is printed again for every sub-table, so printed
  size can grow with the square of a document's size. `toml_float_fuzzer` checks printing against `std::to_chars`
  and read-back, but nothing independent checks the parser's text-to-double conversion.
- clang-tidy checks the tests for bugs only: `tests/.clang-tidy` turns off the static analyzer and the style
  families, which cost seven times the CPU and flagged what tests do on purpose. The fuzz harnesses aren't in the
  clang-tidy job; CIFuzz compiles them.
- MSVC skips toml-test's `inline-table-key-dotted-*` conformance tests: their deeply nested expected tables
  exhaust its heap (C1060). GCC and Clang run them.
- The two Python tools keep their long functions.
- Code placement moves GCC's parse timings by 2 to 3% with no change to the code that runs. The benchmark target
  aligns functions to 64 bytes for that reason.

Not worked on unless new reports arrive:

- #220's nvc++ segfault in the formatters.
- Two MSVC module cases: proving that CI's Windows module step catches #288, and a VS 2022 17.14 crash when a
  module importer instantiates `node_view<node>`.
- MSVC 14.52's STL breaks `import tomlplusplus;` (C3774, `std::partial_ordering` not found). It is an MSVC bug.
  The fix is an `#ifdef _MSC_VER` include of `<compare>` in `tests/consumer/import.cpp`, never a workaround in the
  module source.

Build-cost ideas that were measured and declined:

- `extern template` for `value<T>` and `node_view` in compiled mode: the prototype broke shared-library
  consumers, and they need about 186 new exports
- `<iterator>`, `<fstream>` and the SIMD intrinsics headers out of what users see: noise, or a gain on libc++ only
- one runtime-base integer parser: GCC parses prefixed integers 3 to 5% slower
- `write_decimal` kept out of line: date and time printing 10 to 24% slower
- a smaller inlining budget: parsing 7 to 15% slower

## Reference

### Repository mechanics

- CI (`.github/workflows/ci.yaml`) runs on a push to any branch but `gh-pages`, when a changed file matches its
  path filter. Markdown is not in the filter. Its release job and its commit of regenerated files run for `master`
  only. The `pre-commit` workflow runs on every push.
- CIFuzz runs on pushes to `master` and on every pull request. The docs build and deploy from `master` only.
- The release job runs on `master` whenever no GitHub release exists for the version in `VERSION.txt`. It tags the
  commit and attaches `toml.hpp`. So a version change that reaches `master` is a release.
- `tools/regenerate.py` derives from `VERSION.txt`: the three macros in `version.hpp`, the namespace in the natvis
  file, and the install snippets in the README and the docs main page. For a new version it inserts a heading
  with a release link and the day's date under `## Unreleased` in the changelog, and stops if neither heading
  exists. The changelog has no `## Unreleased` heading, and it skips the changelog when the version's heading is
  already there.
- `symbols.txt` is not derived. Its 83 lines all start `toml::v3::`. `check-symbols` runs only in the Linux GCC
  shared-library job, so it does not see what a Windows DLL exports.
- On Windows a DLL build exports every member function of an exported class, private and inline ones included.
  The exported classes: `node`, `table`, `array`, `path`, `path_component`, `parse_error`, `impl::formatter` and
  the three formatters.
- The inline ABI namespaces: `v3`, from `TOML_LIB_MAJOR`; `ex` and `noex`; `lit_ex` and `lit_noex`; `impl_ex` and
  `impl_noex`; and `custopt` and `stdopt`, which cover only `date_time`. Three non-template functions that return
  an optional are outside the last pair: `wide_path()`, `get_line()` and `to_chrono()`.
- The consumer tests in `tests/consumer` run from CI steps only. Neither ctest nor Meson registers them.
- CI's compilers, as the README lists them: GCC 11 and 15, Clang 21, Apple Clang 21, MSVC 19.51. GCC 11 comes from
  an `ubuntu:22.04` container; the rest are the runner images' defaults. clang-tidy runs Clang 22.
- poxy reads `docs/pages` and `include`. This file is outside both, and prettier skips `docs/`.

### TOML and GitHub

- TOML 1.1.0 is the latest release, and what is unreleased since is clarifications only.
- TOML closed hex floats as not planned (toml-lang/toml#562). `+` in bare keys and non-ASCII bare keys were backed
  out before 1.1.0 (toml-lang/toml#979), which keeps bare keys to ASCII letters, digits, `_` and `-`.
- Open issues: #31, #107, #175, #304, #309, #325, and #328, the packagers' heads-up for 3.5.1. Open pull requests:
  #324 and #327, by one contributor, who was told to wait for v4.
- On #327 the maintainer wrote that a way to pass flags is probably not needed, that formatting the formatter
  object could carry them, and that the formatters get an overhaul in v4 for better non-iostream support.
- On #304: "Customizing float precision in particular I do not think is a good idea; the formatter emits the
  floats in a precision that allows them to be exactly round-tripped."
- On #312, which added partial comment support: "Partial support is _worse_ than no support at all". #28 is closed
  with a commitment not to add it.
- On #31, long ago: the proxy exists to hide the `unique_ptr`, and a node that is "a glorified discriminated
  union" by composition would remove the need for it. D5 is that design.

### Who the build decisions reach

From the packaging survey done for 3.5.0. Repology listed 126 packages in 94 repositories.

- B8: Debian's package passes `-Dcompile_library=true`. Gentoo passes `-Dbuild_lib=true` and is unaffected.
- B6: Fedora's spec lists no example binaries and already stops on them as unpackaged files. Nix carried #233 as a
  patch.
- B7: WrapDB consumers use toml++ as a subproject.
- B2: Spack builds with CMake. Anything that sets a CMake option is reached.
- B3: Meson already uses `share/` for header-only installs. vcpkg builds with Meson, requires the library with
  `find_library`, and expects the config under `lib/cmake`.
- Since 3.5.0 a default Meson build is header-only, and a recipe that wants the library passes
  `-Dbuild_lib=true`. That reached Arch, MSYS2, Homebrew, Alpine, Void, FreeBSD and vcpkg; #328 was the notice.
- The SONAME changes to 4. Arch alone has seven packages that link the shared library.
- Tests with `-Duse_vendored_libs=false` need Catch2 3.
- Conan copies headers only, and the README names a Conan version that ConanCenter may not have yet.

### User-settable macros with no documentation

`TOML_SHARED_LIB`, `TOML_ENABLE_SIMD`, `TOML_INCLUDE_WINDOWS_H`, `TOML_UNDEF_MACROS`, `TOML_MAX_NESTED_VALUES`,
`TOML_LIFETIME_HOOKS`, `TOML_ABI_NAMESPACES`, `TOML_PEDANTIC`, `TOML_BREAK_AT_PARSE_ERRORS`, and
`TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN` with its acknowledgement macro, which goes with GCC 7.

Documented with a `\def` but missing from the README table: `TOML_FLOAT_CHARCONV`, `TOML_INT_CHARCONV` and the two
`TOML_DISABLE_` macros. `TOML_ASSERT` and `TOML_IMPLEMENTATION` are in the table and not in `PUBLIC_MACROS`, on
purpose: `toml.hpp` undefines both.

### Compiler-specific code

- GCC 7: the `for_each` workaround (`preprocessor.hpp:1387-1395`, `table.hpp:355`, `array.hpp:15`), and a version
  test in `std_new.hpp:12`. Nothing is specific to GCC 8.
- Classic Intel: `preprocessor.hpp:132-144`, `155`, `171`, `499-501`, `514-516`; `std_new.hpp:12`;
  `parser.inl:1575`; three "icc bugfix" comments in `date_time.hpp`.
- nvc++: pure virtuals get a body (`node.hpp:13`, #220).
- MSVC: the legacy lambda processor (`preprocessor.hpp:1349`), error C3878 up to 19.43 (`preprocessor.hpp:1366`,
  used at `value.hpp:431`, `451`), warning C4275 (`parse_error.hpp:15`).
- Clang 13: one pragma (`include/toml++/toml.hpp:31`). Apple Clang's versions are mapped to upstream ones
  (`preprocessor.hpp:82-130`), and that stays.

### v3 leftovers the v4 page does not list

- the five old macro spellings, and the two `#error` traps (`preprocessor.hpp:1002`, `1039-1049`, `1116`, `1134`,
  `1186`, `1279-1289`)
- `include/toml++/toml.h`, and the old include guards defined at `include/toml++/toml.hpp:8-9`
- `toml::default_formatter` (`forward_declarations.hpp:407`)
- `table_iterator::proxy_instantiated_` (`table.hpp:42`) and `array::preinsertion_resize` (`array.hpp:284-286`),
  both unused and kept for the ABI
- Meson's `compile_library` and `float16` options

## Working with the maintainer

`AGENTS.md` covers the code, the comments, the tests, the changelog and source control. These rules are not in it.

Decisions:

- When the maintainer has decisions to make, interview them: one question at a time, each with a recommendation,
  each able to stand alone, in short plain sentences. Record the answers yourself. Never hand over a document to
  mark up.
- Before changing a message a user reads, such as an error or a `static_assert` text, ask, one idea per question.
  The options are the wordings the code already has for that idea.
- Anything the maintainer calls v4 goes on the v4 page, breaking or not. What the page holds is their call.
- A question gets an answer. It doesn't start the work behind it.

Plans and batches:

- A batch starts by naming its plan and listing its stages, in one line each. Never refer to a stage by number
  before the list was shown.
- A batch plan covers one batch and is deleted when the batch is done. What it deferred returns to the plan of
  record. A finished item is deleted, never ticked.
- Ask before writing a planning document into the repository.

Handover:

- End a handover with the verified state of the working tree. Don't suggest commit messages or how to split
  commits. Never commit or push unless asked for that piece of work, and never push.
- A clean check is reported only after reading what the check processed. A wrapper that greps for failures passes
  on "nothing to do". After changing how a check is run, break something once and watch it fail.
- Say which standard library a claim was measured on. libc++ can stand in for the MSVC STL locally, and only for a
  change that hinges on standard-library behaviour. Never report a libc++ result as an MSVC one.
- When a fix depends on the behaviour of a compiler version that isn't at hand, pick the fix that doesn't, or say
  that CI is its first test. CI's GCC 11 ignores diagnostic pragmas for warnings that come from inlined code, so
  remove the cause.

Long runs:

- Report progress regularly. Look for a message from the maintainer after every tool result. "Find a good stopping
  point" means stop at the next point that leaves nothing half-broken.
- For an unattended run, ask every decision it could reach once, up front, with the deadline. Then don't stop
  until the deadline. Keep a ledger outside the repository, and finish with a report whose first section says what
  is needed from the maintainer.
- An agent brief follows the same verification rule as the session: one build and the tests, never a copy of CI.
- A review handed to a subagent is a bounded read: the brief carries the diff and the facts it depends on, the
  reviewer appends findings to a file as it goes, and the brief gives a word cap. Split a review that doesn't fit.

Code habits:

- A table inside a function is `static constexpr`. In a `constexpr` function it moves to namespace scope. A braced
  list iterated at run time is the same case. GCC rebuilds a non-static one on the stack on every call.
- A value derived from a constant gets one named constant beside its source.
- An addition to a macro's logic goes in that macro's existing block. If it needs something defined later, move
  that up.
- The agent's editing tools can turn a backslash, a `u` and four hex digits into the character they name. After
  editing a file that holds such an escape, search it for non-ASCII bytes. `tools/regenerate.py` refuses one in the
  library headers.

Scope:

- A consumer test checks the build's own behaviour: install layout, targets, options. It doesn't cover each way of
  fetching the library.
- Build conventions follow the repository's own style first, then the maintainer's other libraries. No
  `CMakePresets.json`.

## Not on this machine

These stayed on the machine this file was written on.

- The earlier master plan. Everything that still applies is in this file.
- The agent's memory for this project. The rules are under "Working with the maintainer".
- Packaging-check scripts, 172 KB in `/var/tmp/tomlplusplus-consumer-check/`. `check.sh` compares what packagers
  and consumers use between a ref and the working tree: the vcpkg, Homebrew and Conan recipes, the installed
  files and the exported symbols. `in-container.sh` runs it under any CMake from PyPI. `mingw.sh` builds the tests
  and examples as a MinGW DLL and runs them under wine. `ossfuzz.sh` builds the fuzzers with master's script on
  OSS-Fuzz's base image. Batches 2 and 10 want the first; it can be rewritten from this description.
- The code quality review's working files, which nothing here needs.

That machine ran Linux, and some steps assume it: `perf` for the profile in batch 0, and the glibc tunable that
`CONTRIBUTING.md` gives for steady benchmark timings. On Windows those steps need other tools.
