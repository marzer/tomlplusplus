#pragma once

////////// CONFIGURATION
// clang-format off

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
	#undef TOML_CONFIG_HEADER
#endif

#ifndef TOML_CHAR_8_STRINGS
	#define TOML_CHAR_8_STRINGS 0
#endif

#ifndef TOML_UNRELEASED_FEATURES
	#define TOML_UNRELEASED_FEATURES 1
#endif

#ifndef TOML_LARGE_FILES
	#define TOML_LARGE_FILES 0
#endif

#ifndef TOML_ASSERT
	#ifdef assert
		#define TOML_ASSERT(expr)	assert(expr)
	#else
		#define TOML_ASSERT(expr)	(void)0
	#endif
#endif

#ifndef TOML_UNDEF_MACROS
	#define TOML_UNDEF_MACROS 1
#endif

////////// COMPILER & ENVIRONMENT STUFF

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif

#if defined(__clang__) || defined(__GNUC__)
	#define TOML_GCC_ATTR(attr)		__attribute__((attr))
#else
	#define TOML_GCC_ATTR(attr)
#endif

#ifdef __clang__

	#ifndef __cpp_exceptions
		#define TOML_EXCEPTIONS	0
	#endif

	#define TOML_PUSH_WARNINGS				_Pragma("clang diagnostic push")
	#define TOML_DISABLE_SWITCH_WARNINGS	_Pragma("clang diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_INIT_WARNINGS		_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("clang diagnostic ignored \"-Weverything\"")
	#define TOML_POP_WARNINGS				_Pragma("clang diagnostic pop")
	#define TOML_ALWAYS_INLINE				TOML_GCC_ATTR(__always_inline__) inline
	#define TOML_ASSUME(cond)				__builtin_assume(cond)
	#define TOML_UNREACHABLE				__builtin_unreachable()

	#if __has_declspec_attribute(novtable)
		#define TOML_INTERFACE			__declspec(novtable)
	#endif

	#if __has_declspec_attribute(empty_bases)
		#define TOML_EMPTY_BASES		__declspec(empty_bases)
	#endif

	//floating-point from_chars and to_chars are not implemented in any version of clang as of 1/1/2020
	#ifndef TOML_USE_STREAMS_FOR_FLOATS
		#define TOML_USE_STREAMS_FOR_FLOATS 1
	#endif

#elif defined(_MSC_VER)

	#ifndef _CPPUNWIND
		#define TOML_EXCEPTIONS	0
	#endif

	#define TOML_CPP_VERSION				_MSVC_LANG
	#define TOML_PUSH_WARNINGS				__pragma(warning(push))
	#define TOML_DISABLE_SWITCH_WARNINGS	__pragma(warning(disable: 4063))
	#define TOML_DISABLE_ALL_WARNINGS		__pragma(warning(pop))	\
											__pragma(warning(push, 0))
	#define TOML_POP_WARNINGS				__pragma(warning(pop))
	#define TOML_ALWAYS_INLINE				__forceinline
	#define TOML_ASSUME(cond)				__assume(cond)
	#define TOML_UNREACHABLE				__assume(0)
	#define TOML_INTERFACE					__declspec(novtable)
	#define TOML_EMPTY_BASES				__declspec(empty_bases)

#elif defined(__GNUC__)

	#ifndef __cpp_exceptions
		#define TOML_EXCEPTIONS	0
	#endif

	#define TOML_PUSH_WARNINGS				_Pragma("GCC diagnostic push")
	#define TOML_DISABLE_SWITCH_WARNINGS	_Pragma("GCC diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_INIT_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")	\
											_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")			\
											_Pragma("GCC diagnostic ignored \"-Wuninitialized\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wall\"")							\
											_Pragma("GCC diagnostic ignored \"-Wextra\"")						\
											_Pragma("GCC diagnostic ignored \"-Wchar-subscripts\"")				\
											_Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
	#define TOML_POP_WARNINGS				_Pragma("GCC diagnostic pop")
	#define TOML_ALWAYS_INLINE				TOML_GCC_ATTR(__always_inline__) inline
	#define TOML_UNREACHABLE				__builtin_unreachable()

	//floating-point from_chars and to_chars are not implemented in any version of gcc as of 1/1/2020
	#ifndef TOML_USE_STREAMS_FOR_FLOATS
		#define TOML_USE_STREAMS_FOR_FLOATS 1
	#endif

#endif

#ifndef TOML_CPP_VERSION
	#define TOML_CPP_VERSION __cplusplus
#endif
#if TOML_CPP_VERSION < 201103L
	#error toml++ requires C++17 or higher. For a TOML parser supporting pre-C++11 see https://github.com/ToruNiina/Boost.toml
#elif TOML_CPP_VERSION < 201703L
	#error toml++ requires C++17 or higher. For a TOML parser supporting C++11 see https://github.com/skystrife/cpptoml
#elif TOML_CPP_VERSION >= 202600L
	#define TOML_CPP 26
#elif TOML_CPP_VERSION >= 202300L
	#define TOML_CPP 23
#elif TOML_CPP_VERSION >= 202000L
	#define TOML_CPP 20
#elif TOML_CPP_VERSION >= 201703L
	#define TOML_CPP 17
#endif

#ifndef TOML_EXCEPTIONS
	#define TOML_EXCEPTIONS 1
#endif
#ifndef TOML_DOXYGEN
	#define TOML_DOXYGEN 0
#endif
#if TOML_EXCEPTIONS
	#define TOML_MAY_THROW
	#define TOML_MAY_THROW_UNLESS(...)	noexcept(__VA_ARGS__)
#else
	#define TOML_MAY_THROW				noexcept
	#define TOML_MAY_THROW_UNLESS(...)	noexcept
#endif

#ifndef TOML_DISABLE_INIT_WARNINGS
	#define	TOML_DISABLE_INIT_WARNINGS
#endif
#ifndef TOML_USE_STREAMS_FOR_FLOATS
	#define TOML_USE_STREAMS_FOR_FLOATS 0
#endif
#ifndef TOML_PUSH_WARNINGS
	#define TOML_PUSH_WARNINGS
#endif
#ifndef TOML_DISABLE_ALL_WARNINGS
	#define TOML_DISABLE_ALL_WARNINGS
#endif
#ifndef TOML_POP_WARNINGS
	#define TOML_POP_WARNINGS
#endif
#ifndef TOML_INTERFACE
	#define TOML_INTERFACE
#endif
#ifndef TOML_EMPTY_BASES
	#define TOML_EMPTY_BASES
#endif
#ifndef TOML_ALWAYS_INLINE
	#define TOML_ALWAYS_INLINE	inline
#endif
#ifndef TOML_ASSUME
	#define TOML_ASSUME(cond)	(void)0
#endif
#ifndef TOML_UNREACHABLE
	#define TOML_UNREACHABLE	TOML_ASSERT(false)
#endif
#define TOML_NO_DEFAULT_CASE	default: TOML_UNREACHABLE
#ifdef __cpp_consteval
	#define TOML_CONSTEVAL		consteval
#else
	#define TOML_CONSTEVAL		constexpr
#endif
#ifndef __INTELLISENSE__
	#if __has_cpp_attribute(likely)
		#define TOML_LIKELY [[likely]]
	#endif
	#if __has_cpp_attribute(unlikely)
		#define TOML_UNLIKELY [[unlikely]]
	#endif
	#if __has_cpp_attribute(no_unique_address)
		#define TOML_NO_UNIQUE_ADDRESS [[no_unique_address]]
	#endif
	#if __has_cpp_attribute(nodiscard) >= 201907L
		#define TOML_NODISCARD_CTOR [[nodiscard]]
	#endif
#endif //__INTELLISENSE__
#ifndef TOML_LIKELY
	#define TOML_LIKELY
#endif
#ifndef TOML_UNLIKELY
	#define TOML_UNLIKELY
#endif
#ifndef TOML_NO_UNIQUE_ADDRESS
	#define TOML_NO_UNIQUE_ADDRESS
#endif
#ifndef TOML_NODISCARD_CTOR
	#define TOML_NODISCARD_CTOR
#endif

#include "toml_version.h"

#define TOML_MAKE_VERSION(maj, min, rev)											\
		((maj) * 1000 + (min) * 25 + (rev))

#if TOML_UNRELEASED_FEATURES
	#define TOML_LANG_EFFECTIVE_VERSION												\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_REVISION+1)
#else
	#define TOML_LANG_EFFECTIVE_VERSION												\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_REVISION)
#endif

#define TOML_LANG_HIGHER_THAN(maj, min, rev)										\
		(TOML_LANG_EFFECTIVE_VERSION > TOML_MAKE_VERSION(maj, min, rev))

#define TOML_LANG_AT_LEAST(maj, min, rev)											\
		(TOML_LANG_EFFECTIVE_VERSION >= TOML_MAKE_VERSION(maj, min, rev))

#define TOML_LANG_EXACTLY(maj, min, rev)											\
		(TOML_LANG_EFFECTIVE_VERSION == TOML_MAKE_VERSION(maj, min, rev))

////////// INCLUDES

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS

#include <cstdint>
#include <cstring>		//memcpy, memset
#include <cmath>		//log10
#include <optional>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <charconv>
#if TOML_USE_STREAMS_FOR_FLOATS
	#include <sstream>
#endif
#if TOML_EXCEPTIONS
	#include <stdexcept>
#endif

TOML_POP_WARNINGS

#if TOML_CHAR_8_STRINGS
	#if !defined(__cpp_lib_char8_t)
		#error toml++ requires implementation support to use char8_t strings, but yours does not provide it.
	#endif

	#define TOML_STRING_PREFIX_1(S) u8##S
	#define TOML_STRING_PREFIX(S) TOML_STRING_PREFIX_1(S)
#else
	#define TOML_STRING_PREFIX(S) S
#endif

////////// FORWARD DECLARATIONS & TYPEDEFS
// clang-format on

/// \brief	The root namespace for all toml++ functions and types.
namespace toml
{
	inline namespace literals
	{
		using namespace std::string_literals;
		using namespace std::string_view_literals;

		[[nodiscard]] TOML_ALWAYS_INLINE
		TOML_CONSTEVAL uint8_t operator"" _u8(unsigned long long n) noexcept
		{
			return static_cast<uint8_t>(n);
		}

		[[nodiscard]] TOML_ALWAYS_INLINE
		TOML_CONSTEVAL size_t operator"" _sz(unsigned long long n) noexcept
		{
			return static_cast<size_t>(n);
		}
	}

	#if TOML_CHAR_8_STRINGS

	using string_char = char8_t;
	using string = std::u8string;
	using string_view = std::u8string_view;

	#else

	/// \brief	The base character type for keys and string values.
	/// \attention This will be `char8_t` if `TOML_CHAR_8_STRINGS` is `1`.
	using string_char = char;

	/// \brief	The string type for keys and string values.
	/// \attention This will be `std::u8string` if `TOML_CHAR_8_STRINGS` is `1`.
	using string = std::string;

	/// \brief	The string type for keys and string values.
	/// \attention This will be `std::u8string_view` if `TOML_CHAR_8_STRINGS` is `1`.
	using string_view = std::string_view;

	#endif

	struct date;
	struct time;
	struct time_offset;
	struct date_time;

	class node;
	template <typename T> class node_view;
	template <typename T> class value;
	class array;
	class table;

	/// \brief	TOML node type identifiers.
	enum class node_type : uint8_t
	{
		table, ///< The node is a toml::table.
		array,  ///< The node is a toml::array.
		string,  ///< The node is a toml::value<toml::string>.
		integer,  ///< The node is a toml::value<int64_t>.
		floating_point,  ///< The node is a toml::value<double>.
		boolean,  ///< The node is a toml::value<bool>.
		date,  ///< The node is a toml::value<toml::date>.
		time,  ///< The node is a toml::value<toml::time>.
		date_time  ///< The node is a toml::value<toml::date_time>.
	};

	#if TOML_LARGE_FILES

	using source_index = uint32_t;

	#else

	/// \brief	The integer type used to tally line numbers and columns.
	/// \attention This will be `uint32_t` if `TOML_LARGE_FILES` is `1`.
	using source_index = uint16_t;

	#endif

	/// \brief	A source document line-and-column pair.
	struct source_position
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

		[[nodiscard]]
		friend constexpr bool operator == (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line
				&& lhs.column == rhs.column;
		}

		[[nodiscard]]
		friend constexpr bool operator != (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line != rhs.line
				|| lhs.column != rhs.column;
		}

		[[nodiscard]]
		friend constexpr bool operator < (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line
				|| (lhs.line == rhs.line && lhs.column < rhs.column);
		}

		[[nodiscard]]
		friend constexpr bool operator <= (const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line
				|| (lhs.line == rhs.line && lhs.column <= rhs.column);
		}

		template <typename CHAR>
		friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_position& rhs)
			TOML_MAY_THROW
		{
			return lhs << "line " << rhs.line << ", column " << rhs.column;
		}
	};

	/// \brief	A pointer to a shared string resource containing a source path.
	using source_path_ptr = std::shared_ptr<const std::string>;

	/// \brief	A source document region.
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

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS

	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

	/// \brief	An error thrown/returned when parsing fails.
	/// 
	/// \remarks This class inherits from `std::runtime_error` when exceptions are enabled.
	/// 		 The public interface is the same regardless of exception mode.
	class parse_error final
	{
		private:
			std::string description_;
			source_region source_;

		public:

			TOML_NODISCARD_CTOR
			parse_error(std::string&& desc, source_region&& src) noexcept
				: description_{ std::move(desc) },
				source_{ std::move(src) }
			{}

			TOML_NODISCARD_CTOR
			parse_error(std::string&& desc, const source_region& src) noexcept
				: parse_error{ std::move(desc), source_region{ src } }
			{}

			TOML_NODISCARD_CTOR
				parse_error(std::string&& desc, const source_position& position, const source_path_ptr& path = {}) noexcept
				: parse_error{ std::move(desc), source_region{ position, position, path } }
			{}


			/// \brief	Returns a textual description of the error.
			[[nodiscard]]
			std::string_view description() const noexcept
			{
				return description_;
			}

			/// \brief	Returns the region of the source document responsible for the error.
			[[nodiscard]]
			const source_region& source() const noexcept
			{
				return source_;
			}
	};

	#else

	class parse_error final
		: public std::runtime_error
	{
		private:
			source_region source_;

		public:

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* desc, source_region&& src) noexcept
				: std::runtime_error{ desc },
				source_{ std::move(src) }
			{}

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* desc, const source_region& src) noexcept
				: parse_error{ desc, source_region{ src } }
			{}

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* desc, const source_position& position, const source_path_ptr& path = {}) noexcept
				: parse_error{ desc, source_region{ position, position, path } }
			{}

			[[nodiscard]]
			std::string_view description() const noexcept
			{
				return std::string_view{ what() };
			}

			[[nodiscard]]
			const source_region& source() const noexcept
			{
				return source_;
			}
	};

	#endif

	TOML_POP_WARNINGS
}

/// \brief	Internal implementation details. No user-serviceable parts within.
namespace toml::impl
{
	template <typename T>
	using string_map = std::map<string, T, std::less<>>; //heterogeneous lookup

	#if defined(__cpp_lib_remove_cvref) || (defined(_MSC_VER) && defined(_HAS_CXX20))

	template <typename T>
	using remove_cvref_t = std::remove_cvref_t<T>;

	#else

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	#endif

	template <typename T, typename... U>
	struct is_one_of_ : std::integral_constant<bool,
		(false || ... || std::is_same_v<T, U>)
	> {};

	template <typename T, typename... U>
	inline constexpr bool is_one_of = is_one_of_<T, U...>::value;

	template <typename T, typename... U>
	using enable_if_one_of = std::enable_if_t<is_one_of<T, U...>>;

	template <typename T, typename... U>
	using enable_if_not_one_of = std::enable_if_t<!is_one_of<T, U...>>;

	template <typename T>
	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr std::underlying_type_t<T> unwrap_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use std::find??"
	// A: Because <algorithm> is _huge_ and std::find would be the only thing I used from it.
	//    I don't want to impose such a heavy burden on users.

	template <typename T>
	inline std::optional<size_t> find(const std::vector<T>& haystack, const T& needle) noexcept
	{
		const auto end = haystack.size();
		for (size_t i = 0; i < end; i++)
			if (haystack[i] == needle)
				return i;
		return {};
	}

	class parser;

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

	template <typename T> struct node_unwrapper { using type = T; };
	template <typename T> struct node_unwrapper<value<T>> { using type = T; };

	template <typename T> struct value_promoter { using type = T; };
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
	inline constexpr auto node_type_of = node_type_of_<promoted<typename node_unwrapper<T>::type>>::value;

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


	#define TOML_P2S_DECL(linkage, type)															\
		template <typename CHAR>																	\
		linkage void print_to_stream(type, std::basic_ostream<CHAR>&) TOML_MAY_THROW

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
	/// \brief	Helper alias that wraps a type up as it's TOML node equivalent.
	template <typename T>
	using node_of = typename impl::node_wrapper<T>::type;

	/// \brief	Helper alias that unwraps TOML node type to it's raw value equivalent.
	template <typename T>
	using value_of = typename impl::node_unwrapper<T>::type;

	/// \brief	Metafunction for determining if a type is a toml::table.
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref_t<T>, table>;
	/// \brief	Metafunction for determining if a type is a toml::array.
	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref_t<T>, array>;
	/// \brief	Metafunction for determining if a type is a toml::value<string>.
	template <typename T>
	inline constexpr bool is_string = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<string>>;
	/// \brief	Metafunction for determining if a type is a toml::value<int64_t>.
	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<int64_t>>;
	/// \brief	Metafunction for determining if a type is a toml::value<double>.
	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<double>>;
	/// \brief	Metafunction for determining if a type is a toml::value<bool>.
	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<bool>>;
	/// \brief	Metafunction for determining if a type is a toml::value<toml::date>.
	template <typename T>
	inline constexpr bool is_date = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<date>>;
	/// \brief	Metafunction for determining if a type is a toml::value<toml::time>.
	template <typename T>
	inline constexpr bool is_time = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<time>>;
	/// \brief	Metafunction for determining if a type is a toml::value<toml::date_time>.
	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<node_of<impl::remove_cvref_t<T>>, value<date_time>>;

	/// \brief	Pretty-prints the value of a node_type to a stream.
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, node_type rhs) TOML_MAY_THROW
	{
		using underlying_t = std::underlying_type_t<node_type>;
		const auto str = impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
		if constexpr (std::is_same_v<CHAR, char>)
			return lhs << str;
		else
		{
			if constexpr (sizeof(CHAR) == 1)
				return lhs << std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(str.data()), str.length() };
			else
				return lhs << str.data();
		}
	}
}
