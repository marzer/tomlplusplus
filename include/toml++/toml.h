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
	#undef TOML_RELOPS_REORDERING
	#undef TOML_ASYMMETRICAL_EQUALITY_OPS
#endif

/// \mainpage toml++
/// 
/// This is the home of toml++, a header-only [TOML](https://github.com/toml-lang/toml) parser and serializer for C++17 and later.
///
/// \tableofcontents
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-features Features
/// - C++17 (plus some C++20 features where supported, e.g. char8_t strings)
/// - Proper UTF-8 handling (incl. BOM)
/// - Works with or without exceptions
/// - Doesn't require RTTI
/// - First-class support for serializing to JSON
/// - Fully [TOML v0.5.0](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md)-compliant  
/// - Supports a number of 'unreleased' TOML features (optional; these can be disabled)
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-adding-lib Adding toml++ to your project
/// Clone [the repository](https://github.com/marzer/tomlplusplus/) from GitHub. It's header-only so there's not much you have to do after that,
/// other than some very minor (optional) configuration. See the [README](https://github.com/marzer/tomlplusplus/blob/master/README.md) for more info.
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-api-documentation API Documentation
/// You're looking at it! Browse the docs using the links at the top of the page. You can search from anywhere by pressing the TAB key.
/// 
/// <em>toml++ is still pretty hot off the presses so there's going to be some omissions, typos and general sparseness throughout the docs.
/// If you spot something or have a suggestion, please [let me know](https://github.com/marzer/tomlplusplus/issues)!</em>
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-example Basic examples
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-parsing-files Parsing TOML files
/// toml++ works whether you have exceptions enabled or not. For the most part the usage is the same,
/// the main difference being how parsing errors are reported to the caller. When exceptions are enabled
/// a toml::parse_error is thrown directly from the site of the error:
/// \cpp
/// #include <iostream>
/// #include <fstream> //required for parse_file()
/// #include <toml++/toml.h>
/// using namespace std::string_view_literals;
/// 
/// int main()
/// {
/// 	toml::table tbl;
/// 	try
/// 	{
/// 		tbl = toml::parse_file("configuration.toml");
/// 	}
/// 	catch (const toml::parse_error& err)
/// 	{
/// 		std::cerr
/// 			<< "Error parsing file '"sv << *err.source().path
/// 			<< "':\n"sv << err.description()
/// 			<< "\n  ("sv << err.source().begin << ")"sv
/// 			<< std::endl;
/// 		return 1;
/// 	}
/// 	
/// 	do_stuff_with_your_config(tbl);
/// 	return 0;
/// }
/// 
/// \ecpp
/// 
/// When exceptions are disabled parsing methods return a toml::parse_error and it is up to the caller
/// to check if parsing has been successful by examining the return value:
/// \cpp
/// #include <iostream>
/// #include <fstream> //required for parse_file()
/// #include <toml++/toml.h>
/// using namespace std::string_view_literals;
/// 
/// int main()
/// {
/// 	toml::parse_result tbl = toml::parse_file("configuration.toml");
/// 	if (!tbl)
/// 	{
/// 		std::cerr
/// 			<< "Error parsing file '"sv << *tbl.error().source().path
/// 			<< "':\n"sv << tbl.error().description()
/// 			<< "\n  ("sv << tbl.error().source().begin << ")"sv
/// 			<< std::endl;
/// 		return 1;
/// 	}
/// 	
/// 	do_stuff_with_your_config(tbl); //toml::parse_result is convertible to toml::table
/// 	return 0;
/// }
/// \ecpp
/// \see toml::parse_file()
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-parsing-strings Parsing TOML directly from strings
/// 
/// \cpp
/// #include <iostream>
/// #include <toml++/toml.h>
/// using namespace std::string_view_literals;
/// 
/// int main()
/// {
///		// parse error handling omitted for brevity.
///		static constexpr auto source = R"(
///			[library]
///			name = "toml++"
///			version = "0.1.0"
///			authors = ["Mark Gillard <mark@notarealwebsite.com>"]
///		
///			[dependencies]
///			cpp = 17
///		)"sv;
///		auto tbl = toml::parse(source);
///		std::cout << tbl << std::endl;
/// 	return 0;
/// }
/// \ecpp
/// 
/// \out
/// [dependencies]
/// cpp = 17
/// 
/// [library]
/// authors = ["Mark Gillard <mark@notarealwebsite.com>"]
/// name = "toml++"
/// version = "0.1.0"
/// \eout
/// \see toml::parse()
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-manipulations Traversing and manipulating data
/// 
/// \cpp
/// #include <iostream>
/// #include <toml++/toml.h>
/// using namespace std::string_view_literals;
/// 
/// int main()
/// {
///		static constexpr auto source = R"(
///			numbers = [ 1, 2, 3, "four", 5.0 ]
///			vegetables = [ "tomato", "onion", "mushroom", "lettuce" ]
///			minerals = [ "quartz", "iron", "copper", "diamond" ]
///
///			[animals]
///			cats = [ "tiger", "lion", "puma" ]
///			birds = [ "macaw", "pigeon", "canary" ]
///			fish = [ "salmon", "trout", "carp" ]
///
///		)"sv;
///		auto tbl = toml::parse(source);
///		
///		auto numbers = tbl["numbers"];
///		std::cout << "table has 'numbers': "sv << !!numbers << std::endl;
///		if (numbers)
///		{
///			std::cout << "'numbers' is a: "sv << numbers.type() << std::endl;
///			std::cout << "'numbers': "sv << numbers << std::endl;
///			for (auto& node : *numbers.as_array())
///			{
///				node.visit([=](auto&& n) noexcept
///				{
///					if constexpr (toml::is_number<decltype(n)>)
///						(*n)++;
///					else if constexpr (toml::is_string<decltype(n)>)
///						n = "five"sv;
///				});
///			}
///			numbers.as_array()->push_back(7);
///			numbers.as_array()->emplace_back<toml::array>(8, 9);
///			std::cout << "'numbers': "sv << numbers << std::endl;
///		}
///
///		std::cout << "'cats': "sv << tbl["animals"]["cats"] << std::endl;
///		std::cout << "'dinosaurs': "sv << tbl["animals"]["dinosaurs"] << std::endl; //no dinosaurs :(
/// 
/// 	return 0;
/// }
/// \ecpp
/// 
/// \out
/// table has 'numbers': true
/// 'numbers' is an: array
/// 'numbers': [1, 2, 3, "four", 5.0]
/// 'numbers': [2, 3, 4, "five", 6.0, 7, [8, 9]]
/// 'cats': ["tiger", "lion", "puma"]
/// 'dinosaurs':
/// \eout
/// 
/// \see toml::node, toml::node_view, toml::array, toml::table
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-serialization Serializing as TOML and JSON
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
/// 	std::cout << "###### TOML ######"sv << std::endl;
/// 	std::cout << tbl << std::endl << std::endl;
/// 	
/// 	std::cout << "###### JSON ######"sv << std::endl;
/// 	std::cout << toml::json_formatter{ tbl } << std::endl;
/// 	return 0;
/// }
/// \ecpp
///
/// \out
/// ###### TOML ######
/// cpp = [17, 20, "and beyond"]
/// lib = "toml++"
/// repo = "https://github.com/marzer/tomlplusplus/"
/// toml = ["0.5.0", "and beyond"]
/// 
/// [author]
/// github = "https://github.com/marzer"
/// name = "Mark Gillard"
/// twitter = "https://twitter.com/marzer8789"
/// 
/// ###### JSON ######
/// {
///     "author" : {
///         "github" : "https://github.com/marzer",
///         "name" : "Mark Gillard",
///         "twitter" : "https://twitter.com/marzer8789"
///     },
///     "cpp" : [
///         17,
///         20,
///         "and beyond"
///     ],
///     "lib" : "toml++",
///     "repo" : "https://github.com/marzer/tomlplusplus/",
///     "toml" : [
///         "0.5.0",
///         "and beyond"
///     ]
/// }
/// \eout
/// \see toml::default_formatter, toml::json_formatter
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-contributing Contributing
/// See the [Contributing](https://github.com/marzer/tomlplusplus/blob/master/README.md#contributing) section of the repository README.
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-license License
///  
/// toml++ is licensed under the terms of the MIT license - see [LICENSE](https://github.com/marzer/tomlplusplus/blob/master/LICENSE).
///
/// UTF-8 decoding is performed using a state machine based on Bjoern Hoehrmann's 'Flexible and Economical UTF - 8 Decoder', which is also subject
/// to the terms of the MIT license - see [LICENSE-utf8-decoder](https://github.com/marzer/tomlplusplus/blob/master/LICENSE-utf8-decoder).
///  
/// 
