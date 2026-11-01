# Vendored dependencies

The code files in this directory are third-party 'vendor' dependencies used by the various test applications.

⚠&#xFE0F; They are **not** required for regular use of toml++.

⚠&#xFE0F; They are **not** `#included` anywhere in toml++.

| Library | Files |
| :--- | :--- |
| [Catch2](https://github.com/catchorg/Catch2) | `catch_amalgamated.hpp`, `catch_amalgamated.cpp` |
| [nlohmann/json](https://github.com/nlohmann/json) | `json.hpp` |

The versions are pinned in `tools/update_vendored_libs.py`. To update a library, change its version there and run the script.
