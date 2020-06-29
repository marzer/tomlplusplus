//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v1.3.3
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
// TOML language specifications:
// Latest:      https://github.com/toml-lang/toml/blob/master/README.md
// v1.0.0-rc.1: https://toml.io/en/v1.0.0-rc.1
// v0.5.0:      https://toml.io/en/v0.5.0
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
#ifndef INCLUDE_TOMLPLUSPLUS_H
#define INCLUDE_TOMLPLUSPLUS_H

#define	TOML_LIB_SINGLE_HEADER 1

//--------------  ↓ toml_preprocessor.h  -------------------------------------------------------------------------------
#if 1

#ifndef TOML_DOXYGEN
	#define TOML_DOXYGEN 0
#endif

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
#endif

#if !defined(TOML_ALL_INLINE) || (defined(TOML_ALL_INLINE) && TOML_ALL_INLINE) || defined(__INTELLISENSE__)
	#undef TOML_ALL_INLINE
	#define TOML_ALL_INLINE 1
#endif

#if TOML_DOXYGEN
	#undef TOML_ALL_INLINE
	#define TOML_ALL_INLINE 0
#endif

#if defined(TOML_IMPLEMENTATION) || TOML_ALL_INLINE
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
	#define TOML_UNRELEASED_FEATURES 0
#endif

#ifndef TOML_LARGE_FILES
	#define TOML_LARGE_FILES 0
#endif

#ifndef TOML_UNDEF_MACROS
	#define TOML_UNDEF_MACROS 1
#endif

#ifndef TOML_PARSER
	#define TOML_PARSER 1
#endif

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif
#ifdef __clang__

	#define TOML_PUSH_WARNINGS				_Pragma("clang diagnostic push")
	#define TOML_DISABLE_SWITCH_WARNINGS	_Pragma("clang diagnostic ignored \"-Wswitch\"")
	#define TOML_DISABLE_INIT_WARNINGS		_Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")
	#define TOML_DISABLE_VTABLE_WARNINGS	_Pragma("clang diagnostic ignored \"-Weverything\"") \
											_Pragma("clang diagnostic ignored \"-Wweak-vtables\"")
	#define TOML_DISABLE_PADDING_WARNINGS	_Pragma("clang diagnostic ignored \"-Wpadded\"")
	#define TOML_DISABLE_FLOAT_WARNINGS		_Pragma("clang diagnostic ignored \"-Wfloat-equal\"") \
											_Pragma("clang diagnostic ignored \"-Wdouble-promotion\"")
	#define TOML_DISABLE_SHADOW_WARNINGS	_Pragma("clang diagnostic ignored \"-Wshadow\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("clang diagnostic ignored \"-Weverything\"")
	#define TOML_POP_WARNINGS				_Pragma("clang diagnostic pop")

	#define TOML_ASSUME(cond)				__builtin_assume(cond)
	#define TOML_UNREACHABLE				__builtin_unreachable()
	#define TOML_GNU_ATTR(...)				__attribute__((__VA_ARGS__))
	#if defined(_MSC_VER) // msvc compat mode
		#ifdef __has_declspec_attribute
			#if __has_declspec_attribute(novtable)
				#define TOML_INTERFACE		__declspec(novtable)
			#endif
			#if __has_declspec_attribute(empty_bases)
				#define TOML_EMPTY_BASES	__declspec(empty_bases)
			#endif
			#ifndef TOML_ALWAYS_INLINE
				#define TOML_ALWAYS_INLINE	__forceinline
			#endif
			#if __has_declspec_attribute(noinline)
				#define TOML_NEVER_INLINE	__declspec(noinline)
			#endif
		#endif
	#endif
	#ifdef __has_attribute
		#if !defined(TOML_ALWAYS_INLINE) && __has_attribute(always_inline)
			#define TOML_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
		#endif
		#if !defined(TOML_NEVER_INLINE) && __has_attribute(noinline)
			#define TOML_NEVER_INLINE		__attribute__((__noinline__))
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
	#define TOML_LIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 1) )
	#define TOML_UNLIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 0) )

	//floating-point from_chars and to_chars are not implemented in any version of clang as of 1/1/2020
	#ifndef TOML_FLOAT_CHARCONV
		#define TOML_FLOAT_CHARCONV 0
	#endif

#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(__ICL))

	#define TOML_PUSH_WARNINGS				__pragma(warning(push))
	#define TOML_DISABLE_SWITCH_WARNINGS	__pragma(warning(disable: 4063))
	#define TOML_DISABLE_ALL_WARNINGS		__pragma(warning(pop))	\
											__pragma(warning(push, 0))
	#define TOML_POP_WARNINGS				__pragma(warning(pop))

	#define TOML_CPP_VERSION				_MSVC_LANG
	#ifndef TOML_ALWAYS_INLINE
		#define TOML_ALWAYS_INLINE			__forceinline
	#endif
	#define TOML_NEVER_INLINE				__declspec(noinline)
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
	#define TOML_DISABLE_SWITCH_WARNINGS	_Pragma("GCC diagnostic ignored \"-Wswitch\"")						\
											_Pragma("GCC diagnostic ignored \"-Wswitch-enum\"")					\
											_Pragma("GCC diagnostic ignored \"-Wswitch-default\"")
	#define TOML_DISABLE_INIT_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")	\
											_Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")			\
											_Pragma("GCC diagnostic ignored \"-Wuninitialized\"")
	#define TOML_DISABLE_PADDING_WARNINGS	_Pragma("GCC diagnostic ignored \"-Wpadded\"")
	#define TOML_DISABLE_FLOAT_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
	#define TOML_DISABLE_SHADOW_WARNINGS	_Pragma("GCC diagnostic ignored \"-Wshadow\"")
	#define TOML_DISABLE_ALL_WARNINGS		_Pragma("GCC diagnostic ignored \"-Wall\"")							\
											_Pragma("GCC diagnostic ignored \"-Wextra\"")						\
											_Pragma("GCC diagnostic ignored \"-Wchar-subscripts\"")				\
											_Pragma("GCC diagnostic ignored \"-Wtype-limits\"")					\
											TOML_DISABLE_SWITCH_WARNINGS										\
											TOML_DISABLE_INIT_WARNINGS											\
											TOML_DISABLE_PADDING_WARNINGS										\
											TOML_DISABLE_FLOAT_WARNINGS											\
											TOML_DISABLE_SHADOW_WARNINGS
	#define TOML_POP_WARNINGS				_Pragma("GCC diagnostic pop")

	#define TOML_GNU_ATTR(...)				__attribute__((__VA_ARGS__))
	#ifndef TOML_ALWAYS_INLINE
		#define TOML_ALWAYS_INLINE			__attribute__((__always_inline__)) inline
	#endif
	#define TOML_NEVER_INLINE				__attribute__((__noinline__))
	#define TOML_UNREACHABLE				__builtin_unreachable()
	#if !defined(TOML_RELOPS_REORDERING) && defined(__cpp_impl_three_way_comparison)
		#define TOML_RELOPS_REORDERING 1
	#endif
	#ifdef __cpp_exceptions
		#define TOML_COMPILER_EXCEPTIONS 1
	#else
		#define TOML_COMPILER_EXCEPTIONS 0
	#endif
	#define TOML_LIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 1) )
	#define TOML_UNLIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 0) )

	// floating-point from_chars and to_chars are not implemented in any version of gcc as of 1/1/2020
	#ifndef TOML_FLOAT_CHARCONV
		#define TOML_FLOAT_CHARCONV 0
	#endif

#endif

#ifndef TOML_CPP_VERSION
	#define TOML_CPP_VERSION __cplusplus
#endif

#if TOML_CPP_VERSION < 201103L
	#error toml++ requires C++17 or higher. For a TOML library supporting pre-C++11 see https://github.com/ToruNiina/Boost.toml
#elif TOML_CPP_VERSION < 201703L
	#error toml++ requires C++17 or higher. For a TOML library supporting C++11 see https://github.com/ToruNiina/toml11
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

#ifndef TOML_INT_CHARCONV
	#define TOML_INT_CHARCONV 1
#endif
#ifndef TOML_FLOAT_CHARCONV
	#define TOML_FLOAT_CHARCONV 1
#endif
#if (TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV) && !__has_include(<charconv>)
	#undef TOML_INT_CHARCONV
	#undef TOML_FLOAT_CHARCONV
	#define TOML_INT_CHARCONV 0
	#define TOML_FLOAT_CHARCONV 0
#endif

#ifndef TOML_PUSH_WARNINGS
	#define TOML_PUSH_WARNINGS
#endif
#ifndef TOML_DISABLE_SWITCH_WARNINGS
	#define	TOML_DISABLE_SWITCH_WARNINGS
#endif
#ifndef TOML_DISABLE_INIT_WARNINGS
	#define	TOML_DISABLE_INIT_WARNINGS
#endif
#ifndef TOML_DISABLE_VTABLE_WARNINGS
	#define TOML_DISABLE_VTABLE_WARNINGS
#endif
#ifndef TOML_DISABLE_PADDING_WARNINGS
	#define TOML_DISABLE_PADDING_WARNINGS
#endif
#ifndef TOML_DISABLE_FLOAT_WARNINGS
	#define TOML_DISABLE_FLOAT_WARNINGS
#endif
#ifndef TOML_DISABLE_SHADOW_WARNINGS
	#define TOML_DISABLE_SHADOW_WARNINGS
#endif
#ifndef TOML_DISABLE_ALL_WARNINGS
	#define TOML_DISABLE_ALL_WARNINGS
#endif
#ifndef TOML_POP_WARNINGS
	#define TOML_POP_WARNINGS
#endif

#ifndef TOML_GNU_ATTR
	#define TOML_GNU_ATTR(...)
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

#ifndef TOML_NEVER_INLINE
	#define TOML_NEVER_INLINE
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

#if !TOML_DOXYGEN && !defined(__INTELLISENSE__)
	#if !defined(TOML_LIKELY) && __has_cpp_attribute(likely)
		#define TOML_LIKELY(...)	(__VA_ARGS__) [[likely]]
	#endif
	#if !defined(TOML_UNLIKELY) && __has_cpp_attribute(unlikely)
		#define TOML_UNLIKELY(...)	(__VA_ARGS__) [[unlikely]]
	#endif
	#if __has_cpp_attribute(nodiscard) >= 201907L
		#define TOML_NODISCARD_CTOR [[nodiscard]]
	#endif
#endif

#ifndef TOML_LIKELY
	#define TOML_LIKELY(...)	(__VA_ARGS__)
#endif
#ifndef TOML_UNLIKELY
	#define TOML_UNLIKELY(...)	(__VA_ARGS__)
#endif
#ifndef TOML_NODISCARD_CTOR
	#define TOML_NODISCARD_CTOR
#endif

#ifndef TOML_TRIVIAL_ABI
	#define TOML_TRIVIAL_ABI
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
	#define TOML_EXTERNAL_LINKAGE	inline
	#define TOML_INTERNAL_LINKAGE	inline
	#define TOML_INTERNAL_NAMESPACE toml::impl
#else
	#define TOML_EXTERNAL_LINKAGE
	#define TOML_INTERNAL_LINKAGE	static
	#define TOML_INTERNAL_NAMESPACE
#endif

#define TOML_LIB_MAJOR		1
#define TOML_LIB_MINOR		3
#define TOML_LIB_PATCH		3

#define TOML_LANG_MAJOR		1
#define TOML_LANG_MINOR		0
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

#define TOML_LANG_UNRELEASED														\
		TOML_LANG_HIGHER_THAN(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)

#if TOML_DOXYGEN || defined(__INTELLISENSE__)
	#define TOML_ABI_NAMESPACES					0
	#define TOML_ABI_NAMESPACE_START(name)
	#define TOML_ABI_NAMESPACE_END
#else
	#define TOML_ABI_NAMESPACES					1
	#define TOML_ABI_NAMESPACE_START(name)		inline namespace abi_##name {
	#define TOML_ABI_NAMESPACE_END				}
#endif

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#ifndef TOML_ASSERT
	#if defined(NDEBUG) || !defined(_DEBUG)
		#define TOML_ASSERT(expr)	(void)0
	#else
		#include <cassert>
		#define TOML_ASSERT(expr)	assert(expr)
	#endif
#endif
TOML_POP_WARNINGS

#if TOML_CHAR_8_STRINGS
	#define TOML_STRING_PREFIX_1(S) u8##S
	#define TOML_STRING_PREFIX(S) TOML_STRING_PREFIX_1(S)
#else
	#define TOML_STRING_PREFIX(S) S
#endif

#endif
//--------------  ↑ toml_preprocessor.h  -------------------------------------------------------------------------------

//------------------------------------------  ↓ toml_common.h  ---------------------------------------------------------
#if 1

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

TOML_POP_WARNINGS

#if TOML_CHAR_8_STRINGS && !defined(__cpp_lib_char8_t)
	#error toml++ requires implementation support to use char8_t strings, but yours does not provide it.
#endif

#ifdef __cpp_lib_launder
	#define TOML_LAUNDER(x)	std::launder(x)
#else
	#define TOML_LAUNDER(x)	x
#endif

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

	using string_char = char;
	using string = std::string;
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

	TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS, TOML_DISABLE_SHADOW_WARNINGS

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

	#if TOML_LARGE_FILES
		TOML_ABI_NAMESPACE_START(lf)
	#else
		TOML_ABI_NAMESPACE_START(sf)
	#endif

	struct TOML_TRIVIAL_ABI source_position
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

	template <typename T>
	struct TOML_TRIVIAL_ABI inserter
	{
		T&& value;
	};
	template <typename T> inserter(T&&) -> inserter<T>;
}

#endif
//------------------------------------------  ↑ toml_common.h  ---------------------------------------------------------

//-----------------------------------------------------------------  ↓ toml_date_time.h  -------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	struct TOML_TRIVIAL_ABI date
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

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const date& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const date&);
	#endif

	struct TOML_TRIVIAL_ABI time
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

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const time&);
	#endif

	struct TOML_TRIVIAL_ABI time_offset
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

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const time_offset& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const time_offset&);
	#endif

	#ifdef TOML_OPTIONAL_TYPE
		TOML_ABI_NAMESPACE_START(custopt)
	#else
		TOML_ABI_NAMESPACE_START(stdopt)
	#endif

	struct date_time
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

	TOML_ABI_NAMESPACE_END // TOML_OPTIONAL_TYPE

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const date_time& rhs)
	{
		impl::print_to_stream(rhs, lhs);
		return lhs;
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const date_time&);
	#endif
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS

#endif
//-----------------------------------------------------------------  ↑ toml_date_time.h  -------------------------------

//---------------------------------------------------------------------------------------  ↓ toml_print_to_stream.h  ---
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <cmath>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
	#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
	#include <sstream>
#endif
#if !TOML_INT_CHARCONV
	#include <iomanip>
#endif
TOML_POP_WARNINGS

namespace toml::impl
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm supporting C++20's char8_t as well; wrapping streams allows switching string modes transparently.
	//    - I'm using <charconv> to format numerics. Faster and locale-independent.
	//    - I can avoid forcing users to drag in <sstream> and <iomanip>.

	// Q: "there's a bit of reinterpret_casting here, is any of it UB?"
	// A: - If the source string data is char and the output string is char8_t, then technically yes,
	//      but not in the other direction. I test in both modes on Clang, GCC and MSVC and have yet to
	//      see it actually causing an issue, but in the event it does present a problem it's not going to
	//      be a show-stopper since all it means is I need to do duplicate some code.
	//    - Strings in C++. Honestly.

	template <typename Char1, typename Char2>
	TOML_ALWAYS_INLINE
	void print_to_stream(std::basic_string_view<Char1> str, std::basic_ostream<Char2>& stream)
	{
		static_assert(sizeof(Char1) == 1);
		static_assert(sizeof(Char2) == 1);
		stream.write(reinterpret_cast<const Char2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename Char1, typename Char2>
	TOML_ALWAYS_INLINE
	void print_to_stream(const std::basic_string<Char1>& str, std::basic_ostream<Char2>& stream)
	{
		static_assert(sizeof(Char1) == 1);
		static_assert(sizeof(Char2) == 1);
		stream.write(reinterpret_cast<const Char2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename Char>
	TOML_ALWAYS_INLINE
	void print_to_stream(char character, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		stream.put(static_cast<Char>(character));
	}

	template <typename Char>
	TOML_GNU_ATTR(nonnull)
	TOML_ALWAYS_INLINE
	void print_to_stream(const char* str, size_t len, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		stream.write(reinterpret_cast<const Char*>(str), static_cast<std::streamsize>(len));
	}

	#if defined(__cpp_lib_char8_t)

	template <typename Char>
	TOML_ALWAYS_INLINE
	void print_to_stream(char8_t character, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		stream.put(static_cast<Char>(character));
	}

	template <typename Char>
	TOML_GNU_ATTR(nonnull)
	TOML_ALWAYS_INLINE
	void print_to_stream(const char8_t* str, size_t len, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		stream.write(reinterpret_cast<const Char*>(str), static_cast<std::streamsize>(len));
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

	template <typename T, typename Char>
	inline void print_integer_to_stream(T val, std::basic_ostream<Char>& stream)
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		#if TOML_INT_CHARCONV

			char buf[charconv_buffer_length<T>];
			const auto res = std::to_chars(buf, buf + sizeof(buf), val);
			const auto len = static_cast<size_t>(res.ptr - buf);
			print_to_stream(buf, len, stream);

		#else

			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			using cast_type = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
			ss << static_cast<cast_type>(val);
			const auto str = std::move(ss).str();
			print_to_stream(str, stream);

		#endif
	}

	#define TOML_P2S_OVERLOAD(Type)											\
		template <typename Char>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(Type val, std::basic_ostream<Char>& stream)	\
		{																	\
			static_assert(sizeof(Char) == 1);								\
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

	template <typename T, typename Char>
	TOML_EXTERNAL_LINKAGE
	void print_floating_point_to_stream(T val, std::basic_ostream<Char>& stream, bool hexfloat = false)
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		switch (std::fpclassify(val))
		{
			case FP_INFINITE:
				if (val < T{})
					print_to_stream('-', stream);
				print_to_stream("inf"sv, stream);
				return;

			case FP_NAN:
				print_to_stream("nan"sv, stream);
				return;

			default:
			{
				static constexpr auto needs_decimal_point = [](auto&& s) noexcept
				{
					for (auto c : s)
						if (c == '.' || c == 'E' || c == 'e')
							return false;
					return true;
				};

				#if TOML_FLOAT_CHARCONV
				{
					char buf[charconv_buffer_length<T>];
					const auto res = hexfloat
						? std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::hex)
						: std::to_chars(buf, buf + sizeof(buf), val);
					const auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
					print_to_stream(str, stream);
					if (!hexfloat && needs_decimal_point(str))
						print_to_stream(".0"sv, stream);
				}
				#else
				{
					std::ostringstream ss;
					ss.imbue(std::locale::classic());
					ss.precision(std::numeric_limits<T>::digits10 + 1);
					if (hexfloat)
						ss << std::hexfloat;
					ss << val;
					const auto str = std::move(ss).str();
					print_to_stream(str, stream);
					if (!hexfloat && needs_decimal_point(str))
						print_to_stream(".0"sv, stream);
				}
				#endif
			}
		}
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API void print_floating_point_to_stream(float, std::ostream&, bool);
		extern template TOML_API void print_floating_point_to_stream(double, std::ostream&, bool);
	#endif

	#define TOML_P2S_OVERLOAD(Type)											\
		template <typename Char>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(Type val, std::basic_ostream<Char>& stream)	\
		{																	\
			static_assert(sizeof(Char) == 1);								\
			print_floating_point_to_stream(val, stream);					\
		}

	TOML_P2S_OVERLOAD(float)
	TOML_P2S_OVERLOAD(double)

	#undef TOML_P2S_OVERLOAD

	template <typename Char>
	TOML_ALWAYS_INLINE
	void print_to_stream(bool val, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		print_to_stream(val ? "true"sv : "false"sv, stream);
	}

	template <typename T, typename Char>
	inline void print_to_stream(T val, std::basic_ostream<Char>& stream, size_t zero_pad_to_digits)
	{
		static_assert(sizeof(Char) == 1);
		#if TOML_INT_CHARCONV

			char buf[charconv_buffer_length<T>];
			const auto res = std::to_chars(buf, buf + sizeof(buf), val);
			const auto len = static_cast<size_t>(res.ptr - buf);
			for (size_t i = len; i < zero_pad_to_digits; i++)
				print_to_stream('0', stream);
			print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);

		#else

			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			using cast_type = std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>;
			ss << std::setfill('0') << std::setw(static_cast<int>(zero_pad_to_digits)) << static_cast<cast_type>(val);
			const auto str = std::move(ss).str();
			print_to_stream(str, stream);

		#endif
	}

	template <typename Char>
	inline void print_to_stream(const toml::date& val, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		print_to_stream(val.year, stream, 4_sz);
		print_to_stream('-', stream);
		print_to_stream(val.month, stream, 2_sz);
		print_to_stream('-', stream);
		print_to_stream(val.day, stream, 2_sz);
	}

	template <typename Char>
	inline void print_to_stream(const toml::time& val, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
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

	template <typename Char>
	inline void print_to_stream(toml::time_offset val, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
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

	template <typename Char>
	inline void print_to_stream(const toml::date_time& val, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		print_to_stream(val.date, stream);
		print_to_stream('T', stream);
		print_to_stream(val.time, stream);
		if (val.time_offset)
			print_to_stream(*val.time_offset, stream);
	}

	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	template <typename T, typename Char>
	void print_to_stream_with_escapes(T && str, std::basic_ostream<Char>& stream)
	{
		static_assert(sizeof(Char) == 1);
		for (auto c : str)
		{
			if TOML_UNLIKELY(c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F'))
				print_to_stream(low_character_escape_table[c], stream);
			else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('\x7F'))
				print_to_stream(TOML_STRING_PREFIX("\\u007F"sv), stream);
			else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('"'))
				print_to_stream(TOML_STRING_PREFIX("\\\""sv), stream);
			else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('\\'))
				print_to_stream(TOML_STRING_PREFIX("\\\\"sv), stream);
			else
				print_to_stream(c, stream);
		}
	}

	TOML_POP_WARNINGS
}

namespace toml
{
	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const source_position& rhs)
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);
		impl::print_to_stream("line "sv, lhs);
		impl::print_to_stream(rhs.line, lhs);
		impl::print_to_stream(", column "sv, lhs);
		impl::print_to_stream(rhs.column, lhs);
		return lhs;
	}

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const source_region& rhs)
	{
		static_assert(
			sizeof(Char) == 1,
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

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const source_position&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const source_region&);
	#endif
}

#endif
//---------------------------------------------------------------------------------------  ↑ toml_print_to_stream.h  ---

//------------------  ↓ toml_node.h  -----------------------------------------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_VTABLE_WARNINGS

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

			template <typename Func, typename N, typename T>
			static constexpr bool can_visit = std::is_invocable_v<Func, ref_cast_type<N, T>>;

			template <typename Func, typename N>
			static constexpr bool can_visit_any =
				can_visit<Func, N, table>
				|| can_visit<Func, N, array>
				|| can_visit<Func, N, string>
				|| can_visit<Func, N, int64_t>
				|| can_visit<Func, N, double>
				|| can_visit<Func, N, bool>
				|| can_visit<Func, N, date>
				|| can_visit<Func, N, time>
				|| can_visit<Func, N, date_time>;

			template <typename Func, typename N>
			static constexpr bool can_visit_all =
				can_visit<Func, N, table>
				&& can_visit<Func, N, array>
				&& can_visit<Func, N, string>
				&& can_visit<Func, N, int64_t>
				&& can_visit<Func, N, double>
				&& can_visit<Func, N, bool>
				&& can_visit<Func, N, date>
				&& can_visit<Func, N, time>
				&& can_visit<Func, N, date_time>;

			template <typename Func, typename N, typename T>
			static constexpr bool visit_is_nothrow_one =
				!can_visit<Func, N, T>
				|| std::is_nothrow_invocable_v<Func, ref_cast_type<N, T>>;

			template <typename Func, typename N>
			static constexpr bool visit_is_nothrow =
				visit_is_nothrow_one<Func, N, table>
				&& visit_is_nothrow_one<Func, N, array>
				&& visit_is_nothrow_one<Func, N, string>
				&& visit_is_nothrow_one<Func, N, int64_t>
				&& visit_is_nothrow_one<Func, N, double>
				&& visit_is_nothrow_one<Func, N, bool>
				&& visit_is_nothrow_one<Func, N, date>
				&& visit_is_nothrow_one<Func, N, time>
				&& visit_is_nothrow_one<Func, N, date_time>;

			template <typename Func, typename N, typename T, bool = can_visit<Func, N, T>>
			struct visit_return_type final
			{
				using type = decltype(std::declval<Func>()(std::declval<ref_cast_type<N, T>>()));
			};
			template <typename Func, typename N, typename T>
			struct visit_return_type<Func, N, T, false> final
			{
				using type = void;
			};

			template <typename A, typename B>
			using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

			template <typename N, typename Func>
			static decltype(auto) do_visit(N&& n, Func&& visitor)
				noexcept(visit_is_nothrow<Func&&, N&&>)
			{
				static_assert(
					can_visit_any<Func&&, N&&>,
					"Visitors must be invocable for at least one of the toml::node specializations"
				);

				switch (n.type())
				{
					case node_type::table:
						if constexpr (can_visit<Func&&, N&&, table>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<table>());
						break;

					case node_type::array:
						if constexpr (can_visit<Func&&, N&&, array>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<array>());
						break;

					case node_type::string:
						if constexpr (can_visit<Func&&, N&&, string>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<string>());
						break;

					case node_type::integer:
						if constexpr (can_visit<Func&&, N&&, int64_t>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<int64_t>());
						break;

					case node_type::floating_point:
						if constexpr (can_visit<Func&&, N&&, double>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<double>());
						break;

					case node_type::boolean:
						if constexpr (can_visit<Func&&, N&&, bool>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<bool>());
						break;

					case node_type::date:
						if constexpr (can_visit<Func&&, N&&, date>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<date>());
						break;

					case node_type::time:
						if constexpr (can_visit<Func&&, N&&, time>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<time>());
						break;

					case node_type::date_time:
						if constexpr (can_visit<Func&&, N&&, date_time>)
							return std::forward<Func>(visitor)(std::forward<N>(n).template ref_cast<date_time>());
						break;

					case node_type::none: TOML_UNREACHABLE;
					TOML_NO_DEFAULT_CASE;
				}

				if constexpr (can_visit_all<Func&&, N&&>)
					TOML_UNREACHABLE;
				else
				{
					using return_type =
						nonvoid<typename visit_return_type<Func&&, N&&, table>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, array>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, string>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, int64_t>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, double>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, bool>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, date>::type,
						nonvoid<typename visit_return_type<Func&&, N&&, time>::type,
								typename visit_return_type<Func&&, N&&, date_time>::type
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

			template <typename Func>
			decltype(auto) visit(Func&& visitor) & noexcept(visit_is_nothrow<Func&&, node&>)
			{
				return do_visit(*this, std::forward<Func>(visitor));
			}

			template <typename Func>
			decltype(auto) visit(Func&& visitor) && noexcept(visit_is_nothrow<Func&&, node&&>)
			{
				return do_visit(std::move(*this), std::forward<Func>(visitor));
			}

			template <typename Func>
			decltype(auto) visit(Func&& visitor) const& noexcept(visit_is_nothrow<Func&&, const node&>)
			{
				return do_visit(*this, std::forward<Func>(visitor));
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

TOML_POP_WARNINGS //TOML_DISABLE_VTABLE_WARNINGS

#endif
//------------------  ↑ toml_node.h  -----------------------------------------------------------------------------------

//------------------------------------------  ↓ toml_value.h  ----------------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_FLOAT_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	template <typename Char, typename T>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const value<T>&);

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

			template <typename Char, typename U>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const value<U>& rhs);
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

			[[nodiscard]] friend bool operator == (const value& lhs, value_arg rhs) noexcept
			{
				if constexpr (std::is_same_v<value_type, double>)
				{
					static constexpr auto pack = [](auto l, auto r) constexpr noexcept
					{
						return ((static_cast<uint64_t>(l) << 32) | static_cast<uint64_t>(r));
					};

					switch (pack(std::fpclassify(lhs.val_), std::fpclassify(rhs)))
					{
						case pack(FP_INFINITE, FP_INFINITE): return (lhs.val_ < 0.0) == (rhs < 0.0);
						case pack(FP_NAN, FP_NAN): return true;
						default: return lhs.val_ == rhs;
					}
				}
				else
					return lhs.val_ == rhs;

			}
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
					return lhs == rhs.val_; //calls asymmetrical value-equality operator defined above
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
		TOML_PUSH_WARNINGS
		TOML_DISABLE_VTABLE_WARNINGS
		extern template class TOML_API value<string>;
		extern template class TOML_API value<int64_t>;
		extern template class TOML_API value<double>;
		extern template class TOML_API value<bool>;
		extern template class TOML_API value<date>;
		extern template class TOML_API value<time>;
		extern template class TOML_API value<date_time>;
		TOML_POP_WARNINGS
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

	template <typename Char, typename T>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const value<T>& rhs)
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

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<toml::string>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<int64_t>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<double>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<bool>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<toml::time>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date_time>&);
	#endif

	TOML_PUSH_WARNINGS
	TOML_DISABLE_INIT_WARNINGS

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

	TOML_POP_WARNINGS

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

TOML_POP_WARNINGS // TOML_DISABLE_FLOAT_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//------------------------------------------  ↑ toml_value.h  ----------------------------------------------------------

//-------------------------------------------------------------------  ↓ toml_array.h  ---------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_VTABLE_WARNINGS

namespace toml::impl
{
	template <bool IsConst>
	class array_iterator final
	{
		private:
			friend class ::toml::array;

			using raw_mutable_iterator = std::vector<std::unique_ptr<node>>::iterator;
			using raw_const_iterator = std::vector<std::unique_ptr<node>>::const_iterator;
			using raw_iterator = std::conditional_t<IsConst, raw_const_iterator, raw_mutable_iterator>;

			mutable raw_iterator raw_;

			array_iterator(raw_mutable_iterator raw) noexcept
				: raw_{ raw }
			{}

			template <bool C = IsConst, typename = std::enable_if_t<C>>
			TOML_NODISCARD_CTOR
			array_iterator(raw_const_iterator raw) noexcept
				: raw_{ raw }
			{}

		public:

			using value_type = std::conditional_t<IsConst, const node, node>;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = ptrdiff_t;

			array_iterator() noexcept = default;
			array_iterator(const array_iterator&) noexcept = default;
			array_iterator& operator = (const array_iterator&) noexcept = default;

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

			template <bool C = IsConst, typename = std::enable_if_t<!C>>
			[[nodiscard]]
			operator array_iterator<true>() const noexcept
			{
				return array_iterator<true>{ raw_ };
			}
	};

	template <typename T>
	[[nodiscard]]
	TOML_ALWAYS_INLINE
	auto make_node(T&& val) noexcept
	{
		using type = unwrapped<remove_cvref_t<T>>;
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

	template <typename T>
	[[nodiscard]]
	TOML_ALWAYS_INLINE
	auto make_node(inserter<T>&& val) noexcept
	{
		return make_node(std::move(val.value));
	}
}

namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const array& lhs, const array& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const array& lhs, const array& rhs) noexcept;
	template <typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const array&);

	using array_iterator = impl::array_iterator<false>;
	using const_array_iterator = impl::array_iterator<true>;

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
			using iterator = array_iterator;
			using const_iterator = const_array_iterator;

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

			[[nodiscard]] size_t max_size() const noexcept;
			[[nodiscard]] size_t capacity() const noexcept;
			void shrink_to_fit();

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

			template <typename Iter>
			iterator insert(const_iterator pos, Iter first, Iter last) noexcept
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
			void resize(size_t new_size, U&& default_init_val) noexcept
			{
				if (!new_size)
					values.clear();
				else if (new_size < values.size())
					values.resize(new_size);
				else if (new_size > values.size())
					insert(cend(), new_size - values.size(), std::forward<U>(default_init_val));
			}

			void truncate(size_t new_size);

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
			array& flatten() &;
			array&& flatten()&&
			{
				return static_cast<toml::array&&>(static_cast<toml::array&>(*this).flatten());
			}

			template <typename Char>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const array&);
	};
}

TOML_POP_WARNINGS //TOML_DISABLE_VTABLE_WARNINGS

#endif
//-------------------------------------------------------------------  ↑ toml_array.h  ---------------------------------

//--------------------------------------------------------------------------------------------  ↓ toml_table.h  --------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_VTABLE_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml::impl
{
	template <bool IsConst>
	struct table_proxy_pair final
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const string& first;
		value_type& second;
	};

	template <bool IsConst>
	class table_iterator final
	{
		private:
			friend class ::toml::table;

			using proxy_type = table_proxy_pair<IsConst>;
			using raw_mutable_iterator = string_map<std::unique_ptr<node>>::iterator;
			using raw_const_iterator = string_map<std::unique_ptr<node>>::const_iterator;
			using raw_iterator = std::conditional_t<IsConst, raw_const_iterator, raw_mutable_iterator>;

			mutable raw_iterator raw_;
			mutable std::aligned_storage_t<sizeof(proxy_type), alignof(proxy_type)> proxy;
			mutable bool proxy_instantiated = false;

			[[nodiscard]]
			proxy_type* get_proxy() const noexcept
			{
				if (!proxy_instantiated)
				{
					auto p = new (&proxy) proxy_type{ raw_->first, *raw_->second.get() };
					proxy_instantiated = true;
					return p;
				}
				else
					return TOML_LAUNDER(reinterpret_cast<proxy_type*>(&proxy));
			}

			table_iterator(raw_mutable_iterator raw) noexcept
				: raw_{ raw }
			{}

			template <bool C = IsConst, typename = std::enable_if_t<C>>
			TOML_NODISCARD_CTOR
			table_iterator(raw_const_iterator raw) noexcept
				: raw_{ raw }
			{}

		public:

			table_iterator() noexcept = default;

			table_iterator(const table_iterator& other) noexcept
				: raw_{ other.raw_ }
			{}

			table_iterator& operator = (const table_iterator& rhs) noexcept
			{
				raw_ = rhs.raw_;
				proxy_instantiated = false;
				return *this;
			}

			using value_type = table_proxy_pair<IsConst>;
			using reference = value_type&;
			using pointer = value_type*;

			table_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				proxy_instantiated = false;
				return *this;
			}

			table_iterator operator++(int) noexcept // post++
			{
				table_iterator out{ raw_ };
				++raw_;
				proxy_instantiated = false;
				return out;
			}

			table_iterator& operator--() noexcept // --pre
			{
				--raw_;
				proxy_instantiated = false;
				return *this;
			}

			table_iterator operator--(int) noexcept // post--
			{
				table_iterator out{ raw_ };
				--raw_;
				proxy_instantiated = false;
				return out;
			}

			[[nodiscard]]
			reference operator* () const noexcept
			{
				return *get_proxy();
			}

			[[nodiscard]]
			pointer operator -> () const noexcept
			{
				return get_proxy();
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

			template <bool C = IsConst, typename = std::enable_if_t<!C>>
			[[nodiscard]]
			operator table_iterator<true>() const noexcept
			{
				return table_iterator<true>{ raw_ };
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

namespace toml
{
	[[nodiscard]] TOML_API bool operator == (const table& lhs, const table& rhs) noexcept;
	[[nodiscard]] TOML_API bool operator != (const table& lhs, const table& rhs) noexcept;
	template <typename Char>
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const table&);

	using table_iterator = impl::table_iterator<false>;
	using const_table_iterator = impl::table_iterator<true>;

	class TOML_API table final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;

			impl::string_map<std::unique_ptr<node>> values;
			bool inline_ = false;

			table(impl::table_init_pair*, size_t) noexcept;

		public:

			using iterator = table_iterator;
			using const_iterator = const_table_iterator;

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

			template <typename Iter, typename = std::enable_if_t<
				!std::is_convertible_v<Iter&&, string_view>
			>>
			void insert(Iter first, Iter last) noexcept
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

			template <typename Map, typename Key>
			[[nodiscard]] static auto do_get(Map& vals, const Key& key) noexcept
			{
				using return_type = std::conditional_t<
					std::is_const_v<Map>,
					const node*,
					node*
				>;

				if (auto it = vals.find(key); it != vals.end())
					return return_type{ it->second.get() };
				return return_type{};
			}

			template <typename T, typename Map, typename Key>
			[[nodiscard]] static auto do_get_as(Map& vals, const Key& key) noexcept
			{
				const auto node = do_get(vals, key);
				return node ? node->template as<T>() : nullptr;
			}

			template <typename Map, typename Key>
			[[nodiscard]] TOML_ALWAYS_INLINE
			static bool do_contains(Map& vals, const Key& key) noexcept
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

			template <typename Char>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const table&);
	};
}

TOML_POP_WARNINGS // TOML_DISABLE_VTABLE_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//--------------------------------------------------------------------------------------------  ↑ toml_table.h  --------

//---------------  ↓ toml_node_view.h  ---------------------------------------------------------------------------------
#if 1

namespace toml
{
	TOML_PUSH_WARNINGS
	TOML_DISABLE_FLOAT_WARNINGS

	template <typename Char, typename T>
	inline std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const node_view<T>&);

	template <typename T>
	class TOML_API TOML_TRIVIAL_ABI node_view
	{
		public:
			using viewed_type = T;

		private:
			friend class toml::table;
			template <typename U> friend class toml::node_view;

			mutable viewed_type* node_ = nullptr;

			TOML_NODISCARD_CTOR
			node_view(viewed_type* node) noexcept
				: node_{ node }
			{}

			template <typename Func>
			static constexpr bool visit_is_nothrow
				= noexcept(std::declval<viewed_type*>()->visit(std::declval<Func&&>()));

		public:

			TOML_NODISCARD_CTOR
			node_view() noexcept = default;
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

			TOML_PUSH_WARNINGS
			TOML_DISABLE_INIT_WARNINGS

			template <typename U>
			[[nodiscard]] optional<U> value() const noexcept
			{
				if (node_)
					return node_->template value<U>();
				return {};
			}

			TOML_POP_WARNINGS

			template <typename U>
			[[nodiscard]] auto value_or(U&& default_value) const noexcept
			{
				using return_type = decltype(node_->value_or(std::forward<U>(default_value)));
				if (node_)
					return node_->value_or(std::forward<U>(default_value));
				return return_type{ std::forward<U>(default_value) };
			}

			template <typename Func>
			decltype(auto) visit(Func&& visitor) const
				noexcept(visit_is_nothrow<Func&&>)
			{
				using return_type = decltype(node_->visit(std::forward<Func>(visitor)));
				if (node_)
					return node_->visit(std::forward<Func>(visitor));
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

			template <typename Char, typename U>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const node_view<U>&);
	};

	template <typename Char, typename T>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& os, const node_view<T>& nv)
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

	#if !TOML_ALL_INLINE
		extern template class TOML_API node_view<node>;
		extern template class TOML_API node_view<const node>;
		extern template TOML_API std::ostream& operator << (std::ostream&, const node_view<node>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const node_view<const node>&);
	#endif

	TOML_POP_WARNINGS // TOML_DISABLE_FLOAT_WARNINGS
}

#endif
//---------------  ↑ toml_node_view.h  ---------------------------------------------------------------------------------

//--------------------------------------  ↓ toml_utf8_generated.h  -----------------------------------------------------
#if 1

namespace toml::impl
{
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_hexadecimal_digit(char32_t cp) noexcept
	{
		using ui64 = std::uint_least64_t;

		return cp >= U'0' && cp <= U'f' && (1ull << (static_cast<ui64>(cp) - 0x30ull)) & 0x7E0000007E03FFull;
	}

#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_unicode_letter(char32_t cp) noexcept
	{
		using ui64 = std::uint_least64_t;
		using ui32 = std::uint_least32_t;

		if (cp < U'\u00AA' || cp > U'\U00031349')
			return false;

		const auto child_index_0 = (static_cast<ui64>(cp) - 0xAAull) / 0xC4Bull;
		if ((1ull << child_index_0) & 0x8A7FFC004001CFA0ull)
			return true;
		if ((1ull << child_index_0) & 0x26180C0000ull)
			return false;
		switch (child_index_0)
		{
			case 0x00: // [0] 00AA - 0CF4
			{
				if (cp > U'\u0CF2')
					return false;
				TOML_ASSUME(cp >= U'\u00AA');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFDFFFFFC10801ull,	0xFFFFFFFFFFFFDFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x07C000FFF0FFFFFFull,	0x0000000000000014ull,	0x0000000000000000ull,	0xFEFFFFF5D02F37C0ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFEFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFF00FFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFC09FFFFFFFFFBFull,	0x000000007FFFFFFFull,
					0xFFFFFFC000000000ull,	0xFFC00000000001E1ull,	0x00000001FFFFFFFFull,	0xFFFFFFFFFFFFFFB0ull,
					0x18000BFFFFFFFFFFull,	0xFFFFFF4000270030ull,	0xFFFFFFF80000003Full,	0x0FFFFFFFFFFFFFFFull,
					0xFFFFFFFF00000080ull,	0x44010FFFFFC10C01ull,	0xFFC07FFFFFC00000ull,	0xFFC0000000000001ull,
					0x000000003FFFF7FFull,	0xFFFFFFFFFC000000ull,	0x00FFC0400008FFFFull,	0x7FFFFE67F87FFF80ull,
					0x00EC00100008F17Full,	0x7FFFFE61F80400C0ull,	0x001780000000DB7Full,	0x7FFFFEEFF8000700ull,
					0x00C000400008FB7Full,	0x7FFFFE67F8008000ull,	0x00EC00000008FB7Full,	0xC6358F71FA000080ull,
					0x000000400000FFF1ull,	0x7FFFFF77F8000000ull,	0x00C1C0000008FFFFull,	0x7FFFFF77F8400000ull,
					0x00D000000008FBFFull,	0x0000000000000180ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xAAull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xAAull) % 0x40ull));
			}
			case 0x01: // [1] 0CF5 - 193F
			{
				if (cp < U'\u0D04' || cp > U'\u191E')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x027FFFFFFFFFDDFFull,	0x0FC0000038070400ull,	0xF2FFBFFFFFC7FFFEull,	0xE000000000000007ull,
					0xF000DFFFFFFFFFFFull,	0x6000000000000007ull,	0xF200DFFAFFFFFF7Dull,	0x100000000F000005ull,
					0xF000000000000000ull,	0x000001FFFFFFFFEFull,	0x00000000000001F0ull,	0xF000000000000000ull,
					0x0800007FFFFFFFFFull,	0x3FFE1C0623C3F000ull,	0xFFFFFFFFF0000400ull,	0xFF7FFFFFFFFFF20Bull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFF3D7F3DFull,	0xD7F3DFFFFFFFF3DFull,	0xFFFFFFFFFFF7FFF3ull,
					0xFFFFFFFFFFF3DFFFull,	0xF0000000007FFFFFull,	0xFFFFFFFFF0000FFFull,	0xE3F3FFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xEFFFF9FFFFFFFFFFull,	0xFFFFFFFFF07FFFFFull,	0xF01FE07FFFFFFFFFull,
					0xF0003FFFF0003DFFull,	0xF0001DFFF0003FFFull,	0x0000FFFFFFFFFFFFull,	0x0000000001080000ull,
					0xFFFFFFFFF0000000ull,	0xF01FFFFFFFFFFFFFull,	0xFFFFF05FFFFFFFF9ull,	0xF003FFFFFFFFFFFFull,
					0x0000000007FFFFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xD04ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xD04ull) % 0x40ull));
			}
			case 0x02: // [2] 1940 - 258A
			{
				if (cp < U'\u1950' || cp > U'\u2184')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFF001F3FFFFFFFull,	0x03FFFFFF0FFFFFFFull,	0xFFFF000000000000ull,	0xFFFFFFFFFFFF007Full,
					0x000000000000001Full,	0x0000000000800000ull,	0xFFE0000000000000ull,	0x0FE0000FFFFFFFFFull,
					0xFFF8000000000000ull,	0xFFFFFC00C001FFFFull,	0xFFFF0000003FFFFFull,	0xE0000000000FFFFFull,
					0x01FF3FFFFFFFFC00ull,	0x0000E7FFFFFFFFFFull,	0xFFFF046FDE000000ull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0x0000FFFFFFFFFFFFull,	0xFFFF000000000000ull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x3F3FFFFFFFFF3F3Full,
					0xFFFF3FFFFFFFAAFFull,	0x1FDC5FDFFFFFFFFFull,	0x00001FDC1FFF0FCFull,	0x0000000000000000ull,
					0x0000800200000000ull,	0x0000000000001FFFull,	0xFC84000000000000ull,	0x43E0F3FFBD503E2Full,
					0x0018000000000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x1950ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x1950ull) % 0x40ull));
			}
			case 0x03: // [3] 258B - 31D5
			{
				if (cp < U'\u2C00' || cp > U'\u31BF')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFF7FFFFFFFFFFFull,	0xFFFFFFFF7FFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x000C781FFFFFFFFFull,
					0xFFFF20BFFFFFFFFFull,	0x000080FFFFFFFFFFull,	0x7F7F7F7F007FFFFFull,	0x000000007F7F7F7Full,
					0x0000800000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x183E000000000060ull,	0xFFFFFFFFFFFFFFFEull,	0xFFFFFFFEE07FFFFFull,	0xF7FFFFFFFFFFFFFFull,
					0xFFFEFFFFFFFFFFE0ull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFF00007FFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x2C00ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x04: return (cp >= U'\u31F0' && cp <= U'\u31FF') || (cp >= U'\u3400' && cp <= U'\u3E20');
			case 0x06: return (cp >= U'\u4A6C' && cp <= U'\u4DBE') || (cp >= U'\u4E00' && cp <= U'\u56B6');
			case 0x0C: return (cp >= U'\u942E' && cp <= U'\u9FFB') || (cp >= U'\uA000' && cp <= U'\uA078');
			case 0x0D: // [13] A079 - ACC3
			{
				TOML_ASSUME(cp >= U'\uA079' && cp <= U'\uACC3');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x00000000000FFFFFull,	0xFFFFFFFFFF800000ull,	0xFFFFFFFFFFFFFF9Full,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x0006007FFF8FFFFFull,	0x003FFFFFFFFFFF80ull,
					0xFFFFFF9FFFFFFFC0ull,	0x00001FFFFFFFFFFFull,	0xFFFFFE7FC0000000ull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFCFFFFull,	0xF00000000003FE7Full,	0x000003FFFFFBDDFFull,	0x07FFFFFFFFFFFF80ull,
					0x07FFFFFFFFFFFE00ull,	0x7E00000000000000ull,	0xFF801FFFFFFE0034ull,	0xFFFFFF8000003FFFull,
					0x03FFFFFFFFFFF80Full,	0x007FEF8000400000ull,	0x0000FFFFFFFFFFBEull,	0x3FFFFF800007FB80ull,
					0x317FFFFFFFFFFFE2ull,	0x0E03FF9C0000029Full,	0xFFBFBF803F3F3F00ull,	0xFF81FFFBFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0x000003FFFFFFFFFFull,	0xFFFFFFFFFFFFFF80ull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0x00000000000007FFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xA079ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xA079ull) % 0x40ull));
			}
			case 0x11: return (cp >= U'\uD1A5' && cp <= U'\uD7A2') || (cp >= U'\uD7B0' && cp <= U'\uD7C6')
				|| (cp >= U'\uD7CB' && cp <= U'\uD7FB');
			case 0x14: // [20] F686 - 102D0
			{
				if (cp < U'\uF900')
					return false;
				TOML_ASSUME(cp <= U'\U000102D0');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFF3FFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x0000000003FFFFFFull,
					0x5F7FFDFFA0F8007Full,	0xFFFFFFFFFFFFFFDBull,	0x0003FFFFFFFFFFFFull,	0xFFFFFFFFFFF80000ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x3FFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFF0000ull,	0xFFFFFFFFFFFCFFFFull,	0x0FFF0000000000FFull,
					0x0000000000000000ull,	0xFFDF000000000000ull,	0xFFFFFFFFFFFFFFFFull,	0x1FFFFFFFFFFFFFFFull,
					0x07FFFFFE00000000ull,	0xFFFFFFC007FFFFFEull,	0x7FFFFFFFFFFFFFFFull,	0x000000001CFCFCFCull,
					0xB7FFFF7FFFFFEFFFull,	0x000000003FFF3FFFull,	0xFFFFFFFFFFFFFFFFull,	0x07FFFFFFFFFFFFFFull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xFFFFFFFF1FFFFFFFull,	0x000000000001FFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xF900ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x15: // [21] 102D1 - 10F1B
			{
				if (cp < U'\U00010300')
					return false;
				TOML_ASSUME(cp <= U'\U00010F1B');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFE000FFFFFFFFull,	0x003FFFFFFFFF03FDull,	0xFFFFFFFF3FFFFFFFull,	0x000000000000FF0Full,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFF00003FFFFFFFull,	0x0FFFFFFFFF0FFFFFull,
					0xFFFF00FFFFFFFFFFull,	0x0000000FFFFFFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x007FFFFFFFFFFFFFull,	0x000000FF003FFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x91BFFFFFFFFFFD3Full,	0x007FFFFF003FFFFFull,	0x000000007FFFFFFFull,	0x0037FFFF00000000ull,
					0x03FFFFFF003FFFFFull,	0x0000000000000000ull,	0xC0FFFFFFFFFFFFFFull,	0x0000000000000000ull,
					0x003FFFFFFEEF0001ull,	0x1FFFFFFF00000000ull,	0x000000001FFFFFFFull,	0x0000001FFFFFFEFFull,
					0x003FFFFFFFFFFFFFull,	0x0007FFFF003FFFFFull,	0x000000000003FFFFull,	0x0000000000000000ull,
					0xFFFFFFFFFFFFFFFFull,	0x00000000000001FFull,	0x0007FFFFFFFFFFFFull,	0x0007FFFFFFFFFFFFull,
					0x0000000FFFFFFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x000303FFFFFFFFFFull,	0x0000000000000000ull,
					0x000000000FFFFFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x10300ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x16: // [22] 10F1C - 11B66
			{
				if (cp > U'\U00011AF8')
					return false;
				TOML_ASSUME(cp >= U'\U00010F1C');

				constexpr ui64 bitmask_table_1[] =
				{
					0x000003FFFFF00801ull,	0x0000000000000000ull,	0x000001FFFFF00000ull,	0xFFFFFF8007FFFFF0ull,
					0x000000000FFFFFFFull,	0xFFFFFF8000000000ull,	0xFFF00000000FFFFFull,	0xFFFFFF8000001FFFull,
					0xFFF00900000007FFull,	0xFFFFFF80047FFFFFull,	0x400001E0007FFFFFull,	0xFFBFFFF000000001ull,
					0x000000000000FFFFull,	0xFFFBD7F000000000ull,	0xFFFFFFFFFFF01FFBull,	0xFF99FE0000000007ull,
					0x001000023EDFDFFFull,	0x000000000000003Eull,	0x0000000000000000ull,	0xFFFFFFF000000000ull,
					0x0000780001FFFFFFull,	0xFFFFFFF000000038ull,	0x00000B00000FFFFFull,	0x0000000000000000ull,
					0x0000000000000000ull,	0xFFFFFFF000000000ull,	0xF00000000007FFFFull,	0xFFFFFFF000000000ull,
					0x00000100000FFFFFull,	0xFFFFFFF000000000ull,	0x0000000010007FFFull,	0x7FFFFFF000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0xFFFFFFF000000000ull,
					0x000000000000FFFFull,	0x0000000000000000ull,	0xFFFFFFFFFFFFFFF0ull,	0xF6FF27F80000000Full,
					0x00000028000FFFFFull,	0x0000000000000000ull,	0x001FFFFFFFFFCFF0ull,	0xFFFF8010000000A0ull,
					0x00100000407FFFFFull,	0x00003FFFFFFFFFFFull,	0xFFFFFFF000000002ull,	0x000000001FFFFFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x10F1Cull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x10F1Cull) % 0x40ull));
			}
			case 0x17: // [23] 11B67 - 127B1
			{
				if (cp < U'\U00011C00' || cp > U'\U00012543')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x00007FFFFFFFFDFFull,	0xFFFC000000000001ull,	0x000000000000FFFFull,	0x0000000000000000ull,
					0x0001FFFFFFFFFB7Full,	0xFFFFFDBF00000040ull,	0x00000000010003FFull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0007FFFF00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0001000000000000ull,	0x0000000000000000ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x0000000003FFFFFFull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0x000000000000000Full,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x11C00ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x18: return cp >= U'\U00013000';
			case 0x19: return cp <= U'\U0001342E';
			case 0x1A: return cp >= U'\U00014400' && cp <= U'\U00014646';
			case 0x1D: // [29] 16529 - 17173
			{
				if (cp < U'\U00016800')
					return false;
				TOML_ASSUME(cp <= U'\U00017173');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x01FFFFFFFFFFFFFFull,	0x000000007FFFFFFFull,	0x0000000000000000ull,	0x00003FFFFFFF0000ull,
					0x0000FFFFFFFFFFFFull,	0xE0FFFFF80000000Full,	0x000000000000FFFFull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0xFFFFFFFFFFFFFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,
					0xFFFFFFFFFFFFFFFFull,	0x00000000000107FFull,	0x00000000FFF80000ull,	0x0000000B00000000ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0x000FFFFFFFFFFFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x16800ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x1F: return (cp >= U'\U00017DBF' && cp <= U'\U000187F6') || (cp >= U'\U00018800' && cp <= U'\U00018A09');
			case 0x20: return (cp >= U'\U00018A0A' && cp <= U'\U00018CD5') || (cp >= U'\U00018D00' && cp <= U'\U00018D07');
			case 0x23: // [35] 1AEEB - 1BB35
			{
				if (cp < U'\U0001B000' || cp > U'\U0001B2FB')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0x000000007FFFFFFFull,	0xFFFF00F000070000ull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x0FFFFFFFFFFFFFFFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x1B000ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x24: // [36] 1BB36 - 1C780
			{
				if (cp < U'\U0001BC00' || cp > U'\U0001BC99')
					return false;

				switch ((static_cast<ui64>(cp) - 0x1BC00ull) / 0x40ull)
				{
					case 0x01: return cp <= U'\U0001BC7C'
						&& (1ull << (static_cast<ui64>(cp) - 0x1BC40ull)) & 0x1FFF07FFFFFFFFFFull;
					case 0x02: return (1u << (static_cast<ui32>(cp) - 0x1BC80u)) & 0x3FF01FFu;
					default: return true;
				}
			}
			case 0x26: // [38] 1D3CC - 1E016
			{
				if (cp < U'\U0001D400' || cp > U'\U0001D7CB')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFDFFFFFull,	0xEBFFDE64DFFFFFFFull,	0xFFFFFFFFFFFFFFEFull,
					0x7BFFFFFFDFDFE7BFull,	0xFFFFFFFFFFFDFC5Full,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFF3FFFFFFFFFull,	0xF7FFFFFFF7FFFFFDull,
					0xFFDFFFFFFFDFFFFFull,	0xFFFF7FFFFFFF7FFFull,	0xFFFFFDFFFFFFFDFFull,	0x0000000000000FF7ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x1D400ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x27: // [39] 1E017 - 1EC61
			{
				if (cp < U'\U0001E100' || cp > U'\U0001E94B')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x3F801FFFFFFFFFFFull,	0x0000000000004000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x00000FFFFFFFFFFFull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0xFFFFFFFFFFFFFFFFull,	0x000000000000001Full,
					0xFFFFFFFFFFFFFFFFull,	0x000000000000080Full,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x1E100ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x28: // [40] 1EC62 - 1F8AC
			{
				if (cp < U'\U0001EE00' || cp > U'\U0001EEBB')
					return false;

				switch ((static_cast<ui64>(cp) - 0x1EE00ull) / 0x40ull)
				{
					case 0x00: return cp <= U'\U0001EE3B'
						&& (1ull << (static_cast<ui64>(cp) - 0x1EE00ull)) & 0xAF7FE96FFFFFFEFull;
					case 0x01: return cp >= U'\U0001EE42' && cp <= U'\U0001EE7E'
						&& (1ull << (static_cast<ui64>(cp) - 0x1EE42ull)) & 0x17BDFDE5AAA5BAA1ull;
					case 0x02: return (1ull << (static_cast<ui64>(cp) - 0x1EE80ull)) & 0xFFFFBEE0FFFFBFFull;
					TOML_NO_DEFAULT_CASE;
				}
			}
			case 0x29: return cp >= U'\U00020000';
			case 0x37: return (cp >= U'\U0002A4C7' && cp <= U'\U0002A6DC') || (cp >= U'\U0002A700' && cp <= U'\U0002B111');
			case 0x38: return (cp >= U'\U0002B112' && cp <= U'\U0002B733') || (cp >= U'\U0002B740' && cp <= U'\U0002B81C')
				|| (cp >= U'\U0002B820' && cp <= U'\U0002BD5C');
			case 0x3A: return (cp >= U'\U0002C9A8' && cp <= U'\U0002CEA0') || (cp >= U'\U0002CEB0' && cp <= U'\U0002D5F2');
			case 0x3C: return cp <= U'\U0002EBDF';
			case 0x3D: return cp >= U'\U0002F800' && cp <= U'\U0002FA1D';
			case 0x3E: return cp >= U'\U00030000';
			TOML_NO_DEFAULT_CASE;
		}
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_unicode_number(char32_t cp) noexcept
	{
		using ui64 = std::uint_least64_t;

		if (cp < U'\u0660' || cp > U'\U0001FBF9')
			return false;

		const auto child_index_0 = (static_cast<ui64>(cp) - 0x660ull) / 0x7D7ull;
		if ((1ull << child_index_0) & 0x47FFDFE07FCFFFD0ull)
			return false;
		switch (child_index_0)
		{
			case 0x00: // [0] 0660 - 0E36
			{
				if (cp > U'\u0DEF')
					return false;
				TOML_ASSUME(cp >= U'\u0660');

				constexpr ui64 bitmask_table_1[] =
				{
					0x00000000000003FFull,	0x0000000000000000ull,	0x0000000003FF0000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x000003FF00000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x000000000000FFC0ull,	0x0000000000000000ull,	0x000000000000FFC0ull,	0x0000000000000000ull,
					0x000000000000FFC0ull,	0x0000000000000000ull,	0x000000000000FFC0ull,	0x0000000000000000ull,
					0x000000000000FFC0ull,	0x0000000000000000ull,	0x000000000000FFC0ull,	0x0000000000000000ull,
					0x000000000000FFC0ull,	0x0000000000000000ull,	0x000000000000FFC0ull,	0x0000000000000000ull,
					0x000000000000FFC0ull,	0x0000000000000000ull,	0x000000000000FFC0ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x660ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x660ull) % 0x40ull));
			}
			case 0x01: // [1] 0E37 - 160D
			{
				if (cp < U'\u0E50' || cp > U'\u1099')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x00000000000003FFull,	0x0000000000000000ull,	0x00000000000003FFull,	0x0000000003FF0000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x03FF000000000000ull,
					0x0000000000000000ull,	0x00000000000003FFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xE50ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xE50ull) % 0x40ull));
			}
			case 0x02: // [2] 160E - 1DE4
			{
				if (cp < U'\u16EE' || cp > U'\u1C59')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x0000000000000007ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0FFC000000000000ull,
					0x00000FFC00000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x00000003FF000000ull,	0x0000000000000000ull,	0x00000FFC00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x00000FFC0FFC0000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x00000FFC00000000ull,	0x0000000000000000ull,	0x0000000000000FFCull,
					0x0000000000000000ull,	0x00000FFC0FFC0000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x16EEull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x16EEull) % 0x40ull));
			}
			case 0x03: return cp >= U'\u2160' && cp <= U'\u2188'
				&& (1ull << (static_cast<ui64>(cp) - 0x2160ull)) & 0x1E7FFFFFFFFull;
			case 0x05: return cp >= U'\u3007' && cp <= U'\u303A'
				&& (1ull << (static_cast<ui64>(cp) - 0x3007ull)) & 0xE0007FC000001ull;
			case 0x14: // [20] A32C - AB02
			{
				if (cp < U'\uA620' || cp > U'\uAA59')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x00000000000003FFull,	0x0000000000000000ull,	0x0000000000000000ull,	0x000000000000FFC0ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x03FF000000000000ull,	0x000003FF00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x03FF000000000000ull,	0x0000000003FF0000ull,
					0x03FF000000000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xA620ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xA620ull) % 0x40ull));
			}
			case 0x15: return cp >= U'\uABF0' && cp <= U'\uABF9';
			case 0x1F: return cp >= U'\uFF10' && cp <= U'\uFF19';
			case 0x20: // [32] 10140 - 10916
			{
				if (cp > U'\U000104A9')
					return false;
				TOML_ASSUME(cp >= U'\U00010140');

				constexpr ui64 bitmask_table_1[] =
				{
					0x001FFFFFFFFFFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000402ull,	0x0000000000000000ull,	0x00000000003E0000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x000003FF00000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x10140ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x21: return (cp >= U'\U00010D30' && cp <= U'\U00010D39') || (cp >= U'\U00011066' && cp <= U'\U0001106F');
			case 0x22: // [34] 110EE - 118C4
			{
				if (cp < U'\U000110F0' || cp > U'\U00011739')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x00000000000003FFull,	0x000000000000FFC0ull,	0x0000000000000000ull,	0x000003FF00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x00000000000003FFull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x000003FF00000000ull,	0x0000000000000000ull,	0x000003FF00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x000003FF00000000ull,	0x0000000000000000ull,	0x0000000003FF0000ull,
					0x0000000000000000ull,	0x00000000000003FFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x110F0ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x110F0ull) % 0x40ull));
			}
			case 0x23: // [35] 118C5 - 1209B
			{
				if (cp < U'\U000118E0' || cp > U'\U00011DA9')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x00000000000003FFull,	0x03FF000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x03FF000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x03FF000000000000ull,	0x0000000000000000ull,	0x00000000000003FFull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x118E0ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x118E0ull) % 0x40ull));
			}
			case 0x24: return cp >= U'\U00012400' && cp <= U'\U0001246E';
			case 0x2D: return (cp >= U'\U00016A60' && cp <= U'\U00016A69') || (cp >= U'\U00016B50' && cp <= U'\U00016B59');
			case 0x3B: return cp >= U'\U0001D7CE' && cp <= U'\U0001D7FF';
			case 0x3C: return (cp >= U'\U0001E140' && cp <= U'\U0001E149') || (cp >= U'\U0001E2F0' && cp <= U'\U0001E2F9');
			case 0x3D: return cp >= U'\U0001E950' && cp <= U'\U0001E959';
			case 0x3F: return cp >= U'\U0001FBF0';
			TOML_NO_DEFAULT_CASE;
		}
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_unicode_combining_mark(char32_t cp) noexcept
	{
		using ui64 = std::uint_least64_t;

		if (cp < U'\u0300' || cp > U'\U000E01EF')
			return false;

		const auto child_index_0 = (static_cast<ui64>(cp) - 0x300ull) / 0x37FCull;
		if ((1ull << child_index_0) & 0x7FFFFFFFFFFFFE02ull)
			return false;
		switch (child_index_0)
		{
			case 0x00: // [0] 0300 - 3AFB
			{
				if (cp > U'\u309A')
					return false;
				TOML_ASSUME(cp >= U'\u0300');

				constexpr ui64 bitmask_table_1[] =
				{
					0xFFFFFFFFFFFFFFFFull,	0x0000FFFFFFFFFFFFull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x00000000000000F8ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xBFFFFFFFFFFE0000ull,	0x00000000000000B6ull,
					0x0000000007FF0000ull,	0x00010000FFFFF800ull,	0x0000000000000000ull,	0x00003D9F9FC00000ull,
					0xFFFF000000020000ull,	0x00000000000007FFull,	0x0001FFC000000000ull,	0x200FF80000000000ull,
					0x00003EEFFBC00000ull,	0x000000000E000000ull,	0x0000000000000000ull,	0xFFFFFFFBFFF80000ull,
					0xDC0000000000000Full,	0x0000000C00FEFFFFull,	0xD00000000000000Eull,	0x4000000C0080399Full,
					0xD00000000000000Eull,	0x0023000000023987ull,	0xD00000000000000Eull,	0xFC00000C00003BBFull,
					0xD00000000000000Eull,	0x0000000C00E0399Full,	0xC000000000000004ull,	0x0000000000803DC7ull,
					0xC00000000000001Full,	0x0000000C00603DDFull,	0xD00000000000000Eull,	0x0000000C00603DDFull,
					0xD80000000000000Full,	0x0000000C00803DDFull,	0x000000000000000Eull,	0x000C0000FF5F8400ull,
					0x07F2000000000000ull,	0x0000000000007F80ull,	0x1FF2000000000000ull,	0x0000000000003F00ull,
					0xC2A0000003000000ull,	0xFFFE000000000000ull,	0x1FFFFFFFFEFFE0DFull,	0x0000000000000040ull,
					0x7FFFF80000000000ull,	0x001E3F9DC3C00000ull,	0x000000003C00BFFCull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x00000000E0000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x001C0000001C0000ull,	0x000C0000000C0000ull,	0xFFF0000000000000ull,	0x00000000200FFFFFull,
					0x0000000000003800ull,	0x0000000000000000ull,	0x0000020000000060ull,	0x0000000000000000ull,
					0x0FFF0FFF00000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x000000000F800000ull,	0x9FFFFFFF7FE00000ull,	0xBFFF000000000000ull,	0x0000000000000001ull,
					0xFFF000000000001Full,	0x000FF8000000001Full,	0x00003FFE00000007ull,	0x000FFFC000000000ull,
					0x00FFFFF000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x039021FFFFF70000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0xFBFFFFFFFFFFFFFFull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0001FFE21FFF0000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0003800000000000ull,
					0x0000000000000000ull,	0x8000000000000000ull,	0x0000000000000000ull,	0xFFFFFFFF00000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000FC0000000000ull,	0x0000000000000000ull,	0x0000000006000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x300ull) / 0x40ull]
					& (0x1ull << (static_cast<ui64>(cp) % 0x40ull));
			}
			case 0x02: // [2] 72F8 - AAF3
			{
				if (cp < U'\uA66F' || cp > U'\uAAEF')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x0001800000007FE1ull,	0x0000000000000000ull,	0x0000000000000006ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x21F0000010880000ull,	0x0000000000000000ull,
					0x0000000000060000ull,	0xFFFE0000007FFFE0ull,	0x7F80000000010007ull,	0x0000001FFF000000ull,
					0x00000000001E0000ull,	0x004000000003FFF0ull,	0xFC00000000000000ull,	0x00000000601000FFull,
					0x0000000000007000ull,	0xF00000000005833Aull,	0x0000000000000001ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xA66Full) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xA66Full) % 0x40ull));
			}
			case 0x03: return cp == U'\uAAF5' || cp == U'\uAAF6' || (cp >= U'\uABE3' && cp <= U'\uABEA') || cp == U'\uABEC'
				|| cp == U'\uABED';
			case 0x04: // [4] E2F0 - 11AEB
			{
				if (cp < U'\uFB1E' || cp > U'\U00011A99')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x0000000000000001ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0003FFFC00000000ull,
					0x000000000003FFFCull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000080000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000004ull,
					0x0000000000000000ull,	0x000000001F000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0003C1B800000000ull,
					0x000000021C000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000180ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x00000000000003C0ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000006000ull,	0x0000000000000000ull,
					0x0007FF0000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000001C00000000ull,
					0x000001FFFC000000ull,	0x0000001E00000000ull,	0x000000001FFC0000ull,	0x0000001C00000000ull,
					0x00000180007FFE00ull,	0x0000001C00200000ull,	0x00037807FFE00000ull,	0x0000000000000000ull,
					0x0000000103FFC000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000003C00001FFEull,
					0x0200E67F60000000ull,	0x00000000007C7F30ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x000001FFFF800000ull,	0x0000000000000001ull,	0x0000003FFFFC0000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xC0000007FCFE0000ull,	0x0000000000000000ull,
					0x00000007FFFC0000ull,	0x0000000000000000ull,	0x0000000003FFE000ull,	0x8000000000000000ull,
					0x0000000000003FFFull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x000000001FFFC000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x00000035E6FC0000ull,	0x0000000000000000ull,	0xF3F8000000000000ull,	0x00001FF800000047ull,
					0x3FF80201EFE00000ull,	0x0FFFF00000000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0xFB1Eull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0xFB1Eull) % 0x40ull));
			}
			case 0x05: // [5] 11AEC - 152E7
			{
				if (cp < U'\U00011C2F' || cp > U'\U00011EF6')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x000000000001FEFFull,	0xFDFFFFF800000000ull,	0x00000000000000FFull,	0x0000000000000000ull,
					0x00000000017F68FCull,	0x000001F6F8000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x00000000000000F0ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x11C2Full) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x11C2Full) % 0x40ull));
			}
			case 0x06: // [6] 152E8 - 18AE3
			{
				if (cp < U'\U00016AF0' || cp > U'\U00016FF1')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x000000000000001Full,	0x000000000000007Full,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0xFFFFFFFE80000000ull,	0x0000000780FFFFFFull,	0x0010000000000000ull,
					0x0000000000000003ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x16AF0ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x16AF0ull) % 0x40ull));
			}
			case 0x07: return cp >= U'\U0001BC9D' && cp <= U'\U0001BC9E';
			case 0x08: // [8] 1C2E0 - 1FADB
			{
				if (cp < U'\U0001D165' || cp > U'\U0001E94A')
					return false;

				constexpr ui64 bitmask_table_1[] =
				{
					0x0000007F3FC03F1Full,	0x00000000000001E0ull,	0x0000000000000000ull,	0x00000000E0000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xFFFFFFFFF8000000ull,	0xFFFFFFFFFFC3FFFFull,
					0xF7C00000800100FFull,	0x00000000000007FFull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0xDFCFFFFBF8000000ull,	0x000000000000003Eull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x000000000003F800ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000780ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,	0x0000000000000000ull,
					0x0000000000000000ull,	0x0003F80000000000ull,	0x0000000000000000ull,	0x0000003F80000000ull,
				};
				return bitmask_table_1[(static_cast<ui64>(cp) - 0x1D165ull) / 0x40ull]
					& (0x1ull << ((static_cast<ui64>(cp) - 0x1D165ull) % 0x40ull));
			}
			case 0x3F: return cp >= U'\U000E0100';
			TOML_NO_DEFAULT_CASE;
		}
	}

#endif // TOML_LANG_UNRELEASED
} // toml::impl

#endif
//--------------------------------------  ↑ toml_utf8_generated.h  -----------------------------------------------------

//--------------------------------------------------------------------  ↓ toml_utf8.h  ---------------------------------
#if 1

namespace toml::impl
{
	template <typename... T>
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_match(char32_t codepoint, T... vals) noexcept
	{
		static_assert((std::is_same_v<char32_t, T> && ...));
		return ((codepoint == vals) || ...);
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_ascii_whitespace(char32_t codepoint) noexcept
	{
		return codepoint == U'\t' || codepoint == U' ';
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
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
	TOML_GNU_ATTR(const)
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
		return is_ascii_whitespace(codepoint) || is_unicode_whitespace(codepoint);
	}

	template <bool IncludeCarriageReturn = true>
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_ascii_line_break(char32_t codepoint) noexcept
	{
		constexpr auto low_range_end = IncludeCarriageReturn ? U'\r' : U'\f';
		return (codepoint >= U'\n' && codepoint <= low_range_end);
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_unicode_line_break(char32_t codepoint) noexcept
	{
		// see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that say "is a line-break")

		return codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;
	}

	template <bool IncludeCarriageReturn = true>
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		return is_ascii_line_break<IncludeCarriageReturn>(codepoint) || is_unicode_line_break(codepoint);
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_string_delimiter(char32_t codepoint) noexcept
	{
		return codepoint == U'"' || codepoint == U'\'';
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_ascii_letter(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'z')
			|| (codepoint >= U'A' && codepoint <= U'Z');
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_binary_digit(char32_t codepoint) noexcept
	{
		return codepoint == U'0' || codepoint == U'1';
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_octal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'7');
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_decimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'9');
	}

	template <typename T>
	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr std::uint_least32_t hex_to_dec(const T codepoint) noexcept
	{
		if constexpr (std::is_same_v<remove_cvref_t<T>, std::uint_least32_t>)
			return codepoint >= 0x41u // >= 'A'
				? 10u + (codepoint | 0x20u) - 0x61u // - 'a'
				: codepoint - 0x30u // - '0'
			;
		else
			return hex_to_dec(static_cast<std::uint_least32_t>(codepoint));
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	constexpr bool is_bare_key_character(char32_t codepoint) noexcept
	{
		return is_ascii_letter(codepoint)
			|| is_decimal_digit(codepoint)
			|| codepoint == U'-'
			|| codepoint == U'_'
			#if TOML_LANG_UNRELEASED // toml/issues/644 ('+' in bare keys) & toml/issues/687 (unicode bare keys)
			|| codepoint == U'+'
			|| is_unicode_letter(codepoint)
			|| is_unicode_number(codepoint)
			|| is_unicode_combining_mark(codepoint)
			#endif
		;
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
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

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_control_character(char32_t codepoint) noexcept
	{
		return codepoint <= U'\u001F' || codepoint == U'\u007F';
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_nontab_control_character(char32_t codepoint) noexcept
	{
		return codepoint <= U'\u0008'
			|| (codepoint >= U'\u000A' && codepoint <= U'\u001F')
			|| codepoint == U'\u007F';
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr bool is_unicode_surrogate(char32_t codepoint) noexcept
	{
		return codepoint >= 0xD800u && codepoint <= 0xDFFF;
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
}

#endif
//--------------------------------------------------------------------  ↑ toml_utf8.h  ---------------------------------

//------------------------------------------------------------------------------------------  ↓ toml_formatter.h  ------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	enum class format_flags : uint8_t
	{
		none,
		quote_dates_and_times = 1
	};

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr format_flags operator & (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>(impl::unbox_enum(lhs) & impl::unbox_enum(rhs));
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr format_flags operator | (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>( impl::unbox_enum(lhs) | impl::unbox_enum(rhs) );
	}
}

namespace toml::impl
{
	template <typename Char = char>
	class TOML_API formatter
	{
		private:
			const toml::node* source_;
			std::basic_ostream<Char>* stream_ = nullptr;
			format_flags flags_;
			int8_t indent_;
			bool naked_newline_;

		protected:

			[[nodiscard]] const toml::node& source() const noexcept { return *source_; }
			[[nodiscard]] format_flags flags() const noexcept { return flags_; }
			[[nodiscard]] std::basic_ostream<Char>& stream() const noexcept { return *stream_; }

			static constexpr size_t indent_columns = 4;
			static constexpr toml::string_view indent_string = TOML_STRING_PREFIX("    "sv);
			[[nodiscard]] int8_t indent() const noexcept { return indent_; }
			void indent(int8_t level) noexcept { indent_ = level; }
			void increase_indent() noexcept { indent_++; }
			void decrease_indent() noexcept { indent_--; }

			TOML_ALWAYS_INLINE
			void clear_naked_newline() noexcept { naked_newline_ = false; }
			void attach(std::basic_ostream<Char>& stream) noexcept
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

			void print_value(const node& val_node, node_type type)
			{
				TOML_ASSUME(type > node_type::array);
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

	#if !TOML_ALL_INLINE
		extern template class TOML_API formatter<char>;
	#endif
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//------------------------------------------------------------------------------------------  ↑ toml_formatter.h  ------

//-----------  ↓ toml_default_formatter.h  -----------------------------------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml::impl
{
	[[nodiscard]] TOML_API
	toml::string default_formatter_make_key_segment(const toml::string& str) noexcept;

	[[nodiscard]] TOML_API
	size_t default_formatter_inline_columns(const node& node) noexcept;

	[[nodiscard]] TOML_API
	bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept;
}

namespace toml
{
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&);
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, default_formatter<U>&&);

	template <typename Char = char>
	class TOML_API default_formatter final : impl::formatter<Char>
	{
		private:
			using base = impl::formatter<Char>;
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

			void print_inline(const table& /*tbl*/);
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
						TOML_ASSUME(type != node_type::none);
						switch (type)
						{
							case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print_value(v, type);
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
				for (auto&& [k, v] : tbl)
				{
					const auto type = v.type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(&v)->is_inline())
						|| (type == node_type::array && is_non_inline_array_of_tables(v)))
						continue;

					base::print_newline();
					base::print_indent();
					print_key_segment(k);
					impl::print_to_stream(" = "sv, base::stream());
					TOML_ASSUME(type != node_type::none);
					switch (type)
					{
						case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
						case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
						default:
							base::print_value(v, type);
					}
				}

				//non-inline tables
				for (auto&& [k, v] : tbl)
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
					for (auto&& [child_k, child_v] : child_tbl)
					{
						(void)child_k;
						const auto child_type = child_v.type();
						TOML_ASSUME(child_type != node_type::none);
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
				for (auto&& [k, v] : tbl)
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
						base::print_value(base::source(), source_type);
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

	#if !TOML_ALL_INLINE
		extern template class TOML_API default_formatter<char>;
	#endif

	default_formatter(const table&) -> default_formatter<char>;
	default_formatter(const array&) -> default_formatter<char>;
	template <typename T> default_formatter(const value<T>&) -> default_formatter<char>;

	template <typename T, typename U>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, default_formatter<U>& rhs)
	{
		rhs.attach(lhs);
		rhs.key_path.clear();
		rhs.print();
		rhs.detach();
		return lhs;
	}

	template <typename T, typename U>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, default_formatter<U>&& rhs)
	{
		return lhs << rhs; //as lvalue
	}

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const table& rhs)
	{
		return lhs << default_formatter<Char>{ rhs };
	}

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const array& rhs)
	{
		return lhs << default_formatter<Char>{ rhs };
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const table&);
		extern template TOML_API std::ostream& operator << (std::ostream&, const array&);
	#endif
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//-----------  ↑ toml_default_formatter.h  -----------------------------------------------------------------------------

//--------------------------------------  ↓ toml_json_formatter.h  -----------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&);
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&&);

	template <typename Char = char>
	class TOML_API json_formatter final : impl::formatter<Char>
	{
		private:
			using base = impl::formatter<Char>;

			void print(const toml::table& tbl);
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
						TOML_ASSUME(type != node_type::none);
						switch (type)
						{
							case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print_value(v, type);
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
					default: base::print_value(base::source(), source_type);
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

	#if !TOML_ALL_INLINE
		extern template class TOML_API json_formatter<char>;
	#endif

	json_formatter(const table&) -> json_formatter<char>;
	json_formatter(const array&) -> json_formatter<char>;
	template <typename T> json_formatter(const value<T>&) -> json_formatter<char>;

	template <typename T, typename U>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>& rhs)
	{
		rhs.attach(lhs);
		rhs.print();
		rhs.detach();
		return lhs;
	}

	template <typename T, typename U>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>&& rhs)
	{
		return lhs << rhs; //as lvalue
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&);
		extern template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&&);
	#endif
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//--------------------------------------  ↑ toml_json_formatter.h  -----------------------------------------------------

#if TOML_PARSER

//----------------------------------------------------------------  ↓ toml_parse_error.h  ------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#if TOML_EXCEPTIONS
	#include <stdexcept>
#endif
TOML_POP_WARNINGS

TOML_PUSH_WARNINGS
TOML_DISABLE_INIT_WARNINGS
TOML_DISABLE_VTABLE_WARNINGS

namespace toml
{
	#if TOML_LARGE_FILES
		TOML_ABI_NAMESPACE_START(lf)
	#else
		TOML_ABI_NAMESPACE_START(sf)
	#endif

	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

	TOML_ABI_NAMESPACE_START(noex)

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

	TOML_ABI_NAMESPACE_START(ex)

	class parse_error final
		: public std::runtime_error
	{
		private:
			source_region source_;

		public:

			TOML_NODISCARD_CTOR
			TOML_GNU_ATTR(nonnull)
			parse_error(const char* desc, source_region&& src) noexcept
				: std::runtime_error{ desc },
				source_{ std::move(src) }
			{}

			TOML_NODISCARD_CTOR
			TOML_GNU_ATTR(nonnull)
			parse_error(const char* desc, const source_region& src) noexcept
				: parse_error{ desc, source_region{ src } }
			{}

			TOML_NODISCARD_CTOR
			TOML_GNU_ATTR(nonnull)
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

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	TOML_ABI_NAMESPACE_END // TOML_LARGE_FILES

	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	std::basic_ostream<Char>& operator << (std::basic_ostream<Char>& lhs, const parse_error& rhs)
	{
		lhs << rhs.description();
		lhs << "\n\t(error occurred at "sv;
		lhs << rhs.source();
		lhs << ")"sv;
		return lhs;
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API std::ostream& operator << (std::ostream&, const parse_error&);
	#endif
}

TOML_POP_WARNINGS

#endif
//----------------------------------------------------------------  ↑ toml_parse_error.h  ------------------------------

//-----------------------------------------------------------------------------------------  ↓ toml_utf8_streams.h  ----
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml::impl
{
	template <typename T>
	class utf8_byte_stream;

	inline constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	template <typename Char>
	class utf8_byte_stream<std::basic_string_view<Char>> final
	{
		static_assert(sizeof(Char) == 1_sz);

		private:
			std::basic_string_view<Char> source;
			size_t position = {};

		public:
			explicit constexpr utf8_byte_stream(std::basic_string_view<Char> sv) noexcept
				: source{ sv }
			{
				// trim trailing nulls
				size_t actual_len = source.length();
				for (size_t i = actual_len; i --> 0_sz;)
				{
					if (source[i] != Char{}) // not '\0'
					{
						actual_len = i + 1_sz;
						break;
					}
				}
				if (source.length() != actual_len) // not '\0'
					source = source.substr(0_sz, actual_len);

				// skip bom
				if (source.length() >= 3_sz && memcmp(utf8_byte_order_mark.data(), source.data(), 3_sz) == 0)
					position += 3_sz;
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool eof() const noexcept
			{
				return position >= source.length();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool peek_eof() const noexcept
			{
				return eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool error() const noexcept
			{
				return false;
			}

			[[nodiscard]]
			constexpr unsigned int operator() () noexcept
			{
				if (position >= source.length())
					return 0xFFFFFFFFu;
				return static_cast<unsigned int>(static_cast<uint8_t>(source[position++]));
			}
	};

	template <typename Char>
	class utf8_byte_stream<std::basic_istream<Char>> final
	{
		static_assert(sizeof(Char) == 1_sz);

		private:
			std::basic_istream<Char>* source;

		public:
			explicit utf8_byte_stream(std::basic_istream<Char>& stream)
				: source{ &stream }
			{
				if (!source->good()) // eof, fail, bad
					return;

				const auto initial_pos = source->tellg();
				Char bom[3];
				source->read(bom, 3);
				if (source->bad() || (source->gcount() == 3 && memcmp(utf8_byte_order_mark.data(), bom, 3_sz) == 0))
					return;

				source->clear();
				source->seekg(initial_pos, std::ios::beg);
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool eof() const noexcept
			{
				return source->eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool peek_eof() const
			{
				using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
				return eof() || source->peek() == stream_traits::eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool error() const noexcept
			{
				return !(*source);
			}

			[[nodiscard]]
			unsigned int operator() ()
			{
				auto val = source->get();
				if (val == std::basic_istream<Char>::traits_type::eof())
					return 0xFFFFFFFFu;
				return static_cast<unsigned int>(val);
			}
	};

	#if TOML_LARGE_FILES
		TOML_ABI_NAMESPACE_START(impl_lf)
	#else
		TOML_ABI_NAMESPACE_START(impl_sf)
	#endif

	struct utf8_codepoint final
	{
		char32_t value;
		string_char bytes[4];
		source_position position;

		template <typename Char = string_char>
		[[nodiscard]]
		TOML_ALWAYS_INLINE
		std::basic_string_view<Char> as_view() const noexcept
		{
			static_assert(
				sizeof(Char) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<Char>{ reinterpret_cast<const Char*>(bytes), 4_sz }
				: std::basic_string_view<Char>{ reinterpret_cast<const Char*>(bytes) };
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr operator char32_t& () noexcept
		{
			return value;
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr operator const char32_t& () const noexcept
		{
			return value;
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr const char32_t& operator* () const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	TOML_ABI_NAMESPACE_END // TOML_LARGE_FILES

	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK	(void)0
		#define TOML_ERROR			throw parse_error
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR			err.emplace
		TOML_ABI_NAMESPACE_START(impl_noex)
	#endif

	TOML_PUSH_WARNINGS
	TOML_DISABLE_VTABLE_WARNINGS

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const source_path_ptr& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;
		[[nodiscard]]
		virtual bool peek_eof() const = 0;

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

			template <typename U, typename String = std::string_view>
			explicit utf8_reader(U && source, String&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				std::memset(codepoints, 0, sizeof(codepoints));
				codepoints[0].position = { 1, 1 };
				codepoints[1].position = { 1, 1 };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<String>(source_path));
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
					uint8_t next_byte;
					{
						unsigned int next_byte_raw{ 0xFFFFFFFFu };
						if constexpr (noexcept(stream()) || !TOML_EXCEPTIONS)
						{
							next_byte_raw = stream();
						}
						#if TOML_EXCEPTIONS
						else
						{
							try
							{
								next_byte_raw = stream();
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

						if (next_byte_raw >= 256u)
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
						next_byte = static_cast<uint8_t>(next_byte_raw);
					}

					decoder(next_byte);
					if (decoder.error())
						TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

					TOML_ERROR_CHECK;

					auto& current = codepoints[cp_idx % 2_sz];
					current.bytes[current_byte_count++] = static_cast<string_char>(next_byte);
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

			[[nodiscard]]
			bool peek_eof() const override
			{
				return stream.peek_eof();
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
			{
				return std::move(err);
			}

			#endif
	};

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string_view) -> utf8_reader<std::basic_string_view<Char>>;

	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string_view) -> utf8_reader<std::basic_istream<Char>>;

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string&&) -> utf8_reader<std::basic_string_view<Char>>;

	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string&&) -> utf8_reader<std::basic_istream<Char>>;

	#if !TOML_EXCEPTIONS
		#undef TOML_ERROR_CHECK
		#define TOML_ERROR_CHECK	if (reader.error()) return nullptr
	#endif

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		public:
			static constexpr size_t max_history_length = 72;

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
					if TOML_UNLIKELY(!history.count && !head)
						head = reader.read_next();

					// subsequent characters and not eof
					else if (head)
					{
						if TOML_UNLIKELY(history.count < history_buffer_size)
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

			[[nodiscard]]
			bool peek_eof() const override
			{
				return reader.peek_eof();
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
	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	TOML_POP_WARNINGS
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS

#endif
//-----------------------------------------------------------------------------------------  ↑ toml_utf8_streams.h  ----

//-----------------  ↓ toml_parser.h  ----------------------------------------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

	TOML_ABI_NAMESPACE_START(parse_noex)

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

			using iterator = table_iterator;
			using const_iterator = const_table_iterator;
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

			[[nodiscard]] node_view<node> operator[] (string_view key) noexcept
			{
				return is_err ? node_view<node>{} : get()[key];
			}

			[[nodiscard]] node_view<const node> operator[] (string_view key) const noexcept
			{
				return is_err ? node_view<const node>{} : get()[key];
			}

			[[nodiscard]] table_iterator begin() noexcept
			{
				return is_err ? table_iterator{} : get().begin();
			}

			[[nodiscard]] const_table_iterator begin() const noexcept
			{
				return is_err ? const_table_iterator{} : get().begin();
			}

			[[nodiscard]] const_table_iterator cbegin() const noexcept
			{
				return is_err ? const_table_iterator{} : get().cbegin();
			}

			[[nodiscard]] table_iterator end() noexcept
			{
				return is_err ? table_iterator{} : get().end();
			}

			[[nodiscard]] const_table_iterator end() const noexcept
			{
				return is_err ? const_table_iterator{} : get().end();
			}

			[[nodiscard]] const_table_iterator cend() const noexcept
			{
				return is_err ? const_table_iterator{} : get().cend();
			}
	};

	TOML_ABI_NAMESPACE_END

	#else

	using parse_result = table;

	#endif
}

namespace toml::impl
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		TOML_ABI_NAMESPACE_START(impl_noex)
	#endif

	[[nodiscard]] TOML_API
	parse_result do_parse(utf8_reader_interface&&) TOML_MAY_THROW;

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
}

namespace toml
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(parse_ex)
	#else
		TOML_ABI_NAMESPACE_START(parse_noex)
	#endif

	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#ifdef __cpp_lib_char8_t

	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#endif // __cpp_lib_char8_t

	template <typename Char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::basic_istream<Char>& doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	template <typename Char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::basic_istream<Char>& doc, std::string&& source_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	// Q: "why are the parse_file functions templated??"
	// A: I don't want to force users to drag in <fstream> if they're not going to do
	//    any parsing directly from files. Keeping them templated delays their instantiation
	//    until they're actually required, so only those users wanting to use parse_file()
	//    are burdened by the <fstream> overhead.

	template <typename Char, typename StreamChar = char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::basic_string_view<Char> file_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The path's character type must be 1 byte in size."
		);
		static_assert(
			sizeof(StreamChar) == 1,
			"The stream's character type must be 1 byte in size."
		);

		#if TOML_EXCEPTIONS
			#define TOML_PARSE_FILE_ERROR(msg, pos)															\
				throw parse_error{ msg, pos, std::make_shared<const std::string>(std::move(file_path_str)) }
		#else
			#define TOML_PARSE_FILE_ERROR(msg, pos)															\
				return parse_result{																		\
					parse_error{ msg, pos, std::make_shared<const std::string>(std::move(file_path_str)) }	\
				}
		#endif

		auto file_path_str = std::string(reinterpret_cast<const char*>(file_path.data()), file_path.length());

		// open file with a custom-sized stack buffer
		using ifstream = std::basic_ifstream<StreamChar>;
		ifstream file;
		StreamChar file_buffer[sizeof(void*) * 4096_sz];
		file.rdbuf()->pubsetbuf(file_buffer, sizeof(file_buffer));
		file.open(file_path_str, ifstream::in | ifstream::binary | ifstream::ate);
		if (!file.is_open())
			TOML_PARSE_FILE_ERROR("File could not be opened for reading", source_position{});

		// get size
		const auto file_size = file.tellg();
		if (file_size == -1)
			TOML_PARSE_FILE_ERROR("Could not determine file size", source_position{});
		file.seekg(0, std::ios::beg);

		// if the file size is the sweet spot, read the whole thing into memory and parse from there
		constexpr auto small_file_threshold = 1024 * 32; //32 kilobytes
		constexpr auto large_file_threshold = 1024 * 1024 * static_cast<int>(sizeof(void*)) * 8; // 64 megabytes on 64-bit
		if (file_size >= small_file_threshold && file_size <= large_file_threshold)
		{
			std::vector<StreamChar> file_data;
			file_data.resize(static_cast<size_t>(file_size));
			file.read(file_data.data(), static_cast<std::streamsize>(file_size));
			return parse(std::basic_string_view<StreamChar>{ file_data.data(), file_data.size() }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		else
			return parse(file, std::move(file_path_str));

		#undef TOML_PARSE_FILE_ERROR
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API parse_result parse(std::istream&, std::string_view) TOML_MAY_THROW;
		extern template TOML_API parse_result parse(std::istream&, std::string&&) TOML_MAY_THROW;
		extern template TOML_API parse_result parse_file(std::string_view) TOML_MAY_THROW;
		#ifdef __cpp_lib_char8_t
			extern template TOML_API parse_result parse_file(std::u8string_view) TOML_MAY_THROW;
		#endif
	#endif

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const std::basic_string<Char>& file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const Char* file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	inline namespace literals
	{
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(lit_ex)
		#else
			TOML_ABI_NAMESPACE_START(lit_noex)
		#endif

		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW;

		#ifdef __cpp_lib_char8_t

		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW;

		#endif // __cpp_lib_char8_t

		TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS

#endif
//-----------------  ↑ toml_parser.h  ----------------------------------------------------------------------------------

#endif // TOML_PARSER

#if TOML_IMPLEMENTATION

//------------------------------------------  ↓ toml_node.hpp  ---------------------------------------------------------
#if 1

namespace toml
{
	TOML_EXTERNAL_LINKAGE
	node::node(node && other) noexcept
		: source_{ std::move(other.source_) }
	{
		other.source_.begin = {};
		other.source_.end = {};
	}

	TOML_EXTERNAL_LINKAGE
	node & node::operator= (node && rhs) noexcept
	{
		source_ = std::move(rhs.source_);
		rhs.source_.begin = {};
		rhs.source_.end = {};
		return *this;
	}

	TOML_EXTERNAL_LINKAGE bool node::is_string() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_integer() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_floating_point() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_number() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_boolean() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_date() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_time() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_date_time() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_array_of_tables() const noexcept { return false; }

	TOML_EXTERNAL_LINKAGE table* node::as_table() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE array* node::as_array() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<string>* node::as_string() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<int64_t>* node::as_integer() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<double>* node::as_floating_point() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<bool>* node::as_boolean() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<date>* node::as_date() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<time>* node::as_time() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<date_time>* node::as_date_time() noexcept { return nullptr; }

	TOML_EXTERNAL_LINKAGE const table* node::as_table() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const array* node::as_array() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<string>* node::as_string() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<int64_t>* node::as_integer() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<double>* node::as_floating_point() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<bool>* node::as_boolean() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<date>* node::as_date() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<time>* node::as_time() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<date_time>* node::as_date_time() const noexcept { return nullptr; }

	TOML_EXTERNAL_LINKAGE const source_region& node::source() const noexcept { return source_; }
}

#endif
//------------------------------------------  ↑ toml_node.hpp  ---------------------------------------------------------

//------------------------------------------------------------------  ↓ toml_array.hpp  --------------------------------
#if 1

namespace toml
{
	TOML_EXTERNAL_LINKAGE
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

	TOML_EXTERNAL_LINKAGE
	array::array() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	array::array(array&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) }
	{}

	TOML_EXTERNAL_LINKAGE
	array& array::operator= (array&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		return *this;
	}

	TOML_EXTERNAL_LINKAGE node_type array::type() const noexcept { return node_type::array; }
	TOML_EXTERNAL_LINKAGE bool array::is_table() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool array::is_array() const noexcept { return true; }
	TOML_EXTERNAL_LINKAGE bool array::is_value() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE array* array::as_array() noexcept { return this; }
	TOML_EXTERNAL_LINKAGE const array* array::as_array() const noexcept { return this; }

	TOML_EXTERNAL_LINKAGE node& array::operator[] (size_t index) noexcept { return *values[index]; }
	TOML_EXTERNAL_LINKAGE const node& array::operator[] (size_t index) const noexcept { return *values[index]; }

	TOML_EXTERNAL_LINKAGE node& array::front() noexcept { return *values.front(); }
	TOML_EXTERNAL_LINKAGE const node& array::front() const noexcept { return *values.front(); }
	TOML_EXTERNAL_LINKAGE node& array::back() noexcept { return *values.back(); }
	TOML_EXTERNAL_LINKAGE const node& array::back() const noexcept { return *values.back(); }

	TOML_EXTERNAL_LINKAGE array::iterator array::begin() noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::begin() const noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_EXTERNAL_LINKAGE array::iterator array::end() noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::end() const noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::cend() const noexcept { return { values.cend() }; }

	TOML_EXTERNAL_LINKAGE bool array::empty() const noexcept { return values.empty(); }
	TOML_EXTERNAL_LINKAGE size_t array::size() const noexcept { return values.size(); }
	TOML_EXTERNAL_LINKAGE void array::reserve(size_t new_capacity) { values.reserve(new_capacity); }
	TOML_EXTERNAL_LINKAGE void array::clear() noexcept { values.clear(); }

	TOML_EXTERNAL_LINKAGE size_t array::max_size() const noexcept { return values.max_size(); }
	TOML_EXTERNAL_LINKAGE size_t array::capacity() const noexcept { return values.capacity(); }
	TOML_EXTERNAL_LINKAGE void array::shrink_to_fit() { values.shrink_to_fit(); }

	TOML_EXTERNAL_LINKAGE
	void array::truncate(size_t new_size)
	{
		if (new_size < values.size())
			values.resize(new_size);
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	void array::pop_back() noexcept
	{
		values.pop_back();
	}

	TOML_EXTERNAL_LINKAGE
	node* array::get(size_t index) noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	const node* array::get(size_t index) const noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
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
	TOML_EXTERNAL_LINKAGE
	bool operator != (const array& lhs, const array& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	TOML_EXTERNAL_LINKAGE
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

	TOML_EXTERNAL_LINKAGE
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

	TOML_EXTERNAL_LINKAGE
	array& array::flatten() &
	{
		if (values.empty())
			return *this;

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
			return *this;

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

		return *this;
	}
}

#endif
//------------------------------------------------------------------  ↑ toml_array.hpp  --------------------------------

//-------------------------------------------------------------------------------------------  ↓ toml_table.hpp  -------
#if 1

namespace toml
{
	TOML_EXTERNAL_LINKAGE
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

	TOML_EXTERNAL_LINKAGE
	table::table() noexcept {}

	TOML_EXTERNAL_LINKAGE
	table::table(table&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) },
		inline_{ other.inline_ }
	{}

	TOML_EXTERNAL_LINKAGE
	table& table::operator = (table&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		inline_ = rhs.inline_;
		return *this;
	}

	TOML_EXTERNAL_LINKAGE node_type table::type() const noexcept { return node_type::table; }
	TOML_EXTERNAL_LINKAGE bool table::is_table() const noexcept { return true; }
	TOML_EXTERNAL_LINKAGE bool table::is_array() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool table::is_value() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE table* table::as_table() noexcept { return this; }
	TOML_EXTERNAL_LINKAGE const table* table::as_table() const noexcept { return this; }
	TOML_EXTERNAL_LINKAGE bool table::is_inline() const noexcept { return inline_; }
	TOML_EXTERNAL_LINKAGE void table::is_inline(bool val) noexcept { inline_ = val; }

	TOML_EXTERNAL_LINKAGE
	node_view<node> table::operator[] (string_view key) noexcept
	{
		return { this->get(key) };
	}
	TOML_EXTERNAL_LINKAGE
	node_view<const node> table::operator[] (string_view key) const noexcept
	{
		return { this->get(key) };
	}

	TOML_EXTERNAL_LINKAGE table::iterator table::begin() noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::begin() const noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_EXTERNAL_LINKAGE table::iterator table::end() noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::end() const noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::cend() const noexcept { return { values.cend() }; }

	TOML_EXTERNAL_LINKAGE bool table::empty() const noexcept { return values.empty(); }
	TOML_EXTERNAL_LINKAGE size_t table::size() const noexcept { return values.size(); }
	TOML_EXTERNAL_LINKAGE void table::clear() noexcept { values.clear(); }

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	bool table::erase(string_view key) noexcept
	{
		if (auto it = values.find(key); it != values.end())
		{
			values.erase(it);
			return true;
		}
		return false;
	}

	TOML_EXTERNAL_LINKAGE
	node* table::get(string_view key) noexcept
	{
		return do_get(values, key);
	}

	TOML_EXTERNAL_LINKAGE
	const node* table::get(string_view key) const noexcept
	{
		return do_get(values, key);
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::find(string_view key) noexcept
	{
		return { values.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	table::const_iterator table::find(string_view key) const noexcept
	{
		return { values.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	bool table::contains(string_view key) const noexcept
	{
		return do_contains(values, key);
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
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
	TOML_EXTERNAL_LINKAGE
	bool operator != (const table& lhs, const table& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

#endif
//-------------------------------------------------------------------------------------------  ↑ toml_table.hpp  -------

//----------  ↓ toml_default_formatter.hpp  ----------------------------------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_FLOAT_WARNINGS

namespace toml::impl
{
	inline constexpr size_t default_formatter_line_wrap = 120_sz;

	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	TOML_API
	TOML_EXTERNAL_LINKAGE
	string default_formatter_make_key_segment(const string& str) noexcept
	{
		if (str.empty())
			return TOML_STRING_PREFIX("''"s);
		else
		{
			bool requiresQuotes = false;
			{
				utf8_decoder decoder;
				for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
				{
					decoder(static_cast<uint8_t>(str[i]));
					if (decoder.error())
						requiresQuotes = true;
					else if (decoder.has_code_point())
						requiresQuotes = !is_bare_key_character(decoder.codepoint);
				}
			}

			if (requiresQuotes)
			{
				string s;
				s.reserve(str.length() + 2_sz);
				s += TOML_STRING_PREFIX('"');
				for (auto c : str)
				{
					if TOML_UNLIKELY(c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F'))
					{
						const auto& sv = low_character_escape_table[c];
						s.append(reinterpret_cast<const string_char*>(sv.data()), sv.length());
					}
					else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('\x7F'))
						s.append(TOML_STRING_PREFIX("\\u007F"sv));
					else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('"'))
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

	TOML_API
	TOML_EXTERNAL_LINKAGE
	size_t default_formatter_inline_columns(const node& node) noexcept
	{
		switch (node.type())
		{
			case node_type::table:
			{
				auto& n = *reinterpret_cast<const table*>(&node);
				if (n.empty())
					return 2_sz; // "{}"
				size_t weight = 3_sz; // "{ }"
				for (auto&& [k, v] : n)
				{
					weight += k.length() + default_formatter_inline_columns(v) + 2_sz; // +  ", "
					if (weight >= default_formatter_line_wrap)
						break;
				}
				return weight;
			}

			case node_type::array:
			{
				auto& n = *reinterpret_cast<const array*>(&node);
				if (n.empty())
					return 2_sz; // "[]"
				size_t weight = 3_sz; // "[ ]"
				for (auto& elem : n)
				{
					weight += default_formatter_inline_columns(elem) + 2_sz; // +  ", "
					if (weight >= default_formatter_line_wrap)
						break;
				}
				return weight;
			}

			case node_type::string:
			{
				auto& n = *reinterpret_cast<const value<string>*>(&node);
				return n.get().length() + 2_sz; // + ""
			}

			case node_type::integer:
			{
				auto& n = *reinterpret_cast<const value<int64_t>*>(&node);
				auto v = n.get();
				if (!v)
					return 1_sz;
				size_t weight = {};
				if (v < 0)
				{
					weight += 1;
					v *= -1;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(v))) + 1_sz;
			}

			case node_type::floating_point:
			{
				auto& n = *reinterpret_cast<const value<double>*>(&node);
				auto v = n.get();
				if (v == 0.0)
					return 3_sz;  // "0.0"
				size_t weight = 2_sz; // ".0"
				if (v < 0.0)
				{
					weight += 1;
					v *= -1.0;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(v))) + 1_sz;
				break;
			}

			case node_type::boolean: return 5_sz;
			case node_type::date: [[fallthrough]];
			case node_type::time: return 10_sz;
			case node_type::date_time: return 30_sz;
			case node_type::none: TOML_UNREACHABLE;
			TOML_NO_DEFAULT_CASE;
		}

		TOML_UNREACHABLE;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias) noexcept
	{
		return (default_formatter_inline_columns(node) + starting_column_bias) > default_formatter_line_wrap;
	}
}

namespace toml
{
	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	void default_formatter<Char>::print_inline(const toml::table& tbl)
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream("{ "sv, base::stream());

			bool first = false;
			for (auto&& [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;

				print_key_segment(k);
				impl::print_to_stream(" = "sv, base::stream());

				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print_value(v, type);
				}
			}

			impl::print_to_stream(" }"sv, base::stream());
		}
		base::clear_naked_newline();
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_FLOAT_WARNINGS

#endif
//----------  ↑ toml_default_formatter.hpp  ----------------------------------------------------------------------------

//-------------------------------------  ↓ toml_json_formatter.hpp  ----------------------------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS

namespace toml
{
	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	void json_formatter<Char>::print(const toml::table& tbl)
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream('{', base::stream());
			base::increase_indent();
			bool first = false;
			for (auto&& [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;
				base::print_newline(true);
				base::print_indent();

				base::print_quoted_string(k);
				impl::print_to_stream(" : "sv, base::stream());

				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print_value(v, type);
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

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS

#endif
//-------------------------------------  ↑ toml_json_formatter.hpp  ----------------------------------------------------

#if TOML_PARSER

//------------------------------------------------------------------  ↓ toml_parser.hpp  -------------------------------
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <cmath>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
	#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
	#include <sstream>
#endif
#if !TOML_ALL_INLINE
	using namespace std::string_view_literals;
#endif
TOML_POP_WARNINGS

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

#if TOML_EXCEPTIONS && !defined(__INTELLISENSE__)
	#define TOML_RETURNS_BY_THROWING		[[noreturn]]
#else
	#define TOML_RETURNS_BY_THROWING
#endif

namespace TOML_INTERNAL_NAMESPACE
{
	template <uint64_t> struct parse_integer_traits;
	template <> struct parse_integer_traits<2> final
	{
		static constexpr auto scope_qualifier = "binary integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_binary_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 63;
		static constexpr auto prefix_codepoint = U'b';
		static constexpr auto prefix = "b"sv;
	};
	template <> struct parse_integer_traits<8> final
	{
		static constexpr auto scope_qualifier = "octal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_octal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 21; // strlen("777777777777777777777")
		static constexpr auto prefix_codepoint = U'o';
		static constexpr auto prefix = "o"sv;
	};
	template <> struct parse_integer_traits<10> final
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_decimal_digit;
		static constexpr auto is_signed = true;
		static constexpr auto buffer_length = 19; //strlen("9223372036854775807")
	};
	template <> struct parse_integer_traits<16> final
	{
		static constexpr auto scope_qualifier = "hexadecimal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_hexadecimal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 16; //strlen("7FFFFFFFFFFFFFFF")
		static constexpr auto prefix_codepoint = U'x';
		static constexpr auto prefix = "x"sv;
	};

	[[nodiscard]]
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(::toml::node_type val) noexcept
	{
		using namespace ::toml::impl;

		return node_type_friendly_names[unbox_enum(val)];
	}

	[[nodiscard]]
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const std::string& str) noexcept
	{
		return std::string_view{ str };
	}

	[[nodiscard]]
	TOML_GNU_ATTR(const)
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(bool val) noexcept
	{
		using namespace std::string_view_literals;

		return val ? "true"sv : "false"sv;
	}

	[[nodiscard]]
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const ::toml::impl::utf8_codepoint& cp) noexcept
	{
		using namespace ::toml;
		using namespace ::toml::impl;

		if TOML_UNLIKELY(cp.value <= U'\x1F')
			return low_character_escape_table[cp.value];
		else if TOML_UNLIKELY(cp.value == U'\x7F')
			return "\\u007F"sv;
		else
			return cp.template as_view<char>();
	}

	template <typename T>
	TOML_GNU_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	TOML_NEVER_INLINE
	void concatenate(char*& write_pos, char *const buf_end, const T& arg) noexcept
	{
		using namespace ::toml;
		using namespace ::toml::impl;

		static_assert(
			is_one_of<remove_cvref_t<T>, std::string_view, int64_t, uint64_t, double>,
			"concatenate inputs are limited to std::string_view, int64_t, uint64_t and double to keep "
			"instantiations to a minimum as an anti-bloat measure (hint: to_sv will probably help)"
		);

		if (write_pos >= buf_end)
			return;

		using arg_t = remove_cvref_t<T>;
		if constexpr (std::is_same_v<arg_t, std::string_view>)
		{
			const auto max_chars = static_cast<size_t>(buf_end - write_pos);
			const auto len = max_chars < arg.length() ? max_chars : arg.length();
			std::memcpy(write_pos, arg.data(), len);
			write_pos += len;
		}
		else if constexpr (std::is_floating_point_v<arg_t>)
		{
			#if TOML_FLOAT_CHARCONV
			{
				const auto result = std::to_chars(write_pos, buf_end, arg);
				write_pos = result.ptr;
			}
			#else
			{
				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				ss.precision(std::numeric_limits<arg_t>::digits10 + 1);
				ss << arg;
				concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
			}
			#endif
		}
		else if constexpr (std::is_integral_v<arg_t>)
		{
			#if TOML_INT_CHARCONV
			{
				const auto result = std::to_chars(write_pos, buf_end, arg);
				write_pos = result.ptr;
			}
			#else
			{
				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				using cast_type = std::conditional_t<std::is_signed_v<arg_t>, int64_t, uint64_t>;
				ss << static_cast<cast_type>(arg);
				concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
			}
			#endif
		}
	}

	struct error_builder final
	{
		static constexpr std::size_t buf_size = 512;
		char buf[buf_size];
		char* write_pos = buf;
		char* const max_write_pos = buf + (buf_size - std::size_t{ 1 }); //allow for null terminator

		error_builder(std::string_view scope) noexcept
		{
			concatenate(write_pos, max_write_pos, "Error while parsing "sv);
			concatenate(write_pos, max_write_pos, scope);
			concatenate(write_pos, max_write_pos, ": "sv);
		}

		template <typename T>
		TOML_ALWAYS_INLINE
		void append(const T& arg) noexcept
		{
			concatenate(write_pos, max_write_pos, arg);
		}

		TOML_RETURNS_BY_THROWING
		auto finish(const ::toml::source_position& pos, const ::toml::source_path_ptr& source_path) const TOML_MAY_THROW
		{
			using namespace ::toml;

			*write_pos = '\0';

			#if TOML_EXCEPTIONS
				throw parse_error{ buf, pos, source_path };
			#else
				return parse_error{
					std::string(buf, static_cast<size_t>(write_pos - buf)),
					pos,
					source_path
				};
			#endif
		}
	};

	struct parse_scope final
	{
		std::string_view& storage_;
		std::string_view parent_;

		TOML_NODISCARD_CTOR
		explicit parse_scope(std::string_view& current_scope, std::string_view new_scope) noexcept
			: storage_{ current_scope },
			parent_{ current_scope }
		{
			storage_ = new_scope;
		}

		~parse_scope() noexcept
		{
			storage_ = parent_;
		}
	};
	#define push_parse_scope_2(scope, line)		parse_scope ps_##line{ current_scope, scope }
	#define push_parse_scope_1(scope, line)		push_parse_scope_2(scope, line)
	#define push_parse_scope(scope)				push_parse_scope_1(scope, __LINE__)

	struct parsed_key final
	{
		std::vector<toml::string> segments;
	};

	struct parsed_key_value_pair final
	{
		parsed_key key;
		std::unique_ptr<toml::node> value;
	};
}

namespace toml::impl
{
	#if defined(NDEBUG) || !defined(_DEBUG)
		#define assert_or_assume(cond)			TOML_ASSUME(cond)
	#else
		#define assert_or_assume(cond)			TOML_ASSERT(cond)
	#endif

	// Q: "what the fuck is this? MACROS????"
	// A: The parser needs to work in exceptionless mode (returning error objects directly)
	//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
	//    These macros encapsulate the differences between the two modes so I can write code code
	//    as though I was only targeting one mode and not want yeet myself into the sun.
	//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
	//    of toml++.

	#define is_eof()							!cp
	#define assert_not_eof()					assert_or_assume(cp)
	#define return_if_eof(...)					do { if (is_eof()) return __VA_ARGS__; } while(false)
	#if TOML_EXCEPTIONS
		#define is_error()						false
		#define return_after_error(...)			TOML_UNREACHABLE
		#define assert_not_error()				(void)0
		#define return_if_error(...)			(void)0
		#define return_if_error_or_eof(...)		return_if_eof(__VA_ARGS__)
	#else
		#define is_error()						!!err
		#define return_after_error(...)			return __VA_ARGS__
		#define assert_not_error()				TOML_ASSERT(!is_error())
		#define return_if_error(...)			do { if (is_error()) return __VA_ARGS__; } while(false)
		#define return_if_error_or_eof(...)		do { if (is_eof() || is_error()) return __VA_ARGS__; } while(false)
	#endif
	#define set_error_and_return(ret, ...)		\
		do { if (!is_error()) set_error(__VA_ARGS__); return_after_error(ret); } while(false)
	#define set_error_and_return_default(...)	set_error_and_return({}, __VA_ARGS__)
	#define set_error_and_return_if_eof(...)	\
		do { if (is_eof()) set_error_and_return(__VA_ARGS__, "encountered end-of-file"sv); } while(false)
	#define advance_and_return_if_error(...)	\
		do { assert_not_eof(); advance(); return_if_error(__VA_ARGS__); } while (false)
	#define advance_and_return_if_error_or_eof(...)		\
		do {											\
			assert_not_eof();							\
			advance();									\
			return_if_error(__VA_ARGS__);				\
			set_error_and_return_if_eof(__VA_ARGS__);	\
		} while (false)

	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		TOML_ABI_NAMESPACE_START(impl_noex)
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
			bool recording = false, recording_whitespace = true;
			std::string_view current_scope;
			#if !TOML_EXCEPTIONS
			mutable optional<toml::parse_error> err;
			#endif

			[[nodiscard]]
			source_position current_position(source_index fallback_offset = 0) const noexcept
			{
				if (!is_eof())
					return cp->position;
				return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
			}

			template <typename... T>
			TOML_RETURNS_BY_THROWING
			TOML_NEVER_INLINE
			void set_error_at(source_position pos, const T&... reason) const TOML_MAY_THROW
			{
				static_assert(sizeof...(T) > 0_sz);
				#if !TOML_EXCEPTIONS
				if (err)
					return;
				#endif

				error_builder builder{ current_scope };
				(builder.append(reason), ...);

				#if TOML_EXCEPTIONS
					builder.finish(pos, reader.source_path());
				#else
					err.emplace(builder.finish(pos, reader.source_path()));
				#endif
			}

			template <typename... T>
			TOML_RETURNS_BY_THROWING
			TOML_ALWAYS_INLINE
			void set_error(const T&... reason) const TOML_MAY_THROW
			{
				set_error_at(current_position(1), reason...);
			}

			void go_back(size_t count = 1_sz) noexcept
			{
				return_if_error();
				assert_or_assume(count);

				cp = reader.step_back(count);
				prev_pos = cp->position;
			}

			void advance() TOML_MAY_THROW
			{
				return_if_error();
				assert_not_eof();

				prev_pos = cp->position;
				cp = reader.read_next();

				#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
				#endif

				if (recording && !is_eof())
				{
					if (recording_whitespace || !(is_whitespace(*cp) || is_line_break(*cp)))
						recording_buffer.append(cp->as_view<char>());
				}
			}

			void start_recording(bool include_current = true) noexcept
			{
				return_if_error();

				recording = true;
				recording_whitespace = true;
				recording_buffer.clear();
				if (include_current && !is_eof())
					recording_buffer.append(cp->as_view<char>());
			}

			void stop_recording(size_t pop_bytes = 0_sz) noexcept
			{
				return_if_error();

				recording = false;
				if (pop_bytes)
				{
					if (pop_bytes >= recording_buffer.length())
						recording_buffer.clear();
					else if (pop_bytes == 1_sz)
						recording_buffer.pop_back();
					else
						recording_buffer.erase(
							recording_buffer.begin() + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
							recording_buffer.end()
						);
				}
			}

			bool consume_leading_whitespace() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				bool consumed = false;
				while (!is_eof() && is_whitespace(*cp))
				{
					consumed = true;
					advance_and_return_if_error({});
				}
				return consumed;
			}

			bool consume_line_break() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				if (!is_line_break(*cp))
					return false;

				if (*cp == U'\r')
				{
					advance_and_return_if_error({}); // skip \r

					if (is_eof())
						return true; //eof after \r is 'fine'
					else if (*cp != U'\n')
						set_error_and_return_default("expected \\n, saw '"sv, to_sv(*cp), "'"sv);
				}
				advance_and_return_if_error({}); // skip \n (or other single-character line ending)
				return true;
			}

			bool consume_rest_of_line() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				do
				{
					if (is_line_break(*cp))
						return consume_line_break();
					else
						advance();
					return_if_error({});
				}
				while (!is_eof());

				return true;
			}

			bool consume_comment() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				if (*cp != U'#')
					return false;

				push_parse_scope("comment"sv);

				advance_and_return_if_error({}); //skip the '#'

				while (!is_eof())
				{
					if (consume_line_break())
						return true;

					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						// toml/issues/567 (disallow non-TAB control characters in comments)
						if (is_nontab_control_character(*cp))
							set_error_and_return_default(
								"control characters other than TAB (U+0009) are explicitly prohibited"sv
							);

						// toml/pull/720 (disallow surrogates in comments)
						else if (is_unicode_surrogate(*cp))
							set_error_and_return_default(
								"unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv
							);
					}
					advance_and_return_if_error({});
				}

				return true;
			}

			[[nodiscard]]
			bool consume_expected_sequence(std::u32string_view seq) TOML_MAY_THROW
			{
				return_if_error({});
				TOML_ASSERT(!seq.empty());

				for (auto c : seq)
				{
					set_error_and_return_if_eof({});
					if (*cp != c)
						return false;
					advance_and_return_if_error({});
				}
				return true;
			}

			template <typename T>
			[[nodiscard]]
			bool consume_digit_sequence(T* digits, size_t len) TOML_MAY_THROW
			{
				return_if_error({});
				assert_or_assume(digits);
				assert_or_assume(len);

				for (size_t i = 0; i < len; i++)
				{
					set_error_and_return_if_eof({});
					if (!is_decimal_digit(*cp))
						return false;

					digits[i] = static_cast<T>(*cp - U'0');
					advance_and_return_if_error({});
				}
				return true;
			}

			template <typename T>
			[[nodiscard]]
			size_t consume_variable_length_digit_sequence(T* buffer, size_t max_len) TOML_MAY_THROW
			{
				return_if_error({});
				assert_or_assume(buffer);
				assert_or_assume(max_len);

				size_t i = {};
				for (; i < max_len; i++)
				{
					if (is_eof() || !is_decimal_digit(*cp))
						break;

					buffer[i] = static_cast<T>(*cp - U'0');
					advance_and_return_if_error({});
				}
				return i;
			}

			//template <bool MultiLine>
			[[nodiscard]]
			string parse_basic_string(bool multi_line) TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'"');
				push_parse_scope("string"sv);

				// skip the '"'
				advance_and_return_if_error_or_eof({});

				// multiline strings ignore a single line ending right at the beginning
				if (multi_line)
				{
					consume_line_break();
					return_if_error({});
					set_error_and_return_if_eof({});
				}

				string str;
				bool escaped = false;
				[[maybe_unused]] bool skipping_whitespace = false;
				do
				{
					if (escaped)
					{
						escaped = false;

						// handle 'line ending slashes' in multi-line mode
						if (multi_line)
						{
							if (is_line_break(*cp) || is_whitespace(*cp))
							{
								consume_leading_whitespace();
								if (!consume_line_break())
									set_error_and_return_default(
										"line-ending backslashes must be the last non-whitespace character on the line"sv
									);
								skipping_whitespace = true;
								return_if_error({});
								continue;
							}
						}

						bool skipped_escaped_codepoint = false;
						assert_not_eof();
						switch (const auto escaped_codepoint = *cp)
						{
							// 'regular' escape codes
							case U'b': str += TOML_STRING_PREFIX('\b'); break;
							case U'f': str += TOML_STRING_PREFIX('\f'); break;
							case U'n': str += TOML_STRING_PREFIX('\n'); break;
							case U'r': str += TOML_STRING_PREFIX('\r'); break;
							case U't': str += TOML_STRING_PREFIX('\t'); break;
							case U'"': str += TOML_STRING_PREFIX('"'); break;
							case U'\\': str += TOML_STRING_PREFIX('\\'); break;

							// unicode scalar sequences
							case U'x':
								#if TOML_LANG_UNRELEASED // toml/pull/709 (\xHH unicode scalar sequences)
									[[fallthrough]];
								#else
									set_error_and_return_default(
										"escape sequence '\\x' is not supported in TOML 1.0.0 and earlier"sv
									);
								#endif
							case U'u': [[fallthrough]];
							case U'U':
							{
								push_parse_scope("unicode scalar escape sequence"sv);
								advance_and_return_if_error_or_eof({});
								skipped_escaped_codepoint = true;

								uint32_t place_value = escaped_codepoint == U'U'
									? 0x10000000u
									: (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
								uint32_t sequence_value{};
								while (place_value)
								{
									set_error_and_return_if_eof({});
									if (!is_hexadecimal_digit(*cp))
										set_error_and_return_default("expected hex digit, saw '"sv, to_sv(*cp), "'"sv);
									sequence_value += place_value * hex_to_dec(*cp);
									place_value /= 16u;
									advance_and_return_if_error({});
								}

								if (is_unicode_surrogate(sequence_value))
									set_error_and_return_default(
										"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv
									);
								else if (sequence_value > 0x10FFFFu)
									set_error_and_return_default("values greater than U+10FFFF are invalid"sv);
								else if (sequence_value <= 0x7Fu) //ascii
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
								set_error_and_return_default("unknown escape sequence '\\"sv, to_sv(*cp), "'"sv);
						}

						// skip the escaped character
						if (!skipped_escaped_codepoint)
							advance_and_return_if_error_or_eof({});
					}
					else
					{
						// handle closing delimiters
						if (*cp == U'"')
						{
							if (multi_line)
							{
								size_t lookaheads = {};
								size_t consecutive_delimiters = 1_sz;
								do
								{
									advance_and_return_if_error({});
									lookaheads++;
									if (!is_eof() && *cp == U'"')
										consecutive_delimiters++;
									else
										break;
								}
								while (lookaheads < 4_sz);

								switch (consecutive_delimiters)
								{
									// """ " (one quote somewhere in a ML string)
									case 1_sz:
										str += TOML_STRING_PREFIX('"');
										skipping_whitespace = false;
										continue;

									// """ "" (two quotes somewhere in a ML string)
									case 2_sz:
										str.append(TOML_STRING_PREFIX("\"\""sv));
										skipping_whitespace = false;
										continue;

									// """ """ (the end of the string)
									case 3_sz:
										return str;

									// """ """" (one at the end of the string)
									case 4_sz:
										str += TOML_STRING_PREFIX('"');
										return str;

									// """ """"" (two quotes at the end of the string)
									case 5_sz:
										str.append(TOML_STRING_PREFIX("\"\""sv));
										advance_and_return_if_error({}); // skip the last '"'
										return str;

									TOML_NO_DEFAULT_CASE;
								}
							}
							else
							{
								advance_and_return_if_error({}); // skip the closing delimiter
								return str;
							}
						}

						// handle escapes
						else if (*cp == U'\\')
						{
							advance_and_return_if_error_or_eof({}); // skip the '\'
							skipping_whitespace = false;
							escaped = true;
							continue;
						}

						// handle line endings in multi-line mode
						if (multi_line && is_line_break(*cp))
						{
							consume_line_break();
							return_if_error({});
							if (!skipping_whitespace)
								str += TOML_STRING_PREFIX('\n');
							continue;
						}

						// handle control characters
						if (is_nontab_control_character(*cp))
							set_error_and_return_default(
								"unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv
							);

						// handle surrogates in strings (1.0.0 and later)
						if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
						{
							if (is_unicode_surrogate(*cp))
								set_error_and_return_default(
									"unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv
								);
						}

						if (multi_line)
						{
							if (!skipping_whitespace || !is_whitespace(*cp))
							{
								skipping_whitespace = false;
								str.append(cp->as_view());
							}
						}
						else
							str.append(cp->as_view());

						advance_and_return_if_error({});
					}
				}
				while (!is_eof());

				set_error_and_return_default("encountered end-of-file"sv);
			}

			[[nodiscard]]
			string parse_literal_string(bool multi_line) TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'\'');
				push_parse_scope("literal string"sv);

				// skip the delimiter
				advance_and_return_if_error_or_eof({});

				// multiline strings ignore a single line ending right at the beginning
				if (multi_line)
				{
					consume_line_break();
					return_if_error({});
					set_error_and_return_if_eof({});
				}

				string str;
				do
				{
					assert_not_error();

					// handle closing delimiters
					if (*cp == U'\'')
					{
						if (multi_line)
						{
							size_t lookaheads = {};
							size_t consecutive_delimiters = 1_sz;
							do
							{
								advance_and_return_if_error({});
								lookaheads++;
								if (!is_eof() && *cp == U'\'')
									consecutive_delimiters++;
								else
									break;
							}
							while (lookaheads < 4_sz);

							switch (consecutive_delimiters)
							{
								// ''' ' (one quote somewhere in a ML string)
								case 1_sz:
									str += TOML_STRING_PREFIX('\'');
									continue;

								// ''' '' (two quotes somewhere in a ML string)
								case 2_sz:
									str.append(TOML_STRING_PREFIX("''"sv));
									continue;

								// ''' ''' (the end of the string)
								case 3_sz:
									return str;

								// ''' '''' (one at the end of the string)
								case 4_sz:
									str += TOML_STRING_PREFIX('\'');
									return str;

								// ''' ''''' (two quotes at the end of the string)
								case 5_sz:
									str.append(TOML_STRING_PREFIX("''"sv));
									advance_and_return_if_error({}); // skip the last '
									return str;

								TOML_NO_DEFAULT_CASE;
							}
						}
						else
						{
							advance_and_return_if_error({}); // skip the closing delimiter
							return str;
						}
					}

					// handle line endings in multi-line mode
					if (multi_line && is_line_break(*cp))
					{
						consume_line_break();
						str += TOML_STRING_PREFIX('\n');
						continue;
					}

					// handle control characters
					if (is_nontab_control_character(*cp))
						set_error_and_return_default(
							"control characters other than TAB (U+0009) are explicitly prohibited"sv
						);

					// handle surrogates in strings (1.0.0 and later)
					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						if (is_unicode_surrogate(*cp))
							set_error_and_return_default(
								"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv
							);
					}

					str.append(cp->as_view());
					advance_and_return_if_error({});
				}
				while (!is_eof());

				set_error_and_return_default("encountered end-of-file"sv);
			}

			struct parsed_string final
			{
				string value;
				bool was_multi_line;
			};

			[[nodiscard]]
			parsed_string parse_string() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_string_delimiter(*cp));
				push_parse_scope("string"sv);

				// get the first three characters to determine the string type
				const auto first = cp->value;
				advance_and_return_if_error_or_eof({});
				const auto second = cp->value;
				advance_and_return_if_error({});
				const auto third = cp ? cp->value : U'\0';

				// if we were eof at the third character then first and second need to be
				// the same string character (otherwise it's an unterminated string)
				if (is_eof())
				{
					if (second == first)
						return {};

					set_error_and_return_default("encountered end-of-file"sv);
				}

				// if the first three characters are all the same string delimiter then
				// it's a multi-line string.
				else if (first == second && first == third)
				{
					return
					{
						first == U'\''
							? parse_literal_string(true)
							: parse_basic_string(true),
						true
					};
				}

				// otherwise it's just a regular string.
				else
				{
					// step back two characters so that the current
					// character is the string delimiter
					go_back(2_sz);

					return
					{
						first == U'\''
							? parse_literal_string(false)
							: parse_basic_string(false),
						false
					};
				}
			}

			[[nodiscard]]
			string parse_bare_key_segment() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_bare_key_character(*cp));

				string segment;

				while (!is_eof())
				{
					if (!is_bare_key_character(*cp))
						break;

					segment.append(cp->as_view());
					advance_and_return_if_error({});
				}

				return segment;
			}

			[[nodiscard]]
			bool parse_boolean() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U't', U'f', U'T', U'F'));
				push_parse_scope("boolean"sv);

				start_recording(true);
				auto result = is_match(*cp, U't', U'T');
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
					set_error_and_return_default(
						"expected '"sv, to_sv(result), "', saw '"sv, to_sv(recording_buffer), "'"sv
					);
				stop_recording();

				if (cp && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

				return result;
			}

			[[nodiscard]]
			double parse_inf_or_nan() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
				push_parse_scope("floating-point"sv);

				start_recording(true);
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});

				const bool inf = is_match(*cp, U'i', U'I');
				if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
					set_error_and_return_default(
						"expected '"sv, inf ? "inf"sv : "nan"sv, "', saw '"sv, to_sv(recording_buffer), "'"sv
					);
				stop_recording();

				if (cp && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

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
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U'+', U'-', U'.') || is_decimal_digit(*cp));
				push_parse_scope("floating-point"sv);

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});

				// consume value chars
				char chars[64];
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent = false;
				char first_integer_part = '\0';
				while (!is_eof() && !is_value_terminator(*cp))
				{
					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							set_error_and_return_default("underscores may only follow digits"sv);

						prev = cp;
						advance_and_return_if_error_or_eof({});
						continue;
					}
					else if (prev && *prev == U'_' && !is_decimal_digit(*cp))
						set_error_and_return_default("underscores must be followed by digits"sv);
					else if (*cp == U'.')
					{
						// .1
						// -.1
						// +.1 (no integer part)
						if (!first_integer_part)
							set_error_and_return_default("expected decimal digit, saw '.'"sv);

						// 1.0e+.10 (exponent cannot have '.')
						else if (seen_exponent)
							set_error_and_return_default("expected exponent decimal digit or sign, saw '.'"sv);

						// 1.0.e+.10
						// 1..0
						// (multiple '.')
						else if (seen_decimal)
							set_error_and_return_default("expected decimal digit or exponent, saw '.'"sv);

						seen_decimal = true;
					}
					else if (is_match(*cp, U'e', U'E'))
					{
						if (prev && !is_decimal_digit(*prev))
							set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

						// 1.0ee+10 (multiple 'e')
						else if (seen_exponent)
							set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

						seen_decimal = true; // implied
						seen_exponent = true;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 1.-0 (sign in mantissa)
						if (!seen_exponent)
							set_error_and_return_default("expected decimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

						// 1.0e1-0 (misplaced exponent sign)
						else if (!is_match(*prev, U'e', U'E'))
							set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);
					}
					else if (length == sizeof(chars))
						set_error_and_return_default(
							"exceeds maximum length of "sv, static_cast<uint64_t>(sizeof(chars)), " characters"sv
						);
					else if (is_decimal_digit(*cp))
					{
						if (!seen_decimal)
						{
							if (!first_integer_part)
								first_integer_part = static_cast<char>(cp->bytes[0]);
							else if (first_integer_part == '0')
								set_error_and_return_default("leading zeroes are prohibited"sv);
						}
					}
					else
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

					chars[length++] = static_cast<char>(cp->bytes[0]);
					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity-check ending state
				if (prev)
				{
					if (*prev == U'_')
					{
						set_error_and_return_if_eof({});
						set_error_and_return_default("underscores must be followed by digits"sv);
					}
					else if (is_match(*prev, U'e', U'E', U'+', U'-', U'.'))
					{
						set_error_and_return_if_eof({});
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);
					}
				}

				// convert to double
				double result;
				#if TOML_FLOAT_CHARCONV
				{
					auto fc_result = std::from_chars(chars, chars + length, result);
					switch (fc_result.ec)
					{
						case std::errc{}: //ok
							return result * sign;

						case std::errc::invalid_argument:
							set_error_and_return_default(
								"'"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
							);
							break;

						case std::errc::result_out_of_range:
							set_error_and_return_default(
								"'"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
							);
							break;

						default: //??
							set_error_and_return_default(
								"an unspecified error occurred while trying to interpret '"sv,
								std::string_view{ chars, length }, "' as a value"sv
							);
					}
				}
				#else
				{
					std::stringstream ss;
					ss.imbue(std::locale::classic());
					ss.write(chars, static_cast<std::streamsize>(length));
					if ((ss >> result))
						return result * sign;
					else
						set_error_and_return_default(
							"'"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
						);
				}
				#endif
			}

			[[nodiscard]]
			double parse_hex_float() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U'0', U'+', U'-'));
				push_parse_scope("hexadecimal floating-point"sv);

				#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});

				// '0'
				if (*cp != U'0')
					set_error_and_return_default(" expected '0', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// 'x' or 'X'
				if (!is_match(*cp, U'x', U'X'))
					set_error_and_return_default("expected 'x' or 'X', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// <HEX DIGITS> ([.]<HEX DIGITS>)? [pP] [+-]? <DEC DIGITS>

				// consume value fragments
				struct fragment
				{
					char chars[24];
					size_t length;
					double value;
				};
				fragment fragments[] =
				{
					{}, // mantissa, whole part
					{}, // mantissa, fractional part
					{}  // exponent
				};
				fragment* current_fragment = fragments;
				const utf8_codepoint* prev = {};
				int exponent_sign = 1;
				while (!is_eof() && !is_value_terminator(*cp))
				{
					if (*cp == U'_')
					{
						if (!prev || !is_hexadecimal_digit(*prev))
							set_error_and_return_default("underscores may only follow digits"sv);

						prev = cp;
						advance_and_return_if_error_or_eof({});
						continue;
					}
					else if (prev && *prev == U'_' && !is_hexadecimal_digit(*cp))
						set_error_and_return_default("underscores must be followed by digits"sv);
					else if (*cp == U'.')
					{
						// 0x10.0p-.0 (exponent cannot have '.')
						if (current_fragment == fragments + 2)
							set_error_and_return_default("expected exponent digit or sign, saw '.'"sv);

						// 0x10.0.p-0 (multiple '.')
						else if (current_fragment == fragments + 1)
							set_error_and_return_default("expected hexadecimal digit or exponent, saw '.'"sv);

						else
							current_fragment++;
					}
					else if (is_match(*cp, U'p', U'P'))
					{
						// 0x10.0pp-0 (multiple 'p')
						if (current_fragment == fragments + 2)
							set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);

						// 0x.p-0 (mantissa is just '.')
						else if (fragments[0].length == 0_sz && fragments[1].length == 0_sz)
							set_error_and_return_default("expected hexadecimal digit, saw '"sv, to_sv(*cp), "'"sv);

						else
							current_fragment = fragments + 2;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 0x-10.0p-0 (sign in mantissa)
						if (current_fragment != fragments + 2)
							set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

						// 0x10.0p0- (misplaced exponent sign)
						else if (!is_match(*prev, U'p', U'P'))
							set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);

						else
							exponent_sign = *cp == U'-' ? -1 : 1;
					}
					else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
						set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);
					else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
						set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);
					else if (current_fragment->length == sizeof(fragment::chars))
						set_error_and_return_default(
							"fragment exceeeds maximum length of "sv,
							static_cast<uint64_t>(sizeof(fragment::chars)), " characters"sv
						);
					else
						current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);

					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity-check ending state
				if (current_fragment != fragments + 2 || current_fragment->length == 0_sz)
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("missing exponent"sv);
				}
				else if (prev && *prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits"sv);
				}

				// calculate values for the three fragments
				for (int fragment_idx = 0; fragment_idx < 3; fragment_idx++)
				{
					auto& f = fragments[fragment_idx];
					const uint32_t base = fragment_idx == 2 ? 10 : 16;

					// left-trim zeroes
					const char* c = f.chars;
					size_t sig = {};
					while (f.length && *c == '0')
					{
						f.length--;
						c++;
						sig++;
					}
					if (!f.length)
						continue;

					// calculate value
					auto place = 1u;
					for (size_t i = 0; i < f.length - 1_sz; i++)
						place *= base;
					uint32_t val{};
					while (place)
					{
						if (base == 16)
							val += place * hex_to_dec(*c);
						else
							val += place * static_cast<uint32_t>(*c - '0');
						if (fragment_idx == 1)
							sig++;
						c++;
						place /= base;
					}
					f.value = static_cast<double>(val);

					// shift the fractional part
					if (fragment_idx == 1)
					{
						while (sig--)
							f.value /= base;
					}
				}

				return (fragments[0].value + fragments[1].value)
					* pow(2.0, fragments[2].value * exponent_sign)
					* sign;

				#else // !TOML_LANG_UNRELEASED

				set_error_and_return_default(
					"hexadecimal floating-point values are not supported "
					"in TOML 1.0.0 and earlier"sv
				);

				#endif // !TOML_LANG_UNRELEASED
			}

			template <uint64_t base>
			[[nodiscard]]
			int64_t parse_integer() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				using traits = parse_integer_traits<base>;
				push_parse_scope(traits::scope_qualifier);

				[[maybe_unused]] int64_t sign = 1;
				if constexpr (traits::is_signed)
				{
					sign = *cp == U'-' ? -1 : 1;
					if (is_match(*cp, U'+', U'-'))
						advance_and_return_if_error_or_eof({});
				}

				if constexpr (base == 10)
				{
					if (!traits::is_digit(*cp))
						set_error_and_return_default("expected expected digit or sign, saw '"sv, to_sv(*cp), "'"sv);
				}
				else
				{
					// '0'
					if (*cp != U'0')
						set_error_and_return_default("expected '0', saw '"sv, to_sv(*cp), "'"sv);
					advance_and_return_if_error_or_eof({});

					// 'b', 'o', 'x'
					if (*cp != traits::prefix_codepoint)
						set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, to_sv(*cp), "'"sv);
					advance_and_return_if_error_or_eof({});
				}

				// consume value chars
				char chars[traits::buffer_length];
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (!is_eof() && !is_value_terminator(*cp))
				{
					if (*cp == U'_')
					{
						if (!prev || !traits::is_digit(*prev))
							set_error_and_return_default("underscores may only follow digits"sv);

						prev = cp;
						advance_and_return_if_error_or_eof({});
						continue;
					}
					else if (prev && *prev == U'_' && !traits::is_digit(*cp))
						set_error_and_return_default("underscores must be followed by digits"sv);
					else if (!traits::is_digit(*cp))
						set_error_and_return_default("expected digit, saw '"sv, to_sv(*cp), "'"sv);
					else if (length == sizeof(chars))
						set_error_and_return_default(
							"exceeds maximum length of "sv,
							static_cast<uint64_t>(sizeof(chars)), " characters"sv
						);
					else
						chars[length++] = static_cast<char>(cp->bytes[0]);

					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity check ending state
				if (prev && *prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits"sv);
				}

				// check for leading zeroes
				if constexpr (base == 10)
				{
					if (chars[0] == '0')
						set_error_and_return_default("leading zeroes are prohibited"sv);
				}

				// single digits can be converted trivially
				if (length == 1_sz)
				{
					if constexpr (base == 16)
						return static_cast<int64_t>(hex_to_dec(chars[0]));
					else if constexpr (base <= 10)
						return static_cast<int64_t>(chars[0] - '0');
				}

				// otherwise do the thing
				uint64_t result = {};
				{
					const char* msd = chars;
					const char* end = msd + length;
					while (msd < end && *msd == '0')
						msd++;
					if (msd == end)
						return 0ll;
					uint64_t power = 1;
					while (--end >= msd)
					{
						if constexpr (base == 16)
							result += power * hex_to_dec(*end);
						else
							result += power * static_cast<uint64_t>(*end - '0');
						power *= base;
					}
				}

				// range check
				if (result > static_cast<uint64_t>((std::numeric_limits<int64_t>::max)()) + (sign < 0 ? 1ull : 0ull))
					set_error_and_return_default(
						"'"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
					);

				if constexpr (traits::is_signed)
					return static_cast<int64_t>(result) * sign;
				else
					return static_cast<int64_t>(result);
			}

			[[nodiscard]]
			date parse_date(bool part_of_datetime = false) TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("date"sv);

				// "YYYY"
				uint32_t digits[4];
				if (!consume_digit_sequence(digits, 4_sz))
					set_error_and_return_default("expected 4-digit year, saw '"sv, to_sv(*cp), "'"sv);
				const auto year = digits[3]
					+ digits[2] * 10u
					+ digits[1] * 100u
					+ digits[0] * 1000u;
				const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
				set_error_and_return_if_eof({});

				// '-'
				if (*cp != U'-')
					set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit month, saw '"sv, to_sv(*cp), "'"sv);
				const auto month = digits[1] + digits[0] * 10u;
				if (month == 0u || month > 12u)
					set_error_and_return_default(
						"expected month between 1 and 12 (inclusive), saw "sv, static_cast<uint64_t>(month)
					);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;
				set_error_and_return_if_eof({});

				// '-'
				if (*cp != U'-')
					set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "DD"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit day, saw '"sv, to_sv(*cp), "'"sv);
				const auto day = digits[1] + digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					set_error_and_return_default(
						"expected day between 1 and "sv, static_cast<uint64_t>(max_days_in_month),
						" (inclusive), saw "sv, static_cast<uint64_t>(day)
					);

				if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

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
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("time"sv);

				static constexpr auto max_digits = 9_sz;
				uint32_t digits[max_digits];

				// "HH"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(*cp), "'"sv);
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					set_error_and_return_default(
						"expected hour between 0 to 59 (inclusive), saw "sv, static_cast<uint64_t>(hour)
					);
				set_error_and_return_if_eof({});

				// ':'
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(*cp), "'"sv);
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					set_error_and_return_default(
						"expected minute between 0 and 59 (inclusive), saw "sv, static_cast<uint64_t>(minute)
					);
				auto time = ::toml::time{
					static_cast<uint8_t>(hour),
					static_cast<uint8_t>(minute),
				};

				// ':'
				if constexpr (TOML_LANG_UNRELEASED) // toml/issues/671 (allow omission of seconds)
				{
					if (is_eof()
						|| is_value_terminator(*cp)
						|| (part_of_datetime && is_match(*cp, U'+', U'-', U'Z')))
						return time;
				}
				else
					set_error_and_return_if_eof({});
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "SS"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit second, saw '"sv, to_sv(*cp), "'"sv);
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					set_error_and_return_default(
						"expected second between 0 and 59 (inclusive), saw "sv, static_cast<uint64_t>(second)
					);
				time.second = static_cast<uint8_t>(second);

				// '.' (early-exiting is allowed; fractional is optional)
				if (is_eof()
					|| is_value_terminator(*cp)
					|| (part_of_datetime && is_match(*cp, U'+', U'-', U'Z')))
					return time;
				if (*cp != U'.')
					set_error_and_return_default("expected '.', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "FFFFFFFFF"
				auto digit_count = consume_variable_length_digit_sequence(digits, max_digits);
				if (!digit_count)
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("expected fractional digits, saw '"sv, to_sv(*cp), "'"sv);
				}
				else if (!is_eof())
				{
					if (digit_count == max_digits && is_decimal_digit(*cp))
						set_error_and_return_default(
							"fractional component exceeds maximum precision of "sv, static_cast<uint64_t>(max_digits)
						);
					else if (!part_of_datetime && !is_value_terminator(*cp))
						set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);
				}

				uint32_t value = 0u;
				uint32_t place = 1u;
				for (auto i = digit_count; i --> 0_sz;)
				{
					value += digits[i] * place;
					place *= 10u;
				}
				for (auto i = digit_count; i < max_digits; i++) //implicit zeros
					value *= 10u;
				time.nanosecond = value;
				return time;
			}

			[[nodiscard]]
			date_time parse_date_time() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("date-time"sv);

				// "YYYY-MM-DD"
				auto date = parse_date(true);
				set_error_and_return_if_eof({});

				// ' ' or 'T'
				if (!is_match(*cp, U' ', U'T'))
					set_error_and_return_default("expected space or 'T', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "HH:MM:SS.FFFFFFFFF"
				auto time = parse_time(true);
				return_if_error({});

				// no offset
				if (is_eof() || is_value_terminator(*cp))
					return { date, time };

				// zero offset ("Z")
				time_offset offset;
				if (*cp == U'Z')
					advance_and_return_if_error({});

				// explicit offset ("+/-HH:MM")
				else if (is_match(*cp, U'+', U'-'))
				{
					push_parse_scope("date-time offset"sv);

					// sign
					int sign = *cp == U'-' ? -1 : 1;
					advance_and_return_if_error_or_eof({});

					// "HH"
					int digits[2];
					if (!consume_digit_sequence(digits, 2_sz))
						set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(*cp), "'"sv);
					const auto hour = digits[1] + digits[0] * 10;
					if (hour > 23)
						set_error_and_return_default(
							"expected hour between 0 and 23 (inclusive), saw "sv, static_cast<int64_t>(hour)
						);
					set_error_and_return_if_eof({});

					// ':'
					if (*cp != U':')
						set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
					advance_and_return_if_error_or_eof({});

					// "MM"
					if (!consume_digit_sequence(digits, 2_sz))
						set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(*cp), "'"sv);
					const auto minute = digits[1] + digits[0] * 10;
					if (minute > 59)
						set_error_and_return_default(
							"expected minute between 0 and 59 (inclusive), saw "sv, static_cast<int64_t>(minute)
						);
					offset.minutes = static_cast<int16_t>((hour * 60 + minute) * sign);
				}

				if (!is_eof() && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

				return { date, time, offset };
			}

			TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
			std::unique_ptr<toml::array> parse_array() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<toml::table> parse_inline_table() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<node> parse_value() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(!is_value_terminator(*cp));
				push_parse_scope("value"sv);

				// check if it begins with some control character
				// (note that this will also fail for whitespace but we're assuming we've
				// called consume_leading_whitespace() before calling parse_value())
				if TOML_UNLIKELY(is_control_character(*cp))
					set_error_and_return_default("unexpected control character"sv);

				// underscores at the beginning
				else if (*cp == U'_')
					set_error_and_return_default("values may not begin with underscores"sv);

				const auto begin_pos = cp->position;
				std::unique_ptr<node> val;

				do
				{
					assert_or_assume(!is_control_character(*cp));
					assert_or_assume(*cp != U'_');

					// detect the value type and parse accordingly,
					// starting with value types that can be detected
					// unambiguously from just one character.

					switch (cp->value)
					{
						// arrays
						case U'[':
						{
							val = parse_array();
							if constexpr (!TOML_LANG_AT_LEAST(1, 0, 0)) // toml/issues/665 (heterogeneous arrays)
							{
								if (!val->ref_cast<array>().is_homogeneous())
									set_error_at(
										begin_pos,
										"arrays cannot contain values of different types before TOML 1.0.0"sv
									);
							}
							break;
						}

						// inline tables
						case U'{':
							val = parse_inline_table();
							break;

						// floats beginning with '.'
						case U'.':
							val = std::make_unique<value<double>>(parse_float());
							break;

						// strings
						case U'"': [[fallthrough]];
						case U'\'':
							val = std::make_unique<value<string>>(std::move(parse_string().value));
							break;

						// bools
						case U't': [[fallthrough]];
						case U'f': [[fallthrough]];
						case U'T': [[fallthrough]];
						case U'F':
							val = std::make_unique<value<bool>>(parse_boolean());
							break;

						// inf/nan
						case U'i': [[fallthrough]];
						case U'I': [[fallthrough]];
						case U'n': [[fallthrough]];
						case U'N':
							val = std::make_unique<value<double>>(parse_inf_or_nan());
							break;
					}

					return_if_error({});
					if (val)
						break;

					// value types from here down require more than one character to unambiguously identify
					// so scan ahead and collect a set of value 'traits'.
					enum value_traits : int
					{
						has_nothing		= 0,
						has_digits		= 1,
						has_b			= 1 << 1, // as second char only (0b)
						has_e			= 1 << 2, // only float exponents
						has_o			= 1 << 3, // as second char only (0o)
						has_p			= 1 << 4, // only hexfloat exponents
						has_t			= 1 << 5,
						has_x			= 1 << 6, // as second or third char only (0x, -0x, +0x)
						has_z			= 1 << 7,
						has_colon		= 1 << 8,
						has_plus		= 1 << 9,
						has_minus		= 1 << 10,
						has_dot			= 1 << 11,
						begins_sign		= 1 << 12,
						begins_digit	= 1 << 13,
						begins_zero		= 1 << 14

						// Q: "why not make these real values in the enum??"
						// A: because the visual studio debugger stops treating them as a set of flags if you add
						// non-pow2 values, making them much harder to debug.
						#define signs_msk (has_plus | has_minus)
						#define bzero_msk (begins_zero | has_digits)
						#define bdigit_msk (begins_digit | has_digits)
					};
					value_traits traits = has_nothing;
					const auto has_any = [&](auto t) noexcept { return (traits & t) != has_nothing; };
					const auto has_none = [&](auto t) noexcept { return (traits & t) == has_nothing; };
					const auto add_trait = [&](auto t) noexcept { traits = static_cast<value_traits>(traits | t); };

					// examine the first character to get the 'begins with' traits
					// (good fail-fast opportunity; all the remaining types begin with numeric digits or signs)
					if (is_decimal_digit(*cp))
						add_trait(*cp == U'0' ? begins_zero : begins_digit);
					else if (is_match(*cp, U'+', U'-'))
						add_trait(begins_sign);
					else
						break;

					// scan the rest of the value to determine the remaining traits
					char32_t chars[utf8_buffered_reader::max_history_length];
					size_t char_count = {}, advance_count = {};
					bool eof_while_scanning = false;
					const auto scan = [&]() TOML_MAY_THROW
					{
						if (is_eof())
							return;
						assert_or_assume(!is_value_terminator(*cp));

						do
						{
							if (const auto c = **cp; c != U'_')
							{
								chars[char_count++] = c;

								if (is_decimal_digit(c))
									add_trait(has_digits);
								else if (is_ascii_letter(c))
								{
									assert_or_assume((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'));
									switch (static_cast<char32_t>(c | 32u))
									{
										case U'b':
											if (char_count == 2_sz && has_any(begins_zero))
												add_trait(has_b);
											break;

										case U'e':
											if (char_count > 1_sz
												&& has_none(has_b | has_o | has_p | has_t | has_x | has_z | has_colon)
												&& (has_none(has_plus | has_minus) || has_any(begins_sign)))
												add_trait(has_e);
											break;

										case U'o':
											if (char_count == 2_sz && has_any(begins_zero))
												add_trait(has_o);
											break;

										case U'p':
											if (has_any(has_x))
												add_trait(has_p);
											break;

										case U'x':
											if ((char_count == 2_sz && has_any(begins_zero))
												|| (char_count == 3_sz && has_any(begins_sign) && chars[1] == U'0'))
												add_trait(has_x);
											break;

										case U't': add_trait(has_t); break;
										case U'z': add_trait(has_z); break;
									}
								}
								else if (c <= U':')
								{
									assert_or_assume(c < U'0' || c > U'9');
									switch (c)
									{
										case U'+': add_trait(has_plus); break;
										case U'-': add_trait(has_minus); break;
										case U'.': add_trait(has_dot); break;
										case U':': add_trait(has_colon); break;
									}
								}
							}

							advance_and_return_if_error();
							advance_count++;
							eof_while_scanning = is_eof();
						}
						while (advance_count < utf8_buffered_reader::max_history_length
							&& !is_eof()
							&& !is_value_terminator(*cp)
						);
					};
					scan();
					return_if_error({});

					// force further scanning if this could have been a date-time with a space instead of a T
					if (char_count == 10_sz
						&& traits == (bdigit_msk | has_minus)
						&& chars[4] == U'-'
						&& chars[7] == U'-'
						&& !is_eof()
						&& *cp == U' ')
					{
						const auto pre_advance_count = advance_count;
						const auto pre_scan_traits = traits;
						chars[char_count++] = *cp;
						add_trait(has_t);

						const auto backpedal = [&]() noexcept
						{
							go_back(advance_count - pre_advance_count);
							advance_count = pre_advance_count;
							traits = pre_scan_traits;
							char_count = 10_sz;
						};

						advance_and_return_if_error({});
						advance_count++;

						if (is_eof() || !is_decimal_digit(*cp))
							backpedal();
						else
						{
							chars[char_count++] = *cp;

							advance_and_return_if_error({});
							advance_count++;

							scan();
							return_if_error({});

							if (char_count == 12_sz)
								backpedal();
						}
					}

					// set the reader back to where we started
					go_back(advance_count);
					if (char_count < utf8_buffered_reader::max_history_length - 1_sz)
						chars[char_count] = U'\0';

					// if after scanning ahead we still only have one value character,
					// the only valid value type is an integer.
					if (char_count == 1_sz)
					{
						if (has_any(begins_zero | begins_digit))
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[0] - U'0'));
							advance(); //skip the digit
							break;
						}

						//anything else would be ambiguous.
						else
							set_error_and_return_default(
								eof_while_scanning
									? "encountered end-of-file"sv
									: "could not determine value type"sv
							);
					}

					// now things that can be identified from two or more characters
					return_if_error({});
					assert_or_assume(char_count >= 2_sz);

					// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
					// typed parse functions to take over and show better diagnostics if there's an issue
					// (as opposed to the fallback "could not determine type" message)
					if (has_any(has_p))
						val = std::make_unique<value<double>>(parse_hex_float());
					else if (has_any(has_x))
						val = std::make_unique<value<int64_t>>(parse_integer<16>());
					else if (has_any(has_o))
						val = std::make_unique<value<int64_t>>(parse_integer<8>());
					else if (has_any(has_b))
						val = std::make_unique<value<int64_t>>(parse_integer<2>());
					else if (has_any(has_e) || (has_any(begins_zero | begins_digit) && chars[1] == U'.'))
						val = std::make_unique<value<double>>(parse_float());
					else if (has_any(begins_sign))
					{
						// single-digit signed integers
						if (char_count == 2_sz && has_any(has_digits))
						{
							val = std::make_unique<value<int64_t>>(
								static_cast<int64_t>(chars[1] - U'0')
								* (chars[0] == U'-' ? -1LL : 1LL)
							);
							advance(); //skip the sign
							advance(); //skip the digit
							break;
						}

						// simple signed floats (e.g. +1.0)
						if (is_decimal_digit(chars[1]) && chars[2] == U'.')
							val = std::make_unique<value<double>>(parse_float());

						// signed infinity or nan
						else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
							val = std::make_unique<value<double>>(parse_inf_or_nan());
					}

					return_if_error({});
					if (val)
						break;

					// match trait masks against what they can match exclusively.
					// all correct value parses will come out of this list, so doing this as a switch is likely to
					// be a better friend to the optimizer on the success path (failure path can be slow but that
					// doesn't matter much).
					switch (unbox_enum(traits))
					{
						//=================== binary integers
						// 0b10
						case bzero_msk | has_b:
							val = std::make_unique<value<int64_t>>(parse_integer<2>());
							break;

						//=================== octal integers
						// 0o10
						case bzero_msk | has_o:
							val = std::make_unique<value<int64_t>>(parse_integer<8>());
							break;

						//=================== decimal integers
						// 00
						// 10
						// +10
						// -10
						case bzero_msk:															[[fallthrough]];
						case bdigit_msk:														[[fallthrough]];
						case begins_sign | has_digits | has_minus:								[[fallthrough]];
						case begins_sign | has_digits | has_plus:
							val = std::make_unique<value<int64_t>>(parse_integer<10>());
							break;

						//=================== hexadecimal integers
						// 0x10
						case bzero_msk | has_x:
							val = std::make_unique<value<int64_t>>(parse_integer<16>());
							break;

						//=================== decimal floats
						// 0e1
						// 0e-1
						// 0e+1
						// 0.0
						// 0.0e1
						// 0.0e-1
						// 0.0e+1
						case bzero_msk | has_e:													[[fallthrough]];
						case bzero_msk | has_e | has_minus:										[[fallthrough]];
						case bzero_msk | has_e | has_plus:										[[fallthrough]];
						case bzero_msk | has_dot:												[[fallthrough]];
						case bzero_msk | has_dot | has_e:										[[fallthrough]];
						case bzero_msk | has_dot | has_e | has_minus:							[[fallthrough]];
						case bzero_msk | has_dot | has_e | has_plus:							[[fallthrough]];
						// 1e1
						// 1e-1
						// 1e+1
						// 1.0
						// 1.0e1
						// 1.0e-1
						// 1.0e+1
						case bdigit_msk | has_e:												[[fallthrough]];
						case bdigit_msk | has_e | has_minus:									[[fallthrough]];
						case bdigit_msk | has_e | has_plus:										[[fallthrough]];
						case bdigit_msk | has_dot:												[[fallthrough]];
						case bdigit_msk | has_dot | has_e:										[[fallthrough]];
						case bdigit_msk | has_dot | has_e | has_minus:							[[fallthrough]];
						case bdigit_msk | has_dot | has_e | has_plus:							[[fallthrough]];
						// +1e1
						// +1.0
						// +1.0e1
						// +1.0e+1
						// +1.0e-1
						// -1.0e+1
						case begins_sign | has_digits | has_e | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | signs_msk:			[[fallthrough]];
						// -1e1
						// -1e+1
						// +1e-1
						// -1.0
						// -1.0e1
						// -1.0e-1
						case begins_sign | has_digits | has_e | has_minus:						[[fallthrough]];
						case begins_sign | has_digits | has_e | signs_msk:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_minus:					[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_minus:
							val = std::make_unique<value<double>>(parse_float());
							break;

						//=================== hexadecimal floats
						// 0x10p0
						// 0x10p-0
						// 0x10p+0
						case bzero_msk | has_x | has_p:											[[fallthrough]];
						case bzero_msk | has_x | has_p | has_minus:								[[fallthrough]];
						case bzero_msk | has_x | has_p | has_plus:								[[fallthrough]];
						// -0x10p0
						// -0x10p-0
						// +0x10p0
						// +0x10p+0
						// -0x10p+0
						// +0x10p-0
						case begins_sign | has_digits | has_x | has_p | has_minus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | signs_msk:				[[fallthrough]];
						// 0x10.1p0
						// 0x10.1p-0
						// 0x10.1p+0
						case bzero_msk | has_x | has_dot | has_p:								[[fallthrough]];
						case bzero_msk | has_x | has_dot | has_p | has_minus:					[[fallthrough]];
						case bzero_msk | has_x | has_dot | has_p | has_plus:					[[fallthrough]];
						// -0x10.1p0
						// -0x10.1p-0
						// +0x10.1p0
						// +0x10.1p+0
						// -0x10.1p+0
						// +0x10.1p-0
						case begins_sign | has_digits | has_x | has_dot | has_p | has_minus:	[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | has_plus:		[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | signs_msk:
							val = std::make_unique<value<double>>(parse_hex_float());
							break;

						//=================== times
						// HH:MM
						// HH:MM:SS
						// HH:MM:SS.FFFFFF
						case bzero_msk | has_colon:												[[fallthrough]];
						case bzero_msk | has_colon | has_dot:									[[fallthrough]];
						case bdigit_msk | has_colon:											[[fallthrough]];
						case bdigit_msk | has_colon | has_dot:
							val = std::make_unique<value<time>>(parse_time());
							break;

						//=================== local dates
						// YYYY-MM-DD
						case bzero_msk | has_minus:												[[fallthrough]];
						case bdigit_msk | has_minus:
							val = std::make_unique<value<date>>(parse_date());
							break;

						//=================== date-times
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
						case bzero_msk | has_minus | has_colon | has_t:							[[fallthrough]];
						case bzero_msk | signs_msk | has_colon | has_t:							[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_t:						[[fallthrough]];
						case bdigit_msk | signs_msk | has_colon | has_t:						[[fallthrough]];
						// YYYY-MM-DDTHH:MM:SS.FFFFFF
						// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF
						// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
						case bzero_msk | has_minus | has_colon | has_dot | has_t:				[[fallthrough]];
						case bzero_msk | signs_msk | has_colon | has_dot | has_t:				[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_dot | has_t:				[[fallthrough]];
						case bdigit_msk | signs_msk | has_colon | has_dot | has_t:				[[fallthrough]];
						// YYYY-MM-DDTHH:MMZ
						// YYYY-MM-DD HH:MMZ
						// YYYY-MM-DDTHH:MM:SSZ
						// YYYY-MM-DD HH:MM:SSZ
						// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
						// YYYY-MM-DD HH:MM:SS.FFFFFFZ
						case bzero_msk | has_minus | has_colon | has_z | has_t:					[[fallthrough]];
						case bzero_msk | has_minus | has_colon | has_dot | has_z | has_t:		[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_z | has_t:				[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
							val = std::make_unique<value<date_time>>(parse_date_time());
							break;
					}

					#undef signs_msk
					#undef bzero_msk
					#undef bdigit_msk
				}
				while (false);

				if (!val)
				{
					set_error_at(begin_pos, "could not determine value type"sv);
					return_after_error({});
				}

				val->source_ = { begin_pos, current_position(1), reader.source_path() };
				return val;
			}

			[[nodiscard]]
			parsed_key parse_key() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_bare_key_character(*cp) || is_string_delimiter(*cp));
				push_parse_scope("key"sv);

				parsed_key key;
				recording_whitespace = false;

				while (!is_error())
				{
					#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
					if (is_unicode_combining_mark(*cp))
						set_error_and_return_default("bare keys may not begin with unicode combining marks"sv);
					else
					#endif

					// bare_key_segment
					if (is_bare_key_character(*cp))
						key.segments.emplace_back(parse_bare_key_segment());

					// "quoted key segment"
					else if (is_string_delimiter(*cp))
					{
						const auto begin_pos = cp->position;

						recording_whitespace = true;
						auto str = parse_string();
						recording_whitespace = false;
						return_if_error({});

						if (str.was_multi_line)
						{
							set_error_at(
								begin_pos,
								"multi-line strings are prohibited in "sv,
								key.segments.empty() ? ""sv : "dotted "sv,
								"keys"sv
							);
							return_after_error({});
						}
						else
							key.segments.emplace_back(std::move(str.value));
					}

					// ???
					else
						set_error_and_return_default(
							"expected bare key starting character or string delimiter, saw '"sv, to_sv(*cp), "'"sv
						);

					// whitespace following the key segment
					consume_leading_whitespace();

					// eof or no more key to come
					if (is_eof() || *cp != U'.')
						break;

					// was a dotted key, so go around again to consume the next segment
					advance_and_return_if_error_or_eof({});
					consume_leading_whitespace();
					set_error_and_return_if_eof({});
				}
				return_if_error({});
				return key;
			}

			[[nodiscard]]
			parsed_key_value_pair parse_key_value_pair() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_string_delimiter(*cp) || is_bare_key_character(*cp));
				push_parse_scope("key-value pair"sv);

				// get the key
				start_recording();
				auto key = parse_key();
				stop_recording(1_sz);

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				set_error_and_return_if_eof({});

				// '='
				if (*cp != U'=')
					set_error_and_return_default("expected '=', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// skip past any whitespace that followed the '='
				consume_leading_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});

				// get the value
				if (is_value_terminator(*cp))
					set_error_and_return_default("expected value, saw '"sv, to_sv(*cp), "'"sv);
				return { std::move(key), parse_value() };
			}

			[[nodiscard]]
			table* parse_table_header() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'[');
				push_parse_scope("table header"sv);

				const auto header_begin_pos = cp->position;
				source_position header_end_pos;
				parsed_key key;
				bool is_arr = false;

				//parse header
				{
					// skip first '['
					advance_and_return_if_error_or_eof({});

					// skip past any whitespace that followed the '['
					const bool had_leading_whitespace = consume_leading_whitespace();
					set_error_and_return_if_eof({});

					// skip second '[' (if present)
					if (*cp == U'[')
					{
						if (had_leading_whitespace)
							set_error_and_return_default(
								"[[array-of-table]] brackets must be contiguous (i.e. [ [ this ] ] is prohibited)"sv
							);

						is_arr = true;
						advance_and_return_if_error_or_eof({});

						// skip past any whitespace that followed the '['
						consume_leading_whitespace();
						set_error_and_return_if_eof({});
					}

					// check for a premature closing ']'
					if (*cp == U']')
						set_error_and_return_default("tables with blank bare keys are explicitly prohibited"sv);

					// get the actual key
					start_recording();
					key = parse_key();
					stop_recording(1_sz);
					return_if_error({});

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					return_if_error({});
					set_error_and_return_if_eof({});

					// consume the closing ']'
					if (*cp != U']')
						set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
					if (is_arr)
					{
						advance_and_return_if_error_or_eof({});
						if (*cp != U']')
							set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
					}
					advance_and_return_if_error({});
					header_end_pos = current_position(1);

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (!is_eof() && !consume_comment() && !consume_line_break())
						set_error_and_return_default("expected a comment or whitespace, saw '"sv, to_sv(*cp), "'"sv);
				}
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
							set_error_and_return_default(
								"cannot redefine existing table '"sv, to_sv(recording_buffer), "'"sv
							);
						else
							set_error_and_return_default(
								"cannot redefine existing "sv, to_sv(child->type()),
								" '"sv, to_sv(recording_buffer),
								"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
							);
					}
				}

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
						set_error_and_return_default("cannot redefine existing table '"sv, to_sv(recording_buffer), "'"sv);
					else
						set_error_and_return_default(
							"cannot redefine existing "sv, to_sv(matching_node->type()),
							" '"sv, to_sv(recording_buffer),
							"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
						);
				}
			}

			void parse_key_value_pair_and_insert(table* tab) TOML_MAY_THROW
			{
				return_if_error();
				assert_not_eof();
				push_parse_scope("key-value pair"sv);

				auto kvp = parse_key_value_pair();
				return_if_error();

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
							set_error("cannot redefine existing "sv, to_sv(child->type()), " as dotted key-value pair"sv);
						else
							child->source_.end = kvp.value->source_.end;
						return_if_error();
						tab = &child->ref_cast<table>();
					}
				}

				if (auto conflicting_node = tab->get(kvp.key.segments.back()))
				{
					if (conflicting_node->type() == kvp.value->type())
						set_error(
							"cannot redefine existing "sv, to_sv(conflicting_node->type()),
							" '"sv, to_sv(recording_buffer), "'"sv
						);
					else
						set_error(
							"cannot redefine existing "sv, to_sv(conflicting_node->type()),
							" '"sv, to_sv(recording_buffer),
							"' as "sv, to_sv(kvp.value->type())
						);
				}

				return_if_error();
				tab->values.emplace(
					std::move(kvp.key.segments.back()),
					std::move(kvp.value)
				);
			}

			void parse_document() TOML_MAY_THROW
			{
				assert_not_error();
				assert_not_eof();
				push_parse_scope("root table"sv);
				table* current_table = &root;

				do
				{
					return_if_error();

					// leading whitespace, line endings, comments
					if (consume_leading_whitespace()
						|| consume_line_break()
						|| consume_comment())
						continue;

					// [tables]
					// [[table array]]
					else if (*cp == U'[')
						current_table = parse_table_header();

					// bare_keys
					// dotted.keys
					// "quoted keys"
					else if (is_bare_key_character(*cp) || is_string_delimiter(*cp))
					{
						push_parse_scope("key-value pair"sv);

						parse_key_value_pair_and_insert(current_table);

						// handle the rest of the line after the kvp
						// (this is not done in parse_key_value_pair() because that is also used for inline tables)
						consume_leading_whitespace();
						return_if_error();
						if (!is_eof() && !consume_comment() && !consume_line_break())
							set_error("expected a comment or whitespace, saw '"sv, to_sv(*cp), "'"sv);
					}

					else // ??
						set_error("expected keys, tables, whitespace or comments, saw '"sv, to_sv(*cp), "'"sv);

				}
				while (!is_eof());

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
						(void)k;
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

				if (!reader.peek_eof())
				{
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
				}

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

	TOML_EXTERNAL_LINKAGE
	std::unique_ptr<toml::array> parser::parse_array() TOML_MAY_THROW
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_eof({});

		auto arr = std::make_unique<array>();
		auto& vals = arr->values;
		enum parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (!is_error())
		{
			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
				continue;
			set_error_and_return_if_eof({});

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == val)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				set_error_and_return_default("expected value or closing ']', saw comma"sv);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance_and_return_if_error({});
				break;
			}

			// must be a value
			else
			{
				if (prev == val)
				{
					set_error_and_return_default("expected comma or closing ']', saw '"sv, to_sv(*cp), "'"sv);
					continue;
				}
				prev = val;
				vals.push_back(parse_value());
			}
		}

		return_if_error({});
		return arr;
	}

	TOML_EXTERNAL_LINKAGE
	std::unique_ptr<toml::table> parser::parse_inline_table() TOML_MAY_THROW
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_eof({});

		auto tab = std::make_unique<table>();
		tab->inline_ = true;
		enum parse_elem : int
		{
			none,
			comma,
			kvp
		};
		parse_elem prev = none;

		while (!is_error())
		{
			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
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
			set_error_and_return_if_eof({});

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == kvp)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
				}
				else
					set_error_and_return_default("expected key-value pair or closing '}', saw comma"sv);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				if constexpr (!TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
				{
					if (prev == comma)
					{
						set_error_and_return_default("expected key-value pair, saw closing '}' (dangling comma)"sv);
						continue;
					}
				}
				advance_and_return_if_error({});
				break;
			}

			// key-value pair
			else if (is_string_delimiter(*cp) || is_bare_key_character(*cp))
			{
				if (prev == kvp)
					set_error_and_return_default("expected comma or closing '}', saw '"sv, to_sv(*cp), "'"sv);
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(tab.get());
				}
			}

			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, to_sv(*cp), "'"sv);
		}

		return_if_error({});
		return tab;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result do_parse(utf8_reader_interface&& reader) TOML_MAY_THROW
	{
		return impl::parser{ std::move(reader) };
	}

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

}

#undef push_parse_scope_2
#undef push_parse_scope_1
#undef push_parse_scope
#undef TOML_RETURNS_BY_THROWING
#undef is_eof
#undef assert_not_eof
#undef return_if_eof
#undef is_error
#undef return_after_error
#undef assert_not_error
#undef return_if_error
#undef return_if_error_or_eof
#undef set_error_and_return
#undef set_error_and_return_default
#undef set_error_and_return_if_eof
#undef advance_and_return_if_error
#undef advance_and_return_if_error_or_eof

namespace toml
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(parse_ex)
	#else
		TOML_ABI_NAMESPACE_START(parse_noex)
	#endif

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#ifdef __cpp_lib_char8_t

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#endif // __cpp_lib_char8_t

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	inline namespace literals
	{
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(lit_ex)
		#else
			TOML_ABI_NAMESPACE_START(lit_noex)
		#endif

		TOML_API
		TOML_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::string_view{ str, len });
		}

		#ifdef __cpp_lib_char8_t

		TOML_API
		TOML_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::u8string_view{ str, len });
		}

		#endif // __cpp_lib_char8_t

		TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

#endif
//------------------------------------------------------------------  ↑ toml_parser.hpp  -------------------------------

#endif // TOML_PARSER

#if !TOML_ALL_INLINE

//---------------------------------------------------------------------------------------  ↓ toml_instantiations.hpp  --
#if 1

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <ostream>
#include <istream>
#include <fstream>
TOML_POP_WARNINGS

namespace toml
{
	// value<>
	template class TOML_API value<string>;
	template class TOML_API value<int64_t>;
	template class TOML_API value<double>;
	template class TOML_API value<bool>;
	template class TOML_API value<date>;
	template class TOML_API value<time>;
	template class TOML_API value<date_time>;

	// node_view
	template class TOML_API node_view<node>;
	template class TOML_API node_view<const node>;

	// formatters
	namespace impl
	{
		template class TOML_API formatter<char>;
	}
	template class TOML_API default_formatter<char>;
	template class TOML_API json_formatter<char>;

	// various ostream operators
	template TOML_API std::ostream& operator << (std::ostream&, const source_position&);
	template TOML_API std::ostream& operator << (std::ostream&, const source_region&);
	template TOML_API std::ostream& operator << (std::ostream&, const date&);
	template TOML_API std::ostream& operator << (std::ostream&, const time&);
	template TOML_API std::ostream& operator << (std::ostream&, const time_offset&);
	template TOML_API std::ostream& operator << (std::ostream&, const date_time&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::string>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<int64_t>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<double>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<bool>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::time>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date_time>&);
	template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&);
	template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&&);
	template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&);
	template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&&);
	template TOML_API std::ostream& operator << (std::ostream&, const table&);
	template TOML_API std::ostream& operator << (std::ostream&, const array&);
	template TOML_API std::ostream& operator << (std::ostream&, const node_view<node>&);
	template TOML_API std::ostream& operator << (std::ostream&, const node_view<const node>&);
	template TOML_API std::ostream& operator << (std::ostream&, node_type);

	// print_to_stream() machinery
	namespace impl
	{
		template TOML_API void print_floating_point_to_stream(float, std::ostream&, bool);
		template TOML_API void print_floating_point_to_stream(double, std::ostream&, bool);
	}

	// parser machinery
	#if TOML_PARSER

		// parse error ostream
		template TOML_API std::ostream& operator << (std::ostream&, const parse_error&);

		// parse() and parse_file()
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(parse_ex)
		#else
			TOML_ABI_NAMESPACE_START(parse_noex)
		#endif
		template TOML_API parse_result parse(std::istream&, std::string_view) TOML_MAY_THROW;
		template TOML_API parse_result parse(std::istream&, std::string&&) TOML_MAY_THROW;
		template TOML_API parse_result parse_file(std::string_view) TOML_MAY_THROW;
		#ifdef __cpp_lib_char8_t
			template TOML_API parse_result parse_file(std::u8string_view) TOML_MAY_THROW;
		#endif
		TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	#endif // TOML_PARSER
}

#endif
//---------------------------------------------------------------------------------------  ↑ toml_instantiations.hpp  --

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
	#undef TOML_NEVER_INLINE
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


#endif // INCLUDE_TOMLPLUSPLUS_H
// clang-format on
