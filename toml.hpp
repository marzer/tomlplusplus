//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v0.2.2
// https://github.com/marzer/tomlplusplus
// SPDX-License-Identifier: MIT
//
//----------------------------------------------------------------------------------------------------------------------
//
// -         THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY            -
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
// Copyright (c) 2019-2020 Mark Gillard
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
//
// UTF-8 decoding is performed using a derivative of Bjoern Hoehrmann's 'Flexible and Economical UTF-8 Decoder'
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
//
// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
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

//-----------------  ↓ toml_common.h  ----------------------------------------------------------------------------------
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

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif

#if defined(__clang__) || defined(__GNUC__)
	#define TOML_GCC_ATTR(attr)		__attribute__((attr))
#else
	#define TOML_GCC_ATTR(attr)
#endif

#ifdef __clang__

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

#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(__ICL))

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

	#if !defined(TOML_RELOPS_REORDERING) && defined(__cpp_impl_three_way_comparison)
		#define TOML_RELOPS_REORDERING 1
	#endif

#elif defined(__GNUC__)

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

	#if !defined(TOML_RELOPS_REORDERING) && defined(__cpp_impl_three_way_comparison)
		#define TOML_RELOPS_REORDERING 1
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
#if defined(__EXCEPTIONS) || defined(__cpp_exceptions) || defined(_CPPUNWIND)
	#define TOML_EXCEPTIONS	1
	#define TOML_MAY_THROW
	#define TOML_MAY_THROW_UNLESS(...)	noexcept(__VA_ARGS__)
	#define TOML_INLINE_NS_EX
#else
	#define TOML_EXCEPTIONS	0
	#define TOML_MAY_THROW				noexcept
	#define TOML_MAY_THROW_UNLESS(...)	noexcept
	#define TOML_INLINE_NS_EX _noex
#endif
#ifndef TOML_DOXYGEN
	#define TOML_DOXYGEN 0
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
#ifndef TOML_RELOPS_REORDERING
	#define TOML_RELOPS_REORDERING 0
#endif
#if TOML_RELOPS_REORDERING
	#define TOML_ASYMMETRICAL_EQUALITY_OPS(...)
#else
	#define TOML_ASYMMETRICAL_EQUALITY_OPS(LHS, RHS, ...)														\
		__VA_ARGS__ [[nodiscard]] friend bool operator == (RHS rhs, LHS lhs) noexcept { return lhs == rhs; }	\
		__VA_ARGS__ [[nodiscard]] friend bool operator != (LHS lhs, RHS rhs) noexcept { return !(lhs == rhs); }	\
		__VA_ARGS__ [[nodiscard]] friend bool operator != (RHS rhs, LHS lhs) noexcept { return !(lhs == rhs); }
#endif

#define TOML_LIB_MAJOR		0
#define TOML_LIB_MINOR		2
#define TOML_LIB_PATCH		2

#define TOML_LANG_MAJOR		0
#define TOML_LANG_MINOR		5
#define TOML_LANG_PATCH		0

#define TOML_MAKE_VERSION(maj, min, rev)											\
		((maj) * 1000 + (min) * 25 + (rev))

#if TOML_UNRELEASED_FEATURES
	#define TOML_LANG_EFFECTIVE_VERSION												\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH+1)
#else
	#define TOML_LANG_EFFECTIVE_VERSION												\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)
#endif

#define TOML_LANG_HIGHER_THAN(maj, min, rev)										\
		(TOML_LANG_EFFECTIVE_VERSION > TOML_MAKE_VERSION(maj, min, rev))

#define TOML_LANG_AT_LEAST(maj, min, rev)											\
		(TOML_LANG_EFFECTIVE_VERSION >= TOML_MAKE_VERSION(maj, min, rev))

#define TOML_LANG_EXACTLY(maj, min, rev)											\
		(TOML_LANG_EFFECTIVE_VERSION == TOML_MAKE_VERSION(maj, min, rev))

#if TOML_CHAR_8_STRINGS
	#define TOML_INLINE_NS_CHAR_8 _char8
#else
	#define TOML_INLINE_NS_CHAR_8
#endif

#if !TOML_DOXYGEN

	#define TOML_START_2(VER, ARG1, ARG2)	namespace toml { inline namespace v##VER##ARG1##ARG2
	#define TOML_START_1(VER, ARG1, ARG2)	TOML_START_2(VER, ARG1, ARG2)
	#define TOML_START		TOML_START_1(TOML_LIB_MAJOR,TOML_INLINE_NS_EX,TOML_INLINE_NS_CHAR_8)
	#define TOML_END		}

#endif
#define TOML_IMPL_START		TOML_START { namespace impl
#define TOML_IMPL_END		} TOML_END

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS

#include <cstdint>
#include <cstring>		//memcpy, memset
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

namespace toml { }

TOML_START
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;

	[[nodiscard]] TOML_ALWAYS_INLINE
	TOML_CONSTEVAL size_t operator"" _sz(unsigned long long n) noexcept
	{
		return static_cast<size_t>(n);
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

	struct date;
	struct time;
	struct time_offset;
	struct date_time;
	class node;
	template <typename T> class node_view;
	template <typename T> class value;
	class array;
	class table;

	enum class node_type : uint8_t
	{
		none,
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

	struct source_position
	{
		source_index line;
		source_index column;

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
			TOML_MAY_THROW;
	};

	using source_path_ptr = std::shared_ptr<const std::string>;

	struct source_region
	{
		source_position begin;
		source_position end;
		source_path_ptr path;

		template <typename CHAR>
		friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_region& rhs)
			TOML_MAY_THROW;
	};

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS

	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

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

			[[nodiscard]]
			std::string_view description() const noexcept
			{
				return description_;
			}

			[[nodiscard]]
			const source_region& source() const noexcept
			{
				return source_;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const parse_error& rhs)
				TOML_MAY_THROW;
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

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const parse_error& rhs)
				TOML_MAY_THROW;
	};

	#endif

	TOML_POP_WARNINGS
}
TOML_END

TOML_IMPL_START
{
	template <typename T>
	using string_map = std::map<string, T, std::less<>>; //heterogeneous lookup

	#if defined(__cpp_lib_remove_cvref) || (defined(_MSC_VER) && defined(_HAS_CXX20) && _HAS_CXX20)

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
	constexpr std::underlying_type_t<T> unbox_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use std::find??"
	// A: Because <algorithm> is _huge_ and std::find would be the only thing I used from it.
	//    I don't want to impose such a heavy compile-time burden on users.

	template <typename T>
	inline std::optional<size_t> find(const std::vector<T>& haystack, const T& needle) noexcept
	{
		for (size_t i = 0, e = haystack.size(); i < e; i++)
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
TOML_IMPL_END

TOML_START
{
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref_t<T>, table>;
	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref_t<T>, array>;
	template <typename T>
	inline constexpr bool is_string = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<string>>;
	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<int64_t>>;
	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<double>>;
	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;
	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<bool>>;
	template <typename T>
	inline constexpr bool is_date = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<date>>;
	template <typename T>
	inline constexpr bool is_time = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<time>>;
	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<impl::node_of<impl::remove_cvref_t<T>>, value<date_time>>;

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
TOML_END

#pragma endregion
//-----------------  ↑ toml_common.h  ----------------------------------------------------------------------------------

//----------------------------------------  ↓ toml_date_time.h  --------------------------------------------------------
#pragma region

TOML_START
{
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

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
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

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	struct time_offset final
	{
		int16_t minutes;

		TOML_NODISCARD_CTOR
		constexpr time_offset() noexcept
			: minutes{}
		{}

		TOML_NODISCARD_CTOR
		constexpr time_offset(int8_t h, int8_t m) noexcept
			: minutes{ static_cast<int16_t>(h * 60 + m) }
		{}

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

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time_offset& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};

	struct date_time final
	{
		toml::date date;
		toml::time time;
		std::optional<toml::time_offset> time_offset;

		TOML_NODISCARD_CTOR
		constexpr date_time() noexcept
			: date{},
			time{}
		{}

		TOML_NODISCARD_CTOR
		constexpr date_time(toml::date d, toml::time t) noexcept
			: date{ d },
			time{ t }
		{}

		TOML_NODISCARD_CTOR
			constexpr date_time(toml::date d, toml::time t, toml::time_offset offset) noexcept
			: date{ d },
			time{ t },
			time_offset{ offset }
		{}

		[[nodiscard]]
		constexpr bool is_local() const noexcept
		{
			return !time_offset.has_value();
		}

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

		template <typename CHAR>
		friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date_time& rhs)
			TOML_MAY_THROW
		{
			impl::print_to_stream(rhs, lhs);
			return lhs;
		}
	};
}
TOML_END

#pragma endregion
//----------------------------------------  ↑ toml_date_time.h  --------------------------------------------------------

//--------------------------------------------------------------  ↓ toml_print_to_stream.h  ----------------------------
#pragma region

TOML_IMPL_START
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm supporting C++20's char8_t as well; wrapping streams allows switching string modes transparently.
	//    - I'm using <charconv> to format numerics. Faster and locale-independent.
	//    - I can avoid forcing users to drag in <sstream> and <iomanip>.

	// Q: "there's a lot of reinterpret_casting here, is any of it UB?"
	// A: - If the source string data is char and the output string is char8_t, then technically yes,
	//      but not in the other direction. I test in both modes on Clang, GCC and MSVC and have yet to
	//      see it actually causing an issue, but in the event it does present a problem it's not going to
	//      be a show-stopper since all it means is I need to do duplicate some code.
	//    - Strings in C++. Honestly.

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(std::basic_string_view<CHAR1> str, std::basic_ostream<CHAR2>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(const std::basic_string<CHAR1>& str, std::basic_ostream<CHAR2>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char character, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GCC_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char* str, size_t len, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#if defined(__cpp_lib_char8_t)

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char8_t character, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GCC_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char8_t* str, size_t len, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#endif

	template <typename T> inline constexpr size_t charconv_buffer_length = 0;
	template <> inline constexpr size_t charconv_buffer_length<double> = 60;
	template <> inline constexpr size_t charconv_buffer_length<float> = 40;
	template <> inline constexpr size_t charconv_buffer_length<uint64_t> = 20; // strlen("18446744073709551615")
	template <> inline constexpr size_t charconv_buffer_length<int64_t> = 20;  // strlen("-9223372036854775808")
	template <> inline constexpr size_t charconv_buffer_length<int32_t> = 11;  // strlen("-2147483648")
	template <> inline constexpr size_t charconv_buffer_length<int16_t> = 6;   // strlen("-32768")
	template <> inline constexpr size_t charconv_buffer_length<int8_t> = 4;    // strlen("-128")
	template <> inline constexpr size_t charconv_buffer_length<uint32_t> = 10; // strlen("4294967295")
	template <> inline constexpr size_t charconv_buffer_length<uint16_t> = 5;  // strlen("65535")
	template <> inline constexpr size_t charconv_buffer_length<uint8_t> = 3;   // strlen("255")

	template <typename T, typename CHAR>
	inline void print_integer_to_stream(T val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	#define TOML_P2S_OVERLOAD(type)																\
		template <typename CHAR>																\
		TOML_ALWAYS_INLINE																		\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW			\
		{																						\
			static_assert(sizeof(CHAR) == 1);													\
			print_integer_to_stream(val, stream);												\
		}

	TOML_P2S_OVERLOAD(int8_t)
	TOML_P2S_OVERLOAD(int16_t)
	TOML_P2S_OVERLOAD(int32_t)
	TOML_P2S_OVERLOAD(int64_t)
	TOML_P2S_OVERLOAD(uint8_t)
	TOML_P2S_OVERLOAD(uint16_t)
	TOML_P2S_OVERLOAD(uint32_t)
	TOML_P2S_OVERLOAD(uint64_t)

	#undef TOML_P2S_OVERLOAD

	template <typename T, typename CHAR>
	inline void print_floating_point_to_stream(T val, std::basic_ostream<CHAR>& stream, bool hexfloat = false) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		static constexpr auto needs_decimal_point = [](auto&& s) noexcept
		{
			for (auto c : s)
				if (c == '.' || c == 'E' || c == 'e')
					return false;
			return true;
		};

		#if TOML_USE_STREAMS_FOR_FLOATS
		{
			std::ostringstream oss;
			oss.precision(std::numeric_limits<T>::digits10 + 1);
			if (hexfloat)
				oss << std::hexfloat;
			oss << val;
			const auto str = oss.str();
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#else
		{
			TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
			const auto res = hexfloat
				? std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::hex)
				: std::to_chars(buf, buf + sizeof(buf), val);
			const auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#endif
	}

	#define TOML_P2S_OVERLOAD(type)																\
		template <typename CHAR>																\
		TOML_ALWAYS_INLINE																		\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW			\
		{																						\
			static_assert(sizeof(CHAR) == 1);													\
			print_floating_point_to_stream(val, stream);										\
		}

	TOML_P2S_OVERLOAD(float)
	TOML_P2S_OVERLOAD(double)

	#undef TOML_P2S_OVERLOAD

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(bool val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val ? "true"sv : "false"sv, stream);
	}

	template <typename T, typename CHAR>
	inline void print_to_stream(T val, std::basic_ostream<CHAR>& stream, size_t zero_pad_to_digits) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		const auto len = static_cast<size_t>(res.ptr - buf);
		for (size_t i = len; i < zero_pad_to_digits; i++)
			print_to_stream('0', stream);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.year, stream, 4_sz);
		print_to_stream('-', stream);
		print_to_stream(val.month, stream, 2_sz);
		print_to_stream('-', stream);
		print_to_stream(val.day, stream, 2_sz);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::time& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.hour, stream, 2_sz);
		print_to_stream(':', stream);
		print_to_stream(val.minute, stream, 2_sz);
		print_to_stream(':', stream);
		print_to_stream(val.second, stream, 2_sz);
		if (val.nanosecond && val.nanosecond <= 999999999u)
		{
			print_to_stream('.', stream);
			auto ns = val.nanosecond;
			size_t digits = 9_sz;
			while (ns % 10u == 0u)
			{
				ns /= 10u;
				digits--;
			}
			print_to_stream(ns, stream, digits);
		}
	}

	template <typename CHAR>
	inline void print_to_stream(toml::time_offset val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		if (!val.minutes)
			print_to_stream('Z', stream);
		else
		{
			auto mins = static_cast<int>(val.minutes);
			if (mins < 0)
			{
				print_to_stream('-', stream);
				mins = -mins;
			}
			else
				print_to_stream('+', stream);
			const auto hours = mins / 60;
			if (hours)
			{
				print_to_stream(static_cast<unsigned int>(hours), stream, 2_sz);
				mins -= hours * 60;
			}
			else
				print_to_stream("00"sv, stream);
			print_to_stream(':', stream);
			print_to_stream(static_cast<unsigned int>(mins), stream, 2_sz);
		}
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date_time& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.date, stream);
		print_to_stream('T', stream);
		print_to_stream(val.time, stream);
		if (val.time_offset)
			print_to_stream(*val.time_offset, stream);
	}

	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	template <typename T, typename CHAR>
	void print_to_stream_with_escapes(T && str, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		for (auto c : str)
		{
			if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
				print_to_stream(low_character_escape_table[c], stream);
			else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\u007F"sv), stream);
			else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\\""sv), stream);
			else if (c == TOML_STRING_PREFIX('\\')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\\\"sv), stream);
			else
				print_to_stream(c, stream);
		}
	}

	TOML_POP_WARNINGS
}
TOML_IMPL_END

TOML_START
{
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_position& rhs)
		TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);
		impl::print_to_stream("line "sv, lhs);
		impl::print_to_stream(rhs.line, lhs);
		impl::print_to_stream(", column "sv, lhs);
		impl::print_to_stream(rhs.column, lhs);
		return lhs;
	}

	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_region& rhs)
		TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);
		lhs << rhs.begin;
		if (rhs.path)
		{
			impl::print_to_stream(" of '"sv, lhs);
			impl::print_to_stream(*rhs.path, lhs);
			impl::print_to_stream('\'', lhs);
		}
		return lhs;
	}

	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const parse_error& rhs)
		TOML_MAY_THROW
	{
		impl::print_to_stream(rhs.description(), lhs);
		impl::print_to_stream("\n\t(error occurred at "sv, lhs);
		lhs << rhs.source();
		impl::print_to_stream(")"sv, lhs);
		return lhs;
	}
}
TOML_END

#pragma endregion
//--------------------------------------------------------------  ↑ toml_print_to_stream.h  ----------------------------

//---------------------------------------------------------------------------------------------  ↓ toml_node.h  --------
#pragma region

TOML_START
{
	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			source_region source_{};

		protected:

			node(node&& other) noexcept
				: source_{ std::move(other.source_) }
			{
				other.source_.begin = {};
				other.source_.end = {};
			}

			node& operator= (node&& rhs) noexcept
			{
				source_ = std::move(rhs.source_);
				rhs.source_.begin = {};
				rhs.source_.end = {};
				return *this;
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>& ref_cast() & noexcept { return *reinterpret_cast<impl::node_of<T>*>(this); }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>&& ref_cast() && noexcept { return std::move(*reinterpret_cast<impl::node_of<T>*>(this)); }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const impl::node_of<T>& ref_cast() const & noexcept { return *reinterpret_cast<const impl::node_of<T>*>(this); }

			template <typename N, typename T>
			using ref_cast_type = decltype(std::declval<N>().template ref_cast<T>());

			node() noexcept = default;
			node(const node&) = delete;
			node& operator= (const node&) = delete;

		public:

			virtual ~node() noexcept = default;

			[[nodiscard]] virtual node_type type() const noexcept = 0;
			[[nodiscard]] virtual bool is_table() const noexcept = 0;
			[[nodiscard]] virtual bool is_array() const noexcept = 0;
			[[nodiscard]] virtual bool is_value() const noexcept = 0;
			[[nodiscard]] virtual bool is_string() const noexcept { return false; }
			[[nodiscard]] virtual bool is_integer() const noexcept { return false; }
			[[nodiscard]] virtual bool is_floating_point() const noexcept { return false; }
			[[nodiscard]] virtual bool is_number() const noexcept { return false; }
			[[nodiscard]] virtual bool is_boolean() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date() const noexcept { return false; }
			[[nodiscard]] virtual bool is_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_date_time() const noexcept { return false; }
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept { return false; }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is() const noexcept
			{
				using type = impl::unwrapped<impl::remove_cvref_t<T>>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return is_table();
				else if constexpr (std::is_same_v<type, array>) return is_array();
				else if constexpr (std::is_same_v<type, string>) return is_string();
				else if constexpr (std::is_same_v<type, int64_t>) return is_integer();
				else if constexpr (std::is_same_v<type, double>) return is_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return is_boolean();
				else if constexpr (std::is_same_v<type, date>) return is_date();
				else if constexpr (std::is_same_v<type, time>) return is_time();
				else if constexpr (std::is_same_v<type, date_time>) return is_date_time();
			}

			[[nodiscard]] virtual table* as_table() noexcept { return nullptr; }
			[[nodiscard]] virtual array* as_array() noexcept { return nullptr; }
			[[nodiscard]] virtual value<string>* as_string() noexcept { return nullptr; }
			[[nodiscard]] virtual value<int64_t>* as_integer() noexcept { return nullptr; }
			[[nodiscard]] virtual value<double>* as_floating_point() noexcept { return nullptr; }
			[[nodiscard]] virtual value<bool>* as_boolean() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date>* as_date() noexcept { return nullptr; }
			[[nodiscard]] virtual value<time>* as_time() noexcept { return nullptr; }
			[[nodiscard]] virtual value<date_time>* as_date_time() noexcept { return nullptr; }
			[[nodiscard]] virtual const table* as_table() const noexcept { return nullptr; }
			[[nodiscard]] virtual const array* as_array() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<string>* as_string() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<int64_t>* as_integer() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<double>* as_floating_point() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<bool>* as_boolean() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date>* as_date() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<time>* as_time() const noexcept { return nullptr; }
			[[nodiscard]] virtual const value<date_time>* as_date_time() const noexcept { return nullptr; }

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>* as() noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const impl::node_of<T>* as() const noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

					 if constexpr (std::is_same_v<type, table>) return as_table();
				else if constexpr (std::is_same_v<type, array>) return as_array();
				else if constexpr (std::is_same_v<type, string>) return as_string();
				else if constexpr (std::is_same_v<type, int64_t>) return as_integer();
				else if constexpr (std::is_same_v<type, double>) return as_floating_point();
				else if constexpr (std::is_same_v<type, bool>) return as_boolean();
				else if constexpr (std::is_same_v<type, date>) return as_date();
				else if constexpr (std::is_same_v<type, time>) return as_time();
				else if constexpr (std::is_same_v<type, date_time>) return as_date_time();
			}

			[[nodiscard]] const source_region& source() const noexcept
			{
				return source_;
			}

		private:

			template <typename FUNC, typename N, typename T>
			static constexpr bool can_visit = std::is_invocable_v<FUNC, ref_cast_type<N, T>>;

			template <typename FUNC, typename N>
			static constexpr bool can_visit_any =
				can_visit<FUNC, N, table>
				|| can_visit<FUNC, N, array>
				|| can_visit<FUNC, N, string>
				|| can_visit<FUNC, N, int64_t>
				|| can_visit<FUNC, N, double>
				|| can_visit<FUNC, N, bool>
				|| can_visit<FUNC, N, date>
				|| can_visit<FUNC, N, time>
				|| can_visit<FUNC, N, date_time>;

			template <typename FUNC, typename N>
			static constexpr bool can_visit_all =
				can_visit<FUNC, N, table>
				&& can_visit<FUNC, N, array>
				&& can_visit<FUNC, N, string>
				&& can_visit<FUNC, N, int64_t>
				&& can_visit<FUNC, N, double>
				&& can_visit<FUNC, N, bool>
				&& can_visit<FUNC, N, date>
				&& can_visit<FUNC, N, time>
				&& can_visit<FUNC, N, date_time>;

			#if TOML_EXCEPTIONS

			template <typename FUNC, typename N, typename T>
			static constexpr bool visit_is_nothrow_one =
				!can_visit<FUNC, N, T>
				|| std::is_nothrow_invocable_v<FUNC, ref_cast_type<N, T>>;

			template <typename FUNC, typename N>
			static constexpr bool visit_is_nothrow =
				visit_is_nothrow_one<FUNC, N, table>
				&& visit_is_nothrow_one<FUNC, N, array>
				&& visit_is_nothrow_one<FUNC, N, string>
				&& visit_is_nothrow_one<FUNC, N, int64_t>
				&& visit_is_nothrow_one<FUNC, N, double>
				&& visit_is_nothrow_one<FUNC, N, bool>
				&& visit_is_nothrow_one<FUNC, N, date>
				&& visit_is_nothrow_one<FUNC, N, time>
				&& visit_is_nothrow_one<FUNC, N, date_time>;

			#endif

			template <typename FUNC, typename N, typename T, bool = can_visit<FUNC, N, T>>
			struct visit_return_type final
			{
				using type = decltype(std::declval<FUNC>()(std::declval<ref_cast_type<N, T>>()));
			};
			template <typename FUNC, typename N, typename T>
			struct visit_return_type<FUNC, N, T, false> final
			{
				using type = void;
			};

			template <typename A, typename B>
			using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N&& node, FUNC&& visitor)
				TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, N&&>)
			{
				static_assert(
					can_visit_any<FUNC&&, N&&>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (node.type())
				{
					case node_type::table:
						if constexpr (can_visit<FUNC&&, N&&, table>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<table>());
						break;
					case node_type::array:
						if constexpr (can_visit<FUNC&&, N&&, array>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<array>());
						break;
					case node_type::string:
						if constexpr (can_visit<FUNC&&, N&&, string>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<string>());
						break;
					case node_type::integer:
						if constexpr (can_visit<FUNC&&, N&&, int64_t>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<int64_t>());
						break;
					case node_type::floating_point:
						if constexpr (can_visit<FUNC&&, N&&, double>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<double>());
						break;
					case node_type::boolean:
						if constexpr (can_visit<FUNC&&, N&&, bool>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<bool>());
						break;
					case node_type::date:
						if constexpr (can_visit<FUNC&&, N&&, date>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<date>());
						break;
					case node_type::time:
						if constexpr (can_visit<FUNC&&, N&&, time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<time>());
						break;
					case node_type::date_time:
						if constexpr (can_visit<FUNC&&, N&&, date_time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(node).template ref_cast<date_time>());
						break;
					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (can_visit_all<FUNC&&, N&&>)
					TOML_UNREACHABLE;
				else
				{
					using return_type =
						nonvoid<typename visit_return_type<FUNC&&, N&&, table>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, array>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, string>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, int64_t>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, double>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, bool>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, date>::type,
						nonvoid<typename visit_return_type<FUNC&&, N&&, time>::type,
								typename visit_return_type<FUNC&&, N&&, date_time>::type
					>>>>>>>>;

					if constexpr (!std::is_void_v<return_type>)
					{
						static_assert(
							std::is_default_constructible_v<return_type>,
							"Non-exhaustive visitors must return a default-constructible type, or void"
						);
						return return_type{};
					}
				}
			}

		public:

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) & TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) && TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, node&&>)
			{
				return do_visit(std::move(*this), std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const& TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, const node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}
	};
}
TOML_END

#pragma endregion
//---------------------------------------------------------------------------------------------  ↑ toml_node.h  --------

//-----------------  ↓ toml_value.h  -----------------------------------------------------------------------------------
#pragma region

TOML_START
{
	template <typename T>
	class value final : public node
	{
		static_assert(
			impl::is_value<T>,
			"Template type parameter must be one of the basic value types"
		);

		private:
			friend class impl::parser;

			template <typename U, typename V>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static auto as_value([[maybe_unused]] V* ptr) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return ptr;
				else
					return nullptr;
			}

			T val_;

		public:

			using value_type = T;
			using value_arg = std::conditional_t<
				std::is_same_v<T, string>,
				string_view,
				std::conditional_t<impl::is_one_of<T, double, int64_t, bool>, T, const T&>
			>;

			template <typename... U>
			TOML_NODISCARD_CTOR
			explicit value(U&&... args) TOML_MAY_THROW_UNLESS(std::is_nothrow_constructible_v<T, U &&...>)
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

			value(const value&) = delete;
			value& operator= (const value&) = delete;

			[[nodiscard]] node_type type() const noexcept override { return impl::node_type_of<T>; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_value() const noexcept override { return true; }
			[[nodiscard]] bool is_string() const noexcept override { return std::is_same_v<T, string>; }
			[[nodiscard]] bool is_integer() const noexcept override { return std::is_same_v<T, int64_t>; }
			[[nodiscard]] bool is_floating_point() const noexcept override { return std::is_same_v<T, double>; }
			[[nodiscard]] bool is_number() const noexcept override { return impl::is_one_of<T, int64_t, double>; }
			[[nodiscard]] bool is_boolean() const noexcept override { return std::is_same_v<T, bool>; }
			[[nodiscard]] bool is_date() const noexcept override { return std::is_same_v<T, date>; }
			[[nodiscard]] bool is_time() const noexcept override { return std::is_same_v<T, time>; }
			[[nodiscard]] bool is_date_time() const noexcept override { return std::is_same_v<T, date_time>; }
			[[nodiscard]] value<string>* as_string() noexcept override { return as_value<string>(this); }
			[[nodiscard]] value<int64_t>* as_integer() noexcept override { return as_value<int64_t>(this); }
			[[nodiscard]] value<double>* as_floating_point() noexcept override { return as_value<double>(this); }
			[[nodiscard]] value<bool>* as_boolean() noexcept override { return as_value<bool>(this); }
			[[nodiscard]] value<date>* as_date() noexcept override { return as_value<date>(this); }
			[[nodiscard]] value<time>* as_time() noexcept override { return as_value<time>(this); }
			[[nodiscard]] value<date_time>* as_date_time() noexcept override { return as_value<date_time>(this); }
			[[nodiscard]] const value<string>* as_string() const noexcept override { return as_value<string>(this); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept override { return as_value<int64_t>(this); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept override { return as_value<double>(this); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept override { return as_value<bool>(this); }
			[[nodiscard]] const value<date>* as_date() const noexcept override { return as_value<date>(this); }
			[[nodiscard]] const value<time>* as_time() const noexcept override { return as_value<time>(this); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept override { return as_value<date_time>(this); }
			[[nodiscard]] T& get() & noexcept { return val_; }
			[[nodiscard]] T&& get() && noexcept { return std::move(val_); }
			[[nodiscard]] const T& get() const & noexcept { return val_; }
			[[nodiscard]] T& operator* () & noexcept { return val_; }
			[[nodiscard]] T&& operator* () && noexcept { return std::move(val_); }
			[[nodiscard]] const T& operator* () const& noexcept { return val_; }
			[[nodiscard]] explicit operator T& () & noexcept { return val_; }
			[[nodiscard]] explicit operator T&& () && noexcept { return std::move(val_); }
			[[nodiscard]] explicit operator const T& () const& noexcept { return val_; }

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value& rhs) TOML_MAY_THROW
			{
				// this is the same behaviour as default_formatter, but it's so simple that there's
				// no need to spin up a new instance of it just for individual values.

				if constexpr (std::is_same_v<T, string>)
				{
					impl::print_to_stream('"', lhs);
					impl::print_to_stream_with_escapes(rhs.val_, lhs);
					impl::print_to_stream('"', lhs);
				}
				else
					impl::print_to_stream(rhs.val_, lhs);

				return lhs;
			}

			value& operator= (value_arg rhs) noexcept
			{
				if constexpr (std::is_same_v<T, string>)
					val_.assign(rhs);
				else
					val_ = rhs;
				return *this;
			}

			template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, string>>>
			value& operator= (string&& rhs) noexcept
			{
				val_ = std::move(rhs);
				return *this;
			}

			[[nodiscard]] friend bool operator == (const value& lhs, value_arg rhs) noexcept { return lhs.val_ == rhs; }
			TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, )
			[[nodiscard]] friend bool operator <  (const value& lhs, value_arg rhs) noexcept { return lhs.val_ < rhs; }
			[[nodiscard]] friend bool operator <  (value_arg lhs, const value& rhs) noexcept { return lhs < rhs.val_; }
			[[nodiscard]] friend bool operator <= (const value& lhs, value_arg rhs) noexcept { return lhs.val_ <= rhs; }
			[[nodiscard]] friend bool operator <= (value_arg lhs, const value& rhs) noexcept { return lhs <= rhs.val_; }
			[[nodiscard]] friend bool operator >  (const value& lhs, value_arg rhs) noexcept { return lhs.val_ > rhs; }
			[[nodiscard]] friend bool operator >  (value_arg lhs, const value& rhs) noexcept { return lhs > rhs.val_; }
			[[nodiscard]] friend bool operator >= (const value& lhs, value_arg rhs) noexcept { return lhs.val_ >= rhs; }
			[[nodiscard]] friend bool operator >= (value_arg lhs, const value& rhs) noexcept { return lhs >= rhs.val_; }

			template <typename U>
			[[nodiscard]] friend bool operator == (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ == rhs.val_;
				else
					return false;
			}

			template <typename U>
			[[nodiscard]] friend bool operator != (const value& lhs, const value<U>& rhs) noexcept
			{
				return !(lhs == rhs);
			}

			template <typename U>
			[[nodiscard]] friend bool operator < (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ < rhs.val_;
				else
					return impl::node_type_of<T> < impl::node_type_of<U>;
			}

			template <typename U>
			[[nodiscard]] friend bool operator <= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ <= rhs.val_;
				else
					return impl::node_type_of<T> <= impl::node_type_of<U>;
			}

			template <typename U>
			[[nodiscard]] friend bool operator > (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ > rhs.val_;
				else
					return impl::node_type_of<T> > impl::node_type_of<U>;
			}

			template <typename U>
			[[nodiscard]] friend bool operator >= (const value& lhs, const value<U>& rhs) noexcept
			{
				if constexpr (std::is_same_v<T, U>)
					return lhs.val_ >= rhs.val_;
				else
					return impl::node_type_of<T> >= impl::node_type_of<U>;
			}
	};

	template <size_t N> value(const string_char(&)[N]) -> value<string>;
	template <size_t N> value(const string_char(&&)[N]) -> value<string>;
	value(const string_char*) -> value<string>;
	template <size_t N> value(string_char(&)[N]) -> value<string>;
	template <size_t N> value(string_char(&&)[N]) -> value<string>;
	value(string_char*) -> value<string>;
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
	value(date) -> value<date>;
	value(time) -> value<time>;
	value(date_time) -> value<date_time>;
	#ifdef TOML_SMALL_FLOAT_TYPE
	value(TOML_SMALL_FLOAT_TYPE) -> value<double>;
	#endif
	#ifdef TOML_SMALL_INT_TYPE
	value(TOML_SMALL_INT_TYPE) -> value<int64_t>;
	#endif
}
TOML_END

#pragma endregion
//-----------------  ↑ toml_value.h  -----------------------------------------------------------------------------------

//------------------------------------------  ↓ toml_array.h  ----------------------------------------------------------
#pragma region

TOML_IMPL_START
{
	template <bool is_const>
	class array_iterator final
	{
		private:
			friend class toml::array;

			using raw_iterator = std::conditional_t<
				is_const,
				std::vector<std::unique_ptr<node>>::const_iterator,
				std::vector<std::unique_ptr<node>>::iterator
			>;

			mutable raw_iterator raw_;

			array_iterator(const raw_iterator& raw) noexcept
				: raw_{ raw }
			{}

			array_iterator(raw_iterator&& raw) noexcept
				: raw_{ std::move(raw) }
			{}

		public:

			using value_type = std::conditional_t<is_const, const node, node>;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = ptrdiff_t;

			array_iterator() noexcept = default;

			array_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				return *this;
			}

			array_iterator operator++(int) noexcept // post++
			{
				array_iterator out{ raw_ };
				++raw_;
				return out;
			}

			array_iterator& operator--() noexcept // --pre
			{
				--raw_;
				return *this;
			}

			array_iterator operator--(int) noexcept // post--
			{
				array_iterator out{ raw_ };
				--raw_;
				return out;
			}

			[[nodiscard]]
			reference operator * () const noexcept
			{
				return *raw_->get();
			}

			[[nodiscard]]
			pointer operator -> () const noexcept
			{
				return raw_->get();
			}

			array_iterator& operator += (ptrdiff_t rhs) noexcept
			{
				raw_ += rhs;
				return *this;
			}

			array_iterator& operator -= (ptrdiff_t rhs) noexcept
			{
				raw_ -= rhs;
				return *this;
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ + rhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (ptrdiff_t lhs, const array_iterator& rhs) noexcept
			{
				return { rhs.raw_ + lhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator - (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ - rhs };
			}

			[[nodiscard]]
			friend constexpr ptrdiff_t operator - (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ - rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator == (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ == rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator != (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ != rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator < (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ < rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator <= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ <= rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator > (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ > rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator >= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ >= rhs.raw_;
			}

			[[nodiscard]]
			reference operator[] (ptrdiff_t idx) const noexcept
			{
				return *(raw_ + idx)->get();
			}
	};

	template <typename T>
	[[nodiscard]] TOML_ALWAYS_INLINE
	auto make_node(T&& val) noexcept
	{
		using type = impl::unwrapped<remove_cvref_t<T>>;
		if constexpr (is_one_of<type, array, table>)
		{
			static_assert(
				std::is_rvalue_reference_v<decltype(val)>,
				"Tables and arrays may only be moved (not copied)."
			);
			return new type{ std::forward<T>(val) };
		}
		else
		{
			static_assert(
				is_value_or_promotable<type>,
				"Value initializers must be (or be promotable to) one of the TOML value types"
			);
			return new value{ std::forward<T>(val) };
		}
	}
}
TOML_IMPL_END

TOML_START
{
	[[nodiscard]] bool operator == (const table& lhs, const table& rhs) noexcept;

	class array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::unique_ptr<node>> values;

			void preinsertion_resize(size_t idx, size_t count) noexcept
			{
				const auto new_size = values.size() + count;
				const auto inserting_at_end = idx == values.size();
				values.resize(new_size);
				if (!inserting_at_end)
				{
					for (size_t r = new_size, e = idx + count, l = e; r --> e; l--)
						values[r] = std::move(values[l]);
				}
			}

		public:

			using value_type = node;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using reference = node&;
			using const_reference = const node&;
			using iterator = impl::array_iterator<false>;
			using const_iterator = impl::array_iterator<true>;

			TOML_NODISCARD_CTOR
			array() noexcept = default;

			TOML_NODISCARD_CTOR
			array(array&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) }
			{}

			template <typename U, typename... V>
			TOML_NODISCARD_CTOR
			explicit array(U&& val, V&&... vals) TOML_MAY_THROW
			{
				values.reserve(sizeof...(V) + 1_sz);
				values.emplace_back(impl::make_node(std::forward<U>(val)));
				if constexpr (sizeof...(V) > 0)
				{
					(
						values.emplace_back(impl::make_node(std::forward<V>(vals))),
						...
					);
				}
			}

			array& operator= (array&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				return *this;
			}

			array(const array&) = delete;
			array& operator= (const array&) = delete;

			[[nodiscard]] node_type type() const noexcept override { return node_type::array; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return true; }
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] array* as_array() noexcept override { return this; }
			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			template <typename T = void>
			[[nodiscard]] bool is_homogeneous() const noexcept
			{
				if (values.empty())
					return false;

				if constexpr (std::is_same_v<T, void>)
				{
					const auto type = values[0]->type();
					for (size_t i = 1; i < values.size(); i++)
						if (values[i]->type() != type)
							return false;
				}
				else
				{
					for (auto& v : values)
						if (!v->is<T>())
							return false;
				}
				return true;
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is_array_of_tables() const noexcept override
			{
				return is_homogeneous<toml::table>();
			}

			[[nodiscard]] node& operator[] (size_t index) noexcept { return *values[index]; }
			[[nodiscard]] const node& operator[] (size_t index) const noexcept { return *values[index]; }
			[[nodiscard]] node& front() noexcept { return *values.front(); }
			[[nodiscard]] const node& front() const noexcept { return *values.front(); }
			[[nodiscard]] node& back() noexcept { return *values.back(); }
			[[nodiscard]] const node& back() const noexcept { return *values.back(); }
			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }
			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }
			[[nodiscard]] bool empty() const noexcept { return values.empty(); }
			[[nodiscard]] size_t size() const noexcept { return values.size(); }
			void reserve(size_t new_capacity) TOML_MAY_THROW { values.reserve(new_capacity); }
			void clear() noexcept { values.clear(); }

			template <typename U>
			iterator insert(const_iterator pos, U&& val) noexcept
			{
				return { values.emplace(pos.raw_, impl::make_node(std::forward<U>(val))) };
			}

			template <typename U>
			iterator insert(const_iterator pos, size_t count, U&& val) noexcept
			{
				switch (count)
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, std::forward<U>(val));
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, count);
						size_t i = start_idx;
						for (size_t e = start_idx + count - 1_sz; i < e; i++)
							values[i].reset(impl::make_node(val));

						values[i].reset(impl::make_node(std::forward<U>(val)));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			template <typename ITER>
			iterator insert(const_iterator pos, ITER first, ITER last) noexcept
			{
				const auto count = std::distance(first, last);
				switch (count)
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, *first);
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, count);
						size_t i = start_idx;
						for (auto it = first; it != last; it++)
							values[i].reset(impl::make_node(*it));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			template <typename U>
			iterator insert(const_iterator pos, std::initializer_list<U> ilist) noexcept
			{
				switch (ilist.size())
				{
					case 0: return { values.begin() + (pos.raw_ - values.cbegin()) };
					case 1: return insert(pos, *ilist.begin());
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - values.cbegin());
						preinsertion_resize(start_idx, ilist.size());
						size_t i = start_idx;
						for (auto& val : ilist)
							values[i].reset(impl::make_node(val));
						return { values.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			template <typename U, typename... V>
			iterator emplace(const_iterator pos, V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return { values.emplace(pos.raw_, new impl::node_of<type>{ std::forward<V>(args)...} ) };
			}

			iterator erase(const_iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}

			iterator erase(const_iterator first, const_iterator last) noexcept
			{
				return { values.erase(first.raw_, last.raw_) };
			}

			template <typename U>
			decltype(auto) push_back(U&& val) noexcept
			{
				auto nde = impl::make_node(std::forward<U>(val));
				values.emplace_back(nde);
				return *nde;
			}

			template <typename U, typename... V>
			decltype(auto) emplace_back(V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				auto nde = new impl::node_of<type>{ std::forward<V>(args)... };
				values.emplace_back(nde);
				return *nde;
			}

			void pop_back() noexcept
			{
				values.pop_back();
			}

			[[nodiscard]] node* get(size_t index) noexcept
			{
				return index < values.size() ? values[index].get() : nullptr;
			}

			[[nodiscard]] const node* get(size_t index) const noexcept
			{
				return index < values.size() ? values[index].get() : nullptr;
			}

			template <typename T>
			[[nodiscard]] impl::node_of<T>* get_as(size_t index) noexcept
			{
				if (auto val = get(index))
					return val->as<T>();
				return nullptr;
			}

			template <typename T>
			[[nodiscard]] const impl::node_of<T>* get_as(size_t index) const noexcept
			{
				if (auto val = get(index))
					return val->as<T>();
				return nullptr;
			}

			[[nodiscard]] friend bool operator == (const array& lhs, const array& rhs) noexcept
			{
				if (&lhs == &rhs)
					return true;
				if (lhs.values.size() != rhs.values.size())
					return false;
				for (size_t i = 0, e = lhs.values.size(); i < e; i++)
				{
					const auto lhs_type = lhs.values[i]->type();
					const node& rhs_ = *rhs.values[i];
					const auto rhs_type = rhs_.type();
					if (lhs_type != rhs_type)
						return false;

					const bool equal = lhs.values[i]->visit([&](const auto& lhs_) noexcept
					{
						return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_);
					});
					if (!equal)
						return false;
				}
				return true;
			}

			[[nodiscard]] friend bool operator != (const array& lhs, const array& rhs) noexcept
			{
				return !(lhs == rhs);
			}

		private:

			template <typename T>
			[[nodiscard]] static bool container_equality(const array& lhs, const T& rhs) noexcept
			{
				using elem_t = std::remove_const_t<typename T::value_type>;
				static_assert(
					impl::is_value_or_promotable<elem_t>,
					"Container element type must be (or be promotable to) one of the TOML value types"
				);

				if (lhs.size() != rhs.size())
					return false;
				if (rhs.size() == 0_sz)
					return true;

				size_t i{};
				for (auto& list_elem : rhs)
				{
					const auto elem = lhs.get_as<impl::promoted<elem_t>>(i++);
					if (!elem || *elem != list_elem)
						return false;
				}

				return true;
			}

			[[nodiscard]] size_t total_leaf_count() const noexcept
			{
				size_t leaves{};
				for (size_t i = 0, e = values.size(); i < e; i++)
				{
					auto arr = values[i]->as_array();
					leaves += arr ? arr->total_leaf_count() : 1_sz;
				}
				return leaves;
			}

			void flatten_child(array&& child, size_t& dest_index) noexcept
			{
				for (size_t i = 0, e = child.size(); i < e; i++)
				{
					auto type = child.values[i]->type();
					if (type == node_type::array)
					{
						array& arr = *reinterpret_cast<array*>(child.values[i].get());
						if (!arr.empty())
							flatten_child(std::move(arr), dest_index);
					}
					else
						values[dest_index++] = std::move(child.values[i]);
				}
			}

		public:

			template <typename T>
			[[nodiscard]] friend bool operator == (const array& lhs, const std::initializer_list<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::initializer_list<T>&, template <typename T>)

			template <typename T>
			[[nodiscard]] friend bool operator == (const array& lhs, const std::vector<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>)
			void flatten() TOML_MAY_THROW
			{
				if (values.empty())
					return;

				bool requires_flattening = false;
				size_t size_after_flattening = values.size();
				for (size_t i = values.size(); i --> 0_sz;)
				{
					auto arr = values[i]->as_array();
					if (!arr)
						continue;
					size_after_flattening--; //discount the array itself
					const auto leaf_count = arr->total_leaf_count();
					if (leaf_count > 0_sz)
					{
						requires_flattening = true;
						size_after_flattening += leaf_count;
					}
					else
						values.erase(values.cbegin() + static_cast<ptrdiff_t>(i));
				}

				if (!requires_flattening)
					return;

				values.reserve(size_after_flattening);

				size_t i = 0;
				while (i < values.size())
				{
					auto arr = values[i]->as_array();
					if (!arr)
					{
						i++;
						continue;
					}

					std::unique_ptr<node> arr_storage = std::move(values[i]);
					const auto leaf_count = arr->total_leaf_count();
					if (leaf_count > 1_sz)
						preinsertion_resize(i + 1_sz, leaf_count - 1_sz);
					flatten_child(std::move(*arr), i); //increments i
				}
			}

			template <typename CHAR>
			friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&) TOML_MAY_THROW;
	};
}
TOML_END

#pragma endregion
//------------------------------------------  ↑ toml_array.h  ----------------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_table.h  ---------------------------------
#pragma region

TOML_IMPL_START
{
	template <bool is_const>
	struct table_proxy_pair final
	{
		using value_type = std::conditional_t<is_const, const node, node>;

		const string& key;
		value_type& value;
	};

	template <bool is_const>
	class table_iterator final
	{
		private:
			friend class toml::table;

			using raw_iterator = std::conditional_t<
				is_const,
				string_map<std::unique_ptr<node>>::const_iterator,
				string_map<std::unique_ptr<node>>::iterator
			>;

			mutable raw_iterator raw_;

			table_iterator(const raw_iterator& raw) noexcept
				: raw_{ raw }
			{}

			table_iterator(raw_iterator&& raw) noexcept
				: raw_{ std::move(raw) }
			{}

		public:

			table_iterator() noexcept = default;

			using reference = table_proxy_pair<is_const>;
			using difference_type = ptrdiff_t;

			table_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				return *this;
			}

			table_iterator operator++(int) noexcept // post++
			{
				table_iterator out{ raw_ };
				++raw_;
				return out;
			}

			table_iterator& operator--() noexcept // --pre
			{
				--raw_;
				return *this;
			}

			table_iterator operator--(int) noexcept // post--
			{
				table_iterator out{ raw_ };
				--raw_;
				return out;
			}

			reference operator* () const noexcept
			{
				return { raw_->first, *raw_->second.get() };
			}

			[[nodiscard]]
			friend constexpr bool operator == (const table_iterator& lhs, const table_iterator& rhs) noexcept
			{
				return lhs.raw_ == rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator != (const table_iterator& lhs, const table_iterator& rhs) noexcept
			{
				return lhs.raw_ != rhs.raw_;
			}
	};

	struct table_init_pair final
	{
		string key;
		std::unique_ptr<node> value;

		template <typename T>
		table_init_pair(string&& k, T && v) noexcept
			: key{ std::move(k) },
			value{ make_node(std::forward<T>(v)) }
		{}

		template <typename T>
		table_init_pair(string_view k, T&& v) noexcept
			: key{ k },
			value{ make_node(std::forward<T>(v)) }
		{}

		template <typename T>
		table_init_pair(const string_char* k, T&& v) noexcept
			: key{ k },
			value{ make_node(std::forward<T>(v)) }
		{}
	};
}
TOML_IMPL_END

TOML_START
{
	class table final
		: public node
	{
		private:
			friend class impl::parser;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

		public:

			using iterator = impl::table_iterator<false>;
			using const_iterator = impl::table_iterator<true>;

			TOML_NODISCARD_CTOR
			table() noexcept {}

			template <size_t N>
			TOML_NODISCARD_CTOR
			explicit table(impl::table_init_pair(&& arr)[N]) noexcept
			{
				for (auto&& kvp : arr)
				{
					values.insert_or_assign(
						std::move(kvp.key),
						std::move(kvp.value)
					);
				}
			}

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

			table(const table&) = delete;
			table& operator= (const table&) = delete;

			[[nodiscard]] node_type type() const noexcept override { return node_type::table; }
			[[nodiscard]] bool is_table() const noexcept override { return true; }
			[[nodiscard]] bool is_array() const noexcept override { return false; }
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] table* as_table() noexcept override { return this; }
			[[nodiscard]] const table* as_table() const noexcept override { return this; }
			[[nodiscard]] bool is_inline() const noexcept { return inline_; }
			void is_inline(bool val) noexcept { inline_ = val; }
			[[nodiscard]] inline node_view<node> operator[] (string_view key) noexcept;
			[[nodiscard]] inline node_view<const node> operator[] (string_view key) const noexcept;
			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }
			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }
			[[nodiscard]] bool empty() const noexcept { return values.empty(); }
			[[nodiscard]] size_t size() const noexcept { return values.size(); }
			void clear() noexcept { values.clear(); }

			template <typename K, typename V, typename = std::enable_if_t<
				std::is_convertible_v<K&&, string_view>
			>>
			std::pair<iterator, bool> insert(K&& key, V&& val) noexcept
			{
				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(ipos, std::forward<K>(key), impl::make_node(std::forward<V>(val)));
					return { ipos, true };
				}
				return { ipos, false };
			}

			template <typename ITER, typename = std::enable_if_t<
				!std::is_convertible_v<ITER&&, string_view>
			>>
			void insert(ITER first, ITER last) noexcept
			{
				if (first == last)
					return;
				for (auto it = first; it != last; it++)
				{
					if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
						insert(std::move((*it).first), std::move((*it).second));
					else
						insert((*it).first, (*it).second);
				}
			}

			template <typename K, typename V>
			std::pair<iterator, bool> insert_or_assign(K&& key, V&& val) noexcept
			{
				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(ipos, std::forward<K>(key), impl::make_node(std::forward<V>(val)));
					return { ipos, true };
				}
				else
				{
					(*ipos).second.reset(impl::make_node(std::forward<V>(val)));
					return { ipos, false };
				}
			}

			template <typename U, typename K, typename... V>
			std::pair<iterator, bool> emplace(K&& key, V&&... args) noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Emplacement type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				auto ipos = values.lower_bound(key);
				if (ipos == values.end() || ipos->first != key)
				{
					ipos = values.emplace_hint(
						ipos,
						std::forward<K>(key),
						new impl::node_of<type>{ std::forward<V>(args)... }
					);
					return { ipos, true };
				}
				return { ipos, false };
			}

			iterator erase(iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}

			iterator erase(const_iterator pos) noexcept
			{
				return { values.erase(pos.raw_) };
			}

			iterator erase(const_iterator first, const_iterator last) noexcept
			{
				return { values.erase(first.raw_, last.raw_) };
			}

			bool erase(string_view key) noexcept
			{
				if (auto it = values.find(key); it != values.end())
				{
					values.erase(it);
					return true;
				}
				return false;
			}

		private:

			template <typename MAP, typename KEY>
			[[nodiscard]] static auto do_get(MAP& vals, const KEY& key) noexcept
			{
				using return_type = std::conditional_t<
					std::is_const_v<MAP>,
					const node*,
					node*
				>;

				if (auto it = vals.find(key); it != vals.end())
					return return_type{ it->second.get() };
				return return_type{};
			}

			template <typename T, typename MAP, typename KEY>
			[[nodiscard]] static auto do_get_as(MAP& vals, const KEY& key) noexcept
			{
				const auto node = do_get(vals, key);
				return node ? node->template as<T>() : nullptr;
			}

			template <typename MAP, typename KEY>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static bool do_contains(MAP& vals, const KEY& key) noexcept
			{
				#if TOML_CPP >= 20
				return vals.contains(key);
				#else
				return do_get(vals, key) != nullptr;
				#endif
			}

		public:

			[[nodiscard]] node* get(string_view key) noexcept { return do_get(values, key); }
			[[nodiscard]] const node* get(string_view key) const noexcept { return do_get(values, key); }
			[[nodiscard]] iterator find(string_view key) noexcept { return { values.find(key) }; }
			[[nodiscard]] const_iterator find(string_view key) const noexcept { return { values.find(key) }; }

			template <typename T>
			[[nodiscard]] impl::node_of<T>* get_as(string_view key) noexcept { return do_get_as<T>(values, key); }

			template <typename T>
			[[nodiscard]] const impl::node_of<T>* get_as(string_view key) const noexcept { return do_get_as<T>(values, key); }
			[[nodiscard]] bool contains(string_view key) const noexcept { return do_contains(values, key); }
			[[nodiscard]] friend bool operator == (const table& lhs, const table& rhs) noexcept
			{
				if (&lhs == &rhs)
					return true;
				if (lhs.values.size() != rhs.values.size())
					return false;

				for (auto l = lhs.values.begin(), r = rhs.values.begin(), e = lhs.values.end(); l != e; l++, r++)
				{
					if (l->first != r->first)
						return false;

					const auto lhs_type = l->second->type();
					const node& rhs_ = *r->second;
					const auto rhs_type = rhs_.type();
					if (lhs_type != rhs_type)
						return false;

					const bool equal = l->second->visit([&](const auto& lhs_) noexcept
					{
						return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_);
					});
					if (!equal)
						return false;
				}
				return true;
			}

			[[nodiscard]] friend bool operator != (const table& lhs, const table& rhs) noexcept
			{
				return !(lhs == rhs);
			}

			template <typename CHAR>
			friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const table&) TOML_MAY_THROW;
	};
}
TOML_END

#pragma endregion
//-------------------------------------------------------------------  ↑ toml_table.h  ---------------------------------

//------------------------------------------------------------------------------------------  ↓ toml_node_view.h  ------
#pragma region

TOML_START
{
	template <typename T>
	class node_view final
	{
		public:
			using viewed_type = T;

		private:
			friend class toml::table;

			viewed_type* node_;

			TOML_NODISCARD_CTOR
			node_view(viewed_type* node) noexcept
				: node_{ node }
			{}

		public:

			[[nodiscard]] explicit operator bool() const noexcept { return node_ != nullptr; }
			[[nodiscard]] viewed_type* get() noexcept { return node_; }
			[[nodiscard]] const viewed_type* get() const noexcept { return node_; }

			[[nodiscard]] node_type type() const noexcept { return node_ ? node_->type() : node_type::none; }
			[[nodiscard]] bool is_table() const noexcept { return node_ && node_->is_table(); }
			[[nodiscard]] bool is_array() const noexcept { return node_ && node_->is_array(); }
			[[nodiscard]] bool is_value() const noexcept { return node_ && node_->is_value(); }
			[[nodiscard]] bool is_string() const noexcept { return node_ && node_->is_string(); }
			[[nodiscard]] bool is_integer() const noexcept { return node_ && node_->is_integer(); }
			[[nodiscard]] bool is_floating_point() const noexcept { return node_ && node_->is_floating_point(); }
			[[nodiscard]] bool is_number() const noexcept { return node_ && node_->is_number(); }
			[[nodiscard]] bool is_boolean() const noexcept { return node_ && node_->is_boolean(); }
			[[nodiscard]] bool is_date() const noexcept { return node_ && node_->is_date(); }
			[[nodiscard]] bool is_time() const noexcept { return node_ && node_->is_time(); }
			[[nodiscard]] bool is_date_time() const noexcept { return node_ && node_->is_date_time(); }
			[[nodiscard]] bool is_array_of_tables() const noexcept { return node_ && node_->is_array_of_tables(); }

			template <typename U>
			[[nodiscard]]
			bool is() const noexcept
			{
				return node_ ? node_->template is<U>() : false;
			}

			template <typename U>
			[[nodiscard]]
			auto as() noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			template <typename U>
			[[nodiscard]]
			const impl::node_of<U>* as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			[[nodiscard]] auto as_table() noexcept { return as<table>(); }
			[[nodiscard]] auto as_array() noexcept { return as<array>(); }
			[[nodiscard]] auto as_string() noexcept { return as<string>(); }
			[[nodiscard]] auto as_integer() noexcept { return as<int64_t>(); }
			[[nodiscard]] auto as_floating_point() noexcept { return as<double>(); }
			[[nodiscard]] auto as_boolean() noexcept { return as<bool>(); }
			[[nodiscard]] auto as_date() noexcept { return as<date>(); }
			[[nodiscard]] auto as_time() noexcept { return as<time>(); }
			[[nodiscard]] auto as_date_time() noexcept { return as<date_time>(); }
			[[nodiscard]] const table* as_table() const noexcept { return as<table>(); }
			[[nodiscard]] const array* as_array() const noexcept { return as<array>(); }
			[[nodiscard]] const value<string>* as_string() const noexcept { return as<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept { return as<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept { return as<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept { return as<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept { return as<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept { return as<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept { return as<date_time>(); }

			template <typename U>
			[[nodiscard]] auto value_or(U&& default_value) const noexcept
			{
				static_assert(
					impl::is_value_or_promotable<impl::remove_cvref_t<U>>,
					"Default value type must be (or be promotable to) one of the TOML value types"
				);

				using value_type = impl::promoted<impl::remove_cvref_t<U>>;
				using return_type = std::conditional_t<
					std::is_same_v<value_type, string>,
					std::conditional_t<std::is_same_v<impl::remove_cvref_t<U>, string>, string, string_view>,
					value_type
				>;
				if (auto val = node_ ? node_->template as<value_type>() : nullptr)
					return return_type{ **val };
				return return_type{ std::forward<U>(default_value) };
			}

		private:

			template <typename N, typename FUNC>
			static decltype(auto) do_visit(N* node, FUNC&& visitor)
				TOML_MAY_THROW_UNLESS(noexcept(std::declval<N*>()->visit(std::declval<FUNC&&>())))
			{
				using return_type = decltype(node->visit(std::forward<FUNC>(visitor)));
				if (node)
					return node->visit(std::forward<FUNC>(visitor));
				if constexpr (!std::is_void_v<return_type>)
					return return_type{};
			}

			template <typename FUNC, typename N>
			static constexpr bool visit_is_nothrow =
				noexcept(do_visit(std::declval<N*>(), std::declval<FUNC&&>()));

		public:

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor)
				TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, viewed_type>)
			{
				return do_visit(node_, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const
				TOML_MAY_THROW_UNLESS(visit_is_nothrow<FUNC&&, const viewed_type>)
			{
				return do_visit(node_, std::forward<FUNC>(visitor));
			}

			[[nodiscard]] friend bool operator == (const node_view& lhs, const table& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto tbl = lhs.as<table>();
				return tbl && *tbl == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const table&, )
			[[nodiscard]] friend bool operator == (const node_view& lhs, const array& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const array&, )

			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const value<U>& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto val = lhs.as<U>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const value<U>&, template <typename U>)

			template <typename U, typename = std::enable_if_t<impl::is_value_or_promotable<U>>>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const U& rhs) noexcept
			{
				const auto val = lhs.as<impl::promoted<U>>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(
				const node_view&,
				const U&,
				template <typename U, typename = std::enable_if_t<impl::is_value_or_promotable<U>>>
			)

			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const std::initializer_list<U>& rhs) noexcept
			{
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::initializer_list<U>&, template <typename U>)

			template <typename U>
			[[nodiscard]] friend bool operator == (const node_view& lhs, const std::vector<U>& rhs) noexcept
			{
				const auto arr = lhs.as<array>();
				return arr && *arr == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::vector<U>&, template <typename U>)
			[[nodiscard]] node_view<viewed_type> operator[] (string_view key) noexcept
			{
				if (auto tbl = this->as_table())
					return { tbl->get(key) };
				return { nullptr };
			}

			[[nodiscard]] node_view<viewed_type> operator[] (size_t index) noexcept
			{
				if (auto tbl = this->as_array())
					return { tbl->get(index) };
				return { nullptr };
			}

			[[nodiscard]] node_view<const viewed_type> operator[] (string_view key) const noexcept
			{
				if (auto tbl = this->as_table())
					return { tbl->get(key) };
				return { nullptr };
			}

			[[nodiscard]] node_view<const viewed_type> operator[] (size_t index) const noexcept
			{
				if (auto tbl = this->as_array())
					return { tbl->get(index) };
				return { nullptr };
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& os, const node_view& nv) TOML_MAY_THROW
			{
				if (nv.node_)
				{
					nv.node_->visit([&os](const auto& n) TOML_MAY_THROW
					{
						os << n;
					});
				}
				return os;
			}
	};

	inline node_view<node> table::operator[] (string_view key) noexcept
	{
		return { this->get(key) };
	}

	inline node_view<const node> table::operator[] (string_view key) const noexcept
	{
		return { this->get(key) };
	}
}
TOML_END

#pragma endregion
//------------------------------------------------------------------------------------------  ↑ toml_node_view.h  ------

//------------------  ↓ toml_utf8.h  -----------------------------------------------------------------------------------
#pragma region

#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687

#define TOML_ASSUME_CODEPOINT_BETWEEN(first, last)	\
	TOML_ASSUME(codepoint >= first);				\
	TOML_ASSUME(codepoint <= last)

TOML_IMPL_START
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
TOML_IMPL_END

#undef TOML_ASSUME_CODEPOINT_BETWEEN

#endif // TOML_LANG_HIGHER_THAN(0, 5, 0)

TOML_IMPL_START
{
	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_whitespace(char32_t codepoint) noexcept
	{
		return codepoint == U'\t' || codepoint == U' ';
	}

	[[nodiscard]]
	constexpr bool is_unicode_whitespace(char32_t codepoint) noexcept
	{
		// see: https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that don't say "is a line-break")

		return codepoint == U'\u00A0' // no-break space
			|| codepoint == U'\u1680' // ogham space mark
			|| (codepoint >= U'\u2000' && codepoint <= U'\u200A') // em quad -> hair space
			|| codepoint == U'\u202F' // narrow no-break space
			|| codepoint == U'\u205F' // medium mathematical space
			|| codepoint == U'\u3000' // ideographic space
		;
	}

	[[nodiscard]]
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
		return is_ascii_whitespace(codepoint) || is_unicode_whitespace(codepoint);
	}

	template <bool CR = true>
	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_line_break(char32_t codepoint) noexcept
	{
		constexpr auto low_range_end = CR ? U'\r' : U'\f';
		return (codepoint >= U'\n' && codepoint <= low_range_end);
	}

	[[nodiscard]]
	constexpr bool is_unicode_line_break(char32_t codepoint) noexcept
	{
		// see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that say "is a line-break")

		return codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;
	}

	template <bool CR = true>
	[[nodiscard]]
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		return is_ascii_line_break<CR>(codepoint) || is_unicode_line_break(codepoint);
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_string_delimiter(char32_t codepoint) noexcept
	{
		return codepoint == U'"' || codepoint == U'\'';
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

	[[nodiscard]]
	constexpr bool is_hexadecimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'f')
			|| (codepoint >= U'A' && codepoint <= U'F')
			|| is_decimal_digit(codepoint)
		;
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
		return is_ascii_line_break(codepoint)
			|| is_ascii_whitespace(codepoint)
			|| codepoint == U']'
			|| codepoint == U'}'
			|| codepoint == U','
			|| codepoint == U'#'
			|| is_unicode_line_break(codepoint)
			|| is_unicode_whitespace(codepoint)
		;
	}

	struct utf8_decoder final
	{

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
					&& static_cast<uint8_t>(source[0]) == static_cast<uint8_t>(0xEFu)
					&& static_cast<uint8_t>(source[1]) == static_cast<uint8_t>(0xBBu)
					&& static_cast<uint8_t>(source[2]) == static_cast<uint8_t>(0xBFu))
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
						0xEF,
						0xBB,
						0xBF
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
		string_char bytes[4];
		source_position position;

		template <typename CHAR = string_char>
		[[nodiscard]] TOML_ALWAYS_INLINE
		std::basic_string_view<CHAR> as_view() const noexcept
		{
			static_assert(
				sizeof(CHAR) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(bytes), 4_sz }
				: std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(bytes) };
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
		#define TOML_ERROR			throw parse_error
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR			err.emplace
	#endif

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const source_path_ptr& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() TOML_MAY_THROW = 0;

		#if !TOML_EXCEPTIONS

		[[nodiscard]]
		virtual std::optional<parse_error>&& error() noexcept = 0;

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
			source_path_ptr source_path_;
			#if !TOML_EXCEPTIONS
			std::optional<parse_error> err;
			#endif

		public:

			template <typename U, typename STR = std::string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				TOML_MAY_THROW_UNLESS(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				current.position = { 1, 1 };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<STR>(source_path));
			}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
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
							throw parse_error{ exc.what(), prev.position, source_path_ };
						}
						catch (...)
						{
							throw parse_error{ "An unspecified error occurred", prev.position, source_path_ };
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

					current.bytes[current_byte_count++] = static_cast<string_char>(*nextByte);
					if (decoder.has_code_point())
					{
						current.value = decoder.codepoint;
						prev = current;
						std::memset(current.bytes, 0, sizeof(current.bytes));
						current_byte_count = {};

						if (is_line_break<false>(prev.value))
						{
							current.position.line++;
							current.position.column = 1;
						}
						else
							current.position.column++;

						return &prev;
					}
				}
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			std::optional<parse_error>&& error() noexcept override
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
			static constexpr size_t max_history_length = 64;

		private:
			static constexpr size_t history_buffer_size = max_history_length - 1; //'head' is stored in the reader
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
			const source_path_ptr& source_path() const noexcept override
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
			std::optional<parse_error>&& error() noexcept override
			{
				return reader.error();
			}

			#endif
	};

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
}
TOML_IMPL_END

#pragma endregion
//------------------  ↑ toml_utf8.h  -----------------------------------------------------------------------------------

//------------------------------------------  ↓ toml_parser.h  ---------------------------------------------------------
#pragma region

TOML_START
{
	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

	class parse_result final
	{
		private:
			std::aligned_storage_t<
				(sizeof(table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(table)),
				(alignof(table) < alignof(parse_error) ? alignof(parse_error) : alignof(table))
			> storage;
			bool is_err;
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

	#else

	using parse_result = table;

	#endif
}
TOML_END

TOML_IMPL_START
{
	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK(...)	(void)0
		#define TOML_ERROR				throw toml::parse_error
	#else
		#define TOML_ERROR_CHECK(...)	if (err) return __VA_ARGS__
		#define TOML_ERROR				err.emplace
	#endif

	#if !TOML_EXCEPTIONS || defined(__INTELLISENSE__)
		#define TOML_NORETURN
	#else
		#define TOML_NORETURN			[[noreturn]]
	#endif

	template <int> struct parse_integer_traits;
	template <> struct parse_integer_traits<2> final
	{
		static constexpr auto qualifier = "binary"sv;
		static constexpr auto is_digit = is_binary_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 63;
		static constexpr char32_t prefix_codepoint = U'b';
		static constexpr char prefix = 'b';
	};
	template <> struct parse_integer_traits<8> final
	{
		static constexpr auto qualifier = "octal"sv;
		static constexpr auto is_digit = is_octal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 21; // strlen("777777777777777777777")
		static constexpr char32_t prefix_codepoint = U'o';
		static constexpr char prefix = 'o';
	};
	template <> struct parse_integer_traits<10> final
	{
		static constexpr auto qualifier = "decimal"sv;
		static constexpr auto is_digit = is_decimal_digit;
		static constexpr auto is_signed = true;
		static constexpr auto buffer_length = 19; //strlen("9223372036854775807")
	};
	template <> struct parse_integer_traits<16> final
	{
		static constexpr auto qualifier = "hexadecimal"sv;
		static constexpr auto is_digit = is_hexadecimal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 16; //strlen("7FFFFFFFFFFFFFFF")
		static constexpr char32_t prefix_codepoint = U'x';
		static constexpr char prefix = 'x';
	};

	class parser final
	{
		private:
			utf8_buffered_reader reader;
			table root;
			source_position prev_pos = { 1, 1 };
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
			source_position current_position(source_index fallback_offset = 0) const noexcept
			{
				if (cp)
					return cp->position;
				return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
			}

			template <typename... T>
			TOML_NORETURN
			void abort_with_error(T &&... args) const TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();

				if constexpr (sizeof...(T) == 0_sz)
					TOML_ERROR( "An unspecified error occurred", current_position(1), reader.source_path() );
				else
				{
					static constexpr auto buf_size = 512_sz;
					TOML_GCC_ATTR(uninitialized) char buf[buf_size];
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
							toml::string_view cp_view;
							if (arg.value <= U'\x1F') TOML_UNLIKELY
								cp_view = low_character_escape_table[arg.value];
							else if (arg.value == U'\x7F')  TOML_UNLIKELY
								cp_view = TOML_STRING_PREFIX("\\u007F"sv);
							else
								cp_view = arg.template as_view<string_char>();

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
							const auto str = impl::node_type_friendly_names[
								static_cast<std::underlying_type_t<node_type>>(arg)
							];
							std::memcpy(ptr, str.data(), str.length());
							ptr += str.length();
						}
						else if constexpr (std::is_floating_point_v<arg_t>)
						{
							#if TOML_USE_STREAMS_FOR_FLOATS
							{
								std::ostringstream oss;
								oss.precision(std::numeric_limits<arg_t>::digits10 + 1);
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
					#if TOML_EXCEPTIONS
						TOML_ERROR(buf, current_position(1), reader.source_path());
					#else
						TOML_ERROR(std::string(buf, static_cast<size_t>(ptr - buf)), current_position(1), reader.source_path());
					#endif
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
							"Encountered EOF while parsing "sv,
							(MULTI_LINE ? "multi-line "sv : ""sv),
							node_type::string
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

							#if 0
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
							#endif

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

									if (!is_hexadecimal_digit(*cp))
										abort_with_error(
											"Encountered unexpected character while parsing "sv,
											(MULTI_LINE ? "multi-line "sv : ""sv),
											node_type::string,
											"; expected hex digit, saw '\\"sv, *cp, "'"sv
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
									abort_with_error(
										"Encountered unknown unicode scalar sequence while parsing "sv,
										(MULTI_LINE ? "multi-line "sv : ""sv),
										node_type::string
									);

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
									"Encountered unexpected character while parsing "sv,
									(MULTI_LINE ? "multi-line "sv : ""sv),
									node_type::string,
									"; unknown escape sequence '\\"sv, *cp, "'"sv
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
								"Encountered unexpected character while parsing "sv,
								(MULTI_LINE ? "multi-line "sv : ""sv),
								node_type::string,
								"; control characters must be escaped with back-slashes."sv
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

				abort_with_error(
					"Encountered EOF while parsing "sv,
					(MULTI_LINE ? "multi-line "sv : ""sv),
					node_type::string
				);
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
							"Encountered EOF while parsing "sv,
							(MULTI_LINE ? "multi-line "sv : ""sv),
							"literal "sv, node_type::string
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
							"Encountered unexpected character while parsing "sv,
							(MULTI_LINE ? "multi-line "sv : ""sv),
							"literal "sv, node_type::string,
							"; control characters may not appear in literal strings"sv
						);

					str.append(cp->as_view());
					advance();
					TOML_ERROR_CHECK({});
				}

				abort_with_error("Encountered EOF while parsing "sv,
					(MULTI_LINE ? "multi-line "sv : ""sv),
					"literal "sv, node_type::string
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
					abort_with_error("Encountered EOF while parsing "sv, node_type::string);
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
					abort_with_error("Encountered EOF while parsing "sv, node_type::string);
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

				start_recording(true);
				auto result = *cp == U't';
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				{
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::boolean);
					else
						abort_with_error(
							"Encountered unexpected character while parsing "sv, node_type::boolean,
							"; expected '"sv, result ? "true"sv : "false"sv, "', saw '"sv, recording_buffer, '\''
						);
				}
				stop_recording();
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::boolean,
						"; expected value-terminator, saw '"sv,
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
						abort_with_error("Encountered EOF while parsing "sv, node_type::floating_point);
				};

				start_recording(true);
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
						"Encountered unexpected character while parsing "sv, node_type::floating_point,
						"; expected '"sv, inf ? "inf"sv : "nan"sv, "', saw '"sv, recording_buffer, '\''
					);
				}
				stop_recording();
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::floating_point,
						"; expected value-terminator, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});

				return inf
					? sign * std::numeric_limits<double>::infinity()
					: std::numeric_limits<double>::quiet_NaN();
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_SWITCH_WARNINGS
			TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
			double parse_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && (*cp == U'+' || *cp == U'-' || is_decimal_digit(*cp)));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::floating_point);
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
				TOML_GCC_ATTR(uninitialized) char chars[64];
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
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; underscores may only follow digits"sv
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
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; decimal points may appear only once"sv
							);
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; decimal points may not appear after exponents"sv
							);
						seen_decimal = true;
					}
					else if (*cp == U'e' || *cp == U'E')
					{
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; exponents may appear only once"sv
							);
						seen_exponent = true;
					}
					else if (*cp == U'+' || *cp == U'-')
					{
						if (!seen_exponent || !(*prev == U'e' || *prev == U'E'))
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; exponent signs must immediately follow 'e'"sv
							);
						if (seen_exponent_sign)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; exponents signs may appear only once"sv
							);
						seen_exponent_sign = true;
					}
					else if (!is_decimal_digit(*cp))
						abort_with_error("Encountered unexpected character while parsing "sv,
							node_type::floating_point, "; expected decimal digit, saw '"sv, *cp, '\''
						);

					if (length == sizeof(chars))
						abort_with_error(
							"Error parsing "sv, node_type::floating_point,
							"; exceeds maximum length of "sv, sizeof(chars), " characters"sv
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
						"Encountered unexpected character while parsing "sv, node_type::floating_point,
						"; expected decimal digit, saw '"sv, *cp, '\''
					);
				}

				TOML_ERROR_CHECK({});

				// convert to double
				TOML_GCC_ATTR(uninitialized) double result;
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::stringstream ss;
					ss.write(chars, static_cast<std::streamsize>(length));
					if ((ss >> result))
						return result * sign;
					else
						abort_with_error(
							"Error parsing "sv, node_type::floating_point,
							"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
						);
				}
				#else
				{
					auto fc_result = std::from_chars(chars, chars + length, result);
					switch (fc_result.ec)
					{
						case std::errc{}: //ok
							return result * sign;

						case std::errc::invalid_argument:
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
							);
							break;

						case std::errc::result_out_of_range:
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
							);
							break;

						default: //??
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; an unspecified error occurred while trying to interpret '",
								std::string_view{ chars, length }, "' as a value"sv
							);
					}
				}
				#endif
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/562

			[[nodiscard]]
			double parse_hex_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && *cp == U'0');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing hexadecimal "sv, node_type::floating_point);
				};

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal "sv, node_type::floating_point,
						"; expected '0', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				// 'x' or 'X'
				if (*cp != U'x' && *cp != U'X')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal "sv, node_type::floating_point,
						"; expected 'x' or 'X', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				TOML_ERROR_CHECK({});

				// consume value chars
				TOML_GCC_ATTR(uninitialized) char chars[23]; //23 = strlen("1.0123456789ABCDEFp+999")
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent_sign = false, seen_exponent = false;
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_hexadecimal_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; underscores may only follow digits"sv
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
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; decimal points may appear only once"sv
							);
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; decimal points may not appear after exponents"sv
							);
						seen_decimal = true;
					}
					else if (*cp == U'p' || *cp == U'P')
					{
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; exponents may appear only once"sv
							);
						if (!seen_decimal)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; exponents may not appear before decimal points"sv
							);
						seen_exponent = true;
					}
					else if (*cp == U'+' || *cp == U'-')
					{
						if (!seen_exponent || !(*prev == U'p' || *prev == U'P'))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; exponent signs must immediately follow 'p'"sv
							);
						if (seen_exponent_sign)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; exponents signs may appear only once"sv
							);
						seen_exponent_sign = true;
					}
					else
					{
						if (!seen_exponent && !is_hexadecimal_digit(*cp))
							abort_with_error("Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected hexadecimal digit, saw '"sv, *cp, '\''
							);
						else if (seen_exponent && !is_decimal_digit(*cp))
							abort_with_error("Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, " exponent; expected decimal digit, saw '"sv, *cp, '\''
							);
					}

					if (length == sizeof(chars))
						abort_with_error(
							"Error parsing hexadecimal "sv, node_type::floating_point,
							"; exceeds maximum length of "sv, sizeof(chars), " characters"sv
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
							"Encountered unexpected character while parsing hexadecimal "sv,
							node_type::floating_point, " exponent; expected decimal digit, saw '"sv, *cp, '\''
						);
					else
						abort_with_error(
							"Encountered unexpected character while parsing hexadecimal "sv,
							node_type::floating_point, "; expected hexadecimal digit, saw '"sv, *cp, '\''
						);
				}

				TOML_ERROR_CHECK({});

				// convert to double
				TOML_GCC_ATTR(uninitialized) double result;
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::string str;
					{
						std::stringstream ss;
						ss.write("0x", 2_sz);
						ss.write(chars, static_cast<std::streamsize>(length));
						str = ss.str();
					}

					char* end = {};
					result = std::strtod(str.c_str(), &end);
					if (result == 0.0 && end == str.c_str())
						abort_with_error(
							"Error parsing hexadecimal "sv, node_type::floating_point,
							"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
						);
					else
						return result;
				}
				#else
				{
					auto fc_result = std::from_chars(chars, chars + length, result, std::chars_format::hex);
					switch (fc_result.ec)
					{
						case std::errc{}: //ok
							return result;

						case std::errc::invalid_argument:
							abort_with_error(
								"Error parsing hexadecimal "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
							);
							break;

						case std::errc::result_out_of_range:
							abort_with_error(
								"Error parsing hexadecimal "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
							);
							break;

						default: //??
							abort_with_error(
								"Error parsing hexadecimal "sv, node_type::floating_point,
								"; an unspecified error occurred while trying to interpret '",
								std::string_view{ chars, length }, "' as a value"sv
							);
					}
				}
				#endif
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			#endif //TOML_LANG_HIGHER_THAN(0, 5, 0)

			template <int base>
			[[nodiscard]]
			int64_t parse_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp);
				using traits = parse_integer_traits<base>;

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv, traits::qualifier, ' ', node_type::integer
						);
				};

				[[maybe_unused]] int64_t sign = 1;
				if constexpr (traits::is_signed)
				{
					if (*cp == U'-')
					{
						sign = -1;
						advance();
					}
					else if(*cp == U'+')
						advance();
					eof_check();

					TOML_ERROR_CHECK({});
				}

				if constexpr (base == 10)
				{
					if (!traits::is_digit(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier, ' ',
							node_type::integer, "; expected expected "sv, traits::qualifier,
							" digit or sign, saw '"sv, *cp, '\''
						);
				}
				else
				{
					// '0'
					if (*cp != U'0')
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier,
							' ', node_type::integer, "; expected '0', saw '"sv, *cp, '\''
						);
					advance();
					eof_check();

					// 'b', 'o', 'x'
					if (*cp != traits::prefix_codepoint)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier,
							' ', node_type::integer, "; expected '"sv, traits::prefix,
							"', saw '"sv, *cp, '\''
						);
					advance();
					eof_check();
				}

				TOML_ERROR_CHECK({});

				// consume value chars
				TOML_GCC_ATTR(uninitialized) char chars[traits::buffer_length];
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !traits::is_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing "sv, traits::qualifier,
								' ', node_type::integer, "; expected "sv, traits::qualifier, " digit, saw '_'"sv
							);
					}
					else
					{
						if (!traits::is_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing "sv, traits::qualifier,
								' ', node_type::integer, "; expected "sv, traits::qualifier,
								" digit, saw '"sv, *cp, '\''
							);
						if (length == sizeof(chars))
							abort_with_error(
								"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
								"; exceeds maximum length of "sv, sizeof(chars), " characters"sv
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
						"Encountered unexpected character while parsing "sv, traits::qualifier,
						' ', node_type::integer, "; expected "sv, traits::qualifier, " digit, saw '_'"sv
					);
				}

				// check for leading zeroes
				if constexpr (base == 10)
				{
					if (chars[0] == '0')
						abort_with_error(
							"Error parsing "sv, traits::qualifier,
							' ', node_type::integer, "; leading zeroes are not allowed"sv
						);
				}

				TOML_ERROR_CHECK({});

				// single digits can be converted directly
				if (length == 1_sz)
				{
					if constexpr (base > 10)
					{
						return chars[0] >= 'A'
							? 10LL + static_cast<int64_t>(*cp - (*cp >= 'a' ? 'a' : 'A'))
							: static_cast<int64_t>(*cp - '0');
					}
					else
						return static_cast<int64_t>(chars[0] - '0');
				}

				// otherwise invoke charconv
				TOML_GCC_ATTR(uninitialized) uint64_t result;
				auto fc_result = std::from_chars(chars, chars + length, result, base);
				if constexpr (traits::is_signed)
				{
					if (fc_result.ec == std::errc{} && (
							(sign < 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1ull)
							|| (sign > 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
						))
						fc_result.ec = std::errc::result_out_of_range;
				}
				else
				{
					if (fc_result.ec == std::errc{} &&
							result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())
						)
						fc_result.ec = std::errc::result_out_of_range;
				}
				switch (fc_result.ec)
				{
					case std::errc{}: //ok
						if constexpr (traits::is_signed)
							return static_cast<int64_t>(result) * sign;
						else
							return static_cast<int64_t>(result);

					case std::errc::invalid_argument:
						abort_with_error(
							"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
							"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
						);
						break;

					case std::errc::result_out_of_range:
						abort_with_error(
							"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
							"; '"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
						);
						break;

					default: //??
						abort_with_error(
							"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
							"; an unspecified error occurred while trying to interpret '",
							std::string_view{ chars, length }, "' as a value"sv
						);
				}

				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			date parse_date(bool part_of_datetime = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_decimal_digit(*cp));

				const auto type = part_of_datetime ? node_type::date_time : node_type::date;
				const auto eof_check = [this, type]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv, type);
				};

				// "YYYY"
				TOML_GCC_ATTR(uninitialized) uint32_t year_digits[4];
				if (!consume_digit_sequence(year_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected 4-digit year, saw '"sv, *cp, '\''
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
						"Encountered unexpected character while parsing "sv, type,
						"; expected '-', saw '"sv, *cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "MM"
				TOML_GCC_ATTR(uninitialized) uint32_t month_digits[2];
				if (!consume_digit_sequence(month_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected 2-digit month, saw '"sv, *cp, '\''
					);
				}
				const auto month = month_digits[1] + month_digits[0] * 10u;
				if (month == 0u || month > 12u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected month between 1 and 12 (inclusive), saw "sv, month
					);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;

				// '-'
				eof_check();
				if (*cp != U'-')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected '-', saw '"sv, *cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "DD"
				TOML_GCC_ATTR(uninitialized) uint32_t day_digits[2];
				if (!consume_digit_sequence(day_digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected 2-digit day, saw '"sv, *cp, '\''
					);
				}
				const auto day = day_digits[1] + day_digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					abort_with_error("Error parsing "sv, type,
						"; expected day between 1 and "sv, max_days_in_month, " (inclusive), saw "sv, day
					);

				if (!part_of_datetime)
				{
					if (cp && !is_value_terminator(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected value-terminator, saw '"sv, *cp, '\''
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
			time parse_time(bool part_of_datetime = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_ASSERT(cp && is_decimal_digit(*cp));

				const auto type = part_of_datetime ? node_type::date_time : node_type::date;
				const auto eof_check = [this, type]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, type);
				};

				// "HH"
				TOML_GCC_ATTR(uninitialized) uint32_t digits[2];
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected 2-digit hour, saw '"sv, *cp, '\''
					);
				}
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected hour between 0 to 59 (inclusive), saw "sv, hour
					);

				// ':'
				eof_check();
				if (*cp != U':')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected ':', saw '"sv, *cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// "MM"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected 2-digit minute, saw '"sv,
						*cp, '\''
					);
				}
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected minute between 0 and 59 (inclusive), saw "sv, minute
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
							"Encountered unexpected character while parsing "sv, type,
							"; expected ':', saw '"sv, *cp, '\''
						);
					advance();
				}
				else
				{
					if (cp
						&& !is_value_terminator(*cp)
						&& *cp != U':'
						&& (!part_of_datetime || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected ':'"sv, (part_of_datetime ? ", offset"sv : ""sv),
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
						"Encountered unexpected character while parsing "sv, type,
						"; expected 2-digit second, saw '"sv,
						*cp, '\''
					);
				}
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected second between 0 and 59 (inclusive), saw "sv, second
					);
				time.second = static_cast<uint8_t>(second);

				//early exit here if the fractional is omitted
				if (cp
					&& !is_value_terminator(*cp)
					&& *cp != U'.'
					&& (!part_of_datetime || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected fractional"sv,
						(part_of_datetime ? ", offset"sv : ""sv),
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
				TOML_GCC_ATTR(uninitialized) uint32_t fractional_digits[max_fractional_digits];
				auto digit_count = consume_variable_length_digit_sequence(fractional_digits);
				if (!digit_count)
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected fractional digits, saw '"sv,
						*cp, '\''
					);
				if (digit_count == max_fractional_digits && cp && is_decimal_digit(*cp))
					abort_with_error(
						"Error parsing "sv, type,
						"Fractional component exceeds maximum precision of "sv, max_fractional_digits
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
						abort_with_error("Encountered EOF while parsing "sv, node_type::date_time);
				};

				// "YYYY-MM-DD"
				auto date = parse_date(true);
				TOML_ERROR_CHECK({});

				// ' ' or 'T'
				eof_check();
				if (*cp != U' ' && *cp != U'T' && *cp != U't')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::date_time,
						"; expected space or 'T', saw '"sv, *cp, '\''
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
						TOML_GCC_ATTR(uninitialized) int digits[2];
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								" offset; expected 2-digit hour, saw '"sv, *cp, '\''
							);
						}
						const auto hour = digits[1] + digits[0] * 10;
						if (hour > 23)
							abort_with_error(
								"Error parsing "sv, node_type::date_time,
								" offset; expected hour between 0 and 23 (inclusive), saw "sv, hour
							);

						// ':'
						eof_check();
						if (*cp != U':')
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								"offset; expected ':', saw '"sv, *cp, '\''
							);
						advance();

						// "MM"
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								" offset; expected 2-digit minute, saw '"sv, *cp, '\''
							);
						}
						const auto minute = digits[1] + digits[0] * 10;
						if (minute > 59)
							abort_with_error(
								"Error parsing "sv, node_type::date_time,
								" offset; expected minute between 0 and 59 (inclusive), saw "sv, hour
							);

						offset.emplace();
						offset->minutes = static_cast<int16_t>((hour * 60 + minute) * sign);
					}
				}

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::date_time,
						"; expected value-terminator, saw '"sv, *cp, '\''
					);

				TOML_ERROR_CHECK({});
				if (offset)
					return { date, time, *offset };
				else
					return { date, time };
			}

			// TOML_DISABLE_SWITCH_WARNINGS
			// TOML_DISABLE_INIT_WARNINGS
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
							if (!val->ref_cast<array>().is_homogeneous())
								TOML_ERROR(
									"Arrays cannot contain values of different types in TOML 0.5.0 and earlier.",
									begin_pos,
									reader.source_path()
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
						abort_with_error("Values may not begin with underscores."sv);

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
						has_sign = has_plus | has_minus,
						has_dot = 128,
						has_space = 256
					};
					int traits = has_nothing;
					TOML_GCC_ATTR(uninitialized) char32_t chars[utf8_buffered_reader::max_history_length];
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
								switch (*cp)
								{
									case U'E': [[fallthrough]];
									case U'e': traits |= has_e; break;
									case U'T': traits |= has_t; break;
									case U'Z': traits |= has_z; break;
									case U'+': traits |= has_plus; break;
									case U'-': traits |= has_minus; break;
									case U'.': traits |= has_dot; break;
									case U':': traits |= has_colon; break;
									default: if (is_decimal_digit(*cp)) traits |= has_digits;
								}
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
						&& traits == (has_digits | has_minus)
						&& chars[4] == U'-'
						&& chars[7] == U'-'
						&& cp
						&& *cp == U' ')
					{
						const auto pre_advance_count = advance_count;
						const auto pre_scan_traits = traits;
						chars[char_count++] = *cp;
						traits |= has_space;

						const auto backpedal = [&]() noexcept
						{
							go_back(advance_count - pre_advance_count);
							advance_count = pre_advance_count;
							traits = pre_scan_traits;
							char_count = 10_sz;
						};

						advance();
						advance_count++;
						TOML_ERROR_CHECK({});

						if (!cp || !is_decimal_digit(*cp))
							backpedal();
						else
						{
							chars[char_count++] = *cp;

							advance();
							advance_count++;
							TOML_ERROR_CHECK({});

							scan();
							TOML_ERROR_CHECK({});

							if (char_count == 12_sz)
								backpedal();
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

					// now things that can be identified from two or more characters
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
							case U'b': val = std::make_unique<value<int64_t>>(parse_integer<2>()); break;
							case U'o': val = std::make_unique<value<int64_t>>(parse_integer<8>()); break;
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
												begin_pos,
												reader.source_path()
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

								val = std::make_unique<value<int64_t>>(parse_integer<16>());
								break;
							}
						}
					}

					TOML_ERROR_CHECK({});
					if (val)
						break;

					// from here down it can only be date-times, floats and integers.
					if (begins_with_digit)
					{
						switch (traits)
						{
							// 100
							case has_digits:
								val = std::make_unique<value<int64_t>>(parse_integer<10>());
								break;

							// 1e1
							// 1e-1
							// 1e+1
							// 1.0
							// 1.0e1
							// 1.0e-1
							// 1.0e+1
							case has_digits | has_e:						[[fallthrough]];
							case has_digits | has_e | has_minus:			[[fallthrough]];
							case has_digits | has_e | has_plus:				[[fallthrough]];
							case has_digits | has_dot:						[[fallthrough]];
							case has_digits | has_dot | has_e:				[[fallthrough]];
							case has_digits | has_dot | has_e | has_minus:	[[fallthrough]];
							case has_digits | has_dot | has_e | has_plus:
								val = std::make_unique<value<double>>(parse_float());
								break;

							// HH:MM
							// HH:MM:SS
							// HH:MM:SS.FFFFFF
							case has_digits | has_colon:			[[fallthrough]];
							case has_digits | has_colon | has_dot:
								val = std::make_unique<value<time>>(parse_time());
								break;

							// YYYY-MM-DD
							case has_digits | has_minus:
								val = std::make_unique<value<date>>(parse_date());
								break;

							// YYYY-MM-DDTHH:MM
							// YYYY-MM-DDTHH:MM-HH:MM
							// YYYY-MM-DDTHH:MM+HH:MM
							// YYYY-MM-DD HH:MM
							// YYYY-MM-DD HH:MM-HH:MM
							// YYYY-MM-DD HH:MM+HH:MM
							// YYYY-MM-DDTHH:MM:SS
							// YYYY-MM-DDTHH:MM:SS-HH:MM
							// YYYY-MM-DDTHH:MM:SS+HH:MM
							// YYYY-MM-DD HH:MM:SS
							// YYYY-MM-DD HH:MM:SS-HH:MM
							// YYYY-MM-DD HH:MM:SS+HH:MM
							case has_digits | has_minus | has_colon | has_t:		[[fallthrough]];
							case has_digits | has_sign  | has_colon | has_t:		[[fallthrough]];
							case has_digits | has_minus | has_colon | has_space:	[[fallthrough]];
							case has_digits | has_sign  | has_colon | has_space:	[[fallthrough]];
							// YYYY-MM-DDTHH:MM:SS.FFFFFF
							// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
							// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
							// YYYY-MM-DD HH:MM:SS.FFFFFF
							// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
							// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
							case has_digits | has_minus | has_colon | has_dot | has_t:		[[fallthrough]];
							case has_digits | has_sign  | has_colon | has_dot | has_t:		[[fallthrough]];
							case has_digits | has_minus | has_colon | has_dot | has_space:	[[fallthrough]];
							case has_digits | has_sign  | has_colon | has_dot | has_space:	[[fallthrough]];
							// YYYY-MM-DDTHH:MMZ
							// YYYY-MM-DD HH:MMZ
							// YYYY-MM-DDTHH:MM:SSZ
							// YYYY-MM-DD HH:MM:SSZ
							// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
							// YYYY-MM-DD HH:MM:SS.FFFFFFZ
							case has_digits | has_minus | has_colon | has_z | has_t:				[[fallthrough]];
							case has_digits | has_minus | has_colon | has_z | has_space:			[[fallthrough]];
							case has_digits | has_minus | has_colon | has_dot | has_z | has_t:		[[fallthrough]];
							case has_digits | has_minus | has_colon | has_dot | has_z | has_space:
								val = std::make_unique<value<date_time>>(parse_date_time());
								break;
						}
					}
					else if (begins_with_sign)
					{
						switch (traits)
						{
							// +100
							// -100
							case has_digits | has_minus:	[[fallthrough]];
							case has_digits | has_plus:
								val = std::make_unique<value<int64_t>>(parse_integer<10>());
								break;

							// +1e1
							// +1.0
							// +1.0e1
							// +1.0e+1
							// +1.0e-1
							// -1.0e+1
							case has_digits | has_e | has_plus:				[[fallthrough]];
							case has_digits | has_dot | has_plus:			[[fallthrough]];
							case has_digits | has_dot | has_e | has_plus:	[[fallthrough]];
							case has_digits | has_dot | has_e | has_sign:	[[fallthrough]];
							// -1e1
							// -1.0
							// -1.0e1
							// -1.0e-1
							case has_digits | has_e | has_minus:			[[fallthrough]];
							case has_digits | has_dot | has_minus:			[[fallthrough]];
							case has_digits | has_dot | has_e | has_minus:
								val = std::make_unique<value<double>>(parse_float());
								break;
						}
					}
				}
				while (false);

				if (!val)
				{
					abort_with_error("Could not determine value type"sv);
					TOML_ERROR_CHECK({});
				}

				val->source_ = { begin_pos, current_position(1), reader.source_path() };
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
					else
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
				bool is_arr = false;

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
						is_arr = true;
					}

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					eof_check();

					// sanity-check the key start
					#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
					if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing table key; "
							"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					else
					#endif
					if (!is_bare_key_start_character(cp->value) && !is_string_delimiter(cp->value))
					{
						abort_with_error(
							"Encountered unexpected character while parsing table key; "
							"expected bare key starting character or string delimiter, saw '"sv, *cp, '\''
						);
					}
					TOML_ERROR_CHECK({});

					// get the actual key
					start_recording();
					key = parse_key(); //parse_key() calls stop_recording()
					TOML_ERROR_CHECK({});

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					eof_check();

					// consume the first closing ']'
					if (*cp != U']')
						abort_with_error(
							"Encountered unexpected character while parsing table"sv,
							(is_arr ? " array"sv : ""sv), " header; expected ']', saw '"sv, *cp, '\''
						);
					advance();

					// consume the second closing ']'
					if (is_arr)
					{
						eof_check();

						if (*cp != U']')
							abort_with_error(
								"Encountered unexpected character while parsing table array header; "
								"expected ']', saw '"sv, *cp, '\''
							);
						advance();
					}
					header_end_pos = current_position(1);

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (cp)
					{
						if (!consume_comment() && !consume_line_break())
							abort_with_error(
								"Encountered unexpected character after table"sv, (is_arr ? " array"sv : ""sv),
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
						implicit_tables.push_back(&child->ref_cast<table>());
						child->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						parent = &child->ref_cast<table>();
					}
					else if (child->is_table())
					{
						parent = &child->ref_cast<table>();
					}
					else if (child->is_array() && find(table_arrays, &child->ref_cast<array>()))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!child->ref_cast<array>().values.empty());
						TOML_ASSERT(child->ref_cast<array>().values.back()->is_table());
						parent = &child->ref_cast<array>().values.back()->ref_cast<table>();
					}
					else
					{
						if (!is_arr && child->type() == node_type::table)
							abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
						else
							abort_with_error(
								"Attempt to redefine existing "sv, child->type(),
								" '"sv, recording_buffer,
								"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
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
					if (is_arr)
					{
						auto tab_arr = &parent->values.emplace(key.segments.back(),std::make_unique<array>())
							.first->second->ref_cast<array>();
						table_arrays.push_back(tab_arr);
						tab_arr->source_ = { header_begin_pos, header_end_pos, reader.source_path() };

						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return &tab_arr->values.back()->ref_cast<table>();
					}

					//otherwise we're just making a table
					else
					{
						auto tab = &parent->values.emplace(key.segments.back(),std::make_unique<table>())
							.first->second->ref_cast<table>();
						tab->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab;
					}
				}

				// if there was already a matching node some sanity checking is necessary;
				// this is ok if we're making an array and the existing element is already an array (new element)
				// or if we're making a table and the existing element is an implicitly-created table (promote it),
				// otherwise this is a redefinition error.
				else
				{
					if (is_arr && matching_node->is_array() && find(table_arrays, &matching_node->ref_cast<array>()))
					{
						auto tab_arr = &matching_node->ref_cast<array>();
						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return &tab_arr->values.back()->ref_cast<table>();
					}

					else if (!is_arr
						&& matching_node->is_table()
						&& !implicit_tables.empty())
					{
						auto tbl = &matching_node->ref_cast<table>();
						const auto idx = find(implicit_tables, tbl);
						if (idx)
						{
							implicit_tables.erase(implicit_tables.cbegin() + static_cast<ptrdiff_t>(*idx));
							tbl->source_.begin = header_begin_pos;
							tbl->source_.end = header_end_pos;
							return tbl;
						}
					}

					//if we get here it's a redefinition error.
					if (!is_arr && matching_node->type() == node_type::table)
						abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
					else
						abort_with_error(
							"Attempt to redefine existing "sv, matching_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
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
							dotted_key_tables.push_back(&child->ref_cast<table>());
							dotted_key_tables.back()->inline_ = true;
							child->source_ = kvp.value->source_;
						}
						else if (!child->is_table() || !find(dotted_key_tables, &child->ref_cast<table>()))
						{
							abort_with_error("Attempt to redefine "sv, child->type(), " as dotted key-value pair"sv);
						}
						else
							child->source_.end = kvp.value->source_.end;
						TOML_ERROR_CHECK();
						tab = &child->ref_cast<table>();
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

				auto eof_pos = current_position(1);
				root.source_.end = eof_pos;
				if (current_table
					&& current_table != &root
					&& current_table->source_.end <= current_table->source_.begin)
					current_table->source_.end = eof_pos;
			}

			static void update_region_ends(node& nde) noexcept
			{
				const auto type = nde.type();
				if (type > node_type::array)
					return;

				if (type == node_type::table)
				{
					auto& tbl = nde.ref_cast<table>();
					if (tbl.inline_) //inline tables (and all their inline descendants) are already correctly terminated
						return;

					auto end = nde.source_.end;
					for (auto& [k, v] : tbl.values)
					{
						update_region_ends(*v);
						if (end < v->source_.end)
							end = v->source_.end;
					}
				}
				else //arrays
				{
					auto& arr = nde.ref_cast<array>();
					auto end = nde.source_.end;
					for (auto& v : arr.values)
					{
						update_region_ends(*v);
						if (end < v->source_.end)
							end = v->source_.end;
					}
					nde.source_.end = end;
				}
			}

		public:

			parser(utf8_reader_interface&& reader_) TOML_MAY_THROW
				: reader{ reader_ }
			{
				root.source_ = { prev_pos, prev_pos, reader.source_path() };

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
			TOML_DISABLE_INIT_WARNINGS

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
	#undef TOML_NORETURN
}
TOML_IMPL_END

TOML_START
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

	#endif // defined(__cpp_lib_char8_t)

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

	template <typename CHAR>
	inline parse_result parse_file(std::basic_string_view<CHAR> file_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The path's character type must be 1 byte in size."
		);

		// Q: "why is this function templated??"
		// A: I don't want to force users to drag in <fstream> if they're not going to do
		//    any parsing directly from files.

		auto ifs = std::basic_ifstream<CHAR>{ file_path };
		return parse(
			ifs,
			std::string_view{ reinterpret_cast<const char*>(file_path.data()), file_path.length() }
		);
	}

	inline namespace literals
	{
		[[nodiscard]]
		inline parse_result operator"" _toml(const char* str, size_t len) noexcept
		{
			return parse(std::string_view{ str, len });
		}

		#if defined(__cpp_lib_char8_t)

		[[nodiscard]]
		inline parse_result operator"" _toml(const char8_t* str, size_t len) noexcept
		{
			return parse(std::u8string_view{ str, len });
		}

		#endif

	}
}
TOML_END

#pragma endregion
//------------------------------------------  ↑ toml_parser.h  ---------------------------------------------------------

//-----------------------------------------------------------------  ↓ toml_formatter.h  -------------------------------
#pragma region

TOML_START
{
	enum class format_flags : uint8_t
	{
		none,
		quote_dates_and_times = 1
	};
	[[nodiscard]] constexpr format_flags operator & (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>(impl::unbox_enum(lhs) & impl::unbox_enum(rhs));
	}
	[[nodiscard]] constexpr format_flags operator | (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>( impl::unbox_enum(lhs) | impl::unbox_enum(rhs) );
	}
}
TOML_END

TOML_IMPL_START
{
	template <typename CHAR = char>
	class formatter
	{
		private:
			const toml::node* source_;
			std::basic_ostream<CHAR>* stream_ = nullptr;
			format_flags flags_;
			int8_t indent_;
			bool naked_newline_;

		protected:

			[[nodiscard]] const toml::node& source() const noexcept { return *source_; }
			[[nodiscard]] format_flags flags() const noexcept { return flags_; }
			[[nodiscard]] std::basic_ostream<CHAR>& stream() const noexcept { return *stream_; }

			static constexpr size_t indent_columns = 4;
			static constexpr toml::string_view indent_string = TOML_STRING_PREFIX("    "sv);
			[[nodiscard]] int8_t indent() const noexcept { return indent_; }
			void indent(int8_t level) noexcept { indent_ = level; }
			void increase_indent() noexcept { indent_++; }
			void decrease_indent() noexcept { indent_--; }
			void clear_naked_newline() noexcept { naked_newline_ = false; }
			void attach(std::basic_ostream<CHAR>& stream) noexcept
			{
				indent_ = {};
				naked_newline_ = true;
				stream_ = &stream;
			}

			void detach() noexcept
			{
				stream_ = nullptr;
			}

			void print_newline(bool force = false) TOML_MAY_THROW
			{
				if (!naked_newline_ || force)
				{
					print_to_stream('\n', *stream_);
					naked_newline_ = true;
				}
			}

			void print_indent() TOML_MAY_THROW
			{
				for (int8_t i = 0; i < indent_; i++)
				{
					print_to_stream(indent_string, *stream_);
					naked_newline_ = false;
				}
			}

			void print_quoted_string(toml::string_view str) TOML_MAY_THROW
			{
				if (str.empty())
					print_to_stream("\"\""sv, *stream_);
				else
				{
					print_to_stream('"', *stream_);
					print_to_stream_with_escapes(str, *stream_);
					print_to_stream('"', *stream_);
				}
				naked_newline_ = false;
			}

			template <typename T>
			void print(const value<T>& val) TOML_MAY_THROW
			{
				if constexpr (std::is_same_v<T, string>)
				{
					print_quoted_string(val.get());
				}
				else
				{
					static constexpr auto is_dt =
						std::is_same_v<T, date>
						|| std::is_same_v<T, time>
						|| std::is_same_v<T, date_time>;

					if constexpr (is_dt)
					{
						if ((flags_ & format_flags::quote_dates_and_times) != format_flags::none)
							print_to_stream('"', *stream_);
					}

					*stream_ << val;

					if constexpr (is_dt)
					{
						if ((flags_ & format_flags::quote_dates_and_times) != format_flags::none)
							print_to_stream('"', *stream_);
					}

					naked_newline_ = false;
				}
			}

			void print(const node& val_node, node_type type) TOML_MAY_THROW
			{
				switch (type)
				{
					case node_type::string:			print(*reinterpret_cast<const value<string>*>(&val_node)); break;
					case node_type::integer:		print(*reinterpret_cast<const value<int64_t>*>(&val_node)); break;
					case node_type::floating_point:	print(*reinterpret_cast<const value<double>*>(&val_node)); break;
					case node_type::boolean:		print(*reinterpret_cast<const value<bool>*>(&val_node)); break;
					case node_type::date:			print(*reinterpret_cast<const value<date>*>(&val_node)); break;
					case node_type::time:			print(*reinterpret_cast<const value<time>*>(&val_node)); break;
					case node_type::date_time:		print(*reinterpret_cast<const value<date_time>*>(&val_node)); break;
					TOML_NO_DEFAULT_CASE;
				}
			}

			formatter(const toml::node& source, format_flags flags) noexcept
				: source_{ &source },
				flags_{ flags }
			{}
	};
}
TOML_IMPL_END

#pragma endregion
//-----------------------------------------------------------------  ↑ toml_formatter.h  -------------------------------

//--------------------------------------------------------------------------------------  ↓ toml_default_formatter.h  --
#pragma region

TOML_IMPL_START
{
	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	[[nodiscard]]
	inline toml::string default_formatter_make_key_segment(const toml::string& str) noexcept
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
				{
					if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
						s.append(low_character_escape_table[c]);
					else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
						s.append(TOML_STRING_PREFIX("\\u007F"sv));
					else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
						s.append(TOML_STRING_PREFIX("\\\""sv));
					else
						s += c;
				}
				s += TOML_STRING_PREFIX('"');
				return s;
			}
			else
				return str;
		}
	}

	TOML_POP_WARNINGS

	[[nodiscard]]
	inline size_t default_formatter_inline_columns(const node& node) noexcept
	{
		return node.visit([](const auto& n) noexcept
			-> size_t
			{
				if constexpr (is_table<decltype(n)>)
				{
					if (n.empty())
						return 2_sz; // "{}"
					size_t weight = 3_sz; // "{ }"
					for (auto [k, v] : n)
						weight += k.length() + default_formatter_inline_columns(v) + 2_sz; // +  ", "
					return weight;
				}
				else if constexpr (is_array<decltype(n)>)
				{
					if (n.empty())
						return 2_sz; // "[]"
					size_t weight = 3_sz; // "[ ]"
					for (auto& elem : n)
						weight += default_formatter_inline_columns(elem) + 2_sz; // +  ", "
					return weight;
				}
				else if constexpr (is_string<decltype(n)>)
				{
					return n.get().length() + 2_sz; // + ""
				}
				else if constexpr (is_number<decltype(n)>)
				{
					static constexpr auto digit_count = [](auto num) noexcept
						-> size_t
					{
						using number_t = decltype(num);
						size_t digits = 1_sz;
						while (num >= number_t{ 10 })
						{
							num /= number_t{ 10 };
							digits++;
						}
						return digits;
					};

					if constexpr (is_integer<decltype(n)>)
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
						return weight + digit_count(v);
					}
					else if constexpr (is_floating_point<decltype(n)>)
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
						return weight + digit_count(v);
					}
				}
				else if constexpr (is_boolean<decltype(n)>)
				{
					return 5_sz;
				}
				else if constexpr (is_date<decltype(n)> || is_time<decltype(n)>)
				{
					return 10_sz;
				}
				else if constexpr (is_date_time<decltype(n)>)
				{
					return 30_sz;
				}
				TOML_UNREACHABLE;
			});
	}

	[[nodiscard]]
	inline bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
	{
		return (default_formatter_inline_columns(node) + starting_column_bias) > 120_sz;
	}
}
TOML_IMPL_END

TOML_START
{
	template <typename CHAR = char>
	class default_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;
			std::vector<toml::string> key_path;

			void print_key_segment(const toml::string& str) TOML_MAY_THROW
			{
				if (str.empty())
					impl::print_to_stream("''"sv, base::stream());
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
						base::print_quoted_string(str);
					else
						impl::print_to_stream(str, base::stream());
				}
				base::clear_naked_newline();
			}

			void print_key_path() TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						impl::print_to_stream('.', base::stream());
					impl::print_to_stream(segment, base::stream());
				}
				base::clear_naked_newline();
			}

			inline void print_inline(const table& /*tbl*/) TOML_MAY_THROW;
			void print(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					impl::print_to_stream("[]"sv, base::stream());
				else
				{
					const auto original_indent = base::indent();
					const auto multiline = impl::default_formatter_forces_multiline(
						arr,
						base::indent_columns * static_cast<size_t>(original_indent < 0 ? 0 : original_indent)
					);
					impl::print_to_stream("["sv, base::stream());
					if (multiline)
					{
						if (original_indent < 0)
							base::indent(0);
						base::increase_indent();
					}
					else
						impl::print_to_stream(' ', base::stream());

					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
						{
							impl::print_to_stream(',', base::stream());
							if (!multiline)
								impl::print_to_stream(' ', base::stream());
						}

						if (multiline)
						{
							base::print_newline(true);
							base::print_indent();
						}

						auto& v = arr[i];
						const auto type = v.type();
						switch (type)
						{
							case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print(v, type);
						}

					}
					if (multiline)
					{
						base::indent(original_indent);
						base::print_newline(true);
						base::print_indent();
					}
					else
						impl::print_to_stream(' ', base::stream());
					impl::print_to_stream("]"sv, base::stream());
				}
				base::clear_naked_newline();
			}

			void print(const table& tbl) TOML_MAY_THROW
			{
				static constexpr auto is_non_inline_array_of_tables = [](auto&& nde) noexcept
				{
					auto arr = nde.as_array();
					return arr
						&& arr->is_array_of_tables()
						&& !arr->template get_as<table>(0_sz)->is_inline();
				};

				//values, arrays, and inline tables/table arrays
				for (auto [k, v] : tbl)
				{
					const auto type = v.type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(&v)->is_inline())
						|| (type == node_type::array && is_non_inline_array_of_tables(v)))
						continue;

					base::print_newline();
					base::print_indent();
					print_key_segment(k);
					impl::print_to_stream(" = "sv, base::stream());
					switch (type)
					{
						case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
						case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
						default:
							base::print(v, type);
					}
				}

				//non-inline tables
				for (auto [k, v] : tbl)
				{
					const auto type = v.type();
					if (type != node_type::table || reinterpret_cast<const table*>(&v)->is_inline())
						continue;
					auto& child_tbl = *reinterpret_cast<const table*>(&v);

					//we can skip indenting and emitting the headers for tables that only contain other tables
					//(so we don't over-nest)
					size_t child_value_count{}; //includes inline tables and non-table arrays
					size_t child_table_count{};
					size_t child_table_array_count{};
					for (auto [child_k, child_v] : child_tbl)
					{
						const auto child_type = child_v.type();
						switch (child_type)
						{
							case node_type::table:
								if (reinterpret_cast<const table*>(&child_v)->is_inline())
									child_value_count++;
								else
									child_table_count++;
								break;

							case node_type::array:
								if (is_non_inline_array_of_tables(child_v))
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
						base::increase_indent();
					key_path.push_back(impl::default_formatter_make_key_segment(k));

					if (!skip_self)
					{
						base::print_newline();
						base::print_newline(true);
						base::print_indent();
						impl::print_to_stream("["sv, base::stream());
						print_key_path();
						impl::print_to_stream("]"sv, base::stream());
						base::print_newline(true);
					}

					print(child_tbl);

					key_path.pop_back();
					if (!skip_self)
						base::decrease_indent();
				}

				//table arrays
				for (auto [k, v] : tbl)
				{
					if (!is_non_inline_array_of_tables(v))
						continue;
					auto& arr = *reinterpret_cast<const array*>(&v);

					base::increase_indent();
					key_path.push_back(impl::default_formatter_make_key_segment(k));

					for (size_t i = 0; i < arr.size(); i++)
					{
						base::print_newline();
						base::print_newline(true);
						base::print_indent();
						impl::print_to_stream("[["sv, base::stream());
						print_key_path();
						impl::print_to_stream("]]"sv, base::stream());
						base::print_newline(true);
						print(*reinterpret_cast<const table*>(&arr[i]));
					}

					key_path.pop_back();
					base::decrease_indent();
				}
			}

			void print() TOML_MAY_THROW
			{
				switch (auto source_type = base::source().type())
				{
					case node_type::table:
					{
						auto& tbl = *reinterpret_cast<const table*>(&base::source());
						if (tbl.is_inline())
							print_inline(tbl);
						else
						{
							base::decrease_indent(); // so root kvps and tables have the same indent
							print(tbl);
						}
						break;
					}

					case node_type::array:
						print(*reinterpret_cast<const array*>(&base::source()));
						break;

					default:
						base::print(base::source(), source_type);
				}
			}

		public:

			TOML_NODISCARD_CTOR
			explicit default_formatter(const toml::node& source, format_flags flags = {}) noexcept
				: base{ source, flags }
			{}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, default_formatter& rhs)
				TOML_MAY_THROW
			{
				rhs.attach(lhs);
				rhs.key_path.clear();
				rhs.print();
				rhs.detach();
				return lhs;
			}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, default_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	template <typename CHAR>
	inline void default_formatter<CHAR>::print_inline(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream("{ "sv, base::stream());

			bool first = false;
			for (auto [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;

				print_key_segment(k);
				impl::print_to_stream(" = "sv, base::stream());

				const auto type = v.type();
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print(v, type);
				}
			}

			impl::print_to_stream(" }"sv, base::stream());
		}
		base::clear_naked_newline();
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		return lhs << default_formatter<CHAR>{ rhs };
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const array& rhs) TOML_MAY_THROW
	{
		return lhs << default_formatter<CHAR>{ rhs };
	}
}
TOML_END

#pragma endregion
//--------------------------------------------------------------------------------------  ↑ toml_default_formatter.h  --

//-------------  ↓ toml_json_formatter.h  ------------------------------------------------------------------------------
#pragma region

TOML_START
{
	template <typename CHAR = char>
	class json_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;

			inline void print(const toml::table& tbl) TOML_MAY_THROW;
			void print(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					impl::print_to_stream("[]"sv, base::stream());
				else
				{
					impl::print_to_stream('[', base::stream());
					base::increase_indent();
					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
							impl::print_to_stream(',', base::stream());
						base::print_newline(true);
						base::print_indent();

						auto& v = arr[i];
						const auto type = v.type();
						switch (type)
						{
							case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print(v, type);
						}

					}
					base::decrease_indent();
					base::print_newline(true);
					base::print_indent();
					impl::print_to_stream(']', base::stream());
				}
				base::clear_naked_newline();
			}

			void print() TOML_MAY_THROW
			{
				switch (auto source_type = base::source().type())
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&base::source())); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&base::source())); break;
					default: base::print(base::source(), source_type);
				}
			}

		public:

			TOML_NODISCARD_CTOR
			explicit json_formatter(
				const toml::node& source,
				format_flags flags = format_flags::quote_dates_and_times) noexcept
				: base{ source, flags }
			{}

			template <typename T>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter& rhs)
				TOML_MAY_THROW
			{
				rhs.attach(lhs);
				rhs.print();
				rhs.detach();
				return lhs;
			}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, json_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	template <typename CHAR>
	inline void json_formatter<CHAR>::print(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream('{', base::stream());
			base::increase_indent();
			bool first = false;
			for (auto [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;
				base::print_newline(true);
				base::print_indent();

				base::print_quoted_string(k);
				impl::print_to_stream(" : "sv, base::stream());

				const auto type = v.type();
				switch (type)
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print(v, type);
				}

			}
			base::decrease_indent();
			base::print_newline(true);
			base::print_indent();
			impl::print_to_stream('}', base::stream());
		}
		base::clear_naked_newline();
	}
}
TOML_END

#pragma endregion
//-------------  ↑ toml_json_formatter.h  ------------------------------------------------------------------------------

// macro hygiene
#if TOML_UNDEF_MACROS
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
	#undef TOML_INLINE_NS_EX
	#undef TOML_INLINE_NS_CHAR_8
	#undef TOML_START
	#undef TOML_START_2
	#undef TOML_START_1
	#undef TOML_END
	#undef TOML_IMPL_START
	#undef TOML_IMPL_END
#endif

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
// clang-format on
