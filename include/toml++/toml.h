#pragma once

//# Note: most of these would be included transitively but
//# they're listed explicitly here because this file
//# is used as the source for generate_single_header.py.

#include "toml_common.h"
#include "toml_node.h"
#include "toml_table.h"
#include "toml_array.h"
#include "toml_value.h"
#include "toml_utf8.h"
#include "toml_parser.h"
#include "toml_writer.h"

//macro hygiene
#undef TOML_ASSERT
#undef TOML_STRICT
#undef TOML_CPP_VERSION
#undef TOML_CPP
#undef TOML_PUSH_WARNINGS
#undef TOML_DISABLE_ALL_WARNINGS
#undef TOML_POP_WARNINGS
#undef TOML_INTERFACE
#undef TOML_EMPTY_BASES
#undef TOML_ALWAYS_INLINE
#undef TOML_ASSUME
#undef TOML_UNREACHABLE
#undef TOML_NO_DEFAULT_CASE
#undef TOML_CONSTEVAL
#undef TOML_LIKELY
#undef TOML_UNLIKELY
#undef TOML_NO_UNIQUE_ADDRESS
#undef TOML_NODISCARD_CTOR
#undef TOML_STRING_PREFIX_1
#undef TOML_STRING_PREFIX
#undef TOML_CONDITIONAL_NOEXCEPT
#undef TOML_MAY_THROW
#undef TOML_LANG_HIGHER_THAN
#undef TOML_INDENT
