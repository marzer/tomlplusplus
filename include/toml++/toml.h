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

/// \mainpage toml++
/// 
/// \image html banner_small.png width=1280px
/// 
/// \tableofcontents
/// 
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-features Features
/// - [TOML v1.0.0-rc.1](https://github.com/toml-lang/toml/blob/master/versions/en/toml-v1.0.0-rc.1.md),
///		plus optional support for some unreleased TOML features
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
/// optional.
/// \see \ref configuration
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
/// 			<< "Error parsing file '" << *err.source().path
/// 			<< "':\n" << err.description()
/// 			<< "\n  (" << err.source().begin << ")\n";
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
/// 			<< "Error parsing file '" << *tbl.error().source().path
/// 			<< "':\n" << tbl.error().description()
/// 			<< "\n  (" << tbl.error().source().begin << ")\n";
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
///		std::cerr << "Parsing failed:\n" << err << "\n";
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
/// \m_class{m-note m-warning}
///
/// \parblock
/// <h3>Don't forget &lt;fstream&gt;!</h3>
/// Not everyone who uses the library is going to work directly from files, so not everybody is forced to pay
/// the compilation overhead of including `<fstream>`. You need to explicitly include it if you're going to be calling
/// toml::parse_file().
/// \endparblock
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
///			authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
///		
///			[dependencies]
///			cpp = 17
///		)"sv;
///		
///		// parse directly from a string view:
///		{
///			auto tbl = toml::parse(source);
///			std::cout << tbl << "\n";
///		}
///		
///		// parse from a string stream:
///		{
///			std::stringstream ss{ std::string{ source } };
///			auto tbl = toml::parse(ss);
///			std::cout << tbl << "\n";
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
/// The error-handling semantics are the same as for toml::parse_file.
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
///		std::cout << "table has 'numbers': " << !!numbers << "\n";
///		std::cout << "numbers is a: " << numbers.type() << "\n";
///		std::cout << "numbers: " << numbers << "\n";
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
///			std::cout << "numbers: " << numbers << "\n";
///		}
///
///		// node-views can be chained to quickly query deeper
///		std::cout << "cats: " << tbl["animals"]["cats"] << "\n";
///		std::cout << "fish[1]: " << tbl["animals"]["fish"][1] << "\n";
///		
///		// ...even if the element doesn't exist
///		std::cout << "dinosaurs: " << tbl["animals"]["dinosaurs"] << "\n"; //no dinosaurs :(
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
/// 		{ "toml", toml::array{ "1.0.0-rc.1", "and beyond" } },
/// 		{ "repo", "https://github.com/marzer/tomlplusplus/" },
/// 		{ "author", toml::table{{
/// 				{ "name", "Mark Gillard" },
/// 				{ "github", "https://github.com/marzer" },
/// 				{ "twitter", "https://twitter.com/marzer8789" }
/// 			}}
/// 		},
/// 	}};
///
///		// serializing as TOML is just writing it to a stream
/// 	std::cout << "###### TOML ######" << "\n\n";
/// 	std::cout << tbl << "\n\n";
/// 
///		// serializing as JSON is _also_ just writing it to a stream, but via a json_formatter:
/// 	std::cout << "###### JSON ######" << "\n\n";
/// 	std::cout << toml::json_formatter{ tbl } << "\n\n";
/// 	return 0;
/// }
/// \ecpp
///
/// \out
/// ###### TOML ######
///
/// cpp = [17, 20, "and beyond"]
/// lib = "toml++"
/// repo = "https://github.com/marzer/tomlplusplus/"
/// toml = ["1.0.0-rc.1", "and beyond"]
/// 
/// [author]
/// github = "https://github.com/marzer"
/// name = "Mark Gillard"
/// twitter = "https://twitter.com/marzer8789"
/// 
/// ###### JSON ######
///
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
///         "1.0.0-rc.1",
///         "and beyond"
///     ]
/// }
/// \eout
/// \see
///		- toml::default_formatter
///		- toml::json_formatter
/// 
///////////////////////////////////
/// 
/// \subsection mainpage-example-speed-up-compilation Speeding up compilation
/// Because `toml++` is a header-only library of nontrivial size you might find that compilation times noticeably
/// increase after you add it to your project, especially if you add the library's header somewhere that's visible from
/// a large number of translation units. You can counter this by disabling 'all inline' mode and explicitly controlling
/// where the library's implementation is compiled.
/// 
/// <strong>Step 1: Set `TOML_ALL_INLINE` to `0` before including `toml++`</strong>
/// 
/// This must be the same everywhere, so either set it as a global `#define` in your build system, or
/// do it manually before including toml++ in some global header that's used everywhere in your project:
/// \cpp
/// // global_header_that_includes_toml++.h
/// 
/// #define TOML_ALL_INLINE 0
/// #include <toml.hpp>
/// \ecpp
/// 
/// <strong>Step 2: Define `TOML_IMPLEMENTATION` before including `toml++` in one specific translation unit</strong>
/// 
/// \cpp
/// // some_code_file.cpp
/// 
/// #define TOML_IMPLEMENTATION 
/// #include "global_header_that_includes_toml++.h"
/// \ecpp
/// 
/// Additionally, if all you need to do is serialize some code-generated TOML and don't actually need the parser at all,
/// you can `#define TOML_PARSER 0`.
///
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-contributing Contributing
/// Contributions are very welcome! Either by [reporting issues](https://github.com/marzer/tomlplusplus/issues)
/// or submitting pull requests. If you wish to submit a pull request,
/// please see [CONTRIBUTING](https://github.com/marzer/tomlplusplus/blob/master/CONTRIBUTING.md)
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
///////////////////////////////////////////////////////////////////////
/// 
/// \section mainpage-contact Contacting the author
/// For bug reports and feature requests please consider using the
/// [GitHub issues](https://github.com/marzer/tomlplusplus/issues) system. For anything else though you're welcome
/// to reach out via other means. In order of likely response speed:
/// - Twitter: [marzer8789](https://twitter.com/marzer8789)
/// - Email: [mark.gillard@outlook.com.au](mailto:mark.gillard@outlook.com.au)
/// - Facebook: [marzer](https://www.facebook.com/marzer)
/// - LinkedIn: [marzer](https://www.linkedin.com/in/marzer/)
/// 

//# {{
#endif // INCLUDE_TOMLPLUSPLUS_H
//# }}
