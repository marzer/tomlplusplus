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
#include <cfloat>
#include <climits>
#include <limits>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#if !TOML_HAS_CUSTOM_OPTIONAL_TYPE
	#include <optional>
#endif

TOML_POP_WARNINGS

#ifdef __cpp_lib_launder
	#define TOML_LAUNDER(x)	std::launder(x)
#else
	#define TOML_LAUNDER(x)	x
#endif

////////// ENVIRONMENT GROUND-TRUTHS

static_assert(CHAR_BIT == 8);
static_assert(FLT_RADIX == 2);
static_assert('A' == 65);
static_assert(sizeof(double) == 8);
static_assert(std::numeric_limits<double>::is_iec559);
static_assert(std::numeric_limits<double>::digits == 53);
static_assert(std::numeric_limits<double>::digits10 == 15);
static_assert(std::numeric_limits<double>::max_digits10 == 17);

////////// FORWARD DECLARATIONS & TYPEDEFS

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS
TOML_DISABLE_SHADOW_WARNINGS

/// \brief	The root namespace for all toml++ functions and types.
namespace toml { TOML_ABI_NAMESPACE_VERSION TOML_ABI_NAMESPACE_END }

#if TOML_WINDOWS_COMPAT
namespace toml
{
	TOML_IMPL_NAMESPACE_START

	[[nodiscard]] TOML_API std::string narrow(std::wstring_view) noexcept;
	[[nodiscard]] TOML_API std::wstring widen(std::string_view) noexcept;
	#ifdef __cpp_lib_char8_t
	[[nodiscard]] TOML_API std::wstring widen(std::u8string_view) noexcept;
	#endif

	TOML_IMPL_NAMESPACE_END
}
#endif // TOML_WINDOWS_COMPAT

namespace toml
{
	TOML_ABI_NAMESPACE_VERSION

	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;

	[[nodiscard]]
	TOML_ATTR(const)
	TOML_ALWAYS_INLINE
	TOML_CONSTEVAL size_t operator"" _sz(unsigned long long n) noexcept
	{
		return static_cast<size_t>(n);
	}

	// legacy typedefs
	using string_char = char;
	using string = std::string;
	using string_view = std::string_view;

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

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt)

	struct date_time; 

	TOML_ABI_NAMESPACE_END // TOML_HAS_CUSTOM_OPTIONAL_TYPE

	#endif // !TOML_DOXYGEN

	/// \brief	TOML node type identifiers.
	enum class node_type : uint8_t
	{
		none, ///< Not-a-node.
		table, ///< The node is a toml::table.
		array,  ///< The node is a toml::array.
		string,  ///< The node is a toml::value<std::string>.
		integer,  ///< The node is a toml::value<int64_t>.
		floating_point,  ///< The node is a toml::value<double>.
		boolean,  ///< The node is a toml::value<bool>.
		date,  ///< The node is a toml::value<date>.
		time,  ///< The node is a toml::value<time>.
		date_time  ///< The node is a toml::value<date_time>.
	};

	#if TOML_HAS_CUSTOM_OPTIONAL_TYPE

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

	/// \brief	A pointer to a shared string resource containing a source path.
	using source_path_ptr = std::shared_ptr<const std::string>;

	TOML_ABI_NAMESPACE_BOOL(TOML_LARGE_FILES, lf, sf)

	#if TOML_LARGE_FILES
	using source_index = uint32_t;
	#else
	/// \brief	The integer type used to tally line numbers and columns.
	/// \remarks This will be an alias for uint32_t if #TOML_LARGE_FILES is enabled.
	using source_index = uint16_t;
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

		#if TOML_WINDOWS_COMPAT

		/// \brief	The path to the corresponding source document as a wide-string.
		///
		/// \remarks This will return an empty optional if no path was provided to toml::parse().
		/// 
		/// \attention This function is only available when #TOML_WINDOWS_COMPAT is enabled.
		[[nodiscard]]
		optional<std::wstring> wide_path() const noexcept
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

		#endif
	};

	TOML_ABI_NAMESPACE_END // TOML_LARGE_FILES

	TOML_ABI_NAMESPACE_END // version

} // toml

namespace toml
{
	TOML_IMPL_NAMESPACE_START

	template <typename T>
	using string_map = std::map<std::string, T, std::less<>>; // heterogeneous lookup

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename T, typename... U>
	struct is_one_of_ : std::integral_constant<bool,
		(false || ... || std::is_same_v<T, U>)
	> {};

	template <typename T, typename... U>
	inline constexpr bool is_one_of = is_one_of_<T, U...>::value;

	template <typename T>
	inline constexpr bool is_cvref = std::is_reference_v<T> || std::is_const_v<T> || std::is_volatile_v<T>;

	template <typename T>
	[[nodiscard]]
	TOML_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr std::underlying_type_t<T> unbox_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use the built-in fpclassify?"
	// A: Because it gets broken by -ffast-math and friends

	enum class fp_class : unsigned { ok, neg_inf, pos_inf, nan };

	[[nodiscard]]
	inline fp_class fpclassify(const double& val) noexcept
	{
		constexpr uint64_t sign     = 0b1000000000000000000000000000000000000000000000000000000000000000ull;
		constexpr uint64_t exponent = 0b0111111111110000000000000000000000000000000000000000000000000000ull;
		constexpr uint64_t mantissa = 0b0000000000001111111111111111111111111111111111111111111111111111ull;

		uint64_t val_bits;
		memcpy(&val_bits, &val, sizeof(val));
		if ((val_bits & exponent) != exponent)
			return fp_class::ok;
		if ((val_bits & mantissa))
			return fp_class::nan;
		return (val_bits & sign) ? fp_class::neg_inf : fp_class::pos_inf;
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
			TOML_ABI_NAMESPACE_START(ex)
			#define TOML_PARSER_TYPENAME ::toml::impl::ex::parser
		#else
			TOML_ABI_NAMESPACE_START(noex)
			#define TOML_PARSER_TYPENAME ::toml::impl::noex::parser
		#endif
	#else
		#define TOML_PARSER_TYPENAME ::toml::impl::parser
	#endif

	class parser;

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	// general value traits
	// (as they relate to their equivalent native TOML type)

	template <typename T>
	struct value_traits
	{
		using native_type = void;
		static constexpr bool is_native = false;
		static constexpr bool is_losslessly_convertible_to_native = false;
		static constexpr bool can_represent_native = false;
		static constexpr bool can_partially_represent_native = false;
		static constexpr auto node_type = ::toml::node_type::none;
	};
	template <typename T> struct value_traits<T&>				: value_traits<T> {};
	template <typename T> struct value_traits<T&&>				: value_traits<T> {};
	template <typename T> struct value_traits<T*const>			: value_traits<T*>{};
	template <typename T> struct value_traits<T*volatile>		: value_traits<T*>{};
	template <typename T> struct value_traits<T*const volatile>	: value_traits<T*>{};

	// integer value traits

	template <typename T>
	struct unsigned_integer_value_traits
	{
		using native_type = int64_t;
		static constexpr bool is_native = false;
		static constexpr bool is_signed = false;
		static constexpr auto min = (std::numeric_limits<T>::min)();
		static constexpr auto max = (std::numeric_limits<T>::max)();
		static constexpr bool is_losslessly_convertible_to_native
			= (std::numeric_limits<T>::max)() <= 9223372036854775807ULL;
		static constexpr bool can_represent_native = false;
		static constexpr bool can_partially_represent_native = true;
		static constexpr auto node_type = ::toml::node_type::integer;
	};
	template <typename T>
	struct signed_integer_value_traits
	{
		using native_type = int64_t;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = true;
		static constexpr auto min = (std::numeric_limits<T>::min)();
		static constexpr auto max = (std::numeric_limits<T>::max)();
		static constexpr bool is_losslessly_convertible_to_native
			 = (std::numeric_limits<T>::min)() >= (-9223372036854775807LL - 1LL)
			&& (std::numeric_limits<T>::max)() <= 9223372036854775807LL;
		static constexpr bool can_represent_native
			 = (std::numeric_limits<T>::min)() <= (-9223372036854775807LL - 1LL)
			&& (std::numeric_limits<T>::max)() >= 9223372036854775807LL;
		static constexpr bool can_partially_represent_native = true;
		static constexpr auto node_type = ::toml::node_type::integer;
	};

	template <typename T, bool U = std::is_unsigned_v<T>>
	struct integer_value_traits : signed_integer_value_traits<T> {};
	template <typename T>
	struct integer_value_traits<T, true> : unsigned_integer_value_traits<T> {};

	template <> struct value_traits<signed char>		: integer_value_traits<signed char> {};
	template <> struct value_traits<unsigned char>		: integer_value_traits<unsigned char> {};
	template <> struct value_traits<signed short>		: integer_value_traits<signed short> {};
	template <> struct value_traits<unsigned short>		: integer_value_traits<unsigned short> {};
	template <> struct value_traits<signed int>			: integer_value_traits<signed int> {};
	template <> struct value_traits<unsigned int>		: integer_value_traits<unsigned int> {};
	template <> struct value_traits<signed long>		: integer_value_traits<signed long> {};
	template <> struct value_traits<unsigned long>		: integer_value_traits<unsigned long> {};
	template <> struct value_traits<signed long long>	: integer_value_traits<signed long long> {};
	template <> struct value_traits<unsigned long long>	: integer_value_traits<unsigned long long> {};
	#ifdef __SIZEOF_INT128__
	template <typename T>
	struct big_integer_value_traits
	{
		using native_type = int64_t;
		static constexpr bool is_native = false;
		static constexpr bool is_losslessly_convertible_to_native = false;
		static constexpr bool is_signed = static_cast<T>(-1) < T{}; // for impls not specializing std::is_signed<T>
		static constexpr bool can_represent_native = is_signed;
		static constexpr bool can_partially_represent_native = true;
		static constexpr auto node_type = ::toml::node_type::integer;
	};
	template <>
	struct value_traits<__int128_t> : big_integer_value_traits<__int128_t>
	{
		static constexpr auto max = static_cast<__int128_t>(
			( __uint128_t{ 1u } << ((__SIZEOF_INT128__ * CHAR_BIT) - 1)) - 1
		);
		static constexpr auto min = -max - __int128_t{ 1 };
	};
	template <>
	struct value_traits<__uint128_t> : big_integer_value_traits<__uint128_t>
	{
		static constexpr auto min = __uint128_t{};
		static constexpr auto max = (2u * static_cast<__uint128_t>(value_traits<__int128_t>::max)) + 1u;
	};
	#endif
	#ifdef TOML_SMALL_INT_TYPE
	template <> struct value_traits<TOML_SMALL_INT_TYPE> : signed_integer_value_traits<TOML_SMALL_INT_TYPE> {};
	#endif
	static_assert(value_traits<int64_t>::is_native);
	static_assert(value_traits<int64_t>::is_losslessly_convertible_to_native);
	static_assert(value_traits<int64_t>::can_represent_native);
	static_assert(value_traits<int64_t>::can_partially_represent_native);

	// float value traits

	template <typename T>
	struct float_value_traits
	{
		using native_type = double;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = true;
		static constexpr bool is_losslessly_convertible_to_native
			= std::numeric_limits<T>::is_iec559
			&& std::numeric_limits<T>::digits <= 53
			&& std::numeric_limits<T>::digits10 <= 15
			&& std::numeric_limits<T>::max_digits10 <= 17;
		static constexpr bool can_represent_native
			= std::numeric_limits<T>::is_iec559
			&& std::numeric_limits<T>::digits >= 53
			&& std::numeric_limits<T>::digits10 >= 15
			&& std::numeric_limits<T>::max_digits10 >= 17;
		static constexpr bool can_partially_represent_native //32-bit float values
			= std::numeric_limits<T>::is_iec559
			&& std::numeric_limits<T>::digits >= 24
			&& std::numeric_limits<T>::digits10 >= 6
			&& std::numeric_limits<T>::max_digits10 >= 9;
		static constexpr auto node_type = ::toml::node_type::floating_point;
	};
	template <> struct value_traits<float>					: float_value_traits<float> {};
	template <> struct value_traits<double>					: float_value_traits<double> {};
	template <> struct value_traits<long double>			: float_value_traits<long double> {};
	#if defined(FLT16_MANT_DIG) && defined(FLT16_DIG) && defined(FLT16_DECIMAL_DIG)
	template <> struct value_traits<_Float16>				: float_value_traits<_Float16> {};
	#endif
	//#ifdef __SIZEOF_FLOAT80__
	//template <> struct value_traits<__float80>				: float_value_traits<__float80> {};
	//#endif
	#ifdef __SIZEOF_FLOAT128__
	template <> struct value_traits<__float128>				: float_value_traits<__float128> {};
	#endif
	#ifdef TOML_SMALL_FLOAT_TYPE
	template <> struct value_traits<TOML_SMALL_FLOAT_TYPE>	: float_value_traits<TOML_SMALL_FLOAT_TYPE> {};
	#endif
	static_assert(value_traits<double>::is_native);
	static_assert(value_traits<double>::is_losslessly_convertible_to_native);
	static_assert(value_traits<double>::can_represent_native);
	static_assert(value_traits<double>::can_partially_represent_native);

	// string value traits

	template <typename T>
	struct string_value_traits
	{
		using native_type = std::string;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native
			= !std::is_array_v<T>
			&& (!std::is_pointer_v<T> || std::is_const_v<std::remove_pointer_t<T>>);
		static constexpr bool can_partially_represent_native = can_represent_native;
		static constexpr auto node_type = ::toml::node_type::string;
	};
	template <>         struct value_traits<std::string>		: string_value_traits<std::string> {};
	template <>         struct value_traits<std::string_view>	: string_value_traits<std::string_view> {};
	template <>         struct value_traits<const char*>		: string_value_traits<const char *> {};
	template <size_t N> struct value_traits<const char[N]>		: string_value_traits<const char[N]> {};
	template <>         struct value_traits<char*>				: string_value_traits<char*> {};
	template <size_t N> struct value_traits<char[N]>			: string_value_traits<char[N]> {};
	#ifdef __cpp_lib_char8_t
	template <>         struct value_traits<std::u8string>		: string_value_traits<std::u8string> {};
	template <>         struct value_traits<std::u8string_view>	: string_value_traits<std::u8string_view> {};
	template <>         struct value_traits<const char8_t*>		: string_value_traits<const char8_t*> {};
	template <size_t N> struct value_traits<const char8_t[N]>	: string_value_traits<const char8_t[N]> {};
	template <>         struct value_traits<char8_t*>			: string_value_traits<char8_t*> {};
	template <size_t N> struct value_traits<char8_t[N]>			: string_value_traits<char8_t[N]> {};
	#endif
	#if TOML_WINDOWS_COMPAT
	template <typename T>
	struct wstring_value_traits
	{
		using native_type = std::string;
		static constexpr bool is_native = false;
		static constexpr bool is_losslessly_convertible_to_native = true; //narrow
		static constexpr bool can_represent_native = std::is_same_v<T, std::wstring>; //widen
		static constexpr bool can_partially_represent_native = can_represent_native;
		static constexpr auto node_type = ::toml::node_type::string;
	};
	template <>         struct value_traits<std::wstring>		: wstring_value_traits<std::wstring> {};
	template <>         struct value_traits<std::wstring_view>	: wstring_value_traits<std::wstring_view> {};
	template <>         struct value_traits<const wchar_t*>		: wstring_value_traits<const wchar_t*> {};
	template <size_t N> struct value_traits<const wchar_t[N]>	: wstring_value_traits<const wchar_t[N]> {};
	template <>         struct value_traits<wchar_t*>			: wstring_value_traits<wchar_t*> {};
	template <size_t N> struct value_traits<wchar_t[N]>			: wstring_value_traits<wchar_t[N]> {};
	#endif

	// other native value traits

	template <typename T, ::toml::node_type NodeType>
	struct native_value_traits
	{
		using native_type = T;
		static constexpr bool is_native = true;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native = true;
		static constexpr bool can_partially_represent_native = true;
		static constexpr auto node_type = NodeType;
	};
	template <> struct value_traits<bool>		: native_value_traits<bool,		 node_type::boolean> {};
	template <> struct value_traits<date>		: native_value_traits<date,		 node_type::date> {};
	template <> struct value_traits<time>		: native_value_traits<time,		 node_type::time> {};
	template <> struct value_traits<date_time>	: native_value_traits<date_time, node_type::date_time> {};

	template <typename T>
	inline constexpr bool is_wide_string = is_one_of<
		std::decay_t<T>,
		const wchar_t*,
		wchar_t*,
		std::wstring_view,
		std::wstring
	>;

	// native value category queries

	template <typename T>
	using native_type_of = typename value_traits<T>::native_type;
	template <typename T>
	inline constexpr bool is_native = value_traits<T>::is_native;
	template <typename T>
	inline constexpr bool can_represent_native = value_traits<T>::can_represent_native;
	template <typename T>
	inline constexpr bool can_partially_represent_native = value_traits<T>::can_partially_represent_native;
	template <typename T>
	inline constexpr bool is_losslessly_convertible_to_native = value_traits<T>::is_losslessly_convertible_to_native;
	template <typename T, typename... U>
	inline constexpr bool is_natively_one_of = is_one_of<native_type_of<T>, U...>;

	template <typename T> struct node_wrapper				{ using type = T; };
	template <>           struct node_wrapper<std::string>	{ using type = value<std::string>; };
	template <>           struct node_wrapper<int64_t>		{ using type = value<int64_t>; };
	template <>           struct node_wrapper<double>		{ using type = value<double>; };
	template <>           struct node_wrapper<bool>			{ using type = value<bool>; };
	template <>           struct node_wrapper<date>			{ using type = value<date>; };
	template <>           struct node_wrapper<time>			{ using type = value<time>; };
	template <>           struct node_wrapper<date_time>	{ using type = value<date_time>; };
	template <typename T> using wrap_node = typename node_wrapper<T>::type;

	template <typename T> struct node_unwrapper				{ using type = T; };
	template <typename T> struct node_unwrapper<value<T>>	{ using type = T; };
	template <typename T> using unwrap_node = typename node_unwrapper<T>::type;

	template <typename T> struct node_type_getter		{ static constexpr auto value = value_traits<T>::node_type; };
	template <>           struct node_type_getter<table>{ static constexpr auto value = node_type::table; };
	template <>           struct node_type_getter<array>{ static constexpr auto value = node_type::array; };
	template <typename T>
	inline constexpr node_type node_type_of = node_type_getter<unwrap_node<remove_cvref_t<T>>>::value;
	
	inline constexpr std::string_view low_character_escape_table[] =
	{
		"\\u0000"sv,
		"\\u0001"sv,
		"\\u0002"sv,
		"\\u0003"sv,
		"\\u0004"sv,
		"\\u0005"sv,
		"\\u0006"sv,
		"\\u0007"sv,
		"\\b"sv,
		"\\t"sv,
		"\\n"sv,
		"\\u000B"sv,
		"\\f"sv,
		"\\r"sv,
		"\\u000E"sv,
		"\\u000F"sv,
		"\\u0010"sv,
		"\\u0011"sv,
		"\\u0012"sv,
		"\\u0013"sv,
		"\\u0014"sv,
		"\\u0015"sv,
		"\\u0016"sv,
		"\\u0017"sv,
		"\\u0018"sv,
		"\\u0019"sv,
		"\\u001A"sv,
		"\\u001B"sv,
		"\\u001C"sv,
		"\\u001D"sv,
		"\\u001E"sv,
		"\\u001F"sv,
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

	#define TOML_P2S_DECL(Type)											\
		template <typename Char>										\
		inline void print_to_stream(Type, std::basic_ostream<Char>&)

	TOML_P2S_DECL(int8_t);
	TOML_P2S_DECL(int16_t);
	TOML_P2S_DECL(int32_t);
	TOML_P2S_DECL(int64_t);
	TOML_P2S_DECL(uint8_t);
	TOML_P2S_DECL(uint16_t);
	TOML_P2S_DECL(uint32_t);
	TOML_P2S_DECL(uint64_t);
	TOML_P2S_DECL(float);
	TOML_P2S_DECL(const date&);
	TOML_P2S_DECL(const time&);
	TOML_P2S_DECL(time_offset);
	TOML_P2S_DECL(const date_time&);

	#undef TOML_P2S_DECL

	template <typename T>
	inline constexpr bool dependent_false = false;

	TOML_IMPL_NAMESPACE_END
} // impl

namespace toml
{
	TOML_ABI_NAMESPACE_VERSION

	/// \brief	Metafunction for determining if a type is a toml::table.
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref_t<T>, table>;
	/// \brief	Metafunction for determining if a type is a toml::array.
	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref_t<T>, array>;
	/// \brief	Metafunction for determining if a type is a std::string or toml::value<std::string>.
	template <typename T>
	inline constexpr bool is_string = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<std::string>>;
	/// \brief	Metafunction for determining if a type is an int64_t or toml::value<int64_t>.
	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<int64_t>>;
	/// \brief	Metafunction for determining if a type is a double or toml::value<double>.
	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<double>>;
	/// \brief	Metafunction for determining if a type satisfies `toml::is_integer || toml::is_floating_point`.
	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;
	/// \brief	Metafunction for determining if a type is a bool toml::value<bool>.
	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<bool>>;
	/// \brief	Metafunction for determining if a type is a toml::date or toml::value<date>.
	template <typename T>
	inline constexpr bool is_date = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<date>>;
	/// \brief	Metafunction for determining if a type is a toml::time or toml::value<time>.
	template <typename T>
	inline constexpr bool is_time = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<time>>;
	/// \brief	Metafunction for determining if a type is a toml::date_time or toml::value<date_time>.
	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<date_time>>;

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
	inline std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, node_type rhs)
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

	/// \brief	Helper class for suppressing move-construction in single-argument array constructors.
	///
	/// \detail \cpp
	/// // desired result: [ [ 42 ] ]
	/// auto bad = toml::array{ toml::array{ 42 } }
	/// auto good = toml::array{ toml::inserter{ toml::array{ 42 } } }
	/// std::cout << "bad: " << bad << std::endl;
	/// std::cout << "good:" << good << std::endl;
	/// \ecpp
	/// 
	/// \out
	/// bad:  [ 42 ]
	/// good: [ [ 42 ] ]
	/// \eout
	/// 
	/// \see toml::array
	template <typename T>
	struct TOML_TRIVIAL_ABI inserter
	{
		T&& value;
	};
	template <typename T> inserter(T&&) -> inserter<T>;

	TOML_ABI_NAMESPACE_END // version
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS, TOML_DISABLE_SHADOW_WARNINGS
