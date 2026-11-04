See @README.md for a description of this project.

See @CONTRIBUTING.md for contribution guidelines.

`toml.hpp` at the repository root is generated from the headers in `include/`, and the version numbers in
`include/toml++/impl/version.hpp`, the natvis file and the docs come from `VERSION.txt`:

- Change `include/`, never `toml.hpp`. CMake and Meson builds from a git checkout regenerate it, and so does the
  pre-commit hook. `python3 tools/regenerate.py` does it directly, and `--check` reports what is stale.
- The `#undef` block in `include/toml++/toml.hpp` is generated too.
- Leave `VERSION.txt` alone. Changing it on `master` publishes a release.
- `tests/conformance_*.cpp` come from `tools/generate_conformance_tests.py`. Don't edit them by hand.

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
- NO LLM VOICE SLOP

ANY COMMENTS THAT DO NOT MEET THE ABOVE CRITERIA SHOULD BE CONSIDERED A BUG AND AGGRESSIVELY FLAGGED DURING REVIEW.
Obviously-human comments like in-jokes, banners/headers and slang are fine.
