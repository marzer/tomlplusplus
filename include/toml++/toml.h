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
#include "toml_node_view.h"
#include "toml_parser.h"
#include "toml_formatter.h"
#include "toml_default_formatter.h"
#include "toml_json_formatter.h"

//macro hygiene
#if TOML_UNDEF_MACROS
	#undef TOML_EXCEPTIONS
	#undef TOML_USE_STREAMS_FOR_FLOATS
	#undef TOML_PUSH_WARNINGS
	#undef TOML_DISABLE_SWITCH_WARNING
	#undef TOML_DISABLE_FIELD_INIT_WARNING
	#undef TOML_DISABLE_VAR_INIT_WARNING
	#undef TOML_DISABLE_ALL_WARNINGS
	#undef TOML_POP_WARNINGS
	#undef TOML_ALWAYS_INLINE
	#undef TOML_ASSUME
	#undef TOML_UNREACHABLE
	#undef TOML_INTERFACE
	#undef TOML_EMPTY_BASES
	#undef TOML_CPP
	#undef TOML_CONDITIONAL_NOEXCEPT
	#undef TOML_MAY_THROW
	#undef TOML_NO_DEFAULT_CASE
	#undef TOML_CONSTEVAL
	#undef TOML_LIKELY
	#undef TOML_UNLIKELY
	#undef TOML_NO_UNIQUE_ADDRESS
	#undef TOML_NODISCARD_CTOR
	#undef TOML_MAKE_VERSION
	#undef TOML_LANG_EFFECTIVE_VERSION
	#undef TOML_LANG_HIGHER_THAN
	#undef TOML_LANG_AT_LEAST
	#undef TOML_LANG_EXACTLY
	#undef TOML_STRING_PREFIX_1
	#undef TOML_STRING_PREFIX
	#undef TOML_UNDEF_MACROS
#endif
