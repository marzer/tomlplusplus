//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_preprocessor.h"

////////// INCLUDES

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS

#if __has_include(<version>)
	#include <version>
#endif
#include <cstdint>
#include <cstring>		//memcpy, memset
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#ifndef TOML_OPTIONAL_TYPE
	#include <optional>
#endif
#ifndef TOML_ASSERT
	#ifdef NDEBUG
		#define TOML_ASSERT(expr)	(void)0
	#else
		#include <cassert>
		#define TOML_ASSERT(expr)	assert(expr)
	#endif
#endif

TOML_POP_WARNINGS

#if TOML_CHAR_8_STRINGS && !defined(__cpp_lib_char8_t)
	#error toml++ requires implementation support to use char8_t strings, but yours does not provide it.
#endif

#ifdef __cpp_lib_launder
	#define TOML_LAUNDER(x)	std::launder(x)
#else
	#define TOML_LAUNDER(x)	x
#endif

////////// FORWARD DECLARATIONS & TYPEDEFS

/// \brief	The root namespace for all toml++ functions and types.
namespace toml
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	TOML_CONSTEVAL size_t operator"" _sz(unsigned long long n) noexcept
	{
		return static_cast<size_t>(n);
	}

	#if TOML_CHAR_8_STRINGS

	using string_char = char8_t;
	using string = std::u8string;
	using string_view = std::u8string_view;

	#else

	/// \brief	The base character type for keys and string values.
	/// \remarks This will be an alias for char8_t if #TOML_CHAR_8_STRINGS is enabled.
	using string_char = char;

	/// \brief	The string type for keys and string values.
	/// \remarks This will be an alias for std::u8string if #TOML_CHAR_8_STRINGS is enabled.
	using string = std::string;

	/// \brief	The string type for keys and string values.
	/// \remarks This will be an alias for std::u8string_view if #TOML_CHAR_8_STRINGS is enabled.
	using string_view = std::string_view;

	#endif

	TOML_PUSH_WARNINGS
	TOML_DISABLE_PADDING_WARNINGS
	TOML_DISABLE_SHADOW_WARNINGS // false positive on gcc

	#if !TOML_DOXYGEN

	// foward declarations are hidden from doxygen
	// because they fuck it up =/
	struct date;
	struct time;
	struct time_offset;
	class node;
	class array;
	class table;
	template <typename> class node_view;
	template <typename> class value;
	template <typename> class default_formatter;
	template <typename> class json_formatter;

	#ifdef TOML_OPTIONAL_TYPE
		TOML_ABI_NAMESPACE_START(custopt)
	#else
		TOML_ABI_NAMESPACE_START(stdopt)
	#endif

	struct date_time; 

	TOML_ABI_NAMESPACE_END // TOML_OPTIONAL_TYPE

	#endif // !TOML_DOXYGEN

	/// \brief	TOML node type identifiers.
	enum class node_type : uint8_t
	{
		none, ///< Not-a-node.
		table, ///< The node is a toml::table.
		array,  ///< The node is a toml::array.
		string,  ///< The node is a toml::value<toml::string>.
		integer,  ///< The node is a toml::value<int64_t>.
		floating_point,  ///< The node is a toml::value<double>.
		boolean,  ///< The node is a toml::value<bool>.
		date,  ///< The node is a toml::value<date>.
		time,  ///< The node is a toml::value<time>.
		date_time  ///< The node is a toml::value<date_time>.
	};

	TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS, TOML_DISABLE_SHADOW_WARNINGS

	#ifdef TOML_OPTIONAL_TYPE

	template <typename T>
	using optional = TOML_OPTIONAL_TYPE<T>;

	#else

	/// \brief	The 'optional' type used throughout the library.
	/// 
	/// \remarks By default this will be a simple alias for std::optional, but you can change the optional type
	/// 		 used by the library by defining #TOML_OPTIONAL_TYPE.
	template <typename T>
	using optional = std::optional<T>;

	#endif

	#if TOML_LARGE_FILES

	using source_index = uint32_t;

	#else

	/// \brief	The integer type used to tally line numbers and columns.
	/// \remarks This will be an alias for uint32_t if #TOML_LARGE_FILES is enabled.
	using source_index = uint16_t;

	#endif

	/// \brief	A pointer to a shared string resource containing a source path.
	using source_path_ptr = std::shared_ptr<const std::string>;

	#if TOML_LARGE_FILES
		TOML_ABI_NAMESPACE_START(lf)
	#else
		TOML_ABI_NAMESPACE_START(sf)
	#endif

	/// \brief	A source document line-and-column pair.
	/// 
	/// \detail \cpp
	/// auto table = toml::parse_file("config.toml"sv);
	/// std::cout << "The node 'description' was defined at "sv
	///		<< table.get("description")->source().begin()
	///		<< std::endl;
	///	
	/// \ecpp
	/// 
	/// \out
	///	The value 'description' was defined at line 7, column 15
	/// \eout
	/// 
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 various non-conventional whitespace and newline characters, but it doesn't give
	/// 		 much thought to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct TOML_TRIVIAL_ABI source_position
	{
		/// \brief The line number.
		/// \remarks Valid line numbers start at 1.
		source_index line;

		/// \brief The column number.
		/// \remarks Valid column numbers start at 1.
		source_index column;

		/// \brief	Returns true if both line and column numbers are non-zero.
		[[nodiscard]]
		explicit constexpr operator bool () const noexcept
		{
			return line > source_index{} && column > source_index{};
		}

		/// \brief	Returns true if two source_positions represent the same line and column.
		[[nodiscard]]
		friend constexpr bool operator == (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line
				&& lhs.column == rhs.column;
		}

		/// \brief	Returns true if two source_positions do not represent the same line and column.
		[[nodiscard]]
		friend constexpr bool operator != (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line != rhs.line
				|| lhs.column != rhs.column;
		}

		/// \brief	Returns true if the LHS position is before the RHS position.
		[[nodiscard]]
		friend constexpr bool operator < (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line
				|| (lhs.line == rhs.line && lhs.column < rhs.column);
		}

		/// \brief	Returns true if the LHS position is before the RHS position or equal to it.
		[[nodiscard]]
		friend constexpr bool operator <= (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line
				|| (lhs.line == rhs.line && lhs.column <= rhs.column);
		}
	};

	/// \brief	A source document region.
	/// 
	/// \detail \cpp
	/// #include <fstream>
	/// 
	/// auto tbl = toml::parse_file("config.toml"sv);
	/// if (auto server = tbl.get("server"))
	/// {
	///		std::cout << "begin: "sv << server->source().begin << std::endl;
	///		std::cout << "end: "sv << server->source().end << std::endl;
	///		std::cout << "path: "sv << *server->source().path << std::endl;
	///	}
	///	
	/// \ecpp
	/// 
	/// \out
	///	begin: line 3, column 1
	///	end: line 3, column 22
	///	path: config.toml
	/// \eout
	/// 
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 various non-conventional whitespace and newline characters, but it doesn't give
	/// 		 much thought to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct source_region
	{
		/// \brief The beginning of the region (inclusive).
		source_position begin;

		/// \brief The end of the region (exclusive).
		source_position end;

 		/// \brief	The path to the corresponding source document.
 		/// 
 		/// \remarks This will be `nullptr` if no path was provided to toml::parse().
		source_path_ptr path;
	};

	TOML_ABI_NAMESPACE_END // TOML_LARGE_FILES
}

namespace toml::impl
{
	template <typename T>
	using string_map = std::map<string, T, std::less<>>; //heterogeneous lookup

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename T, typename... U>
	struct is_one_of_ : std::integral_constant<bool,
		(false || ... || std::is_same_v<T, U>)
	> {};

	template <typename T, typename... U>
	inline constexpr bool is_one_of = is_one_of_<T, U...>::value;

	template <typename T>
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr std::underlying_type_t<T> unbox_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use std::find??"
	// A: Because <algorithm> is _huge_ and std::find would be the only thing I used from it.
	//    I don't want to impose such a heavy compile-time burden on users.

	template <typename T>
	[[nodiscard]]
	inline const T* find(const std::vector<T>& haystack, const T& needle) noexcept
	{
		for (size_t i = 0, e = haystack.size(); i < e; i++)
			if (haystack[i] == needle)
				return haystack.data() + i;
		return nullptr;
	}

	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(impl_ex)
			#define TOML_PARSER_TYPENAME ::toml::impl::abi_impl_ex::parser
		#else
			TOML_ABI_NAMESPACE_START(impl_noex)
			#define TOML_PARSER_TYPENAME ::toml::impl::abi_impl_noex::parser
		#endif
	#else
		#define TOML_PARSER_TYPENAME ::toml::impl::parser
	#endif

	class parser;

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	template <typename T>
	inline constexpr bool is_value =
		std::is_same_v<T, string>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, double>
		|| std::is_same_v<T, bool>
		|| std::is_same_v<T, date>
		|| std::is_same_v<T, time>
		|| std::is_same_v<T, date_time>;

	template <typename T>
	inline constexpr bool is_value_or_promotable =
		is_value<T>
		|| std::is_same_v<std::decay_t<T>, string_char*>
		|| std::is_same_v<T, string_view>
		|| std::is_same_v<T, int32_t>
		|| std::is_same_v<T, int16_t>
		|| std::is_same_v<T, int8_t>
		|| std::is_same_v<T, uint32_t>
		|| std::is_same_v<T, uint16_t>
		|| std::is_same_v<T, uint8_t>
		|| std::is_same_v<T, float>
		#ifdef TOML_SMALL_FLOAT_TYPE
		|| std::is_same_v<T, TOML_SMALL_FLOAT_TYPE>
		#endif
		#ifdef TOML_SMALL_INT_TYPE
		|| std::is_same_v<T, TOML_SMALL_INT_TYPE>
		#endif
	;

	template <typename T>
	inline constexpr bool is_value_or_node =
		is_value<T>
		|| std::is_same_v<T, array>
		|| std::is_same_v<T, table>;

	template <typename T> struct node_wrapper { using type = T; };
	template <> struct node_wrapper<string> { using type = value<string>; };
	template <> struct node_wrapper<int64_t> { using type = value<int64_t>; };
	template <> struct node_wrapper<double> { using type = value<double>; };
	template <> struct node_wrapper<bool> { using type = value<bool>; };
	template <> struct node_wrapper<date> { using type = value<date>; };
	template <> struct node_wrapper<time> { using type = value<time>; };
	template <> struct node_wrapper<date_time> { using type = value<date_time>; };

	template <typename T>
	using node_of = typename impl::node_wrapper<T>::type;

	template <typename T> struct node_unwrapper { using type = T; };
	template <typename T> struct node_unwrapper<value<T>> { using type = T; };

	template <typename T> using unwrapped = typename impl::node_unwrapper<T>::type;

	template <typename T> struct value_promoter { using type = T; };
	template <size_t N> struct value_promoter<const string_char[N]> { using type = string; };
	template <size_t N> struct value_promoter<const string_char(&)[N]> { using type = string; };
	template <size_t N> struct value_promoter<const string_char(&&)[N]> { using type = string; };
	template <> struct value_promoter<const string_char*> { using type = string; };
	template <size_t N> struct value_promoter<string_char[N]> { using type = string; };
	template <size_t N> struct value_promoter<string_char(&)[N]> { using type = string; };
	template <size_t N> struct value_promoter<string_char(&&)[N]> { using type = string; };
	template <> struct value_promoter<string_char*> { using type = string; };
	template <> struct value_promoter<string_view> { using type = string; };
	template <> struct value_promoter<int32_t> { using type = int64_t; };
	template <> struct value_promoter<int16_t> { using type = int64_t; };
	template <> struct value_promoter<int8_t> { using type = int64_t; };
	template <> struct value_promoter<uint32_t> { using type = int64_t; };
	template <> struct value_promoter<uint16_t> { using type = int64_t; };
	template <> struct value_promoter<uint8_t> { using type = int64_t; };
	template <> struct value_promoter<float> { using type = double; };
	#ifdef TOML_SMALL_FLOAT_TYPE
		template <> struct value_promoter<TOML_SMALL_FLOAT_TYPE> { using type = double; };
	#endif
	#ifdef TOML_SMALL_INT_TYPE
		template <> struct value_promoter<TOML_SMALL_INT_TYPE> { using type = int64_t; };
	#endif
	template <typename T> using promoted = typename impl::value_promoter<T>::type;

	template <typename T> struct node_type_of_;
	template <> struct node_type_of_<table> { static constexpr auto value = node_type::table; };
	template <> struct node_type_of_<array> { static constexpr auto value = node_type::array; };
	template <> struct node_type_of_<string> { static constexpr auto value = node_type::string; };
	template <> struct node_type_of_<int64_t> { static constexpr auto value = node_type::integer; };
	template <> struct node_type_of_<double> { static constexpr auto value = node_type::floating_point; };
	template <> struct node_type_of_<bool> { static constexpr auto value = node_type::boolean; };
	template <> struct node_type_of_<date> { static constexpr auto value = node_type::date; };
	template <> struct node_type_of_<time> { static constexpr auto value = node_type::time; };
	template <> struct node_type_of_<date_time> { static constexpr auto value = node_type::date_time; };

	template <typename T>
	inline constexpr auto node_type_of = node_type_of_<promoted<typename node_unwrapper<remove_cvref_t<T>>::type>>::value;

	inline constexpr toml::string_view low_character_escape_table[] =
	{
		TOML_STRING_PREFIX("\\u0000"sv),
		TOML_STRING_PREFIX("\\u0001"sv),
		TOML_STRING_PREFIX("\\u0002"sv),
		TOML_STRING_PREFIX("\\u0003"sv),
		TOML_STRING_PREFIX("\\u0004"sv),
		TOML_STRING_PREFIX("\\u0005"sv),
		TOML_STRING_PREFIX("\\u0006"sv),
		TOML_STRING_PREFIX("\\u0007"sv),
		TOML_STRING_PREFIX("\\b"sv),
		TOML_STRING_PREFIX("\\t"sv),
		TOML_STRING_PREFIX("\\n"sv),
		TOML_STRING_PREFIX("\\u000B"sv),
		TOML_STRING_PREFIX("\\f"sv),
		TOML_STRING_PREFIX("\\r"sv),
		TOML_STRING_PREFIX("\\u000E"sv),
		TOML_STRING_PREFIX("\\u000F"sv),
		TOML_STRING_PREFIX("\\u0010"sv),
		TOML_STRING_PREFIX("\\u0011"sv),
		TOML_STRING_PREFIX("\\u0012"sv),
		TOML_STRING_PREFIX("\\u0013"sv),
		TOML_STRING_PREFIX("\\u0014"sv),
		TOML_STRING_PREFIX("\\u0015"sv),
		TOML_STRING_PREFIX("\\u0016"sv),
		TOML_STRING_PREFIX("\\u0017"sv),
		TOML_STRING_PREFIX("\\u0018"sv),
		TOML_STRING_PREFIX("\\u0019"sv),
		TOML_STRING_PREFIX("\\u001A"sv),
		TOML_STRING_PREFIX("\\u001B"sv),
		TOML_STRING_PREFIX("\\u001C"sv),
		TOML_STRING_PREFIX("\\u001D"sv),
		TOML_STRING_PREFIX("\\u001E"sv),
		TOML_STRING_PREFIX("\\u001F"sv),
	};

	inline constexpr std::string_view node_type_friendly_names[] =
	{
		"none"sv,
		"table"sv,
		"array"sv,
		"string"sv,
		"integer"sv,
		"floating-point"sv,
		"boolean"sv,
		"date"sv,
		"time"sv,
		"date-time"sv
	};

	#define TOML_P2S_DECL(Linkage, Type)								\
		template <typename Char>										\
		Linkage void print_to_stream(Type, std::basic_ostream<Char>&)

	TOML_P2S_DECL(TOML_ALWAYS_INLINE, int8_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, int16_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, int32_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, int64_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, uint8_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, uint16_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, uint32_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, uint64_t);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, float);
	TOML_P2S_DECL(TOML_ALWAYS_INLINE, double);
	TOML_P2S_DECL(inline, const date&);
	TOML_P2S_DECL(inline, const time&);
	TOML_P2S_DECL(inline, time_offset);
	TOML_P2S_DECL(inline, const date_time&);

	#undef TOML_P2S_DECL
}

namespace toml
{
	/// \brief	Metafunction for determining if a type is a toml::table.
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref_t<T>, table>;
	/// \brief	Metafunction for determining if a type is a toml::array.
	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref_t<T>, array>;
	/// \brief	Metafunction for determining if a type is a toml::string or toml::value<toml::string>.
	template <typename T>
	inline constexpr bool is_string = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<string>>;
	/// \brief	Metafunction for determining if a type is an int64_t or toml::value<int64_t>.
	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<int64_t>>;
	/// \brief	Metafunction for determining if a type is a double or toml::value<double>.
	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<double>>;
	/// \brief	Metafunction for determining if a type satisfies `toml::is_integer || toml::is_floating_point`.
	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;
	/// \brief	Metafunction for determining if a type is a bool toml::value<bool>.
	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<bool>>;
	/// \brief	Metafunction for determining if a type is a toml::date or toml::value<date>.
	template <typename T>
	inline constexpr bool is_date = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<date>>;
	/// \brief	Metafunction for determining if a type is a toml::time or toml::value<time>.
	template <typename T>
	inline constexpr bool is_time = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<time>>;
	/// \brief	Metafunction for determining if a type is a toml::date_time or toml::value<date_time>.
	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<date_time>>;

	/// \brief	Pretty-prints the value of a node_type to a stream.
	/// 
	/// \detail \cpp
	/// auto arr = toml::array{ 1, 2.0, "3", false };
	/// for (size_t i = 0; i < arr.size() i++)
	/// 	std::cout << "Element ["sv << i << "] is: "sv << arr[i].type() << std::endl;
	///
	/// \ecpp
	/// 
	/// \out
	/// Element [0] is: integer
	/// Element [1] is: floating-point
	/// Element [2] is: string
	/// Element [3] is: boolean
	/// \eout
	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, node_type rhs)
	{
		using underlying_t = std::underlying_type_t<node_type>;
		const auto str = impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
		if constexpr (std::is_same_v<Char, char>)
			return lhs << str;
		else
		{
			if constexpr (sizeof(Char) == 1)
				return lhs << std::basic_string_view<Char>{ reinterpret_cast<const Char*>(str.data()), str.length() };
			else
				return lhs << str.data();
		}
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, node_type);
	#endif
}
