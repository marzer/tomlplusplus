#pragma once

//* note that we don't actually need to explicitly include all the internal headers here;
//* it's done for tooling purposes, since this file is used as the input to a script
//* that generates the single-header version of toml++.

#include "toml_common.h"
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table.h"
#include "toml_table_array.h"
#include "toml_parser.h"

//macro hygiene
#undef TOML_USE_CHAR_8_IF_AVAILABLE
#undef TOML_ASSERT
#undef TOML_CPP_VERSION
#undef TOML_CPP
#undef TOML_DISABLE_WARNINGS
#undef TOML_RESTORE_WARNINGS
#undef TOML_INTERFACE
#undef TOML_EMPTY_BASES
#undef TOML_ALWAYS_INLINE
#undef TOML_ASSUME
#undef TOML_UNREACHABLE
#undef TOML_NO_DEFAULT_CASE
#undef TOML_EXCEPTIONS_ENABLED
#undef TOML_CONSTEVAL
#undef TOML_LIKELY
#undef TOML_UNLIKELY
#undef TOML_NO_UNIQUE_ADDRESS
#undef TOML_STRING_PREFIX
