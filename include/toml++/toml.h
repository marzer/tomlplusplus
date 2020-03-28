//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

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
#include "toml_utf8_generated.h"
#include "toml_utf8.h"
#include "toml_parser.h"
#include "toml_formatter.h"
#include "toml_default_formatter.h"
#include "toml_json_formatter.h"

#if TOML_IMPLEMENTATION

#include "toml_instantiations.h"
#include "toml_node_impl.h"
#include "toml_array_impl.h"
#include "toml_table_impl.h"
#include "toml_parser_impl.h"

#endif

// macro hygiene
#if TOML_UNDEF_MACROS
	#undef TOML_USE_STREAMS_FOR_FLOATS
	#undef TOML_GNU_ATTR
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
	#undef TOML_LANG_EXACTLY
	#undef TOML_STRING_PREFIX_1
	#undef TOML_STRING_PREFIX
	#undef TOML_UNDEF_MACROS
	#undef TOML_RELOPS_REORDERING
	#undef TOML_ASYMMETRICAL_EQUALITY_OPS
	#undef TOML_ALL_INLINE
	#undef TOML_IMPLEMENTATION
	#undef TOML_INLINE_FUNC_IMPL
	#undef TOML_COMPILER_EXCEPTIONS
	#undef TOML_LAUNDER
	#undef TOML_TRIVIAL_ABI
	#undef TOML_ABI_NAMESPACES
	#undef TOML_PARSER_TYPENAME
#endif

/// \mainpage toml++
/// 
/// \image html tomlplusplus-banner-small.png width=1280px
/// 
/// \tableofcontents
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-features Features
/// - [TOML v0.5.0](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md), plus optional support for some
///		unreleased TOML features
/// - C++17 (plus some C++20 features where available, e.g. experimental support for char8_t strings)
/// - Proper UTF-8 handling (incl. BOM)
/// - Works with or without exceptions
/// - Doesn't require RTTI
/// - First-class support for serializing to JSON  
/// - Tested on Clang, GCC and MSVC (VS2019)  
/// - Tested on x64, x86 and ARM
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-adding-lib Adding toml++ to your project
/// It's header-only library so really all you have to do is clone
/// [the repository](https://github.com/marzer/tomlplusplus/) from GitHub and set your include paths.
/// There's some minor configuration you can do to customize some basic library functionality, but that's totally
/// optional. See the [README](https://github.com/marzer/tomlplusplus/blob/master/README.md) for more info.
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-api-documentation API Documentation
/// You're looking at it! Browse the docs using the links at the top of the page.
/// You can search from anywhere by pressing the TAB key.
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
/// a successful call to a parsing function simply returns a toml::table, whereas a failed call sees a toml::parse_error
/// thrown directly from the site of the error:
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
/// When exceptions are disabled parsing functions return a toml::parse_result instead and it is up to the caller
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
/// 
/// Instances of toml::parse_error can be printed directly to streams:
/// \cpp
/// try
///	{
///		auto tbl = toml::parse("enabled = trUe"sv); //fails; TOML booleans are case-sensitive
///	}
///	catch (const toml::parse_error & err)
///	{
///		std::cerr << "Parsing failed:\n"sv << err << std::endl;
///		return 1;
///	}
/// \ecpp
/// 	 
/// \out
/// Parsing failed:
/// Encountered unexpected character while parsing boolean; expected 'true', saw 'trU'
///		(error occurred at line 1, column 13)
/// \eout
/// 
/// If the default error formatting is not be suitable for your use-case you can access the error's
/// toml::source_region and description directly from the error object (as in the examples above).
/// 
/// \see
///		- toml::parse_file()  
///		- toml::parse_result  
///		- toml::parse_error  
///		- toml::source_region  
///		- toml::source_position
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-parsing-strings Parsing TOML directly from strings and streams
/// Strings and std::istreams can be read directly using toml::parse():
/// \cpp
/// #include <iostream>
/// #include <sstream>
/// #include <toml++/toml.h>
/// using namespace std::string_view_literals;
/// 
/// int main()
/// {
///		static constexpr auto source = R"(
///			[library]
///			name = "toml++"
///			authors = ["Mark Gillard <mark@notarealwebsite.com>"]
///		
///			[dependencies]
///			cpp = 17
///		)"sv;
///		
///		// parse directly from a string view:
///		{
///			auto tbl = toml::parse(source);
///			std::cout << tbl << std::endl;
///		}
///		
///		// parse from a string stream:
///		{
///			std::stringstream ss{ std::string{ source } };
///			auto tbl = toml::parse(ss);
///			std::cout << tbl << std::endl;
///		}
///		
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
/// 
/// ... exactly as above, but twice
/// \eout
/// 
/// \see toml::parse()  
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-manipulations Traversing and manipulating data
/// A TOML document is a tree of values, arrays and tables, represented as the toml::value, toml::array
/// and toml::table, respectively. All three inherit from toml::node, and can be easily accessed via
/// the toml::node_view:
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
///		// get a view of the element 'numbers'
///		auto numbers = tbl["numbers"];
///		std::cout << "table has 'numbers': "sv << !!numbers << std::endl;
///		std::cout << "numbers is a: "sv << numbers.type() << std::endl;
///		std::cout << "numbers: "sv << numbers << std::endl;
///
///		// get the underlying array object to do some more advanced stuff
///		if (auto arr = numbers.as_array())
///		{
///			for (auto& elem : *arr)
///			{
///				// visitation helps deal with the polymorphic nature of TOML data
///				elem.visit([](auto&& el) noexcept
///				{
///					if constexpr (toml::is_number<decltype(el)>)
///						(*el)++;
///					else if constexpr (toml::is_string<decltype(el)>)
///						el = "five"sv;
///				});
///			}
///			
///			// arrays are very similar to std::vector
///			arr->push_back(7);
///			arr->emplace_back<toml::array>(8, 9);
///			std::cout << "numbers: "sv << numbers << std::endl;
///		}
///
///		// node-views can be chained to quickly query deeper
///		std::cout << "cats: "sv << tbl["animals"]["cats"] << std::endl;
///		std::cout << "fish[1]: "sv << tbl["animals"]["fish"][1] << std::endl;
///		
///		// ...even if the element doesn't exist
///		std::cout << "dinosaurs: "sv << tbl["animals"]["dinosaurs"] << std::endl; //no dinosaurs :(
/// 
/// 	return 0;
/// }
/// \ecpp
/// 
/// \out
/// table has 'numbers': true
/// numbers is an: array
/// numbers: [1, 2, 3, "four", 5.0]
/// numbers: [2, 3, 4, "five", 6.0, 7, [8, 9]]
/// cats: ["tiger", "lion", "puma"]
/// fish[1]: "trout"
/// dinosaurs:
/// \eout
/// 
/// \see
/// 	- toml::node
/// 	- toml::node_view  
/// 	- toml::value
/// 	- toml::array
/// 	- toml::table
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
/// \see
///		- toml::default_formatter
///		- toml::json_formatter
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-contributing Contributing
/// Contributions are very welcome! Either by [reporting issues](https://github.com/marzer/tomlplusplus/issues) or submitting pull requests.
/// If you wish to submit a pull request, please see [CONTRIBUTING](https://github.com/marzer/tomlplusplus/blob/master/CONTRIBUTING.md)
/// for all the details you need to get going.
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-license License
///  
/// toml++ is licensed under the terms of the MIT license - see
/// [LICENSE](https://github.com/marzer/tomlplusplus/blob/master/LICENSE).
///
/// \m_class{m-note m-default}
/// 
/// If you're using the single-header version of the library you don't need to explicitly distribute the license file;
/// it is embedded in the preamble at the top of the header.
///
