//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "formatter.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	/// \brief	A wrapper for printing TOML objects out to a stream as formatted JSON.
	///
	/// \detail \cpp
	/// auto some_toml = toml::parse(R"(
	///		[fruit]
	///		apple.color = "red"
	///		apple.taste.sweet = true
	///
	///		[fruit.apple.texture]
	///		smooth = true
	/// )"sv);
	///	std::cout << toml::json_formatter{ some_toml } << "\n";
	///
	/// \ecpp
	///
	/// \out
	/// {
	///     "fruit" : {
	///         "apple" : {
	///             "color" : "red",
	///             "taste" : {
	///                 "sweet" : true
	///             },
	///             "texture" : {
	///                 "smooth" : true
	///             }
	///         }
	///     }
	/// }
	/// \eout
	class json_formatter : impl::formatter
	{
	  private:
		/// \cond

		using base = impl::formatter;

		TOML_API
		void print(const toml::table&);

		TOML_API
		void print(const toml::array&);

		TOML_API
		void print();

		/// \endcond

	  public:
		/// \brief	The default flags for a json_formatter.
		static constexpr format_flags default_flags = format_flags::quote_dates_and_times;

		/// \brief	Constructs a JSON formatter and binds it to a TOML object.
		///
		/// \param 	source	The source TOML object.
		/// \param 	flags 	Format option flags.
		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ source, flags }
		{}

#if defined(DOXYGEN) || (TOML_PARSER && !TOML_EXCEPTIONS)

		/// \brief	Constructs a JSON formatter and binds it to a toml::parse_result.
		///
		/// \availability This constructor is only available when exceptions are disabled.
		///
		/// \attention Formatting a failed parse result will simply dump the error message out as-is.
		///		This will not be valid JSON, but at least gives you something to log or show up in diagnostics:
		/// \cpp
		/// std::cout << toml::json_formatter{ toml::parse("a = 'b'"sv) } // ok
		///           << "\n\n"
		///           << toml::json_formatter{ toml::parse("a = "sv) } // malformed
		///           << "\n";
		/// \ecpp
		/// \out
		/// {
		///     "a" : "b"
		/// }
		///
		/// Error while parsing key-value pair: encountered end-of-file
		///         (error occurred at line 1, column 5)
		/// \eout
		/// Use the library with exceptions if you want to avoid this scenario.
		///
		/// \param 	result	The parse result.
		/// \param 	flags 	Format option flags.
		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ result, flags }
		{}

#endif

		/// \brief	Prints the bound TOML object out to the stream as JSON.
		friend std::ostream& operator<<(std::ostream& lhs, json_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.print();
			rhs.detach();
			return lhs;
		}

		/// \brief	Prints the bound TOML object out to the stream as JSON (rvalue overload).
		friend std::ostream& operator<<(std::ostream& lhs, json_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
