//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
// clang-format off

////////// CONFIGURATION

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
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

////////// COMPILER & ENVIRONMENT

#ifndef TOML_DOXYGEN
	#define TOML_DOXYGEN 0
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
	#define TOML_LIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 1) )
	#define TOML_UNLIKELY(...)				(__builtin_expect(!!(__VA_ARGS__), 0) )

	//floating-point from_chars and to_chars are not implemented in any version of clang as of 1/1/2020
	#ifndef TOML_FLOAT_CHARCONV
		#define TOML_FLOAT_CHARCONV 0
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
	#error toml++ requires C++17 or higher. For a TOML library supporting C++11 see https://github.com/skystrife/cpptoml
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

#include "toml_version.h"
//# {{
#define	TOML_LIB_SINGLE_HEADER 0
//# }}

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
	#ifdef NDEBUG
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

//# {{
#if TOML_DOXYGEN

/// \addtogroup		configuration		Library Configuration
/// \brief Preprocessor macros for configuring library functionality.
/// \detail Define these before including `toml++` to alter the way it functions. 
/// \remarks Some of these options have ABI implications; inline namespaces are used to prevent
/// 		 you from trying to link incompatible combinations together.
/// @{


/// \def TOML_ALL_INLINE
/// \brief Sets whether the library is entirely inline.
/// \detail Defaults to `1`.
/// \remark Disabling this means that you must define `TOML_IMPLEMENTATION` in
///	<strong><em>exactly one</em></strong> translation unit in your project:
/// \cpp
/// // global_header_that_includes_toml++.h
/// #define TOML_ALL_INLINE 0
/// #include <toml.hpp>
/// 
/// // some_code_file.cpp
/// #define TOML_IMPLEMENTATION 
/// #include "global_header_that_includes_toml++.h"
/// \ecpp


#define TOML_API
/// \def TOML_API
/// \brief An annotation to add to public symbols.
/// \detail Not defined by default.
///	\remark You'd override this with `__declspec(dllexport)` if you were building the library
/// 		into the public API of a DLL on Windows.


/// \def TOML_ASSERT(expr)
/// \brief Sets the assert function used by the library.
/// \detail Defaults to the standard C `assert()`.


/// \def TOML_CHAR_8_STRINGS
/// \brief Uses C++20 char8_t-based strings as the toml string data type.
/// \detail Defaults to `0`.


#define TOML_CONFIG_HEADER
/// \def TOML_CONFIG_HEADER
/// \brief An additional header to include before any other `toml++` header files.
/// \detail Not defined by default.


/// \def TOML_EXCEPTIONS
/// \brief Sets whether the library uses exceptions to report parsing failures.
/// \detail Defaults to `1` or `0` according to your compiler's exception mode.


/// \def TOML_IMPLEMENTATION
/// \brief Enables the library's implementation when #TOML_ALL_INLINE is enabled.
/// \detail Not defined by default. Meaningless when #TOML_ALL_INLINE is disabled.


/// \def TOML_LARGE_FILES
/// \brief Sets whether line and column indices are 32-bit integers.
/// \detail Defaults to `0`.
/// \see toml::source_index


#define TOML_OPTIONAL_TYPE
/// \def TOML_OPTIONAL_TYPE
/// \brief Overrides the `optional<T>` type used by the library.
/// \detail Not defined by default (use std::optional).
/// \warning The library uses optionals internally in a few places; if you choose to replace the optional type
/// 		 it must be with something that is still API-compatible with std::optional
/// 		 (e.g. [tl::optional](https://github.com/TartanLlama/optional)).


/// \def TOML_PARSER
/// \brief Sets whether the parser-related parts of the library are included.
/// \detail Defaults to `1`.
/// \remarks If you don't need to parse TOML data from any strings or files (e.g. you're only using the library to
/// 		 serialize data as TOML), setting `TOML_PARSER` to `0` can yield decent compilation speed improvements.


#define TOML_SMALL_FLOAT_TYPE
/// \def TOML_SMALL_FLOAT_TYPE
/// \brief If your codebase has an additional 'small' float type (e.g. half-precision), this tells toml++ about it.
/// \detail Not defined by default.


#define TOML_SMALL_INT_TYPE
/// \def TOML_SMALL_INT_TYPE
/// \brief If your codebase has an additional 'small' integer type (e.g. 24-bits), this tells toml++ about it.
/// \detail Not defined by default.


/// \def TOML_UNRELEASED_FEATURES
/// \brief Enables support for unreleased TOML language features not yet part of a
///		[numbered version](https://github.com/toml-lang/toml/releases).
/// \detail Defaults to `0`.
/// \see [TOML Language Support](https://github.com/marzer/tomlplusplus/blob/master/README.md#toml-language-support)


/// @}
#endif // TOML_DOXYGEN
//# }}

// clang-format on
