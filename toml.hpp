//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v0.1.0
// https://github.com/marzer/tomlplusplus
//
//----------------------------------------------------------------------------------------------------------------------
//
// -          THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY           -
//
// If you wish to submit a contribution to toml++, hooray and thanks! Before you crack on, please be aware that this
// file was assembled from a number of smaller files by a python script, and code contributions should not be made
// against it directly. You should instead make your changes in the relevant source file(s). The file names of the files
// that contributed to this header can be found at the beginnings and ends of the corresponding sections of this file.
//
//----------------------------------------------------------------------------------------------------------------------
//
// TOML language specification:
// Latest: https://github.com/toml-lang/toml/blob/master/README.md
// v0.5.0: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
//
//----------------------------------------------------------------------------------------------------------------------
//
// MIT License
//
// Copyright (c) 2019 Mark Gillard
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------------------------------------------------
// clang-format off
#pragma once
#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

//-------------  ↓ toml_common.h  --------------------------------------------------------------------------------------
#pragma region

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

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif

#ifdef __clang__

	#ifndef __cpp_exceptions
		#define TOML_EXCEPTIONS	0
	#endif

	#define TOML_PUSH_WARNINGS				_Pragma("clang diagnostic push")
	#define TOML_DISABLE_SWITCH_WARNING		_Pragma("clang diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_FIELD_INIT_WARNING	_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("clang diagnostic ignored \"-Weverything\"")
	#define TOML_POP_WARNINGS				_Pragma("clang diagnostic pop")
	#define TOML_ALWAYS_INLINE				__attribute__((__always_inline__)) inline
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
	#define TOML_DISABLE_SWITCH_WARNING		__pragma(warning(disable: 4063))
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
	#define TOML_DISABLE_SWITCH_WARNING		_Pragma("GCC diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_FIELD_INIT_WARNING	_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
	#define TOML_DISABLE_VAR_INIT_WARNING	_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wall\"")				\
											_Pragma("GCC diagnostic ignored \"-Wextra\"")			\
											_Pragma("GCC diagnostic ignored \"-Wchar-subscripts\"")	\
											_Pragma("GCC diagnostic ignored \"-Wtype-limits\"")

	#define TOML_POP_WARNINGS				_Pragma("GCC diagnostic pop")
	#define TOML_ALWAYS_INLINE				__attribute__((__always_inline__)) inline
	#define TOML_UNREACHABLE				__builtin_unreachable()

	//floating-point from_chars and to_chars are not implemented in any version of gcc as of 1/1/2020
	#ifndef TOML_USE_STREAMS_FOR_FLOATS
		#define TOML_USE_STREAMS_FOR_FLOATS 1
	#endif

#endif

#ifndef TOML_CPP_VERSION
	#define TOML_CPP_VERSION __cplusplus
#endif
#if TOML_CPP_VERSION >= 202600L
	#define TOML_CPP 26
#elif TOML_CPP_VERSION >= 202300L
	#define TOML_CPP 23
#elif TOML_CPP_VERSION >= 202000L
	#define TOML_CPP 20
#elif TOML_CPP_VERSION >= 201703L
	#define TOML_CPP 17
#elif TOML_CPP_VERSION >= 201100L
	#error toml++ requires C++17 or higher. For a TOML parser supporting C++11 see https://github.com/skystrife/cpptoml
#else
	#error toml++ requires C++17 or higher. For a TOML parser supporting pre-C++11 see https://github.com/ToruNiina/Boost.toml
#endif
#undef TOML_CPP_VERSION

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

#ifndef TOML_DISABLE_FIELD_INIT_WARNING
	#define	TOML_DISABLE_FIELD_INIT_WARNING
#endif
#ifndef TOML_DISABLE_VAR_INIT_WARNING
	#define	TOML_DISABLE_VAR_INIT_WARNING
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

#define TOML_LIB_MAJOR		0
#define TOML_LIB_MINOR		1
#define TOML_LIB_REVISION	0

#define TOML_LANG_MAJOR		0
#define TOML_LANG_MINOR		5
#define TOML_LANG_REVISION	0

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
		friend constexpr bool operator == (time lhs, time rhs) noexcept
		{
			return lhs.hour == rhs.hour
				&& lhs.minute == rhs.minute
				&& lhs.second == rhs.second
				&& lhs.nanosecond == rhs.nanosecond;
		}

		[[nodiscard]]
		friend constexpr bool operator != (time lhs, time rhs) noexcept
		{
			return lhs.hour != rhs.hour
				|| lhs.minute != rhs.minute
				|| lhs.second != rhs.second
				|| lhs.nanosecond != rhs.nanosecond;
		}
	};

	struct time_offset final
	{
		int8_t hours;
		int8_t minutes;

		[[nodiscard]]
		friend constexpr bool operator == (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.hours == rhs.hours
				&& lhs.minutes == rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator != (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.hours != rhs.hours
				|| lhs.minutes != rhs.minutes;
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
	template <typename T>
	class node_view;
	template <typename T>
	class value;
	class array;
	class table;

	class default_formatter;
	class json_formatter;

	namespace impl
	{
		#if defined(__cpp_lib_remove_cvref) || (defined(_MSC_VER) && defined(_HAS_CXX20))

		template <typename T>
		using remove_cvref_t = std::remove_cvref_t<T>;

		#else

		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		#endif

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
		class formatter;

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

		template <typename T> struct value_promoter { using type = T; };
		template <> struct value_promoter<string_view> { using type = string; };
		template <> struct value_promoter<int32_t> { using type = int64_t; };
		template <> struct value_promoter<int16_t> { using type = int64_t; };
		template <> struct value_promoter<int8_t> { using type = int64_t; };
		template <> struct value_promoter<uint32_t> { using type = int64_t; };
		template <> struct value_promoter<uint16_t> { using type = int64_t; };
		template <> struct value_promoter<uint8_t> { using type = int64_t; };
		template <> struct value_promoter<float> { using type = double; };

		template <typename T>
		std::optional<size_t> find(const std::vector<T>& haystack, const T& needle) noexcept
		{
			// Q: "why not use std::find??"
			// A: Because <algorithm> is _huge_ and std::find would be the only thing I used from it.
			//    I don't want to impose such a heavy burden on users.

			const auto end = haystack.size();
			for (size_t i = 0; i < end; i++)
				if (haystack[i] == needle)
					return i;
			return {};
		}
	}

	template <typename T>
	using node_of = typename impl::node_wrapper<T>::type;

	template <typename T>
	using value_of = typename impl::value_promoter<T>::type;

	struct source_position
	{
		uint32_t line;		//begins at 1
		uint32_t column;	//begins at 1

		[[nodiscard]]
		explicit constexpr operator bool () const noexcept
		{
			return line > 0u && column > 0u;
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

	struct source_region
	{
		source_position begin;
		source_position end;
		source_path_ptr path;
	};

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

	TOML_PUSH_WARNINGS
	TOML_DISABLE_FIELD_INIT_WARNING

	#if TOML_EXCEPTIONS

	class parse_error final
		: public std::runtime_error
	{
		private:
			source_region rgn;

		public:

			TOML_NODISCARD_CTOR
			parse_error(const char* description, source_region&& region) noexcept
				: std::runtime_error{ description },
				rgn{ std::move(region) }
			{}

			TOML_NODISCARD_CTOR
			parse_error(const char* description, const source_region& region) noexcept
				: std::runtime_error{ description },
				rgn{ region }
			{}

			TOML_NODISCARD_CTOR
			parse_error(const char* description, const source_position& position, const source_path_ptr& source_path) noexcept
				: std::runtime_error{ description },
				rgn{ position, position, source_path }
			{}

			TOML_NODISCARD_CTOR
			parse_error(const char* description, const source_position& position) noexcept
				: std::runtime_error{ description },
				rgn{ position, position }
			{}

			[[nodiscard]]
			const source_region& where() const noexcept
			{
				return rgn;
			}
	};

	#else

	struct parse_error final
	{
		std::string what;
		source_region where;

		TOML_NODISCARD_CTOR
		parse_error() noexcept = default;

		TOML_NODISCARD_CTOR
		parse_error(const char* description, source_region&& region) noexcept
			: what{ description },
			where{ std::move(region) }
		{}

		TOML_NODISCARD_CTOR
		parse_error(const char* description, const source_region& region) noexcept
			: what{ description },
			where{ region }
		{}

		TOML_NODISCARD_CTOR
		parse_error(const char* description, const source_position& position, const source_path_ptr& source_path) noexcept
			: what{ description },
			where{ position, position, source_path }
		{}

		TOML_NODISCARD_CTOR
		parse_error(const char* description, const source_position& position) noexcept
			: what{ description },
			where{ position, position }
		{}
	};

	#endif

	TOML_POP_WARNINGS
}

#pragma endregion
//-------------  ↑ toml_common.h  --------------------------------------------------------------------------------------

//-------------------------------  ↓ toml_node.h  ----------------------------------------------------------------------
#pragma region

namespace toml
{
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			friend class impl::formatter;
			source_region rgn{};

		protected:

			node(node&& other) noexcept
				: rgn{ std::move(other.rgn) }
			{}

			node& operator= (node&& rhs) noexcept
			{
				rgn = std::move(rhs.rgn);
				return *this;
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>* reinterpret_as() noexcept
			{
				return reinterpret_cast<node_of<T>*>(this);
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const node_of<T>* reinterpret_as() const noexcept
			{
				return reinterpret_cast<const node_of<T>*>(this);
			}

		public:

			node() noexcept = default;
			node(const node&) = delete;
			node& operator= (const node&) = delete;
			virtual ~node() noexcept = default;

			[[nodiscard]] virtual bool is_value() const noexcept { return false; }
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			[[nodiscard]] virtual bool is_boolean() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array() const noexcept { return false; }
			[[nodiscard]] virtual bool is_table() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }

			template <typename T>
			[[nodiscard]] bool is() const noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return is_string();
				else if constexpr (std::is_same_v<type, int64_t>) return is_integer();
				else if constexpr (std::is_same_v<type, double>) return is_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return is_boolean();
				else if constexpr (std::is_same_v<type, date>) return is_date();
				else if constexpr (std::is_same_v<type, time>) return is_time();
				else if constexpr (std::is_same_v<type, date_time>) return is_date_time();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, table>) return is_table();
			}

			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			[[nodiscard]] virtual value<bool>* as_boolean() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }

			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_boolean() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			node_of<T>* as() noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, table>) return as_table();
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const node_of<T>* as() const noexcept
			{
				using type = T;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, table>) return as_table();
			}

			[[nodiscard]] virtual node_type type() const noexcept = 0;

			[[nodiscard]] const source_region& region() const noexcept
			{
				return rgn;
			}

		private:

			// this is done using a static helper to preserve const categories
			// (otherwise I'd have to implement this function twice)
			// (const propagation in C++: a modern horror story)
			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N* node, FUNC&& visitor) TOML_MAY_THROW
			{
				static_assert(
					impl::is_generic_invocable_v<FUNC&&>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>
					|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				static constexpr auto is_exhaustive = impl::is_generic_invocable_v<FUNC&&> || (
					std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>
					&& std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>
				);

				switch (node->type())
				{
					case node_type::table:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<table>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<table>());
						break;
					case node_type::array:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<array>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<array>());
						break;
					case node_type::string:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<string>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<string>());
						break;
					case node_type::integer:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<int64_t>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<int64_t>());
						break;
					case node_type::floating_point:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<double>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<double>());
						break;
					case node_type::boolean:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<bool>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<bool>());
						break;
					case node_type::date:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<date>());
						break;
					case node_type::time:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<time>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<time>());
						break;
					case node_type::date_time:
						if constexpr (impl::is_generic_invocable_v<FUNC&&>
							|| std::is_invocable_v<FUNC&&, decltype(*node->template reinterpret_as<date_time>())>)
							return std::forward<FUNC>(visitor)(*node->template reinterpret_as<date_time>());
						break;
					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (is_exhaustive)
					TOML_UNREACHABLE;
			}

		public:

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) TOML_MAY_THROW
			{
				return do_visit(this, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const TOML_MAY_THROW
			{
				return do_visit(this, std::forward<FUNC>(visitor));
			}
	};
}

#pragma endregion
//-------------------------------  ↑ toml_node.h  ----------------------------------------------------------------------

//-------------------------------------------------  ↓ toml_table.h  ---------------------------------------------------
#pragma region

namespace toml
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;
			friend class default_formatter;
			friend class json_formatter;
			friend class node_view<table>;

			string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

		public:

			TOML_NODISCARD_CTOR
			table() noexcept {}

			TOML_NODISCARD_CTOR
			table(table&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) },
				inline_ { other.inline_ }
			{}

			table& operator= (table&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				inline_ = rhs.inline_;
				return *this;
			}

			[[nodiscard]] bool is_table() const noexcept override { return true; }
			[[nodiscard]] bool is_inline() const noexcept { return inline_; }

			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }

			[[nodiscard]] bool empty() const noexcept { return values.empty(); }

			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]] node* get(string_view key) noexcept
			{
				if (auto it = values.find(key); it != values.end())
					return it->second.get();
				return nullptr;
			}

			[[nodiscard]] const node* get(string_view key) const noexcept
			{
				if (auto it = values.find(key); it != values.end())
					return it->second.get();
				return nullptr;
			}

			template <typename T>
			[[nodiscard]] node_of<T>* get_as(string_view key) noexcept
			{
				const auto node = get(key);
				return node ? node->as<T>() : nullptr;
			}

			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(string_view key) const noexcept
			{
				const auto node = get(key);
				return node ? node->as<T>() : nullptr;
			}

			[[nodiscard]] inline node_view<table> operator[] (string_view) noexcept;
			[[nodiscard]] inline node_view<const table> operator[] (string_view) const noexcept;
	};
}

#pragma endregion
//-------------------------------------------------  ↑ toml_table.h  ---------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_array.h  ---------------------------------
#pragma region

namespace toml
{
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::unique_ptr<node>> values;

		public:

			TOML_NODISCARD_CTOR
			array() noexcept = default;

			TOML_NODISCARD_CTOR
			array(array&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) }
			{}

			array& operator= (array&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				return *this;
			}

			[[nodiscard]] bool is_array() const noexcept override { return true; }

			[[nodiscard]] bool is_array_of_tables() const noexcept override
			{
				if (values.empty())
					return false;

				for (auto& val : values)
					if (!val->is_table())
						return false;

				return true;
			}

			[[nodiscard]] bool is_homogeneous() const noexcept
			{
				if (values.size() <= 1_sz)
					return true;

				const auto type = values[0]->type();
				for (size_t i = 1; i < values.size(); i++)
					if (values[i]->type() != type)
						return false;
				return true;
			}

			[[nodiscard]] array* as_array() noexcept override { return this; }
			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			[[nodiscard]] node_type type() const noexcept override { return node_type::array; }

			[[nodiscard]] bool empty() const noexcept { return values.empty(); }

			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]] node* get(size_t index) noexcept { return values[index].get(); }
			[[nodiscard]] const node* get(size_t index) const noexcept { return values[index].get(); }

			template <typename T>
			[[nodiscard]] node_of<T>* get_as(size_t index) noexcept
			{
				return get(index)->as<T>();
			}

			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(size_t index) const noexcept
			{
				return get(index)->as<T>();
			}
	};
}

#pragma endregion
//-------------------------------------------------------------------  ↑ toml_array.h  ---------------------------------

//-------------------------------------------------------------------------------------  ↓ toml_value.h  ---------------
#pragma region

namespace toml
{
	template <typename T>
	class value final
		: public node
	{
		static_assert(
			impl::is_value<T>,
			"Template type parameter must be one of the basic value types"
		);

		private:
			friend class impl::parser;

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			toml::value<U>* as_value() noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return this;
				else
					return nullptr;
			}

			template <typename U>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const toml::value<U>* as_value() const noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return this;
				else
					return nullptr;
			}

			T val_;

		public:

			template <typename... U>
			TOML_NODISCARD_CTOR
			explicit value(U&&... args) TOML_CONDITIONAL_NOEXCEPT(std::is_nothrow_constructible_v<T, U &&...>)
				: val_{ std::forward<U>(args)... }
			{}

			TOML_NODISCARD_CTOR
			value(value&& other) noexcept
				: node{ std::move(other) },
				val_{ std::move(other.val_) }
			{}

			value& operator= (value&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				val_ = std::move(rhs.val_);
				return *this;
			}

			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_integer() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_floating_point() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_boolean() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_date_time() const noexcept override { return std::is_same_v<T, date_time>; }

			[[nodiscard]] value<string>* as_string() noexcept override { return as_value<string>(); }
			[[nodiscard]] value<int64_t>* as_integer() noexcept override { return as_value<int64_t>(); }
			[[nodiscard]] value<double>* as_floating_point() noexcept override { return as_value<double>(); }
			[[nodiscard]] value<bool>* as_boolean() noexcept override { return as_value<bool>(); }
			[[nodiscard]] value<date>* as_date() noexcept override { return as_value<date>(); }
			[[nodiscard]] value<time>* as_time() noexcept override { return as_value<time>(); }
			[[nodiscard]] value<date_time>* as_date_time() noexcept override { return as_value<date_time>(); }

			[[nodiscard]] const value<string>* as_string() const noexcept override { return as_value<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept override { return as_value<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept override { return as_value<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept override { return as_value<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return as_value<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return as_value<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept override { return as_value<date_time>(); }

			[[nodiscard]] node_type type() const noexcept override
			{
					 if constexpr (std::is_same_v<T, string>) return node_type::string;
				else if constexpr (std::is_same_v<T, int64_t>) return node_type::integer;
				else if constexpr (std::is_same_v<T, double>) return node_type::floating_point;
				else if constexpr (std::is_same_v<T, bool>) return node_type::boolean;
				else if constexpr (std::is_same_v<T, date>) return node_type::date;
				else if constexpr (std::is_same_v<T, time>) return node_type::time;
				else if constexpr (std::is_same_v<T, date_time>) return node_type::date_time;
			}

			[[nodiscard]] T& get() noexcept { return val_; }
			[[nodiscard]] const T& get() const noexcept { return val_; }
	};

	value(const string_char*) -> value<string>;
	value(string_view) -> value<string>;
	value(string) -> value<string>;
	value(bool) -> value<bool>;
	value(float) -> value<double>;
	value(double) -> value<double>;
	value(int8_t) -> value<int64_t>;
	value(int16_t) -> value<int64_t>;
	value(int32_t) -> value<int64_t>;
	value(int64_t) -> value<int64_t>;
	value(uint8_t) -> value<int64_t>;
	value(uint16_t) -> value<int64_t>;
	value(uint32_t) -> value<int64_t>;
}

#pragma endregion
//-------------------------------------------------------------------------------------  ↑ toml_value.h  ---------------

//-------------  ↓ toml_utf8.h  ----------------------------------------------------------------------------------------
#pragma region

#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687

#define TOML_ASSUME_CODEPOINT_BETWEEN(first, last)	\
	TOML_ASSUME(codepoint >= first);				\
	TOML_ASSUME(codepoint <= last)

namespace toml::impl
{

	[[nodiscard]]
	constexpr bool is_unicode_letter(char32_t codepoint) noexcept
	{
		if (codepoint < U'\u00AA' || codepoint > U'\U0002FA1D')
			return false;

		TOML_ASSUME_CODEPOINT_BETWEEN(U'\u00AA', U'\U0002FA1D');
		switch ((static_cast<uint_least32_t>(codepoint) - 0xAAu) / 3046u)
		{
			case 0:
			{
				if (codepoint > U'\u0C8F')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u00AA', U'\u0C8F');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xAAu) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAAull)) & 0xFFFFDFFFFFC10801ull;
					case 1: return codepoint != U'\u00F7';
					case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2AAull)) & 0x7C000FFF0FFFFFFull;
					case 9: return codepoint != U'\u02EA';
					case 10: return false;
					case 11: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x370ull)) & 0x3FBFFFFD740BCDFull;
					case 13: return codepoint != U'\u03F6';
					case 15: return codepoint <= U'\u0481' || codepoint >= U'\u048A';
					case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x52Aull)) & 0xFFC09FFFFFFFFFBFull;
					case 21: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x5EAull)) & 0xFFC00000000001E1ull;
					case 23: return codepoint != U'\u066A';
					case 24: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x6AAull)) & 0x18000BFFFFFFFFFFull;
					case 25: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x6EEull)) & 0xFFFFFF400027003ull;
					case 26: return codepoint <= U'\u072F' || codepoint >= U'\u074D';
					case 28: return codepoint <= U'\u07B1' || codepoint >= U'\u07CA';
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x7EAull)) & 0x44010FFFFFC10C01ull;
					case 30: return codepoint <= U'\u0858' || codepoint >= U'\u0860';
					case 31: return codepoint <= U'\u086A' || codepoint >= U'\u08A0';
					case 32: return codepoint != U'\u08B5';
					case 34: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x92Aull)) & 0xFFC0400008FFFFull;
					case 35: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x971ull)) & 0xFFFFFCCFF0FFFFull;
					case 36: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x9AAull)) & 0xEC00100008F17Full;
					case 37: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x9F0ull)) & 0x1FFFFF987E01003ull;
					case 38: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA2Aull)) & 0x1780000000DB7Full;
					case 39: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA72ull)) & 0x7FFFFEEFF80007ull;
					case 40: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAAAull)) & 0xC000400008FB7Full;
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAF9ull)) & 0xFFFFFCCFF001ull;
					case 42: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB2Aull)) & 0xEC00000008FB7Full;
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB71ull)) & 0x18C6B1EE3F40001ull;
					case 44: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xBAAull)) & 0x400000FFF1ull;
					case 45: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC05ull)) & 0xFFFFFEEFFull;
					case 46: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC2Aull)) & 0xC1C0000008FFFFull;
					case 47: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xC80u)) & 0xDFE1u;
					default: return true;
				}
				// chunk summary: 1867 codepoints from 117 ranges (spanning a search area of 3046)
			}
			case 1:
			{
				if (codepoint < U'\u0C90' || codepoint > U'\u1875')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0C90', U'\u1875');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xC90u) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC90ull)) & 0x23EFFDFFFFFDull;
					case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xCDEull)) & 0x37F800018000Dull;
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD10ull)) & 0x400027FFFFFFFFFDull;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD54ull)) & 0xFFE0FC000003807ull;
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD90ull)) & 0x7F2FFBFFFFFC7Full;
					case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xE10ull)) & 0x7F000DFFFFFFFFull;
					case 7: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xE81u)) & 0x7BEBu;
					case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xE90ull)) & 0x5F200DFFAFFFFFull;
					case 9: return codepoint <= U'\u0EDF' || codepoint >= U'\u0F00';
					case 10: return codepoint != U'\u0F10';
					case 11: return codepoint <= U'\u0F6C' || codepoint >= U'\u0F88';
					case 12: return false;
					case 14: return codepoint <= U'\u102A' || codepoint >= U'\u103F';
					case 15: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1050ull)) & 0x4003FFE1C0623C3Full;
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10A0ull)) & 0x20BFFFFFFFFFull;
					case 17: return codepoint != U'\u10FB';
					case 22: return codepoint != U'\u1249';
					case 23: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1250ull)) & 0x3DFFFFFFFFFF3D7Full;
					case 24: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1290ull)) & 0xFF3D7F3DFFFFFFFFull;
					case 25: return codepoint != U'\u12D7';
					case 26: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1310ull)) & 0xFFFFFFFFFFFFFF3Dull;
					case 27: return codepoint <= U'\u135A' || codepoint >= U'\u1380';
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x13D0ull)) & 0xFFFE3F3FFFFFFFFFull;
					case 39: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1650ull)) & 0xFFFEFFFF9FFFFFFFull;
					case 40: return codepoint <= U'\u169A' || codepoint >= U'\u16A0';
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x16D0ull)) & 0xDFFF01FE07FFFFFFull;
					case 42: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1710ull)) & 0xFFFF0003FFFF0003ull;
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1750ull)) & 0xFFFF0001DFFF0003ull;
					case 45: return codepoint <= U'\u17D7' || codepoint >= U'\u17DC';
					default: return true;
				}
				// chunk summary: 2139 codepoints from 86 ranges (spanning a search area of 3046)
			}
			case 2:
			{
				if (codepoint < U'\u1876' || codepoint > U'\u2184')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u1876', U'\u2184');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1876u) / 63u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1876ull)) & 0x7C17FFFFFFFE7C07ull;
					case 2: return codepoint <= U'\u18F5' || codepoint >= U'\u1900';
					case 3: return codepoint <= U'\u196D' || codepoint >= U'\u1970';
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1972ull)) & 0x43FFFFFFFFFFC007ull;
					case 6: return codepoint <= U'\u1A16' || codepoint >= U'\u1A20';
					case 9: return false;
					case 11: return codepoint <= U'\u1B33' || codepoint >= U'\u1B45';
					case 13: return codepoint <= U'\u1BAF' || codepoint >= U'\u1BBA';
					case 15: return codepoint <= U'\u1C4F' || codepoint >= U'\u1C5A';
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1C66ull)) & 0x7FFFFC07FCFFFFFFull;
					case 17: return codepoint <= U'\u1CBA' || codepoint >= U'\u1CBD';
					case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1CE9ull)) & 0x3FFFFFFFF8237EFull;
					case 26: return codepoint <= U'\u1F15' || codepoint >= U'\u1F18';
					case 27: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F1Bull)) & 0x5FE7E7FFFFFFFFE7ull;
					case 28: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F5Bull)) & 0x3FFFFFE7FFFFFFF5ull;
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F99ull)) & 0x678FEE2FEFFFFFFFull;
					case 30: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1FD8ull)) & 0x1FDC1FFF0Full;
					case 31: return false;
					case 32: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2071ull)) & 0xF80004001ull;
					case 34: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x2102u)) & 0x1FF21u;
					case 35: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2113ull)) & 0x87C1E7FF7AA07C5ull;
					default: return true;
				}
				// chunk summary: 1328 codepoints from 65 ranges (spanning a search area of 2319)
			}
			case 3:
			{
				if (codepoint < U'\u2C00' || codepoint > U'\u3041')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u2C00', U'\u3041');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x2C00u) / 61u)
				{
					case 0: return codepoint != U'\u2C2F';
					case 1: return codepoint != U'\u2C5F';
					case 2: return true;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2CB7ull)) & 0x18F03FFFFFFFFFFFull;
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2D00ull)) & 0x120BFFFFFFFFFull;
					case 5: return true;
					case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2D6Full)) & 0xEFE00FFFFFE0001ull;
					case 7: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2DABull)) & 0xFEFEFEFEFEFEFull;
					case 9: return true;
					case 16: return true;
					case 17: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x3031u)) & 0x10C1Fu;
					default: return false;
				}
				// chunk summary: 420 codepoints from 24 ranges (spanning a search area of 1090)
			}
			case 4:
			{
				if (codepoint < U'\u3042' || codepoint > U'\u3C27')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u3042', U'\u3C27');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x3042u) / 64u)
				{
					case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x3082ull)) & 0xFFFFFFFFB81FFFFFull;
					case 2: return codepoint != U'\u30FB';
					case 3: return codepoint != U'\u3102';
					case 5: return codepoint <= U'\u318E' || codepoint >= U'\u31A0';
					case 7: return false;
					case 8: return false;
					case 9: return false;
					case 10: return false;
					case 11: return false;
					case 12: return false;
					case 13: return false;
					default: return true;
				}
				// chunk summary: 2450 codepoints from 9 ranges (spanning a search area of 3046)
			}
			case 6: return codepoint <= U'\u4DB4' || codepoint >= U'\u4E00';
			case 13:
			{
				if (codepoint < U'\u9B58' || codepoint > U'\uA73D')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u9B58', U'\uA73D');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x9B58u) / 64u)
				{
					case 18: return codepoint <= U'\u9FEE' || codepoint >= U'\uA000';
					case 38: return codepoint <= U'\uA4FD' || codepoint >= U'\uA500';
					case 42: return codepoint <= U'\uA60C' || codepoint >= U'\uA610';
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA618ull)) & 0xFFFFFF00000C00FFull;
					case 44: return codepoint <= U'\uA66E' || codepoint >= U'\uA67F';
					case 45: return codepoint <= U'\uA69D' || codepoint >= U'\uA6A0';
					case 46: return codepoint <= U'\uA6E5' || codepoint >= U'\uA717';
					case 47: return codepoint <= U'\uA71F' || codepoint >= U'\uA722';
					default: return true;
				}
				// chunk summary: 2858 codepoints from 11 ranges (spanning a search area of 3046)
			}
			case 14:
			{
				if (codepoint < U'\uA73E' || codepoint > U'\uB323')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uA73E', U'\uB323');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xA73Eu) / 64u)
				{
					case 1: return codepoint <= U'\uA788' || codepoint >= U'\uA78B';
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA7BEull)) & 0xFE000000000001F3ull;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA7FEull)) & 0x1FFFFFDEEFull;
					case 6: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA8F2u)) & 0xA3Fu;
					case 7: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA8FEull)) & 0xFFFC00FFFFFFF001ull;
					case 8: return codepoint <= U'\uA946' || codepoint >= U'\uA960';
					case 10: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA9CFull)) & 0x7801FFBE0001ull;
					case 11: return codepoint != U'\uA9FF';
					case 12: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA40ull)) & 0x47FFFFF00000FF7ull;
					case 13: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA7Eull)) & 0xF98BFFFFFFFFFFFFull;
					case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAAC0ull)) & 0x1C07FF38000005ull;
					case 15: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAB01ull)) & 0x1FFFBFBF803F3F3Full;
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAB3Eull)) & 0xFFFC03FFDFFFFFFFull;
					default: return true;
				}
				// chunk summary: 2616 codepoints from 43 ranges (spanning a search area of 3046)
			}
			case 18: return codepoint <= U'\uD7A2' || (codepoint >= U'\uD7B0' && codepoint <= U'\uD7C6')
					|| codepoint >= U'\uD7CB';
			case 19: return false;
			case 20: return codepoint <= U'\uFA6D' || codepoint >= U'\uFA70';
			case 21:
			{
				if (codepoint < U'\uFA88' || codepoint > U'\U0001066D')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uFA88', U'\U0001066D');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xFA88u) / 64u)
				{
					case 1: return codepoint <= U'\uFAD9' || codepoint >= U'\uFB00';
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xFB13ull)) & 0x1B6BEFFFBFF41Full;
					case 12: return codepoint <= U'\uFD8F' || codepoint >= U'\uFD92';
					case 14: return false;
					case 15: return codepoint != U'\uFE48';
					case 18: return codepoint <= U'\uFF3A' || codepoint >= U'\uFF41';
					case 19: return codepoint <= U'\uFF5A' || codepoint >= U'\uFF66';
					case 20: return codepoint <= U'\uFFBE' || codepoint >= U'\uFFC2';
					case 21: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xFFCAull)) & 0x3FC0000000073F3Full;
					case 22: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10008ull)) & 0xFFB7FFFF7FFFFFEFull;
					case 23: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10048ull)) & 0xFF000000003FFF3Full;
					case 26: return false;
					case 27: return false;
					case 28: return false;
					case 29: return false;
					case 30: return false;
					case 32: return codepoint <= U'\U0001029C' || codepoint >= U'\U000102A0';
					case 33: return codepoint <= U'\U000102D0' || codepoint >= U'\U00010300';
					case 34: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10308ull)) & 0xFDFFFFE000FFFFFFull;
					case 35: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10348ull)) & 0xFF003FFFFFFFFF03ull;
					case 36: return codepoint <= U'\U0001039D' || codepoint >= U'\U000103A0';
					case 37: return codepoint <= U'\U000103CF' || codepoint >= U'\U00010400';
					case 40: return codepoint <= U'\U0001049D' || codepoint >= U'\U000104B0';
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x104C8ull)) & 0xFF0FFFFFFFFF0FFFull;
					case 42: return codepoint <= U'\U00010527' || codepoint >= U'\U00010530';
					case 44: return false;
					default: return true;
				}
				// chunk summary: 1924 codepoints from 46 ranges (spanning a search area of 3046)
			}
			case 22:
			{
				if (codepoint < U'\U0001066E' || codepoint > U'\U0001122B')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001066E', U'\U0001122B');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1066Eu) / 64u)
				{
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1072Eull)) & 0x3FC00FFFFFC01FFull;
					case 4: return false;
					case 5: return false;
					case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10800ull)) & 0x3FFFFFFFFD3Full;
					case 7: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1082Eull)) & 0xFFFC00FFFFFE46FFull;
					case 8: return codepoint <= U'\U00010876' || codepoint >= U'\U00010880';
					case 10: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x108EEull)) & 0xFFFC00FFFFFC00DFull;
					case 13: return codepoint <= U'\U000109B7' || codepoint >= U'\U000109BE';
					case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10A00ull)) & 0x3FFFFEEF0001ull;
					case 15: return codepoint <= U'\U00010A35' || codepoint >= U'\U00010A60';
					case 16: return codepoint <= U'\U00010A7C' || codepoint >= U'\U00010A80';
					case 17: return codepoint != U'\U00010AAE';
					case 19: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10B2Eull)) & 0xFFFC00FFFFFC00FFull;
					case 20: return codepoint <= U'\U00010B72' || codepoint >= U'\U00010B80';
					case 21: return false;
					case 25: return codepoint <= U'\U00010CB2' || codepoint >= U'\U00010CC0';
					case 26: return codepoint <= U'\U00010CF2' || codepoint >= U'\U00010D00';
					case 27: return false;
					case 28: return false;
					case 29: return false;
					case 30: return false;
					case 31: return false;
					case 32: return false;
					case 33: return false;
					case 34: return codepoint <= U'\U00010F1C' || codepoint >= U'\U00010F27';
					case 36: return false;
					case 38: return codepoint <= U'\U00010FF6' || codepoint >= U'\U00011003';
					case 41: return codepoint <= U'\U000110AF' || codepoint >= U'\U000110D0';
					case 43: return codepoint <= U'\U00011144' || codepoint >= U'\U00011150';
					case 44: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1116Eull)) & 0xFFFFFFFFFFE0011Full;
					case 45: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x111AEull)) & 0x50000078001Full;
					case 46: return codepoint != U'\U000111EE';
					default: return true;
				}
				// chunk summary: 1312 codepoints from 50 ranges (spanning a search area of 3006)
			}
			case 23:
			{
				if (codepoint < U'\U00011280' || codepoint > U'\U00011D98')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00011280', U'\U00011D98');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x11280u) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11280ull)) & 0xFFFF01FFBFFFBD7Full;
					case 1: return true;
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11305ull)) & 0x11F6FEFFFFFCCFFull;
					case 3: return codepoint <= U'\U00011350' || codepoint >= U'\U0001135D';
					case 6: return true;
					case 7: return codepoint <= U'\U0001144A' || codepoint >= U'\U0001145F';
					case 8: return true;
					case 9: return codepoint != U'\U000114C0';
					case 12: return true;
					case 13: return true;
					case 14: return true;
					case 15: return true;
					case 16: return codepoint <= U'\U000116AA' || codepoint >= U'\U000116B8';
					case 18: return true;
					case 22: return true;
					case 24: return true;
					case 25: return codepoint <= U'\U000118DF' || codepoint >= U'\U000118FF';
					case 28: return codepoint <= U'\U000119A7' || codepoint >= U'\U000119AA';
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x119C0ull)) & 0xA0001FFFFull;
					case 30: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11A00ull)) & 0x407FFFFFFFFF801ull;
					case 31: return codepoint <= U'\U00011A50' || codepoint >= U'\U00011A5C';
					case 32: return codepoint <= U'\U00011A89' || codepoint >= U'\U00011A9D';
					case 33: return true;
					case 38: return codepoint != U'\U00011C09';
					case 39: return codepoint <= U'\U00011C40' || codepoint >= U'\U00011C72';
					case 40: return true;
					case 42: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11D00ull)) & 0x1FFFFFFFFFB7Full;
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11D46ull)) & 0x3FFFFF6FC000001ull;
					case 44: return codepoint <= U'\U00011D89' || codepoint >= U'\U00011D98';
					default: return false;
				}
				// chunk summary: 888 codepoints from 54 ranges (spanning a search area of 2841)
			}
			case 24: return codepoint <= U'\U00011EF2' || (codepoint >= U'\U00012000' && codepoint <= U'\U00012399')
					|| codepoint >= U'\U00012480';
			case 26: return false;
			case 28: return false;
			case 29: return false;
			case 30:
			{
				if (codepoint < U'\U00016800' || codepoint > U'\U00017183')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00016800', U'\U00017183');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x16800u) / 63u)
				{
					case 9: return codepoint <= U'\U00016A38' || codepoint >= U'\U00016A40';
					case 10: return false;
					case 13: return codepoint <= U'\U00016B43' || codepoint >= U'\U00016B63';
					case 14: return codepoint <= U'\U00016B77' || codepoint >= U'\U00016B7D';
					case 15: return false;
					case 16: return false;
					case 17: return false;
					case 18: return false;
					case 19: return false;
					case 20: return false;
					case 21: return false;
					case 22: return false;
					case 23: return false;
					case 24: return false;
					case 27: return false;
					case 29: return codepoint <= U'\U00016F4A' || codepoint >= U'\U00016F50';
					case 31: return false;
					case 32: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x16FE0ull)) & 0x7FFFFFFF0000000Bull;
					default: return true;
				}
				// chunk summary: 1266 codepoints from 14 ranges (spanning a search area of 2436)
			}
			case 32: return codepoint <= U'\U000187F6' || codepoint >= U'\U00018800';
			case 34: return false;
			case 35: return false;
			case 36: return codepoint <= U'\U0001B11E' || (codepoint >= U'\U0001B150' && codepoint <= U'\U0001B152')
					|| (codepoint >= U'\U0001B164' && codepoint <= U'\U0001B167') || codepoint >= U'\U0001B170';
			case 37: return codepoint <= U'\U0001BC6A' || (codepoint >= U'\U0001BC70' && codepoint <= U'\U0001BC7C')
					|| (codepoint >= U'\U0001BC80' && codepoint <= U'\U0001BC88') || codepoint >= U'\U0001BC90';
			case 38: return false;
			case 39:
			{
				if (codepoint < U'\U0001D400' || codepoint > U'\U0001D7CB')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001D400', U'\U0001D7CB');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1D400u) / 61u)
				{
					case 1: return codepoint != U'\U0001D455';
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D47Aull)) & 0x1FF79937FFFFFFFFull;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D4B7ull)) & 0x1FFFFFFFFFFFDFD7ull;
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D4F4ull)) & 0x1FFFFDFDFE7BFFFFull;
					case 5: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D531ull)) & 0x1FFFFFFEFE2FBDFFull;
					case 11: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D69Full)) & 0xFFFFFFBFFFFFE7Full;
					case 12: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D6DCull)) & 0x1DFFFFFF7FFFFFFFull;
					case 13: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D719ull)) & 0x1FBFFFFFEFFFFFFFull;
					case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D756ull)) & 0x1FF7FFFFFDFFFFFFull;
					case 15: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D793ull)) & 0x1FEFFFFFFBFFFFFull;
					default: return true;
				}
				// chunk summary: 936 codepoints from 30 ranges (spanning a search area of 972)
			}
			case 40:
			{
				if (codepoint < U'\U0001E100' || codepoint > U'\U0001E87F')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001E100', U'\U0001E87F');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1E100u) / 64u)
				{
					case 0: return codepoint <= U'\U0001E12C' || codepoint >= U'\U0001E137';
					case 1: return true;
					case 7: return true;
					case 28: return true;
					case 29: return true;
					default: return false;
				}
				// chunk summary: 225 codepoints from 5 ranges (spanning a search area of 1920)
			}
			case 41:
			{
				if (codepoint < U'\U0001E880' || codepoint > U'\U0001EEBB')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001E880', U'\U0001EEBB');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1E880u) / 64u)
				{
					case 0: return true;
					case 1: return true;
					case 2: return true;
					case 3: return codepoint <= U'\U0001E943' || codepoint >= U'\U0001E94B';
					case 22: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE00ull)) & 0xAF7FE96FFFFFFEFull;
					case 23: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE42ull)) & 0x17BDFDE5AAA5BAA1ull;
					case 24: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE80ull)) & 0xFFFFBEE0FFFFBFFull;
					default: return false;
				}
				// chunk summary: 279 codepoints from 36 ranges (spanning a search area of 1596)
			}
			case 58: return codepoint <= U'\U0002B733' || (codepoint >= U'\U0002B740' && codepoint <= U'\U0002B81C')
					|| codepoint >= U'\U0002B820';
			case 60: return codepoint <= U'\U0002CEA0' || codepoint >= U'\U0002CEB0';
			default: return true;
		}
		// chunk summary: 125582 codepoints from 607 ranges (spanning a search area of 194932)
	}

	[[nodiscard]]
	constexpr bool is_unicode_number(char32_t codepoint) noexcept
	{
		if (codepoint < U'\u0660' || codepoint > U'\U0001E959')
			return false;

		TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0660', U'\U0001E959');
		switch ((static_cast<uint_least32_t>(codepoint) - 0x660u) / 1932u)
		{
			case 0:
			{
				if (codepoint > U'\u0DEB')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x660u) / 63u) & 0x55555025ull;
				// chunk summary: 126 codepoints from 13 ranges (spanning a search area of 1932)
			}
			case 1:
			{
				if (codepoint < U'\u0DEC' || codepoint > U'\u1099')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0xDECu) / 63u) & 0x63Bull;
				// chunk summary: 54 codepoints from 6 ranges (spanning a search area of 686)
			}
			case 2:
			{
				if (codepoint < U'\u16EE' || codepoint > U'\u1C59')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u16EE', U'\u1C59');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x16EEu) / 64u)
				{
					case 0: return true;
					case 3: return true;
					case 4: return true;
					case 9: return true;
					case 11: return true;
					case 14: return codepoint <= U'\u1A89' || codepoint >= U'\u1A90';
					case 17: return true;
					case 19: return true;
					case 21: return codepoint <= U'\u1C49' || codepoint >= U'\u1C50';
					default: return false;
				}
				// chunk summary: 103 codepoints from 11 ranges (spanning a search area of 1388)
			}
			case 3: return codepoint <= U'\u2182' || codepoint >= U'\u2185';
			case 5: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x3007ull)) & 0xE0007FC000001ull;
			case 21:
			{
				if (codepoint < U'\uA620' || codepoint > U'\uABF9')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uA620', U'\uABF9');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xA620u) / 63u)
				{
					case 0: return true;
					case 3: return true;
					case 10: return true;
					case 11: return codepoint <= U'\uA8D9' || codepoint >= U'\uA900';
					case 14: return true;
					case 15: return codepoint <= U'\uA9D9' || codepoint >= U'\uA9F0';
					case 17: return true;
					case 23: return true;
					default: return false;
				}
				// chunk summary: 80 codepoints from 8 ranges (spanning a search area of 1498)
			}
			case 32: return true;
			case 33:
			{
				if (codepoint < U'\U00010140' || codepoint > U'\U000104A9')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00010140', U'\U000104A9');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x10140u) / 63u)
				{
					case 0: return true;
					case 8: return codepoint <= U'\U00010341' || codepoint >= U'\U0001034A';
					case 10: return true;
					case 13: return true;
					default: return false;
				}
				// chunk summary: 70 codepoints from 5 ranges (spanning a search area of 874)
			}
			case 34: return true;
			case 35:
			{
				if (codepoint < U'\U00011066' || codepoint > U'\U000114D9')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x11066u) / 64u) & 0x2842Dull;
				// chunk summary: 70 codepoints from 7 ranges (spanning a search area of 1140)
			}
			case 36:
			{
				if (codepoint < U'\U00011650' || codepoint > U'\U00011D59')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x11650u) / 63u) & 0x1100040Bull;
				// chunk summary: 60 codepoints from 6 ranges (spanning a search area of 1802)
			}
			case 37: return codepoint <= U'\U00011DA9' || codepoint >= U'\U00012400';
			case 47: return codepoint <= U'\U00016A69' || codepoint >= U'\U00016B50';
			case 61: return true;
			case 62: return true;
			case 63: return codepoint <= U'\U0001E2F9' || codepoint >= U'\U0001E950';
			default: return false;
		}
		// chunk summary: 856 codepoints from 70 ranges (spanning a search area of 123642)
	}

	[[nodiscard]]
	constexpr bool is_unicode_combining_mark(char32_t codepoint) noexcept
	{
		if (codepoint < U'\u0300' || codepoint > U'\U000E01EF')
			return false;

		TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0300', U'\U000E01EF');
		switch ((static_cast<uint_least32_t>(codepoint) - 0x300u) / 14332u)
		{
			case 0:
			{
				if (codepoint > U'\u309A')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0300', U'\u309A');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x300u) / 183u)
				{
					case 0: return true;
					case 2: return true;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x591ull)) & 0x5B5FFFFFFFFFFFull;
					case 4: return codepoint <= U'\u061A' || (codepoint >= U'\u064B' && codepoint <= U'\u065F')
							|| codepoint == U'\u0670';
					case 5:
					{
						if (codepoint < U'\u06D6' || codepoint > U'\u0749')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u06D6', U'\u0749');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x6D6u) / 58u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x6D6u)) & 0xF67E7Fu;
							case 1: return codepoint <= U'\u0711' || codepoint >= U'\u0730';
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 46 codepoints from 6 ranges (spanning a search area of 116)
					}
					case 6: return codepoint == U'\u074A' || (codepoint >= U'\u07A6' && codepoint <= U'\u07B0')
							|| (codepoint >= U'\u07EB' && codepoint <= U'\u07F3') || codepoint == U'\u07FD';
					case 7:
					{
						if (codepoint < U'\u0816' || codepoint > U'\u085B')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0816', U'\u085B');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x816u) / 35u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x816u)) & 0xFBBFEFu;
							default: return true;
						}
						// chunk summary: 24 codepoints from 5 ranges (spanning a search area of 70)
					}
					case 8:
					{
						if (codepoint < U'\u08D3' || codepoint > U'\u0963')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u08D3', U'\u0963');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x8D3u) / 49u)
						{
							case 0: return codepoint != U'\u08E2';
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x93Aull)) & 0x3003FBFFFF7ull;
							default: return false;
						}
						// chunk summary: 78 codepoints from 6 ranges (spanning a search area of 145)
					}
					case 9:
					{
						if (codepoint < U'\u0981' || codepoint > U'\u0A03')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0981', U'\u0A03');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x981u) / 44u)
						{
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x9BCu)) & 0x80399FDu;
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x9E2ull)) & 0x390000003ull;
							default: return true;
						}
						// chunk summary: 23 codepoints from 9 ranges (spanning a search area of 131)
					}
					case 10:
					{
						if (codepoint < U'\u0A3C' || codepoint > U'\u0ACD')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0A3C', U'\u0ACD');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xA3Cu) / 49u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA3Cu)) & 0x23987Du;
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA70u)) & 0xE0023u;
							case 2: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xABCu)) & 0x3BBFDu;
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 33 codepoints from 12 ranges (spanning a search area of 146)
					}
					case 11:
					{
						if (codepoint < U'\u0AE2' || codepoint > U'\u0B82')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0AE2', U'\u0B82');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xAE2u) / 54u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAE2ull)) & 0x3BF000003ull;
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xB3Cu)) & 0x399FDu;
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB56ull)) & 0x100000003003ull;
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 29 codepoints from 10 ranges (spanning a search area of 161)
					}
					case 12:
					{
						if (codepoint < U'\u0BBE' || codepoint > U'\u0C4A')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0BBE', U'\u0C4A');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xBBEu) / 47u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xBBEu)) & 0x200F71Fu;
							case 2: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xC3Eu)) & 0x177Fu;
							default: return true;
						}
						// chunk summary: 29 codepoints from 8 ranges (spanning a search area of 141)
					}
					case 13:
					{
						if (codepoint < U'\u0C4B' || codepoint > U'\u0D01')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0C4B', U'\u0D01');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xC4Bu) / 61u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC4Bull)) & 0x1C0000001800C07ull;
							case 1: return codepoint != U'\u0C88';
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xCC6ull)) & 0xC000000300180F7ull;
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 31 codepoints from 11 ranges (spanning a search area of 183)
					}
					case 14:
					{
						if (codepoint < U'\u0D02' || codepoint > U'\u0D83')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0D02', U'\u0D83');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xD02u) / 44u)
						{
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xD3Bu)) & 0x1007BBFBu;
							case 2: return codepoint <= U'\u0D63' || codepoint >= U'\u0D82';
							default: return true;
						}
						// chunk summary: 23 codepoints from 8 ranges (spanning a search area of 130)
					}
					case 15:
					{
						if (codepoint < U'\u0DCA' || codepoint > U'\u0E4E')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0DCA', U'\u0E4E');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xDCAu) / 45u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xDCAull)) & 0x300003FD7E1ull;
							case 2: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xE31u)) & 0x3FC003F9u;
							default: return false;
						}
						// chunk summary: 34 codepoints from 8 ranges (spanning a search area of 133)
					}
					case 16: return codepoint == U'\u0EB1' || (codepoint >= U'\u0EB4' && codepoint <= U'\u0EBC')
							|| (codepoint >= U'\u0EC8' && codepoint <= U'\u0ECD') || codepoint >= U'\u0F18';
					case 17:
					{
						if (codepoint < U'\u0F35' || codepoint > U'\u0FC6')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0F35', U'\u0FC6');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xF35u) / 49u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xF35u)) & 0x615u;
							case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xF71ull)) & 0x3FF06FFFFFull;
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xF97ull)) & 0x803FFFFFFFFDull;
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 75 codepoints from 9 ranges (spanning a search area of 146)
					}
					case 18:
					{
						if (codepoint < U'\u102B' || codepoint > U'\u108F')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u102B', U'\u108F');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x102Bu) / 51u)
						{
							case 0: return codepoint <= U'\u103E' || codepoint >= U'\u1056';
							case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x105Eull)) & 0x2FFF00078FE77ull;
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 54 codepoints from 8 ranges (spanning a search area of 101)
					}
					case 19: return true;
					case 22: return true;
					case 28:
					{
						if (codepoint < U'\u1712' || codepoint > U'\u17BA')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u1712', U'\u17BA');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1712u) / 57u)
						{
							case 0: return codepoint <= U'\u1714' || codepoint >= U'\u1732';
							case 1: return codepoint <= U'\u1753' || codepoint >= U'\u1772';
							default: return true;
						}
						// chunk summary: 17 codepoints from 5 ranges (spanning a search area of 169)
					}
					case 29: return codepoint <= U'\u17D3' || codepoint == U'\u17DD' || codepoint >= U'\u180B';
					case 30: return codepoint <= U'\u1886' || codepoint == U'\u18A9' || codepoint >= U'\u1920';
					case 31: return codepoint <= U'\u192B' || codepoint >= U'\u1930';
					case 32: return codepoint <= U'\u1A1B' || (codepoint >= U'\u1A55' && codepoint <= U'\u1A5E')
							|| (codepoint >= U'\u1A60' && codepoint <= U'\u1A7C') || codepoint == U'\u1A7F';
					case 33: return codepoint <= U'\u1ABD' || (codepoint >= U'\u1B00' && codepoint <= U'\u1B04')
							|| codepoint >= U'\u1B34';
					case 34: return codepoint <= U'\u1B73' || (codepoint >= U'\u1B80' && codepoint <= U'\u1B82')
							|| (codepoint >= U'\u1BA1' && codepoint <= U'\u1BAD') || codepoint >= U'\u1BE6';
					case 35: return true;
					case 36: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1CD0ull)) & 0x39021FFFFF7ull;
					case 37: return codepoint != U'\u1D73';
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x20D0ull)) & 0x1FFE21FFFull;
					case 58: return true;
					case 59: return codepoint <= U'\u2D7F' || codepoint >= U'\u2DE0';
					case 60: return true;
					case 63: return codepoint <= U'\u302F' || codepoint >= U'\u3099';
					default: return false;
				}
				// chunk summary: 1102 codepoints from 155 ranges (spanning a search area of 11675)
			}
			case 2:
			{
				if (codepoint < U'\uA66F' || codepoint > U'\uAAEF')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uA66F', U'\uAAEF');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xA66Fu) / 61u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA66Full)) & 0x1800000007FE1ull;
					case 1: return false;
					case 3: return false;
					case 4: return false;
					case 5: return false;
					case 6: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA802u)) & 0x211u;
					case 10: return codepoint <= U'\uA8F1' || codepoint >= U'\uA8FF';
					case 11: return codepoint <= U'\uA92D' || codepoint >= U'\uA947';
					case 12: return codepoint <= U'\uA953' || codepoint >= U'\uA980';
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA43ull)) & 0x100000000000601ull;
					case 17: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA7Cull)) & 0x19D0000000000003ull;
					case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAABEull)) & 0x3E0000000000Bull;
					default: return true;
				}
				// chunk summary: 136 codepoints from 27 ranges (spanning a search area of 1153)
			}
			case 3: return codepoint <= U'\uAAF6' || (codepoint >= U'\uABE3' && codepoint <= U'\uABEA')
					|| codepoint >= U'\uABEC';
			case 4:
			{
				if (codepoint < U'\uFB1E' || codepoint > U'\U00011A99')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uFB1E', U'\U00011A99');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xFB1Eu) / 128u)
				{
					case 0: return true;
					case 5: return true;
					case 6: return true;
					case 13: return true;
					case 15: return true;
					case 16: return true;
					case 29: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x10A01u)) & 0x7837u;
					case 30: return codepoint <= U'\U00010A3A' || codepoint >= U'\U00010A3F';
					case 31: return true;
					case 36: return true;
					case 40: return true;
					case 41: return true;
					case 42: return codepoint <= U'\U00011046' || codepoint >= U'\U0001107F';
					case 43: return codepoint <= U'\U000110BA' || codepoint >= U'\U00011100';
					case 44: return codepoint <= U'\U00011134' || (codepoint >= U'\U00011145' && codepoint <= U'\U00011146')
							|| codepoint == U'\U00011173' || codepoint >= U'\U00011180';
					case 45: return codepoint <= U'\U000111C0' || codepoint >= U'\U000111C9';
					case 46: return codepoint <= U'\U00011237' || codepoint >= U'\U0001123E';
					case 47: return codepoint <= U'\U000112EA' || codepoint >= U'\U00011300';
					case 48: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1133Bull)) & 0x3E3F980100733FBull;
					case 50: return codepoint <= U'\U00011446' || codepoint >= U'\U0001145E';
					case 51: return true;
					case 53: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x115AFull)) & 0x60000003FE7Full;
					case 54: return true;
					case 55: return codepoint <= U'\U000116B7' || codepoint >= U'\U0001171D';
					case 56: return true;
					case 58: return true;
					case 61: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x119D1ull)) & 0x3FF00000008FE7Full;
					case 62:
					{
						if (codepoint < U'\U00011A33')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00011A33', U'\U00011A99');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x11A33u) / 52u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11A33ull)) & 0x1FFC0100F7Full;
							default: return true;
						}
						// chunk summary: 39 codepoints from 5 ranges (spanning a search area of 103)
					}
					default: return false;
				}
				// chunk summary: 383 codepoints from 56 ranges (spanning a search area of 8060)
			}
			case 5:
			{
				if (codepoint < U'\U00011C2F' || codepoint > U'\U00011EF6')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00011C2F', U'\U00011EF6');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x11C2Fu) / 60u)
				{
					case 0: return codepoint != U'\U00011C37';
					case 1: return true;
					case 2: return codepoint != U'\U00011CA8';
					case 4: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x11D31u)) & 0x5FDA3Fu;
					case 5: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x11D8Au)) & 0x1EDFu;
					case 6: return true;
					case 11: return true;
					default: return false;
				}
				// chunk summary: 85 codepoints from 13 ranges (spanning a search area of 712)
			}
			case 6:
			{
				if (codepoint < U'\U00016AF0' || codepoint > U'\U00016F92')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00016AF0', U'\U00016F92');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x16AF0u) / 63u)
				{
					case 0: return true;
					case 1: return true;
					case 17: return codepoint != U'\U00016F1F';
					case 18: return codepoint <= U'\U00016F87' || codepoint >= U'\U00016F8F';
					default: return false;
				}
				// chunk summary: 72 codepoints from 5 ranges (spanning a search area of 1187)
			}
			case 7: return true;
			case 8:
			{
				if (codepoint < U'\U0001D165' || codepoint > U'\U0001E94A')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001D165', U'\U0001E94A');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1D165u) / 128u)
				{
					case 0:
					{
						if (codepoint > U'\U0001D1AD')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001D165', U'\U0001D1AD');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1D165u) / 37u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D165ull)) & 0x1F3FC03F1Full;
							case 1: return codepoint <= U'\U0001D18B' || codepoint >= U'\U0001D1AA';
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 30 codepoints from 5 ranges (spanning a search area of 73)
					}
					case 1: return true;
					case 17: return codepoint <= U'\U0001DA36' || codepoint >= U'\U0001DA3B';
					case 18:
					{
						if (codepoint < U'\U0001DA65' || codepoint > U'\U0001DAAF')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001DA65', U'\U0001DAAF');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1DA65u) / 38u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x1DA65u)) & 0x800100FFu;
							case 1: return codepoint != U'\U0001DA8B';
							TOML_NO_DEFAULT_CASE;
						}
						// chunk summary: 30 codepoints from 5 ranges (spanning a search area of 75)
					}
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1E000ull)) & 0x7DBF9FFFF7Full;
					case 31: return true;
					case 35: return true;
					case 46: return true;
					case 47: return true;
					default: return false;
				}
				// chunk summary: 223 codepoints from 21 ranges (spanning a search area of 6118)
			}
			case 63: return true;
			default: return false;
		}
		// chunk summary: 2255 codepoints from 282 ranges (spanning a search area of 917232)
	}
}

#undef TOML_ASSUME_CODEPOINT_BETWEEN

#endif // TOML_LANG_HIGHER_THAN(0, 5, 0)

namespace toml::impl
{
	[[nodiscard]]
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
		// see: https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that don't say "is a line-break")

		return codepoint == U'\t'
			|| codepoint == U' '
			|| codepoint == U'\u00A0' // no-break space
			|| codepoint == U'\u1680' // ogham space mark
			|| (codepoint >= U'\u2000' && codepoint <= U'\u200A') // em quad -> hair space
			|| codepoint == U'\u202F' // narrow no-break space
			|| codepoint == U'\u205F' // medium mathematical space
			|| codepoint == U'\u3000' // ideographic space
		;
	}

	template <bool CR = true>
	[[nodiscard]]
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		// see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that say "is a line-break")

		constexpr auto low_range_end = CR ? U'\r' : U'\f';

		return (codepoint >= U'\n' && codepoint <= low_range_end)
			|| codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_string_delimiter(char32_t codepoint) noexcept
	{
		return codepoint == U'"'
			|| codepoint == U'\'';
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_letter(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'z')
			|| (codepoint >= U'A' && codepoint <= U'Z');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_binary_digit(char32_t codepoint) noexcept
	{
		return codepoint == U'0' || codepoint == U'1';
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_octal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'7');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_decimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'9');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_hex_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'f')
			|| (codepoint >= U'A' && codepoint <= U'F')
			|| is_decimal_digit(codepoint);
	}

	[[nodiscard]]
	constexpr bool is_bare_key_start_character(char32_t codepoint) noexcept
	{
		return is_ascii_letter(codepoint)
			|| is_decimal_digit(codepoint)
			|| codepoint == U'-'
			|| codepoint == U'_'
			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/644 & toml/issues/687
            || codepoint == U'+'
			|| is_unicode_letter(codepoint)
			|| is_unicode_number(codepoint)
			#endif
		;
	}

	[[nodiscard]]
	constexpr bool is_bare_key_character(char32_t codepoint) noexcept
	{
		return is_bare_key_start_character(codepoint)
			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
			|| is_unicode_combining_mark(codepoint)
			#endif
		;
	}

	[[nodiscard]]
	constexpr bool is_value_terminator(char32_t codepoint) noexcept
	{
		return is_line_break(codepoint)
			|| is_whitespace(codepoint)
			|| codepoint == U']'
			|| codepoint == U'}'
			|| codepoint == U','
			|| codepoint == U'#'
		;
	}

	struct utf8_decoder final
	{
		// This decoder is based on code from here: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
		//
		// License:
		//
		// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
		//
		// Permission is hereby granted, free of charge, to any person obtaining a copy of this
		// software and associated documentation files (the "Software"), to deal in the Software
		// without restriction, including without limitation the rights to use, copy, modify, merge,
		// publish, distribute, sublicense, and/or sell copies of the Software, and to permit
		// persons to whom the Software is furnished to do so, subject to the following conditions:
		//
		// The above copyright notice and this permission notice shall be included in all copies
		// or substantial portions of the Software.
		//
		// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
		// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
		// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
		// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
		// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
		// DEALINGS IN THE SOFTWARE.

		uint_least32_t state{};
		char32_t codepoint{};

		static constexpr uint8_t state_table[]
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,		11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

			0,12,24,36,60,96,84,12,12,12,48,72,		12,12,12,12,12,12,12,12,12,12,12,12,
			12, 0,12,12,12,12,12, 0,12, 0,12,12,	12,24,12,12,12,12,12,24,12,24,12,12,
			12,12,12,12,12,12,12,24,12,12,12,12,	12,24,12,12,12,12,12,12,12,24,12,12,
			12,12,12,12,12,12,12,36,12,36,12,12,	12,36,12,12,12,12,12,36,12,36,12,12,
			12,36,12,12,12,12,12,12,12,12,12,12
		};

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool has_code_point() const noexcept
		{
			return state == uint_least32_t{};
		}

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool needs_more_input() const noexcept
		{
			return state > uint_least32_t{} && state != uint_least32_t{ 12u };
		}

		constexpr void operator () (uint8_t byte) noexcept
		{
			TOML_ASSERT(!error());

			const auto type = state_table[byte];

			codepoint = static_cast<char32_t>(
				has_code_point()
					? (uint_least32_t{ 255u } >> type) & byte
					: (byte & uint_least32_t{ 63u }) | (static_cast<uint_least32_t>(codepoint) << 6)
				);

			state = state_table[state + uint_least32_t{ 256u } + type];
		}
	};

	template <typename T>
	class utf8_byte_stream;

	template <typename CHAR>
	class utf8_byte_stream<std::basic_string_view<CHAR>> final
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_string_view<CHAR> source;
			size_t position = {};

		public:
			explicit constexpr utf8_byte_stream(std::basic_string_view<CHAR> sv) noexcept
				: source{ sv }
			{
				if (source.length() >= 3_sz
					&& static_cast<uint8_t>(source[0]) == 0xEF_u8
					&& static_cast<uint8_t>(source[1]) == 0xBB_u8
					&& static_cast<uint8_t>(source[2]) == 0xBF_u8)
				{
					position += 3_sz;
				}
			}

			[[nodiscard]]
			constexpr bool eof() const noexcept
			{
				return position >= source.length();
			}

			[[nodiscard]]
			constexpr bool error() const noexcept
			{
				return false;
			}

			[[nodiscard]]
			constexpr std::optional<uint8_t> operator() () noexcept
			{
				if (position >= source.length())
					return {};
				return static_cast<uint8_t>(source[position++]);
			}
	};

	template <typename CHAR>
	class utf8_byte_stream<std::basic_istream<CHAR>> final
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_istream<CHAR>* source;

		public:
			explicit utf8_byte_stream(std::basic_istream<CHAR>& stream) TOML_MAY_THROW
				: source{ &stream }
			{
				if (*source)
				{
					static constexpr uint8_t bom[] {
						0xEF_u8,
						0xBB_u8,
						0xBF_u8
					};

					using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
					const auto initial_pos = source->tellg();
					size_t bom_pos{};
					auto bom_char = source->get();
					while (*source && bom_char != stream_traits::eof() && bom_char == bom[bom_pos])
					{
						bom_pos++;
						bom_char = source->get();
					}
					if (!(*source) || bom_pos < 3_sz)
						source->seekg(initial_pos);
				}
			}

			[[nodiscard]]
			bool eof() const noexcept
			{
				return source->eof();
			}

			[[nodiscard]]
			bool error() const noexcept
			{
				return !(*source);
			}

			[[nodiscard]]
			std::optional<uint8_t> operator() () TOML_MAY_THROW
			{
				auto val = source->get();
				if (val == std::basic_istream<CHAR>::traits_type::eof())
					return {};
				return static_cast<uint8_t>(val);
			}
	};

	struct utf8_codepoint final
	{
		char32_t value;
		uint8_t bytes[4];
		toml::source_position position;

		template <typename CHAR = toml::string_char>
		[[nodiscard]] TOML_ALWAYS_INLINE
		std::basic_string_view<CHAR> as_view() const noexcept
		{
			static_assert(
				sizeof(CHAR) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes), 4_sz }
				: std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes) };
		}

		[[nodiscard]]
		constexpr operator char32_t& () noexcept
		{
			return value;
		}

		[[nodiscard]]
		constexpr operator const char32_t& () const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK	(void)0
		#define TOML_ERROR(...)		throw toml::parse_error{ __VA_ARGS__ }
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR(...)		err.emplace( __VA_ARGS__ )
	#endif

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const std::shared_ptr<const std::string>& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() TOML_MAY_THROW = 0;

		#if !TOML_EXCEPTIONS

		[[nodiscard]]
		virtual std::optional<toml::parse_error>&& error() noexcept = 0;

		#endif

		virtual ~utf8_reader_interface() noexcept = default;
	};

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_byte_stream<T> stream;
			utf8_decoder decoder;
			utf8_codepoint prev{}, current{};
			uint8_t current_byte_count{};
			std::shared_ptr<const std::string> source_path_;
			#if !TOML_EXCEPTIONS
			std::optional<toml::parse_error> err;
			#endif

		public:

			template <typename U, typename STR = std::string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				TOML_CONDITIONAL_NOEXCEPT(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				current.position = { 1u, 1u };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<STR>(source_path));
			}

			[[nodiscard]]
			const std::shared_ptr<const std::string>& source_path() const noexcept override
			{
				return source_path_;
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() TOML_MAY_THROW override
			{
				TOML_ERROR_CHECK;

				if (stream.eof())
					return nullptr;
				else if (stream.error())
					TOML_ERROR("An error occurred while reading from the underlying stream", prev.position, source_path_ );
				else if (decoder.error())
					TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

				TOML_ERROR_CHECK;

				while (true)
				{
					std::optional<uint8_t> nextByte;
					if constexpr (!TOML_EXCEPTIONS || noexcept(stream()))
					{
						nextByte = stream();
					}
					#if TOML_EXCEPTIONS
					else
					{
						try
						{
							nextByte = stream();
						}
						catch (const std::exception& exc)
						{
							throw toml::parse_error{ exc.what(), prev.position, source_path_ };
						}
						catch (...)
						{
							throw toml::parse_error{ "An unspecified error occurred", prev.position, source_path_ };
						}
					}
					#endif

					if (!nextByte)
					{
						if (stream.eof())
						{
							if (decoder.needs_more_input())
								TOML_ERROR("Encountered EOF during incomplete utf-8 code point sequence",
									prev.position, source_path_);
							return nullptr;
						}
						else
							TOML_ERROR("An error occurred while reading from the underlying stream",
								prev.position, source_path_);
					}

					TOML_ERROR_CHECK;

					decoder(*nextByte);
					if (decoder.error())
						TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

					TOML_ERROR_CHECK;

					current.bytes[current_byte_count++] = *nextByte;
					if (decoder.has_code_point())
					{
						current.value = decoder.codepoint;
						prev = current;
						std::memset(current.bytes, 0, sizeof(current.bytes));
						current_byte_count = {};

						if (is_line_break<false>(prev.value))
						{
							current.position.line++;
							current.position.column = 1u;
						}
						else
							current.position.column++;

						return &prev;
					}
				}
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			std::optional<toml::parse_error>&& error() noexcept override
			{
				return std::move(err);
			}

			#endif
	};

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, std::string_view) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, std::string_view) -> utf8_reader<std::basic_istream<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, std::string&&) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, std::string&&) -> utf8_reader<std::basic_istream<CHAR>>;

	#if !TOML_EXCEPTIONS
		#undef TOML_ERROR_CHECK
		#define TOML_ERROR_CHECK	if (reader.error()) return nullptr
	#endif

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		public:
			static constexpr auto max_history_length = 64_sz;

		private:
			static constexpr auto history_buffer_size = max_history_length - 1_sz; //'head' is stored in the reader
			utf8_reader_interface& reader;
			struct
			{

				utf8_codepoint buffer[history_buffer_size];
				size_t count, first;
			}
			history = {};
			const utf8_codepoint* head = {};
			size_t negative_offset = {};

		public:

			explicit utf8_buffered_reader(utf8_reader_interface& reader_) noexcept
				: reader{ reader_ }
			{}

			[[nodiscard]]
			const std::shared_ptr<const std::string>& source_path() const noexcept override
			{
				return reader.source_path();
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() TOML_MAY_THROW override
			{
				TOML_ERROR_CHECK;

				if (negative_offset)
				{
					negative_offset--;

					// an entry negative offset of 1 just means "replay the current head"
					if (!negative_offset)
						return head;

					// otherwise step back into the history buffer
					else
						return history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size);
				}
				else
				{
					// first character read from stream
					if (!history.count && !head) TOML_UNLIKELY
						head = reader.read_next();

					// subsequent characters and not eof
					else if (head)
					{
						if (history.count < history_buffer_size) TOML_UNLIKELY
							history.buffer[history.count++] = *head;
						else
							history.buffer[(history.first++ + history_buffer_size) % history_buffer_size] = *head;

						head = reader.read_next();
					}

					return head;
				}
			}

			[[nodiscard]]
			const utf8_codepoint* step_back(size_t count) noexcept
			{
				TOML_ERROR_CHECK;
				TOML_ASSERT(history.count);
				TOML_ASSERT(negative_offset + count <= history.count);

				negative_offset += count;

				return negative_offset
					? history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size)
					: head;
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			std::optional<toml::parse_error>&& error() noexcept override
			{
				return reader.error();
			}

			#endif
	};

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
}

#pragma endregion
//-------------  ↑ toml_utf8.h  ----------------------------------------------------------------------------------------

//-------------------------------  ↓ toml_node_view.h  -----------------------------------------------------------------
#pragma region

namespace toml::impl
{
	template <typename T>
	struct node_view_traits;

	template <>
	struct node_view_traits<const table>
	{
		using member_type = const table*;
		using key_type = string_view;

		[[nodiscard]] static const node* get(const table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const table* tbl, key_type key) noexcept
		{
			return tbl->get_as<U>(key);
		}
	};

	template <>
	struct node_view_traits<table>
	{
		using member_type = table*;
		using key_type = string_view;

		[[nodiscard]] static node* get(table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		[[nodiscard]] static const node* get(const table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		template <typename T>
		[[nodiscard]] static node_of<T>* as(table* tbl, key_type key) noexcept
		{
			return tbl->get_as<T>(key);
		}

		template <typename T>
		[[nodiscard]] static const node_of<T>* as(const table* tbl, key_type key) noexcept
		{
			return tbl->get_as<T>(key);
		}
	};

	template <typename T, typename K>
	struct sub_view final { };

	template <typename T>
	struct node_view_traits<sub_view<T, string_view>>
	{
		using member_type = T;
		using key_type = string_view;

		[[nodiscard]] static auto get(member_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->get(key) : nullptr;
		}

		[[nodiscard]] static const node* get(const member_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->get(key) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static auto as(member_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->template get_as<U>(key) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const member_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->template get_as<U>(key) : nullptr;
		}
	};

	template <typename T>
	struct node_view_traits<sub_view<T, size_t>>
	{
		using member_type = T;
		using key_type = size_t;

		[[nodiscard]] static auto get(member_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->get(index) : nullptr;
		}

		[[nodiscard]] static const node* get(const member_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->get(index) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static auto as(member_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->template get_as<U>(index) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const member_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->template get_as<U>(index) : nullptr;
		}
	};
}

namespace toml
{
	template <typename T>
	class node_view final
	{
		public:
			using traits = impl::node_view_traits<T>;
			using key_type = typename traits::key_type;

		private:
			using member_type = typename traits::member_type;
			member_type obj_;
			key_type key_;

		public:

			TOML_NODISCARD_CTOR
			node_view(member_type obj, key_type key) noexcept
				: obj_{ obj },
				key_{ key }
			{}

			[[nodiscard]] auto get() noexcept { return traits::get(obj_, key_); }
			[[nodiscard]] const node* get() const noexcept { return traits::get(obj_, key_); }

			[[nodiscard]] explicit operator bool() const noexcept { return !!get(); }

			template <typename U>
			[[nodiscard]] auto as() noexcept
			{
				static_assert(
					impl::is_value_or_node<U>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return traits::template as<U>(obj_, key_);
			}

			template <typename U>
			[[nodiscard]] const node_of<U>* as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<U>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return traits::template as<U>(obj_, key_);
			}

			[[nodiscard]] auto as_string() noexcept { return as<string>(); }
			[[nodiscard]] auto as_integer() noexcept { return as<int64_t>(); }
			[[nodiscard]] auto as_floating_point() noexcept { return as<double>(); }
			[[nodiscard]] auto as_boolean() noexcept { return as<bool>(); }
			[[nodiscard]] auto as_date() noexcept { return as<date>(); }
			[[nodiscard]] auto as_time() noexcept { return as<time>(); }
			[[nodiscard]] auto as_date_time() noexcept { return as<date_time>(); }
			[[nodiscard]] auto as_array() noexcept { return as<array>(); }
			[[nodiscard]] auto as_table() noexcept { return as<table>(); }

			[[nodiscard]] const value<string>* as_string() const noexcept { return as<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept { return as<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept { return as<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept { return as<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept { return as<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept { return as<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept { return as<date_time>(); }
			[[nodiscard]] const array* as_array() const noexcept { return as<array>(); }
			[[nodiscard]] const table* as_table() const noexcept { return as<table>(); }

		private:

			template <typename U>
			[[nodiscard]] static bool value_equality(const node_view& lhs, const U& rhs) noexcept
			{
				const auto val = lhs.as<value_of<U>>();
				return val && val->get() == rhs;
			}

			template <typename U>
			[[nodiscard]] static bool container_equality(const node_view& lhs, const U& rhs) noexcept
			{
				using elem_t = std::remove_const_t<typename U::value_type>;

				static_assert(
					impl::is_value_or_promotable<elem_t>,
					"Container element type must be (or be convertible to) one of the basic value types"
				);

				const array* arr = lhs.as<array>();
				if (!arr || arr->size() != rhs.size())
					return false;
				if (rhs.size() == 0_sz)
					return true;

				size_t i{};
				for (auto& list_elem : rhs)
				{
					const auto elem = arr->get_as<value_of<elem_t>>(i++);
					if (!elem || elem->get() != list_elem)
						return false;
				}

				return true;
			}

		public:

			[[nodiscard]] bool operator == (const string& rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (string_view rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int64_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int32_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int16_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int8_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint32_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint16_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint8_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (double rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (float rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (bool rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const date& rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const time& rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const date_time& rhs) const noexcept { return value_equality(*this, rhs); }

			template <typename U>
			[[nodiscard]] bool operator == (const std::initializer_list<U>& rhs) const noexcept
			{
				return container_equality(*this, rhs);
			}

			template <typename U>
			[[nodiscard]] bool operator == (const std::vector<U>& rhs) const noexcept
			{
				return container_equality(*this, rhs);
			}

			template <typename U>
			[[nodiscard]] friend bool operator == (const U& lhs, const node_view& rhs) noexcept
			{
				return rhs == lhs;
			}

			[[nodiscard]] node_view<impl::sub_view<node_view<T>, string_view>> operator[] (string_view key) noexcept
			{
				return { *this, key };
			}

			[[nodiscard]] node_view<impl::sub_view<node_view<T>, size_t>> operator[] (size_t index) noexcept
			{
				return { *this, index };
			}
	};

	inline node_view<table> table::operator[] (string_view key) noexcept
	{
		return { this, key };
	}

	inline node_view<const table> table::operator[] (string_view key) const noexcept
	{
		return { this, key };
	}
}

#pragma endregion
//-------------------------------  ↑ toml_node_view.h  -----------------------------------------------------------------

//-------------------------------------------------  ↓ toml_parser.h  --------------------------------------------------
#pragma region

namespace toml
{
	#if TOML_EXCEPTIONS

	using parse_result = table;

	#else

	class parse_result final
	{
		private:
			bool is_err;
			std::aligned_storage_t<
				(sizeof(table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(table)),
				(alignof(table) < alignof(parse_error) ? alignof(parse_error) : alignof(table))
			> storage;

			void destroy() noexcept
			{
				if (is_err)
					std::launder(reinterpret_cast<parse_error*>(&storage))->~parse_error();
				else
					std::launder(reinterpret_cast<table*>(&storage))->~table();
			}

		public:

			[[nodiscard]] bool succeeded() const noexcept { return !is_err; }
			[[nodiscard]] bool failed() const noexcept { return is_err; }
			[[nodiscard]] explicit operator bool() const noexcept { return !is_err; }

			[[nodiscard]] table& get() & noexcept
			{
				TOML_ASSERT(!is_err);
				return *std::launder(reinterpret_cast<table*>(&storage));
			}
			[[nodiscard]] table&& get() && noexcept
			{
				TOML_ASSERT(!is_err);
				return std::move(*std::launder(reinterpret_cast<table*>(&storage)));
			}
			[[nodiscard]] const table& get() const& noexcept
			{
				TOML_ASSERT(!is_err);
				return *std::launder(reinterpret_cast<const table*>(&storage));
			}

			[[nodiscard]] parse_error& error() & noexcept
			{
				TOML_ASSERT(is_err);
				return *std::launder(reinterpret_cast<parse_error*>(&storage));
			}
			[[nodiscard]] parse_error&& error() && noexcept
			{
				TOML_ASSERT(is_err);
				return std::move(*std::launder(reinterpret_cast<parse_error*>(&storage)));
			}
			[[nodiscard]] const parse_error& error() const& noexcept
			{
				TOML_ASSERT(is_err);
				return *std::launder(reinterpret_cast<const parse_error*>(&storage));
			}

			[[nodiscard]] table& operator* () & noexcept { return get(); }
			[[nodiscard]] table&& operator* () && noexcept { return std::move(get()); }
			[[nodiscard]] const table& operator* () const & noexcept { return get(); }

			[[nodiscard]] table* operator-> () noexcept { return &get(); }
			[[nodiscard]] const table* operator-> () const noexcept { return &get(); }

			[[nodiscard]] operator table& () noexcept { return get(); }
			[[nodiscard]] operator table&& () noexcept { return std::move(get()); }
			[[nodiscard]] operator const table& () const noexcept { return get(); }

			[[nodiscard]] explicit operator parse_error& () noexcept { return error(); }
			[[nodiscard]] explicit operator parse_error && () noexcept { return std::move(error()); }
			[[nodiscard]] explicit operator const parse_error& () const noexcept { return error(); }

			TOML_NODISCARD_CTOR
			explicit parse_result(table&& tbl) noexcept
				: is_err{ false }
			{
				::new (&storage) table{ std::move(tbl) };
			}

			TOML_NODISCARD_CTOR
			explicit parse_result(parse_error&& err) noexcept
				: is_err{ true }
			{
				::new (&storage) parse_error{ std::move(err) };
			}

			TOML_NODISCARD_CTOR
			parse_result(parse_result&& res) noexcept
				: is_err{ res.is_err }
			{
				if (is_err)
					::new (&storage) parse_error{ std::move(res).error() };
				else
					::new (&storage) table{ std::move(res).get() };
			}

			parse_result& operator=(parse_result&& rhs) noexcept
			{
				if (is_err != rhs.is_err)
				{
					destroy();
					is_err = rhs.is_err;
					if (is_err)
						::new (&storage) parse_error{ std::move(rhs).error() };
					else
						::new (&storage) table{ std::move(rhs).get() };
				}
				else
				{
					if (is_err)
						error() = std::move(rhs).error();
					else
						get() = std::move(rhs).get();
				}
				return *this;
			}

			~parse_result() noexcept
			{
				destroy();
			}
	};

	#endif
}

namespace toml::impl
{
	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK(...)	(void)0
		#define TOML_ERROR(...)			throw toml::parse_error{ __VA_ARGS__ }
	#else
		#define TOML_ERROR_CHECK(...)	if (err) return __VA_ARGS__
		#define TOML_ERROR(...)			err.emplace( __VA_ARGS__ )
	#endif

	class parser final
	{
		private:
			utf8_buffered_reader reader;
			table root;
			source_position prev_pos = { 1u, 1u };
			const utf8_codepoint* cp = {};
			std::vector<table*> implicit_tables;
			std::vector<table*> dotted_key_tables;
			std::vector<array*> table_arrays;
			std::string recording_buffer; //for diagnostics
			bool recording = false;
			#if !TOML_EXCEPTIONS
			mutable std::optional<toml::parse_error> err;
			#endif

			[[nodiscard]]
			source_position current_position_or_assumed_next() const noexcept
			{
				if (cp)
					return cp->position;
				return { prev_pos.line, prev_pos.column + 1 };
			}

			template <typename... T>
			#if TOML_EXCEPTIONS
			[[noreturn]]
			#endif
			void abort_with_error(T &&... args) const TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();

				if constexpr (sizeof...(T) == 0_sz)
					TOML_ERROR( "An unspecified error occurred", current_position_or_assumed_next(), reader.source_path() );
				else
				{
					static constexpr auto buf_size = 512_sz;
					char buf[buf_size];
					auto ptr = buf;
					const auto concatenator = [&](auto&& arg) noexcept //a.k.a. "no stringstreams, thanks"
					{
						using arg_t = remove_cvref_t<decltype(arg)>;
						if constexpr (std::is_same_v<arg_t, std::string_view> || std::is_same_v<arg_t, std::string>)
						{
							std::memcpy(ptr, arg.data(), arg.length());
							ptr += arg.length();
						}
						else if constexpr (std::is_same_v<arg_t, utf8_codepoint>)
						{
							std::string_view cp_view;
							switch (arg.value)
							{
								case U'\x00': cp_view = "\\u0000"sv; break;
								case U'\x01': cp_view = "\\u0001"sv; break;
								case U'\x02': cp_view = "\\u0002"sv; break;
								case U'\x03': cp_view = "\\u0003"sv; break;
								case U'\x04': cp_view = "\\u0004"sv; break;
								case U'\x05': cp_view = "\\u0005"sv; break;
								case U'\x06': cp_view = "\\u0006"sv; break;
								case U'\x07': cp_view = "\\u0007"sv; break;
								case U'\x08': cp_view = "\\b"sv; break;
								case U'\x09': cp_view = "\\t"sv; break;
								case U'\x0A': cp_view = "\\n"sv; break;
								case U'\x0B': cp_view = "\\u000B"sv; break;
								case U'\x0C': cp_view = "\\f"sv; break;
								case U'\x0D': cp_view = "\\r"sv; break;
								case U'\x0E': cp_view = "\\u000E"sv; break;
								case U'\x0F': cp_view = "\\u000F"sv; break;
								case U'\x10': cp_view = "\\u0010"sv; break;
								case U'\x11': cp_view = "\\u0011"sv; break;
								case U'\x12': cp_view = "\\u0012"sv; break;
								case U'\x13': cp_view = "\\u0013"sv; break;
								case U'\x14': cp_view = "\\u0014"sv; break;
								case U'\x15': cp_view = "\\u0015"sv; break;
								case U'\x16': cp_view = "\\u0016"sv; break;
								case U'\x17': cp_view = "\\u0017"sv; break;
								case U'\x18': cp_view = "\\u0018"sv; break;
								case U'\x19': cp_view = "\\u0019"sv; break;
								case U'\x1A': cp_view = "\\u001A"sv; break;
								case U'\x1B': cp_view = "\\u001B"sv; break;
								case U'\x1C': cp_view = "\\u001C"sv; break;
								case U'\x1D': cp_view = "\\u001D"sv; break;
								case U'\x1E': cp_view = "\\u001E"sv; break;
								case U'\x1F': cp_view = "\\u001F"sv; break;
								case U'\x7F': cp_view = "\\u007F"sv; break;

								default:
									cp_view = arg.template as_view<char>();
							}
							std::memcpy(ptr, cp_view.data(), cp_view.length());
							ptr += cp_view.length();
						}
						else if constexpr (std::is_same_v<arg_t, char>)
						{
							*ptr++ = arg;
						}
						else if constexpr (std::is_same_v<arg_t, bool>)
						{
							const auto boolval = arg ? "true"sv : "false"sv;
							std::memcpy(ptr, boolval.data(), boolval.length());
							ptr += boolval.length();
						}
						else if constexpr (std::is_same_v<arg_t, node_type>)
						{
							std::string_view type;
							switch (arg)
							{
								case node_type::array: type = "array"sv; break;
								case node_type::table: type = "table"sv; break;
								case node_type::string: type = "string"sv; break;
								case node_type::integer: type = "integer"sv; break;
								case node_type::floating_point: type = "floating-point"sv; break;
								case node_type::boolean: type = "boolean"sv; break;
								case node_type::date: type = "date"sv; break;
								case node_type::time: type = "time"sv; break;
								case node_type::date_time: type = "date-time"sv; break;
								TOML_NO_DEFAULT_CASE;
							}
							std::memcpy(ptr, type.data(), type.length());
							ptr += type.length();
						}
						else if constexpr (std::is_floating_point_v<arg_t>)
						{
							#if TOML_USE_STREAMS_FOR_FLOATS
							{
								std::ostringstream oss;
								oss << arg;
								const auto str = oss.str();
								std::memcpy(ptr, str.c_str(), str.length());
								ptr += str.length();
							}
							#else
							{
								const auto result = std::to_chars(ptr, buf + buf_size, arg);
								ptr = result.ptr;
							}
							#endif
						}
						else if constexpr (std::is_integral_v<arg_t>)
						{
							const auto result = std::to_chars(ptr, buf + buf_size, arg);
							ptr = result.ptr;
						}
					};
					(concatenator(std::forward<T>(args)), ...);
					*ptr = '\0';
					TOML_ERROR( buf, current_position_or_assumed_next(), reader.source_path() );
				}
			}

			void go_back(size_t count = 1_sz) noexcept
			{
				TOML_ERROR_CHECK();
				TOML_ASSERT(count);

				cp = reader.step_back(count);
				prev_pos = cp->position;
			}

			void advance() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();
				TOML_ASSERT(cp);

				prev_pos = cp->position;
				cp = reader.read_next();

				#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
				#endif

				if (recording && cp)
					recording_buffer.append(cp->as_view<char>());
			}

			void start_recording(bool include_current = true) noexcept
			{
				TOML_ERROR_CHECK();

				recording = true;
				recording_buffer.clear();
				if (include_current && cp)
					recording_buffer.append(cp->as_view<char>());
			}

			void stop_recording(size_t pop_bytes = 0_sz) noexcept
			{
				TOML_ERROR_CHECK();

				recording = false;
				if (pop_bytes)
				{
					if (pop_bytes >= recording_buffer.length())
						recording_buffer.clear();
					else
						recording_buffer.erase(
							recording_buffer.cbegin() + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
							recording_buffer.cend()
						);
				}
			}

			bool consume_leading_whitespace() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				bool consumed = false;
				while (cp && is_whitespace(*cp))
				{
					consumed = true;
					advance();
					TOML_ERROR_CHECK({});
				}
				return consumed;
			}

			bool consume_line_break() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp || !is_line_break(*cp))
					return false;

				if (*cp == U'\r')
				{
					advance();  // skip \r
					TOML_ERROR_CHECK({});

					if (!cp)
						abort_with_error("Encountered EOF while consuming CRLF"sv);
					if (*cp != U'\n')
						abort_with_error("Encountered unexpected character while consuming CRLF"sv);
				}
				advance(); // skip \n (or other single-character line ending)
				TOML_ERROR_CHECK({});

				return true;
			}

			bool consume_rest_of_line() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp)
					return false;

				do
				{
					if (is_line_break(*cp))
						return consume_line_break();
					else
						advance();

					TOML_ERROR_CHECK({});
				}
				while (cp);
				return true;
			}

			bool consume_comment() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp || *cp != U'#')
					return false;

				advance(); //skip the '#'
				TOML_ERROR_CHECK({});

				while (cp)
				{
					if (consume_line_break())
						return true;

					if constexpr (TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/567
					{
						if (*cp <= U'\u0008'
							|| (*cp >= U'\u000A' && *cp <= U'\u001F')
							|| *cp == U'\u007F')
							abort_with_error(
								"Encountered unexpected character while parsing comment; control characters "
								"U+0000-U+0008, U+000A-U+001F and U+007F are explicitly prohibited from appearing "
								"in comments."sv
							);
					}

					advance();
					TOML_ERROR_CHECK({});
				}

				return true;
			}

			[[nodiscard]]
			bool consume_expected_sequence(std::u32string_view seq) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				for (auto c : seq)
				{
					if (!cp || *cp != c)
						return false;

					advance();
					TOML_ERROR_CHECK({});
				}
				return true;
			}

			template <typename T, size_t N>
			[[nodiscard]]
			bool consume_digit_sequence(T(&digits)[N]) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				for (size_t i = 0; i < N; i++)
				{
					if (!cp || !is_decimal_digit(*cp))
						return false;
					digits[i] = static_cast<T>(*cp - U'0');
					advance();
					TOML_ERROR_CHECK({});
				}
				return true;
			}

			template <typename T, size_t N>
			[[nodiscard]]
			size_t consume_variable_length_digit_sequence(T(&buffer)[N]) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				size_t i = {};
				for (; i < N; i++)
				{
					if (!cp || !is_decimal_digit(*cp))
						break;
					buffer[i] = static_cast<T>(*cp - U'0');
					advance();
					TOML_ERROR_CHECK({});
				}
				return i;
			}

			template <bool MULTI_LINE>
			[[nodiscard]]
			string parse_basic_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'"');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string"sv
						);
				};

				// skip the '"'
				advance();
				eof_check();
				TOML_ERROR_CHECK({});

				string str;
				bool escaped = false, skipping_whitespace = false;
				while (cp)
				{
					if (escaped)
					{
						escaped = false;

						// handle 'line ending slashes' in multi-line mode
						if constexpr (MULTI_LINE)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								skipping_whitespace = true;
								TOML_ERROR_CHECK({});
								continue;
							}
						}

						// skip the escaped character
						const auto escaped_codepoint = cp->value;
						advance();
						TOML_ERROR_CHECK({});

						switch (escaped_codepoint)
						{
							// 'regular' escape codes
							case U'b': str += TOML_STRING_PREFIX('\b'); break;
							case U'f': str += TOML_STRING_PREFIX('\f'); break;
							case U'n': str += TOML_STRING_PREFIX('\n'); break;
							case U'r': str += TOML_STRING_PREFIX('\r'); break;

							case U's':
							{
								if constexpr (!TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/622
								{
									abort_with_error("Escape sequence '\\s' (Space, U+0020) is not supported "
										"in TOML 0.5.0 and earlier."sv
									);
									break;
								}
								else
								{
									str += TOML_STRING_PREFIX(' ');
									break;
								}
							}

							case U't': str += TOML_STRING_PREFIX('\t'); break;
							case U'"': str += TOML_STRING_PREFIX('"'); break;
							case U'\\': str += TOML_STRING_PREFIX('\\'); break;

							// unicode scalar sequences
							case U'u': [[fallthrough]];
							case U'U':
							{
								uint32_t place_value = escaped_codepoint == U'U' ? 0x10000000u : 0x1000u;
								uint32_t sequence_value{};
								while (place_value)
								{
									eof_check();

									if (!is_hex_digit(*cp))
										abort_with_error(
											"Encountered unexpected character while parsing"sv,
											(MULTI_LINE ? " multi-line"sv : ""sv),
											" string; expected hex digit, saw '\\"sv, *cp, "'"sv
										);
									sequence_value += place_value * (
										*cp >= U'A'
											? 10u + static_cast<uint32_t>(*cp - (*cp >= U'a' ? U'a' : U'A'))
											: static_cast<uint32_t>(*cp - U'0')
										);
									place_value /= 16u;
									advance();
									TOML_ERROR_CHECK({});
								}

								if ((sequence_value > 0xD7FFu && sequence_value < 0xE000u) || sequence_value > 0x10FFFFu)
									abort_with_error("Unknown Unicode scalar sequence"sv);

								if (sequence_value <= 0x7Fu) //ascii
									str += static_cast<string_char>(sequence_value & 0x7Fu);
								else if (sequence_value <= 0x7FFu)
								{
									str += static_cast<string_char>(0xC0u | ((sequence_value >> 6) & 0x1Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}
								else if (sequence_value <= 0xFFFFu)
								{
									str += static_cast<string_char>(0xE0u | ((sequence_value >> 12) & 0x0Fu));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 6) & 0x1Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}
								else
								{
									str += static_cast<string_char>(0xF0u | ((sequence_value >> 18) & 0x07u));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 12) & 0x3Fu));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 6) & 0x3Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}

								break;
							}

							// ???
							default:
								abort_with_error(
									"Encountered unexpected character while parsing"sv,
									(MULTI_LINE ? " multi-line"sv : ""sv),
									" string; unknown escape sequence '\\"sv, *cp, "'"sv
								);
						}
					}
					else TOML_LIKELY
					{
						// handle closing delimiters
						if (*cp == U'"')
						{
							if constexpr (MULTI_LINE)
							{
								advance();
								eof_check();
								TOML_ERROR_CHECK({});
								const auto second = cp->value;

								advance();
								eof_check();
								TOML_ERROR_CHECK({});
								const auto third = cp->value;

								if (second == U'"' && third == U'"')
								{
									advance(); // skip the third closing delimiter
									TOML_ERROR_CHECK({});

									//multi-line basic strings are allowed one additional terminating '"'
									//so that things like this work: """this is a "quote""""
									if (cp && *cp == U'"')
									{
										str += TOML_STRING_PREFIX('"');
										advance(); // skip the final closing delimiter
										TOML_ERROR_CHECK({});
									}

									return str;
								}
								else
								{
									str += TOML_STRING_PREFIX('"');
									go_back(1_sz);
									skipping_whitespace = false;
									continue;
								}
							}
							else
							{
								advance(); // skip the closing delimiter
								TOML_ERROR_CHECK({});
								return str;
							}
						}

						// handle escapes
						if (*cp == U'\\')
						{
							advance(); // skip the '\'
							TOML_ERROR_CHECK({});
							skipping_whitespace = false;
							escaped = true;
							continue;
						}

						// handle line endings in multi-line mode
						if constexpr (MULTI_LINE)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								TOML_ERROR_CHECK({});
								if (!str.empty() && !skipping_whitespace)
									str += TOML_STRING_PREFIX('\n');
								continue;
							}
						}

						// handle illegal characters
						if (*cp <= U'\u0008'
							|| (*cp >= U'\u000A' && *cp <= U'\u001F')
							|| *cp == U'\u007F')
							abort_with_error(
								"Encountered unexpected character while parsing"sv,
								(MULTI_LINE ? " multi-line"sv : ""sv),
								" string; control characters must be escaped with back-slashes."sv
							);

						if constexpr (MULTI_LINE)
						{
							if (!skipping_whitespace || !is_whitespace(*cp))
							{
								skipping_whitespace = false;
								str.append(cp->as_view());
							}
						}
						else
							str.append(cp->as_view());

						advance();
						TOML_ERROR_CHECK({});
					}
				}

				abort_with_error("Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string"sv);
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			template <bool MULTI_LINE>
			[[nodiscard]]
			string parse_literal_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'\'');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing"sv,
							(MULTI_LINE ? " multi-line"sv : ""sv),
							" literal string"sv
						);
				};

				// skip the delimiter
				advance();
				eof_check();

				string str;
				while (cp)
				{
					TOML_ERROR_CHECK({});

					// handle closing delimiters
					if (*cp == U'\'')
					{
						if constexpr (MULTI_LINE)
						{
							advance();
							eof_check();
							TOML_ERROR_CHECK({});
							const auto second = cp->value;

							advance();
							eof_check();
							TOML_ERROR_CHECK({});
							const auto third = cp->value;

							if (second == U'\'' && third == U'\'')
							{
								advance(); // skip the third closing delimiter
								TOML_ERROR_CHECK({});
								return str;
							}
							else
							{
								str += TOML_STRING_PREFIX('\'');
								go_back(1_sz);
								continue;
							}
						}
						else
						{
							advance(); // skip the closing delimiter
							TOML_ERROR_CHECK({});
							return str;
						}
					}

					// handle line endings in multi-line mode
					if constexpr (MULTI_LINE)
					{
						if (is_line_break(*cp))
						{
							consume_line_break();
							if (!str.empty())
								str += TOML_STRING_PREFIX('\n');
							continue;
						}
					}

					// handle illegal characters
					if (*cp <= U'\u0008'
						|| (*cp >= U'\u000A' && *cp <= U'\u001F')
						|| *cp == U'\u007F')
						abort_with_error(
							"Encountered unexpected character while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv),
							" literal string; control characters may not appear in literal strings"sv
						);

					str.append(cp->as_view());
					advance();
					TOML_ERROR_CHECK({});
				}

				abort_with_error("Encountered EOF while parsing"sv,
					(MULTI_LINE ? " multi-line"sv : ""sv),
					" literal string"sv
				);
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			string parse_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_string_delimiter(*cp));

				// get the first three characters to determine the string type
				const auto first = cp->value;
				advance();
				TOML_ERROR_CHECK({});
				if (!cp)
					abort_with_error("Encountered EOF while parsing string"sv);
				const auto second = cp->value;
				advance();
				TOML_ERROR_CHECK({});
				const auto third = cp ? cp->value : U'\0';

				// if we were eof at the third character then first and second need to be
				// the same string character (otherwise it's an unterminated string)
				if (!cp)
				{
					if (second == first)
						return string{};
					abort_with_error("Encountered EOF while parsing string"sv);
				}

				// if the first three characters are all the same string delimiter then
				// it's a multi-line string.
				if (first == second && first == third)
				{
					return first == U'\''
						? parse_literal_string<true>()
						: parse_basic_string<true>();
				}

				// otherwise it's just a regular string.
				else
				{
					// step back two characters so that the current
					// character is the string delimiter
					go_back(2_sz);

					return first == U'\''
						? parse_literal_string<false>()
						: parse_basic_string<false>();
				}
			}

			[[nodiscard]]
			string parse_bare_key_segment() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_bare_key_start_character(*cp));

				string segment;

				while (cp)
				{
					if (!is_bare_key_character(*cp))
						break;

					segment.append(cp->as_view());
					advance();
					TOML_ERROR_CHECK({});
				}

				return segment;
			}

			[[nodiscard]]
			bool parse_bool() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (*cp == U't' || *cp == U'f'));

				auto result = *cp == U't';
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				{
					if (!cp)
						abort_with_error("Encountered EOF while parsing boolean"sv);
					else
						abort_with_error(
							"Encountered unexpected character while parsing boolean; "
							"expected 'true' or 'false', saw '"sv, *cp, '\''
						);
				}
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing boolean; expected value-terminator, saw '"sv,
						*cp, '\''
					);

				TOML_ERROR_CHECK({});
				return result;
			}

			[[nodiscard]]
			double parse_inf_or_nan() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (*cp == U'i' || *cp == U'n' || *cp == U'+' || *cp == U'-'));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing floating-point"sv);
				};

				const int sign = *cp == U'-' ? -1 : 1;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
				}

				const bool inf = *cp == U'i';
				if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing floating-point; expected '"sv,
						inf ? "inf"sv : "nan"sv, "', saw '"sv, *cp, '\''
					);
				}
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing floating-point; "
						"expected value-terminator, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});

				return inf
					? sign * std::numeric_limits<double>::infinity()
					: std::numeric_limits<double>::quiet_NaN();
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_SWITCH_WARNING
			TOML_DISABLE_FIELD_INIT_WARNING
			TOML_DISABLE_VAR_INIT_WARNING

			[[nodiscard]]
			double parse_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (*cp == U'+' || *cp == U'-' || is_decimal_digit(*cp)));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing floating-point"sv);
				};

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
				}

				// consume value chars
				char chars[64];
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent_sign = false, seen_exponent = false;
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"underscores may only follow digits"sv
							);
						prev = cp;
						advance();
						TOML_ERROR_CHECK({});
						continue;
					}

					if (*cp == U'.')
					{
						if (seen_decimal)
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"decimal points may appear only once"sv
							);
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"decimal points may not appear after exponents"sv
							);
						seen_decimal = true;
					}
					else if (*cp == U'e' || *cp == U'E')
					{
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"exponents may appear only once"sv
							);
						seen_exponent = true;
					}
					else if (*cp == U'+' || *cp == U'-')
					{
						if (!seen_exponent || !(*prev == U'e' || *prev == U'E'))
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"exponent signs must immediately follow 'e'"sv
							);
						if (seen_exponent_sign)
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"exponents signs may appear only once"sv
							);
						seen_exponent_sign = true;
					}
					else if (!is_decimal_digit(*cp))
						abort_with_error("Encountered unexpected character while parsing floating-point; "
							"expected decimal digit, saw '"sv, *cp, '\''
						);

					if (length == sizeof(chars))
						abort_with_error(
							"Floating-point value out-of-range; exceeds maximum length of "sv,
							sizeof(chars), " characters"sv
						);

					chars[length++] = static_cast<char>(cp->bytes[0]);
					prev = cp;
					advance();

					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing floating-point; "
						"expected decimal digit, saw '"sv, *cp, '\''
					);
				}

				TOML_ERROR_CHECK({});

				// convert to double
				double result;
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::stringstream ss;
					ss.write(chars, static_cast<std::streamsize>(length));
					if ((ss >> result))
						return result * sign;
					else
						abort_with_error(
							"Error parsing floating-point; "
							"the character sequence could not be interpreted as a floating-point value"sv
						);
				}
				#else
				{
					auto parse_result = std::from_chars(chars, chars + length, result);
					if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
					{
						eof_check();
						parse_result.ec = std::errc::invalid_argument;
					}
					switch (parse_result.ec)
					{
						case std::errc{}: //ok
							return result * sign;

						case std::errc::invalid_argument:
							abort_with_error(
								"Error parsing floating-point; "
								"the character sequence could not be interpreted as a floating-point value"sv
							);

						case std::errc::result_out_of_range:
							abort_with_error(
								"Error parsing floating-point; the character sequence contained a value "
								"not representable by a 64-bit floating-point"sv
							);

						default: //??
							abort_with_error(
								"Error parsing floating-point; an unspecified error occurred while trying to "
								"interpret the character sequence as a floating-point value"sv
							);
					}
				}
				#endif
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			#if !TOML_USE_STREAMS_FOR_FLOATS && TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/562

			[[nodiscard]]
			double parse_hex_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'0');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing hexadecimal floating-point"sv);
				};

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal floating-point; "
						"expected '0', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				// 'x' or 'X'
				if (*cp != U'x' && *cp != U'X')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal floating-point; "
						"expected 'x' or 'X', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[23]; //23 = strlen("1.0123456789ABCDEFp+999")
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent_sign = false, seen_exponent = false;
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_hex_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"underscores may only follow digits"sv
							);
						prev = cp;
						advance();
						TOML_ERROR_CHECK({});
						continue;
					}

					if (*cp == U'.')
					{
						if (seen_decimal)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"decimal points may appear only once"sv
							);
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing floating-point; "
								"decimal points may not appear after exponents"sv
							);
						seen_decimal = true;
					}
					else if (*cp == U'p' || *cp == U'P')
					{
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"exponents may appear only once"sv
							);
						if (!seen_decimal)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"exponents may not appear before decimal points"sv
							);
						seen_exponent = true;
					}
					else if (*cp == U'+' || *cp == U'-')
					{
						if (!seen_exponent || !(*prev == U'p' || *prev == U'P'))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"exponent signs must immediately follow 'p'"sv
							);
						if (seen_exponent_sign)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal floating-point; "
								"exponents signs may appear only once"sv
							);
						seen_exponent_sign = true;
					}
					else
					{
						if (!seen_exponent && !is_hex_digit(*cp))
							abort_with_error("Encountered unexpected character while parsing hexadecimal "
							"floating-point; expected hexadecimal digit, saw '"sv, *cp, '\''
							);
						else if (seen_exponent && !is_decimal_digit(*cp))
							abort_with_error("Encountered unexpected character while parsing hexadecimal "
							"floating-point exponent; expected decimal digit, saw '"sv, *cp, '\''
							);
					}

					if (length == sizeof(chars))
						abort_with_error(
							"Hexadecimal floating-point value out-of-range; "
							"exceeds maximum length of "sv, sizeof(chars), " characters"sv
						);
					chars[length++] = static_cast<char>(cp->bytes[0]);
					prev = cp;
					advance();

					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					if (seen_exponent)
						abort_with_error(
							"Encountered unexpected character while parsing hexadecimal floating-point exponent; "
							"expected decimal digit, saw '"sv, *cp, '\''
						);
					else
						abort_with_error(
							"Encountered unexpected character while parsing hexadecimal floating-point; "
							"expected hexadecimal digit, saw '"sv, *cp, '\''
						);
				}

				TOML_ERROR_CHECK({});

				// convert to double
				double result;
				auto parse_result = std::from_chars(chars, chars + length, result, std::chars_format::hex);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing hexadecimal floating-point; the character sequence could not "
							"be interpreted as a hexadecimal floating-point value"sv
						);

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing hexadecimal floating-point; the character sequence contained a "
							"value not representable by a 64-bit floating-point"sv
						);

					default: //??
						abort_with_error(
							"Error parsing hexadecimal floating-point; an unspecified error occurred while trying "
							"to interpret the character sequence as a hexadecimal floating-point value"sv
						);
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			#endif //!TOML_USE_STREAMS_FOR_FLOATS && TOML_LANG_HIGHER_THAN(0, 5, 0)

			[[nodiscard]]
			int64_t parse_binary_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'0');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing binary integer"sv);
				};

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing binary integer; expected '0', saw '"sv,
						*cp, '\''
					);
				advance();
				eof_check();

				// 'b' or 'B'
				if (*cp != U'b' && *cp != U'B')
					abort_with_error(
						"Encountered unexpected character while parsing binary integer; expected 'b' or 'B', saw '"sv,
						*cp, '\''
					);
				advance();
				eof_check();

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[64];
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_binary_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing binary integer; "
								"underscores may only follow digits"sv
							);
					}
					else
					{
						if (!is_binary_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing binary integer; "
								"expected binary digit, saw '"sv, *cp, '\''
							);
						if (length == sizeof(chars))
							abort_with_error(
								"Binary integer value out-of-range; exceeds maximum length of "sv,
								sizeof(chars), " characters"sv
							);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}

					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing binary integer; "
						"expected binary digit, saw '"sv, *cp, '\''
					);
				}

				// single digits can be converted directly
				if (length == 1_sz)
					return chars[0] == '1' ? 1ull : 0ull;

				TOML_ERROR_CHECK({});

				// otherwise invoke charconv
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 2);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing binary integer; the character sequence could not be "
							"interpreted as a binary integer value"sv
						);
						break;

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing binary integer; the character sequence contained a value not representable "
							"by a signed 64-bit integer"sv
						);
						break;

					default: //??
						abort_with_error(
							"Error parsing binary integer; an unspecified error occurred while trying to interpret "
							"the character sequence as a binary integer value"sv
						);
						break;
				}

				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_octal_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'0');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing octal integer"sv);
				};

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing octal integer; expected '0', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				// 'o'
				if (*cp != U'o')
					abort_with_error(
						"Encountered unexpected character while parsing octal integer; expected 'o', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[21]; //21 == strlen("777777777777777777777") (max 64-bit uint)
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_octal_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing octal integer; "
								"underscores may only follow digits"sv
							);
					}
					else
					{
						if (!is_octal_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing octal integer; "
								"expected octal digit, saw '"sv, *cp, '\''
							);
						if (length == sizeof(chars))
							abort_with_error(
								"Octal integer value out-of-range; exceeds maximum length of "sv,
								sizeof(chars), " characters"sv
							);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}

					prev = cp;
					advance();

					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing octal integer; "
						"expected octal digit, saw '"sv, *cp, '\''
					);
				}

				// single digits can be converted directly
				if (length == 1_sz)
					return static_cast<int64_t>(chars[0] - '0');

				TOML_ERROR_CHECK({});

				// otherwise invoke charconv
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 8);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing octal integer; the character sequence could not be interpreted "
							"as a octal integer value"sv
						);
						break;

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing octal integer; the character sequence contained a value not representable "
							"by a signed 64-bit integer"sv
						);
						break;

					default: //??
						abort_with_error(
							"Error parsing octal integer; an unspecified error occurred while trying to interpret the "
							"character sequence as a octal integer value"sv
						);
						break;
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_decimal_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (*cp == U'+' || *cp == U'-' || is_decimal_digit(*cp)));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing integer"sv);
				};

				// sign
				const int64_t sign = *cp == U'-' ? -1ll : 1ll;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
				}

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[19]; //19 == strlen("9223372036854775807") (max 64-bit uint)
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing integer; "
								"underscores may only follow digits"sv
							);
					}
					else
					{
						if (!is_decimal_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing integer; "
								"expected decimal digit, saw '"sv, *cp, '\''
							);
						if (length == sizeof(chars))
							abort_with_error(
								"Integer value out-of-range; exceeds maximum length of "sv,
								sizeof(chars), " characters"sv
							);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}

					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing integer; expected decimal digit, saw '"sv,
						*cp, '\''
					);
				}

				// check for leading zeroes etc
				if (chars[0] == '0')
					abort_with_error(
						"Encountered unexpected character while parsing integer; "
						"leading zeroes are not allowed"sv
					);

				TOML_ERROR_CHECK({});

				// convert to int
				uint64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result);
				if (parse_result.ec == std::errc{})
				{
					if (parse_result.ptr < (chars + length))
					{
						eof_check();
						parse_result.ec = std::errc::invalid_argument;
					}
					else if ((sign < 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1ull)
						|| (sign > 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())))
						parse_result.ec = std::errc::result_out_of_range;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return static_cast<int64_t>(result) * sign;

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing integer; "
							"the character sequence could not be interpreted as an integer value"sv
						);
						break;

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing integer; the character sequence contained a value not representable "
							"by a signed 64-bit integer"sv
						);
						break;

					default: //??
						abort_with_error(
							"Error parsing integer; an unspecified error occurred while trying to interpret the "
							"character sequence as an integer value"sv
						);
						break;
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_hex_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'0');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing hexadecimal integer"sv);
				};

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal integer; expected '0', saw '"sv,
						*cp, '\''
					);
				advance();
				eof_check();

				// 'x' or 'X'
				if (*cp != U'x' && *cp != U'X')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal integer; "
						"expected 'x' or 'X', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[16]; //16 == strlen("FFFFFFFFFFFFFFFF") (max 64-bit uint)
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_hex_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal integer; "
								"underscores may only follow digits"
							);
					}
					else
					{
						if (!is_hex_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal integer; "
								"expected hexadecimal digit, saw '"sv, *cp, '\''
							);
						if (length == sizeof(chars))
							abort_with_error(
								"Hexadecimal integer value out-of-range; exceeds maximum length of "sv,
								sizeof(chars), " characters"sv
							);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}

					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal integer; "
						"expected hexadecimal digit, saw '"sv, *cp, '\''
					);
				}

				// single digits can be converted directly
				if (length == 1_sz)
					return chars[0] >= 'A'
						? static_cast<int64_t>(10 + (chars[0] - (chars[0] >= 'a' ? 'a' : 'A')))
						: static_cast<int64_t>(chars[0] - '0');

				TOML_ERROR_CHECK({});

				// otherwise invoke charconv
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 16);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing hexadecimal integer; the character sequence '"sv,
							std::string_view{ chars, length },
							"' could not be interpreted as a hexadecimal integer value"sv
						);
						break;

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing hexadecimal integer; the character sequence '"sv,
							std::string_view{ chars, length },
							"' contained a value not representable by a signed 64-bit integer"sv
						);
						break;

					default: //??
						abort_with_error(
							"Error parsing hexadecimal integer; "
							"an unspecified error occurred while trying to interpret the character sequence '"sv,
							std::string_view{ chars, length },
							"' as a hexadecimal integer value"sv
						);
						break;
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			date parse_date(bool time_may_follow = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_decimal_digit(*cp));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing date"sv);
				};

				// "YYYY"
				uint32_t year_digits[4];
				if (!consume_digit_sequence(year_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing date; "
						"expected 4-digit year, saw '"sv, *cp, '\''
					);
				}
				const auto year = year_digits[3]
					+ year_digits[2] * 10u
					+ year_digits[1] * 100u
					+ year_digits[0] * 1000u;
				const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));

				// '-'
				eof_check();
				if (*cp != U'-')
					abort_with_error(
						"Encountered unexpected character while parsing date; expected '-', saw '"sv,
						*cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "MM"
				uint32_t month_digits[2];
				if (!consume_digit_sequence(month_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing date; expected 2-digit month, saw '"sv,
						*cp, '\''
					);
				}
				const auto month = month_digits[1] + month_digits[0] * 10u;
				if (month == 0u || month > 12u)
					abort_with_error("Month value out-of-range; expected 1-12 (inclusive), saw "sv, month);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;

				// '-'
				eof_check();
				if (*cp != U'-')
					abort_with_error(
						"Encountered unexpected character while parsing date; expected '-', saw '"sv,
						*cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "DD"
				uint32_t day_digits[2];
				if (!consume_digit_sequence(day_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing date; expected 2-digit day, saw '"sv,
						*cp, '\''
					);
				}
				const auto day = day_digits[1] + day_digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					abort_with_error(
						"Day value out-of-range; expected 1-"sv, max_days_in_month,
						" (inclusive), saw "sv, day
					);

				if (!time_may_follow)
				{
					if (cp && !is_value_terminator(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing date; "
							"expected value-terminator, saw '"sv, *cp, '\''
						);
				}

				TOML_ERROR_CHECK({});

				return
				{
					static_cast<uint16_t>(year),
					static_cast<uint8_t>(month),
					static_cast<uint8_t>(day)
				};
			}

			[[nodiscard]]
			time parse_time(bool offset_may_follow = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_decimal_digit(*cp));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing time"sv);
				};

				// "HH"
				uint32_t digits[2];
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing time; expected 2-digit hour, saw '"sv,
						*cp, '\''
					);
				}
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					abort_with_error(
						"Hour value out-of-range; expected 0-23 (inclusive), saw "sv, hour
					);

				// ':'
				eof_check();
				if (*cp != U':')
					abort_with_error(
						"Encountered unexpected character while parsing time; expected ':', saw '"sv, *cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "MM"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing time; expected 2-digit minute, saw '"sv,
						*cp, '\''
					);
				}
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					abort_with_error(
						"Minute value out-of-range; expected 0-59 (inclusive), saw "sv,
						minute
					);

				auto time = ::toml::time{
					static_cast<uint8_t>(hour),
					static_cast<uint8_t>(minute),
				};

				if constexpr (!TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/671
				{
					// ':'
					eof_check();
					if (*cp != U':')
						abort_with_error(
							"Encountered unexpected character while parsing time; expected ':', saw '"sv, *cp, '\''
						);
					advance();
				}
				else
				{
					if (cp
						&& !is_value_terminator(*cp)
						&& *cp != U':'
						&& (!offset_may_follow || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
						abort_with_error(
							"Encountered unexpected character while parsing time; expected ':'"sv,
							(offset_may_follow ? ", offset"sv : ""sv),
							" or value-terminator, saw '"sv, *cp, '\''
						);
					TOML_ERROR_CHECK({});
					if (!cp || *cp != U':')
						return time;

					// ':'
					advance();
				}

				TOML_ERROR_CHECK({});

				// "SS"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing time; expected 2-digit second, saw '"sv,
						*cp, '\''
					);
				}
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					abort_with_error("Second value out-of-range; expected 0-59 (inclusive), saw "sv, second);
				time.second = static_cast<uint8_t>(second);

				//early exit here if the fractional is omitted
				if (cp
					&& !is_value_terminator(*cp)
					&& *cp != U'.'
					&& (!offset_may_follow || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
					abort_with_error(
						"Encountered unexpected character while parsing time; expected fractional"sv,
						(offset_may_follow ? ", offset"sv : ""sv),
						" or value-terminator, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});
				if (!cp || *cp != U'.')
					return time;

				// '.'
				advance();
				eof_check();
				TOML_ERROR_CHECK({});

				// ".FFFFFFFFF"
				static constexpr auto max_fractional_digits = 9_sz;
				uint32_t fractional_digits[max_fractional_digits];
				auto digit_count = consume_variable_length_digit_sequence(fractional_digits);
				if (!digit_count)
					abort_with_error(
						"Encountered unexpected character while parsing time; expected fractional digits, saw '"sv,
						*cp, '\''
					);
				if (digit_count == max_fractional_digits && cp && is_decimal_digit(*cp))
					abort_with_error(
						"Fractional value out-of-range; exceeds maximum precision of "sv, max_fractional_digits
					);

				uint32_t value = 0u;
				uint32_t place = 1u;
				for (auto i = digit_count; i --> 0_sz;)
				{
					value += fractional_digits[i] * place;
					place *= 10u;
				}
				for (auto i = digit_count; i < max_fractional_digits; i++) //implicit zeros
					value *= 10u;
				time.nanosecond = value;

				return time;
			}

			[[nodiscard]]
			date_time parse_date_time() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_decimal_digit(*cp));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing date-time"sv);
				};

				// "YYYY-MM-DD"
				auto date = parse_date(true);
				TOML_ERROR_CHECK({});

				// ' ' or 'T'
				eof_check();
				if (*cp != U' ' && *cp != U'T' && *cp != U't')
					abort_with_error(
						"Encountered unexpected character while parsing date-time; expected space or 'T', saw '"sv,
						*cp, '\''
					);
				advance();

				// "HH:MM:SS.fractional"

				auto time = parse_time(true);
				TOML_ERROR_CHECK({});

				// offset
				std::optional<time_offset> offset;
				if (cp)
				{
					// zero offset ("Z")
					if (*cp == U'Z' || *cp == U'z')
					{
						offset.emplace(time_offset{});
						advance();
					}

					// explicit offset ("+/-HH:MM")
					else if (*cp == U'+' || *cp == U'-')
					{
						// sign
						int sign = *cp == U'-' ? -1 : 1;
						advance();
						eof_check();

						// "HH"
						uint32_t digits[2];
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing date-time offset; "
								"expected 2-digit hour, saw '"sv, *cp, '\''
							);
						}
						const auto hour = digits[1] + digits[0] * 10u;
						if (hour > 23u)
							abort_with_error("Hour value out-of-range; expected 0-23 (inclusive), saw "sv, hour);

						// ':'
						eof_check();
						if (*cp != U':')
							abort_with_error(
								"Encountered unexpected character while parsing date-time offset; "
								"expected ':', saw '"sv, *cp, '\''
							);
						advance();

						// "MM"
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing date-time offset; "
								"expected 2-digit minute, saw '"sv, *cp, '\''
							);
						}
						const auto minute = digits[1] + digits[0] * 10u;
						if (minute > 59u)
							abort_with_error("Minute value out-of-range; expected 0-59 (inclusive), saw "sv, minute);

						offset.emplace(time_offset{
							static_cast<int8_t>(static_cast<int>(hour) * sign),
							static_cast<int8_t>(static_cast<int>(minute)* sign)
						});
					}
				}

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing date-time; expected value-terminator, saw '"sv,
						*cp, '\''
					);

				TOML_ERROR_CHECK({});
				return {
					date,
					time,
					offset
				};
			}

			// TOML_DISABLE_SWITCH_WARNING
			// TOML_DISABLE_VAR_INIT_WARNING
			// TOML_DISABLE_FIELD_INIT_WARNING
			TOML_POP_WARNINGS

			[[nodiscard]]
			inline std::unique_ptr<toml::array> parse_array() TOML_MAY_THROW;

			[[nodiscard]]
			inline std::unique_ptr<toml::table> parse_inline_table() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<node> parse_value() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && !is_value_terminator(*cp));

				const auto begin_pos = cp->position;
				std::unique_ptr<node> val;

				do
				{
					// detect the value type and parse accordingly,
					// starting with value types that can be detected
					// unambiguously from just one character.

					// strings
					if (is_string_delimiter(*cp))
						val = std::make_unique<value<string>>(parse_string());

					// bools
					else if (*cp == U't' || *cp == U'f')
						val = std::make_unique<value<bool>>(parse_bool());

					// arrays
					else if (*cp == U'[')
					{
						val = parse_array();
						if constexpr (!TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/665
						{
							// arrays must be homogeneous in toml 0.5.0 and earlier
							if (!val->reinterpret_as<array>()->is_homogeneous())
								TOML_ERROR(
									"Arrays cannot contain values of different types in TOML 0.5.0 and earlier.",
									begin_pos
								);
						}
					}

					// inline tables
					else if (*cp == U'{')
						val = parse_inline_table();

					// inf or nan
					else if (*cp == U'i' || *cp == U'n')
						val = std::make_unique<value<double>>(parse_inf_or_nan());

					// underscores at the beginning
					else if (*cp == U'_')
						break;

					TOML_ERROR_CHECK({});
					if (val)
						break;

					// value types from here down require more than one character
					// to unambiguously identify, so scan ahead a bit.
					enum value_string_traits : int
					{
						has_nothing = 0,
						has_digits = 1,
						has_e = 2,
						has_t = 4,
						has_z = 8,
						has_colon = 16,
						has_plus = 32,
						has_minus = 64,
						has_dot = 128,
						has_space = 256
					};
					int traits = has_nothing;
					char32_t chars[utf8_buffered_reader::max_history_length];
					size_t char_count = {}, advance_count = {};
					bool eof_while_scanning = false;
					const auto scan = [&]() TOML_MAY_THROW
					{
						while (advance_count < utf8_buffered_reader::max_history_length)
						{
							if (!cp || is_value_terminator(*cp))
							{
								eof_while_scanning = !cp;
								break;
							}

							if (*cp != U'_')
							{
								chars[char_count++] = *cp;
								if (is_decimal_digit(*cp))
									traits |= has_digits;
								else if (*cp == U'E' || *cp == U'e')
									traits |= has_e;
								else if (*cp == U'T' || *cp == U't')
									traits |= has_t;
								else if (*cp == U'Z' || *cp == U'z')
									traits |= has_z;
								else if (*cp == U':')
									traits |= has_colon;
								else if (*cp == U'+')
									traits |= has_plus;
								else if (*cp == U'-')
									traits |= has_minus;
								else if (*cp == U'.')
									traits |= has_dot;
							}
							advance();
							advance_count++;
							TOML_ERROR_CHECK();
						}
					};
					scan();
					TOML_ERROR_CHECK({});

					//force further scanning if this could have been a date-time with a space instead of a T
					if (char_count == 10_sz
						&& chars[4] == U'-'
						&& chars[7] == U'-'
						&& is_decimal_digit(chars[9])
						&& cp
						&& *cp == U' ')
					{
						chars[char_count++] = *cp;
						const auto pre_advance_count = advance_count;
						traits |= has_space;

						advance();
						advance_count++;

						scan();
						TOML_ERROR_CHECK({});

						if (char_count == 11_sz) //backpedal if we found nothing useful after the space
						{
							const auto delta = advance_count - pre_advance_count;
							go_back(delta);
							advance_count -= delta;
						}
					}

					//set the reader back to where we started
					go_back(advance_count);
					if (char_count < utf8_buffered_reader::max_history_length - 1_sz)
						chars[char_count] = U'\0';

					// if after scanning ahead we still only have one value character,
					// the only valid value type is an integer.
					if (char_count == 1_sz)
					{
						if ((traits & has_digits))
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[0] - U'0'));
							advance(); //skip the digit
							break;
						}

						//anything else would be ambiguous.
						abort_with_error(
							eof_while_scanning
								? "Encountered EOF while parsing value"sv
								: "Could not determine value type"sv
						);
					}

					// now things that can be identified from two characters
					TOML_ERROR_CHECK({});
					TOML_ASSERT(char_count >= 2_sz);

					// numbers that begin with a sign
					const auto begins_with_sign = chars[0] == U'+' || chars[0] == U'-';
					const auto begins_with_digit = is_decimal_digit(chars[0]);
					if (begins_with_sign)
					{
						if (char_count == 2_sz && is_decimal_digit(chars[1]))
						{
							val = std::make_unique<value<int64_t>>(
								static_cast<int64_t>(chars[1] - U'0')
								* (chars[1] == U'-' ? -1LL : 1LL)
							);
							advance(); //skip the sign
							advance(); //skip the digit
						}

						else if (chars[1] == U'i' || chars[1] == U'n')
							val = std::make_unique<value<double>>(parse_inf_or_nan());
						else if (is_decimal_digit(chars[1]) && (chars[2] == U'.' || chars[2] == U'e' || chars[2] == U'E'))
							val = std::make_unique<value<double>>(parse_float());
					}

					// numbers that begin with 0
					else if (chars[0] == U'0')
					{
						switch (chars[1])
						{
							case U'E': [[fallthrough]];
							case U'e': [[fallthrough]];
							case U'.': val = std::make_unique<value<double>>(parse_float()); break;
							case U'b': val = std::make_unique<value<int64_t>>(parse_binary_integer()); break;
							case U'o': val = std::make_unique<value<int64_t>>(parse_octal_integer()); break;
							case U'X': [[fallthrough]];
							case U'x':
							{
								for (size_t i = char_count; i-- > 2_sz;)
								{
									if (chars[i] == U'p' || chars[i] == U'P')
									{
										#if !TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/562
											TOML_ERROR(
												"Hexadecimal floating-point values are not supported "
												"in TOML 0.5.0 and earlier.",
												begin_pos
											);
										#elif TOML_USE_STREAMS_FOR_FLOATS
											TOML_ERROR(
												"Hexadecimal floating-point values are not supported when streams "
												"are used to interpret floats (TOML_USE_STREAMS_FOR_FLOATS = 1).",
												begin_pos
											);
										#else
											val = std::make_unique<value<double>>(parse_hex_float());
											break;
										#endif
									}
								}
								TOML_ERROR_CHECK({});
								if (val)
									break;

								val = std::make_unique<value<int64_t>>(parse_hex_integer());
								break;
							}
						}
					}

					//floats
					else if (begins_with_digit && (chars[1] == U'.' || chars[1] == U'e' || chars[1] == U'E'))
						val = std::make_unique<value<double>>(parse_float());

					TOML_ERROR_CHECK({});
					if (val || !(begins_with_sign || begins_with_digit))
						break;

					// dates, times, date-times
					if (begins_with_digit)
					{
						//1987-03-16 10:20:00
						if ((traits & has_space)
							|| (traits & has_t)
							|| (traits & has_z)
							|| (traits & (has_minus | has_colon)) == (has_minus | has_colon))
							val = std::make_unique<value<date_time>>(parse_date_time());

						//1987-03-16
						//1987-03-16 10:20:00
						//1987-03-16T10:20:00 ... etc.
						if (!val && char_count >= 8_sz && (chars[4] == U'-' || chars[7] == U'-'))
						{
							if (char_count > 10_sz)
								val = std::make_unique<value<date_time>>(parse_date_time());
							else
								val = std::make_unique<value<date>>(parse_date());
						}

						//10:20:00
						//10:20:00.87234
						if (!val && char_count >= 6_sz && (traits & has_colon))
							val = std::make_unique<value<time>>(parse_time());
					}

					TOML_ERROR_CHECK({});
					if (val
						|| (traits & (has_space | has_t | has_z | has_colon))) //
						break;

					//only digits, dots, pluses, minuses and 'E' remains.
					//can only be floats and ints from here.
					if ((traits & (has_e | has_dot)))
						val = std::make_unique<value<double>>(parse_float());
					else
						val = std::make_unique<value<int64_t>>(parse_decimal_integer());
				}
				while (false);

				if (!val)
				{
					abort_with_error("Could not determine value type"sv);
					TOML_ERROR_CHECK({});
				}

				val->rgn = { begin_pos, current_position_or_assumed_next(), reader.source_path() };
				return val;
			}

			struct key final
			{
				std::vector<string> segments;
			};

			[[nodiscard]]
			key parse_key() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (is_bare_key_start_character(*cp) || is_string_delimiter(*cp)));

				key key;

				while (true)
				{
					if (!cp)
						abort_with_error("Encountered EOF while parsing key"sv);

					// bare_key_segment
					#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
					if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing key; expected bare key starting character "
							"or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					#endif
					if (is_bare_key_start_character(*cp))
						key.segments.push_back(parse_bare_key_segment());

					// "quoted key segment"
					else if (is_string_delimiter(*cp))
						key.segments.push_back(parse_string());

					// ???
					else
						abort_with_error(
							"Encountered unexpected character while parsing key; expected bare key "
							"starting character or string delimiter, saw '"sv, *cp, '\''
						);

					consume_leading_whitespace();

					// eof or no more key to come
					if (!cp)
						break;
					if (*cp != U'.')
					{
						if (recording)
							stop_recording(1_sz);
						break;
					}

					// was a dotted key, so go around again to consume the next segment
					TOML_ASSERT(*cp == U'.');
					advance();
					consume_leading_whitespace();
				}
				TOML_ERROR_CHECK({});
				return key;
			}

			struct key_value_pair final
			{
				parser::key key;
				std::unique_ptr<node> value;
			};

			[[nodiscard]]
			key_value_pair parse_key_value_pair() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing key-value pair"sv);
				};

				// get the key
				TOML_ASSERT(cp && (is_string_delimiter(cp->value) || is_bare_key_start_character(cp->value)));
				start_recording();
				auto key = parse_key(); //parse_key() calls stop_recording()

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				eof_check();

				// consume the '='
				if (*cp != U'=')
					abort_with_error(
						"Encountered unexpected character while parsing key-value pair; "
						"expected '=', saw '"sv, *cp, '\''
					);
				advance();

				// skip past any whitespace that followed the '='
				consume_leading_whitespace();
				eof_check();

				// get the value
				if (is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing key-value pair; "
						"expected value, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});
				return { std::move(key), parse_value() };
			}

			[[nodiscard]]
			table* parse_table_header() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'[');

				const auto header_begin_pos = cp->position;
				source_position header_end_pos;
				key key;
				bool is_array = false;

				//parse header
				{
					const auto eof_check = [this]() TOML_MAY_THROW
					{
						TOML_ERROR_CHECK();
						if (!cp)
							abort_with_error("Encountered EOF while parsing table header"sv);
					};

					// skip first '['
					advance();
					eof_check();

					// skip second '[' (if present)
					if (*cp == U'[')
					{
						advance();
						eof_check();
						is_array = true;
					}

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					eof_check();

					// get the actual key
					start_recording();
					key = parse_key(); //parse_key() calls stop_recording()

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					eof_check();

					// consume the first closing ']'
					if (*cp != U']')
						abort_with_error(
							"Encountered unexpected character while parsing table"sv,
							(is_array ? " array"sv : ""sv), " header; expected ']', saw '"sv, *cp, '\''
						);
					advance();

					// consume the second closing ']'
					if (is_array)
					{
						eof_check();

						if (*cp != U']')
							abort_with_error(
								"Encountered unexpected character while parsing table array header; "
								"expected ']', saw '"sv, *cp, '\''
							);
						advance();
					}
					header_end_pos = current_position_or_assumed_next();

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (cp)
					{
						if (!consume_comment() && !consume_line_break())
							abort_with_error(
								"Encountered unexpected character after table"sv, (is_array ? " array"sv : ""sv),
								" header; expected a comment or whitespace, saw '"sv, *cp, '\''
							);
					}
				}
				TOML_ERROR_CHECK({});
				TOML_ASSERT(!key.segments.empty());

				// check if each parent is a table/table array, or can be created implicitly as a table.
				auto parent = &root;
				for (size_t i = 0; i < key.segments.size() - 1_sz; i++)
				{
					auto child = parent->get(key.segments[i]);
					if (!child)
					{
						child = parent->values.emplace(
							key.segments[i],
							std::make_unique<table>()
						).first->second.get();
						implicit_tables.push_back(child->reinterpret_as<table>());
						child->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						parent = child->reinterpret_as<table>();
					}
					else if (child->is_table())
					{
						parent = child->reinterpret_as<table>();
					}
					else if (child->is_array() && find(table_arrays, child->reinterpret_as<array>()))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!child->reinterpret_as<array>()->values.empty());
						TOML_ASSERT(child->reinterpret_as<array>()->values.back()->is_table());
						parent = child->reinterpret_as<array>()->values.back()->reinterpret_as<table>();
					}
					else
					{
						if (!is_array && child->type() == node_type::table)
							abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
						else
							abort_with_error(
								"Attempt to redefine existing "sv, child->type(),
								" '"sv, recording_buffer,
								"' as "sv, is_array ? "array-of-tables"sv : "table"sv
							);
					}
				}
				TOML_ERROR_CHECK({});

				// check the last parent table for a node matching the last key.
				// if there was no matching node, then sweet;
				// we can freely instantiate a new table/table array.
				auto matching_node = parent->get(key.segments.back());
				if (!matching_node)
				{
					// if it's an array we need to make the array and it's first table element,
					// set the starting regions, and return the table element
					if (is_array)
					{
						auto tab_arr = parent->values.emplace(key.segments.back(),std::make_unique<array>())
							.first->second->reinterpret_as<array>();
						table_arrays.push_back(tab_arr);
						tab_arr->rgn = { header_begin_pos, header_end_pos, reader.source_path() };

						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab_arr->values.back()->reinterpret_as<table>();
					}

					//otherwise we're just making a table
					else
					{
						auto tab = parent->values.emplace(key.segments.back(),std::make_unique<table>())
							.first->second->reinterpret_as<table>();
						tab->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab;
					}
				}

				// if there was already a matching node some sanity checking is necessary;
				// this is ok if we're making an array and the existing element is already an array (new element)
				// or if we're making a table and the existing element is an implicitly-created table (promote it),
				// otherwise this is a redefinition error.
				else
				{
					if (is_array && matching_node->is_array() && find(table_arrays, matching_node->reinterpret_as<array>()))
					{
						auto tab_arr = matching_node->reinterpret_as<array>();
						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab_arr->values.back()->reinterpret_as<table>();
					}

					else if (!is_array
						&& matching_node->is_table()
						&& !implicit_tables.empty())
					{
						auto tbl = matching_node->reinterpret_as<table>();
						const auto idx = find(implicit_tables, tbl);
						if (idx)
						{
							implicit_tables.erase(implicit_tables.cbegin() + *idx);
							tbl->rgn.begin = header_begin_pos;
							tbl->rgn.end = header_end_pos;
							return tbl;
						}
					}

					//if we get here it's a redefinition error.
					if (!is_array && matching_node->type() == node_type::table)
						abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
					else
						abort_with_error(
							"Attempt to redefine existing "sv, matching_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, is_array ? "array-of-tables"sv : "table"sv
						);
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			void parse_key_value_pair_and_insert(table* tab) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();

				auto kvp = parse_key_value_pair();
				TOML_ERROR_CHECK();
				TOML_ASSERT(kvp.key.segments.size() >= 1_sz);

				if (kvp.key.segments.size() > 1_sz)
				{
					for (size_t i = 0; i < kvp.key.segments.size() - 1_sz; i++)
					{
						auto child = tab->get(kvp.key.segments[i]);
						if (!child)
						{
							child = tab->values.emplace(
								std::move(kvp.key.segments[i]),
								std::make_unique<table>()
							).first->second.get();
							dotted_key_tables.push_back(child->reinterpret_as<table>());
							dotted_key_tables.back()->inline_ = true;
							child->rgn = kvp.value->rgn;
						}
						else if (!child->is_table() || !find(dotted_key_tables, child->reinterpret_as<table>()))
						{
							abort_with_error("Attempt to redefine "sv, child->type(), " as dotted key-value pair"sv);
						}
						else
							child->rgn.end = kvp.value->rgn.end;
						TOML_ERROR_CHECK();
						tab = child->reinterpret_as<table>();
					}
				}

				if (auto conflicting_node = tab->get(kvp.key.segments.back()))
				{
					if (conflicting_node->type() == kvp.value->type())
						abort_with_error("Attempt to redefine "sv, conflicting_node->type(), " '"sv, recording_buffer, '\'');
					else
						abort_with_error(
							"Attempt to redefine "sv, conflicting_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, kvp.value->type()
						);
				}

				TOML_ERROR_CHECK();
				tab->values.emplace(
					std::move(kvp.key.segments.back()),
					std::move(kvp.value)
				);
			}

			void parse_document() TOML_MAY_THROW
			{
				TOML_ASSERT(cp);

				table* current_table = &root;

				do
				{
					TOML_ERROR_CHECK();

					// leading whitespace, line endings, comments
					if (consume_leading_whitespace()
						|| consume_line_break()
						|| consume_comment())
						continue;

					// [tables]
					// [[table array]]
					else if (*cp == U'[')
					{
						current_table = parse_table_header();
					}

					// bare_keys
					// dotted.keys
					// "quoted keys"
					#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
					else if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing key; "
							"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					#endif
					else if (is_bare_key_start_character(cp->value) || is_string_delimiter(cp->value))
					{
						parse_key_value_pair_and_insert(current_table);

						// handle the rest of the line after the kvp
						// (this is not done in parse_key_value_pair() because that function is also used for inline tables)
						consume_leading_whitespace();
						if (cp)
						{
							if (!consume_comment() && !consume_line_break())
								abort_with_error(
									"Encountered unexpected character after key-value pair; "
									"expected a comment or whitespace, saw '"sv, *cp, '\''
								);
						}
					}

					else // ??
						abort_with_error(
							"Encountered unexpected character while parsing top level of document; "
							"expected keys, tables, whitespace or comments, saw '"sv, *cp, '\''
						);

				}
				while (cp);

				auto eof_pos = current_position_or_assumed_next();
				eof_pos.column++;
				root.rgn.end = eof_pos;
				if (current_table
					&& current_table != &root
					&& current_table->rgn.end <= current_table->rgn.begin)
					current_table->rgn.end = eof_pos;
			}

			static void update_region_ends(node& nde) noexcept
			{
				const auto type = nde.type();
				if (type > node_type::array)
					return;

				if (type == node_type::table)
				{
					auto& tbl = *nde.reinterpret_as<table>();
					if (tbl.inline_) //inline tables (and all their inline descendants) are already correctly terminated
						return;

					auto end = nde.rgn.end;
					for (auto& [k, v] : tbl.values)
					{
						update_region_ends(*v);
						if (end < v->rgn.end)
							end = v->rgn.end;
					}
				}
				else //arrays
				{
					auto& arr = *nde.reinterpret_as<array>();
					auto end = nde.rgn.end;
					for (auto& v : arr.values)
					{
						update_region_ends(*v);
						if (end < v->rgn.end)
							end = v->rgn.end;
					}
					nde.rgn.end = end;
				}
			}

		public:

			parser(utf8_reader_interface&& reader_) TOML_MAY_THROW
				: reader{ reader_ }
			{
				root.rgn = { prev_pos, prev_pos, reader.source_path() };

				cp = reader.read_next();

				#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
				#endif

				if (cp)
					parse_document();

				update_region_ends(root);
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_FIELD_INIT_WARNING

			[[nodiscard]]
			operator parse_result() && noexcept
			{
				#if TOML_EXCEPTIONS

				return { std::move(root) };

				#else

				if (err)
					return parse_result{ *std::move(err) };
				else
					return parse_result{ std::move(root) };

				#endif

			}

			TOML_POP_WARNINGS
	};

	inline std::unique_ptr<toml::array> parser::parse_array() TOML_MAY_THROW
	{
		TOML_ERROR_CHECK({});
		TOML_ASSERT(cp && *cp == U'[');

		const auto eof_check = [this]() TOML_MAY_THROW
		{
			TOML_ERROR_CHECK();
			if (!cp)
				abort_with_error("Encountered EOF while parsing array"sv);
		};

		// skip opening '['
		advance();
		eof_check();
		TOML_ERROR_CHECK({});

		auto arr = std::make_unique<array>();
		auto& vals = arr->values;

		enum parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (true)
		{
			TOML_ERROR_CHECK({});

			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
				continue;
			eof_check();

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == val)
				{
					prev = comma;
					advance();
					continue;
				}
				abort_with_error(
					"Encountered unexpected character while parsing array; "
					"expected value or closing ']', saw comma"sv
				);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance();
				arr->rgn.end = current_position_or_assumed_next();
				break;
			}

			// must be a value
			else
			{
				if (prev == val)
				{
					abort_with_error(
						"Encountered unexpected character while parsing array; "
						"expected comma or closing ']', saw '"sv, *cp, '\''
					);
					continue;
				}
				prev = val;

				vals.push_back(parse_value());
			}
		}

		TOML_ERROR_CHECK({});
		return arr;
	}

	inline std::unique_ptr<toml::table> parser::parse_inline_table() TOML_MAY_THROW
	{
		TOML_ERROR_CHECK({});
		TOML_ASSERT(cp && *cp == U'{');

		const auto eof_check = [this]() TOML_MAY_THROW
		{
			TOML_ERROR_CHECK();
			if (!cp)
				abort_with_error("Encountered EOF while parsing inline table"sv);
		};

		// skip opening '{'
		advance();
		eof_check();
		TOML_ERROR_CHECK({});

		auto tab = std::make_unique<table>();
		tab->inline_ = true;

		enum parse_elem : int
		{
			none,
			comma,
			kvp
		};
		parse_elem prev = none;

		while (true)
		{
			TOML_ERROR_CHECK({});

			if constexpr (TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/516
			{
				while (consume_leading_whitespace()
					|| consume_line_break()
					|| consume_comment())
					continue;
			}
			else
			{
				while (consume_leading_whitespace())
					continue;
			}

			eof_check();

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == kvp)
				{
					prev = comma;
					advance();
				}
				else
					abort_with_error(
						"Encountered unexpected character while parsing inline table; "
						"expected key-value pair or closing '}', saw comma"sv
					);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				if constexpr (!TOML_LANG_HIGHER_THAN(0, 5, 0)) // toml/issues/516
				{
					if (prev == comma)
					{
						abort_with_error(
							"Encountered unexpected character while parsing inline table; "
							"expected key-value pair, saw closing '}' (dangling comma)"sv
						);
						continue;
					}
				}

				advance();
				tab->rgn.end = current_position_or_assumed_next();
				break;
			}

			// key-value pair
			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
			else if (is_unicode_combining_mark(*cp))
			{
				abort_with_error(
					"Encountered unexpected character while parsing inline table; "
					"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
				);
			}
			#endif
			else if (is_string_delimiter(*cp) || is_bare_key_start_character(*cp))
			{
				if (prev == kvp)
					abort_with_error(
						"Encountered unexpected character while parsing inline table; "
						"expected comma or closing '}', saw '"sv, *cp, '\''
					);
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(tab.get());
				}
			}

			else
				abort_with_error(
					"Encountered unexpected character while parsing inline table; "
					"expected key or closing '}', saw '"sv, *cp, '\''
				);
		}

		TOML_ERROR_CHECK({});
		return tab;
	}

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
}

namespace toml
{
	[[nodiscard]]
	inline parse_result parse(std::string_view doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}

	[[nodiscard]]
	inline parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::parser{ impl::utf8_reader{ doc, std::move(source_path) } };
	}

#if defined(__cpp_lib_char8_t)

	[[nodiscard]]
	inline parse_result parse(std::u8string_view doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}

	[[nodiscard]]
	inline parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::parser{ impl::utf8_reader{ doc, std::move(source_path) } };
	}

#endif

	template <typename CHAR>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<CHAR>& doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}

	template <typename CHAR>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<CHAR>& doc, std::string&& source_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, std::move(source_path) } };
	}

}

#pragma endregion
//-------------------------------------------------  ↑ toml_parser.h  --------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_formatter.h  -----------------------------
#pragma region

namespace toml::impl
{
	struct TOML_INTERFACE formatter_writer_interface
	{
		virtual void operator() (const char*, size_t) TOML_MAY_THROW = 0;
		virtual void operator() (char) TOML_MAY_THROW = 0;

		virtual ~formatter_writer_interface() noexcept = default;
	};

	template <typename T>
	class formatter_writer;

	template <typename CHAR>
	class TOML_EMPTY_BASES formatter_writer<std::basic_ostream<CHAR>> : public formatter_writer_interface
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		private:
			std::basic_ostream<CHAR>& target_;

		public:

			void operator() (const char* characters, size_t num) TOML_MAY_THROW override
			{
				TOML_ASSERT(data && size);
				target_.write(reinterpret_cast<const CHAR*>(characters), static_cast<std::streamsize>(num));
			}

			void operator() (char character) TOML_MAY_THROW override
			{
				target_.put(static_cast<CHAR>(character));
			}

			TOML_NODISCARD_CTOR
			formatter_writer(std::basic_ostream<CHAR>& target) noexcept
				: target_{ target }
			{}
	};

	template <typename CHAR>
	formatter_writer(std::basic_ostream<CHAR>&) -> formatter_writer<std::basic_ostream<CHAR>>;

	struct formatter_options final
	{
		toml::string_view indent_string;
		bool quote_dates_and_times;
	};

	class formatter
	{
		private:
			formatter_writer_interface* writer_ = nullptr;
			formatter_options opts;

		protected:

			[[nodiscard]] TOML_ALWAYS_INLINE
			const formatter_options& options() const noexcept
			{
				return opts;
			}

			const toml::table& source;
			size_t indent_string_columns = {};
			int indent_level;
			bool naked_newline;

			void attach(formatter_writer_interface& writer) noexcept
			{
				indent_level = 0;
				naked_newline = true;
				writer_ = &writer;
			}

			void detach() noexcept
			{
				writer_ = nullptr;
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_ALL_WARNINGS // some compilers will complain about a tautological unsigned >= 0.
									  // TINAE - char can have signed _or_ unsigned semantics and I can't
									  // be arsed handling this differently

			static toml::string_view escape_string_character(const toml::string_char& c) noexcept
			{
				static constexpr toml::string_view low_character_escape_table[]
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

				if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
					return low_character_escape_table[c];
				else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
					return TOML_STRING_PREFIX("\\u007F"sv);
				else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
					return TOML_STRING_PREFIX("\\\""sv);
				else
					return toml::string_view{ &c, 1_sz };
			}

			TOML_POP_WARNINGS

			TOML_ALWAYS_INLINE
			void write(char character) TOML_MAY_THROW
			{
				(*writer_)(character);
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const CHAR* characters, size_t num) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(characters), num);
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(std::basic_string_view<CHAR> strv) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(strv.data()), strv.length());
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const std::basic_string<CHAR>& str) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(str.data()), str.length());
			}

			void write_newline(bool force = false) TOML_MAY_THROW
			{
				if (!naked_newline || force)
				{
					write('\n');
					naked_newline = true;
				}
			}

			void write_indent() TOML_MAY_THROW
			{
				for (int i = 0; i < indent_level; i++)
				{
					write(opts.indent_string);
					naked_newline = false;
				}
			}

			template <typename T>
			void write_integer(T val) TOML_MAY_THROW
			{
				char buf[20_sz]; //strlen("-9223372036854775808")
				const auto res = std::to_chars(buf, buf + sizeof(buf), val);
				write(buf, static_cast<size_t>(res.ptr - buf));
				naked_newline = false;
			}

			void write(const value<int64_t>& val) TOML_MAY_THROW
			{
				write_integer(val.get());
			}

			void write(const value<double>& val) TOML_MAY_THROW
			{
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::ostringstream oss;
					oss << val.get();
					write(oss.str());
					if (val.get() == 0.0)
						write(".0"sv);
				}
				#else
				{
					char buf[32_sz];
					const auto res = std::to_chars(buf, buf + sizeof(buf), val.get());
					const auto sv = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
					write(sv);
					bool needs_decimal_point = true;
					for (auto ch : sv)
					{
						if (ch == 'e' || ch == 'E' || ch == '.')
						{
							needs_decimal_point = false;
							break;
						}
					}
					if (needs_decimal_point)
						write(".0"sv);
				}
				#endif
				naked_newline = false;
			}

			void write(const value<bool>& val) TOML_MAY_THROW
			{
				write(val.get() ? "true"sv : "false"sv);
				naked_newline = false;
			}

			void write_quoted_string(toml::string_view str) TOML_MAY_THROW
			{
				if (str.empty())
					write("\"\""sv);
				else
				{
					write('"');
					for (auto c : str)
						write(escape_string_character(c));
					write('"');
				}
				naked_newline = false;
			}

			void write(const value<toml::string>& val) TOML_MAY_THROW
			{
				write_quoted_string(string_view{ val.get() });
			}

			template <typename T>
			void write_zero_padded_integer(T intval, size_t digits) TOML_MAY_THROW
			{
				static_assert(std::is_unsigned_v<T>);

				char buf[19_sz]; //strlen("9223372036854775807")
				const auto res = std::to_chars(buf, buf + sizeof(buf), intval);
				const auto len = static_cast<size_t>(res.ptr - buf);
				for (size_t i = len; i < digits; i++)
					write('0');
				write(buf, len);
				naked_newline = false;
			}

			void write(const time& tm) TOML_MAY_THROW
			{
				write_zero_padded_integer(tm.hour, 2_sz);
				write(':');
				write_zero_padded_integer(tm.minute, 2_sz);
				write(':');
				write_zero_padded_integer(tm.second, 2_sz);

				if (tm.nanosecond && tm.nanosecond <= 999999999u)
				{
					write('.');
					auto ns = tm.nanosecond;
					size_t digits = 9_sz;
					while (ns % 10u == 0u)
					{
						ns /= 10u;
						digits--;
					}
					write_integer(ns);
				}
			}

			void write(const date& dt) TOML_MAY_THROW
			{
				write_zero_padded_integer(dt.year, 4_sz);
				write('-');
				write_zero_padded_integer(dt.month, 2_sz);
				write('-');
				write_zero_padded_integer(dt.day, 2_sz);
			}

			void write(const date_time& dt) TOML_MAY_THROW
			{
				write(dt.date);
				write('T');
				write(dt.time);
				if (dt.time_offset)
				{
					const auto& to = *dt.time_offset;
					if (!to.hours && !to.minutes)
						write('Z');
					else
					{
						write(to.hours < 0 || to.minutes < 0 ? '-' : '+');
						write_zero_padded_integer(static_cast<uint8_t>(to.hours < 0 ? -to.hours : to.hours), 2_sz);
						write(':');
						write_zero_padded_integer(static_cast<uint8_t>(to.minutes < 0 ? -to.minutes : to.minutes), 2_sz);
					}
				}
			}

			void write(const value<time>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write(const value<date>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write(const value<date_time>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write_value(const node* val_node, node_type type) TOML_MAY_THROW
			{
				switch (type)
				{

					case node_type::string: write(*val_node->reinterpret_as<string>()); break;
					case node_type::integer: write(*val_node->reinterpret_as<int64_t>()); break;
					case node_type::floating_point: write(*val_node->reinterpret_as<double>()); break;
					case node_type::boolean: write(*val_node->reinterpret_as<bool>()); break;
					case node_type::date: write(*val_node->reinterpret_as<date>()); break;
					case node_type::time: write(*val_node->reinterpret_as<time>()); break;
					case node_type::date_time: write(*val_node->reinterpret_as<date_time>()); break;
					TOML_NO_DEFAULT_CASE;
				}
			}

			formatter(const toml::table& source_, formatter_options&& options_) noexcept
				: opts{ std::move(options_) },
				source{ source_ }
			{
				if (opts.indent_string.empty())
				{
					opts.indent_string = TOML_STRING_PREFIX("    "sv);
					indent_string_columns = 4;
				}
				else
				{
					for (auto c : opts.indent_string)
						indent_string_columns += c == '\t' ? 4_sz : 1_sz;
				}
			}
	};
}

#pragma endregion
//-------------------------------------------------------------------  ↑ toml_formatter.h  -----------------------------

//-------------------------------------------------------------------------------------  ↓ toml_default_formatter.h  ---
#pragma region

namespace toml
{
	class default_formatter final : impl::formatter
	{
		private:
			using base = impl::formatter;
			std::vector<toml::string> key_path;

			static toml::string make_key_segment(const toml::string& str) noexcept
			{
				if (str.empty())
					return TOML_STRING_PREFIX("''"s);
				else
				{
					bool requiresQuotes = false;
					{
						impl::utf8_decoder decoder;
						for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
						{
							decoder(static_cast<uint8_t>(str[i]));
							if (decoder.error())
								requiresQuotes = true;
							else if (decoder.has_code_point())
								requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
						}
					}

					if (requiresQuotes)
					{
						toml::string s;
						s.reserve(str.length() + 2_sz);
						s += TOML_STRING_PREFIX('"');
						for (auto c : str)
							s.append(escape_string_character(c));
						s += TOML_STRING_PREFIX('"');
						return s;
					}
					else
						return str;
				}
			}

			void write_key_segment(const toml::string& str) TOML_MAY_THROW
			{
				if (str.empty())
					base::write("''"sv);
				else
				{
					bool requiresQuotes = false;
					{
						impl::utf8_decoder decoder;
						for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
						{
							decoder(static_cast<uint8_t>(str[i]));
							if (decoder.error())
								requiresQuotes = true;
							else if (decoder.has_code_point())
								requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
						}
					}

					if (requiresQuotes)
						write_quoted_string(str);
					else
						base::write(str);
				}
			}

			void write_key_path() TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						base::write('.');
					base::write(segment);
				}
				naked_newline = false;
			}

			inline void write_inline(const table& /*tbl*/) TOML_MAY_THROW;

			static size_t inline_columns(const node& node) noexcept
			{
				return node.visit([](const auto& n) noexcept
					-> size_t
				{
					using node_t = impl::remove_cvref_t<decltype(n)>;
					if constexpr (std::is_same_v<node_t, table>)
					{
						const auto& values = n.values;
						if (values.empty())
							return 2_sz; // "{}"
						size_t weight = 3_sz; // "{ }"
						for (const auto& [k, v] : values)
							weight += k.length() + inline_columns(*v) + 2_sz; // +  ", "
						return weight;
					}
					else if constexpr (std::is_same_v<node_t, array>)
					{
						if (n.empty())
							return 2_sz; // "[]"
						size_t weight = 3_sz; // "[ ]"
						for (size_t i = 0; i < n.size(); i++)
							weight += inline_columns(*n.get(i)) + 2_sz; // +  ", "
						return weight;
					}
					else if constexpr (std::is_same_v<node_t, value<string>>)
					{
						return n.get().length() + 2_sz; // + ""
					}
					else if constexpr (std::is_same_v<node_t, value<int64_t>>)
					{
						auto v = n.get();
						if (!v)
							return 1_sz;
						size_t weight = {};
						if (v < 0)
						{
							weight += 1;
							v *= -1;
						}
						return weight + static_cast<size_t>(std::log10(static_cast<double>(v)));
					}
					else if constexpr (std::is_same_v<node_t, value<double>>)
					{
						auto v = n.get();
						if (v == 0.0)
							return 3_sz;
						size_t weight = 2_sz; // ".0"
						if (v < 0.0)
						{
							weight += 1;
							v *= -1.0;
						}
						return weight + static_cast<size_t>(std::log10(v));
					}
					else if constexpr (std::is_same_v<node_t, value<bool>>)
					{
						return 5_sz;
					}
					else if constexpr (std::is_same_v<node_t, value<date>> || std::is_same_v<node_t, value<time>>)
					{
						return 10_sz;
					}
					else if constexpr (std::is_same_v<node_t, value<date_time>>)
					{
						return 30_sz;
					}
					TOML_UNREACHABLE;
				});
			}

			static bool forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
			{
				return (inline_columns(node) + starting_column_bias) > 120_sz;
			}

			void write(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					base::write("[]"sv);
				else
				{
					const auto multiline = forces_multiline(
						arr,
						base::indent_string_columns * static_cast<size_t>(indent_level < 0 ? 0 : indent_level)
					);
					const auto original_indent = indent_level;
					base::write("["sv);
					if (multiline)
					{
						if (indent_level < 0)
							indent_level++;
						indent_level++;
					}
					else
						base::write(' ');

					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
						{
							base::write(',');
							if (!multiline)
								base::write(' ');
						}

						if (multiline)
						{
							write_newline(true);
							write_indent();
						}

						auto v = arr.get(i);
						const auto type = v->type();
						switch (type)
						{
							case node_type::table: write_inline(*reinterpret_cast<const table*>(v)); break;
							case node_type::array: write(*reinterpret_cast<const array*>(v)); break;
							default:
								write_value(v, type);
						}

					}
					if (multiline)
					{
						indent_level = original_indent;
						write_newline(true);
						write_indent();
					}
					else
						base::write(' ');
					base::write("]"sv);
				}
				naked_newline = false;
			}

			void write(const table& tbl) TOML_MAY_THROW
			{
				//values, arrays, and inline tables
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(v.get())->is_inline())
						|| (type == node_type::array && reinterpret_cast<const array*>(v.get())->is_array_of_tables()))
						continue;

					write_newline();
					write_indent();
					write_key_segment(k);
					base::write(" = "sv);
					switch (type)
					{
						case node_type::table: write_inline(*reinterpret_cast<const table*>(v.get())); break;
						case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
						default:
							write_value(v.get(), type);
					}
				}

				//non-inline tables
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if (type != node_type::table || reinterpret_cast<const table*>(v.get())->is_inline())
						continue;
					auto& child_tbl = *reinterpret_cast<const table*>(v.get());

					//we can skip indenting and emitting the headers for tables that only contain other tables
					//(so we don't over-nest)
					size_t child_value_count{}; //includes inline tables and non-table arrays
					size_t child_table_count{};
					size_t child_table_array_count{};
					for (auto& [child_k, child_v] : child_tbl.values)
					{
						const auto child_type = child_v->type();
						switch (child_type)
						{
							case node_type::table:
								if (reinterpret_cast<const table*>(child_v.get())->is_inline())
									child_value_count++;
								else
									child_table_count++;
								break;

							case node_type::array:
								if (reinterpret_cast<const array*>(child_v.get())->is_array_of_tables())
									child_table_array_count++;
								else
									child_value_count++;
								break;

							default:
								child_value_count++;
						}
					}
					bool skip_self = false;
					if (child_value_count == 0_sz && (child_table_count > 0_sz || child_table_array_count > 0_sz))
						skip_self = true;

					if (!skip_self)
						indent_level++;
					key_path.push_back(make_key_segment(k));

					if (!skip_self)
					{
						write_newline();
						write_newline(true);
						write_indent();
						base::write("["sv);
						write_key_path();
						base::write("]"sv);
						write_newline(true);
					}

					write(child_tbl);

					key_path.pop_back();
					if (!skip_self)
						indent_level--;
				}

				//table arrays
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if (type != node_type::array || !reinterpret_cast<const array*>(v.get())->is_array_of_tables())
						continue;
					auto& arr = *reinterpret_cast<const array*>(v.get());

					indent_level++;
					key_path.push_back(make_key_segment(k));

					for (size_t i = 0; i < arr.size(); i++)
					{
						write_newline();
						write_newline(true);
						write_indent();
						base::write("[["sv);
						write_key_path();
						base::write("]]"sv);
						write_newline(true);
						write(*reinterpret_cast<const table*>(arr.get(i)));
					}

					key_path.pop_back();
					indent_level--;
				}
			}

			void write() TOML_MAY_THROW
			{
				write(source);
			}

		public:

			TOML_NODISCARD_CTOR
			default_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: formatter{
					source_,
					impl::formatter_options{
						indent_string,
						false //quote_dates_and_times
					}
				}
			{}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, default_formatter& rhs)
				TOML_MAY_THROW
			{
				auto fw = impl::formatter_writer{ lhs };
				rhs.attach(fw);
				rhs.indent_level--; //starts at -1 so root kvps and first-level child tables have the same indent
				rhs.key_path.clear();
				rhs.write();
				rhs.detach();
				return lhs;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, default_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	inline void default_formatter::write_inline(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.values.empty())
			base::write("{}"sv);
		else
		{
			base::write("{ "sv);

			bool first = false;
			for (auto& [k, v] : tbl.values)
			{
				if (first)
					base::write(", "sv);
				first = true;

				write_key_segment(k);
				base::write(" = "sv);

				const auto type = v->type();
				switch (type)
				{
					case node_type::table: write_inline(*reinterpret_cast<const table*>(v.get())); break;
					case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
					default:
						write_value(v.get(), type);
				}
			}

			base::write(" }"sv);
		}
		naked_newline = false;
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		return lhs << default_formatter{ rhs };
	}
}

#pragma endregion
//-------------------------------------------------------------------------------------  ↑ toml_default_formatter.h  ---

//-------------  ↓ toml_json_formatter.h  ------------------------------------------------------------------------------
#pragma region

namespace toml
{
	class json_formatter final : impl::formatter
	{
		private:
			using base = impl::formatter;

			inline void write(const toml::table& tbl) TOML_MAY_THROW;

			void write(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					base::write("[]"sv);
				else
				{
					base::write('[');
					indent_level++;
					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
							base::write(',');
						write_newline(true);
						write_indent();

						auto v = arr.get(i);
						const auto type = v->type();
						switch (type)
						{
							case node_type::table: write(*reinterpret_cast<const table*>(v)); break;
							case node_type::array: write(*reinterpret_cast<const array*>(v)); break;
							default:
								write_value(v, type);
						}

					}
					indent_level--;
					write_newline(true);
					write_indent();
					base::write(']');
				}
				naked_newline = false;
			}

			void write() TOML_MAY_THROW
			{
				write(source);
			}

		public:

			TOML_NODISCARD_CTOR
				json_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: formatter{
					source_,
					impl::formatter_options{
						indent_string,
						true //quote_dates_and_times
					}
				}
			{}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, json_formatter& rhs)
				TOML_MAY_THROW
			{
				auto fw = impl::formatter_writer{ lhs };
				rhs.attach(fw);
				rhs.write();
				rhs.detach();
				return lhs;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, json_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	inline void json_formatter::write(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			base::write("{}"sv);
		else
		{
			base::write('{');
			indent_level++;
			bool first = false;
			for (auto& [k, v] : tbl.values)
			{
				if (first)
					base::write(", "sv);
				first = true;
				write_newline(true);
				write_indent();

				base::write_quoted_string(k);
				base::write(" : "sv);

				const auto type = v->type();
				switch (type)
				{
					case node_type::table: write(*reinterpret_cast<const table*>(v.get())); break;
					case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
					default:
						write_value(v.get(), type);
				}

			}
			indent_level--;
			write_newline(true);
			write_indent();
			base::write('}');
		}
		naked_newline = false;
	}
}

#pragma endregion
//-------------  ↑ toml_json_formatter.h  ------------------------------------------------------------------------------

//macro hygiene
#if TOML_UNDEF_MACROS
	#undef TOML_EXCEPTIONS
	#undef TOML_USE_STREAMS_FOR_FLOATS
	#undef TOML_PUSH_WARNINGS
	#undef TOML_DISABLE_SWITCH_WARNING
	#undef TOML_DISABLE_FIELD_INIT_WARNING
	#undef TOML_DISABLE_VAR_INIT_WARNING
	#undef TOML_DISABLE_ALL_WARNINGS
	#undef TOML_POP_WARNINGS
	#undef TOML_ALWAYS_INLINE
	#undef TOML_ASSUME
	#undef TOML_UNREACHABLE
	#undef TOML_INTERFACE
	#undef TOML_EMPTY_BASES
	#undef TOML_CPP
	#undef TOML_CONDITIONAL_NOEXCEPT
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
#endif

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
// clang-format on
