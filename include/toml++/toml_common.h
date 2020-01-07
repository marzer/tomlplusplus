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

#if defined(__clang__) or defined(__GNUC__)
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
#if TOML_EXCEPTIONS
	#define TOML_CONDITIONAL_NOEXCEPT(...)	noexcept(__VA_ARGS__)
	#define TOML_MAY_THROW
#else
	#define TOML_CONDITIONAL_NOEXCEPT(...)	noexcept
	#define TOML_MAY_THROW					noexcept
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

	using string_char = char;
	using string = std::string;
	using string_view = std::string_view;

	#endif

	template <typename T>
	using string_map = std::map<string, T, std::less<>>; //heterogeneous lookup

	class node;
	template <typename T> class node_view;
	template <typename T> class value;
	class array;
	class table;

	enum class node_type : uint8_t
	{
		table,
		array,
		string,
		integer,
		floating_point,
		boolean,
		date,
		time,
		date_time
	};

	#if TOML_LARGE_FILES

	using source_index = uint32_t;

	#else

	using source_index = uint16_t;

	#endif

	struct source_position final
	{
		source_index line;	//begins at 1
		source_index column;	//begins at 1

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

	using source_path_ptr = std::shared_ptr<const std::string>;

	struct source_region final
	{
		source_position begin;
		source_position end;
		source_path_ptr path;
	};

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS

	#if TOML_EXCEPTIONS

	class parse_error final
		: public std::runtime_error
	{
		private:
			source_region source_;

		public:

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* description, source_region&& source) noexcept
				: std::runtime_error{ description },
				source_{ std::move(source) }
			{}

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* description, const source_region& source) noexcept
				: parse_error{ description, source_region{ source } }
			{}

			TOML_NODISCARD_CTOR TOML_GCC_ATTR(nonnull)
			parse_error(const char* description, const source_position& position, const source_path_ptr& path = {}) noexcept
				: parse_error{ description, source_region{ position, position, path } }
			{}

			[[nodiscard]]
			const source_region& where() const noexcept
			{
				return source_;
			}
	};

	#else

	class parse_error final
	{
		private:
			std::string description_;
			source_region source_;

		public:

			TOML_NODISCARD_CTOR
			parse_error(std::string&& description, source_region&& source) noexcept
				: description_{ std::move(description) },
				source_{ std::move(source) }
			{}

			TOML_NODISCARD_CTOR
			parse_error(std::string&& description, const source_region& source) noexcept
				: parse_error{ std::move(description), source_region{ source } }
			{}

			TOML_NODISCARD_CTOR
				parse_error(std::string&& description, const source_position& position, const source_path_ptr& path = {}) noexcept
				: parse_error{ std::move(description), source_region{ position, position, path } }
			{}

			[[nodiscard]]
			std::string_view what() const noexcept
			{
				return description_;
			}

			[[nodiscard]]
			const source_region& where() const noexcept
			{
				return source_;
			}
	};

	#endif

	TOML_POP_WARNINGS

	struct date final
	{
		uint16_t year;
		uint8_t month;
		uint8_t day;

		[[nodiscard]]
		friend constexpr bool operator == (date lhs, date rhs) noexcept
		{
			return lhs.year == rhs.year
				&& lhs.month == rhs.month
				&& lhs.day == rhs.day;
		}

		[[nodiscard]]
		friend constexpr bool operator != (date lhs, date rhs) noexcept
		{
			return lhs.year != rhs.year
				|| lhs.month != rhs.month
				|| lhs.day != rhs.day;
		}
	};

	struct time final
	{
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint32_t nanosecond;

		[[nodiscard]]
		friend constexpr bool operator == (const time& lhs, const time& rhs) noexcept
		{
			return lhs.hour == rhs.hour
				&& lhs.minute == rhs.minute
				&& lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		[[nodiscard]]
		friend constexpr bool operator != (const time& lhs, const time& rhs) noexcept
		{
			return !(lhs == rhs);
		}
	};

	struct time_offset final
	{
		int16_t minutes;

		[[nodiscard]]
		static constexpr time_offset from_hh_mm(int8_t hours, int8_t minutes) noexcept
		{
			return time_offset{ static_cast<int16_t>(hours * 60 + minutes) };
		}

		[[nodiscard]]
		friend constexpr bool operator == (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator != (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}
	};

	struct date_time final
	{
		toml::date date;
		toml::time time;
		std::optional<toml::time_offset> time_offset;

		[[nodiscard]]
		friend constexpr bool operator == (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date
				&& lhs.time == rhs.time
				&& lhs.time_offset == rhs.time_offset;
		}

		[[nodiscard]]
		friend constexpr bool operator != (const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date != rhs.date
				|| lhs.time != rhs.time
				|| lhs.time_offset != rhs.time_offset;
		}

	};
}

namespace toml::impl
{
	#if defined(__cpp_lib_remove_cvref) || (defined(_MSC_VER) && defined(_HAS_CXX20))

	template <typename T>
	using remove_cvref_t = std::remove_cvref_t<T>;

	#else

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	#endif

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

	template <typename T>
	struct is_generic_invocable
	{
		template <typename U>
		static constexpr auto test(U&&) -> decltype(std::declval<T>()(std::declval<U&&>()), std::true_type{});
		static constexpr std::false_type test(...);

		struct tester {};
		static constexpr auto value = decltype(test(tester{}))::value;
	};

	template <typename T>
	inline constexpr bool is_generic_invocable_v = is_generic_invocable<T>::value;

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
		|| std::is_same_v<T, float>;

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
}

namespace toml
{
	template <typename T>
	using node_of = typename impl::node_wrapper<T>::type;

	template <typename T>
	using value_of = typename impl::node_unwrapper<T>::type;

	template <typename T>
	using promoted = typename impl::value_promoter<T>::type;

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, node_type rhs) TOML_MAY_THROW
	{
		using underlying_t = std::underlying_type_t<node_type>;
		if constexpr (std::is_same_v<CHAR, char>)
			return lhs << impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
		else if constexpr (sizeof(CHAR) == 1)
		{
			const auto str = impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
			return lhs << std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(str.data()), str.length() };
		}
		else
			return lhs << lhs.data();
	}
}
