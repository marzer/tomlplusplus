//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

//# {{
#ifndef INCLUDE_TOMLPLUSPLUS_H
#define INCLUDE_TOMLPLUSPLUS_H
//# }}

//# Note: most of these would be included transitively but
//# they're listed explicitly here because this file
//# is used as the source for generate_single_header.py.

#include "toml_preprocessor.h"
#include "toml_common.h"
#include "toml_date_time.h"
#include "toml_print_to_stream.h"
#include "toml_node.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table.h"
#include "toml_node_view.h"
#include "toml_utf8_generated.h"
#include "toml_utf8.h"
#include "toml_formatter.h"
#include "toml_default_formatter.h"
#include "toml_json_formatter.h"
#if TOML_PARSER
	#include "toml_parse_error.h"
	#include "toml_utf8_streams.h"
	#include "toml_parser.h"
#endif // TOML_PARSER

#if TOML_IMPLEMENTATION
	#include "toml_node.hpp"
	#include "toml_array.hpp"
	#include "toml_table.hpp"
	#include "toml_default_formatter.hpp"
	#include "toml_json_formatter.hpp"
#if TOML_PARSER
	#include "toml_parser.hpp"
#endif // TOML_PARSER

#if !TOML_ALL_INLINE
	#include "toml_instantiations.hpp"
#endif // !TOML_ALL_INLINE

#endif // TOML_IMPLEMENTATION

// macro hygiene
#if TOML_UNDEF_MACROS
	#undef TOML_INT_CHARCONV
	#undef TOML_FLOAT_CHARCONV
	#undef TOML_GNU_ATTR
	#undef TOML_PUSH_WARNINGS
	#undef TOML_DISABLE_SWITCH_WARNINGS
	#undef TOML_DISABLE_INIT_WARNINGS
	#undef TOML_DISABLE_VTABLE_WARNINGS
	#undef TOML_DISABLE_PADDING_WARNINGS
	#undef TOML_DISABLE_FLOAT_WARNINGS
	#undef TOML_DISABLE_ALL_WARNINGS
	#undef TOML_POP_WARNINGS
	#undef TOML_ALWAYS_INLINE
	#undef TOML_NEVER_INLINE
	#undef TOML_ASSUME
	#undef TOML_UNREACHABLE
	#undef TOML_INTERFACE
	#undef TOML_EMPTY_BASES
	#undef TOML_CPP_VERSION
	#undef TOML_CPP
	#undef TOML_MAY_THROW
	#undef TOML_NO_DEFAULT_CASE
	#undef TOML_CONSTEVAL
	#undef TOML_LIKELY
	#undef TOML_UNLIKELY
	#undef TOML_NODISCARD_CTOR
	#undef TOML_MAKE_VERSION
	#undef TOML_LANG_EFFECTIVE_VERSION
	#undef TOML_LANG_HIGHER_THAN
	#undef TOML_LANG_AT_LEAST
	#undef TOML_LANG_UNRELEASED
	#undef TOML_STRING_PREFIX_1
	#undef TOML_STRING_PREFIX
	#undef TOML_UNDEF_MACROS
	#undef TOML_RELOPS_REORDERING
	#undef TOML_ASYMMETRICAL_EQUALITY_OPS
	#undef TOML_ALL_INLINE
	#undef TOML_IMPLEMENTATION
	#undef TOML_EXTERNAL_LINKAGE
	#undef TOML_INTERNAL_LINKAGE
	#undef TOML_INTERNAL_NAMESPACE
	#undef TOML_COMPILER_EXCEPTIONS
	#undef TOML_TRIVIAL_ABI
	#undef TOML_ABI_NAMESPACES
	#undef TOML_ABI_NAMESPACE_START
	#undef TOML_ABI_NAMESPACE_END
	#undef TOML_PARSER_TYPENAME
	#undef TOML_LAUNDER
#endif

//# {{
#endif // INCLUDE_TOMLPLUSPLUS_H
//# }}
