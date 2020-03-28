//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v1.0.0
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
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de> (utf8_decoder)
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

#if !defined(TOML_ALL_INLINE) || (defined(TOML_ALL_INLINE) && TOML_ALL_INLINE)
	#undef TOML_ALL_INLINE
	#define TOML_ALL_INLINE 1
#endif

#if defined(TOML_IMPLEMENTATION) || TOML_ALL_INLINE || defined(__INTELLISENSE__)
	#undef TOML_IMPLEMENTATION
	#define TOML_IMPLEMENTATION 1
#else
	#define TOML_IMPLEMENTATION 0
#endif

#ifndef TOML_API
	#define TOML_API
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

#ifndef TOML_UNDEF_MACROS
	#define TOML_UNDEF_MACROS 1
#endif

//TOML_ASSERT

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif
#ifdef __clang__

	#define TOML_PUSH_WARNINGS				_Pragma("clang diagnostic push")
	#define TOML_DISABLE_SWITCH_WARNINGS	_Pragma("clang diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_INIT_WARNINGS		_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("clang diagnostic ignored \"-Weverything\"")
	#define TOML_POP_WARNINGS				_Pragma("clang diagnostic pop")
	#define TOML_ASSUME(cond)				__builtin_assume(cond)
	#define TOML_UNREACHABLE				__builtin_unreachable()
	#define TOML_GNU_ATTR(attr)				__attribute__((attr))
	#if defined(_MSC_VER) // msvc compat mode
		#ifdef __has_declspec_attribute
			#if __has_declspec_attribute(novtable)
				#define TOML_INTERFACE		__declspec(novtable)
			#endif
			#if __has_declspec_attribute(empty_bases)
				#define TOML_EMPTY_BASES	__declspec(empty_bases)
			#endif
			#define TOML_ALWAYS_INLINE		__forceinline
		#endif
	#endif
	#ifdef __has_attribute
		#if !defined(TOML_ALWAYS_INLINE) && __has_attribute(always_inline)
			#define TOML_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
		#endif
		#if !defined(TOML_TRIVIAL_ABI) && __has_attribute(trivial_abi)
			#define TOML_TRIVIAL_ABI		__attribute__((__trivial_abi__))
		#endif
	#endif
	#ifdef __EXCEPTIONS
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
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
	#ifdef _CPPUNWIND
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
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
	#define TOML_GNU_ATTR(attr)				__attribute__((attr))
	#define TOML_ALWAYS_INLINE				__attribute__((__always_inline__)) inline
	#define TOML_UNREACHABLE				__builtin_unreachable()
	#if !defined(TOML_RELOPS_REORDERING) && defined(__cpp_impl_three_way_comparison)
		#define TOML_RELOPS_REORDERING 1
	#endif
	#ifdef __cpp_exceptions
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
	#endif

	// these pass the __has_attribute() test but cause warnings on if/else branches =/
	#define TOML_LIKELY
	#define TOML_UNLIKELY

	// floating-point from_chars and to_chars are not implemented in any version of gcc as of 1/1/2020
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
#elif TOML_CPP_VERSION >= 202002L
	#define TOML_CPP 20
#elif TOML_CPP_VERSION >= 201703L
	#define TOML_CPP 17
#endif
#ifndef TOML_COMPILER_EXCEPTIONS
	#define TOML_COMPILER_EXCEPTIONS 1
#endif
#if TOML_COMPILER_EXCEPTIONS
	#ifndef TOML_EXCEPTIONS
		#define TOML_EXCEPTIONS 1
	#endif
#else
	#if defined(TOML_EXCEPTIONS) && TOML_EXCEPTIONS
		#error TOML_EXCEPTIONS was explicitly enabled but exceptions are disabled/unsupported by the compiler.
	#endif
	#undef TOML_EXCEPTIONS
	#define TOML_EXCEPTIONS	0
#endif
#if TOML_EXCEPTIONS
	#define TOML_MAY_THROW
#else
	#define TOML_MAY_THROW				noexcept
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
#ifndef TOML_GNU_ATTR
	#define TOML_GNU_ATTR(attr)
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
	#if !defined(TOML_LIKELY) && __has_cpp_attribute(likely)
		#define TOML_LIKELY [[likely]]
	#endif
	#if !defined(TOML_UNLIKELY) && __has_cpp_attribute(unlikely)
		#define TOML_UNLIKELY [[unlikely]]
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
#ifndef TOML_TRIVIAL_ABI
	#define TOML_TRIVIAL_ABI
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
#if TOML_ALL_INLINE
	#define TOML_INLINE_FUNC_IMPL	inline
#else
	#define TOML_INLINE_FUNC_IMPL
#endif

#define TOML_LIB_MAJOR		1
#define TOML_LIB_MINOR		0
#define TOML_LIB_PATCH		0

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
#include <charconv>
#ifndef TOML_ASSERT
	#if !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)
		#include <cassert>
		#define TOML_ASSERT(expr)			assert(expr)
	#else
		#define TOML_ASSERT(expr)			(void)0
	#endif
#endif
#ifndef TOML_OPTIONAL_TYPE
	#include <optional>
#endif
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

#ifdef __cpp_lib_launder
	#define TOML_LAUNDER(x)	std::launder(x)
#else
	#define TOML_LAUNDER(x)	x
#endif

#if !defined(DOXYGEN) && !defined(__INTELLISENSE__)
	#define TOML_ABI_NAMESPACES 1
#else
	#define TOML_ABI_NAMESPACES 0
#endif

namespace toml
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

	#ifndef DOXYGEN

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

	#if TOML_ABI_NAMESPACES
		#ifdef TOML_OPTIONAL_TYPE
			inline namespace abi_custopt {
		#else
			inline namespace abi_stdopt {
		#endif
	#endif

	struct date_time;

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_OPTIONAL_TYPE
	#endif

	#endif // !DOXYGEN

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

	#ifdef TOML_OPTIONAL_TYPE

	template <typename T>
	using optional = TOML_OPTIONAL_TYPE<T>;

	#else

	template <typename T>
	using optional = std::optional<T>;

	#endif

	#if TOML_LARGE_FILES

	using source_index = uint32_t;

	#else

	using source_index = uint16_t;

	#endif

	using source_path_ptr = std::shared_ptr<const std::string>;

	#if TOML_ABI_NAMESPACES
		#if TOML_LARGE_FILES
			inline namespace abi_lf {
		#else
			inline namespace abi_sf {
		#endif
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
	};

	struct source_region
	{
		source_position begin;
		source_position end;
		source_path_ptr path;
	};

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS

	#if defined(DOXYGEN) || !TOML_EXCEPTIONS

	#if TOML_ABI_NAMESPACES
		inline namespace abi_noex {
	#endif

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
	};

	#else

	#if TOML_ABI_NAMESPACES
		inline namespace abi_ex {
	#endif

	class parse_error final
		: public std::runtime_error
	{
		private:
			source_region source_;

		public:

			TOML_NODISCARD_CTOR TOML_GNU_ATTR(nonnull)
			parse_error(const char* desc, source_region&& src) noexcept
				: std::runtime_error{ desc },
				source_{ std::move(src) }
			{}

			TOML_NODISCARD_CTOR TOML_GNU_ATTR(nonnull)
			parse_error(const char* desc, const source_region& src) noexcept
				: parse_error{ desc, source_region{ src } }
			{}

			TOML_NODISCARD_CTOR TOML_GNU_ATTR(nonnull)
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

	#if TOML_ABI_NAMESPACES
		}  //end abi namespace for TOML_EXCEPTIONS
		} //end abi namespace for TOML_LARGE_FILES
	#endif
}

namespace toml::impl
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
			inline namespace abi_impl_ex {
			#define TOML_PARSER_TYPENAME ::toml::impl::abi_impl_ex::parser
		#else
			inline namespace abi_impl_noex {
			#define TOML_PARSER_TYPENAME ::toml::impl::abi_impl_noex::parser
		#endif
	#else
		#define TOML_PARSER_TYPENAME ::toml::impl::parser
	#endif

	class parser;

	#if TOML_ABI_NAMESPACES
		}
	#endif

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
		linkage void print_to_stream(type, std::basic_ostream<CHAR>&)

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
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, node_type rhs)
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

#pragma endregion
//-----------------  ↑ toml_common.h  ----------------------------------------------------------------------------------

//----------------------------------------  ↓ toml_date_time.h  --------------------------------------------------------
#pragma region

namespace toml
{
	struct TOML_TRIVIAL_ABI date final
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

	private:

		[[nodiscard]] TOML_ALWAYS_INLINE
		static constexpr uint32_t pack(date d) noexcept
		{
			return static_cast<uint32_t>(d.year) << 16
				| static_cast<uint32_t>(d.month) << 8
				| static_cast<uint32_t>(d.day);
		}

	public:

		[[nodiscard]]
		friend constexpr bool operator <  (date lhs, date rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator <= (date lhs, date rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator >  (date lhs, date rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator >= (date lhs, date rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}
	};

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	struct TOML_TRIVIAL_ABI time final
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

	private:

		[[nodiscard]] TOML_ALWAYS_INLINE
		static constexpr uint64_t pack(time t) noexcept
		{
			return static_cast<uint64_t>(t.hour) << 48
				| static_cast<uint64_t>(t.minute) << 40
				| static_cast<uint64_t>(t.second) << 32
				| static_cast<uint64_t>(t.nanosecond);
		}

	public:

		[[nodiscard]]
		friend constexpr bool operator <  (time lhs, time rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator <= (time lhs, time rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator >  (time lhs, time rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator >= (time lhs, time rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}
	};

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	struct TOML_TRIVIAL_ABI time_offset final
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

		[[nodiscard]]
		friend constexpr bool operator <  (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes < rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator <= (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes <= rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator >  (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes > rhs.minutes;
		}

		[[nodiscard]]
		friend constexpr bool operator >= (time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes >= rhs.minutes;
		}
	};

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const time_offset& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if TOML_ABI_NAMESPACES
		#ifdef TOML_OPTIONAL_TYPE
			inline namespace abi_custopt {
		#else
			inline namespace abi_stdopt {
		#endif
	#endif

	struct date_time final
	{
		toml::date date;
		toml::time time;
		optional<toml::time_offset> time_offset;

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
			return !(lhs == rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator <  (const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.time_offset < rhs.time_offset;
		}

		[[nodiscard]]
		friend constexpr bool operator <= (const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.time_offset <= rhs.time_offset;
		}

		[[nodiscard]]
		friend constexpr bool operator >  (const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs <= rhs);
		}

		[[nodiscard]]
		friend constexpr bool operator >= (const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs < rhs);
		}
	};

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_OPTIONAL_TYPE
	#endif

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const date_time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}
}

#pragma endregion
//----------------------------------------  ↑ toml_date_time.h  --------------------------------------------------------

//--------------------------------------------------------------  ↓ toml_print_to_stream.h  ----------------------------
#pragma region

namespace toml::impl
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
	void print_to_stream(std::basic_string_view<CHAR1> str, std::basic_ostream<CHAR2>& stream)
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(const std::basic_string<CHAR1>& str, std::basic_ostream<CHAR2>& stream)
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char character, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GNU_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char* str, size_t len, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#if defined(__cpp_lib_char8_t)

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char8_t character, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GNU_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char8_t* str, size_t len, std::basic_ostream<CHAR>& stream)
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
	inline void print_integer_to_stream(T val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	#define TOML_P2S_OVERLOAD(type)											\
		template <typename CHAR>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream)	\
		{																	\
			static_assert(sizeof(CHAR) == 1);								\
			print_integer_to_stream(val, stream);							\
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
	inline void print_floating_point_to_stream(T val, std::basic_ostream<CHAR>& stream, bool hexfloat = false)
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
			std::ostringstream ss;
			ss.precision(std::numeric_limits<T>::digits10 + 1);
			if (hexfloat)
				ss << std::hexfloat;
			ss << val;
			const auto str = std::move(ss).str();
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#else
		{
			char buf[charconv_buffer_length<T>];
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

	#define TOML_P2S_OVERLOAD(type)											\
		template <typename CHAR>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream)	\
		{																	\
			static_assert(sizeof(CHAR) == 1);								\
			print_floating_point_to_stream(val, stream);					\
		}

	TOML_P2S_OVERLOAD(float)
	TOML_P2S_OVERLOAD(double)

	#undef TOML_P2S_OVERLOAD

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(bool val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val ? "true"sv : "false"sv, stream);
	}

	template <typename T, typename CHAR>
	inline void print_to_stream(T val, std::basic_ostream<CHAR>& stream, size_t zero_pad_to_digits)
	{
		static_assert(sizeof(CHAR) == 1);
		char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		const auto len = static_cast<size_t>(res.ptr - buf);
		for (size_t i = len; i < zero_pad_to_digits; i++)
			print_to_stream('0', stream);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date& val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.year, stream, 4_sz);
		print_to_stream('-', stream);
		print_to_stream(val.month, stream, 2_sz);
		print_to_stream('-', stream);
		print_to_stream(val.day, stream, 2_sz);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::time& val, std::basic_ostream<CHAR>& stream)
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
	inline void print_to_stream(toml::time_offset val, std::basic_ostream<CHAR>& stream)
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
	inline void print_to_stream(const toml::date_time& val, std::basic_ostream<CHAR>& stream)
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
	void print_to_stream_with_escapes(T && str, std::basic_ostream<CHAR>& stream)
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

namespace toml
{
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_position& rhs)
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
	{
		impl::print_to_stream(rhs.description(), lhs);
		impl::print_to_stream("\n\t(error occurred at "sv, lhs);
		lhs << rhs.source();
		impl::print_to_stream(")"sv, lhs);
		return lhs;
	}
}

#pragma endregion
//--------------------------------------------------------------  ↑ toml_print_to_stream.h  ----------------------------

//---------------------------------------------------------------------------------------------  ↓ toml_node.h  --------
#pragma region

namespace toml
{
	class TOML_INTERFACE TOML_API node
	{
		private:
			friend class TOML_PARSER_TYPENAME;
			source_region source_{};

		protected:

			node(node&& other) noexcept;
			node& operator= (node&& rhs) noexcept;

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>& ref_cast() & noexcept
			{
				return *reinterpret_cast<impl::node_of<T>*>(this);
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>&& ref_cast() && noexcept
			{
				return std::move(*reinterpret_cast<impl::node_of<T>*>(this));
			}

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			const impl::node_of<T>& ref_cast() const & noexcept
			{
				return *reinterpret_cast<const impl::node_of<T>*>(this);
			}

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
			[[nodiscard]] virtual bool is_string() const noexcept;
			[[nodiscard]] virtual bool is_integer() const noexcept;
			[[nodiscard]] virtual bool is_floating_point() const noexcept;
			[[nodiscard]] virtual bool is_number() const noexcept;
			[[nodiscard]] virtual bool is_boolean() const noexcept;
			[[nodiscard]] virtual bool is_date() const noexcept;
			[[nodiscard]] virtual bool is_time() const noexcept;
			[[nodiscard]] virtual bool is_date_time() const noexcept;
			[[nodiscard]] virtual bool is_array_of_tables() const noexcept;

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is() const noexcept
			{
				using type = impl::unwrapped<impl::remove_cvref_t<T>>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
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

			[[nodiscard]] virtual table* as_table() noexcept;
			[[nodiscard]] virtual array* as_array() noexcept;
			[[nodiscard]] virtual toml::value<string>* as_string() noexcept;
			[[nodiscard]] virtual toml::value<int64_t>* as_integer() noexcept;
			[[nodiscard]] virtual toml::value<double>* as_floating_point() noexcept;
			[[nodiscard]] virtual toml::value<bool>* as_boolean() noexcept;
			[[nodiscard]] virtual toml::value<date>* as_date() noexcept;
			[[nodiscard]] virtual toml::value<time>* as_time() noexcept;
			[[nodiscard]] virtual toml::value<date_time>* as_date_time() noexcept;
			[[nodiscard]] virtual const table* as_table() const noexcept;
			[[nodiscard]] virtual const array* as_array() const noexcept;
			[[nodiscard]] virtual const toml::value<string>* as_string() const noexcept;
			[[nodiscard]] virtual const toml::value<int64_t>* as_integer() const noexcept;
			[[nodiscard]] virtual const toml::value<double>* as_floating_point() const noexcept;
			[[nodiscard]] virtual const toml::value<bool>* as_boolean() const noexcept;
			[[nodiscard]] virtual const toml::value<date>* as_date() const noexcept;
			[[nodiscard]] virtual const toml::value<time>* as_time() const noexcept;
			[[nodiscard]] virtual const toml::value<date_time>* as_date_time() const noexcept;

			template <typename T>
			[[nodiscard]] optional<T> value() const noexcept;

			template <typename T>
			[[nodiscard]] auto value_or(T&& default_value) const noexcept;

			template <typename T>
			[[nodiscard]] TOML_ALWAYS_INLINE
			impl::node_of<T>* as() noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
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
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
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

			[[nodiscard]] const source_region& source() const noexcept;

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
			static decltype(auto) do_visit(N&& n, FUNC&& visitor)
				noexcept(visit_is_nothrow<FUNC&&, N&&>)
			{
				static_assert(
					can_visit_any<FUNC&&, N&&>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (n.type())
				{
					case node_type::table:
						if constexpr (can_visit<FUNC&&, N&&, table>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<table>());
						break;

					case node_type::array:
						if constexpr (can_visit<FUNC&&, N&&, array>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<array>());
						break;

					case node_type::string:
						if constexpr (can_visit<FUNC&&, N&&, string>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<string>());
						break;

					case node_type::integer:
						if constexpr (can_visit<FUNC&&, N&&, int64_t>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<int64_t>());
						break;

					case node_type::floating_point:
						if constexpr (can_visit<FUNC&&, N&&, double>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<double>());
						break;

					case node_type::boolean:
						if constexpr (can_visit<FUNC&&, N&&, bool>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<bool>());
						break;

					case node_type::date:
						if constexpr (can_visit<FUNC&&, N&&, date>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<date>());
						break;

					case node_type::time:
						if constexpr (can_visit<FUNC&&, N&&, time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<time>());
						break;

					case node_type::date_time:
						if constexpr (can_visit<FUNC&&, N&&, date_time>)
							return std::forward<FUNC>(visitor)(std::forward<N>(n).template ref_cast<date_time>());
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

			template <typename T, typename N>
			[[nodiscard]] static decltype(auto) do_ref(N&& n) noexcept
			{
				using type = impl::unwrapped<T>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);
				TOML_ASSERT(
					n.template is<T>()
					&& "template type argument T provided to toml::node::ref() didn't match the node's actual type"
				);
				if constexpr (impl::is_value<type>)
					return std::forward<N>(n).template ref_cast<type>().get();
				else
					return std::forward<N>(n).template ref_cast<type>();
			}

		public:

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) & noexcept(visit_is_nothrow<FUNC&&, node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) && noexcept(visit_is_nothrow<FUNC&&, node&&>)
			{
				return do_visit(std::move(*this), std::forward<FUNC>(visitor));
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const& noexcept(visit_is_nothrow<FUNC&&, const node&>)
			{
				return do_visit(*this, std::forward<FUNC>(visitor));
			}

			template <typename T>
			[[nodiscard]] impl::unwrapped<T>& ref() & noexcept
			{
				return do_ref<T>(*this);
			}

			template <typename T>
			[[nodiscard]] impl::unwrapped<T>&& ref() && noexcept
			{
				return do_ref<T>(std::move(*this));
			}

			template <typename T>
			[[nodiscard]] const impl::unwrapped<T>& ref() const& noexcept
			{
				return do_ref<T>(*this);
			}
	};
}

#pragma endregion
//---------------------------------------------------------------------------------------------  ↑ toml_node.h  --------

//-----------------  ↓ toml_value.h  -----------------------------------------------------------------------------------
#pragma region

namespace toml
{
	template <typename CHAR, typename T>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const value<T>&);

	template <typename T>
	class TOML_API value final : public node
	{
		static_assert(
			impl::is_value<T>,
			"Template type parameter must be one of the TOML value types"
		);

		private:
			friend class TOML_PARSER_TYPENAME;

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
			explicit value(U&&... args) noexcept(std::is_nothrow_constructible_v<T, U&&...>)
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

			template <typename CHAR, typename U>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value<U>& rhs);
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

	#if !TOML_ALL_INLINE
	extern template class TOML_API value<string>;
	extern template class TOML_API value<int64_t>;
	extern template class TOML_API value<double>;
	extern template class TOML_API value<bool>;
	extern template class TOML_API value<date>;
	extern template class TOML_API value<time>;
	extern template class TOML_API value<date_time>;
	#endif

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

	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const value<T>& rhs)
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

	template <typename T>
	inline optional<T> node::value() const noexcept
	{
		static_assert(
			impl::is_value<T> || std::is_same_v<T, string_view>,
			"Value type must be one of the TOML value types (or string_view)"
		);

		switch (type())
		{
			case node_type::none:	[[fallthrough]];
			case node_type::table:	[[fallthrough]];
			case node_type::array:
				return {};

			case node_type::string:
			{
				if constexpr (std::is_same_v<T, string> || std::is_same_v<T, string_view>)
					return { T{ ref_cast<string>().get() } };
				else
					return {};
			}

			case node_type::integer:
			{
				if constexpr (std::is_same_v<T, int64_t>)
					return ref_cast<int64_t>().get();
				else if constexpr (std::is_same_v<T, double>)
					return static_cast<double>(ref_cast<int64_t>().get());
				else
					return {};
			}

			case node_type::floating_point:
			{
				if constexpr (std::is_same_v<T, double>)
					return ref_cast<double>().get();
				else if constexpr (std::is_same_v<T, int64_t>)
					return static_cast<int64_t>(ref_cast<double>().get());
				else
					return {};
			}

			case node_type::boolean:
			{
				if constexpr (std::is_same_v<T, bool>)
					return ref_cast<bool>().get();
				else
					return {};
			}

			case node_type::date:
			{
				if constexpr (std::is_same_v<T, date>)
					return ref_cast<date>().get();
				else
					return {};
			}

			case node_type::time:
			{
				if constexpr (std::is_same_v<T, time>)
					return ref_cast<time>().get();
				else
					return {};
			}

			case node_type::date_time:
			{
				if constexpr (std::is_same_v<T, date_time>)
					return ref_cast<date_time>().get();
				else
					return {};
			}

			TOML_NO_DEFAULT_CASE;
		}

		TOML_UNREACHABLE;
	}

	template <typename T>
	inline auto node::value_or(T&& default_value) const noexcept
	{
		static_assert(
			impl::is_value_or_promotable<impl::remove_cvref_t<T>>,
			"Default value type must be (or be promotable to) one of the TOML value types"
		);

		using value_type = impl::promoted<impl::remove_cvref_t<T>>;
		using return_type = std::conditional_t<
			std::is_same_v<value_type, string>,
			std::conditional_t<std::is_same_v<impl::remove_cvref_t<T>, string>, string, string_view>,
			value_type
		>;

		if (auto val = this->value<return_type>())
			return *val;
		return return_type{ std::forward<T>(default_value) };
	}
}

#pragma endregion
//-----------------  ↑ toml_value.h  -----------------------------------------------------------------------------------

//------------------------------------------  ↓ toml_array.h  ----------------------------------------------------------
#pragma region

namespace toml::impl
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

	template class array_iterator<true>;
	template class array_iterator<false>;

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

namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const array& lhs, const array& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const array& lhs, const array& rhs) noexcept;
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&);

	class TOML_API array final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;
			std::vector<std::unique_ptr<node>> values;

			void preinsertion_resize(size_t idx, size_t count) noexcept;

		public:

			using value_type = node;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using reference = node&;
			using const_reference = const node&;
			using iterator = impl::array_iterator<false>;
			using const_iterator = impl::array_iterator<true>;

			TOML_NODISCARD_CTOR
			array() noexcept;

			TOML_NODISCARD_CTOR
			array(array&& other) noexcept;

			template <typename U, typename... V>
			TOML_NODISCARD_CTOR
			explicit array(U&& val, V&&... vals)
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

			array& operator= (array&& rhs) noexcept;

			array(const array&) = delete;
			array& operator= (const array&) = delete;

			[[nodiscard]] node_type type() const noexcept override;
			[[nodiscard]] bool is_table() const noexcept override;
			[[nodiscard]] bool is_array() const noexcept override;
			[[nodiscard]] bool is_value() const noexcept override;
			[[nodiscard]] array* as_array() noexcept override;
			[[nodiscard]] const array* as_array() const noexcept override;

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

			[[nodiscard]] node& operator[] (size_t index) noexcept;
			[[nodiscard]] const node& operator[] (size_t index) const noexcept;
			[[nodiscard]] node& front() noexcept;
			[[nodiscard]] const node& front() const noexcept;
			[[nodiscard]] node& back() noexcept;
			[[nodiscard]] const node& back() const noexcept;
			[[nodiscard]] iterator begin() noexcept;
			[[nodiscard]] const_iterator begin() const noexcept;
			[[nodiscard]] const_iterator cbegin() const noexcept;
			[[nodiscard]] iterator end() noexcept;
			[[nodiscard]] const_iterator end() const noexcept;
			[[nodiscard]] const_iterator cend() const noexcept;
			[[nodiscard]] bool empty() const noexcept;
			[[nodiscard]] size_t size() const noexcept;
			void reserve(size_t new_capacity);
			void clear() noexcept;

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

			iterator erase(const_iterator pos) noexcept;
			iterator erase(const_iterator first, const_iterator last) noexcept;

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

			void pop_back() noexcept;
			[[nodiscard]] node* get(size_t index) noexcept;
			[[nodiscard]] const node* get(size_t index) const noexcept;

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

			friend bool operator == (const array& lhs, const array& rhs) noexcept;
			friend bool operator != (const array& lhs, const array& rhs) noexcept;

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

			[[nodiscard]] size_t total_leaf_count() const noexcept;
			void flatten_child(array&& child, size_t& dest_index) noexcept;

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
			void flatten();

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&);
	};
}

#pragma endregion
//------------------------------------------  ↑ toml_array.h  ----------------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_table.h  ---------------------------------
#pragma region

namespace toml::impl
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

	template struct table_proxy_pair<true>;
	template struct table_proxy_pair<false>;
	template class table_iterator<true>;
	template class table_iterator<false>;

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

namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const table& lhs, const table& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const table& lhs, const table& rhs) noexcept;
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const table&);

	class TOML_API table final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

			table(impl::table_init_pair*, size_t) noexcept;

		public:

			using iterator = impl::table_iterator<false>;
			using const_iterator = impl::table_iterator<true>;

			TOML_NODISCARD_CTOR
			table() noexcept;

			template <size_t N>
			TOML_NODISCARD_CTOR
			explicit table(impl::table_init_pair(&& arr)[N]) noexcept
				: table{ arr, N }
			{}

			TOML_NODISCARD_CTOR
			table(table&& other) noexcept;
			table& operator= (table&& rhs) noexcept;

			table(const table&) = delete;
			table& operator= (const table&) = delete;

			[[nodiscard]] node_type type() const noexcept override;
			[[nodiscard]] bool is_table() const noexcept override;
			[[nodiscard]] bool is_array() const noexcept override;
			[[nodiscard]] bool is_value() const noexcept override;
			[[nodiscard]] table* as_table() noexcept override;
			[[nodiscard]] const table* as_table() const noexcept override;
			[[nodiscard]] bool is_inline() const noexcept;
			void is_inline(bool val) noexcept;
			[[nodiscard]] node_view<node> operator[] (string_view key) noexcept;
			[[nodiscard]] node_view<const node> operator[] (string_view key) const noexcept;
			[[nodiscard]] iterator begin() noexcept;
			[[nodiscard]] const_iterator begin() const noexcept;
			[[nodiscard]] const_iterator cbegin() const noexcept;
			[[nodiscard]] iterator end() noexcept;
			[[nodiscard]] const_iterator end() const noexcept;
			[[nodiscard]] const_iterator cend() const noexcept;
			[[nodiscard]] bool empty() const noexcept;
			[[nodiscard]] size_t size() const noexcept;
			void clear() noexcept;

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

			iterator erase(iterator pos) noexcept;
			iterator erase(const_iterator pos) noexcept;
			iterator erase(const_iterator first, const_iterator last) noexcept;
			bool erase(string_view key) noexcept;

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

			[[nodiscard]] node* get(string_view key) noexcept;
			[[nodiscard]] const node* get(string_view key) const noexcept;
			[[nodiscard]] iterator find(string_view key) noexcept;
			[[nodiscard]] const_iterator find(string_view key) const noexcept;

			template <typename T>
			[[nodiscard]] impl::node_of<T>* get_as(string_view key) noexcept
			{
				return do_get_as<T>(values, key);
			}

			template <typename T>
			[[nodiscard]] const impl::node_of<T>* get_as(string_view key) const noexcept
			{
				return do_get_as<T>(values, key);
			}

			[[nodiscard]] bool contains(string_view key) const noexcept;
			friend bool operator == (const table& lhs, const table& rhs) noexcept;
			friend bool operator != (const table& lhs, const table& rhs) noexcept;

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const table&);
	};
}

#pragma endregion
//-------------------------------------------------------------------  ↑ toml_table.h  ---------------------------------

//------------------------------------------------------------------------------------------  ↓ toml_node_view.h  ------
#pragma region

namespace toml
{
	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const node_view<T>&);

	template <typename T>
	class TOML_API node_view final
	{
		public:
			using viewed_type = T;

		private:
			friend class toml::table;
			template <typename U> friend class toml::node_view;

			mutable viewed_type* node_;

			TOML_NODISCARD_CTOR
			node_view(viewed_type* node) noexcept
				: node_{ node }
			{}

			template <typename FUNC>
			static constexpr bool visit_is_nothrow
				= noexcept(std::declval<viewed_type*>()->visit(std::declval<FUNC&&>()));

		public:

			[[nodiscard]] explicit operator bool() const noexcept { return node_ != nullptr; }
			[[nodiscard]] viewed_type* get() const noexcept { return node_; }

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
			auto as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return node_ ? node_->template as<U>() : nullptr;
			}

			[[nodiscard]] auto as_table() const noexcept { return as<table>(); }
			[[nodiscard]] auto as_array() const noexcept { return as<array>(); }
			[[nodiscard]] auto as_string() const noexcept { return as<string>(); }
			[[nodiscard]] auto as_integer() const noexcept { return as<int64_t>(); }
			[[nodiscard]] auto as_floating_point() const noexcept { return as<double>(); }
			[[nodiscard]] auto as_boolean() const noexcept { return as<bool>(); }
			[[nodiscard]] auto as_date() const noexcept { return as<date>(); }
			[[nodiscard]] auto as_time() const noexcept { return as<time>(); }
			[[nodiscard]] auto as_date_time() const noexcept { return as<date_time>(); }

			template <typename U>
			[[nodiscard]] optional<U> value() const noexcept
			{
				if (node_)
					return node_->template value<U>();
				return {};
			}

			template <typename U>
			[[nodiscard]] auto value_or(U&& default_value) const noexcept
			{
				using return_type = decltype(node_->value_or(std::forward<U>(default_value)));
				if (node_)
					return node_->value_or(std::forward<U>(default_value));
				return return_type{ std::forward<U>(default_value) };
			}

			template <typename FUNC>
			decltype(auto) visit(FUNC&& visitor) const
				noexcept(visit_is_nothrow<FUNC&&>)
			{
				using return_type = decltype(node_->visit(std::forward<FUNC>(visitor)));
				if (node_)
					return node_->visit(std::forward<FUNC>(visitor));
				if constexpr (!std::is_void_v<return_type>)
					return return_type{};
			}

			template <typename U>
			[[nodiscard]] decltype(auto) ref() const noexcept
			{
				using type = impl::unwrapped<U>;
				static_assert(
					impl::is_value_or_node<type>,
					"Template type parameter must be one of the TOML value types, a toml::table, or a toml::array"
				);
				TOML_ASSERT(
					node_
					&& "toml::node_view::ref() called on a node_view that did not reference a node"
				);
				return node_->template ref<type>();
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
			[[nodiscard]] friend bool operator == (const node_view& lhs, const toml::value<U>& rhs) noexcept
			{
				if (lhs.node_ == &rhs)
					return true;
				const auto val = lhs.as<U>();
				return val && *val == rhs;
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const toml::value<U>&, template <typename U>)

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

			[[nodiscard]] node_view operator[] (string_view key) const noexcept
			{
				if (auto tbl = this->as_table())
					return { tbl->get(key) };
				return { nullptr };
			}

			[[nodiscard]] node_view operator[] (size_t index) const noexcept
			{
				if (auto arr = this->as_array())
					return { arr->get(index) };
				return { nullptr };
			}

			template <typename CHAR, typename U>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const node_view<U>&);
	};

	#if !TOML_ALL_INLINE
	extern template class TOML_API node_view<node>;
	extern template class TOML_API node_view<const node>;
	#endif

	template <typename CHAR, typename T>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& os, const node_view<T>& nv)
	{
		if (nv.node_)
		{
			nv.node_->visit([&os](const auto& n)
			{
				os << n;
			});
		}
		return os;
	}
}

#pragma endregion
//------------------------------------------------------------------------------------------  ↑ toml_node_view.h  ------

//-------------  ↓ toml_utf8_generated.h  ------------------------------------------------------------------------------
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
		if (codepoint < U'\u00AA' || codepoint > U'\U00031349')
			return false;

		TOML_ASSUME_CODEPOINT_BETWEEN(U'\u00AA', U'\U00031349');
		switch ((static_cast<uint_least32_t>(codepoint) - 0xAAu) / 3147u)
		{
			case 0:
			{
				if (codepoint > U'\u0CF2')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u00AA', U'\u0CF2');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xAAu) / 63u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAAull)) & 0x7FFFDFFFFFC10801ull;
					case 1: return codepoint != U'\u00F7';
					case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2A2ull)) & 0x4000FFF0FFFFFFFFull;
					case 9: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x2E1u)) & 0x280Fu;
					case 10: return false;
					case 11: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x370ull)) & 0x3FFFD740BCDFull;
					case 12: return codepoint != U'\u03A2';
					case 13: return codepoint != U'\u03F6';
					case 15: return codepoint <= U'\u0481' || codepoint >= U'\u048A';
					case 18: return codepoint != U'\u0530';
					case 19: return codepoint <= U'\u0559' || codepoint >= U'\u0560';
					case 21: return codepoint <= U'\u05EA' || codepoint >= U'\u05EF';
					case 23: return codepoint != U'\u0653';
					case 25: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x6D1ull)) & 0x4E0060300017ull;
					case 26: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x710ull)) & 0x60000000FFFFFFFDull;
					case 28: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x78Eull)) & 0x7000000800FFFFFFull;
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x7CDull)) & 0x7FF821803FFFFFFFull;
					case 30: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x80Cull)) & 0x7FF00000110043FFull;
					case 31: return codepoint <= U'\u0858' || codepoint >= U'\u0860';
					case 32: return codepoint != U'\u088A';
					case 34: return codepoint <= U'\u0939' || codepoint >= U'\u093D';
					case 35: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x950ull)) & 0x21FFFE0003FF01ull;
					case 36: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x986ull)) & 0x8F17F7FFFFE67Full;
					case 37: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x9CEull)) & 0x400C000EC001ull;
					case 38: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA05ull)) & 0x1B6FEFFFFFCC3Full;
					case 39: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA59u)) & 0xE00002Fu;
					case 40: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA85ull)) & 0x11F6FEFFFFFDDFFull;
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAD0ull)) & 0x20000030001ull;
					case 42: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB05ull)) & 0x11F6FEFFFFFCCFFull;
					case 43: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xB5Cu)) & 0x20003Bu;
					case 44: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB83ull)) & 0x7FF8E31AC7B8FDull;
					case 46: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC05ull)) & 0x1FFFEFFFFFEEFFull;
					case 47: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC3Dull)) & 0x1838000001ull;
					case 48: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC80ull)) & 0x1EFFDFFFFFDDFE1ull;
					case 49: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xCB9ull)) & 0x30001A000000011ull;
					default: return true;
				}
			}
			case 1:
			{
				if (codepoint < U'\u0D04' || codepoint > U'\u191E')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0D04', U'\u191E');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xD04u) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD04ull)) & 0x27FFFFFFFFFDDFFull;
					case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD4Eull)) & 0x3F000000E01C1ull;
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD85ull)) & 0x797FDFFFFFE3FFFFull;
					case 3: return codepoint <= U'\u0DC6' || codepoint >= U'\u0E01';
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xE04ull)) & 0xF000DFFFFFFFFFFFull;
					case 5: return codepoint <= U'\u0E46' || codepoint >= U'\u0E81';
					case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xE84ull)) & 0xF200DFFAFFFFFF7Dull;
					case 7: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xEC4ull)) & 0x100000000F000005ull;
					case 9: return codepoint != U'\u0F48';
					case 12: return codepoint <= U'\u102A' || codepoint >= U'\u103F';
					case 13: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1050ull)) & 0x3FFE1C0623C3Full;
					case 14: return codepoint <= U'\u108E' || codepoint >= U'\u10A0';
					case 15: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10C4ull)) & 0xFF7FFFFFFFFFF20Bull;
					case 21: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1244ull)) & 0xFFFFFFFFF3D7F3DFull;
					case 22: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1284ull)) & 0xD7F3DFFFFFFFF3DFull;
					case 23: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x12C4ull)) & 0xFFFFFFFFFFF7FFF3ull;
					case 24: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1304ull)) & 0xFFFFFFFFFFF3DFFFull;
					case 25: return codepoint <= U'\u135A' || codepoint >= U'\u1380';
					case 26: return codepoint <= U'\u138F' || codepoint >= U'\u13A0';
					case 27: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x13C4ull)) & 0xE3F3FFFFFFFFFFFFull;
					case 37: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1644ull)) & 0xEFFFF9FFFFFFFFFFull;
					case 38: return codepoint <= U'\u169A' || codepoint >= U'\u16A0';
					case 39: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x16C4ull)) & 0xF01FE07FFFFFFFFFull;
					case 40: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1704ull)) & 0xF0003FFFF0003DFFull;
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1744ull)) & 0xF0001DFFF0003FFFull;
					case 43: return codepoint <= U'\u17D7' || codepoint >= U'\u17DC';
					case 45: return codepoint <= U'\u1878' || codepoint >= U'\u1880';
					case 46: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1884ull)) & 0xFFFFF05FFFFFFFF9ull;
					case 47: return codepoint <= U'\u18F5' || codepoint >= U'\u1900';
					default: return true;
				}
			}
			case 2:
			{
				if (codepoint < U'\u1950' || codepoint > U'\u2184')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u1950', U'\u2184');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1950u) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1950ull)) & 0xFFFF001F3FFFFFFFull;
					case 1: return codepoint <= U'\u19AB' || codepoint >= U'\u19B0';
					case 3: return codepoint <= U'\u1A16' || codepoint >= U'\u1A20';
					case 7: return codepoint <= U'\u1B33' || codepoint >= U'\u1B45';
					case 9: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1B90ull)) & 0xFFFFFC00C001FFFFull;
					case 10: return codepoint <= U'\u1BE5' || codepoint >= U'\u1C00';
					case 11: return codepoint <= U'\u1C23' || codepoint >= U'\u1C4D';
					case 12: return codepoint <= U'\u1C7D' || codepoint >= U'\u1C80';
					case 13: return codepoint <= U'\u1CBA' || codepoint >= U'\u1CBD';
					case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1CE9ull)) & 0x7FFF8237EFull;
					case 23: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F10ull)) & 0x3F3FFFFFFFFF3F3Full;
					case 24: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F50ull)) & 0xFFFF3FFFFFFFAAFFull;
					case 25: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1F90ull)) & 0x1FDC5FDFFFFFFFFFull;
					case 26: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1FD0ull)) & 0x1FDC1FFF0FCFull;
					case 27: return false;
					case 28: return codepoint <= U'\u2071' || codepoint >= U'\u207F';
					case 30: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x2102u)) & 0x3F21u;
					case 31: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2110ull)) & 0x43E0F3FFBD503E2Full;
					default: return true;
				}
			}
			case 3:
			{
				if (codepoint < U'\u2C00' || codepoint > U'\u31BF')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\u2C00', U'\u31BF');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x2C00u) / 64u)
				{
					case 0: return codepoint != U'\u2C2F';
					case 1: return codepoint != U'\u2C5F';
					case 2: return true;
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2CC0ull)) & 0xC781FFFFFFFFFull;
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2D00ull)) & 0xFFFF20BFFFFFFFFFull;
					case 5: return codepoint <= U'\u2D67' || codepoint >= U'\u2D6F';
					case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x2D80ull)) & 0x7F7F7F7F007FFFFFull;
					case 7: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x2DC0u)) & 0x7F7F7F7Fu;
					case 8: return true;
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x3005ull)) & 0xC1F00000000003ull;
					case 17: return true;
					case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x3080ull)) & 0xFFFFFFFEE07FFFFFull;
					case 19: return codepoint != U'\u30FB';
					case 20: return codepoint != U'\u3100';
					case 21: return true;
					case 22: return codepoint <= U'\u318E' || codepoint >= U'\u31A0';
					default: return false;
				}
			}
			case 4: return codepoint <= U'\u31FF' || codepoint >= U'\u3400';
			case 6: return codepoint <= U'\u4DBE' || codepoint >= U'\u4E00';
			case 12: return codepoint <= U'\u9FFB' || codepoint >= U'\uA000';
			case 13:
			{
				if (codepoint < U'\uA079' || codepoint > U'\uACC3')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uA079', U'\uACC3');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xA079u) / 63u)
				{
					case 18: return codepoint <= U'\uA4FD' || codepoint >= U'\uA500';
					case 22: return codepoint <= U'\uA60C' || codepoint >= U'\uA610';
					case 23: return codepoint <= U'\uA62B' || codepoint >= U'\uA640';
					case 24: return codepoint <= U'\uA66E' || codepoint >= U'\uA67F';
					case 26: return codepoint <= U'\uA6E5' || codepoint >= U'\uA717';
					case 27: return codepoint <= U'\uA71F' || codepoint >= U'\uA722';
					case 28: return codepoint <= U'\uA788' || codepoint >= U'\uA78B';
					case 29: return codepoint <= U'\uA7BF' || codepoint >= U'\uA7C2';
					case 30: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA7F5ull)) & 0x1FFFBDDFFFull;
					case 31: return codepoint <= U'\uA822' || codepoint >= U'\uA840';
					case 32: return codepoint <= U'\uA873' || codepoint >= U'\uA882';
					case 34: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA8F2ull)) & 0xFFF001A3Full;
					case 35: return codepoint <= U'\uA925' || codepoint >= U'\uA930';
					case 36: return codepoint <= U'\uA97C' || codepoint >= U'\uA984';
					case 37: return codepoint <= U'\uA9B2' || codepoint >= U'\uA9CF';
					case 38: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xA9E0ull)) & 0x3FFFF7C00FFDFull;
					case 39: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA12ull)) & 0x3FDC000007FFFFFull;
					case 40: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA60ull)) & 0xFFFFC47FFFFFull;
					case 41: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA90ull)) & 0x53E62FFFFFFFFull;
					case 42: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAADBull)) & 0x7CFC00380FFE7ull;
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAB0Eull)) & 0x7FFFFFFDFDFC01F9ull;
					case 44: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAB4Dull)) & 0x7FFFFFF81FFFBFFFull;
					case 46: return codepoint <= U'\uABE2' || codepoint >= U'\uAC00';
					default: return true;
				}
			}
			case 17: return codepoint <= U'\uD7A2' || (codepoint >= U'\uD7B0' && codepoint <= U'\uD7C6')
					|| codepoint >= U'\uD7CB';
			case 18: return false;
			case 19: return false;
			case 20:
			{
				if (codepoint < U'\uF900' || codepoint > U'\U000102D0')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uF900', U'\U000102D0');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xF900u) / 63u)
				{
					case 5: return codepoint <= U'\uFA6D' || codepoint >= U'\uFA70';
					case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xFB00ull)) & 0x7FFDFFA0F8007Full;
					case 9: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xFB38ull)) & 0x3FFFFFFFFFFFDB5Full;
					case 17: return codepoint <= U'\uFD3D' || codepoint >= U'\uFD50';
					case 18: return codepoint <= U'\uFD8F' || codepoint >= U'\uFD92';
					case 21: return false;
					case 22: return codepoint != U'\uFE6A';
					case 24: return codepoint <= U'\uFEFC' || codepoint >= U'\uFF21';
					case 25: return codepoint <= U'\uFF3A' || codepoint >= U'\uFF41';
					case 27: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xFFA5ull)) & 0xE7E7E7E3FFFFFFull;
					case 28: return codepoint != U'\uFFE4';
					case 29: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10023ull)) & 0x7FFE7FFF6FFFFEFull;
					case 33: return false;
					case 34: return false;
					case 35: return false;
					case 36: return false;
					case 37: return false;
					case 39: return codepoint <= U'\U0001029C' || codepoint >= U'\U000102A0';
					default: return true;
				}
			}
			case 21:
			{
				if (codepoint < U'\U00010300' || codepoint > U'\U00010F1B')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00010300', U'\U00010F1B');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x10300u) / 64u)
				{
					case 0: return codepoint <= U'\U0001031F' || codepoint >= U'\U0001032D';
					case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10340ull)) & 0x3FFFFFFFFF03FDull;
					case 2: return codepoint <= U'\U0001039D' || codepoint >= U'\U000103A0';
					case 3: return codepoint <= U'\U000103C3' || codepoint >= U'\U000103C8';
					case 6: return codepoint <= U'\U0001049D' || codepoint >= U'\U000104B0';
					case 7: return codepoint <= U'\U000104D3' || codepoint >= U'\U000104D8';
					case 8: return codepoint <= U'\U00010527' || codepoint >= U'\U00010530';
					case 10: return false;
					case 11: return false;
					case 17: return codepoint <= U'\U00010755' || codepoint >= U'\U00010760';
					case 18: return false;
					case 19: return false;
					case 20: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10800ull)) & 0x91BFFFFFFFFFFD3Full;
					case 21: return codepoint <= U'\U00010855' || codepoint >= U'\U00010860';
					case 23: return codepoint != U'\U000108C0';
					case 24: return codepoint <= U'\U00010915' || codepoint >= U'\U00010920';
					case 25: return false;
					case 26: return codepoint <= U'\U000109B7' || codepoint >= U'\U000109BE';
					case 27: return false;
					case 28: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10A00ull)) & 0x3FFFFFFEEF0001ull;
					case 31: return codepoint != U'\U00010AC8';
					case 33: return codepoint <= U'\U00010B55' || codepoint >= U'\U00010B60';
					case 35: return false;
					case 41: return false;
					case 42: return false;
					case 43: return false;
					case 44: return false;
					case 45: return false;
					case 46: return codepoint <= U'\U00010EA9' || codepoint >= U'\U00010EB0';
					case 47: return false;
					default: return true;
				}
			}
			case 22:
			{
				if (codepoint < U'\U00010F1C' || codepoint > U'\U00011AF8')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00010F1C', U'\U00011AF8');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x10F1Cu) / 64u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10F1Cull)) & 0x3FFFFF00801ull;
					case 1: return false;
					case 3: return codepoint <= U'\U00010FF6' || codepoint >= U'\U00011003';
					case 6: return codepoint <= U'\U000110AF' || codepoint >= U'\U000110D0';
					case 7: return codepoint <= U'\U000110E8' || codepoint >= U'\U00011103';
					case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1111Cull)) & 0xFFF00900000007FFull;
					case 9: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1115Cull)) & 0xFFFFFF80047FFFFFull;
					case 10: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1119Cull)) & 0x400001E0007FFFFFull;
					case 11: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x111DCull)) & 0xFFBFFFF000000001ull;
					case 13: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x11280u)) & 0xFFFBD7Fu;
					case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1129Cull)) & 0xFFFFFFFFFFF01FFBull;
					case 15: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x112DCull)) & 0xFF99FE0000000007ull;
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1131Cull)) & 0x1000023EDFDFFFull;
					case 18: return false;
					case 20: return codepoint <= U'\U00011434' || codepoint >= U'\U00011447';
					case 21: return codepoint <= U'\U00011461' || codepoint >= U'\U00011480';
					case 22: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1149Cull)) & 0xB00000FFFFFull;
					case 23: return false;
					case 24: return false;
					case 26: return codepoint <= U'\U000115AE' || codepoint >= U'\U000115D8';
					case 28: return codepoint <= U'\U0001162F' || codepoint >= U'\U00011644';
					case 30: return codepoint <= U'\U000116AA' || codepoint >= U'\U000116B8';
					case 32: return false;
					case 33: return false;
					case 34: return false;
					case 37: return false;
					case 39: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x118DCull)) & 0xF6FF27F80000000Full;
					case 40: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1191Cull)) & 0x28000FFFFFull;
					case 41: return false;
					case 42: return codepoint <= U'\U000119A7' || codepoint >= U'\U000119AA';
					case 43: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x119E1ull)) & 0x7FFFC0080000005ull;
					case 44: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11A1Cull)) & 0x100000407FFFFFull;
					case 46: return codepoint <= U'\U00011A9D' || codepoint >= U'\U00011AC0';
					default: return true;
				}
			}
			case 23:
			{
				if (codepoint < U'\U00011C00' || codepoint > U'\U00012543')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00011C00', U'\U00012543');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x11C00u) / 63u)
				{
					case 0: return codepoint != U'\U00011C09';
					case 1: return codepoint <= U'\U00011C40' || codepoint >= U'\U00011C72';
					case 3: return false;
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11D00ull)) & 0x1FFFFFFFFFB7Full;
					case 5: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11D46ull)) & 0xFFFF6FC000001ull;
					case 6: return codepoint <= U'\U00011D89' || codepoint >= U'\U00011D98';
					case 7: return false;
					case 8: return false;
					case 9: return false;
					case 10: return false;
					case 12: return false;
					case 13: return false;
					case 15: return false;
					case 31: return false;
					case 32: return false;
					case 33: return false;
					default: return true;
				}
			}
			case 27: return false;
			case 28: return false;
			case 29:
			{
				if (codepoint < U'\U00016800' || codepoint > U'\U00017173')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00016800', U'\U00017173');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x16800u) / 64u)
				{
					case 10: return false;
					case 13: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x16B40ull)) & 0xE0FFFFF80000000Full;
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
					case 26: return false;
					case 27: return false;
					case 29: return codepoint <= U'\U00016F4A' || codepoint >= U'\U00016F50';
					case 31: return codepoint != U'\U00016FC0';
					default: return true;
				}
			}
			case 31: return codepoint <= U'\U000187F6' || codepoint >= U'\U00018800';
			case 32: return codepoint <= U'\U00018CD5' || codepoint >= U'\U00018D00';
			case 33: return false;
			case 34: return false;
			case 35: return codepoint <= U'\U0001B11E' || (codepoint >= U'\U0001B150' && codepoint <= U'\U0001B152')
					|| (codepoint >= U'\U0001B164' && codepoint <= U'\U0001B167') || codepoint >= U'\U0001B170';
			case 36: return codepoint <= U'\U0001BC6A' || (codepoint >= U'\U0001BC70' && codepoint <= U'\U0001BC7C')
					|| (codepoint >= U'\U0001BC80' && codepoint <= U'\U0001BC88') || codepoint >= U'\U0001BC90';
			case 37: return false;
			case 38:
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
			}
			case 39:
			{
				if (codepoint < U'\U0001E100' || codepoint > U'\U0001E94B')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001E100', U'\U0001E94B');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1E100u) / 63u)
				{
					case 0: return codepoint <= U'\U0001E12C' || codepoint >= U'\U0001E137';
					case 1: return true;
					case 7: return true;
					case 28: return true;
					case 29: return true;
					case 30: return true;
					case 31: return true;
					case 32: return true;
					case 33: return codepoint <= U'\U0001E943' || codepoint >= U'\U0001E94B';
					default: return false;
				}
			}
			case 40:
			{
				if (codepoint < U'\U0001EE00' || codepoint > U'\U0001EEBB')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001EE00', U'\U0001EEBB');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1EE00u) / 63u)
				{
					case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE00ull)) & 0xAF7FE96FFFFFFEFull;
					case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE42ull)) & 0x7BDFDE5AAA5BAA1ull;
					case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1EE7Eull)) & 0x3FFFEFB83FFFEFFDull;
					TOML_NO_DEFAULT_CASE;
				}
			}
			case 55: return codepoint <= U'\U0002A6DC' || codepoint >= U'\U0002A700';
			case 56: return codepoint <= U'\U0002B733' || (codepoint >= U'\U0002B740' && codepoint <= U'\U0002B81C')
					|| codepoint >= U'\U0002B820';
			case 58: return codepoint <= U'\U0002CEA0' || codepoint >= U'\U0002CEB0';
			default: return true;
		}
	}

	[[nodiscard]]
	constexpr bool is_unicode_number(char32_t codepoint) noexcept
	{
		if (codepoint < U'\u0660' || codepoint > U'\U0001FBF9')
			return false;

		TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0660', U'\U0001FBF9');
		switch ((static_cast<uint_least32_t>(codepoint) - 0x660u) / 2007u)
		{
			case 0:
			{
				if (codepoint > U'\u0DEF')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x660u) / 63u) & 0x55555025ull;
			}
			case 1:
			{
				if (codepoint < U'\u0E50' || codepoint > U'\u1099')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0xE50u) / 59u) & 0x30Dull;
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
			}
			case 3: return codepoint <= U'\u2182' || codepoint >= U'\u2185';
			case 5: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x3007ull)) & 0xE0007FC000001ull;
			case 20:
			{
				if (codepoint < U'\uA620' || codepoint > U'\uAA59')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0xA620u) / 64u) & 0x1CC09ull;
			}
			case 21: return true;
			case 31: return true;
			case 32:
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
			}
			case 33: return codepoint <= U'\U00010D39' || codepoint >= U'\U00011066';
			case 34:
			{
				if (codepoint < U'\U000110F0' || codepoint > U'\U00011739')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x110F0u) / 62u) & 0x341610Bull;
			}
			case 35:
			{
				if (codepoint < U'\U000118E0' || codepoint > U'\U00011DA9')
					return false;

				return ((static_cast<uint_least32_t>(codepoint) - 0x118E0u) / 62u) & 0xC4003ull;
			}
			case 36: return true;
			case 45: return codepoint <= U'\U00016A69' || codepoint >= U'\U00016B50';
			case 59: return true;
			case 60: return codepoint <= U'\U0001E149' || codepoint >= U'\U0001E2F0';
			case 61: return true;
			case 63: return true;
			default: return false;
		}
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
				switch ((static_cast<uint_least32_t>(codepoint) - 0x300u) / 1460u)
				{
					case 0:
					{
						if (codepoint > U'\u085B')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0300', U'\u085B');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x300u) / 63u)
						{
							case 0: return true;
							case 1: return true;
							case 6: return true;
							case 10: return true;
							case 11: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x5B5u)) & 0x5B5FFu;
							case 12: return true;
							case 13: return codepoint <= U'\u065F' || codepoint >= U'\u0670';
							case 15: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x6D6u)) & 0xF67E7Fu;
							case 16: return true;
							case 17: return true;
							case 18: return true;
							case 19: return codepoint <= U'\u07B0' || codepoint >= U'\u07EB';
							case 20: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x7ECull)) & 0x6EFFBC00000200FFull;
							case 21: return codepoint <= U'\u082D' || codepoint >= U'\u0859';
							default: return false;
						}
					}
					case 1:
					{
						if (codepoint < U'\u08D3' || codepoint > U'\u0E4E')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u08D3', U'\u0E4E');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x8D3u) / 64u)
						{
							case 0: return codepoint != U'\u08E2';
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x93Au)) & 0x1BFFFF7u;
							case 2: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x953ull)) & 0x1C0000001801Full;
							case 3: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x9BCu)) & 0x399FDu;
							case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x9D7ull)) & 0x1C8000001801ull;
							case 5: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA3Cu)) & 0x23987Du;
							case 6: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xA70u)) & 0xE0023u;
							case 7: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xABCu)) & 0x3BBFDu;
							case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAE2ull)) & 0x3BF000003ull;
							case 9: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xB3Cu)) & 0x399FDu;
							case 10: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xB55ull)) & 0x200000006007ull;
							case 11: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xBBEu)) & 0xF71Fu;
							case 12: return codepoint <= U'\u0BD7' || codepoint >= U'\u0C00';
							case 13: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xC3Eu)) & 0xF77Fu;
							case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xC55ull)) & 0x700000006003ull;
							case 15: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xCBCu)) & 0x3DDFDu;
							case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xCD5ull)) & 0x780000006003ull;
							case 17: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xD3Bu)) & 0x7BBFBu;
							case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xD57ull)) & 0x1C0000001801ull;
							case 19: return codepoint <= U'\u0DCA' || codepoint >= U'\u0DCF';
							case 20: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xDD3ull)) & 0x180001FEBull;
							case 21: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xE31u)) & 0x3FC003F9u;
							TOML_NO_DEFAULT_CASE;
						}
					}
					case 2:
					{
						if (codepoint < U'\u0EB1' || codepoint > U'\u135F')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u0EB1', U'\u135F');
						switch ((static_cast<uint_least32_t>(codepoint) - 0xEB1u) / 64u)
						{
							case 0: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xEB1u)) & 0x1F800FF9u;
							case 1: return true;
							case 2: return (1u << (static_cast<uint_least32_t>(codepoint) - 0xF35u)) & 0x615u;
							case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xF71ull)) & 0xFFFFFF7FF06FFFFFull;
							case 4: return codepoint <= U'\u0FBC' || codepoint >= U'\u0FC6';
							case 5: return true;
							case 6: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1031ull)) & 0x1FCEE1E000003FFFull;
							case 7: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1071ull)) & 0x1E005FFE000Full;
							case 18: return true;
							default: return false;
						}
					}
					case 3:
					{
						if (codepoint < U'\u1712' || codepoint > U'\u193B')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u1712', U'\u193B');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1712u) / 62u)
						{
							case 0: return codepoint <= U'\u1714' || codepoint >= U'\u1732';
							case 1: return codepoint <= U'\u1753' || codepoint >= U'\u1772';
							case 3: return codepoint <= U'\u17D3' || codepoint >= U'\u17DD';
							case 6: return codepoint <= U'\u1886' || codepoint >= U'\u18A9';
							case 7: return false;
							case 8: return codepoint <= U'\u192B' || codepoint >= U'\u1930';
							default: return true;
						}
					}
					case 4:
					{
						if (codepoint < U'\u1A17' || codepoint > U'\u1DFF')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\u1A17', U'\u1DFF');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1A17u) / 63u)
						{
							case 0: return codepoint <= U'\u1A1B' || codepoint >= U'\u1A55';
							case 1: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1A56ull)) & 0x27FFFFFFDFFull;
							case 2: return codepoint != U'\u1A95';
							case 5: return codepoint <= U'\u1B73' || codepoint >= U'\u1B80';
							case 9: return false;
							case 10: return false;
							case 11: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1CD0ull)) & 0x39021FFFFF7ull;
							case 12: return false;
							case 13: return false;
							case 15: return codepoint != U'\u1DFA';
							default: return true;
						}
					}
					case 5: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x20D0ull)) & 0x1FFE21FFFull;
					case 7:
					{
						if (codepoint < U'\u2CEF')
							return false;

						return ((static_cast<uint_least32_t>(codepoint) - 0x2CEFu) / 63u) & 0x601Dull;
					}
					default: return false;
				}
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
					case 7: return codepoint <= U'\uA827' || codepoint >= U'\uA82C';
					case 10: return codepoint <= U'\uA8F1' || codepoint >= U'\uA8FF';
					case 11: return codepoint <= U'\uA92D' || codepoint >= U'\uA947';
					case 12: return codepoint <= U'\uA953' || codepoint >= U'\uA980';
					case 16: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA43ull)) & 0x100000000000601ull;
					case 17: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAA7Cull)) & 0x19D0000000000003ull;
					case 18: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0xAABEull)) & 0x3E0000000000Bull;
					default: return true;
				}
			}
			case 3: return codepoint <= U'\uAAF6' || (codepoint >= U'\uABE3' && codepoint <= U'\uABEA')
					|| codepoint >= U'\uABEC';
			case 4:
			{
				if (codepoint < U'\uFB1E' || codepoint > U'\U00011A99')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\uFB1E', U'\U00011A99');
				switch ((static_cast<uint_least32_t>(codepoint) - 0xFB1Eu) / 1008u)
				{
					case 0: return codepoint == U'\uFB1E' || (codepoint >= U'\uFE00' && codepoint <= U'\uFE0F')
							|| codepoint >= U'\uFE20';
					case 1: return codepoint <= U'\U000101FD' || codepoint >= U'\U000102E0';
					case 3: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x10A01ull)) & 0x4380000000007837ull;
					case 4: return codepoint <= U'\U00010AE6' || (codepoint >= U'\U00010D24' && codepoint <= U'\U00010D27')
							|| codepoint >= U'\U00010EAB';
					case 5:
					{
						if (codepoint < U'\U00010F46' || codepoint > U'\U0001123E')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00010F46', U'\U0001123E');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x10F46u) / 64u)
						{
							case 1: return false;
							case 4: return codepoint <= U'\U00011046' || codepoint >= U'\U0001107F';
							case 7: return codepoint <= U'\U00011134' || codepoint >= U'\U00011145';
							case 8: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11146ull)) & 0x1C00200000000001ull;
							case 10: return codepoint != U'\U000111C6';
							case 11: return codepoint <= U'\U00011237' || codepoint >= U'\U0001123E';
							default: return true;
						}
					}
					case 6:
					{
						if (codepoint < U'\U000112DF' || codepoint > U'\U000116AD')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U000112DF', U'\U000116AD');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x112DFu) / 61u)
						{
							case 0: return codepoint <= U'\U000112EA' || codepoint >= U'\U00011300';
							case 1: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x1133Bu)) & 0x100733FBu;
							case 2: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x11362u)) & 0x7C7F3u;
							case 3: return false;
							case 4: return false;
							case 8: return false;
							case 9: return false;
							case 10: return false;
							case 11: return codepoint <= U'\U000115B5' || codepoint >= U'\U000115B8';
							case 12: return codepoint <= U'\U000115C0' || codepoint >= U'\U000115DC';
							default: return true;
						}
					}
					case 7:
					{
						if (codepoint < U'\U000116AE')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U000116AE', U'\U00011A99');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x116AEu) / 63u)
						{
							case 0: return true;
							case 1: return true;
							case 6: return true;
							case 10: return (1u << (static_cast<uint_least32_t>(codepoint) - 0x11930u)) & 0xD79BFu;
							case 12: return codepoint <= U'\U000119D7' || codepoint >= U'\U000119DA';
							case 13: return codepoint <= U'\U000119E4' || codepoint >= U'\U00011A01';
							case 14: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x11A33ull)) & 0x1FFC0100F7Full;
							case 15: return true;
							default: return false;
						}
					}
					default: return true;
				}
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
			}
			case 6:
			{
				if (codepoint < U'\U00016AF0' || codepoint > U'\U00016FF1')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U00016AF0', U'\U00016FF1');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x16AF0u) / 62u)
				{
					case 0: return true;
					case 1: return true;
					case 18: return codepoint != U'\U00016F4C';
					case 19: return true;
					case 20: return codepoint <= U'\U00016FE4' || codepoint >= U'\U00016FF0';
					default: return false;
				}
			}
			case 7: return true;
			case 8:
			{
				if (codepoint < U'\U0001D165' || codepoint > U'\U0001E94A')
					return false;

				TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001D165', U'\U0001E94A');
				switch ((static_cast<uint_least32_t>(codepoint) - 0x1D165u) / 765u)
				{
					case 0:
					{
						if (codepoint > U'\U0001D244')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001D165', U'\U0001D244');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1D165u) / 56u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1D165ull)) & 0x7F3FC03F1Full;
							case 2: return false;
							default: return true;
						}
					}
					case 2: return codepoint <= U'\U0001DA36' || codepoint >= U'\U0001DA3B';
					case 3:
					{
						if (codepoint < U'\U0001DA5C' || codepoint > U'\U0001DAAF')
							return false;

						TOML_ASSUME_CODEPOINT_BETWEEN(U'\U0001DA5C', U'\U0001DAAF');
						switch ((static_cast<uint_least32_t>(codepoint) - 0x1DA5Cu) / 42u)
						{
							case 0: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1DA5Cull)) & 0x1000201FFFFull;
							case 1: return codepoint != U'\U0001DA86';
							TOML_NO_DEFAULT_CASE;
						}
					}
					case 4: return (1ull << (static_cast<uint_least64_t>(codepoint) - 0x1E000ull)) & 0x7DBF9FFFF7Full;
					case 5: return codepoint <= U'\U0001E136' || codepoint >= U'\U0001E2EC';
					case 7: return codepoint <= U'\U0001E8D6' || codepoint >= U'\U0001E944';
					default: return false;
				}
			}
			case 63: return true;
			default: return false;
		}
	}
}

#undef TOML_ASSUME_CODEPOINT_BETWEEN

#endif // TOML_LANG_HIGHER_THAN(0, 5, 0)

#pragma endregion
//-------------  ↑ toml_utf8_generated.h  ------------------------------------------------------------------------------

//-------------------------------------------  ↓ toml_utf8.h  ----------------------------------------------------------
#pragma region

namespace toml::impl
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
			constexpr optional<uint8_t> operator() () noexcept
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
			explicit utf8_byte_stream(std::basic_istream<CHAR>& stream)
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
			optional<uint8_t> operator() ()
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
		#if TOML_ABI_NAMESPACES
			inline namespace abi_impl_ex {
		#endif
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR			err.emplace
		#if TOML_ABI_NAMESPACES
			inline namespace abi_impl_noex {
		#endif
	#endif

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const source_path_ptr& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;

		#if !TOML_EXCEPTIONS

		[[nodiscard]]
		virtual optional<parse_error>&& error() noexcept = 0;

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
			utf8_codepoint codepoints[2];
			size_t cp_idx = 1;
			uint8_t current_byte_count{};
			source_path_ptr source_path_;
			#if !TOML_EXCEPTIONS
			optional<parse_error> err;
			#endif

		public:

			template <typename U, typename STR = std::string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				std::memset(codepoints, 0, sizeof(codepoints));
				codepoints[0].position = { 1, 1 };
				codepoints[1].position = { 1, 1 };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<STR>(source_path));
			}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
			{
				return source_path_;
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				TOML_ERROR_CHECK;

				auto& prev = codepoints[(cp_idx - 1_sz) % 2_sz];

				if (stream.eof())
					return nullptr;
				else if (stream.error())
					TOML_ERROR("An error occurred while reading from the underlying stream", prev.position, source_path_ );
				else if (decoder.error())
					TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

				TOML_ERROR_CHECK;

				while (true)
				{
					optional<uint8_t> nextByte;
					if constexpr (noexcept(stream()) || !TOML_EXCEPTIONS)
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

					auto& current = codepoints[cp_idx % 2_sz];
					current.bytes[current_byte_count++] = static_cast<string_char>(*nextByte);
					if (decoder.has_code_point())
					{
						//store codepoint
						current.value = decoder.codepoint;

						//reset prev (will be the next 'current')
						std::memset(prev.bytes, 0, sizeof(prev.bytes));
						current_byte_count = {};
						if (is_line_break<false>(current.value))
							prev.position = { static_cast<source_index>(current.position.line + 1), 1 };
						else
							prev.position = { current.position.line, static_cast<source_index>(current.position.column + 1) };
						cp_idx++;
						return &current;
					}
				}

				TOML_UNREACHABLE;
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
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
			const utf8_codepoint* read_next() override
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
			optional<parse_error>&& error() noexcept override
			{
				return reader.error();
			}

			#endif
	};

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif
}

#pragma endregion
//-------------------------------------------  ↑ toml_utf8.h  ----------------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_parser.h  --------------------------------
#pragma region

namespace toml
{
	#if defined(DOXYGEN) || !TOML_EXCEPTIONS

	#if TOML_ABI_NAMESPACES
		inline namespace abi_parse_noex {
	#endif

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
					TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage))->~parse_error();
				else
					TOML_LAUNDER(reinterpret_cast<table*>(&storage))->~table();
			}

		public:

			[[nodiscard]] bool succeeded() const noexcept { return !is_err; }
			[[nodiscard]] bool failed() const noexcept { return is_err; }
			[[nodiscard]] explicit operator bool() const noexcept { return !is_err; }
			[[nodiscard]] table& get() & noexcept
			{
				TOML_ASSERT(!is_err);
				return *TOML_LAUNDER(reinterpret_cast<table*>(&storage));
			}
			[[nodiscard]] table&& get() && noexcept
			{
				TOML_ASSERT(!is_err);
				return std::move(*TOML_LAUNDER(reinterpret_cast<table*>(&storage)));
			}
			[[nodiscard]] const table& get() const& noexcept
			{
				TOML_ASSERT(!is_err);
				return *TOML_LAUNDER(reinterpret_cast<const table*>(&storage));
			}

			[[nodiscard]] parse_error& error() & noexcept
			{
				TOML_ASSERT(is_err);
				return *TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage));
			}
			[[nodiscard]] parse_error&& error() && noexcept
			{
				TOML_ASSERT(is_err);
				return std::move(*TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage)));
			}
			[[nodiscard]] const parse_error& error() const& noexcept
			{
				TOML_ASSERT(is_err);
				return *TOML_LAUNDER(reinterpret_cast<const parse_error*>(&storage));
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

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif

	#else

	using parse_result = table;

	#endif
}

namespace toml::impl
{
	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_impl_ex {
		#else
			inline namespace abi_impl_noex {
		#endif
	#endif

	[[nodiscard]] TOML_API
	parse_result do_parse(utf8_reader_interface&&) TOML_MAY_THROW;

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif
}

namespace toml
{
	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_parse_ex {
		#else
			inline namespace abi_parse_noex {
		#endif
	#endif

	[[nodiscard]] TOML_API
	parse_result parse(std::string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	[[nodiscard]] TOML_API
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#if defined(__cpp_lib_char8_t)

	[[nodiscard]] TOML_API
	parse_result parse(std::u8string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	[[nodiscard]] TOML_API
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#endif // defined(__cpp_lib_char8_t)

	template <typename CHAR>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<CHAR>& doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	template <typename CHAR>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<CHAR>& doc, std::string&& source_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	template <typename CHAR>
	inline parse_result parse_file(std::basic_string_view<CHAR> file_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The path's character type must be 1 byte in size."
		);

		auto str = std::string(reinterpret_cast<const char*>(file_path.data()), file_path.length());
		auto ifs = std::basic_ifstream<CHAR>{ str };
		return parse( ifs, std::move(str) );
	}

	// Q: "why are the parse_file functions templated??"
	// A: I don't want to force users to drag in <fstream> if they're not going to do
	//    any parsing directly from files. Keeping them templated delays their instantiation
	//    until they're actually required, so only those users wanting to use parse_file()
	//    are burdened by the <fstream> overhead.

	template <typename CHAR>
	inline parse_result parse_file(const std::basic_string<CHAR>& file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<CHAR>{ file_path });
	}

	template <typename CHAR>
	inline parse_result parse_file(const CHAR* file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<CHAR>{ file_path });
	}

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif

	inline namespace literals
	{
		#if TOML_ABI_NAMESPACES
			#if TOML_EXCEPTIONS
				inline namespace abi_lit_ex {
			#else
				inline namespace abi_lit_noex {
			#endif
		#endif

		[[nodiscard]] TOML_API
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW;

		#if defined(__cpp_lib_char8_t)

		[[nodiscard]] TOML_API
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW;

		#endif

		#if TOML_ABI_NAMESPACES
			} //end abi namespace for TOML_EXCEPTIONS
		#endif
	}
}

#pragma endregion
//-------------------------------------------------------------------  ↑ toml_parser.h  --------------------------------

//------------------------------------------------------------------------------------------  ↓ toml_formatter.h  ------
#pragma region

namespace toml
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

namespace toml::impl
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

			void print_newline(bool force = false)
			{
				if (!naked_newline_ || force)
				{
					print_to_stream('\n', *stream_);
					naked_newline_ = true;
				}
			}

			void print_indent()
			{
				for (int8_t i = 0; i < indent_; i++)
				{
					print_to_stream(indent_string, *stream_);
					naked_newline_ = false;
				}
			}

			void print_quoted_string(toml::string_view str)
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
			void print(const value<T>& val)
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

			void print(const node& val_node, node_type type)
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

#pragma endregion
//------------------------------------------------------------------------------------------  ↑ toml_formatter.h  ------

//-----------  ↓ toml_default_formatter.h  -----------------------------------------------------------------------------
#pragma region

namespace toml::impl
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

namespace toml
{
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&);
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&&);

	template <typename CHAR = char>
	class default_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;
			std::vector<toml::string> key_path;

			void print_key_segment(const toml::string& str)
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

			void print_key_path()
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						impl::print_to_stream('.', base::stream());
					impl::print_to_stream(segment, base::stream());
				}
				base::clear_naked_newline();
			}

			inline void print_inline(const table& /*tbl*/);
			void print(const array& arr)
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

			void print(const table& tbl)
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

			void print()
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

			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&);
			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&&);
	};

	default_formatter(const table&) -> default_formatter<char>;
	default_formatter(const array&) -> default_formatter<char>;
	template <typename T> default_formatter(const value<T>&) -> default_formatter<char>;

	template <typename CHAR>
	inline void default_formatter<CHAR>::print_inline(const toml::table& tbl)
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

	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, default_formatter<U>& rhs)
	{
		rhs.attach(lhs);
		rhs.key_path.clear();
		rhs.print();
		rhs.detach();
		return lhs;
	}

	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, default_formatter<U>&& rhs)
	{
		return lhs << rhs; //as lvalue
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs)
	{
		return lhs << default_formatter<CHAR>{ rhs };
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const array& rhs)
	{
		return lhs << default_formatter<CHAR>{ rhs };
	}
}

#pragma endregion
//-----------  ↑ toml_default_formatter.h  -----------------------------------------------------------------------------

//--------------------------------------  ↓ toml_json_formatter.h  -----------------------------------------------------
#pragma region

namespace toml
{
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&);
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&&);

	template <typename CHAR = char>
	class json_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;

			inline void print(const toml::table& tbl);
			void print(const array& arr)
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

			void print()
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
				explicit json_formatter(const toml::node& source, format_flags flags = {}) noexcept
				: base{ source, flags | format_flags::quote_dates_and_times }
			{}

			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&);
			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&&);
	};

	json_formatter(const table&) -> json_formatter<char>;
	json_formatter(const array&) -> json_formatter<char>;
	template <typename T> json_formatter(const value<T>&) -> json_formatter<char>;

	template <typename CHAR>
	inline void json_formatter<CHAR>::print(const toml::table& tbl)
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

	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>& rhs)
	{
		rhs.attach(lhs);
		rhs.print();
		rhs.detach();
		return lhs;
	}

	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>&& rhs)
	{
		return lhs << rhs; //as lvalue
	}
}

#pragma endregion
//--------------------------------------  ↑ toml_json_formatter.h  -----------------------------------------------------

#if TOML_IMPLEMENTATION

//---------------------------------------------------------------  ↓ toml_instantiations.h  ----------------------------
#pragma region

namespace toml
{
	template class TOML_API value<string>;
	template class TOML_API value<int64_t>;
	template class TOML_API value<double>;
	template class TOML_API value<bool>;
	template class TOML_API value<date>;
	template class TOML_API value<time>;
	template class TOML_API value<date_time>;

	template class TOML_API node_view<node>;
	template class TOML_API node_view<const node>;
}

#pragma endregion
//---------------------------------------------------------------  ↑ toml_instantiations.h  ----------------------------

//------------------------------------------------------------------------------------------  ↓ toml_node_impl.h  ------
#pragma region

namespace toml
{
	TOML_INLINE_FUNC_IMPL
	node::node(node && other) noexcept
		: source_{ std::move(other.source_) }
	{
		other.source_.begin = {};
		other.source_.end = {};
	}

	TOML_INLINE_FUNC_IMPL
	node & node::operator= (node && rhs) noexcept
	{
		source_ = std::move(rhs.source_);
		rhs.source_.begin = {};
		rhs.source_.end = {};
		return *this;
	}

	TOML_INLINE_FUNC_IMPL bool node::is_string() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_integer() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_floating_point() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_number() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_boolean() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_date() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_time() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_date_time() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool node::is_array_of_tables() const noexcept { return false; }

	TOML_INLINE_FUNC_IMPL table* node::as_table() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL array* node::as_array() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<string>* node::as_string() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<int64_t>* node::as_integer() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<double>* node::as_floating_point() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<bool>* node::as_boolean() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<date>* node::as_date() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<time>* node::as_time() noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL value<date_time>* node::as_date_time() noexcept { return nullptr; }

	TOML_INLINE_FUNC_IMPL const table* node::as_table() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const array* node::as_array() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<string>* node::as_string() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<int64_t>* node::as_integer() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<double>* node::as_floating_point() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<bool>* node::as_boolean() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<date>* node::as_date() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<time>* node::as_time() const noexcept { return nullptr; }
	TOML_INLINE_FUNC_IMPL const value<date_time>* node::as_date_time() const noexcept { return nullptr; }

	TOML_INLINE_FUNC_IMPL const source_region& node::source() const noexcept { return source_; }
}

#pragma endregion
//------------------------------------------------------------------------------------------  ↑ toml_node_impl.h  ------

//---------------  ↓ toml_array_impl.h  --------------------------------------------------------------------------------
#pragma region

namespace toml
{
	TOML_INLINE_FUNC_IMPL
	void array::preinsertion_resize(size_t idx, size_t count) noexcept
	{
		const auto new_size = values.size() + count;
		const auto inserting_at_end = idx == values.size();
		values.resize(new_size);
		if (!inserting_at_end)
		{
			for (size_t r = new_size, e = idx + count, l = e; r-- > e; l--)
				values[r] = std::move(values[l]);
		}
	}

	TOML_INLINE_FUNC_IMPL
	array::array() noexcept = default;

	TOML_INLINE_FUNC_IMPL
	array::array(array&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) }
	{}

	TOML_INLINE_FUNC_IMPL
	array& array::operator= (array&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		return *this;
	}

	TOML_INLINE_FUNC_IMPL node_type array::type() const noexcept { return node_type::array; }
	TOML_INLINE_FUNC_IMPL bool array::is_table() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool array::is_array() const noexcept { return true; }
	TOML_INLINE_FUNC_IMPL bool array::is_value() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL array* array::as_array() noexcept { return this; }
	TOML_INLINE_FUNC_IMPL const array* array::as_array() const noexcept { return this; }

	TOML_INLINE_FUNC_IMPL node& array::operator[] (size_t index) noexcept { return *values[index]; }
	TOML_INLINE_FUNC_IMPL const node& array::operator[] (size_t index) const noexcept { return *values[index]; }

	TOML_INLINE_FUNC_IMPL node& array::front() noexcept { return *values.front(); }
	TOML_INLINE_FUNC_IMPL const node& array::front() const noexcept { return *values.front(); }
	TOML_INLINE_FUNC_IMPL node& array::back() noexcept { return *values.back(); }
	TOML_INLINE_FUNC_IMPL const node& array::back() const noexcept { return *values.back(); }

	TOML_INLINE_FUNC_IMPL array::iterator array::begin() noexcept { return { values.begin() }; }
	TOML_INLINE_FUNC_IMPL array::const_iterator array::begin() const noexcept { return { values.begin() }; }
	TOML_INLINE_FUNC_IMPL array::const_iterator array::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_INLINE_FUNC_IMPL array::iterator array::end() noexcept { return { values.end() }; }
	TOML_INLINE_FUNC_IMPL array::const_iterator array::end() const noexcept { return { values.end() }; }
	TOML_INLINE_FUNC_IMPL array::const_iterator array::cend() const noexcept { return { values.cend() }; }

	TOML_INLINE_FUNC_IMPL bool array::empty() const noexcept { return values.empty(); }
	TOML_INLINE_FUNC_IMPL size_t array::size() const noexcept { return values.size(); }
	TOML_INLINE_FUNC_IMPL void array::reserve(size_t new_capacity) { values.reserve(new_capacity); }
	TOML_INLINE_FUNC_IMPL void array::clear() noexcept { values.clear(); }

	TOML_INLINE_FUNC_IMPL
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_INLINE_FUNC_IMPL
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_INLINE_FUNC_IMPL
	void array::pop_back() noexcept
	{
		values.pop_back();
	}

	TOML_INLINE_FUNC_IMPL
	node* array::get(size_t index) noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_INLINE_FUNC_IMPL
	const node* array::get(size_t index) const noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_API
	TOML_INLINE_FUNC_IMPL
	bool operator == (const array& lhs, const array& rhs) noexcept
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

	TOML_API
	TOML_INLINE_FUNC_IMPL
	bool operator != (const array& lhs, const array& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	TOML_INLINE_FUNC_IMPL
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (size_t i = 0, e = values.size(); i < e; i++)
		{
			auto arr = values[i]->as_array();
			leaves += arr ? arr->total_leaf_count() : 1_sz;
		}
		return leaves;
	}

	TOML_INLINE_FUNC_IMPL
	void array::flatten_child(array&& child, size_t& dest_index) noexcept
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

	TOML_INLINE_FUNC_IMPL
	void array::flatten()
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
}

#pragma endregion
//---------------  ↑ toml_array_impl.h  --------------------------------------------------------------------------------

//----------------------------------------  ↓ toml_table_impl.h  -------------------------------------------------------
#pragma region

namespace toml
{
	TOML_INLINE_FUNC_IMPL
	table::table(impl::table_init_pair* pairs, size_t count) noexcept
	{
		for (size_t i = 0; i < count; i++)
		{
			values.insert_or_assign(
				std::move(pairs[i].key),
				std::move(pairs[i].value)
			);
		}
	}

	TOML_INLINE_FUNC_IMPL
	table::table() noexcept {}

	TOML_INLINE_FUNC_IMPL
	table::table(table&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) },
		inline_{ other.inline_ }
	{}

	TOML_INLINE_FUNC_IMPL
	table& table::operator = (table&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		inline_ = rhs.inline_;
		return *this;
	}

	TOML_INLINE_FUNC_IMPL node_type table::type() const noexcept { return node_type::table; }
	TOML_INLINE_FUNC_IMPL bool table::is_table() const noexcept { return true; }
	TOML_INLINE_FUNC_IMPL bool table::is_array() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL bool table::is_value() const noexcept { return false; }
	TOML_INLINE_FUNC_IMPL table* table::as_table() noexcept { return this; }
	TOML_INLINE_FUNC_IMPL const table* table::as_table() const noexcept { return this; }
	TOML_INLINE_FUNC_IMPL bool table::is_inline() const noexcept { return inline_; }
	TOML_INLINE_FUNC_IMPL void table::is_inline(bool val) noexcept { inline_ = val; }

	TOML_INLINE_FUNC_IMPL
	node_view<node> table::operator[] (string_view key) noexcept
	{
		return { this->get(key) };
	}
	TOML_INLINE_FUNC_IMPL
	node_view<const node> table::operator[] (string_view key) const noexcept
	{
		return { this->get(key) };
	}

	TOML_INLINE_FUNC_IMPL table::iterator table::begin() noexcept { return { values.begin() }; }
	TOML_INLINE_FUNC_IMPL table::const_iterator table::begin() const noexcept { return { values.begin() }; }
	TOML_INLINE_FUNC_IMPL table::const_iterator table::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_INLINE_FUNC_IMPL table::iterator table::end() noexcept { return { values.end() }; }
	TOML_INLINE_FUNC_IMPL table::const_iterator table::end() const noexcept { return { values.end() }; }
	TOML_INLINE_FUNC_IMPL table::const_iterator table::cend() const noexcept { return { values.cend() }; }

	TOML_INLINE_FUNC_IMPL bool table::empty() const noexcept { return values.empty(); }
	TOML_INLINE_FUNC_IMPL size_t table::size() const noexcept { return values.size(); }
	TOML_INLINE_FUNC_IMPL void table::clear() noexcept { values.clear(); }

	TOML_INLINE_FUNC_IMPL
	table::iterator table::erase(iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_INLINE_FUNC_IMPL
	table::iterator table::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_INLINE_FUNC_IMPL
	table::iterator table::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_INLINE_FUNC_IMPL
	bool table::erase(string_view key) noexcept
	{
		if (auto it = values.find(key); it != values.end())
		{
			values.erase(it);
			return true;
		}
		return false;
	}

	TOML_INLINE_FUNC_IMPL
	node* table::get(string_view key) noexcept
	{
		return do_get(values, key);
	}

	TOML_INLINE_FUNC_IMPL
	const node* table::get(string_view key) const noexcept
	{
		return do_get(values, key);
	}

	TOML_INLINE_FUNC_IMPL
	table::iterator table::find(string_view key) noexcept
	{
		return { values.find(key) };
	}

	TOML_INLINE_FUNC_IMPL
	table::const_iterator table::find(string_view key) const noexcept
	{
		return { values.find(key) };
	}

	TOML_INLINE_FUNC_IMPL
	bool table::contains(string_view key) const noexcept
	{
		return do_contains(values, key);
	}

	TOML_API
	TOML_INLINE_FUNC_IMPL
	bool operator == (const table& lhs, const table& rhs) noexcept
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

	TOML_API
	TOML_INLINE_FUNC_IMPL
	bool operator != (const table& lhs, const table& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

#pragma endregion
//----------------------------------------  ↑ toml_table_impl.h  -------------------------------------------------------

//----------------------------------------------------------------  ↓ toml_parser_impl.h  ------------------------------
#pragma region

namespace toml::impl
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

	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_impl_ex {
		#else
			inline namespace abi_impl_noex {
		#endif
	#endif

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
			mutable optional<toml::parse_error> err;
			#endif

			[[nodiscard]]
			source_position current_position(source_index fallback_offset = 0) const noexcept
			{
				if (cp)
					return cp->position;
				return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
			}

			template <typename T, size_t N>
			static void abort_with_error_concatenate(T&& arg, char(& buf)[N], char*& ptr) noexcept
			{
				(void)buf;

				using arg_t = remove_cvref_t<T>;
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
						std::ostringstream ss;
						ss.precision(std::numeric_limits<arg_t>::digits10 + 1);
						ss << arg;
						const auto str = std::move(ss).str();
						std::memcpy(ptr, str.c_str(), str.length());
						ptr += str.length();
					}
					#else
					{
						const auto result = std::to_chars(ptr, buf + N, arg);
						ptr = result.ptr;
					}
					#endif
				}
				else if constexpr (std::is_integral_v<arg_t>)
				{
					const auto result = std::to_chars(ptr, buf + N, arg);
					ptr = result.ptr;
				}
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
					char buf[buf_size];
					auto ptr = buf;
					(abort_with_error_concatenate(std::forward<T>(args), buf, ptr), ...);
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
							recording_buffer.begin() + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
							recording_buffer.end()
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
							case U'x':
								#if TOML_LANG_HIGHER_THAN(0, 5, 0)
									[[fallthrough]];
								#else
									abort_with_error("Escape sequence '\\x' is not supported "
										"in TOML 0.5.0 and earlier."sv
									);
									break;
								#endif
							case U'u': [[fallthrough]];
							case U'U':
							{
								uint32_t place_value = escaped_codepoint == U'U' ? 0x10000000u : (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
								uint32_t sequence_value{};
								while (place_value)
								{
									eof_check();
									TOML_ERROR_CHECK({});

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
				double result;
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
								"; an unspecified error occurred while trying to interpret '"sv,
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
				double result;
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::string str;
					{
						std::ostringstream ss;
						ss.write("0x", 2_sz);
						ss.write(chars, static_cast<std::streamsize>(length));
						str = std::move(ss).str();
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
								"; an unspecified error occurred while trying to interpret '"sv,
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
				char chars[traits::buffer_length];
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
				uint64_t result;
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
							"; an unspecified error occurred while trying to interpret '"sv,
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
				uint32_t year_digits[4];
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
				uint32_t month_digits[2];
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
				uint32_t day_digits[2];
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
				uint32_t digits[2];
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
				uint32_t fractional_digits[max_fractional_digits];
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
				time_offset offset;
				bool has_offset = false;
				if (cp)
				{
					// zero offset ("Z")
					if (*cp == U'Z' || *cp == U'z')
					{
						has_offset = true;
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
						int digits[2];
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

						has_offset = true;
						offset.minutes = static_cast<int16_t>((hour * 60 + minute) * sign);
					}
				}

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::date_time,
						"; expected value-terminator, saw '"sv, *cp, '\''
					);

				TOML_ERROR_CHECK({});
				if (has_offset)
					return { date, time, offset };
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
						if (auto found = find(implicit_tables, tbl))
						{
							implicit_tables.erase(implicit_tables.cbegin() + (found - implicit_tables.data()));
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

	TOML_API
	TOML_INLINE_FUNC_IMPL
	parse_result do_parse(utf8_reader_interface&& reader) TOML_MAY_THROW
	{
		return impl::parser{ std::move(reader) };
	}

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif
}

namespace toml
{
	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_parse_ex {
		#else
			inline namespace abi_parse_noex {
		#endif
	#endif

	TOML_API
	TOML_INLINE_FUNC_IMPL
	parse_result parse(std::string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_INLINE_FUNC_IMPL
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#if defined(__cpp_lib_char8_t)

	TOML_API
	TOML_INLINE_FUNC_IMPL
	parse_result parse(std::u8string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_INLINE_FUNC_IMPL
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#endif // defined(__cpp_lib_char8_t)

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif

	inline namespace literals
	{
		#if TOML_ABI_NAMESPACES
			#if TOML_EXCEPTIONS
				inline namespace abi_lit_ex {
			#else
				inline namespace abi_lit_noex {
			#endif
		#endif

		TOML_API
		TOML_INLINE_FUNC_IMPL
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::string_view{ str, len });
		}

		#if defined(__cpp_lib_char8_t)

		TOML_API
		TOML_INLINE_FUNC_IMPL
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::u8string_view{ str, len });
		}

		#endif // defined(__cpp_lib_char8_t)

		#if TOML_ABI_NAMESPACES
			} //end abi namespace for TOML_EXCEPTIONS
		#endif
	}
}

#pragma endregion
//----------------------------------------------------------------  ↑ toml_parser_impl.h  ------------------------------

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

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
// clang-format on
