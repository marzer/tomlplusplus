#pragma once

//# Note: most of these would be included transitively but
//# they're listed explicitly here because this file
//# is used as the source for generate_single_header.py.

#include "toml_common.h"
#include "toml_date_time.h"
#include "toml_print_to_stream.h"
#include "toml_node.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table.h"
#include "toml_node_view.h"
#include "toml_utf8.h"
#include "toml_parser.h"
#include "toml_formatter.h"
#include "toml_default_formatter.h"
#include "toml_json_formatter.h"

// macro hygiene
#if TOML_UNDEF_MACROS
	#undef TOML_EXCEPTIONS
	#undef TOML_USE_STREAMS_FOR_FLOATS
	#undef TOML_GCC_ATTR
	#undef TOML_PUSH_WARNINGS
	#undef TOML_DISABLE_SWITCH_WARNINGS
	#undef TOML_DISABLE_INIT_WARNINGS
	#undef TOML_DISABLE_ALL_WARNINGS
	#undef TOML_POP_WARNINGS
	#undef TOML_ALWAYS_INLINE
	#undef TOML_ASSUME
	#undef TOML_UNREACHABLE
	#undef TOML_INTERFACE
	#undef TOML_EMPTY_BASES
	#undef TOML_CPP_VERSION
	#undef TOML_CPP
	#undef TOML_MAY_THROW_UNLESS
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
	#undef TOML_DOXYGEN
#endif

/// \mainpage toml++
/// 
/// This is the home of the API documentation for toml++, a [TOML](https://github.com/toml-lang/toml) parser for C++17 and later.
/// If you're looking for information about how to add toml++ to your project etc, see the
/// see [README](https://github.com/marzer/tomlplusplus/blob/master/README.md) on GitHub.
/// Otherwise, browse the docs using the links at the top of the page. You can search from anywhere by pressing the TAB key.
/// 
/// <em>Obviously this page is pretty sparse and could do with some more content. If you have concrete suggestions for what
/// should go here, please [let me know](https://github.com/marzer/tomlplusplus/issues)!</em>
/// 
/// \cpp
/// #include <iostream>
/// #include <toml++/toml.h>
/// 
/// int main()
/// {
/// 	auto tbl = toml::table{{
/// 		{ "lib", "toml++" },
/// 		{ "cpp", toml::array{ 17, 20, "and beyond" } },
/// 		{ "toml", toml::array{ "0.5.0", "and beyond" } },
/// 		{ "repo", "https://github.com/marzer/tomlplusplus/" },
/// 		{ "author", toml::table{{
/// 				{ "name", "Mark Gillard" },
/// 				{ "github", "https://github.com/marzer" },
/// 				{ "twitter", "https://twitter.com/marzer8789" }
/// 			}}
/// 		},
/// 	}};
/// 	
/// 	std::cout << tbl << std::endl;
/// 	return 0;
/// }
/// \ecpp
/// 
/// \out
/// cpp = [ 17, 20, "and beyond" ]
/// lib = "toml++"
/// repo = "https://github.com/marzer/tomlplusplus/"
/// toml = [ "0.5.0", "and beyond" ]
/// 
/// [author]
/// github = "https://github.com/marzer"
/// name = "Mark Gillard"
/// twitter = "https://twitter.com/marzer8789"
/// \eout
/// 
