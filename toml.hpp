//----------------------------------------------------------------------------------------------------------------------
//
// toml++ v3.4.0
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
// TOML Language Specifications:
// latest:      https://github.com/toml-lang/toml/blob/master/README.md
// v1.1.0:      https://toml.io/en/v1.1.0
// v1.0.0:      https://toml.io/en/v1.0.0
// v0.5.0:      https://toml.io/en/v0.5.0
// changelog:   https://github.com/toml-lang/toml/blob/master/CHANGELOG.md
//
//----------------------------------------------------------------------------------------------------------------------
//
// MIT License
//
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
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
#ifndef TOMLPLUSPLUS_HPP
#define TOMLPLUSPLUS_HPP

#define INCLUDE_TOMLPLUSPLUS_H // old guard name used pre-v3
#define TOMLPLUSPLUS_H		   // guard name used in the legacy toml.h

//********  impl/preprocessor.hpp  *************************************************************************************

#ifndef __cplusplus
#error toml++ is a C++ library.
#endif

#ifndef TOML_CPP
#ifdef _MSVC_LANG
#if _MSVC_LANG > __cplusplus
#define TOML_CPP _MSVC_LANG
#endif
#endif
#ifndef TOML_CPP
#define TOML_CPP __cplusplus
#endif
#if TOML_CPP >= 202900L
#undef TOML_CPP
#define TOML_CPP 29
#elif TOML_CPP >= 202600L
#undef TOML_CPP
#define TOML_CPP 26
#elif TOML_CPP >= 202302L
#undef TOML_CPP
#define TOML_CPP 23
#elif TOML_CPP >= 202002L
#undef TOML_CPP
#define TOML_CPP 20
#elif TOML_CPP >= 201703L
#undef TOML_CPP
#define TOML_CPP 17
#elif TOML_CPP >= 201402L
#undef TOML_CPP
#define TOML_CPP 14
#elif TOML_CPP >= 201103L
#undef TOML_CPP
#define TOML_CPP 11
#else
#undef TOML_CPP
#define TOML_CPP 0
#endif
#endif

#if !TOML_CPP
#error toml++ requires C++17 or higher. For a pre-C++11 TOML library see https://github.com/ToruNiina/Boost.toml
#elif TOML_CPP < 17
#error toml++ requires C++17 or higher. For a C++11 TOML library see https://github.com/ToruNiina/toml11
#endif

#ifndef TOML_MAKE_VERSION
#define TOML_MAKE_VERSION(major, minor, patch) (((major) * 1000000) + ((minor) * 1000) + ((patch)))
#endif

#ifndef TOML_INTELLISENSE
#ifdef __INTELLISENSE__
#define TOML_INTELLISENSE 1
#else
#define TOML_INTELLISENSE 0
#endif
#endif

#ifndef TOML_DOXYGEN
#if defined(DOXYGEN) || defined(__DOXYGEN) || defined(__DOXYGEN__) || defined(__doxygen__) || defined(__POXY__)        \
	|| defined(__poxy__)
#define TOML_DOXYGEN 1
#else
#define TOML_DOXYGEN 0
#endif
#endif

#ifndef TOML_CLANG
#ifdef __clang__
#define TOML_CLANG __clang_major__
#else
#define TOML_CLANG 0
#endif

// special handling for apple clang; see:
// - https://github.com/marzer/tomlplusplus/issues/189
// - https://en.wikipedia.org/wiki/Xcode
// -
// https://stackoverflow.com/questions/19387043/how-can-i-reliably-detect-the-version-of-clang-at-preprocessing-time
#if TOML_CLANG && defined(__apple_build_version__)
#undef TOML_CLANG
#define TOML_CLANG_VERSION TOML_MAKE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#if TOML_CLANG_VERSION <= TOML_MAKE_VERSION(10, 0, 0)
#define TOML_CLANG 6 // not strictly correct but doesn't matter below this
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(10, 0, 1)
#define TOML_CLANG 7
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(11, 0, 0)
#define TOML_CLANG 8
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(11, 0, 3)
#define TOML_CLANG 9
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(12, 0, 0)
#define TOML_CLANG 10
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(12, 0, 5)
#define TOML_CLANG 11
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(13, 0, 0)
#define TOML_CLANG 12
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(13, 1, 6)
#define TOML_CLANG 13
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(14, 0, 0)
#define TOML_CLANG 14
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(14, 0, 3)
#define TOML_CLANG 15
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(15, 0, 0)
#define TOML_CLANG 16
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(16, 0, 0)
#define TOML_CLANG 17
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(17, 0, 0)
#define TOML_CLANG 19
#elif TOML_CLANG_VERSION <= TOML_MAKE_VERSION(21, 0, 0)
#define TOML_CLANG 21
#else
#define TOML_CLANG __clang_major__
#endif
#undef TOML_CLANG_VERSION
#endif
#endif

#ifndef TOML_ICC
#ifdef __INTEL_COMPILER
#define TOML_ICC __INTEL_COMPILER
#ifdef __ICL
#define TOML_ICC_CL TOML_ICC
#else
#define TOML_ICC_CL 0
#endif
#else
#define TOML_ICC	0
#define TOML_ICC_CL 0
#endif
#endif

#ifndef TOML_MSVC_LIKE
#ifdef _MSC_VER
#define TOML_MSVC_LIKE _MSC_VER
#else
#define TOML_MSVC_LIKE 0
#endif
#endif

#ifndef TOML_MSVC
#if TOML_MSVC_LIKE && !TOML_CLANG && !TOML_ICC
#define TOML_MSVC TOML_MSVC_LIKE
#else
#define TOML_MSVC 0
#endif
#endif

#ifndef TOML_GCC_LIKE
#ifdef __GNUC__
#define TOML_GCC_LIKE __GNUC__
#else
#define TOML_GCC_LIKE 0
#endif
#endif

#ifndef TOML_GCC
#if TOML_GCC_LIKE && !TOML_CLANG && !TOML_ICC
#define TOML_GCC TOML_GCC_LIKE
#else
#define TOML_GCC 0
#endif
#endif

#ifndef TOML_CUDA
#if defined(__CUDACC__) || defined(__CUDA_ARCH__) || defined(__CUDA_LIBDEVICE__)
#define TOML_CUDA 1
#else
#define TOML_CUDA 0
#endif
#endif

#ifndef TOML_NVCC
#ifdef __NVCOMPILER_MAJOR__
#define TOML_NVCC __NVCOMPILER_MAJOR__
#else
#define TOML_NVCC 0
#endif
#endif

#ifndef TOML_ARCH_ITANIUM
#if defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
#define TOML_ARCH_ITANIUM 1
#define TOML_ARCH_BITNESS 64
#else
#define TOML_ARCH_ITANIUM 0
#endif
#endif

#ifndef TOML_ARCH_AMD64
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
#define TOML_ARCH_AMD64	  1
#define TOML_ARCH_BITNESS 64
#else
#define TOML_ARCH_AMD64 0
#endif
#endif

#ifndef TOML_ARCH_X86
#if defined(__i386__) || defined(_M_IX86)
#define TOML_ARCH_X86	  1
#define TOML_ARCH_BITNESS 32
#else
#define TOML_ARCH_X86 0
#endif
#endif

#ifndef TOML_ARCH_ARM
#if defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64) || defined(_M_ARM64) || defined(__ARM_64BIT_STATE)             \
	|| defined(_M_ARM64EC)
#define TOML_ARCH_ARM32	  0
#define TOML_ARCH_ARM64	  1
#define TOML_ARCH_ARM	  1
#define TOML_ARCH_BITNESS 64
#elif defined(__arm__) || defined(_M_ARM) || defined(__ARM_32BIT_STATE)
#define TOML_ARCH_ARM32	  1
#define TOML_ARCH_ARM64	  0
#define TOML_ARCH_ARM	  1
#define TOML_ARCH_BITNESS 32
#else
#define TOML_ARCH_ARM32 0
#define TOML_ARCH_ARM64 0
#define TOML_ARCH_ARM	0
#endif
#endif

#ifndef TOML_ARCH_BITNESS
#define TOML_ARCH_BITNESS 0
#endif

#ifndef TOML_ARCH_X64
#if TOML_ARCH_BITNESS == 64
#define TOML_ARCH_X64 1
#else
#define TOML_ARCH_X64 0
#endif
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__CYGWIN__)
#define TOML_WINDOWS 1
#else
#define TOML_WINDOWS 0
#endif

#ifdef __unix__
#define TOML_UNIX 1
#else
#define TOML_UNIX 0
#endif

#ifdef __linux__
#define TOML_LINUX 1
#else
#define TOML_LINUX 0
#endif

// TOML_HAS_INCLUDE
#ifndef TOML_HAS_INCLUDE
#ifdef __has_include
#define TOML_HAS_INCLUDE(header)		__has_include(header)
#else
#define TOML_HAS_INCLUDE(header) 0
#endif
#endif

// TOML_HAS_BUILTIN
#ifndef TOML_HAS_BUILTIN
#ifdef __has_builtin
#define TOML_HAS_BUILTIN(name) __has_builtin(name)
#else
#define TOML_HAS_BUILTIN(name) 0
#endif
#endif

// TOML_HAS_FEATURE
#ifndef TOML_HAS_FEATURE
#ifdef __has_feature
#define TOML_HAS_FEATURE(name) __has_feature(name)
#else
#define TOML_HAS_FEATURE(name) 0
#endif
#endif

// TOML_HAS_ATTR
#ifndef TOML_HAS_ATTR
#ifdef __has_attribute
#define TOML_HAS_ATTR(attr) __has_attribute(attr)
#else
#define TOML_HAS_ATTR(attr) 0
#endif
#endif

// TOML_HAS_CPP_ATTR
#ifndef TOML_HAS_CPP_ATTR
#ifdef __has_cpp_attribute
#define TOML_HAS_CPP_ATTR(attr) __has_cpp_attribute(attr)
#else
#define TOML_HAS_CPP_ATTR(attr) 0
#endif
#endif

// TOML_ATTR (gnu attributes)
#ifndef TOML_ATTR
#if TOML_CLANG || TOML_GCC_LIKE
#define TOML_ATTR(...) __attribute__((__VA_ARGS__))
#else
#define TOML_ATTR(...)
#endif
#endif

// TOML_DECLSPEC (msvc attributes)
#ifndef TOML_DECLSPEC
#if TOML_MSVC_LIKE
#define TOML_DECLSPEC(...) __declspec(__VA_ARGS__)
#else
#define TOML_DECLSPEC(...)
#endif
#endif

// TOML_COMPILER_HAS_EXCEPTIONS
#ifndef TOML_COMPILER_HAS_EXCEPTIONS
#if defined(__EXCEPTIONS) || defined(_CPPUNWIND) || defined(__cpp_exceptions)
#define TOML_COMPILER_HAS_EXCEPTIONS 1
#else
#define TOML_COMPILER_HAS_EXCEPTIONS 0
#endif
#endif

// TOML_COMPILER_HAS_RTTI
#ifndef TOML_COMPILER_HAS_RTTI
#if defined(_CPPRTTI) || defined(__GXX_RTTI) || TOML_HAS_FEATURE(cxx_rtti)
#define TOML_COMPILER_HAS_RTTI 1
#else
#define TOML_COMPILER_HAS_RTTI 0
#endif
#endif

// TOML_CONCAT
#define TOML_CONCAT_1(x, y) x##y
#define TOML_CONCAT(x, y)	TOML_CONCAT_1(x, y)

// TOML_MAKE_STRING
#define TOML_MAKE_STRING_1(s) #s
#define TOML_MAKE_STRING(s)	  TOML_MAKE_STRING_1(s)

// TOML_PRAGMA_XXXX (compiler-specific pragmas)
#if TOML_CLANG
#define TOML_PRAGMA_CLANG(decl) _Pragma(TOML_MAKE_STRING(clang decl))
#else
#define TOML_PRAGMA_CLANG(decl)
#endif
#if TOML_GCC
#define TOML_PRAGMA_GCC(decl) _Pragma(TOML_MAKE_STRING(GCC decl))
#else
#define TOML_PRAGMA_GCC(decl)
#endif
#if TOML_MSVC
#define TOML_PRAGMA_MSVC(...) __pragma(__VA_ARGS__)
#else
#define TOML_PRAGMA_MSVC(...)
#endif

// TOML_ALWAYS_INLINE
#ifndef TOML_ALWAYS_INLINE
#if TOML_MSVC_LIKE
#define TOML_ALWAYS_INLINE __forceinline
#elif TOML_GCC || TOML_CLANG || TOML_HAS_ATTR(__always_inline__)
#define TOML_ALWAYS_INLINE                                                                                             \
	TOML_ATTR(__always_inline__)                                                                                       \
	inline
#else
#define TOML_ALWAYS_INLINE inline
#endif
#endif

// TOML_NEVER_INLINE
#ifndef TOML_NEVER_INLINE
#if TOML_MSVC_LIKE
#define TOML_NEVER_INLINE TOML_DECLSPEC(noinline)
#elif TOML_CUDA // https://gitlab.gnome.org/GNOME/glib/-/issues/2555
#define TOML_NEVER_INLINE TOML_ATTR(noinline)
#else
#if TOML_GCC || TOML_CLANG || TOML_HAS_ATTR(__noinline__)
#define TOML_NEVER_INLINE TOML_ATTR(__noinline__)
#endif
#endif
#ifndef TOML_NEVER_INLINE
#define TOML_NEVER_INLINE
#endif
#endif

// MSVC attributes
#ifndef TOML_ABSTRACT_INTERFACE
#define TOML_ABSTRACT_INTERFACE TOML_DECLSPEC(novtable)
#endif
#ifndef TOML_EMPTY_BASES
#define TOML_EMPTY_BASES TOML_DECLSPEC(empty_bases)
#endif

// TOML_TRIVIAL_ABI
#ifndef TOML_TRIVIAL_ABI
#if TOML_CLANG || TOML_HAS_ATTR(__trivial_abi__)
#define TOML_TRIVIAL_ABI TOML_ATTR(__trivial_abi__)
#else
#define TOML_TRIVIAL_ABI
#endif
#endif

// TOML_NODISCARD
#ifndef TOML_NODISCARD
#if TOML_HAS_CPP_ATTR(nodiscard) >= 201603
#define TOML_NODISCARD [[nodiscard]]
#elif TOML_CLANG || TOML_GCC || TOML_HAS_ATTR(__warn_unused_result__)
#define TOML_NODISCARD TOML_ATTR(__warn_unused_result__)
#else
#define TOML_NODISCARD
#endif
#endif

// TOML_NODISCARD_CTOR
#ifndef TOML_NODISCARD_CTOR
#if TOML_HAS_CPP_ATTR(nodiscard) >= 201907
#define TOML_NODISCARD_CTOR [[nodiscard]]
#else
#define TOML_NODISCARD_CTOR
#endif
#endif

// pure + const
#ifndef TOML_PURE
#ifdef NDEBUG
#define TOML_PURE TOML_ATTR(pure)
#else
#define TOML_PURE
#endif
#endif
#ifndef TOML_CONST
#ifdef NDEBUG
#define TOML_CONST TOML_ATTR(const)
#else
#define TOML_CONST
#endif
#endif
#ifndef TOML_INLINE_GETTER
#define TOML_INLINE_GETTER                                                                                             \
	TOML_NODISCARD                                                                                                     \
	TOML_ALWAYS_INLINE
#endif
#ifndef TOML_PURE_GETTER
#define TOML_PURE_GETTER                                                                                               \
	TOML_NODISCARD                                                                                                     \
	TOML_PURE
#endif
#ifndef TOML_PURE_INLINE_GETTER
#define TOML_PURE_INLINE_GETTER                                                                                        \
	TOML_NODISCARD                                                                                                     \
	TOML_ALWAYS_INLINE                                                                                                 \
	TOML_PURE
#endif
#ifndef TOML_CONST_GETTER
#define TOML_CONST_GETTER                                                                                              \
	TOML_NODISCARD                                                                                                     \
	TOML_CONST
#endif
#ifndef TOML_CONST_INLINE_GETTER
#define TOML_CONST_INLINE_GETTER                                                                                       \
	TOML_NODISCARD                                                                                                     \
	TOML_ALWAYS_INLINE                                                                                                 \
	TOML_CONST
#endif

// TOML_ASSUME
#ifndef TOML_ASSUME
#if TOML_MSVC_LIKE
#define TOML_ASSUME(expr) __assume(expr)
#elif TOML_ICC || TOML_CLANG || TOML_HAS_BUILTIN(__builtin_assume)
#define TOML_ASSUME(expr) __builtin_assume(expr)
#elif TOML_HAS_ATTR(__assume__)
#define TOML_ASSUME(expr) __attribute__((__assume__(expr)))
#elif TOML_HAS_CPP_ATTR(assume) >= 202207
#define TOML_ASSUME(expr) [[assume(expr)]]
#else
#define TOML_ASSUME(expr) static_cast<void>(0)
#endif
#endif

// TOML_UNREACHABLE
#ifndef TOML_UNREACHABLE
#if TOML_MSVC_LIKE
#define TOML_UNREACHABLE __assume(0)
#elif TOML_ICC || TOML_CLANG || TOML_GCC || TOML_HAS_BUILTIN(__builtin_unreachable)
#define TOML_UNREACHABLE __builtin_unreachable()
#else
#define TOML_UNREACHABLE static_cast<void>(0)
#endif
#endif

// TOML_LIKELY
#if TOML_CPP >= 20 && TOML_HAS_CPP_ATTR(likely) >= 201803
#define TOML_LIKELY(...) (__VA_ARGS__) [[likely]]
#define TOML_LIKELY_CASE [[likely]]
#elif TOML_GCC || TOML_CLANG || TOML_HAS_BUILTIN(__builtin_expect)
#define TOML_LIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 1))
#else
#define TOML_LIKELY(...) (__VA_ARGS__)
#endif
#ifndef TOML_LIKELY_CASE
#define TOML_LIKELY_CASE
#endif

// TOML_UNLIKELY
#if TOML_CPP >= 20 && TOML_HAS_CPP_ATTR(unlikely) >= 201803
#define TOML_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
#define TOML_UNLIKELY_CASE [[unlikely]]
#elif TOML_GCC || TOML_CLANG || TOML_HAS_BUILTIN(__builtin_expect)
#define TOML_UNLIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 0))
#else
#define TOML_UNLIKELY(...) (__VA_ARGS__)
#endif
#ifndef TOML_UNLIKELY_CASE
#define TOML_UNLIKELY_CASE
#endif

// TOML_FLAGS_ENUM
#if TOML_CLANG || TOML_HAS_ATTR(flag_enum)
#define TOML_FLAGS_ENUM __attribute__((flag_enum))
#else
#define TOML_FLAGS_ENUM
#endif

// TOML_OPEN_ENUM + TOML_CLOSED_ENUM
#if TOML_CLANG || TOML_HAS_ATTR(enum_extensibility)
#define TOML_OPEN_ENUM	 __attribute__((enum_extensibility(open)))
#define TOML_CLOSED_ENUM __attribute__((enum_extensibility(closed)))
#else
#define TOML_OPEN_ENUM
#define TOML_CLOSED_ENUM
#endif

// TOML_OPEN_FLAGS_ENUM + TOML_CLOSED_FLAGS_ENUM
#define TOML_OPEN_FLAGS_ENUM   TOML_OPEN_ENUM TOML_FLAGS_ENUM
#define TOML_CLOSED_FLAGS_ENUM TOML_CLOSED_ENUM TOML_FLAGS_ENUM

// TOML_MAKE_FLAGS
#define TOML_MAKE_FLAGS_2(T, op, linkage)                                                                              \
	TOML_CONST_INLINE_GETTER                                                                                           \
	linkage constexpr T operator op(T lhs, T rhs) noexcept                                                             \
	{                                                                                                                  \
		using under = std::underlying_type_t<T>;                                                                       \
		return static_cast<T>(static_cast<under>(lhs) op static_cast<under>(rhs));                                     \
	}                                                                                                                  \
                                                                                                                       \
	linkage constexpr T& operator TOML_CONCAT(op, =)(T & lhs, T rhs) noexcept                                          \
	{                                                                                                                  \
		return lhs = (lhs op rhs);                                                                                     \
	}                                                                                                                  \
                                                                                                                       \
	static_assert(true)
#define TOML_MAKE_FLAGS_1(T, linkage)                                                                                  \
	static_assert(std::is_enum_v<T>);                                                                                  \
                                                                                                                       \
	TOML_MAKE_FLAGS_2(T, &, linkage);                                                                                  \
	TOML_MAKE_FLAGS_2(T, |, linkage);                                                                                  \
	TOML_MAKE_FLAGS_2(T, ^, linkage);                                                                                  \
                                                                                                                       \
	TOML_CONST_INLINE_GETTER                                                                                           \
	linkage constexpr T operator~(T val) noexcept                                                                      \
	{                                                                                                                  \
		using under = std::underlying_type_t<T>;                                                                       \
		return static_cast<T>(~static_cast<under>(val));                                                               \
	}                                                                                                                  \
                                                                                                                       \
	TOML_CONST_INLINE_GETTER                                                                                           \
	linkage constexpr bool operator!(T val) noexcept                                                                   \
	{                                                                                                                  \
		using under = std::underlying_type_t<T>;                                                                       \
		return !static_cast<under>(val);                                                                               \
	}                                                                                                                  \
                                                                                                                       \
	static_assert(true)
#define TOML_MAKE_FLAGS(T) TOML_MAKE_FLAGS_1(T, )

#define TOML_UNUSED(...) static_cast<void>(__VA_ARGS__)

#define TOML_DELETE_DEFAULTS(T)                                                                                        \
	T(const T&)			   = delete;                                                                                   \
	T(T&&)				   = delete;                                                                                   \
	T& operator=(const T&) = delete;                                                                                   \
	T& operator=(T&&)	   = delete

#define TOML_ASYMMETRICAL_EQUALITY_OPS(LHS, RHS, ...)                                                                  \
	__VA_ARGS__ TOML_NODISCARD                                                                                         \
	friend bool operator==(RHS lhs, LHS rhs) TOML_NOEXCEPT_AS(rhs == lhs)                                              \
	{                                                                                                                  \
		return rhs == lhs;                                                                                             \
	}                                                                                                                  \
	__VA_ARGS__ TOML_NODISCARD                                                                                         \
	friend bool operator!=(LHS lhs, RHS rhs) TOML_NOEXCEPT_AS(lhs == rhs)                                              \
	{                                                                                                                  \
		return !(lhs == rhs);                                                                                          \
	}                                                                                                                  \
	__VA_ARGS__ TOML_NODISCARD                                                                                         \
	friend bool operator!=(RHS lhs, LHS rhs) TOML_NOEXCEPT_AS(rhs == lhs)                                              \
	{                                                                                                                  \
		return !(rhs == lhs);                                                                                          \
	}                                                                                                                  \
	static_assert(true)

#define TOML_EVAL_BOOL_1(T, F) T
#define TOML_EVAL_BOOL_0(T, F) F

#if !defined(__POXY__) && !defined(POXY_IMPLEMENTATION_DETAIL)
#define POXY_IMPLEMENTATION_DETAIL(...) __VA_ARGS__
#endif

// set by this repository's own builds, which then see the warnings toml.hpp turns off for users
#ifndef TOML_PEDANTIC
#define TOML_PEDANTIC 0
#endif

#if TOML_CLANG
#define TOML_PUSH_WARNINGS                                                                                             \
	TOML_PRAGMA_CLANG(diagnostic push)                                                                                 \
	TOML_PRAGMA_CLANG(diagnostic ignored "-Wunknown-warning-option")                                                   \
	static_assert(true)
#define TOML_POP_WARNINGS                                                                                              \
	TOML_PRAGMA_CLANG(diagnostic pop)                                                                                  \
	static_assert(true)
#elif TOML_MSVC
#define TOML_PUSH_WARNINGS                                                                                             \
	__pragma(warning(push))                                                                                            \
	static_assert(true)
#define TOML_POP_WARNINGS                                                                                              \
	__pragma(warning(pop))                                                                                             \
	static_assert(true)
#elif TOML_GCC
#define TOML_PUSH_WARNINGS                                                                                             \
	TOML_PRAGMA_GCC(diagnostic push)                                                                                   \
	static_assert(true)
#define TOML_POP_WARNINGS                                                                                              \
	TOML_PRAGMA_GCC(diagnostic pop)                                                                                    \
	static_assert(true)
#else
#define TOML_PUSH_WARNINGS static_assert(true)
#define TOML_POP_WARNINGS  static_assert(true)
#endif

#define TOML_DISABLE_CLANG_WARNING(...)                                                                                \
	TOML_PRAGMA_CLANG(diagnostic ignored "-Wunknown-warning-option")                                                   \
	TOML_PRAGMA_CLANG(diagnostic ignored __VA_ARGS__)                                                                  \
	static_assert(true)

#define TOML_DISABLE_GCC_WARNING(...)                                                                                  \
	TOML_PRAGMA_GCC(diagnostic ignored "-Wpragmas")                                                                    \
	TOML_PRAGMA_GCC(diagnostic ignored __VA_ARGS__)                                                                    \
	static_assert(true)

#define TOML_DISABLE_MSVC_WARNING(...)                                                                                 \
	TOML_PRAGMA_MSVC(warning(disable : __VA_ARGS__))                                                                   \
	static_assert(true)

#if TOML_MSVC && TOML_HAS_INCLUDE(<CodeAnalysis/Warnings.h>)
#pragma warning(push, 0)
#include <CodeAnalysis/Warnings.h>
#pragma warning(pop)
#define TOML_DISABLE_CODE_ANALYSIS_WARNINGS                                                                            \
	__pragma(warning(disable : ALL_CODE_ANALYSIS_WARNINGS))                                                            \
	static_assert(true)
#else
#define TOML_DISABLE_CODE_ANALYSIS_WARNINGS static_assert(true)
#endif

#define TOML_DISABLE_SWITCH_WARNINGS                                                                                   \
	TOML_DISABLE_CLANG_WARNING("-Wswitch");                                                                            \
	TOML_DISABLE_CLANG_WARNING("-Wswitch-enum");                                                                       \
	TOML_DISABLE_CLANG_WARNING("-Wswitch-default");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wswitch");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wswitch-enum");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wswitch-default");                                                                      \
	TOML_DISABLE_MSVC_WARNING(4061);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4062);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4063);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4468);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(5262); /* switch-case implicit fallthrough (false-positive) */                           \
	static_assert(true)

#define TOML_DISABLE_ARITHMETIC_WARNINGS                                                                               \
	TOML_DISABLE_CLANG_WARNING("-Wimplicit-int-float-conversion");                                                     \
	TOML_DISABLE_CLANG_WARNING("-Wfloat-equal");                                                                       \
	TOML_DISABLE_CLANG_WARNING("-Wdouble-promotion");                                                                  \
	TOML_DISABLE_CLANG_WARNING("-Wchar-subscripts");                                                                   \
	TOML_DISABLE_CLANG_WARNING("-Wshift-sign-overflow");                                                               \
	TOML_DISABLE_GCC_WARNING("-Wfloat-equal");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wsign-conversion");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wchar-subscripts");                                                                     \
	TOML_DISABLE_MSVC_WARNING(4365); /* argument signed/unsigned mismatch */                                           \
	TOML_DISABLE_MSVC_WARNING(4738); /* storing 32-bit float result in memory */                                       \
	TOML_DISABLE_MSVC_WARNING(5219); /* implicit conversion from integral to float */                                  \
	static_assert(true)

#define TOML_DISABLE_ANALYSIS_WARNINGS                                                                                 \
	TOML_DISABLE_CLANG_WARNING("-Wtautological-pointer-compare");                                                      \
	TOML_DISABLE_GCC_WARNING("-Wcast-align");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wtype-limits");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wmaybe-uninitialized");                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wnull-dereference");                                                                    \
	TOML_DISABLE_MSVC_WARNING(26451);                                                                                  \
	TOML_DISABLE_MSVC_WARNING(26495);                                                                                  \
	static_assert(true)

#define TOML_DISABLE_SUGGEST_ATTR_WARNINGS                                                                             \
	TOML_DISABLE_GCC_WARNING("-Wsuggest-attribute=const");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wsuggest-attribute=pure");                                                              \
	static_assert(true)

#define TOML_DISABLE_SPAM_WARNINGS                                                                                     \
	TOML_DISABLE_CLANG_WARNING("-Wdefaulted-function-deleted");                                                        \
	TOML_DISABLE_CLANG_WARNING("-Wctad-maybe-unsupported");                                                            \
	TOML_DISABLE_CLANG_WARNING("-Wzero-as-null-pointer-constant");                                                     \
	TOML_DISABLE_CLANG_WARNING("-Wsuggest-destructor-override");                                                       \
	TOML_DISABLE_CLANG_WARNING("-Wweak-vtables");                                                                      \
	TOML_DISABLE_CLANG_WARNING("-Wweak-template-vtables");                                                             \
	TOML_DISABLE_CLANG_WARNING("-Wmissing-field-initializers");                                                        \
	TOML_DISABLE_CLANG_WARNING("-Wpadded");                                                                            \
	TOML_DISABLE_CLANG_WARNING("-Wc++20-compat");                                                                      \
	TOML_DISABLE_CLANG_WARNING("-Wc++2a-compat");                                                                      \
	TOML_DISABLE_CLANG_WARNING("-Wc++98-compat");                                                                      \
	TOML_DISABLE_CLANG_WARNING("-Wc++98-compat-pedantic");                                                             \
	TOML_DISABLE_CLANG_WARNING("-Wc2y-extensions");                                                                    \
	TOML_DISABLE_CLANG_WARNING("-Wcovered-switch-default");                                                            \
	TOML_DISABLE_CLANG_WARNING("-Wdisabled-macro-expansion");                                                          \
	TOML_DISABLE_CLANG_WARNING("-Wnrvo");                                                                              \
	TOML_DISABLE_CLANG_WARNING("-Wpacked");                                                                            \
	TOML_DISABLE_CLANG_WARNING("-Wunsafe-buffer-usage");                                                               \
	TOML_DISABLE_CLANG_WARNING("-Wused-but-marked-unused");                                                            \
	TOML_DISABLE_GCC_WARNING("-Wpadded");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wcomment");                                                                             \
	TOML_DISABLE_GCC_WARNING("-Wuseless-cast");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wsubobject-linkage");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wmissing-field-initializers");                                                          \
	TOML_DISABLE_GCC_WARNING("-Wnoexcept");                                                                            \
	TOML_DISABLE_GCC_WARNING("-Wduplicated-branches");                                                                 \
	TOML_DISABLE_MSVC_WARNING(4127); /* conditional expr is constant */                                                \
	TOML_DISABLE_MSVC_WARNING(4324); /* structure was padded due to alignment specifier */                             \
	TOML_DISABLE_MSVC_WARNING(4348);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4464); /* relative include path contains '..' */                                         \
	TOML_DISABLE_MSVC_WARNING(4505); /* unreferenced local function removed */                                         \
	TOML_DISABLE_MSVC_WARNING(4514); /* unreferenced inline function has been removed */                               \
	TOML_DISABLE_MSVC_WARNING(4582); /* constructor is not implicitly called */                                        \
	TOML_DISABLE_MSVC_WARNING(4619); /*  there is no warning number 'XXXX' */                                          \
	TOML_DISABLE_MSVC_WARNING(4623); /* default constructor was implicitly defined as deleted */                       \
	TOML_DISABLE_MSVC_WARNING(4625); /* copy constructor was implicitly defined as deleted */                          \
	TOML_DISABLE_MSVC_WARNING(4626); /* assignment operator was implicitly defined as deleted */                       \
	TOML_DISABLE_MSVC_WARNING(4686);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4710); /* function not inlined */                                                        \
	TOML_DISABLE_MSVC_WARNING(4711); /* function selected for automatic expansion */                                   \
	TOML_DISABLE_MSVC_WARNING(4820); /* N bytes padding added */                                                       \
	TOML_DISABLE_MSVC_WARNING(4848);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4866);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(4946);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(5026); /* move constructor was implicitly defined as deleted	*/                          \
	TOML_DISABLE_MSVC_WARNING(5027); /* move assignment operator was implicitly defined as deleted	*/                  \
	TOML_DISABLE_MSVC_WARNING(5039); /* potentially throwing function passed to 'extern "C"' function */               \
	TOML_DISABLE_MSVC_WARNING(5045); /* Compiler will insert Spectre mitigation */                                     \
	TOML_DISABLE_MSVC_WARNING(5246);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(5264); /* const variable is not used (false-positive) */                                 \
	TOML_DISABLE_MSVC_WARNING(26490);                                                                                  \
	TOML_DISABLE_MSVC_WARNING(26812);                                                                                  \
	TOML_DISABLE_MSVC_WARNING(26819); /* cg: unannotated fallthrough */                                                \
	static_assert(true)

#if TOML_CLANG
#define TOML_DISABLE_WARNINGS                                                                                          \
	TOML_PRAGMA_CLANG(diagnostic push)                                                                                 \
	TOML_PRAGMA_CLANG(diagnostic ignored "-Weverything")                                                               \
	static_assert(true)
#elif TOML_MSVC
#define TOML_DISABLE_WARNINGS                                                                                          \
	__pragma(warning(push, 0))                                                                                         \
	TOML_DISABLE_MSVC_WARNING(4668);                                                                                   \
	TOML_DISABLE_MSVC_WARNING(5105);                                                                                   \
	TOML_DISABLE_CODE_ANALYSIS_WARNINGS;                                                                               \
	TOML_DISABLE_SWITCH_WARNINGS;                                                                                      \
	TOML_DISABLE_SPAM_WARNINGS;                                                                                        \
	TOML_DISABLE_ARITHMETIC_WARNINGS;                                                                                  \
	TOML_DISABLE_ANALYSIS_WARNINGS;                                                                                    \
	static_assert(true)
#elif TOML_GCC
#define TOML_DISABLE_WARNINGS                                                                                          \
	TOML_PRAGMA_GCC(diagnostic push)                                                                                   \
	TOML_DISABLE_SWITCH_WARNINGS;                                                                                      \
	TOML_DISABLE_ARITHMETIC_WARNINGS;                                                                                  \
	TOML_DISABLE_ANALYSIS_WARNINGS;                                                                                    \
	TOML_DISABLE_SUGGEST_ATTR_WARNINGS;                                                                                \
	TOML_DISABLE_SPAM_WARNINGS;                                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wabi");                                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wabi-tag");                                                                             \
	TOML_DISABLE_GCC_WARNING("-Waddress");                                                                             \
	TOML_DISABLE_GCC_WARNING("-Waggregate-return");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wall");                                                                                 \
	TOML_DISABLE_GCC_WARNING("-Walloc-zero");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Walloca");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Warith-conversion");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Warray-bounds");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wbuiltin-macro-redefined");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wc++17-extensions");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wcast-qual");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wchkp");                                                                                \
	TOML_DISABLE_GCC_WARNING("-Wclass-memaccess");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wclobbered");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wcomma-subscript");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wconditionally-supported");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wconversion");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wctad-maybe-unsupported");                                                              \
	TOML_DISABLE_GCC_WARNING("-Wctor-dtor-privacy");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wdate-time");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wdelete-non-virtual-dtor");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated-copy");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated-copy-dtor");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated-declarations");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated-enum-enum-conversion");                                                     \
	TOML_DISABLE_GCC_WARNING("-Wdeprecated-enum-float-conversion");                                                    \
	TOML_DISABLE_GCC_WARNING("-Wdisabled-optimization");                                                               \
	TOML_DISABLE_GCC_WARNING("-Wdouble-promotion");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wduplicated-cond");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wempty-body");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wendif-labels");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wenum-compare");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wextra");                                                                               \
	TOML_DISABLE_GCC_WARNING("-Wextra-semi");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wfloat-conversion");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wformat");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wformat-contains-nul");                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wformat-extra-args");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wformat-nonliteral");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wformat-security");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wformat-signedness");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wformat-zero-length");                                                                  \
	TOML_DISABLE_GCC_WARNING("-Wignored-qualifiers");                                                                  \
	TOML_DISABLE_GCC_WARNING("-Winit-self");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Winline");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Winterference-size");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Winvalid-imported-macros");                                                             \
	TOML_DISABLE_GCC_WARNING("-Winvalid-pch");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wliteral-suffix");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wlogical-op");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wlong-long");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wmain");                                                                                \
	TOML_DISABLE_GCC_WARNING("-Wmismatched-tags");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wmissing-braces");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wmissing-declarations");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wmissing-format-attribute");                                                            \
	TOML_DISABLE_GCC_WARNING("-Wmissing-include-dirs");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wmissing-noreturn");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wmultichar");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wmultiple-inheritance");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wnamespaces");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wnarrowing");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wnoexcept-type");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wnon-virtual-dtor");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wnonnull");                                                                             \
	TOML_DISABLE_GCC_WARNING("-Wold-style-cast");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Woverlength-strings");                                                                  \
	TOML_DISABLE_GCC_WARNING("-Woverloaded-virtual");                                                                  \
	TOML_DISABLE_GCC_WARNING("-Wpacked");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wpacked-bitfield-compat");                                                              \
	TOML_DISABLE_GCC_WARNING("-Wparentheses");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wpedantic");                                                                            \
	TOML_DISABLE_GCC_WARNING("-Wpessimizing-move");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wpointer-arith");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wredundant-decls");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wredundant-tags");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wregister");                                                                            \
	TOML_DISABLE_GCC_WARNING("-Wreorder");                                                                             \
	TOML_DISABLE_GCC_WARNING("-Wrestrict");                                                                            \
	TOML_DISABLE_GCC_WARNING("-Wreturn-type");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wsequence-point");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wshadow");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wshift-negative-value");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wsign-compare");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wsign-promo");                                                                          \
	TOML_DISABLE_GCC_WARNING("-Wstack-protector");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wstrict-aliasing");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wstrict-null-sentinel");                                                                \
	TOML_DISABLE_GCC_WARNING("-Wstrict-overflow");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wstring-compare");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wstringop-overread");                                                                   \
	TOML_DISABLE_GCC_WARNING("-Wstringop-truncation");                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wsuggest-final-methods");                                                               \
	TOML_DISABLE_GCC_WARNING("-Wsuggest-final-types");                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wsuggest-override");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wsynth");                                                                               \
	TOML_DISABLE_GCC_WARNING("-Wsystem-headers");                                                                      \
	TOML_DISABLE_GCC_WARNING("-Wtemplates");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wterminate");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wtrampolines");                                                                         \
	TOML_DISABLE_GCC_WARNING("-Wtrigraphs");                                                                           \
	TOML_DISABLE_GCC_WARNING("-Wundef");                                                                               \
	TOML_DISABLE_GCC_WARNING("-Wuninitialized");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wunknown-pragmas");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wunreachable-code");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wunsafe-loop-optimizations");                                                           \
	TOML_DISABLE_GCC_WARNING("-Wunused");                                                                              \
	TOML_DISABLE_GCC_WARNING("-Wunused-but-set-parameter");                                                            \
	TOML_DISABLE_GCC_WARNING("-Wunused-but-set-variable");                                                             \
	TOML_DISABLE_GCC_WARNING("-Wunused-function");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wunused-label");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wunused-local-typedefs");                                                               \
	TOML_DISABLE_GCC_WARNING("-Wunused-macros");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wunused-parameter");                                                                    \
	TOML_DISABLE_GCC_WARNING("-Wunused-result");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wunused-value");                                                                        \
	TOML_DISABLE_GCC_WARNING("-Wunused-variable");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wvariadic-macros");                                                                     \
	TOML_DISABLE_GCC_WARNING("-Wvector-operation-performance");                                                        \
	TOML_DISABLE_GCC_WARNING("-Wvirtual-inheritance");                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wvla");                                                                                 \
	TOML_DISABLE_GCC_WARNING("-Wvolatile");                                                                            \
	TOML_DISABLE_GCC_WARNING("-Wvolatile-register-var");                                                               \
	TOML_DISABLE_GCC_WARNING("-Wwrite-strings");                                                                       \
	TOML_DISABLE_GCC_WARNING("-Wzero-as-null-pointer-constant");                                                       \
	static_assert(true)
#else
#define TOML_DISABLE_WARNINGS static_assert(true)
#endif

#define TOML_ENABLE_WARNINGS TOML_POP_WARNINGS

#if TOML_CLANG
#define TOML_SIMPLE_STATIC_ASSERT_MESSAGES 1
#endif

#ifndef TOML_SIMPLE_STATIC_ASSERT_MESSAGES
#define TOML_SIMPLE_STATIC_ASSERT_MESSAGES 0
#endif

#ifdef TOML_CONFIG_HEADER
#include TOML_CONFIG_HEADER
#endif

// some defaults below depend on the standard library's version macros
#if TOML_HAS_INCLUDE(<version>)
TOML_DISABLE_WARNINGS;
#include <version>
TOML_ENABLE_WARNINGS;
#endif

// is the library being built as a shared lib/dll using meson and friends?
#ifndef TOML_SHARED_LIB
#define TOML_SHARED_LIB 0
#endif

// header-only mode
#if !defined(TOML_HEADER_ONLY) && defined(TOML_ALL_INLINE) // was TOML_ALL_INLINE pre-2.0
#define TOML_HEADER_ONLY TOML_ALL_INLINE
#endif
#if !defined(TOML_HEADER_ONLY) || (defined(TOML_HEADER_ONLY) && TOML_HEADER_ONLY) || TOML_INTELLISENSE
#undef TOML_HEADER_ONLY
#define TOML_HEADER_ONLY 1
#endif
#if TOML_DOXYGEN || TOML_SHARED_LIB
#undef TOML_HEADER_ONLY
#define TOML_HEADER_ONLY 0
#endif

// internal implementation switch
#if defined(TOML_IMPLEMENTATION) || TOML_HEADER_ONLY
#undef TOML_IMPLEMENTATION
#define TOML_IMPLEMENTATION 1
#else
#define TOML_IMPLEMENTATION 0
#endif

// dll/shared lib function exports (legacy - TOML_API was the old name for this setting)
#if !defined(TOML_EXPORTED_MEMBER_FUNCTION) && !defined(TOML_EXPORTED_STATIC_FUNCTION)                                 \
	&& !defined(TOML_EXPORTED_FREE_FUNCTION) && !defined(TOML_EXPORTED_CLASS) && defined(TOML_API)
#define TOML_EXPORTED_MEMBER_FUNCTION TOML_API
#define TOML_EXPORTED_STATIC_FUNCTION TOML_API
#define TOML_EXPORTED_FREE_FUNCTION	  TOML_API
#endif

// dll/shared lib exports
#if TOML_SHARED_LIB
#undef TOML_API
#undef TOML_EXPORTED_CLASS
#undef TOML_EXPORTED_MEMBER_FUNCTION
#undef TOML_EXPORTED_STATIC_FUNCTION
#undef TOML_EXPORTED_FREE_FUNCTION
#if TOML_WINDOWS
#if TOML_IMPLEMENTATION
#define TOML_EXPORTED_CLASS			__declspec(dllexport)
#define TOML_EXPORTED_FREE_FUNCTION __declspec(dllexport)
#else
#define TOML_EXPORTED_CLASS			__declspec(dllimport)
#define TOML_EXPORTED_FREE_FUNCTION __declspec(dllimport)
#endif
#ifndef TOML_CALLCONV
#define TOML_CALLCONV __cdecl
#endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#define TOML_EXPORTED_CLASS			  __attribute__((visibility("default")))
#define TOML_EXPORTED_MEMBER_FUNCTION __attribute__((visibility("default")))
#define TOML_EXPORTED_STATIC_FUNCTION __attribute__((visibility("default")))
#define TOML_EXPORTED_FREE_FUNCTION	  __attribute__((visibility("default")))
#endif
#endif
#ifndef TOML_EXPORTED_CLASS
#define TOML_EXPORTED_CLASS
#endif
#ifndef TOML_EXPORTED_MEMBER_FUNCTION
#define TOML_EXPORTED_MEMBER_FUNCTION
#endif
#ifndef TOML_EXPORTED_STATIC_FUNCTION
#define TOML_EXPORTED_STATIC_FUNCTION
#endif
#ifndef TOML_EXPORTED_FREE_FUNCTION
#define TOML_EXPORTED_FREE_FUNCTION
#endif

// gcc leaves an inline static member of a dllexport class out of the dll unless something in the dll uses it
#if TOML_WINDOWS && TOML_GCC && TOML_IMPLEMENTATION && !TOML_HEADER_ONLY
#define TOML_EXPORTED_CONSTANT TOML_ATTR(used)
#else
#define TOML_EXPORTED_CONSTANT
#endif

// experimental language features
#if !defined(TOML_ENABLE_UNRELEASED_FEATURES) && defined(TOML_UNRELEASED_FEATURES) // was TOML_UNRELEASED_FEATURES
																				   // pre-3.0
#define TOML_ENABLE_UNRELEASED_FEATURES TOML_UNRELEASED_FEATURES
#endif
#if (defined(TOML_ENABLE_UNRELEASED_FEATURES) && TOML_ENABLE_UNRELEASED_FEATURES) || TOML_INTELLISENSE
#undef TOML_ENABLE_UNRELEASED_FEATURES
#define TOML_ENABLE_UNRELEASED_FEATURES 1
#endif
#ifndef TOML_ENABLE_UNRELEASED_FEATURES
#define TOML_ENABLE_UNRELEASED_FEATURES 0
#endif

// parser
#if !defined(TOML_ENABLE_PARSER) && defined(TOML_PARSER) // was TOML_PARSER pre-3.0
#define TOML_ENABLE_PARSER TOML_PARSER
#endif
#if !defined(TOML_ENABLE_PARSER) || (defined(TOML_ENABLE_PARSER) && TOML_ENABLE_PARSER) || TOML_INTELLISENSE
#undef TOML_ENABLE_PARSER
#define TOML_ENABLE_PARSER 1
#endif

// formatters
#if !defined(TOML_ENABLE_FORMATTERS) || (defined(TOML_ENABLE_FORMATTERS) && TOML_ENABLE_FORMATTERS) || TOML_INTELLISENSE
#undef TOML_ENABLE_FORMATTERS
#define TOML_ENABLE_FORMATTERS 1
#endif

// std::chrono
#if !defined(TOML_ENABLE_CHRONO) || (defined(TOML_ENABLE_CHRONO) && TOML_ENABLE_CHRONO) || TOML_INTELLISENSE
#undef TOML_ENABLE_CHRONO
#define TOML_ENABLE_CHRONO 1
#endif
// libstdc++ has <version> from GCC 9 and the calendar types from GCC 11
#if TOML_CPP < 20 || !TOML_HAS_INCLUDE(<version>) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE < 11)
#undef TOML_ENABLE_CHRONO
#define TOML_ENABLE_CHRONO 0
#endif

// SIMD
#if !defined(TOML_ENABLE_SIMD) || (defined(TOML_ENABLE_SIMD) && TOML_ENABLE_SIMD) || TOML_INTELLISENSE
#undef TOML_ENABLE_SIMD
#define TOML_ENABLE_SIMD 1
#endif

// windows compat
#if !defined(TOML_ENABLE_WINDOWS_COMPAT) && defined(TOML_WINDOWS_COMPAT) // was TOML_WINDOWS_COMPAT pre-3.0
#define TOML_ENABLE_WINDOWS_COMPAT TOML_WINDOWS_COMPAT
#endif
#if !defined(TOML_ENABLE_WINDOWS_COMPAT) || (defined(TOML_ENABLE_WINDOWS_COMPAT) && TOML_ENABLE_WINDOWS_COMPAT)        \
	|| TOML_INTELLISENSE
#undef TOML_ENABLE_WINDOWS_COMPAT
#define TOML_ENABLE_WINDOWS_COMPAT 1
#endif

#if !TOML_WINDOWS
#undef TOML_ENABLE_WINDOWS_COMPAT
#define TOML_ENABLE_WINDOWS_COMPAT 0
#endif

#ifndef TOML_INCLUDE_WINDOWS_H
#define TOML_INCLUDE_WINDOWS_H 0
#endif

// custom optional
#ifdef TOML_OPTIONAL_TYPE
#define TOML_HAS_CUSTOM_OPTIONAL_TYPE 1
#else
#define TOML_HAS_CUSTOM_OPTIONAL_TYPE 0
#endif

// exceptions (library use)
#if TOML_COMPILER_HAS_EXCEPTIONS
#if !defined(TOML_EXCEPTIONS) || (defined(TOML_EXCEPTIONS) && TOML_EXCEPTIONS)
#undef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 1
#endif
#else
#if defined(TOML_EXCEPTIONS) && TOML_EXCEPTIONS
#error TOML_EXCEPTIONS was explicitly enabled but exceptions are disabled/unsupported by the compiler.
#endif
#undef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif

// calling convention for static/free/friend functions
#ifndef TOML_CALLCONV
#define TOML_CALLCONV
#endif

#ifndef TOML_UNDEF_MACROS
#define TOML_UNDEF_MACROS 1
#endif

#ifndef TOML_MAX_NESTED_VALUES
#define TOML_MAX_NESTED_VALUES 128
// this refers to the depth of any value, whether nested by arrays, inline tables, dotted keys or table headers.
// 128 is very generous; real TOML files rarely exceed single-digit nesting.
// keep this value low enough to avoid stack overflows in sanitizer-instrumented builds
// where each recursion cycle may consume ~3KB of stack.
#endif

#ifdef TOML_CHAR_8_STRINGS
#if TOML_CHAR_8_STRINGS
#error TOML_CHAR_8_STRINGS was removed in toml++ 2.0.0; all value setters and getters now work with char8_t strings implicitly.
#endif
#endif

#ifdef TOML_LARGE_FILES
#if !TOML_LARGE_FILES
#error Support for !TOML_LARGE_FILES (i.e. 'small files') was removed in toml++ 3.0.0.
#endif
#endif

#ifndef TOML_LIFETIME_HOOKS
#define TOML_LIFETIME_HOOKS 0
#endif

#ifdef NDEBUG
#undef TOML_ASSERT
#define TOML_ASSERT(expr) static_assert(true)
#endif
#ifndef TOML_ASSERT
#ifdef __clang_tidy__
TOML_DISABLE_WARNINGS;
#include <cstdlib>
TOML_ENABLE_WARNINGS;
#define TOML_ASSERT(expr)                                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		TOML_DISABLE_WARNINGS;                                                                                         \
		if (!(expr))	  /* NOLINT */                                                                                 \
			std::abort(); /* NOLINT */                                                                                 \
		TOML_ENABLE_WARNINGS;                                                                                          \
	}                                                                                                                  \
	while (false)
#else
#ifndef assert
TOML_DISABLE_WARNINGS;
#include <cassert>
TOML_ENABLE_WARNINGS;
#endif
#define TOML_ASSERT(expr) assert(expr)
#endif
#endif
#ifdef NDEBUG
#define TOML_ASSERT_ASSUME(expr) TOML_ASSUME(expr)
#else
#define TOML_ASSERT_ASSUME(expr) TOML_ASSERT(expr)
#endif

#ifndef TOML_ENABLE_FLOAT16
#define TOML_ENABLE_FLOAT16 0
#endif

#ifndef TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA
#if TOML_MSVC && TOML_CPP < 20
#define TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA 1
#else
#define TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA 0
#endif
#endif

#ifndef TOML_DISABLE_NOEXCEPT_NOEXCEPT
#define TOML_DISABLE_NOEXCEPT_NOEXCEPT 0
#if TOML_MSVC_LIKE && TOML_MSVC_LIKE <= 1943 // Up to Visual Studio 2022 Version 17.13.6
#undef TOML_DISABLE_NOEXCEPT_NOEXCEPT
#define TOML_DISABLE_NOEXCEPT_NOEXCEPT 1
#endif
#endif

#if TOML_DISABLE_NOEXCEPT_NOEXCEPT
#define TOML_NOEXCEPT_AS(...) noexcept
#else
#define TOML_NOEXCEPT_AS(...) noexcept(noexcept(__VA_ARGS__))
#endif

#ifndef TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN
#if TOML_GCC && TOML_GCC <= 7
#define TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN 1
#else
#define TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN 0
#endif
#endif

#if TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN && !defined(TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_ACKNOWLEDGED)
#define TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_MESSAGE                                                                  \
	"If you're seeing this error it's because you're using one of toml++'s for_each() functions on a compiler with "   \
	"known bugs in that area (e.g. GCC 7). On these compilers returning a bool (or bool-convertible) value from the "  \
	"for_each() callable causes spurious compilation failures, while returning nothing (void) works fine. "            \
	"If you believe this message is incorrect for your compiler, you can try your luck by #defining "                  \
	"TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN as 0 and recompiling - if it works, great! Let me know at "                 \
	"https://github.com/marzer/tomlplusplus/issues. Alternatively, if you don't have any need for early-exiting from " \
	"for_each(), you can suppress this error by #defining TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_ACKNOWLEDGED "         \
	"and moving on with your life."
#endif

// libc++ never defines __cpp_lib_to_chars, and its availability flags follow the deployment target on apple
#if !TOML_HAS_INCLUDE(<charconv>)
#define TOML_HAS_FLOAT_CHARCONV 0
#elif defined(_LIBCPP_VERSION)
#if defined(_LIBCPP_AVAILABILITY_HAS_FROM_CHARS_FLOATING_POINT) && _LIBCPP_AVAILABILITY_HAS_FROM_CHARS_FLOATING_POINT  \
	&& defined(_LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT) && _LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT
#define TOML_HAS_FLOAT_CHARCONV 1
#else
#define TOML_HAS_FLOAT_CHARCONV 0
#endif
#elif defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
#define TOML_HAS_FLOAT_CHARCONV 1
#else
#define TOML_HAS_FLOAT_CHARCONV 0
#endif

#ifndef TOML_FLOAT_CHARCONV
#if TOML_HAS_FLOAT_CHARCONV
#define TOML_FLOAT_CHARCONV 1
#else
#define TOML_FLOAT_CHARCONV 0
#endif
#endif

#ifndef TOML_INT_CHARCONV
// libc++ before 15 needed a newer apple deployment target for integer to_chars, and early emscripten didn't build it
#if TOML_HAS_INCLUDE(<charconv>)                                                                                                   \
					 && !(defined(_LIBCPP_VERSION) && _LIBCPP_VERSION < 15000                                          \
						  && (defined(__APPLE__) || defined(__EMSCRIPTEN__)))
#define TOML_INT_CHARCONV 1
#else
#define TOML_INT_CHARCONV 0
#endif
#endif

#if (TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV) && !TOML_HAS_INCLUDE(<charconv>)
#undef TOML_INT_CHARCONV
#undef TOML_FLOAT_CHARCONV
#define TOML_INT_CHARCONV	0
#define TOML_FLOAT_CHARCONV 0
#endif

#if defined(__cpp_concepts) && __cpp_concepts >= 201907
#define TOML_REQUIRES(...) requires(__VA_ARGS__)
#else
#define TOML_REQUIRES(...)
#endif
#define TOML_ENABLE_IF(...) , typename std::enable_if<(__VA_ARGS__), int>::type = 0
#define TOML_CONSTRAINED_TEMPLATE(condition, ...)                                                                      \
	template <__VA_ARGS__ TOML_ENABLE_IF(condition)>                                                                   \
	TOML_REQUIRES(condition)
#define TOML_HIDDEN_CONSTRAINT(condition, ...) TOML_CONSTRAINED_TEMPLATE(condition, __VA_ARGS__)

#if defined(__SIZEOF_FLOAT128__) && defined(__FLT128_MANT_DIG__) && defined(__LDBL_MANT_DIG__)                         \
	&& __FLT128_MANT_DIG__ > __LDBL_MANT_DIG__
#define TOML_FLOAT128 __float128
#endif

#ifdef __SIZEOF_INT128__
#define TOML_INT128	 __int128_t
#define TOML_UINT128 __uint128_t
#endif

// clang-format off

//********  impl/version.hpp  ******************************************************************************************

#define TOML_LIB_MAJOR 3
#define TOML_LIB_MINOR 4
#define TOML_LIB_PATCH 0

#define TOML_LANG_MAJOR 1
#define TOML_LANG_MINOR 1
#define TOML_LANG_PATCH 0

//********  impl/preprocessor.hpp  *************************************************************************************

#define	TOML_LIB_SINGLE_HEADER 1

#if TOML_ENABLE_UNRELEASED_FEATURES
	#define TOML_LANG_EFFECTIVE_VERSION													\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH+1)
#else
	#define TOML_LANG_EFFECTIVE_VERSION													\
		TOML_MAKE_VERSION(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)
#endif

#define TOML_LANG_HIGHER_THAN(major, minor, patch)										\
		(TOML_LANG_EFFECTIVE_VERSION > TOML_MAKE_VERSION(major, minor, patch))

#define TOML_LANG_AT_LEAST(major, minor, patch)											\
		(TOML_LANG_EFFECTIVE_VERSION >= TOML_MAKE_VERSION(major, minor, patch))

#define TOML_LANG_UNRELEASED															\
		TOML_LANG_HIGHER_THAN(TOML_LANG_MAJOR, TOML_LANG_MINOR, TOML_LANG_PATCH)

#ifndef TOML_ABI_NAMESPACES
	#if TOML_DOXYGEN
		#define TOML_ABI_NAMESPACES 0
	#else
		#define TOML_ABI_NAMESPACES 1
	#endif
#endif
#if TOML_ABI_NAMESPACES
	#define TOML_NAMESPACE_START				namespace toml { inline namespace TOML_CONCAT(v, TOML_LIB_MAJOR)
	#define TOML_NAMESPACE_END					} static_assert(true)
	#define TOML_NAMESPACE						::toml::TOML_CONCAT(v, TOML_LIB_MAJOR)
	#define TOML_ABI_NAMESPACE_START(name)		inline namespace name { static_assert(true)
	#define TOML_ABI_NAMESPACE_BOOL(cond, T, F)	TOML_ABI_NAMESPACE_START(TOML_CONCAT(TOML_EVAL_BOOL_, cond)(T, F))
	#define TOML_ABI_NAMESPACE_END				} static_assert(true)
#else
	#define TOML_NAMESPACE_START				namespace toml
	#define TOML_NAMESPACE_END					static_assert(true)
	#define TOML_NAMESPACE						toml
	#define TOML_ABI_NAMESPACE_START(...)		static_assert(true)
	#define TOML_ABI_NAMESPACE_BOOL(...)		static_assert(true)
	#define TOML_ABI_NAMESPACE_END				static_assert(true)
#endif
#define TOML_IMPL_NAMESPACE_START				TOML_NAMESPACE_START { namespace impl
#define TOML_IMPL_NAMESPACE_END					} TOML_NAMESPACE_END
#if TOML_HEADER_ONLY
	#if TOML_ABI_NAMESPACES
		#define TOML_ANON_NAMESPACE_START		static_assert(TOML_IMPLEMENTATION); TOML_IMPL_NAMESPACE_START {	\
												inline namespace TOML_CONCAT(TOML_EVAL_BOOL_, TOML_EXCEPTIONS)(impl_ex, impl_noex)
		#define TOML_ANON_NAMESPACE_END			} TOML_IMPL_NAMESPACE_END
	#else
		#define TOML_ANON_NAMESPACE_START		static_assert(TOML_IMPLEMENTATION); TOML_IMPL_NAMESPACE_START
		#define TOML_ANON_NAMESPACE_END			TOML_IMPL_NAMESPACE_END
	#endif
	#define TOML_ANON_NAMESPACE					TOML_NAMESPACE::impl
	#define TOML_EXTERNAL_LINKAGE				inline
	#define TOML_INTERNAL_LINKAGE				inline
#else
	#define TOML_ANON_NAMESPACE_START			static_assert(TOML_IMPLEMENTATION); TOML_IMPL_NAMESPACE_START {	\
												namespace
	#define TOML_ANON_NAMESPACE_END				} TOML_IMPL_NAMESPACE_END
	#define TOML_ANON_NAMESPACE					TOML_NAMESPACE::impl
	#define TOML_EXTERNAL_LINKAGE
	#define TOML_INTERNAL_LINKAGE				static
#endif

// clang-format on

// clang-format off

#if TOML_SIMPLE_STATIC_ASSERT_MESSAGES

	#define TOML_SA_NEWLINE		" "
	#define TOML_SA_LIST_SEP	", "
	#define TOML_SA_LIST_BEG	" ("
	#define TOML_SA_LIST_END	")"
	#define TOML_SA_LIST_NEW	" "
	#define TOML_SA_LIST_NXT	", "

#else

	#define TOML_SA_NEWLINE			"\n| "
	#define TOML_SA_LIST_SEP		TOML_SA_NEWLINE "  - "
	#define TOML_SA_LIST_BEG		TOML_SA_LIST_SEP
	#define TOML_SA_LIST_END
	#define TOML_SA_LIST_NEW		TOML_SA_NEWLINE TOML_SA_NEWLINE
	#define TOML_SA_LIST_NXT		TOML_SA_LIST_NEW

#endif

#define TOML_SA_NATIVE_VALUE_TYPE_LIST							\
	TOML_SA_LIST_BEG	"std::string"							\
	TOML_SA_LIST_SEP	"int64_t"								\
	TOML_SA_LIST_SEP	"double"								\
	TOML_SA_LIST_SEP	"bool"									\
	TOML_SA_LIST_SEP	"toml::date"							\
	TOML_SA_LIST_SEP	"toml::time"							\
	TOML_SA_LIST_SEP	"toml::date_time"						\
	TOML_SA_LIST_END

#define TOML_SA_NODE_TYPE_LIST									\
	TOML_SA_LIST_BEG	"toml::table"							\
	TOML_SA_LIST_SEP	"toml::array"							\
	TOML_SA_LIST_SEP	"toml::value<std::string>"				\
	TOML_SA_LIST_SEP	"toml::value<int64_t>"					\
	TOML_SA_LIST_SEP	"toml::value<double>"					\
	TOML_SA_LIST_SEP	"toml::value<bool>"						\
	TOML_SA_LIST_SEP	"toml::value<toml::date>"				\
	TOML_SA_LIST_SEP	"toml::value<toml::time>"				\
	TOML_SA_LIST_SEP	"toml::value<toml::date_time>"			\
	TOML_SA_LIST_END

#define TOML_SA_UNWRAPPED_NODE_TYPE_LIST						\
	TOML_SA_LIST_NEW	"A native TOML value type"				\
	TOML_SA_NATIVE_VALUE_TYPE_LIST								\
																\
	TOML_SA_LIST_NXT	"A TOML node type"						\
	TOML_SA_NODE_TYPE_LIST

// clang-format on

TOML_PUSH_WARNINGS;
TOML_DISABLE_SPAM_WARNINGS;
TOML_DISABLE_SUGGEST_ATTR_WARNINGS;
#if !TOML_PEDANTIC
TOML_DISABLE_SWITCH_WARNINGS;
TOML_DISABLE_ARITHMETIC_WARNINGS;
TOML_DISABLE_ANALYSIS_WARNINGS;
#endif

// misc warning false-positives
TOML_DISABLE_MSVC_WARNING(5031); // #pragma warning(pop): likely mismatch
#if TOML_SHARED_LIB
TOML_DISABLE_MSVC_WARNING(4251); // dll exports for std lib types
#endif
TOML_DISABLE_CLANG_WARNING("-Wheader-hygiene");
TOML_DISABLE_CLANG_WARNING("-Wdocumentation");
TOML_DISABLE_CLANG_WARNING("-Wdocumentation-unknown-command");
TOML_DISABLE_CLANG_WARNING("-Wc++20-extensions");
#if TOML_CLANG == 13
TOML_DISABLE_CLANG_WARNING("-Wreserved-identifier");
#endif

//********  impl/std_new.hpp  ******************************************************************************************

TOML_DISABLE_WARNINGS;
#include <new>
TOML_ENABLE_WARNINGS;

#if (!defined(__apple_build_version__) && TOML_CLANG >= 8) || TOML_GCC >= 7 || TOML_ICC >= 1910 || TOML_MSVC >= 1914
#define TOML_LAUNDER(x) __builtin_launder(x)
#elif defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
#define TOML_LAUNDER(x) std::launder(x)
#else
#define TOML_LAUNDER(x) x
#endif

//********  impl/std_string.hpp  ***************************************************************************************

TOML_DISABLE_WARNINGS;
#include <string_view>
#include <string>
TOML_ENABLE_WARNINGS;

#if TOML_DOXYGEN                                                                                                       \
	|| (defined(__cpp_char8_t) && __cpp_char8_t >= 201811 && defined(__cpp_lib_char8_t)                                \
		&& __cpp_lib_char8_t >= 201907)
#define TOML_HAS_CHAR8 1
#else
#define TOML_HAS_CHAR8 0
#endif

namespace toml // non-abi namespace; this is not an error
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
}

#if TOML_ENABLE_WINDOWS_COMPAT

TOML_IMPL_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::string narrow(std::wstring_view);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::wstring widen(std::string_view);

#if TOML_HAS_CHAR8

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	std::wstring widen(std::u8string_view);

#endif
}
TOML_IMPL_NAMESPACE_END;

#endif // TOML_ENABLE_WINDOWS_COMPAT

//********  impl/std_optional.hpp  *************************************************************************************

TOML_DISABLE_WARNINGS;
#if !TOML_HAS_CUSTOM_OPTIONAL_TYPE
#include <optional>
#endif
TOML_ENABLE_WARNINGS;

TOML_NAMESPACE_START
{
#if TOML_HAS_CUSTOM_OPTIONAL_TYPE

	template <typename T>
	using optional = TOML_OPTIONAL_TYPE<T>;

#else

	template <typename T>
	using optional = std::optional<T>;

#endif
}
TOML_NAMESPACE_END;

//********  impl/std_chrono.hpp  ***************************************************************************************

TOML_DISABLE_WARNINGS;
#if TOML_ENABLE_CHRONO
#include <chrono>
#endif
TOML_ENABLE_WARNINGS;

//********  impl/forward_declarations.hpp  *****************************************************************************

TOML_DISABLE_WARNINGS;
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cfloat>
#include <climits>
#include <limits>
#include <memory>
#include <iosfwd>
#include <type_traits>
#if TOML_IMPLEMENTATION
#include <cfenv>
#if !TOML_GCC_LIKE
#include <cmath>
#endif
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#ifndef TOML_DISABLE_ENVIRONMENT_CHECKS
#define TOML_ENV_MESSAGE                                                                                               \
	"If you're seeing this error it's because you're building toml++ for an environment that doesn't conform to "      \
	"one of the 'ground truths' assumed by the library. Essentially this just means that I don't have the "            \
	"resources to test on more platforms, but I wish I did! You can try disabling the checks by defining "             \
	"TOML_DISABLE_ENVIRONMENT_CHECKS, but your mileage may vary. Please consider filing an issue at "                  \
	"https://github.com/marzer/tomlplusplus/issues to help me improve support for your target environment. "           \
	"Thanks!"

static_assert(CHAR_BIT == 8, TOML_ENV_MESSAGE);
#ifdef FLT_RADIX
static_assert(FLT_RADIX == 2, TOML_ENV_MESSAGE);
#endif
static_assert('A' == 65, TOML_ENV_MESSAGE);
static_assert(sizeof(double) == 8, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::is_iec559, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits == 53, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits10 == 15, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::radix == 2, TOML_ENV_MESSAGE);

#undef TOML_ENV_MESSAGE
#endif // !TOML_DISABLE_ENVIRONMENT_CHECKS

// undocumented forward declarations are hidden from doxygen because they fuck it up =/

namespace toml // non-abi namespace; this is not an error
{
	using ::std::size_t;
	using ::std::intptr_t;
	using ::std::uintptr_t;
	using ::std::ptrdiff_t;
	using ::std::nullptr_t;
	using ::std::int8_t;
	using ::std::int16_t;
	using ::std::int32_t;
	using ::std::int64_t;
	using ::std::uint8_t;
	using ::std::uint16_t;
	using ::std::uint32_t;
	using ::std::uint64_t;
	using ::std::uint_least32_t;
	using ::std::uint_least64_t;
}

TOML_NAMESPACE_START
{
	struct date;
	struct time;
	struct time_offset;

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);
	struct date_time;
	TOML_ABI_NAMESPACE_END;

	struct source_position;
	struct source_region;

	class node;
	template <typename>
	class node_view;

	class key;
	class array;
	class table;
	template <typename>
	class value;

	class path;

	class toml_formatter;
	class json_formatter;
	class yaml_formatter;

	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);
#if TOML_EXCEPTIONS
	using parse_result = table;
#else
	class parse_result;
#endif
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	using node_ptr = std::unique_ptr<node>;

	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);
	class parser;
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	// clang-format off

	inline constexpr std::string_view control_char_escapes[] =
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

	// clang-format on

	template <typename T>
	TOML_CONST_INLINE_GETTER
	constexpr std::underlying_type_t<T> unwrap_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}
}
TOML_IMPL_NAMESPACE_END;

#if TOML_ABI_NAMESPACES
#if TOML_EXCEPTIONS
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::impl_ex::parser
#else
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::impl_noex::parser
#endif
#else
#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::parser
#endif

namespace toml
{
}

TOML_NAMESPACE_START // abi namespace
{
	inline namespace literals
	{
	}

	enum class TOML_CLOSED_ENUM node_type : uint8_t
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

	template <typename Char>
	inline std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& lhs, node_type rhs)
	{
		const auto str	 = impl::node_type_friendly_names[impl::unwrap_enum(rhs)];
		using str_char_t = decltype(str)::value_type;
		if constexpr (std::is_same_v<Char, str_char_t>)
			return lhs << str;
		else
		{
			if constexpr (sizeof(Char) == sizeof(str_char_t))
				return lhs << std::basic_string_view<Char>{ reinterpret_cast<const Char*>(str.data()), str.length() };
			else
				return lhs << str.data();
		}
	}

	enum class TOML_OPEN_FLAGS_ENUM value_flags : uint16_t // being an "OPEN" flags enum is not an error
	{
		none = 0,
		format_as_binary = 1,
		format_as_octal = 2,
		format_as_hexadecimal = 3,
	};
	TOML_MAKE_FLAGS(value_flags);

	inline constexpr value_flags preserve_source_value_flags =
		POXY_IMPLEMENTATION_DETAIL(value_flags{ static_cast<std::underlying_type_t<value_flags>>(-1) });

	enum class TOML_CLOSED_FLAGS_ENUM format_flags : uint64_t
	{
		none = 0,
		quote_dates_and_times = (1ull << 0),
		quote_infinities_and_nans = (1ull << 1),
		allow_literal_strings = (1ull << 2),
		allow_multi_line_strings = (1ull << 3),
		allow_real_tabs_in_strings = (1ull << 4),
		allow_unicode_strings = (1ull << 5),
		allow_binary_integers = (1ull << 6),
		allow_octal_integers = (1ull << 7),
		allow_hexadecimal_integers = (1ull << 8),
		indent_sub_tables = (1ull << 9),
		indent_array_elements = (1ull << 10),
		indentation = indent_sub_tables | indent_array_elements,
		relaxed_float_precision = (1ull << 11),
		terse_key_value_pairs = (1ull << 12),
		force_multi_line_arrays = (1ull << 13),
		// the flag's first spelling; removed in v4
		force_multiline_arrays = force_multi_line_arrays,
	};
	TOML_MAKE_FLAGS(format_flags);

	template <typename T>
	struct TOML_TRIVIAL_ABI inserter
	{
		static_assert(std::is_reference_v<T>);

		T value;
	};
	template <typename T>
	inserter(T&&) -> inserter<T&&>;
	template <typename T>
	inserter(T&) -> inserter<T&>;

	using default_formatter = toml_formatter;
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template <typename... T>
	using common_signed_type = std::common_type_t<std::make_signed_t<T>...>;

	template <typename T, typename... U>
	inline constexpr bool is_one_of = (false || ... || std::is_same_v<T, U>);

	template <typename... T>
	inline constexpr bool all_integral = (std::is_integral_v<T> && ...);

	template <typename T>
	inline constexpr bool is_cvref = std::is_reference_v<T> || std::is_const_v<T> || std::is_volatile_v<T>;

	template <typename T>
	inline constexpr bool is_wide_string =
		is_one_of<std::decay_t<T>, const wchar_t*, wchar_t*, std::wstring_view, std::wstring>;

	// user-defined 'string-like' types (see toml::is_string_like), detected via expression SFINAE as any type with:
	//   - a const-qualified data() or c_str() member returning (something convertible to) const char*, and
	//   - a const-qualified length() member returning (something convertible to) size_t.

	template <typename T, typename = void>
	struct has_string_like_data : std::false_type
	{};
	template <typename T>
	struct has_string_like_data<
		T,
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<const T&>().data()), const char*>>>
		: std::true_type
	{};

	template <typename T, typename = void>
	struct has_string_like_c_str : std::false_type
	{};
	template <typename T>
	struct has_string_like_c_str<
		T,
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<const T&>().c_str()), const char*>>>
		: std::true_type
	{};

	template <typename T, typename = void>
	struct has_string_like_length : std::false_type
	{};
	template <typename T>
	struct has_string_like_length<
		T,
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<const T&>().length()), size_t>>> : std::true_type
	{};

	// (std::string and std::string_view would satisfy the member checks but are first-class citizens
	// with their own dedicated handling, so they're excluded from the user-defined detection)
	template <typename T>
	inline constexpr bool is_string_like =							  //
		std::is_class_v<remove_cvref<T>>							  //
		&& !is_one_of<remove_cvref<T>, std::string, std::string_view> //
		&& (has_string_like_data<remove_cvref<T>>::value			  //
			|| has_string_like_c_str<remove_cvref<T>>::value)		  //
		&& has_string_like_length<remove_cvref<T>>::value;

	// the view over a string-like's character data (preferring data() where both it and c_str() are present)
	template <typename T>
	TOML_NODISCARD
	inline std::string_view string_like_to_view(const T& str)
	{
		static_assert(is_string_like<T>);

		if constexpr (has_string_like_data<remove_cvref<T>>::value)
			return std::string_view{ static_cast<const char*>(str.data()), static_cast<size_t>(str.length()) };
		else
			return std::string_view{ static_cast<const char*>(str.c_str()), static_cast<size_t>(str.length()) };
	}

	// ... usable as a target type when retrieving a TOML string value (storage is always std::string,
	// so the type must be constructible from the stored string or a view/pointer into it)
	template <typename T>
	inline constexpr bool string_like_is_target = is_string_like<T>
											   && (std::is_constructible_v<remove_cvref<T>, std::string_view>
												   || std::is_constructible_v<remove_cvref<T>, const char*, size_t>
												   || std::is_constructible_v<remove_cvref<T>, const std::string&>);

	// ... and whether constructing that target can throw (it is built from the first of
	// std::string_view / (const char*, size_t) / const std::string& that the type supports)
	template <typename T>
	inline constexpr bool string_like_target_is_nothrow =
		(std::is_constructible_v<remove_cvref<T>, std::string_view>
			 ? std::is_nothrow_constructible_v<remove_cvref<T>, std::string_view>
			 : (std::is_constructible_v<remove_cvref<T>, const char*, size_t>
					? std::is_nothrow_constructible_v<remove_cvref<T>, const char*, size_t>
					: std::is_nothrow_constructible_v<remove_cvref<T>, const std::string&>))
		&& std::is_nothrow_move_constructible_v<remove_cvref<T>>;

	template <typename T>
	inline constexpr bool value_retrieval_is_nothrow =
		!std::is_same_v<remove_cvref<T>, std::string>
#if TOML_HAS_CHAR8
		&& !std::is_same_v<remove_cvref<T>, std::u8string>
#endif
		// user-defined string types are constructed from the stored
		// std::string, so they can throw iff that construction can
		&& !(string_like_is_target<T> && !string_like_target_is_nothrow<T>) && !is_wide_string<T>;

	template <typename, typename>
	struct copy_ref_;
	template <typename Dest, typename Src>
	using copy_ref = typename copy_ref_<Dest, Src>::type;

	template <typename Dest, typename Src>
	struct copy_ref_
	{
		using type = Dest;
	};

	template <typename Dest, typename Src>
	struct copy_ref_<Dest, Src&>
	{
		using type = std::add_lvalue_reference_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_ref_<Dest, Src&&>
	{
		using type = std::add_rvalue_reference_t<Dest>;
	};

	template <typename, typename>
	struct copy_cv_;
	template <typename Dest, typename Src>
	using copy_cv = typename copy_cv_<Dest, Src>::type;

	template <typename Dest, typename Src>
	struct copy_cv_
	{
		using type = Dest;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, const Src>
	{
		using type = std::add_const_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, volatile Src>
	{
		using type = std::add_volatile_t<Dest>;
	};

	template <typename Dest, typename Src>
	struct copy_cv_<Dest, const volatile Src>
	{
		using type = std::add_cv_t<Dest>;
	};

	template <typename Dest, typename Src>
	using copy_cvref =
		copy_ref<copy_ref<copy_cv<std::remove_reference_t<Dest>, std::remove_reference_t<Src>>, Dest>, Src>;

	template <typename...>
	inline constexpr bool always_false = false;

	template <typename T, typename... U>
	inline constexpr bool first_is_same = false;
	template <typename T, typename... U>
	inline constexpr bool first_is_same<T, T, U...> = true;

	template <typename T, bool = std::is_enum_v<T>>
	struct underlying_type_
	{
		using type = std::underlying_type_t<T>;
	};
	template <typename T>
	struct underlying_type_<T, false>
	{
		using type = T;
	};
	template <typename T>
	using underlying_type = typename underlying_type_<T>::type;

	// general value traits
	// (as they relate to their equivalent native TOML type)
	struct default_value_traits
	{
		using native_type										  = void;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = false;
		static constexpr bool can_represent_native				  = false;
		static constexpr bool can_partially_represent_native	  = false;
		static constexpr auto type								  = node_type::none;
	};

	template <typename T>
	struct value_traits;

	// (defined alongside the other string traits, below)
	template <typename T>
	struct user_string_traits;

	// note: enums cannot have member functions, so the enum and string-like cases are mutually exclusive
	template <typename T, bool = std::is_enum_v<T>>
	struct value_traits_base_selector
	{
		static_assert(!is_cvref<T>);

		using type = std::conditional_t<is_string_like<T>, user_string_traits<T>, default_value_traits>;
	};
	template <typename T>
	struct value_traits_base_selector<T, true>
	{
		static_assert(!is_cvref<T>);

		using type = value_traits<underlying_type<T>>;
	};

	template <typename T>
	struct value_traits : value_traits_base_selector<T>::type
	{};
	template <typename T>
	struct value_traits<const T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<volatile T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<const volatile T> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T&> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T&&> : value_traits<T>
	{};

	// integer value_traits specializations - standard types
	template <typename T>
	struct integer_limits
	{
		static constexpr T min = T{ (std::numeric_limits<underlying_type<T>>::min)() };
		static constexpr T max = T{ (std::numeric_limits<underlying_type<T>>::max)() };
	};
	template <typename T>
	struct integer_traits_base : integer_limits<T>
	{
		using native_type				= int64_t;
		static constexpr bool is_native = std::is_same_v<underlying_type<T>, native_type>;
		static constexpr bool is_signed = static_cast<underlying_type<T>>(-1) < underlying_type<T>{};
		static constexpr auto type		= node_type::integer;
		static constexpr bool can_partially_represent_native = true;
	};
	template <typename T>
	struct unsigned_integer_traits : integer_traits_base<T>
	{
		static constexpr bool is_losslessly_convertible_to_native =
			integer_limits<underlying_type<T>>::max <= 9223372036854775807ULL;
		static constexpr bool can_represent_native = false;
	};
	template <typename T>
	struct signed_integer_traits : integer_traits_base<T>
	{
		static constexpr bool is_losslessly_convertible_to_native =
			integer_limits<underlying_type<T>>::min >= (-9223372036854775807LL - 1LL)
			&& integer_limits<underlying_type<T>>::max <= 9223372036854775807LL;
		static constexpr bool can_represent_native =
			integer_limits<underlying_type<T>>::min <= (-9223372036854775807LL - 1LL)
			&& integer_limits<underlying_type<T>>::max >= 9223372036854775807LL;
	};
	template <typename T, bool S = integer_traits_base<T>::is_signed>
	struct integer_traits : signed_integer_traits<T>
	{};
	template <typename T>
	struct integer_traits<T, false> : unsigned_integer_traits<T>
	{};
	template <>
	struct value_traits<signed char> : integer_traits<signed char>
	{};
	template <>
	struct value_traits<unsigned char> : integer_traits<unsigned char>
	{};
	template <>
	struct value_traits<signed short> : integer_traits<signed short>
	{};
	template <>
	struct value_traits<unsigned short> : integer_traits<unsigned short>
	{};
	template <>
	struct value_traits<signed int> : integer_traits<signed int>
	{};
	template <>
	struct value_traits<unsigned int> : integer_traits<unsigned int>
	{};
	template <>
	struct value_traits<signed long> : integer_traits<signed long>
	{};
	template <>
	struct value_traits<unsigned long> : integer_traits<unsigned long>
	{};
	template <>
	struct value_traits<signed long long> : integer_traits<signed long long>
	{};
	template <>
	struct value_traits<unsigned long long> : integer_traits<unsigned long long>
	{};
	static_assert(value_traits<int64_t>::is_native);
	static_assert(value_traits<int64_t>::is_signed);
	static_assert(value_traits<int64_t>::is_losslessly_convertible_to_native);
	static_assert(value_traits<int64_t>::can_represent_native);
	static_assert(value_traits<int64_t>::can_partially_represent_native);

	// integer value_traits specializations - non-standard types
#ifdef TOML_INT128
	template <>
	struct integer_limits<TOML_INT128>
	{
		static constexpr TOML_INT128 max =
			static_cast<TOML_INT128>((TOML_UINT128{ 1u } << ((__SIZEOF_INT128__ * CHAR_BIT) - 1)) - 1);
		static constexpr TOML_INT128 min = -max - TOML_INT128{ 1 };
	};
	template <>
	struct integer_limits<TOML_UINT128>
	{
		static constexpr TOML_UINT128 min = TOML_UINT128{};
		static constexpr TOML_UINT128 max = (2u * static_cast<TOML_UINT128>(integer_limits<TOML_INT128>::max)) + 1u;
	};
	template <>
	struct value_traits<TOML_INT128> : integer_traits<TOML_INT128>
	{};
	template <>
	struct value_traits<TOML_UINT128> : integer_traits<TOML_UINT128>
	{};
#endif
#ifdef TOML_SMALL_INT_TYPE
	template <>
	struct value_traits<TOML_SMALL_INT_TYPE> : signed_integer_traits<TOML_SMALL_INT_TYPE>
	{};
#endif

	// floating-point traits base
	template <typename T, int MantissaDigits, int DecimalDigits>
	struct float_traits_base
	{
		static constexpr auto type		= node_type::floating_point;
		using native_type				= double;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = true;

		static constexpr int bits	  = static_cast<int>(sizeof(T) * CHAR_BIT);
		static constexpr int digits	  = MantissaDigits;
		static constexpr int digits10 = DecimalDigits;

		static constexpr bool is_losslessly_convertible_to_native = bits <= 64		//
																 && digits <= 53	// DBL_MANT_DIG
																 && digits10 <= 15; // DBL_DIG

		static constexpr bool can_represent_native = digits >= 53	 // DBL_MANT_DIG
												  && digits10 >= 15; // DBL_DIG

		static constexpr bool can_partially_represent_native = digits > 0 && digits10 > 0;
	};
	template <typename T>
	struct float_traits : float_traits_base<T, std::numeric_limits<T>::digits, std::numeric_limits<T>::digits10>
	{};
#if TOML_ENABLE_FLOAT16
	template <>
	struct float_traits<_Float16> : float_traits_base<_Float16, __FLT16_MANT_DIG__, __FLT16_DIG__>
	{};
#endif
#ifdef TOML_FLOAT128
	template <>
	struct float_traits<TOML_FLOAT128> : float_traits_base<TOML_FLOAT128, __FLT128_MANT_DIG__, __FLT128_DIG__>
	{};
#endif

	template <typename T>
	struct float_limits
	{
		TOML_NODISCARD
		static constexpr T lowest() noexcept
		{
			return (std::numeric_limits<T>::lowest)();
		}

		TOML_NODISCARD
		static constexpr T highest() noexcept
		{
			return (std::numeric_limits<T>::max)();
		}
	};
#if TOML_ENABLE_FLOAT16
	template <>
	struct float_limits<_Float16>
	{
		TOML_NODISCARD
		static constexpr _Float16 lowest() noexcept
		{
			return static_cast<_Float16>(-65504.0f);
		}

		TOML_NODISCARD
		static constexpr _Float16 highest() noexcept
		{
			return static_cast<_Float16>(65504.0f);
		}
	};
#endif

	// floating-point traits
	template <>
	struct value_traits<float> : float_traits<float>
	{};
	template <>
	struct value_traits<double> : float_traits<double>
	{};
	template <>
	struct value_traits<long double> : float_traits<long double>
	{};
#if TOML_ENABLE_FLOAT16
	template <>
	struct value_traits<_Float16> : float_traits<_Float16>
	{};
#endif
#ifdef TOML_FLOAT128
	template <>
	struct value_traits<TOML_FLOAT128> : float_traits<TOML_FLOAT128>
	{};
#endif
#ifdef TOML_SMALL_FLOAT_TYPE
	template <>
	struct value_traits<TOML_SMALL_FLOAT_TYPE> : float_traits<TOML_SMALL_FLOAT_TYPE>
	{};
#endif
	static_assert(value_traits<double>::is_native);
	static_assert(value_traits<double>::is_losslessly_convertible_to_native);
	static_assert(value_traits<double>::can_represent_native);
	static_assert(value_traits<double>::can_partially_represent_native);

	// string value_traits specializations - char-based strings
	template <typename T>
	struct string_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = std::is_same_v<T, native_type>;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native =
			!std::is_array_v<T> && (!std::is_pointer_v<T> || std::is_const_v<std::remove_pointer_t<T>>);
		static constexpr bool can_partially_represent_native = can_represent_native;
		static constexpr auto type							 = node_type::string;
	};
	template <>
	struct value_traits<std::string> : string_traits<std::string>
	{};
	template <>
	struct value_traits<std::string_view> : string_traits<std::string_view>
	{};
	template <>
	struct value_traits<const char*> : string_traits<const char*>
	{};
	template <size_t N>
	struct value_traits<const char[N]> : string_traits<const char[N]>
	{};
	template <>
	struct value_traits<char*> : string_traits<char*>
	{};
	template <size_t N>
	struct value_traits<char[N]> : string_traits<char[N]>
	{};

	// string value_traits specializations - user-defined string-like types (see toml::is_string_like)
	//
	// unlike the built-in string types these are never 'native' (storage is always std::string), and the two
	// directions are tracked independently, since a type may support only one of them:
	//   - is_losslessly_convertible_to_native => usable as an initializer (always true - the detected
	//     data()/c_str() + length() members provide the character data)
	//   - can_represent_native                => usable as a retrieval target (requires a suitable constructor)
	template <typename T>
	struct user_string_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native				  = string_like_is_target<T>;
		static constexpr bool can_partially_represent_native	  = can_represent_native;
		static constexpr auto type								  = node_type::string;
	};

	// string value_traits specializations - char8_t-based strings
#if TOML_HAS_CHAR8
	template <>
	struct value_traits<std::u8string> : string_traits<std::u8string>
	{};
	template <>
	struct value_traits<std::u8string_view> : string_traits<std::u8string_view>
	{};
	template <>
	struct value_traits<const char8_t*> : string_traits<const char8_t*>
	{};
	template <size_t N>
	struct value_traits<const char8_t[N]> : string_traits<const char8_t[N]>
	{};
	template <>
	struct value_traits<char8_t*> : string_traits<char8_t*>
	{};
	template <size_t N>
	struct value_traits<char8_t[N]> : string_traits<char8_t[N]>
	{};
#endif

	// string value_traits specializations - wchar_t-based strings on Windows
#if TOML_ENABLE_WINDOWS_COMPAT
	template <typename T>
	struct wstring_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = true;							 // narrow
		static constexpr bool can_represent_native				  = std::is_same_v<T, std::wstring>; // widen
		static constexpr bool can_partially_represent_native	  = can_represent_native;
		static constexpr auto type								  = node_type::string;
	};
	template <>
	struct value_traits<std::wstring> : wstring_traits<std::wstring>
	{};
	template <>
	struct value_traits<std::wstring_view> : wstring_traits<std::wstring_view>
	{};
	template <>
	struct value_traits<const wchar_t*> : wstring_traits<const wchar_t*>
	{};
	template <size_t N>
	struct value_traits<const wchar_t[N]> : wstring_traits<const wchar_t[N]>
	{};
	template <>
	struct value_traits<wchar_t*> : wstring_traits<wchar_t*>
	{};
	template <size_t N>
	struct value_traits<wchar_t[N]> : wstring_traits<wchar_t[N]>
	{};
#endif

	// other 'native' value_traits specializations
	template <typename T, node_type NodeType>
	struct native_value_traits
	{
		using native_type										  = T;
		static constexpr bool is_native							  = true;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native				  = true;
		static constexpr bool can_partially_represent_native	  = true;
		static constexpr auto type								  = NodeType;
	};
	template <>
	struct value_traits<bool> : native_value_traits<bool, node_type::boolean>
	{};
	template <>
	struct value_traits<date> : native_value_traits<date, node_type::date>
	{};
	template <>
	struct value_traits<time> : native_value_traits<time, node_type::time>
	{};
	template <>
	struct value_traits<date_time> : native_value_traits<date_time, node_type::date_time>
	{};

#if TOML_ENABLE_CHRONO
	template <typename T>
	struct is_chrono_local_time : std::false_type
	{};
	template <typename Duration>
	struct is_chrono_local_time<std::chrono::local_time<Duration>> : std::true_type
	{};

	template <typename T>
	struct is_chrono_hh_mm_ss : std::false_type
	{};
	template <typename Duration>
	struct is_chrono_hh_mm_ss<std::chrono::hh_mm_ss<Duration>> : std::true_type
	{
		using duration_type = Duration;
	};

	template <typename Duration, typename T, node_type NodeType>
	struct chrono_value_traits : native_value_traits<T, NodeType>
	{
		static constexpr bool is_native = false;
		static constexpr bool is_losslessly_convertible_to_native =
			!std::chrono::treat_as_floating_point_v<typename Duration::rep>
			&& std::ratio_divide<typename Duration::period, std::nano>::den == 1;
		static constexpr bool can_represent_native = !std::chrono::treat_as_floating_point_v<typename Duration::rep>
												  && std::ratio_divide<std::nano, typename Duration::period>::den == 1;
	};

	template <typename Duration>
	struct value_traits<std::chrono::local_time<Duration>>
		: chrono_value_traits<Duration, date_time, node_type::date_time>
	{};

	template <typename Duration>
	struct value_traits<std::chrono::hh_mm_ss<Duration>> : chrono_value_traits<Duration, time, node_type::time>
	{};

	template <>
	struct value_traits<std::chrono::year_month_day> : native_value_traits<date, node_type::date>
	{
		static constexpr bool is_native = false;
	};
#endif

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

	// native value types => nodes
	template <typename T>
	struct node_wrapper
	{
		using type = T;
	};
	template <typename T>
	struct node_wrapper<const T>
	{
		using type = std::add_const_t<typename node_wrapper<T>::type>;
	};
	template <typename T>
	struct node_wrapper<volatile T>
	{
		using type = std::add_volatile_t<typename node_wrapper<T>::type>;
	};
	template <typename T>
	struct node_wrapper<const volatile T>
	{
		using type = std::add_const_t<std::add_volatile_t<typename node_wrapper<T>::type>>;
	};
	template <>
	struct node_wrapper<std::string>
	{
		using type = value<std::string>;
	};
	template <>
	struct node_wrapper<int64_t>
	{
		using type = value<int64_t>;
	};
	template <>
	struct node_wrapper<double>
	{
		using type = value<double>;
	};
	template <>
	struct node_wrapper<bool>
	{
		using type = value<bool>;
	};
	template <>
	struct node_wrapper<date>
	{
		using type = value<date>;
	};
	template <>
	struct node_wrapper<time>
	{
		using type = value<time>;
	};
	template <>
	struct node_wrapper<date_time>
	{
		using type = value<date_time>;
	};
	template <typename T>
	using wrap_node = typename node_wrapper<std::remove_reference_t<T>>::type;

	// nodes => native value types
	template <typename T>
	struct node_unwrapper
	{
		using type = T;
	};
	template <typename T>
	struct node_unwrapper<value<T>>
	{
		using type = T;
	};
	template <typename T>
	struct node_unwrapper<const value<T>>
	{
		using type = std::add_const_t<T>;
	};
	template <typename T>
	struct node_unwrapper<volatile value<T>>
	{
		using type = std::add_volatile_t<T>;
	};
	template <typename T>
	struct node_unwrapper<const volatile value<T>>
	{
		using type = std::add_volatile_t<std::add_const_t<T>>;
	};
	template <typename T>
	using unwrap_node = typename node_unwrapper<std::remove_reference_t<T>>::type;

	template <typename T, typename Container>
	using for_each_ref = copy_cvref<wrap_node<remove_cvref<unwrap_node<T>>>, Container>;

	template <typename T>
	struct node_type_getter
	{
		static constexpr auto value = value_traits<T>::type;
	};
	template <>
	struct node_type_getter<table>
	{
		static constexpr auto value = node_type::table;
	};
	template <>
	struct node_type_getter<array>
	{
		static constexpr auto value = node_type::array;
	};
	template <>
	struct node_type_getter<void>
	{
		static constexpr auto value = node_type::none;
	};
	template <typename T>
	inline constexpr node_type node_type_of = node_type_getter<unwrap_node<remove_cvref<T>>>::value;

	template <typename T, typename ConvertFrom>
	inline constexpr bool is_constructible_or_convertible = std::is_constructible_v<T, ConvertFrom> //
														 || std::is_convertible_v<ConvertFrom, T>;

	// calls a for_each() visitor; false means it returned something that converts to false, to stop the loop
	template <typename Func, typename... Args>
	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	bool for_each_keep_going(Func && visitor, Args && ... args) noexcept(std::is_nothrow_invocable_v<Func&&, Args&&...>)
	{
		using return_type = decltype(static_cast<Func&&>(visitor)(static_cast<Args&&>(args)...));

		if constexpr (is_constructible_or_convertible<bool, return_type>)
			return static_cast<bool>(static_cast<Func&&>(visitor)(static_cast<Args&&>(args)...));
		else
		{
			static_cast<Func&&>(visitor)(static_cast<Args&&>(args)...);
			return true;
		}
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref<T>, table>;

	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref<T>, array>;

	template <typename T>
	inline constexpr bool is_container = is_table<T> || is_array<T>;

	template <typename T>
	inline constexpr bool is_string = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<std::string>>;

	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<int64_t>>;

	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<		//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<double>>;

	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;

	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<				//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<bool>>;

	template <typename T>
	inline constexpr bool is_date = std::is_same_v<					//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<date>>;

	template <typename T>
	inline constexpr bool is_time = std::is_same_v<					//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<time>>;

	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<			//
		impl::remove_cvref<impl::wrap_node<impl::remove_cvref<T>>>, //
		value<date_time>>;

	template <typename T>
	inline constexpr bool is_chronological = is_date<T> || is_time<T> || is_date_time<T>;

	template <typename T>
	inline constexpr bool is_value = is_string<T> || is_number<T> || is_boolean<T> || is_chronological<T>;

	template <typename T>
	inline constexpr bool is_node = std::is_same_v<toml::node, impl::remove_cvref<T>> //
								 || std::is_base_of_v<toml::node, impl::remove_cvref<T>>;

	template <typename T>
	inline constexpr bool is_node_view = impl::is_one_of<impl::remove_cvref<T>, node_view<node>, node_view<const node>>;

	template <typename T>
	inline constexpr bool is_string_like = impl::is_string_like<T>;
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	// Q: "why not use std::fpclassify?"
	// A: Because it gets broken by -ffast-math and friends
	enum class TOML_CLOSED_ENUM fp_class : unsigned
	{
		ok,
		neg_inf,
		pos_inf,
		nan
	};

	// by reference on purpose: a by-value copy has lost its inf/nan bits under fast-math on some compilers
	TOML_PURE_GETTER
	inline fp_class fpclassify(const double& val) noexcept
	{
		static_assert(sizeof(uint64_t) == sizeof(double));

		static constexpr uint64_t sign	   = 0b1000000000000000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t exponent = 0b0111111111110000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t mantissa = 0b0000000000001111111111111111111111111111111111111111111111111111ull;

		uint64_t val_bits = {};
		std::memcpy(&val_bits, &val, sizeof(val));
		if ((val_bits & exponent) != exponent)
			return fp_class::ok;
		if ((val_bits & mantissa))
			return fp_class::nan;
		return (val_bits & sign) ? fp_class::neg_inf : fp_class::pos_inf;
	}

#if TOML_IMPLEMENTATION
#if TOML_GCC_LIKE
	TOML_CONST_INLINE_GETTER
	double ldexp(double x, int exp) noexcept
	{
		return __builtin_ldexp(x, exp);
	}

	template <typename T>
	TOML_CONST_INLINE_GETTER
	bool signbit(T x) noexcept
	{
		return __builtin_signbit(x);
	}
#else
	using std::ldexp;
	using std::signbit;
#endif

	// printf, strtod and libc++'s from_chars follow the rounding mode, and strtod raises floating-point exceptions
	// that trap in programs which unmask them
	struct default_fenv
	{
		std::fenv_t saved = {};

		default_fenv() noexcept
		{
			std::feholdexcept(&saved);
#ifdef FE_TONEAREST
			std::fesetround(FE_TONEAREST);
#endif
		}

		~default_fenv() noexcept
		{
			std::fesetenv(&saved);
		}

		default_fenv(const default_fenv&)			 = delete;
		default_fenv& operator=(const default_fenv&) = delete;
	};
#endif

	// Q: "why not use std::min?"
	// A: Because <algorithm> is _huge_ and this would be the only thing I used from it.
	//    I don't want to impose such a heavy compile-time burden on users.

	template <typename T>
	TOML_PURE_INLINE_GETTER
	constexpr T min(const T& a, const T& b) noexcept //
	{
		return a < b ? a : b;
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/print_to_stream.hpp  **********************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm using <charconv> to format numerics. Faster and locale-independent.
	//    - I can (potentially) avoid forcing users to drag in <sstream> and <iomanip>.
	//    - Strings in C++. Honestly.

	TOML_EXPORTED_FREE_FUNCTION
	TOML_ATTR(nonnull)
	void TOML_CALLCONV print_to_stream(std::ostream&, const char*, size_t);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, std::string_view);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const std::string&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, char);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, signed char, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, signed short, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, signed int, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, signed long, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, signed long long, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, unsigned char, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, unsigned short, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, unsigned int, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, unsigned long, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, unsigned long long, value_flags = {}, size_t min_digits = 0);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, float, value_flags = {}, bool relaxed_precision = false);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, double, value_flags = {}, bool relaxed_precision = false);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, bool);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const toml::date&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const toml::time&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const toml::time_offset&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const toml::date_time&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const source_position&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const source_region&);

#if TOML_ENABLE_FORMATTERS

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const array&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const table&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<std::string>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<int64_t>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<double>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<bool>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<date>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<time>&);

	TOML_EXPORTED_FREE_FUNCTION
	void TOML_CALLCONV print_to_stream(std::ostream&, const value<date_time>&);

#endif

	template <typename T, typename U>
	inline void print_to_stream_bookended(std::ostream & stream, const T& val, const U& bookend)
	{
		print_to_stream(stream, bookend);
		print_to_stream(stream, val);
		print_to_stream(stream, bookend);
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/source_region.hpp  ************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	using source_index = uint32_t;

	using source_path_ptr = std::shared_ptr<const std::string>;

	struct TOML_TRIVIAL_ABI source_position
	{
		source_index line;

		source_index column;

		TOML_PURE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return line > source_index{} //
				&& column > source_index{};
		}

		TOML_PURE_GETTER
		friend constexpr bool operator==(source_position lhs, source_position rhs) noexcept
		{
			return lhs.line == rhs.line //
				&& lhs.column == rhs.column;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(source_position lhs, source_position rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint64_t pack(source_position pos) noexcept
		{
			return static_cast<uint64_t>(pos.line) << 32 | static_cast<uint64_t>(pos.column);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(source_position lhs, source_position rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(source_position lhs, source_position rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(source_position lhs, source_position rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(source_position lhs, source_position rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const source_position& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct source_region
	{
		source_position begin;

		source_position end;

		source_path_ptr path;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		optional<std::wstring> wide_path() const
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, const source_region& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_NODISCARD
	constexpr optional<std::string_view> get_line(std::string_view doc, source_index line_num) noexcept
	{
		// Invalid line number. Should be greater than zero.
		if (line_num == 0)
			return {};

		// The position of the first character of the specified line.
		size_t begin_of_line{};
		for (source_index current_line_num{ 1 }; current_line_num < line_num; ++current_line_num)
		{
			const auto line_break = doc.find('\n', begin_of_line);
			if (line_break == std::string_view::npos)
				return {};
			begin_of_line = line_break + 1;
		}

		if (begin_of_line >= doc.size())
			return {};

		// The last line of a doc with no trailing line break runs to the end of the doc.
		auto line = doc.substr(begin_of_line, doc.find('\n', begin_of_line) - begin_of_line);

		// Trim an optional trailing carriage return.
		if (!line.empty() && line.back() == '\r')
			line.remove_suffix(1u);
		return line;
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/date_time.hpp  ****************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#if TOML_ENABLE_CHRONO
TOML_IMPL_NAMESPACE_START
{
	template <typename ToDuration, typename FromDuration>
	TOML_NODISCARD
	constexpr optional<ToDuration> duration_cast_checked(FromDuration value) noexcept
	{
		using to_rep = typename ToDuration::rep;
		static_assert(std::is_integral_v<typename FromDuration::rep>);

		if constexpr (!std::is_integral_v<to_rep>)
			return std::chrono::duration_cast<ToDuration>(value);
		else
		{
			using ratio		 = std::ratio_divide<typename FromDuration::period, typename ToDuration::period>;
			using to_limits	 = std::numeric_limits<to_rep>;
			using max_limits = std::numeric_limits<std::intmax_t>;
			static_assert(ratio::num <= (max_limits::max)() / ratio::den,
						  "the periods of the two durations are too far apart to convert between");

			// the result is count * num / den, worked out in intmax_t without overflowing it
			constexpr bool wide = sizeof(to_rep) >= sizeof(std::intmax_t);
			constexpr std::intmax_t highest =
				wide ? (max_limits::max)() : static_cast<std::intmax_t>((to_limits::max)());
			constexpr std::intmax_t lowest = !to_limits::is_signed ? std::intmax_t{}
										   : wide				   ? (max_limits::lowest)()
																   : static_cast<std::intmax_t>((to_limits::lowest)());

			const auto count = static_cast<std::intmax_t>(value.count());
			const auto whole = count / ratio::den;
			if (whole > highest / ratio::num || whole < lowest / ratio::num)
				return {};

			const auto scaled	 = whole * ratio::num;
			const auto remainder = (count % ratio::den) * ratio::num / ratio::den;
			if ((remainder > 0 && remainder > highest - scaled) || (remainder < 0 && remainder < lowest - scaled))
				return {};

			return ToDuration{ static_cast<to_rep>(scaled + remainder) };
		}
	}

	TOML_CONST_GETTER
	constexpr auto time_of_day(uint8_t hour, uint8_t minute, uint8_t second, uint32_t nanosecond) noexcept
	{
		return std::chrono::hours{ hour } + std::chrono::minutes{ minute } + std::chrono::seconds{ second }
			 + std::chrono::nanoseconds{ nanosecond };
	}
}
TOML_IMPL_NAMESPACE_END;
#endif

TOML_NAMESPACE_START
{
	struct TOML_TRIVIAL_ABI date
	{
		uint16_t year;

		uint8_t month;

		uint8_t day;

		TOML_NODISCARD_CTOR
		date() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<Y, M, D>), typename Y, typename M, typename D)
		TOML_NODISCARD_CTOR
		constexpr date(Y y, M m, D d) noexcept //
			: year{ static_cast<uint16_t>(y) },
			  month{ static_cast<uint8_t>(m) },
			  day{ static_cast<uint8_t>(d) }
		{}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		TOML_NODISCARD_CTOR
		constexpr date(std::chrono::year_month_day value) noexcept
			: year{ static_cast<uint16_t>(static_cast<int>(value.year())) },
			  month{ static_cast<uint8_t>(static_cast<unsigned>(value.month())) },
			  day{ static_cast<uint8_t>(static_cast<unsigned>(value.day())) }
		{}

		TOML_NODISCARD_CTOR
		constexpr date(std::chrono::year y, std::chrono::month m, std::chrono::day d) noexcept
			: date{ std::chrono::year_month_day{ y, m, d } }
		{}
#endif

		TOML_PURE_GETTER
		friend constexpr bool operator==(date lhs, date rhs) noexcept
		{
			return lhs.year == rhs.year	  //
				&& lhs.month == rhs.month //
				&& lhs.day == rhs.day;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(date lhs, date rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint32_t pack(date d) noexcept
		{
			return (static_cast<uint32_t>(d.year) << 16) | (static_cast<uint32_t>(d.month) << 8)
				 | static_cast<uint32_t>(d.day);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(date lhs, date rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(date lhs, date rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(date lhs, date rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(date lhs, date rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		TOML_PURE_GETTER
		constexpr std::chrono::year_month_day to_chrono() const noexcept
		{
			return { std::chrono::year{ static_cast<int>(year) },
					 std::chrono::month{ month },
					 std::chrono::day{ day } };
		}
#endif

		friend std::ostream& operator<<(std::ostream& lhs, const date& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct TOML_TRIVIAL_ABI time
	{
		uint8_t hour;

		uint8_t minute;

		uint8_t second;

		uint32_t nanosecond;

		TOML_NODISCARD_CTOR
		time() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M, S, NS>),
								  typename H,
								  typename M,
								  typename S  = uint8_t,
								  typename NS = uint32_t)
		TOML_NODISCARD_CTOR
		constexpr time(H h, M m, S s = S{}, NS ns = NS{}) noexcept //
			: hour{ static_cast<uint8_t>(h) },
			  minute{ static_cast<uint8_t>(m) },
			  second{ static_cast<uint8_t>(s) },
			  nanosecond{ static_cast<uint32_t>(ns) }
		{}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr time(std::chrono::hh_mm_ss<Duration> value) noexcept
			: hour{ static_cast<uint8_t>(value.hours().count()) },
			  minute{ static_cast<uint8_t>(value.minutes().count()) },
			  second{ static_cast<uint8_t>(value.seconds().count()) },
			  nanosecond{ static_cast<uint32_t>(
				  std::chrono::duration_cast<std::chrono::nanoseconds>(value.subseconds()).count()) }
		{}
#endif

		TOML_PURE_GETTER
		friend constexpr bool operator==(time lhs, time rhs) noexcept
		{
			return lhs.hour == rhs.hour		//
				&& lhs.minute == rhs.minute //
				&& lhs.second == rhs.second //
				&& lhs.nanosecond == rhs.nanosecond;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(time lhs, time rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint64_t pack(time t) noexcept
		{
			return (static_cast<uint64_t>(t.hour) << 48) | (static_cast<uint64_t>(t.minute) << 40)
				 | (static_cast<uint64_t>(t.second) << 32) | static_cast<uint64_t>(t.nanosecond);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(time lhs, time rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(time lhs, time rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(time lhs, time rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(time lhs, time rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		template <typename Duration = std::chrono::nanoseconds>
		TOML_PURE_GETTER
		constexpr optional<std::chrono::hh_mm_ss<Duration>> to_chrono() const noexcept
		{
			if (auto value = impl::duration_cast_checked<Duration>(impl::time_of_day(hour, minute, second, nanosecond)))
				return std::chrono::hh_mm_ss<Duration>{ *value };
			return {};
		}
#endif

		friend std::ostream& operator<<(std::ostream& lhs, const time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct TOML_TRIVIAL_ABI time_offset
	{
		int16_t minutes;

		TOML_NODISCARD_CTOR
		time_offset() noexcept = default;

		TOML_CONSTRAINED_TEMPLATE((impl::all_integral<H, M>), typename H, typename M)
		TOML_NODISCARD_CTOR
		constexpr time_offset(H h, M m) noexcept //
			: minutes{ static_cast<int16_t>(
				  (static_cast<impl::common_signed_type<H, M>>(h) * impl::common_signed_type<H, M>{ 60 })
				  + static_cast<impl::common_signed_type<H, M>>(m)) }
		{}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		TOML_NODISCARD_CTOR
		constexpr time_offset(std::chrono::hours h, std::chrono::minutes m) noexcept //
			: time_offset{ h + m }
		{}

		TOML_NODISCARD_CTOR
		constexpr time_offset(std::chrono::minutes m) noexcept : minutes{ static_cast<int16_t>(m.count()) }
		{}
#endif

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator==(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes == rhs.minutes;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes != rhs.minutes;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator<(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes < rhs.minutes;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator<=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes <= rhs.minutes;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes > rhs.minutes;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>=(time_offset lhs, time_offset rhs) noexcept
		{
			return lhs.minutes >= rhs.minutes;
		}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		TOML_PURE_GETTER
		constexpr std::chrono::minutes to_chrono() const noexcept
		{
			return std::chrono::minutes{ minutes };
		}
#endif

		friend std::ostream& operator<<(std::ostream& lhs, const time_offset& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);

	struct date_time
	{
		toml::date date;

		toml::time time;

		optional<toml::time_offset> offset;

		TOML_NODISCARD_CTOR
		date_time() noexcept = default;

		TOML_NODISCARD_CTOR
		constexpr date_time(toml::date d, toml::time t) noexcept //
			: date{ d },
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		explicit constexpr date_time(toml::date d) noexcept //
			: date{ d },
			  time{},
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		explicit constexpr date_time(toml::time t) noexcept //
			: date{},
			  time{ t },
			  offset{} // TINAE - icc bugfix
		{}

		TOML_NODISCARD_CTOR
		constexpr date_time(toml::date d, toml::time t, toml::time_offset off) noexcept
			: date{ d },
			  time{ t },
			  offset{ off }
		{}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::year_month_day d, std::chrono::hh_mm_ss<Duration> t) noexcept
			: date_time{ toml::date{ d }, toml::time{ t } }
		{}

		TOML_NODISCARD_CTOR
		explicit constexpr date_time(std::chrono::year_month_day d) noexcept //
			: date_time{ toml::date{ d } }
		{}

		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::year_month_day d,
							std::chrono::hh_mm_ss<Duration> t,
							std::chrono::minutes off) noexcept
			: date_time{ toml::date{ d }, toml::time{ t }, toml::time_offset{ off } }
		{}

		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::local_time<Duration> dt) noexcept
			: date_time{ std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(dt) },
						 std::chrono::hh_mm_ss<Duration>{ dt - std::chrono::floor<std::chrono::days>(dt) } }
		{}

		template <typename Duration>
		TOML_NODISCARD_CTOR
		constexpr date_time(std::chrono::local_time<Duration> dt, std::chrono::minutes off) noexcept
			: date_time{ std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(dt) },
						 std::chrono::hh_mm_ss<Duration>{ dt - std::chrono::floor<std::chrono::days>(dt) },
						 off }
		{}
#endif

		TOML_PURE_INLINE_GETTER
		constexpr bool is_local() const noexcept
		{
			return !offset.has_value();
		}

		TOML_PURE_GETTER
		friend constexpr bool operator==(const date_time& lhs, const date_time& rhs) noexcept
		{
			return lhs.date == rhs.date //
				&& lhs.time == rhs.time //
				&& lhs.offset == rhs.offset;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset < rhs.offset;
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(const date_time& lhs, const date_time& rhs) noexcept
		{
			if (lhs.date != rhs.date)
				return lhs.date < rhs.date;
			if (lhs.time != rhs.time)
				return lhs.time < rhs.time;
			return lhs.offset <= rhs.offset;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs <= rhs);
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator>=(const date_time& lhs, const date_time& rhs) noexcept
		{
			return !(lhs < rhs);
		}

#if TOML_DOXYGEN || TOML_ENABLE_CHRONO

		template <typename Duration = std::chrono::nanoseconds>
		TOML_PURE_GETTER
		constexpr optional<std::chrono::local_time<Duration>> to_chrono() const noexcept
		{
			const auto days			 = std::chrono::local_days{ date.to_chrono() }.time_since_epoch();
			const auto time_value	 = impl::time_of_day(time.hour, time.minute, time.second, time.nanosecond);
			const auto day_duration	 = impl::duration_cast_checked<Duration>(days);
			const auto time_duration = impl::duration_cast_checked<Duration>(time_value);
			if (!day_duration || !time_duration
				|| day_duration->count()
					   > (std::numeric_limits<typename Duration::rep>::max)() - time_duration->count())
				return {};
			return std::chrono::local_time<Duration>{ *day_duration + *time_duration };
		}
#endif

		friend std::ostream& operator<<(std::ostream& lhs, const date_time& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END; // TOML_HAS_CUSTOM_OPTIONAL_TYPE
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/at_path.hpp  ******************************************************************************************

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	using parse_path_callback = bool(TOML_CALLCONV*)(void*, T);

	TOML_NODISCARD
	bool TOML_CALLCONV parse_path(std::string_view,
								  void*,
								  parse_path_callback<std::string_view>,
								  parse_path_callback<size_t>);

	TOML_NODISCARD
	bool TOML_CALLCONV has_children(const node&) noexcept;

	TOML_NODISCARD
	node* TOML_CALLCONV child_at(node&, std::string_view) noexcept;

	TOML_NODISCARD
	node* TOML_CALLCONV child_at(node&, size_t) noexcept;
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, std::string_view path) noexcept;

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::string_view path) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, std::wstring_view path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::wstring_view path);

#endif
}
TOML_NAMESPACE_END;

//********  impl/std_vector.hpp  ***************************************************************************************

TOML_DISABLE_WARNINGS;
#include <vector>
#include <iterator>
TOML_ENABLE_WARNINGS;

//********  impl/path.hpp  *********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	enum class TOML_CLOSED_ENUM path_component_type : uint8_t
	{
		key			= 0x1,
		array_index = 0x2
	};

	class TOML_EXPORTED_CLASS path_component
	{
		struct storage_t
		{
			static constexpr size_t size =
				(sizeof(size_t) < sizeof(std::string) ? sizeof(std::string) : sizeof(size_t));
			static constexpr size_t align =
				(alignof(size_t) < alignof(std::string) ? alignof(std::string) : alignof(size_t));

			alignas(align) unsigned char bytes[size];
		};
		alignas(storage_t::align) storage_t value_storage_;

		path_component_type type_;

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path_component&, const path_component&) noexcept;

		template <typename Type>
		TOML_PURE_INLINE_GETTER
		static Type* get_as(storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<Type*>(s.bytes));
		}

		template <typename Type>
		TOML_PURE_INLINE_GETTER
		static const Type* get_as(const storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<const Type*>(s.bytes));
		}

		static void store_key(std::string_view key, storage_t& storage)
		{
			::new (static_cast<void*>(storage.bytes)) std::string{ key };
		}

		static void store_key(std::string&& key, storage_t& storage) noexcept
		{
			::new (static_cast<void*>(storage.bytes)) std::string{ std::move(key) };
		}

		static void store_index(size_t index, storage_t& storage) noexcept
		{
			::new (static_cast<void*>(storage.bytes)) size_t{ index };
		}

		void destroy() noexcept
		{
			if (type_ == path_component_type::key)
				get_as<std::string>(value_storage_)->~basic_string();
		}

		TOML_NODISCARD
		size_t& index_ref() noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::array_index);
			return *get_as<size_t>(value_storage_);
		}

		TOML_NODISCARD
		std::string& key_ref() noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::key);
			return *get_as<std::string>(value_storage_);
		}

	  public:

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component();

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(size_t index) noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(std::string_view key);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(std::wstring_view key);

#endif

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(const path_component& pc);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(path_component&& pc) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(const path_component& rhs);

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(path_component&& rhs) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(size_t rhs) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(std::string_view rhs);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(std::wstring_view rhs);

#endif

		~path_component() noexcept
		{
			destroy();
		}

		TOML_PURE_GETTER
		size_t index() const noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::array_index);
			return *get_as<const size_t>(value_storage_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator size_t() const noexcept
		{
			return index();
		}

		TOML_PURE_GETTER
		const std::string& key() const noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::key);
			return *get_as<const std::string>(value_storage_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const std::string&() const noexcept
		{
			return key();
		}

		TOML_PURE_INLINE_GETTER
		path_component_type type() const noexcept
		{
			return type_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const path_component& lhs, const path_component& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const path_component& lhs, const path_component& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}
	};

	class TOML_EXPORTED_CLASS path
	{
	  private:

		std::vector<path_component> components_;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_to(std::ostream&) const;

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path&, const path&) noexcept;

	  public:

		TOML_NODISCARD_CTOR
		path() noexcept = default;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::wstring_view);

#endif

		~path() noexcept = default;

		TOML_NODISCARD_CTOR
		path(const path&) = default;

		TOML_NODISCARD_CTOR
		path(path&&) noexcept = default;

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return components_.size();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return !components_.empty();
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return components_.empty();
		}

		TOML_PURE_INLINE_GETTER
		path_component& operator[](size_t index) noexcept
		{
			TOML_ASSERT(index < size());
			return components_[index];
		}

		TOML_PURE_INLINE_GETTER
		const path_component& operator[](size_t index) const noexcept
		{
			TOML_ASSERT(index < size());
			return components_[index];
		}

		path& operator=(const path&) = default;

		path& operator=(path&&) noexcept = default;

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::wstring_view);

#endif

		TOML_ALWAYS_INLINE
		path& assign(const path& p)
		{
			return *this = p;
		}

		TOML_ALWAYS_INLINE
		path& assign(path&& p) noexcept
		{
			return *this = std::move(p);
		}

		TOML_ALWAYS_INLINE
		path& assign(std::string_view str)
		{
			return *this = str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_ALWAYS_INLINE
		path& assign(std::wstring_view str)
		{
			return *this = str;
		}

#endif

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(const path&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(path&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::wstring_view);

#endif

		TOML_ALWAYS_INLINE
		path& append(const path& p)
		{
			return *this += p;
		}

		TOML_ALWAYS_INLINE
		path& append(path&& p)
		{
			return *this += std::move(p);
		}

		TOML_ALWAYS_INLINE
		path& append(std::string_view str)
		{
			return *this += str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_ALWAYS_INLINE
		path& append(std::wstring_view str)
		{
			return *this += str;
		}

#endif

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(const path&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(path&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::wstring_view);

#endif

		TOML_NODISCARD
		friend path operator+(const path& lhs, const path& rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(const path& lhs, std::string_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(std::string_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		friend path operator+(const path& lhs, std::wstring_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(std::wstring_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#endif

		TOML_ALWAYS_INLINE
		friend std::ostream& operator<<(std::ostream& os, const path& rhs)
		{
			rhs.print_to(os);
			return os;
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::string str() const;

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::string() const
		{
			return str();
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::wstring wide_str() const;

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::wstring() const
		{
			return wide_str();
		}

#endif

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const path& lhs, const path& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const path& lhs, const path& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(const path& lhs, std::string_view rhs)
		{
			return lhs == path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(std::string_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(const path& lhs, std::string_view rhs)
		{
			return lhs != path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(std::string_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(const path& lhs, std::wstring_view rhs)
		{
			return lhs == path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(std::wstring_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(const path& lhs, std::wstring_view rhs)
		{
			return lhs != path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(std::wstring_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		using iterator = std::vector<path_component>::iterator;

		using const_iterator = std::vector<path_component>::const_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return components_.end();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return components_.end();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return components_.end();
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path& truncate(size_t n);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path truncated(size_t n) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path parent() const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path leaf(size_t n = 1) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(const_iterator start, const_iterator end) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(size_t start, size_t length) const;
	};

	inline namespace literals
	{
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		path operator""_tpath(const char* str, size_t len)
		{
			return path(std::string_view{ str, len });
		}
	}

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, const toml::path& p) noexcept;

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, const toml::path& p) noexcept;
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/std_utility.hpp  **************************************************************************************

TOML_DISABLE_WARNINGS;
#include <utility>
TOML_ENABLE_WARNINGS;

//********  impl/node.hpp  *********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

// workaround for this: https://github.com/marzer/tomlplusplus/issues/220
#if TOML_NVCC
#define TOML_NVCC_WORKAROUND                                                                                           \
	{                                                                                                                  \
		return {};                                                                                                     \
	}
#else
#define TOML_NVCC_WORKAROUND = 0
#endif

TOML_NAMESPACE_START
{
	class TOML_ABSTRACT_INTERFACE TOML_EXPORTED_CLASS node
	{
	  private:

		friend class TOML_PARSER_TYPENAME;
		source_region source_{};

		template <typename T>
		TOML_NODISCARD
		decltype(auto) get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T, typename N>
		using ref_type_ = std::conditional_t<													//
			std::is_reference_v<T>,																//
			impl::copy_ref<impl::copy_cv<impl::unwrap_node<T>, std::remove_reference_t<N>>, T>, //
			impl::copy_cvref<impl::unwrap_node<T>, N>											//
			>;

		template <typename T, typename N>
		using ref_type = std::conditional_t<			 //
			std::is_reference_v<N>,						 //
			ref_type_<T, N>,							 //
			ref_type_<T, std::add_lvalue_reference_t<N>> //
			>;

		template <typename T, typename N>
		TOML_PURE_GETTER
		static ref_type<T, N&&> do_ref(N&& n) noexcept
		{
			using unwrapped_type = impl::unwrap_node<T>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::ref() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			TOML_ASSERT_ASSUME(n.template is<unwrapped_type>()
							   && "template type argument provided to node::ref() didn't match the node's actual type");

			using val_type = std::remove_volatile_t<unwrapped_type>;
			using out_ref  = ref_type<T, N&&>;
			static_assert(std::is_reference_v<out_ref>);

			if constexpr (toml::is_value<unwrapped_type>)
				return static_cast<out_ref>(ref_cast<val_type>(n).get());
			else
				return static_cast<out_ref>(ref_cast<val_type>(n));
		}

		template <typename T, typename N>
		TOML_PURE_INLINE_GETTER
		static auto do_as(N& n) noexcept
		{
			using unwrapped_type = impl::remove_cvref<impl::unwrap_node<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::as() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return n.as_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return n.as_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return n.as_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return n.as_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return n.as_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return n.as_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return n.as_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return n.as_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return n.as_date_time();

			TOML_UNREACHABLE;
		}

	  protected:
		TOML_EXPORTED_MEMBER_FUNCTION
		node() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(node&&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(node&&) noexcept;

		template <typename T, typename N>
		using ref_cast_type_ = std::conditional_t<											  //
			std::is_reference_v<T>,															  //
			impl::copy_ref<impl::copy_cv<impl::wrap_node<T>, std::remove_reference_t<N>>, T>, //
			impl::copy_cvref<impl::wrap_node<T>, N>											  //
			>;

		template <typename T, typename N>
		using ref_cast_type = std::conditional_t<			  //
			std::is_reference_v<N>,							  //
			ref_cast_type_<T, N>,							  //
			ref_cast_type_<T, std::add_lvalue_reference_t<N>> //
			>;

		template <typename T, typename N>
		TOML_PURE_INLINE_GETTER
		static ref_cast_type<T, N&&> ref_cast(N&& n) noexcept
		{
			using out_ref  = ref_cast_type<T, N&&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(static_cast<out_type&>(n));
		}

	  public:
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		virtual ~node() noexcept;

		TOML_NODISCARD
		virtual bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept = 0;

		TOML_NODISCARD
		virtual bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype) const noexcept = 0;

		template <typename ElemType = void>
		TOML_PURE_INLINE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of node::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}

		TOML_PURE_GETTER
		virtual node_type type() const noexcept TOML_NVCC_WORKAROUND;

		TOML_PURE_GETTER
		virtual bool is_table() const noexcept TOML_NVCC_WORKAROUND;

		TOML_PURE_GETTER
		virtual bool is_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_array_of_tables() const noexcept TOML_NVCC_WORKAROUND;

		TOML_PURE_GETTER
		virtual bool is_value() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_number() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		bool is() const noexcept
		{
			using unwrapped_type = impl::remove_cvref<impl::unwrap_node<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::is() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return type() == impl::node_type_of<unwrapped_type>;
		}

		TOML_PURE_GETTER
		virtual table* as_table() noexcept = 0;

		TOML_PURE_GETTER
		virtual array* as_array() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<std::string>* as_string() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<int64_t>* as_integer() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<double>* as_floating_point() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<bool>* as_boolean() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date>* as_date() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<time>* as_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date_time>* as_date_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual const table* as_table() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const array* as_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<std::string>* as_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<int64_t>* as_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<double>* as_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<bool>* as_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date>* as_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<time>* as_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date_time>* as_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		impl::wrap_node<T>* as() noexcept
		{
			return do_as<T>(*this);
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		const impl::wrap_node<T>* as() const noexcept
		{
			return do_as<T>(*this);
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() & noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() && noexcept
		{
			return do_ref<T>(static_cast<node&&>(*this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() const& noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() const&& noexcept
		{
			return do_ref<T>(static_cast<const node&&>(*this));
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

	  private:

		template <typename Func, typename Node, typename T>
		static constexpr bool can_visit = std::is_invocable_v<Func, ref_cast_type<T, Node>>;

		template <typename Func, typename Node, typename T>
		static constexpr bool can_visit_nothrow = std::is_nothrow_invocable_v<Func, ref_cast_type<T, Node>>;

		template <typename Func, typename Node>
		static constexpr bool can_visit_any = can_visit<Func, Node, table>		 //
										   || can_visit<Func, Node, array>		 //
										   || can_visit<Func, Node, std::string> //
										   || can_visit<Func, Node, int64_t>	 //
										   || can_visit<Func, Node, double>		 //
										   || can_visit<Func, Node, bool>		 //
										   || can_visit<Func, Node, date>		 //
										   || can_visit<Func, Node, time>		 //
										   || can_visit<Func, Node, date_time>;

		template <typename Func, typename Node>
		static constexpr bool can_visit_all = can_visit<Func, Node, table>		 //
										   && can_visit<Func, Node, array>		 //
										   && can_visit<Func, Node, std::string> //
										   && can_visit<Func, Node, int64_t>	 //
										   && can_visit<Func, Node, double>		 //
										   && can_visit<Func, Node, bool>		 //
										   && can_visit<Func, Node, date>		 //
										   && can_visit<Func, Node, time>		 //
										   && can_visit<Func, Node, date_time>;

		template <typename Func, typename Node, typename T>
		static constexpr bool visit_is_nothrow_one = !can_visit<Func, Node, T> || can_visit_nothrow<Func, Node, T>;

		template <typename Func, typename Node>
		static constexpr bool visit_is_nothrow = visit_is_nothrow_one<Func, Node, table>	   //
											  && visit_is_nothrow_one<Func, Node, array>	   //
											  && visit_is_nothrow_one<Func, Node, std::string> //
											  && visit_is_nothrow_one<Func, Node, int64_t>	   //
											  && visit_is_nothrow_one<Func, Node, double>	   //
											  && visit_is_nothrow_one<Func, Node, bool>		   //
											  && visit_is_nothrow_one<Func, Node, date>		   //
											  && visit_is_nothrow_one<Func, Node, time>		   //
											  && visit_is_nothrow_one<Func, Node, date_time>;

		template <typename Func, typename Node, typename T, bool = can_visit<Func, Node, T>>
		struct visit_return_type_
		{
			using type = decltype(std::declval<Func>()(std::declval<ref_cast_type<T, Node>>()));
		};
		template <typename Func, typename Node, typename T>
		struct visit_return_type_<Func, Node, T, false>
		{
			using type = void;
		};

		template <typename Func, typename Node, typename T>
		using visit_return_type = typename visit_return_type_<Func, Node, T>::type;

		template <typename A, typename B>
		using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

		// clang-format off

		template <typename Func, typename Node>
		using visit_fallback_type =
			nonvoid<visit_return_type<Func, Node, table>,
			nonvoid<visit_return_type<Func, Node, array>,
			nonvoid<visit_return_type<Func, Node, std::string>,
			nonvoid<visit_return_type<Func, Node, int64_t>,
			nonvoid<visit_return_type<Func, Node, double>,
			nonvoid<visit_return_type<Func, Node, bool>,
			nonvoid<visit_return_type<Func, Node, date>,
			nonvoid<visit_return_type<Func, Node, time>,
					visit_return_type<Func, Node, date_time>
		>>>>>>>>;

		// clang-format on

		template <typename Func, typename Node>
		static decltype(auto) do_visit(Func&& visitor, Node&& n) noexcept(visit_is_nothrow<Func&&, Node&&>)
		{
			static_assert(can_visit_any<Func&&, Node&&>,
						  "TOML node visitors must be invocable for at least one of the toml::node "
						  "types:" TOML_SA_NODE_TYPE_LIST);

			switch (n.type())
			{
				case node_type::table:
					if constexpr (can_visit<Func&&, Node&&, table>)
						return static_cast<Func&&>(visitor)(ref_cast<table>(static_cast<Node&&>(n)));
					break;

				case node_type::array:
					if constexpr (can_visit<Func&&, Node&&, array>)
						return static_cast<Func&&>(visitor)(ref_cast<array>(static_cast<Node&&>(n)));
					break;

				case node_type::string:
					if constexpr (can_visit<Func&&, Node&&, std::string>)
						return static_cast<Func&&>(visitor)(ref_cast<std::string>(static_cast<Node&&>(n)));
					break;

				case node_type::integer:
					if constexpr (can_visit<Func&&, Node&&, int64_t>)
						return static_cast<Func&&>(visitor)(ref_cast<int64_t>(static_cast<Node&&>(n)));
					break;

				case node_type::floating_point:
					if constexpr (can_visit<Func&&, Node&&, double>)
						return static_cast<Func&&>(visitor)(ref_cast<double>(static_cast<Node&&>(n)));
					break;

				case node_type::boolean:
					if constexpr (can_visit<Func&&, Node&&, bool>)
						return static_cast<Func&&>(visitor)(ref_cast<bool>(static_cast<Node&&>(n)));
					break;

				case node_type::date:
					if constexpr (can_visit<Func&&, Node&&, date>)
						return static_cast<Func&&>(visitor)(ref_cast<date>(static_cast<Node&&>(n)));
					break;

				case node_type::time:
					if constexpr (can_visit<Func&&, Node&&, time>)
						return static_cast<Func&&>(visitor)(ref_cast<time>(static_cast<Node&&>(n)));
					break;

				case node_type::date_time:
					if constexpr (can_visit<Func&&, Node&&, date_time>)
						return static_cast<Func&&>(visitor)(ref_cast<date_time>(static_cast<Node&&>(n)));
					break;

				case node_type::none:
				default: TOML_UNREACHABLE;
			}

			if constexpr (!can_visit_all<Func&&, Node&&>)
			{
				using return_type = visit_fallback_type<Func&&, Node&&>;

				if constexpr (!std::is_void_v<return_type>)
				{
					static_assert(std::is_default_constructible_v<return_type>,
								  "Non-exhaustive visitors must return a default-constructible type, or void");
					return return_type{};
				}
			}
		}

	  public:

		template <typename Func>
		decltype(auto) visit(Func&& visitor) & noexcept(visit_is_nothrow<Func&&, node&>)
		{
			return do_visit(static_cast<Func&&>(visitor), *this);
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) && noexcept(visit_is_nothrow<Func&&, node&&>)
		{
			return do_visit(static_cast<Func&&>(visitor), static_cast<node&&>(*this));
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const& noexcept(visit_is_nothrow<Func&&, const node&>)
		{
			return do_visit(static_cast<Func&&>(visitor), *this);
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const&& noexcept(visit_is_nothrow<Func&&, const node&&>)
		{
			return do_visit(static_cast<Func&&>(visitor), static_cast<const node&&>(*this));
		}

		TOML_NODISCARD
		explicit operator node_view<node>() noexcept;

		TOML_NODISCARD
		explicit operator node_view<const node>() const noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::string_view path) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::string_view path) const noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(const toml::path& p) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(const toml::path& p) const noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::wstring_view path);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::wstring_view path) const;

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> operator[](const toml::path& p) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> operator[](const toml::path& p) const noexcept;
	};
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_EXPORTED_FREE_FUNCTION
	bool TOML_CALLCONV node_deep_equality(const node*, const node*) noexcept;

	struct equal_to_same_type
	{
		const node* rhs = {};

		template <typename T>
		TOML_PURE_GETTER
		bool operator()(const T& lhs) const noexcept
		{
			return lhs == static_cast<const T&>(*rhs);
		}
	};
}
TOML_IMPL_NAMESPACE_END;

#undef TOML_NVCC_WORKAROUND

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/std_initializer_list.hpp  *****************************************************************************

TOML_DISABLE_WARNINGS;
#include <initializer_list>
TOML_ENABLE_WARNINGS;

//********  impl/node_view.hpp  ****************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	template <typename ViewedType>
	class TOML_TRIVIAL_ABI node_view
	{
		static_assert(impl::is_one_of<ViewedType, toml::node, const toml::node>,
					  "A toml::node_view<> must wrap toml::node or const toml::node");

	  public:

		using viewed_type = ViewedType;

	  private:
		viewed_type* node_ = {};

		template <typename Func>
		static constexpr bool visit_is_nothrow = noexcept(std::declval<viewed_type*>()->visit(std::declval<Func>()));

	  public:

		TOML_NODISCARD_CTOR
		node_view() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type* n) noexcept //
			: node_{ n }
		{}

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type& n) noexcept //
			: node_{ &n }
		{}

		TOML_NODISCARD_CTOR
		node_view(const node_view&) noexcept = default;

		TOML_NODISCARD_CTOR
		node_view(node_view&&) noexcept = default;

		node_view& operator=(const node_view&) & noexcept = default;

		node_view& operator=(node_view&&) & noexcept = default;

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return node_ != nullptr;
		}

		TOML_PURE_INLINE_GETTER
		viewed_type* node() const noexcept
		{
			return node_;
		}

		TOML_PURE_GETTER
		node_type type() const noexcept
		{
			return node_ ? node_->type() : node_type::none;
		}

		TOML_PURE_GETTER
		bool is_table() const noexcept
		{
			return node_ && node_->is_table();
		}

		TOML_PURE_GETTER
		bool is_array() const noexcept
		{
			return node_ && node_->is_array();
		}

		TOML_PURE_GETTER
		bool is_value() const noexcept
		{
			return node_ && node_->is_value();
		}

		TOML_PURE_GETTER
		bool is_string() const noexcept
		{
			return node_ && node_->is_string();
		}

		TOML_PURE_GETTER
		bool is_integer() const noexcept
		{
			return node_ && node_->is_integer();
		}

		TOML_PURE_GETTER
		bool is_floating_point() const noexcept
		{
			return node_ && node_->is_floating_point();
		}

		TOML_PURE_GETTER
		bool is_number() const noexcept
		{
			return node_ && node_->is_number();
		}

		TOML_PURE_GETTER
		bool is_boolean() const noexcept
		{
			return node_ && node_->is_boolean();
		}

		TOML_PURE_GETTER
		bool is_date() const noexcept
		{
			return node_ && node_->is_date();
		}

		TOML_PURE_GETTER
		bool is_time() const noexcept
		{
			return node_ && node_->is_time();
		}

		TOML_PURE_GETTER
		bool is_date_time() const noexcept
		{
			return node_ && node_->is_date_time();
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept
		{
			return node_ && node_->is_array_of_tables();
		}

		template <typename T>
		TOML_PURE_GETTER
		bool is() const noexcept
		{
			return node_ && node_->template is<T>();
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, viewed_type*& first_nonmatch) const noexcept
		{
			if (!node_)
			{
				first_nonmatch = {};
				return false;
			}
			return node_->is_homogeneous(ntype, first_nonmatch);
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype) const noexcept
		{
			return node_ && node_->is_homogeneous(ntype);
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			return node_ && node_->template is_homogeneous<ElemType>();
		}

		template <typename T>
		TOML_PURE_GETTER
		auto* as() const noexcept
		{
			return node_ ? node_->template as<T>() : nullptr;
		}

		TOML_PURE_GETTER
		auto* as_table() const noexcept
		{
			return as<table>();
		}

		TOML_PURE_GETTER
		auto* as_array() const noexcept
		{
			return as<array>();
		}

		TOML_PURE_GETTER
		auto* as_string() const noexcept
		{
			return as<std::string>();
		}

		TOML_PURE_GETTER
		auto* as_integer() const noexcept
		{
			return as<int64_t>();
		}

		TOML_PURE_GETTER
		auto* as_floating_point() const noexcept
		{
			return as<double>();
		}

		TOML_PURE_GETTER
		auto* as_boolean() const noexcept
		{
			return as<bool>();
		}

		TOML_PURE_GETTER
		auto* as_date() const noexcept
		{
			return as<date>();
		}

		TOML_PURE_GETTER
		auto* as_time() const noexcept
		{
			return as<time>();
		}

		TOML_PURE_GETTER
		auto* as_date_time() const noexcept
		{
			return as<date_time>();
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value_exact<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			using return_type = decltype(node_->value_or(static_cast<T&&>(default_value)));
			if (node_)
				return node_->value_or(static_cast<T&&>(default_value));
			return static_cast<return_type>(static_cast<T&&>(default_value));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() const noexcept
		{
			TOML_ASSERT_ASSUME(node_ && "node_view::ref() called on a node_view that did not reference a node");
			return node_->template ref<T>();
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const noexcept(visit_is_nothrow<Func&&>)
		{
			using return_type = decltype(node_->visit(static_cast<Func&&>(visitor)));
			if (node_)
				return node_->visit(static_cast<Func&&>(visitor));
			if constexpr (!std::is_void_v<return_type>)
			{
				static_assert(std::is_default_constructible_v<return_type>,
							  "node_view visitors must return a default-constructible type, or void");
				return return_type{};
			}
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return impl::node_deep_equality(lhs.node_, rhs.node());
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator!=(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const table& rhs) noexcept
		{
			const auto tbl = lhs.as<table>();
			return tbl && *tbl == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const table&, );

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const array& rhs) noexcept
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const array&, );

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const toml::value<T>& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto val = lhs.as<T>();
			return val && *val == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const toml::value<T>&, template <typename T>);

		TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>, typename T)
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const T& rhs) noexcept(!impl::is_wide_string<T>)
		{
			static_assert(!impl::is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Comparison with wide-character strings is only "
						  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

			if constexpr (impl::is_wide_string<T>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return lhs == impl::narrow(rhs);
#else
				static_assert(impl::always_false<T>, "Evaluated unreachable branch");
#endif
			}
			else
			{
				const auto val = lhs.as<impl::native_type_of<T>>();
				if constexpr (impl::is_string_like<T>)
					return val && *val == impl::string_like_to_view(rhs);
				else if constexpr (impl::is_natively_one_of<T, double>)
					return val && *val == static_cast<double>(rhs);
				else
					return val && *val == rhs;
			}
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&,
									   const T&,
									   TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>,
																 typename T));

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, std::initializer_list<T> rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, std::initializer_list<T>, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const std::vector<T>& rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::vector<T>&, template <typename T>);

		TOML_NODISCARD
		node_view operator[](std::string_view k) const noexcept
		{
			if (auto tbl = as<table>())
				return node_view{ tbl->get(k) };
			return {};
		}

		TOML_NODISCARD
		node_view operator[](const toml::path& p) const noexcept
		{
			return node_ ? node_->at_path(p) : node_view{};
		}

		TOML_NODISCARD
		node_view at_path(std::string_view p) const noexcept
		{
			return node_ ? node_->at_path(p) : node_view{};
		}

		TOML_NODISCARD
		node_view at_path(const toml::path& p) const noexcept
		{
			return node_ ? node_->at_path(p) : node_view{};
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view operator[](std::wstring_view k) const
		{
			if (auto tbl = as<table>())
				return node_view{ tbl->get(k) };
			return {};
		}

		TOML_NODISCARD
		node_view at_path(std::wstring_view p) const
		{
			return node_ ? node_->at_path(p) : node_view{};
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view operator[](size_t index) const noexcept
		{
			if (auto arr = as<array>())
				return node_view{ arr->get(index) };
			return {};
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const node_view& rhs)
		{
			if (rhs.node_)
				rhs.node_->visit([&lhs](const auto& n) { lhs << n; });
			return lhs;
		}

#endif
	};

	template <typename T>
	node_view(const T&) -> node_view<const node>;

	template <typename T>
	node_view(const T*) -> node_view<const node>;

	template <typename T>
	node_view(T&) -> node_view<node>;

	template <typename T>
	node_view(T*) -> node_view<node>;

	inline node::operator node_view<node>() noexcept
	{
		return node_view<node>{ this };
	}

	inline node::operator node_view<const node>() const noexcept
	{
		return node_view<const node>{ this };
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/value.hpp  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

// clang-format off

#if TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_SA_VALUE_MESSAGE_WSTRING			TOML_SA_LIST_SEP "std::wstring"
#else
	#define TOML_SA_VALUE_MESSAGE_WSTRING
#endif
#if TOML_HAS_CHAR8
	#define TOML_SA_VALUE_MESSAGE_U8STRING			TOML_SA_LIST_SEP "std::u8string"
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW		TOML_SA_LIST_SEP "std::u8string_view"
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8		TOML_SA_LIST_SEP "const char8_t*"
#else
	#define TOML_SA_VALUE_MESSAGE_U8STRING
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#endif
#if TOML_ENABLE_CHRONO
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS															\
		TOML_SA_LIST_SEP "std::chrono::year_month_day"														\
		TOML_SA_LIST_SEP "any std::chrono::hh_mm_ss or std::chrono::local_time of nanoseconds or finer"
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY																\
		TOML_SA_LIST_SEP "any other std::chrono::hh_mm_ss or std::chrono::local_time"
#else
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS
	#define TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY
#endif

#define TOML_SA_VALUE_MESSAGE_LOSSLESS																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_U8STRING																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS																	\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_LOSSY																			\
	TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"		\
	TOML_SA_LIST_BEG "any other integral type"																\
	TOML_SA_LIST_SEP "any floating-point type"																\
	TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY																		\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_VIEW																			\
	TOML_SA_LIST_NXT "A compatible view type"																\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_MESSAGE_STRING_LIKE																	\
	TOML_SA_LIST_NXT "A user-defined 'string-like' type (see toml::is_string_like)"							\
	TOML_SA_LIST_BEG "with const char* data() const or const char* c_str() const"							\
	TOML_SA_LIST_SEP "and size_t length() const"															\
	TOML_SA_LIST_SEP "and constructible from std::string_view, (const char*, size_t) or const std::string&"	\
	TOML_SA_LIST_END

#define TOML_SA_VALUE_EXACT_FUNC_MESSAGE(type_arg)															\
	"The " type_arg " must be one of:"																		\
	TOML_SA_VALUE_MESSAGE_LOSSLESS																			\
	TOML_SA_VALUE_MESSAGE_VIEW																				\
	TOML_SA_VALUE_MESSAGE_STRING_LIKE

#define TOML_SA_VALUE_FUNC_MESSAGE(type_arg)																\
	"The " type_arg " must be one of:"																		\
	TOML_SA_VALUE_MESSAGE_LOSSLESS																			\
	TOML_SA_VALUE_MESSAGE_LOSSY																				\
	TOML_SA_VALUE_MESSAGE_VIEW																				\
	TOML_SA_VALUE_MESSAGE_STRING_LIKE

// clang-format on
TOML_IMPL_NAMESPACE_START
{
	template <typename T, typename...>
	struct native_value_maker
	{
		template <typename... Args>
		TOML_NODISCARD
		static T make(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
		{
			// user-defined string-likes (see toml::is_string_like) provide their character data via data()/c_str()
			// + length() when they have no conversion to std::string, or one via const char* (it stops at a nul)
			if constexpr (std::is_same_v<T, std::string> && sizeof...(Args) == 1u && (is_string_like<Args> && ...)
						  && (!std::is_constructible_v<std::string, Args&&...>
							  || ((std::is_convertible_v<Args&&, const char*>
								   && !std::is_convertible_v<Args&&, std::string>)
								  && ...)))
				return std::string{ string_like_to_view(args)... };
			else
				return T(static_cast<Args&&>(args)...);
		}
	};

	template <typename T>
	struct native_value_maker<T, T>
	{
		template <typename U>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static U&& make(U&& val) noexcept
		{
			return static_cast<U&&>(val);
		}
	};

#if TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	struct string_maker
	{
		template <typename T>
		TOML_NODISCARD
		static std::string make(T&& arg)
		{
			using arg_type = std::decay_t<T>;
#if TOML_HAS_CHAR8
			if constexpr (is_one_of<arg_type, char8_t*, const char8_t*>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg)));
			}
			if constexpr (is_one_of<arg_type, std::u8string, std::u8string_view>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg.data())),
								   arg.length());
			}
#endif

#if TOML_ENABLE_WINDOWS_COMPAT
			if constexpr (is_wide_string<arg_type>)
			{
				return narrow(static_cast<T&&>(arg));
			}
#endif
		}
	};

#if TOML_HAS_CHAR8
	template <>
	struct native_value_maker<std::string, char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string_view> : string_maker
	{};
#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT
	template <>
	struct native_value_maker<std::string, wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring_view> : string_maker
	{};
#endif // TOML_ENABLE_WINDOWS_COMPAT

#endif // TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_integer_cast(int64_t val) noexcept
	{
		static_assert(node_type_of<T> == node_type::integer);
		static_assert(!is_cvref<T>);

		using traits = value_traits<T>;
		if constexpr (!traits::is_signed)
		{
			if constexpr ((sizeof(T) * CHAR_BIT) < 63) // 63 bits == int64_max
			{
				if (val < int64_t{} || val > static_cast<int64_t>(traits::max))
					return {};
			}
			else
			{
				if (val < int64_t{})
					return {};
			}
		}
		else
		{
			if (val < traits::min || val > traits::max)
				return {};
		}
		return { static_cast<T>(val) };
	}

	enum class TOML_CLOSED_ENUM numeric_order : int
	{
		less,
		equal,
		greater,
		unordered
	};

	// the first value past the int64_t range; its negative is the lowest int64_t
	inline constexpr double two_to_the_63 = 9223372036854775808.0;

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_value_from_integer([[maybe_unused]] int64_t val) noexcept
	{
		// int -> int
		if constexpr (is_natively_one_of<T, int64_t>)
		{
			if constexpr (is_native<T> || can_represent_native<T>)
				return static_cast<T>(val);
			else
				return node_integer_cast<T>(val);
		}

		// int -> float
		else if constexpr (is_natively_one_of<T, double>)
		{
			if constexpr (value_traits<T>::digits < 64)
			{
				constexpr auto largest_whole_float = (int64_t{ 1 } << value_traits<T>::digits);
				if (val < -largest_whole_float || val > largest_whole_float)
					return {};
			}
			return static_cast<T>(val);
		}

		// int -> bool
		else if constexpr (is_natively_one_of<T, bool>)
			return static_cast<bool>(val);

		// int -> anything else
		else
			return {};
	}

	// by reference, as fpclassify() takes it
	template <typename T>
	TOML_PURE_GETTER
	inline optional<T> node_value_from_float([[maybe_unused]] const double& val) noexcept
	{
		// float -> float
		if constexpr (is_natively_one_of<T, double>)
		{
			if constexpr (is_native<T> || can_represent_native<T>)
				return { static_cast<T>(val) };
			else
			{
				if (fpclassify(val) == fp_class::ok
					&& (val < static_cast<double>(float_limits<T>::lowest())
						|| val > static_cast<double>(float_limits<T>::highest())))
					return {};
				return { static_cast<T>(val) };
			}
		}

		// float -> int
		else if constexpr (is_natively_one_of<T, int64_t>)
		{
			if (fpclassify(val) == fp_class::ok && val >= -two_to_the_63 && val < two_to_the_63
				&& static_cast<double>(static_cast<int64_t>(val)) == val)
				return node_integer_cast<T>(static_cast<int64_t>(val));
			return {};
		}

		// float -> anything else
		else
			return {};
	}

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_value_from_boolean([[maybe_unused]] bool val) noexcept
	{
		// bool -> bool
		if constexpr (is_natively_one_of<T, bool>)
			return { val };

		// bool -> int
		else if constexpr (is_natively_one_of<T, int64_t>)
			return { static_cast<T>(val) };

		// bool -> anything else
		else
			return {};
	}

	// compares as numbers: converting either operand to the other's type can change its value
	template <typename T>
	TOML_CONST_GETTER
	inline numeric_order compare_integer_with_float(int64_t lhs, T rhs) noexcept
	{
		static_assert(std::is_floating_point_v<T>);

		// a float16 can't hold 2^63
		using wide			 = std::common_type_t<T, double>;
		constexpr auto limit = static_cast<wide>(two_to_the_63);
		const auto widened	 = static_cast<wide>(rhs);
		if (widened >= limit)
			return numeric_order::less;
		if (widened < -limit)
			return numeric_order::greater;
		if (!(widened >= -limit))
			return numeric_order::unordered; // nan

		const auto whole = static_cast<int64_t>(widened);
		if (lhs != whole)
			return lhs < whole ? numeric_order::less : numeric_order::greater;

		const wide fraction = widened - static_cast<wide>(whole);
		if (fraction > wide{})
			return numeric_order::less;
		if (fraction < wide{})
			return numeric_order::greater;
		return numeric_order::equal;
	}

#if TOML_ENABLE_CHRONO

	// a local time can't hold a date-time that has an offset
	template <typename T>
	TOML_PURE_GETTER
	inline optional<T> to_chrono_local_time(const date_time& dt) noexcept
	{
		if (dt.offset)
			return {};
		return dt.template to_chrono<typename T::duration>();
	}

#endif

	template <typename...>
	struct value_variadic_ctor_allowed : std::true_type
	{};

	template <typename T, typename... Args>
	struct value_variadic_ctor_allowed<value<T>, value<T>, Args...> : std::false_type
	{};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename ValueType>
	class value : public node
	{
		static_assert(impl::is_native<ValueType> && !impl::is_cvref<ValueType>,
					  "A toml::value<> must model one of the native TOML value types:" TOML_SA_NATIVE_VALUE_TYPE_LIST);

	  private:

		friend class TOML_PARSER_TYPENAME;

		ValueType val_;
		value_flags flags_ = {};

		template <typename T, typename U>
		TOML_CONST_INLINE_GETTER
		static auto as_value([[maybe_unused]] U* ptr) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return ptr;
			else
				return nullptr;
		}

	  public:

		using value_type = ValueType;

		using value_arg = POXY_IMPLEMENTATION_DETAIL(
			std::conditional_t<
				std::is_same_v<value_type, std::string>,
				std::string_view,
				std::conditional_t<impl::is_one_of<value_type, double, int64_t, bool>, value_type, const value_type&>>);

		TOML_HIDDEN_CONSTRAINT(
			(impl::value_variadic_ctor_allowed<value<ValueType>, impl::remove_cvref<Args>...>::value),
			typename... Args)
		TOML_NODISCARD_CTOR
		explicit value(Args&&... args)
#if !TOML_DISABLE_NOEXCEPT_NOEXCEPT
			noexcept(noexcept(value_type(
				impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))))
#endif
			: val_(impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_HIDDEN_CONSTRAINT((impl::value_variadic_ctor_allowed<value<ValueType>, impl::remove_cvref<T>>::value),
							   typename T)
		TOML_NODISCARD_CTOR
		explicit value(T&& val, value_flags flags)
#if !TOML_DISABLE_NOEXCEPT_NOEXCEPT
			noexcept(noexcept(
				value_type(impl::native_value_maker<value_type, std::decay_t<T>>::make(static_cast<T&&>(val)))))
#endif
			: val_(impl::native_value_maker<value_type, std::decay_t<T>>::make(static_cast<T&&>(val))),
			  flags_{ flags == preserve_source_value_flags ? value_flags{} : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other, value_flags flags) noexcept //
			: node(other),
			  val_{ other.val_ },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		// NOLINTBEGIN(bugprone-use-after-move)

		TOML_NODISCARD_CTOR
		value(value&& other) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ std::exchange(other.flags_, value_flags{}) }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(value&& other, value_flags flags) noexcept //
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
			other.flags_ = {};
		}

		value& operator=(const value& rhs) noexcept
		{
			node::operator=(rhs);
			val_   = rhs.val_;
			flags_ = rhs.flags_;
			return *this;
		}

		value& operator=(value&& rhs) noexcept
		{
			if (&rhs != this)
			{
				node::operator=(std::move(rhs));
				val_   = std::move(rhs.val_);
				flags_ = std::exchange(rhs.flags_, value_flags{});
			}
			return *this;
		}
		// NOLINTEND(bugprone-use-after-move)

#if TOML_LIFETIME_HOOKS
		~value() noexcept
		{
			TOML_VALUE_DESTROYED;
		}
#endif

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype) const noexcept final
		{
			return ntype == node_type::none || ntype == impl::node_type_of<value_type>;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final
		{
			if (!is_homogeneous(ntype))
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final
		{
			if (!is_homogeneous(ntype))
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of value::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_void_v<type>)
				return true;
			else
				return impl::node_type_of<type> == impl::node_type_of<value_type>;
		}
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return std::is_same_v<value_type, std::string>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return std::is_same_v<value_type, int64_t>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return std::is_same_v<value_type, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return impl::is_one_of<value_type, int64_t, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return std::is_same_v<value_type, bool>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return std::is_same_v<value_type, date>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return std::is_same_v<value_type, time>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return std::is_same_v<value_type, date_time>;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		value<std::string>* as_string() noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<int64_t>* as_integer() noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<double>* as_floating_point() noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<bool>* as_boolean() noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date>* as_date() noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<time>* as_time() noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date_time>* as_date_time() noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const value<std::string>* as_string() const noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<int64_t>* as_integer() const noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<double>* as_floating_point() const noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<bool>* as_boolean() const noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date>* as_date() const noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<time>* as_time() const noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date_time>* as_date_time() const noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_PURE_INLINE_GETTER
		value_type& get() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& get() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& get() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& get() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		value_type& operator*() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& operator*() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& operator*() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& operator*() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&&() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&&() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		value_type* operator->() noexcept
		{
			return &val_;
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		const value_type* operator->() const noexcept
		{
			return &val_;
		}

		TOML_PURE_INLINE_GETTER
		value_flags flags() const noexcept
		{
			return flags_;
		}

		value& flags(value_flags new_flags) noexcept
		{
			flags_ = new_flags;
			return *this;
		}

		value& operator=(value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, std::string>)
				val_.assign(rhs);
			else
				val_ = rhs;
			return *this;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_same_v<T, std::string>), typename T = value_type)
		value& operator=(std::string&& rhs) noexcept
		{
			val_ = std::move(rhs);
			return *this;
		}

		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, double>)
			{
				const auto lhs_nan = impl::fpclassify(lhs.val_) == impl::fp_class::nan;
				const auto rhs_nan = impl::fpclassify(rhs) == impl::fp_class::nan;
				if (lhs_nan != rhs_nan)
					return false;
				if (lhs_nan)
					return true;
			}
			return lhs.val_ == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, );

		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ < rhs;
		}

		TOML_PURE_GETTER
		friend bool operator<(value_arg lhs, const value& rhs) noexcept
		{
			return lhs < rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ <= rhs;
		}

		TOML_PURE_GETTER
		friend bool operator<=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs <= rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ > rhs;
		}

		TOML_PURE_GETTER
		friend bool operator>(value_arg lhs, const value& rhs) noexcept
		{
			return lhs > rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ >= rhs;
		}

		TOML_PURE_GETTER
		friend bool operator>=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs >= rhs.val_;
		}

		// an integer against a floating-point operand, which value_arg would truncate

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, T rhs) noexcept
		{
			return impl::compare_integer_with_float(lhs.val_, rhs) == impl::numeric_order::equal;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&,
									   T,
									   TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T>
																  && std::is_same_v<V, int64_t>),
																 typename T,
																 typename V = value_type));

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, T rhs) noexcept
		{
			return impl::compare_integer_with_float(lhs.val_, rhs) == impl::numeric_order::less;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<(T lhs, const value& rhs) noexcept
		{
			return impl::compare_integer_with_float(rhs.val_, lhs) == impl::numeric_order::greater;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, T rhs) noexcept
		{
			const auto order = impl::compare_integer_with_float(lhs.val_, rhs);
			return order == impl::numeric_order::less || order == impl::numeric_order::equal;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator<=(T lhs, const value& rhs) noexcept
		{
			const auto order = impl::compare_integer_with_float(rhs.val_, lhs);
			return order == impl::numeric_order::greater || order == impl::numeric_order::equal;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, T rhs) noexcept
		{
			return rhs < lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>(T lhs, const value& rhs) noexcept
		{
			return rhs < lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, T rhs) noexcept
		{
			return rhs <= lhs;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_floating_point_v<T> && std::is_same_v<V, int64_t>),
								  typename T,
								  typename V = value_type)
		TOML_PURE_GETTER
		friend bool operator>=(T lhs, const value& rhs) noexcept
		{
			return rhs <= lhs;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs == rhs.val_; // calls asymmetrical value-equality operator defined above
			else
				return false;
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const value& lhs, const value<T>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ < rhs.val_;
			else
				return impl::node_type_of<value_type> < impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ <= rhs.val_;
			else
				return impl::node_type_of<value_type> <= impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ > rhs.val_;
			else
				return impl::node_type_of<value_type> > impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ >= rhs.val_;
			else
				return impl::node_type_of<value_type> >= impl::node_type_of<T>;
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const value& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};

	template <typename T>
	value(T) -> value<impl::native_type_of<impl::remove_cvref<T>>>;
	template <typename T>
	value(T, value_flags) -> value<impl::native_type_of<impl::remove_cvref<T>>>;

	template <typename T>
	TOML_NODISCARD
	inline decltype(auto) node::get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(node_type_of<T> != node_type::none);
		static_assert(node_type_of<T> != node_type::table);
		static_assert(node_type_of<T> != node_type::array);
		static_assert(is_native<T> || can_represent_native<T>);
		static_assert(!is_cvref<T>);
		TOML_ASSERT(this->type() == node_type_of<T>);

		if constexpr (node_type_of<T> == node_type::string)
		{
			const auto& str = *ref_cast<std::string>(*this);
			if constexpr (std::is_same_v<T, std::string>)
				return str;
			else if constexpr (std::is_same_v<T, std::string_view>)
				return T{ str };
			else if constexpr (std::is_same_v<T, const char*>)
				return str.c_str();

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return widen(str);
#else
				static_assert(always_false<T>, "Evaluated unreachable branch");
#endif
			}

			// char -> user-defined string-like type (see toml::is_string_like)
			else if constexpr (string_like_is_target<T>)
			{
				// construction preference order mirrors string_like_target_is_nothrow
				// (parenthesized init, matching the std::is_constructible checks)
				if constexpr (std::is_constructible_v<T, std::string_view>)
					return T(std::string_view{ str });
				else if constexpr (std::is_constructible_v<T, const char*, size_t>)
					return T(str.data(), str.length());
				else
					return T(str);
			}

#if TOML_HAS_CHAR8

			// char -> char8_t (potentially unsafe - the feature is 'experimental'!)
			else if constexpr (is_one_of<T, std::u8string, std::u8string_view>)
				return T(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char8_t*>)
				return reinterpret_cast<const char8_t*>(str.c_str());

#endif

			else
				static_assert(always_false<T>, "Evaluated unreachable branch");
		}
#if TOML_ENABLE_CHRONO
		else if constexpr (std::is_same_v<T, std::chrono::year_month_day>)
			return ref_cast<date>(*this)->to_chrono();
#endif
		else
			return static_cast<T>(*ref_cast<native_type_of<T>>(*this));
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_exact() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

		static_assert((is_native<T> || can_represent_native<T>) && !is_cvref<T>,
					  TOML_SA_VALUE_EXACT_FUNC_MESSAGE("return type of node::value_exact()"));

		// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
		if constexpr ((is_native<T> || can_represent_native<T>) && !is_cvref<T>)
		{
			if (type() != node_type_of<T>)
				return {};

#if TOML_ENABLE_CHRONO
			if constexpr (is_chrono_local_time<T>::value)
				return to_chrono_local_time<T>(*ref_cast<date_time>(*this));
			else if constexpr (is_chrono_hh_mm_ss<T>::value)
				return ref_cast<time>(*this)->template to_chrono<typename is_chrono_hh_mm_ss<T>::duration_type>();
			else
				return { this->get_value_exact<T>() };
#else
			return { this->get_value_exact<T>() };
#endif
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");
		static_assert((is_native<T> || can_represent_native<T> || can_partially_represent_native<T>) && !is_cvref<T>,
					  TOML_SA_VALUE_FUNC_MESSAGE("return type of node::value()"));

		// when asking for strings, dates, times and date_times there's no 'fuzzy' conversion
		// semantics to be mindful of so the exact retrieval is enough.
		if constexpr (is_natively_one_of<T, std::string, time, date, date_time>)
		{
			if (type() != node_type_of<T>)
				return {};

#if TOML_ENABLE_CHRONO
			if constexpr (is_chrono_local_time<T>::value)
				return to_chrono_local_time<T>(*ref_cast<date_time>(*this));
			else if constexpr (is_chrono_hh_mm_ss<T>::value)
				return ref_cast<time>(*this)->template to_chrono<typename is_chrono_hh_mm_ss<T>::duration_type>();
			else
				return { this->get_value_exact<T>() };
#else
			return { this->get_value_exact<T>() };
#endif
		}

		// everything else requires a bit of logicking.
		else
		{
			switch (type())
			{
				// int -> *
				case node_type::integer: return node_value_from_integer<T>(*ref_cast<int64_t>(*this));

				// float -> *
				case node_type::floating_point: return node_value_from_float<T>(*ref_cast<double>(*this));

				// bool -> *
				case node_type::boolean: return node_value_from_boolean<T>(*ref_cast<bool>(*this));

				// non-values, or 'exact' types covered above
				default: return {};
			}
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline auto node::value_or(T && default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_or() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

		if constexpr (is_wide_string<T>)
		{
#if TOML_ENABLE_WINDOWS_COMPAT

			if (type() == node_type::string)
				return widen(*ref_cast<std::string>(*this));
			return std::wstring{ static_cast<T&&>(default_value) };

#else

			static_assert(always_false<T>, "Evaluated unreachable branch");

#endif
		}
		else
		{
			using value_type =
				std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
								   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
								   std::decay_t<T>>;
			using traits = value_traits<value_type>;

			// clang-format off

			static_assert(
				traits::is_native || traits::can_represent_native || traits::can_partially_represent_native,
				"The default value type of node::value_or() must be one of:"
				TOML_SA_VALUE_MESSAGE_LOSSLESS
				TOML_SA_VALUE_MESSAGE_LOSSY

				TOML_SA_LIST_NXT "A compatible view type"
				TOML_SA_LIST_BEG "std::string_view"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "std::u8string_view"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring_view"
				#endif
				TOML_SA_LIST_SEP "const char*"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "const char8_t*"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "const wchar_t*"
				#endif
				TOML_SA_LIST_END

				TOML_SA_VALUE_MESSAGE_STRING_LIKE
			);

			// clang-format on

			// prevent additional compiler error spam when the static_assert fails by gating behind if constexpr
			if constexpr (traits::is_native || traits::can_represent_native || traits::can_partially_represent_native)
			{
				if constexpr (traits::is_native)
				{
					if (type() == node_type_of<value_type>)
						return *ref_cast<typename traits::native_type>(*this);
				}
				if (auto val = this->value<value_type>())
					return *std::move(val);
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/make_node.hpp  ****************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(returns_nonnull)
	auto* make_node_impl_specialized(T && val, [[maybe_unused]] value_flags flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		static_assert(!std::is_same_v<unwrapped_type, node>);
		static_assert(!is_node_view<unwrapped_type>);

		// arrays + tables - invoke copy/move ctor
		if constexpr (is_one_of<unwrapped_type, array, table>)
		{
			return new unwrapped_type(static_cast<T&&>(val));
		}

		// values
		else
		{
			using native_type = native_type_of<unwrapped_type>;
			using value_type  = value<native_type>;

			value_type* out = {};

			// copy/move ctor
			if constexpr (std::is_same_v<remove_cvref<T>, value_type>)
			{
				out = new value_type{ static_cast<T&&>(val), flags };
			}

			// creating from raw value
			else
			{
				static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
							  "Instantiating values from wide-character strings is only "
							  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled");

				if constexpr (!is_losslessly_convertible_to_native<unwrapped_type>)
				{
					if constexpr (std::is_same_v<native_type, int64_t>)
						static_assert(always_false<T>,
									  "Integral value initializers must be losslessly convertible to int64_t");
					else if constexpr (std::is_same_v<native_type, double>)
						static_assert(always_false<T>,
									  "Floating-point value initializers must be losslessly convertible to double");
					else
						static_assert(
							always_false<T>,
							"Value initializers must be losslessly convertible to one of the TOML value types");
				}

				if constexpr (is_wide_string<T>)
				{
#if TOML_ENABLE_WINDOWS_COMPAT
					out = new value_type{ narrow(static_cast<T&&>(val)) };
#else
					static_assert(always_false<T>, "Evaluated unreachable branch");
#endif
				}
				else
					out = new value_type{ static_cast<T&&>(val) };

				if (flags != preserve_source_value_flags)
					out->flags(flags);
			}

			return out;
		}
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(T && val, value_flags flags = preserve_source_value_flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		if constexpr (std::is_same_v<unwrapped_type, node> || is_node_view<unwrapped_type>)
		{
			if constexpr (is_node_view<unwrapped_type>)
			{
				if (!val)
					return static_cast<toml::node*>(nullptr);
			}

			return static_cast<T&&>(val).visit(
				[flags](auto&& concrete)
				{
					return static_cast<toml::node*>(
						make_node_impl_specialized(static_cast<decltype(concrete)&&>(concrete), flags));
				});
		}
		else
			return make_node_impl_specialized(static_cast<T&&>(val), flags);
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(inserter<T> && val, value_flags flags = preserve_source_value_flags)
	{
		return make_node_impl(static_cast<T&&>(val.value), flags);
	}

	template <typename T,
			  bool = (is_node<T> || is_node_view<T> || is_value<T> || can_partially_represent_native<T>
					  || is_losslessly_convertible_to_native<T>)>
	struct inserted_type_of_
	{
		using type = std::remove_pointer_t<decltype(make_node_impl(std::declval<T>()))>;
	};

	template <typename T>
	struct inserted_type_of_<inserter<T>, false>
	{
		using type = typename inserted_type_of_<remove_cvref<T>>::type;
	};

	template <typename T>
	struct inserted_type_of_<T, false>
	{
		using type = void;
	};

	template <typename T>
	TOML_NODISCARD
	node_ptr make_node(T && val, value_flags flags = preserve_source_value_flags)
	{
		return node_ptr{ make_node_impl(static_cast<T&&>(val), flags) };
	}

	// make_node() returns null for these
	template <typename T>
	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	bool is_null_node_view([[maybe_unused]] const T& val) noexcept
	{
		if constexpr (is_node_view<T>)
			return !val;
		else
			return false;
	}

	template <typename T>
	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	bool is_null_node_view(const inserter<T>& val) noexcept
	{
		return is_null_node_view(val.value);
	}

	template <typename... T>
	struct emplaced_type_of_
	{
		using type = void;
	};

	template <typename T>
	struct emplaced_type_of_<T>
	{
		using type = std::conditional_t<std::is_same_v<T, node> || is_node_view<T>, //
										void,
										typename inserted_type_of_<T>::type>;
	};

	template <typename T>
	struct emplaced_type_of_<inserter<T>>
	{
		using type = typename emplaced_type_of_<remove_cvref<T>>::type;
	};

	template <typename... T>
	using emplaced_type_of = typename emplaced_type_of_<remove_cvref<T>...>::type;

	// the type an emplace function makes: the one it was given, or else the one its arguments imply
	template <typename T, typename... Args>
	using emplaced_type_or =
		std::conditional_t<std::is_void_v<remove_cvref<T>>, emplaced_type_of<Args...>, remove_cvref<T>>;
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	template <typename T>
	using inserted_type_of = POXY_IMPLEMENTATION_DETAIL(typename impl::inserted_type_of_<impl::remove_cvref<T>>::type);
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/array.hpp  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	class TOML_TRIVIAL_ABI array_iterator
	{
	  private:
		template <bool>
		friend class array_iterator;

		using mutable_vector_iterator = std::vector<node_ptr>::iterator;
		using const_vector_iterator	  = std::vector<node_ptr>::const_iterator;
		using vector_iterator		  = std::conditional_t<IsConst, const_vector_iterator, mutable_vector_iterator>;

		vector_iterator iter_;

	  public:
		using value_type		= std::conditional_t<IsConst, const node, node>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= ptrdiff_t;
		using iterator_category = typename std::iterator_traits<vector_iterator>::iterator_category;

		TOML_NODISCARD_CTOR
		array_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit array_iterator(mutable_vector_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit array_iterator(const_vector_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator<false>& other) noexcept //
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator&) noexcept = default;

		array_iterator& operator=(const array_iterator&) noexcept = default;

		array_iterator& operator++() noexcept // ++pre
		{
			++iter_;
			return *this;
		}

		array_iterator operator++(int) noexcept // post++
		{
			array_iterator out{ iter_ };
			++iter_;
			return out;
		}

		array_iterator& operator--() noexcept // --pre
		{
			--iter_;
			return *this;
		}

		array_iterator operator--(int) noexcept // post--
		{
			array_iterator out{ iter_ };
			--iter_;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const vector_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const_vector_iterator() const noexcept
		{
			return iter_;
		}

		array_iterator& operator+=(ptrdiff_t rhs) noexcept
		{
			iter_ += rhs;
			return *this;
		}

		array_iterator& operator-=(ptrdiff_t rhs) noexcept
		{
			iter_ -= rhs;
			return *this;
		}

		TOML_NODISCARD
		friend array_iterator operator+(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ + rhs };
		}

		TOML_NODISCARD
		friend array_iterator operator+(ptrdiff_t lhs, const array_iterator& rhs) noexcept
		{
			return array_iterator{ rhs.iter_ + lhs };
		}

		TOML_NODISCARD
		friend array_iterator operator-(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ - rhs };
		}

		TOML_PURE_INLINE_GETTER
		friend ptrdiff_t operator-(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ - rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ < rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ <= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ > rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ >= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		reference operator[](ptrdiff_t idx) const noexcept
		{
			return *(iter_ + idx)->get();
		}
	};

	struct array_init_elem
	{
		mutable node_ptr value;

		template <typename T>
		TOML_NODISCARD_CTOR
		// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
		array_init_elem(T&& val, value_flags flags = preserve_source_value_flags) //
			: value{ make_node(static_cast<T&&>(val), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	using array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<false>);

	using const_array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<true>);

	class TOML_EXPORTED_CLASS array : public node
	{
	  private:

		using vector_type			= std::vector<impl::node_ptr>;
		using vector_iterator		= typename vector_type::iterator;
		using const_vector_iterator = typename vector_type::const_iterator;
		vector_type elems_;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const impl::array_init_elem*, const impl::array_init_elem*);

		TOML_NODISCARD_CTOR
		array(std::false_type, std::initializer_list<impl::array_init_elem> elems) //
			: array{ elems.begin(), elems.end() }
		{}

		// unused, kept for the ABI
		TOML_EXPORTED_MEMBER_FUNCTION
		void preinsertion_resize(size_t idx, size_t count);

		TOML_EXPORTED_MEMBER_FUNCTION
		void insert_at_back(impl::node_ptr&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		vector_iterator insert_at(const_vector_iterator, impl::node_ptr&&);

		TOML_NODISCARD
		vector_iterator to_vector_iterator(const_vector_iterator pos) noexcept
		{
			return elems_.begin() + (pos - elems_.cbegin());
		}

		TOML_NODISCARD
		vector_iterator insert_range_at(const_vector_iterator pos, vector_type&& new_elems)
		{
			return elems_.insert(pos,
								 std::make_move_iterator(new_elems.begin()),
								 std::make_move_iterator(new_elems.end()));
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		size_t total_leaf_count() const noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void flatten_child(array&& child, size_t& dest_index) noexcept;

	  public:
		using value_type	  = node;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using reference		  = node&;
		using const_reference = const node&;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		~array() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const array&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(array&& other) noexcept;

		TOML_CONSTRAINED_TEMPLATE((sizeof...(ElemTypes) > 0 || !std::is_same_v<impl::remove_cvref<ElemType>, array>),
								  typename ElemType,
								  typename... ElemTypes)
		TOML_NODISCARD_CTOR
		explicit array(ElemType&& val, ElemTypes&&... vals)
			: array{ std::false_type{},
					 std::initializer_list<impl::array_init_elem>{ static_cast<ElemType&&>(val),
																   static_cast<ElemTypes&&>(vals)... } }
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(const array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(array&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::array;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		using node::is_homogeneous;
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return true;
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return is_homogeneous(node_type::table);
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		TOML_PURE_INLINE_GETTER
		node* get(size_t index) noexcept
		{
			return index < elems_.size() ? elems_[index].get() : nullptr;
		}

		TOML_PURE_INLINE_GETTER
		const node* get(size_t index) const noexcept
		{
			return const_cast<array&>(*this).get(index);
		}

		template <typename ElemType>
		TOML_PURE_GETTER
		impl::wrap_node<ElemType>* get_as(size_t index) noexcept
		{
			if (auto val = get(index))
				return val->template as<ElemType>();
			return nullptr;
		}

		template <typename ElemType>
		TOML_PURE_GETTER
		const impl::wrap_node<ElemType>* get_as(size_t index) const noexcept
		{
			return const_cast<array&>(*this).template get_as<ElemType>(index);
		}

		using node::operator[]; // inherit operator[toml::path]
		TOML_PURE_INLINE_GETTER
		node& operator[](size_t index) noexcept
		{
			return *elems_[index];
		}

		TOML_PURE_INLINE_GETTER
		const node& operator[](size_t index) const noexcept
		{
			return *elems_[index];
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(size_t index);

		TOML_NODISCARD
		const node& at(size_t index) const
		{
			return const_cast<array&>(*this).at(index);
		}

		TOML_PURE_INLINE_GETTER
		node& front() noexcept
		{
			return *elems_.front();
		}

		TOML_PURE_INLINE_GETTER
		const node& front() const noexcept
		{
			return *elems_.front();
		}

		TOML_PURE_INLINE_GETTER
		node& back() noexcept
		{
			return *elems_.back();
		}

		TOML_PURE_INLINE_GETTER
		const node& back() const noexcept
		{
			return *elems_.back();
		}

		using iterator = array_iterator;

		using const_iterator = const_array_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return iterator{ elems_.begin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return iterator{ elems_.end() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

	  private:

		template <typename Func, typename Array, typename T>
		using can_for_each = std::disjunction<std::is_invocable<Func, impl::for_each_ref<T, Array>, size_t>,
											  std::is_invocable<Func, size_t, impl::for_each_ref<T, Array>>,
											  std::is_invocable<Func, impl::for_each_ref<T, Array>>>;

		template <typename Func, typename Array, typename T>
		using can_for_each_nothrow = std::conditional_t<
			// first form
			std::is_invocable_v<Func, impl::for_each_ref<T, Array>, size_t>,
			std::is_nothrow_invocable<Func, impl::for_each_ref<T, Array>, size_t>,
			std::conditional_t<
				// second form
				std::is_invocable_v<Func, size_t, impl::for_each_ref<T, Array>>,
				std::is_nothrow_invocable<Func, size_t, impl::for_each_ref<T, Array>>,
				std::conditional_t<
					// third form
					std::is_invocable_v<Func, impl::for_each_ref<T, Array>>,
					std::is_nothrow_invocable<Func, impl::for_each_ref<T, Array>>,
					std::false_type>>>;

		template <typename Func, typename Array>
		using can_for_each_any = std::disjunction<can_for_each<Func, Array, table>,
												  can_for_each<Func, Array, array>,
												  can_for_each<Func, Array, std::string>,
												  can_for_each<Func, Array, int64_t>,
												  can_for_each<Func, Array, double>,
												  can_for_each<Func, Array, bool>,
												  can_for_each<Func, Array, date>,
												  can_for_each<Func, Array, time>,
												  can_for_each<Func, Array, date_time>>;

		template <typename Func, typename Array, typename T>
		using for_each_is_nothrow_one = std::disjunction<std::negation<can_for_each<Func, Array, T>>, //
														 can_for_each_nothrow<Func, Array, T>>;

		template <typename Func, typename Array>
		using for_each_is_nothrow = std::conjunction<for_each_is_nothrow_one<Func, Array, table>,
													 for_each_is_nothrow_one<Func, Array, array>,
													 for_each_is_nothrow_one<Func, Array, std::string>,
													 for_each_is_nothrow_one<Func, Array, int64_t>,
													 for_each_is_nothrow_one<Func, Array, double>,
													 for_each_is_nothrow_one<Func, Array, bool>,
													 for_each_is_nothrow_one<Func, Array, date>,
													 for_each_is_nothrow_one<Func, Array, time>,
													 for_each_is_nothrow_one<Func, Array, date_time>>;

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

		// false stops the loop
		template <typename Array, typename Func, typename Elem>
		TOML_NODISCARD
		static bool for_each_visit([[maybe_unused]] Func&& visitor,
								   [[maybe_unused]] Elem&& elem,
								   [[maybe_unused]] size_t i) //
			noexcept(for_each_is_nothrow_one<Func&&, Array, Elem&&>::value)
		{
			using elem_ref = impl::for_each_ref<Elem&&, Array>;
			static_assert(std::is_reference_v<elem_ref>);

			// func(elem, i)
			if constexpr (std::is_invocable_v<Func&&, elem_ref, size_t>)
				return impl::for_each_keep_going(static_cast<Func&&>(visitor), static_cast<elem_ref>(elem), i);

			// func(i, elem)
			else if constexpr (std::is_invocable_v<Func&&, size_t, elem_ref>)
				return impl::for_each_keep_going(static_cast<Func&&>(visitor), i, static_cast<elem_ref>(elem));

			// func(elem)
			else if constexpr (std::is_invocable_v<Func&&, elem_ref>)
				return impl::for_each_keep_going(static_cast<Func&&>(visitor), static_cast<elem_ref>(elem));

			// visitor not compatible with this particular type
			else
				return true;
		}

#endif

		template <typename Func, typename Array>
		static void do_for_each(Func&& visitor, Array&& arr) //
			noexcept(for_each_is_nothrow<Func&&, Array&&>::value)
		{
			static_assert(can_for_each_any<Func&&, Array&&>::value,
						  "TOML array for_each visitors must be invocable for at least one of the toml::node "
						  "types:" TOML_SA_NODE_TYPE_LIST);

			for (size_t i = 0; i < arr.size(); i++)
			{
				using node_ref = impl::copy_cvref<toml::node, Array&&>;
				static_assert(std::is_reference_v<node_ref>);

#if TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

#ifndef TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_ACKNOWLEDGED
				static_assert(impl::always_false<Func, Array, node_ref>, //
							  TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_MESSAGE);
#endif

				static_cast<node_ref>(static_cast<Array&&>(arr)[i])
					.visit(
						[&]([[maybe_unused]] auto&& elem) //
#if !TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA
						noexcept(for_each_is_nothrow_one<Func&&, Array&&, decltype(elem)>::value)
#endif
						{
							using elem_ref = impl::for_each_ref<decltype(elem), Array&&>;
							static_assert(std::is_reference_v<elem_ref>);

							// func(elem, i)
							if constexpr (std::is_invocable_v<Func&&, elem_ref, size_t>)
							{
								static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem), i);
							}

							// func(i, elem)
							else if constexpr (std::is_invocable_v<Func&&, size_t, elem_ref>)
							{
								static_cast<Func&&>(visitor)(i, static_cast<elem_ref>(elem));
							}

							// func(elem)
							else if constexpr (std::is_invocable_v<Func&&, elem_ref>)
							{
								static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem));
							}
						});

#else
				const auto keep_going =
					static_cast<node_ref>(static_cast<Array&&>(arr)[i])
						.visit(
							[&](auto&& elem) //
// left out for MSVC's "legacy lambda processor", which can't compile it
#if !TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA
							noexcept(for_each_is_nothrow_one<Func&&, Array&&, decltype(elem)>::value)
#endif
							{
								return array::for_each_visit<Array&&>(static_cast<Func&&>(visitor),
																	  static_cast<decltype(elem)>(elem),
																	  i);
							});

				if (!keep_going)
					return;
#endif
			}
		}

	  public:

		template <typename Func>
		array& for_each(Func&& visitor) & //
			noexcept(for_each_is_nothrow<Func&&, array&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		array&& for_each(Func&& visitor) && //
			noexcept(for_each_is_nothrow<Func&&, array&&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<array&&>(*this));
			return static_cast<array&&>(*this);
		}

		template <typename Func>
		const array& for_each(Func&& visitor) const& //
			noexcept(for_each_is_nothrow<Func&&, const array&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		const array&& for_each(Func&& visitor) const&& //
			noexcept(for_each_is_nothrow<Func&&, const array&&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<const array&&>(*this));
			return static_cast<const array&&>(*this);
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return elems_.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return elems_.size();
		}

		TOML_PURE_INLINE_GETTER
		size_t max_size() const noexcept
		{
			return elems_.max_size();
		}

		TOML_PURE_INLINE_GETTER
		size_t capacity() const noexcept
		{
			return elems_.capacity();
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void reserve(size_t new_capacity);

		TOML_EXPORTED_MEMBER_FUNCTION
		void shrink_to_fit();

		TOML_EXPORTED_MEMBER_FUNCTION
		void truncate(size_t new_size);

		template <typename ElemType>
		void resize(size_t new_size,
					ElemType&& default_init_val,
					value_flags default_init_flags = preserve_source_value_flags)
		{
			static_assert(!is_node_view<ElemType>,
						  "The default element type argument of array::resize() must not be toml::node_view");

			if (new_size > elems_.size())
				insert(cend(), new_size - elems_.size(), static_cast<ElemType&&>(default_init_val), default_init_flags);
			else
				truncate(new_size);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator pos) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator first, const_iterator last) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		array& flatten() &;

		array&& flatten() &&
		{
			return static_cast<toml::array&&>(this->flatten());
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& prune(bool recursive = true) & noexcept;

		array&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::array&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void pop_back() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		template <typename ElemType>
		iterator insert(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			if (impl::is_null_node_view(val))
				return end();

			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::make_node(static_cast<ElemType&&>(val), flags)) };
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						size_t count,
						ElemType&& val,
						value_flags flags = preserve_source_value_flags)
		{
			if (impl::is_null_node_view(val))
				return end();

			if (!count)
				return iterator{ to_vector_iterator(const_vector_iterator{ pos }) };
			if (count == 1u)
				return insert(pos, static_cast<ElemType&&>(val), flags);

			vector_type new_elems;
			new_elems.reserve(count);
			for (size_t i = 1u; i < count; i++)
				new_elems.push_back(impl::make_node(val, flags));
			new_elems.push_back(impl::make_node(static_cast<ElemType&&>(val), flags));
			return iterator{ insert_range_at(const_vector_iterator{ pos }, std::move(new_elems)) };
		}

		TOML_CONSTRAINED_TEMPLATE(!std::is_integral_v<Iter>, typename Iter)
		iterator insert(const_iterator pos, Iter first, Iter last, value_flags flags = preserve_source_value_flags)
		{
			const auto distance = std::distance(first, last);
			if (distance <= 0)
				return iterator{ to_vector_iterator(const_vector_iterator{ pos }) };

			vector_type new_elems;
			new_elems.reserve(static_cast<size_t>(distance));
			for (auto it = first; it != last; ++it)
			{
				if (impl::is_null_node_view(*it))
					continue;

				new_elems.push_back(impl::make_node(*it, flags));
			}
			return iterator{ insert_range_at(const_vector_iterator{ pos }, std::move(new_elems)) };
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						std::initializer_list<ElemType> ilist,
						value_flags flags = preserve_source_value_flags)
		{
			return insert(pos, ilist.begin(), ilist.end(), flags);
		}

		template <typename ElemType = void, typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			using elem_type = impl::emplaced_type_or<ElemType, Args&&...>;

			using type = impl::remove_cvref<impl::unwrap_node<elem_type>>;
			static_assert(
				impl::is_native<type> || impl::is_one_of<type, table, array>,
				"The template type argument of array::emplace() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::node_ptr{ new impl::wrap_node<type>{ static_cast<Args&&>(args)... } }) };
		}

		template <typename ElemType>
		iterator replace(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			TOML_ASSERT(pos >= cbegin() && pos < cend());

			if (impl::is_null_node_view(val))
				return end();

			const auto it = to_vector_iterator(const_vector_iterator{ pos });
			*it			  = impl::make_node(static_cast<ElemType&&>(val), flags);
			return iterator{ it };
		}

		template <typename ElemType>
		void push_back(ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			if (impl::is_null_node_view(val))
				return;

			insert_at_back(impl::make_node(static_cast<ElemType&&>(val), flags));
		}

		template <typename ElemType = void, typename... Args>
		decltype(auto) emplace_back(Args&&... args)
		{
			using elem_type = impl::emplaced_type_or<ElemType, Args&&...>;

			static constexpr auto moving_node_ptr = std::is_same_v<elem_type, impl::node_ptr> //
												 && sizeof...(Args) == 1u					  //
												 && impl::first_is_same<impl::node_ptr&&, Args&&...>;

			using unwrapped_type = impl::remove_cvref<impl::unwrap_node<elem_type>>;

			static_assert(
				moving_node_ptr										  //
					|| impl::is_native<unwrapped_type>				  //
					|| impl::is_one_of<unwrapped_type, table, array>, //
				"The template type argument of array::emplace_back() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (moving_node_ptr)
			{
				insert_at_back(static_cast<Args&&>(args)...);
				return *elems_.back();
			}
			else
			{
				auto ptr = new impl::wrap_node<unwrapped_type>{ static_cast<Args&&>(args)... };
				insert_at_back(impl::node_ptr{ ptr });
				return *ptr;
			}
		}

	  private:

		TOML_NODISCARD
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const array&, const array&) noexcept;

		template <typename T>
		TOML_NODISCARD
		static bool equal_to_container(const array& lhs, const T& rhs) noexcept
		{
			using element_type = std::remove_const_t<typename T::value_type>;
			static_assert(
				impl::is_losslessly_convertible_to_native<element_type>,
				"Container element type must be losslessly convertible to one of the native TOML value types");

			if (lhs.size() != rhs.size())
				return false;

			size_t i{};
			for (const auto& list_elem : rhs)
			{
				const auto elem = lhs.get_as<impl::native_type_of<element_type>>(i++);
				if (!elem)
					return false;

				if constexpr (impl::is_string_like<element_type>)
				{
					if (*elem != impl::string_like_to_view(list_elem))
						return false;
				}
				else if (*elem != list_elem)
					return false;
			}

			return true;
		}

	  public:

		TOML_NODISCARD
		friend bool operator==(const array& lhs, const array& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const array& lhs, const array& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, std::initializer_list<T> rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, std::initializer_list<T>, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, const std::vector<T>& rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>);

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const array& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/key.hpp  **********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class key
	{
	  private:
		std::string key_;
		source_region source_{};

	  public:

		TOML_NODISCARD_CTOR
		key() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, source_region&& src = {}) //
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, const source_region& src) //
			: key_{ k },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, source_region&& src = {}) noexcept //
			: key_{ std::move(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, const source_region& src) noexcept //
			: key_{ std::move(k) },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, source_region&& src = {}) //
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, const source_region& src) //
			: key_{ k },
			  source_{ src }
		{}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, source_region&& src = {}) //
			: key_{ impl::narrow(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, const source_region& src) //
			: key_{ impl::narrow(k) },
			  source_{ src }
		{}

#endif

		TOML_PURE_INLINE_GETTER
		std::string_view str() const noexcept
		{
			return std::string_view{ key_ };
		}

		TOML_PURE_INLINE_GETTER
		/*implicit*/ operator std::string_view() const noexcept
		{
			return str();
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return key_.empty();
		}

		TOML_PURE_INLINE_GETTER
		const char* data() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		size_t length() const noexcept
		{
			return key_.length();
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ >= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ == rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ != rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ < rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ <= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ > rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ >= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs >= rhs.key_;
		}

		using const_iterator = const char*;

		using iterator = const_iterator;

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return key_.data() + key_.length();
		}

		friend std::ostream& operator<<(std::ostream& lhs, const key& rhs)
		{
			impl::print_to_stream(lhs, rhs.key_);
			return lhs;
		}
	};

	template <typename T>
	inline constexpr bool is_key = std::is_same_v<impl::remove_cvref<T>, toml::key>;

	template <typename T>
	inline constexpr bool is_key_or_convertible = is_key<T> //
											   || impl::is_constructible_or_convertible<toml::key, T>;
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/std_map.hpp  ******************************************************************************************

TOML_DISABLE_WARNINGS;
#include <map>
#include <iterator>
TOML_ENABLE_WARNINGS;

//********  impl/table.hpp  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	struct table_proxy_pair
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const toml::key& first; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
		value_type& second;		// NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
	};

	template <bool IsConst>
	class table_iterator
	{
	  private:
		template <bool>
		friend class table_iterator;

		using proxy_type		   = table_proxy_pair<IsConst>;
		using mutable_map_iterator = std::map<toml::key, node_ptr, std::less<>>::iterator;
		using const_map_iterator   = std::map<toml::key, node_ptr, std::less<>>::const_iterator;
		using map_iterator		   = std::conditional_t<IsConst, const_map_iterator, mutable_map_iterator>;

		map_iterator iter_;
		alignas(proxy_type) mutable unsigned char proxy_[sizeof(proxy_type)];
		mutable bool proxy_instantiated_ = false; // unused, kept for the layout

		TOML_NODISCARD
		proxy_type* get_proxy() const noexcept
		{
			// rebuilt on every call: insert_or_assign() replaces a node without invalidating iterators
			return ::new (static_cast<void*>(proxy_)) proxy_type{ iter_->first, *iter_->second.get() };
		}

	  public:
		using value_type		= proxy_type;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= typename std::iterator_traits<map_iterator>::difference_type;
		using iterator_category = typename std::iterator_traits<map_iterator>::iterator_category;

		TOML_NODISCARD_CTOR
		table_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit table_iterator(mutable_map_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit table_iterator(const_map_iterator iter) noexcept //
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator<false>& other) noexcept //
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator& other) noexcept //
			: iter_{ other.iter_ }
		{}

		// NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
		table_iterator& operator=(const table_iterator& rhs) noexcept
		{
			iter_ = rhs.iter_;
			return *this;
		}

		table_iterator& operator++() noexcept // ++pre
		{
			++iter_;
			return *this;
		}

		table_iterator operator++(int) noexcept // post++
		{
			table_iterator out{ iter_ };
			++iter_;
			return out;
		}

		table_iterator& operator--() noexcept // --pre
		{
			--iter_;
			return *this;
		}

		table_iterator operator--(int) noexcept // post--
		{
			table_iterator out{ iter_ };
			--iter_;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const map_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const const_map_iterator() const noexcept
		{
			return iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}
	};

	struct table_init_pair
	{
		mutable toml::key key;
		mutable node_ptr value;

		template <typename K, typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(K&& k, V&& v, value_flags flags = preserve_source_value_flags) //
			: key{ static_cast<K&&>(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	using table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<false>);

	using const_table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<true>);

	class TOML_EXPORTED_CLASS table : public node
	{
	  private:

		using map_type			 = std::map<toml::key, impl::node_ptr, std::less<>>;
		using map_pair			 = std::pair<const toml::key, impl::node_ptr>;
		using map_iterator		 = typename map_type::iterator;
		using const_map_iterator = typename map_type::const_iterator;
		map_type map_;

		bool inline_ = false;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const impl::table_init_pair*, const impl::table_init_pair*);

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator get_lower_bound(std::string_view) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator, const_map_iterator) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator insert_with_hint(toml::const_table_iterator, key&&, impl::node_ptr&&);

		template <typename KeyType>
		TOML_NODISCARD
		static decltype(auto) narrow_key(KeyType&& key)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Wide-character keys are only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled");

#if TOML_ENABLE_WINDOWS_COMPAT
			if constexpr (impl::is_wide_string<KeyType>)
				return impl::narrow(static_cast<KeyType&&>(key));
			else
#endif
				return static_cast<KeyType&&>(key);
		}

		template <typename KeyType, typename MakeNode>
		std::pair<map_iterator, bool> insert_if_absent(KeyType&& key, MakeNode&& make_node)
		{
			const auto key_view = std::string_view{ key };
			const auto ipos		= get_lower_bound(key_view);
			if (ipos != map_.end() && ipos->first == key_view)
				return { ipos, false };

			return { insert_with_hint(const_iterator{ ipos }, toml::key{ static_cast<KeyType&&>(key) }, make_node()),
					 true };
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const table&, const table&) noexcept;

		template <typename Func, typename Table, typename T>
		using can_for_each = std::disjunction<std::is_invocable<Func, const key&, impl::for_each_ref<T, Table>>, //
											  std::is_invocable<Func, impl::for_each_ref<T, Table>>>;

		template <typename Func, typename Table, typename T>
		using can_for_each_nothrow = std::conditional_t<
			// first form
			std::is_invocable_v<Func, const key&, impl::for_each_ref<T, Table>>,
			std::is_nothrow_invocable<Func, const key&, impl::for_each_ref<T, Table>>,
			std::conditional_t<
				// second form
				std::is_invocable_v<Func, impl::for_each_ref<T, Table>>,
				std::is_nothrow_invocable<Func, impl::for_each_ref<T, Table>>,
				std::false_type>>;

		template <typename Func, typename Table>
		using can_for_each_any = std::disjunction<can_for_each<Func, Table, table>,
												  can_for_each<Func, Table, array>,
												  can_for_each<Func, Table, std::string>,
												  can_for_each<Func, Table, int64_t>,
												  can_for_each<Func, Table, double>,
												  can_for_each<Func, Table, bool>,
												  can_for_each<Func, Table, date>,
												  can_for_each<Func, Table, time>,
												  can_for_each<Func, Table, date_time>>;

		template <typename Func, typename Table, typename T>
		using for_each_is_nothrow_one = std::disjunction<std::negation<can_for_each<Func, Table, T>>, //
														 can_for_each_nothrow<Func, Table, T>>;

		template <typename Func, typename Table>
		using for_each_is_nothrow = std::conjunction<for_each_is_nothrow_one<Func, Table, table>,
													 for_each_is_nothrow_one<Func, Table, array>,
													 for_each_is_nothrow_one<Func, Table, std::string>,
													 for_each_is_nothrow_one<Func, Table, int64_t>,
													 for_each_is_nothrow_one<Func, Table, double>,
													 for_each_is_nothrow_one<Func, Table, bool>,
													 for_each_is_nothrow_one<Func, Table, date>,
													 for_each_is_nothrow_one<Func, Table, time>,
													 for_each_is_nothrow_one<Func, Table, date_time>>;

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

		// false stops the loop
		template <typename Table, typename Func, typename Value>
		TOML_NODISCARD
		static bool for_each_visit([[maybe_unused]] Func&& visitor,
								   [[maybe_unused]] const key& k,
								   [[maybe_unused]] Value&& v) //
			noexcept(for_each_is_nothrow_one<Func&&, Table, Value&&>::value)
		{
			using value_ref = impl::for_each_ref<Value&&, Table>;
			static_assert(std::is_reference_v<value_ref>);

			// func(key, val)
			if constexpr (std::is_invocable_v<Func&&, const key&, value_ref>)
				return impl::for_each_keep_going(static_cast<Func&&>(visitor), k, static_cast<value_ref>(v));

			// func(val)
			else if constexpr (std::is_invocable_v<Func&&, value_ref>)
				return impl::for_each_keep_going(static_cast<Func&&>(visitor), static_cast<value_ref>(v));

			// visitor not compatible with this particular type
			else
				return true;
		}

#endif

		template <typename Func, typename Table>
		static void do_for_each(Func&& visitor, Table&& tbl) //
			noexcept(for_each_is_nothrow<Func&&, Table&&>::value)
		{
			static_assert(can_for_each_any<Func&&, Table&&>::value,
						  "TOML table for_each visitors must be invocable for at least one of the toml::node "
						  "types:" TOML_SA_NODE_TYPE_LIST);

			using kvp_type = impl::copy_cv<map_pair, std::remove_reference_t<Table>>;

			for (kvp_type& kvp : tbl.map_)
			{
				using node_ref = impl::copy_cvref<toml::node, Table&&>;
				static_assert(std::is_reference_v<node_ref>);

#if TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

#ifndef TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_ACKNOWLEDGED
				static_assert(impl::always_false<Func, Table, kvp_type, node_ref>, //
							  TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_MESSAGE);
#endif

				static_cast<node_ref>(*kvp.second)
					.visit(
						[&]([[maybe_unused]] auto&& v) //
#if !TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA
						noexcept(for_each_is_nothrow_one<Func&&, Table&&, decltype(v)>::value)
#endif
						{
							using value_ref = impl::for_each_ref<decltype(v), Table&&>;
							static_assert(std::is_reference_v<value_ref>);

							// func(key, val)
							if constexpr (std::is_invocable_v<Func&&, const key&, value_ref>)
							{
								static_cast<Func&&>(visitor)(static_cast<const key&>(kvp.first),
															 static_cast<value_ref>(v));
							}

							// func(val)
							else if constexpr (std::is_invocable_v<Func&&, value_ref>)
							{
								static_cast<Func&&>(visitor)(static_cast<value_ref>(v));
							}
						});

#else
				const auto keep_going = static_cast<node_ref>(*kvp.second)
											.visit(
												[&](auto&& v) //
// left out for MSVC's "legacy lambda processor", which can't compile it
#if !TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA
												noexcept(for_each_is_nothrow_one<Func&&, Table&&, decltype(v)>::value)
#endif
												{
													return table::for_each_visit<Table&&>(static_cast<Func&&>(visitor),
																						  kvp.first,
																						  static_cast<decltype(v)>(v));
												});

				if (!keep_going)
					return;
#endif
			}
		}

	  public:

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		~table() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const table&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(table&& other) noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit table(std::initializer_list<impl::table_init_pair> kvps) //
			: table(kvps.begin(), kvps.end())
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(const table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(table&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::table;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		TOML_EXTERNAL_LINKAGE
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		using node::is_homogeneous;
		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		TOML_PURE_INLINE_GETTER
		bool is_inline() const noexcept
		{
			return inline_;
		}

		void is_inline(bool val) noexcept
		{
			inline_ = val;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		node* get(std::string_view key) noexcept;

		TOML_PURE_INLINE_GETTER
		const node* get(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).get(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node* get(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get(impl::narrow(key));
		}

		TOML_NODISCARD
		const node* get(std::wstring_view key) const
		{
			return const_cast<table&>(*this).get(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		template <typename T>
		TOML_PURE_GETTER
		impl::wrap_node<T>* get_as(std::string_view key) noexcept
		{
			if (const auto n = get(key))
				return n->template as<T>();
			return nullptr;
		}

		template <typename T>
		TOML_PURE_GETTER
		const impl::wrap_node<T>* get_as(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		template <typename T>
		TOML_NODISCARD
		impl::wrap_node<T>* get_as(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get_as<T>(impl::narrow(key));
		}

		template <typename T>
		TOML_NODISCARD
		const impl::wrap_node<T>* get_as(std::wstring_view key) const
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(std::string_view key);

		TOML_NODISCARD
		const node& at(std::string_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node& at(std::wstring_view key)
		{
			return at(impl::narrow(key));
		}

		TOML_NODISCARD
		const node& at(std::wstring_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		using iterator = toml::table_iterator;

		using const_iterator = toml::const_table_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return iterator{ map_.begin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return iterator{ map_.end() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

		template <typename Func>
		table& for_each(Func&& visitor) & //
			noexcept(for_each_is_nothrow<Func&&, table&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		table&& for_each(Func&& visitor) && //
			noexcept(for_each_is_nothrow<Func&&, table&&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<table&&>(*this));
			return static_cast<table&&>(*this);
		}

		template <typename Func>
		const table& for_each(Func&& visitor) const& //
			noexcept(for_each_is_nothrow<Func&&, const table&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		const table&& for_each(Func&& visitor) const&& //
			noexcept(for_each_is_nothrow<Func&&, const table&&>::value)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<const table&&>(*this));
			return static_cast<const table&&>(*this);
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return map_.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return map_.size();
		}

		TOML_PURE_GETTER
		iterator lower_bound(std::string_view key) noexcept
		{
			return iterator{ get_lower_bound(key) };
		}

		TOML_PURE_GETTER
		const_iterator lower_bound(std::string_view key) const noexcept
		{
			return const_iterator{ const_cast<table&>(*this).get_lower_bound(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator lower_bound(std::wstring_view key)
		{
			if (empty())
				return end();

			return lower_bound(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator lower_bound(std::wstring_view key) const
		{
			return const_cast<table&>(*this).lower_bound(key);
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		iterator find(std::string_view key) noexcept;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		const_iterator find(std::string_view key) const noexcept;

		TOML_PURE_GETTER
		bool contains(std::string_view key) const noexcept
		{
			return get(key) != nullptr;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator find(std::wstring_view key)
		{
			if (empty())
				return end();

			return find(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator find(std::wstring_view key) const
		{
			return const_cast<table&>(*this).find(key);
		}

		TOML_NODISCARD
		bool contains(std::wstring_view key) const
		{
			return get(key) != nullptr;
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		iterator erase(iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator begin, const_iterator end) noexcept
		{
			return iterator{ erase(const_map_iterator{ begin }, const_map_iterator{ end }) };
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		size_t erase(std::string_view key) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		size_t erase(std::wstring_view key)
		{
			if (empty())
				return size_t{};

			return erase(impl::narrow(key));
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		table& prune(bool recursive = true) & noexcept;

		table&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::table&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType = void,
								  typename KeyType,
								  typename... ValueArgs)
		iterator emplace_hint(const_iterator hint, KeyType&& key, ValueArgs&&... args)
		{
			using value_type = impl::emplaced_type_or<ValueType, ValueArgs&&...>;

			static constexpr auto moving_node_ptr = std::is_same_v<value_type, impl::node_ptr> //
												 && sizeof...(ValueArgs) == 1u				   //
												 && impl::first_is_same<impl::node_ptr&&, ValueArgs&&...>;
			using unwrapped_type				  = impl::remove_cvref<impl::unwrap_node<value_type>>;

			static_assert(moving_node_ptr										//
							  || impl::is_native<unwrapped_type>				//
							  || impl::is_one_of<unwrapped_type, table, array>, //
						  "The template type argument of table::emplace_hint() must be one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			const auto ipos = insert_with_hint(hint, toml::key{ narrow_key(static_cast<KeyType&&>(key)) }, nullptr);

			// if second is nullptr then we successfully claimed the key and inserted the empty sentinel,
			// so now we have to construct the actual value
			if (!ipos->second)
			{
				if constexpr (moving_node_ptr)
					ipos->second = std::move(static_cast<ValueArgs&&>(args)...);
				else
				{
#if TOML_COMPILER_HAS_EXCEPTIONS
					try
					{
#endif
						ipos->second.reset(new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... });
#if TOML_COMPILER_HAS_EXCEPTIONS
					}
					catch (...)
					{
						erase(const_map_iterator{ ipos }); // strong exception guarantee
						throw;
					}
#endif
				}
			}
			return iterator{ ipos };
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert(KeyType&& key,
										 ValueType&& val,
										 value_flags flags = preserve_source_value_flags)
		{
			if (impl::is_null_node_view(val))
				return { end(), false };

			const auto [ipos, inserted] =
				insert_if_absent(narrow_key(static_cast<KeyType&&>(key)),
								 [&]() { return impl::make_node(static_cast<ValueType&&>(val), flags); });
			return { iterator{ ipos }, inserted };
		}

		TOML_CONSTRAINED_TEMPLATE((!is_key_or_convertible<Iter> && !impl::is_wide_string<Iter>), typename Iter)
		void insert(Iter begin, Iter end, value_flags flags = preserve_source_value_flags)
		{
			for (auto it = begin; it != end; ++it)
			{
				if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
					insert(std::move((*it).first), std::move((*it).second), flags);
				else
					insert((*it).first, (*it).second, flags);
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert_or_assign(KeyType&& key,
												   ValueType&& val,
												   value_flags flags = preserve_source_value_flags)
		{
			if (impl::is_null_node_view(val))
				return { end(), false };

			const auto make_node		= [&]() { return impl::make_node(static_cast<ValueType&&>(val), flags); };
			const auto [ipos, inserted] = insert_if_absent(narrow_key(static_cast<KeyType&&>(key)), make_node);
			if (!inserted)
				ipos->second = make_node();
			return { iterator{ ipos }, inserted };
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType = void,
								  typename KeyType,
								  typename... ValueArgs)
		std::pair<iterator, bool> emplace(KeyType&& key, ValueArgs&&... args)
		{
			using value_type	 = impl::emplaced_type_or<ValueType, ValueArgs&&...>;
			using unwrapped_type = impl::remove_cvref<impl::unwrap_node<value_type>>;
			static_assert((impl::is_native<unwrapped_type> || impl::is_one_of<unwrapped_type, table, array>),
						  "The template type argument of table::emplace() must be one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			const auto [ipos, inserted] = insert_if_absent(
				narrow_key(static_cast<KeyType&&>(key)),
				[&]()
				{ return impl::node_ptr{ new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... } }; });
			return { iterator{ ipos }, inserted };
		}

		using node::operator[]; // inherit operator[toml::path]

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key)
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const
		{
			return node_view<const node>{ get(key) };
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		friend bool operator==(const table& lhs, const table& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const table& lhs, const table& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const table& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/unicode.hpp  ******************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_CONST_GETTER
	constexpr bool is_string_delimiter(char32_t c) noexcept
	{
		return c == U'"' || c == U'\'';
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_letter(char32_t c) noexcept
	{
		return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
	}

	TOML_CONST_GETTER
	constexpr bool is_binary_digit(char32_t c) noexcept
	{
		return c == U'0' || c == U'1';
	}

	TOML_CONST_GETTER
	constexpr bool is_octal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'7';
	}

	TOML_CONST_GETTER
	constexpr bool is_decimal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'9';
	}

	TOML_CONST_GETTER
	constexpr bool is_hexadecimal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'f' && (1ull << (static_cast<uint_least64_t>(c) - 0x30u)) & 0x7E0000007E03FFull;
	}

	template <typename T>
	TOML_CONST_GETTER
	constexpr uint_least32_t hex_to_dec(const T c) noexcept
	{
		if constexpr (std::is_same_v<T, uint_least32_t>)
			return c >= 0x41u					 // >= 'A'
					 ? 10u + (c | 0x20u) - 0x61u // - 'a'
					 : c - 0x30u				 // - '0'
				;
		else
			return hex_to_dec(static_cast<uint_least32_t>(c));
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		return c == U'\t' || c == U' ';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		if (c < U'\u00A0')
			return false;

		return c == U'\u00A0' || c == U'\u1680' || (c >= U'\u2000' && c <= U'\u200A') || c == U'\u202F'
			|| c == U'\u205F' || c == U'\u3000';
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return c >= U'\n' && c <= U'\r';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return c == U'\u0085' || c == U'\u2028' || c == U'\u2029';
	}

	TOML_CONST_GETTER
	constexpr bool is_horizontal_whitespace(char32_t c) noexcept
	{
		return is_ascii_horizontal_whitespace(c) || is_non_ascii_horizontal_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_vertical_whitespace(char32_t c) noexcept
	{
		return is_ascii_vertical_whitespace(c) || is_non_ascii_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_whitespace(char32_t c) noexcept
	{
		return is_horizontal_whitespace(c) || is_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_default_ignorable(char32_t c) noexcept
	{
		if (c < U'\u00AD')
			return false;

		return c == U'\u00AD' || c == U'\u034F' || c == U'\u061C' || (c >= U'\u115F' && c <= U'\u1160')
			|| (c >= U'\u17B4' && c <= U'\u17B5') || (c >= U'\u180B' && c <= U'\u180F')
			|| (c >= U'\u200B' && c <= U'\u200F') || (c >= U'\u202A' && c <= U'\u202E')
			|| (c >= U'\u2060' && c <= U'\u206F') || c == U'\u3164' || (c >= U'\uFE00' && c <= U'\uFE0F')
			|| c == U'\uFEFF' || c == U'\uFFA0' || (c >= U'\uFFF0' && c <= U'\uFFF8')
			|| (c >= U'\U0001BCA0' && c <= U'\U0001BCA3') || (c >= U'\U0001D173' && c <= U'\U0001D17A')
			|| (c >= U'\U000E0000' && c <= U'\U000E0FFF');
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_bare_key_character(char32_t c) noexcept
	{
		return is_ascii_letter(c) || is_decimal_digit(c) || c == U'-' || c == U'_'
#if TOML_LANG_UNRELEASED // toml/issues/644 ('+' in bare keys)
			|| c == U'+'
#endif
			;
	}

#if TOML_LANG_UNRELEASED // toml/pull/891 (unicode bare keys)

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_bare_key_character(char32_t c) noexcept
	{
		if (c < U'\u00B2')
			return false;

		return c == U'\u00B2' || c == U'\u00B3' || c == U'\u00B9' || (c >= U'\u00BC' && c <= U'\u00BE')
			|| (c >= U'\u00C0' && c <= U'\u00D6') || (c >= U'\u00D8' && c <= U'\u00F6')
			|| (c >= U'\u00F8' && c <= U'\u037D') || (c >= U'\u037F' && c <= U'\u1FFF')
			|| (c >= U'\u200C' && c <= U'\u200D') || (c >= U'\u203F' && c <= U'\u2040')
			|| (c >= U'\u2070' && c <= U'\u218F') || (c >= U'\u2460' && c <= U'\u24FF')
			|| (c >= U'\u2C00' && c <= U'\u2FEF') || (c >= U'\u3001' && c <= U'\uD7FF')
			|| (c >= U'\uF900' && c <= U'\uFDCF') || (c >= U'\uFDF0' && c <= U'\uFFFD')
			|| (c >= U'\U00010000' && c <= U'\U000EFFFF');
	}

#endif

	TOML_CONST_GETTER
	constexpr bool is_bare_key_character(char32_t c) noexcept
	{
		return is_ascii_bare_key_character(c)
#if TOML_LANG_UNRELEASED // toml/pull/891 (unicode bare keys)
			|| is_non_ascii_bare_key_character(c)
#endif
			;
	}

	TOML_CONST_GETTER
	constexpr bool is_value_terminator(char32_t c) noexcept
	{
		return is_whitespace(c) || c == U']' || c == U'}' || c == U',' || c == U'#';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char c) noexcept
	{
		return static_cast<unsigned char>(c) <= 0x1Fu || c == '\x7F';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char32_t c) noexcept
	{
		return c <= U'\u001F' || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_nontab_control_character(char32_t c) noexcept
	{
		return c <= U'\u0008' || (c >= U'\u000A' && c <= U'\u001F') || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_unicode_surrogate(char32_t c) noexcept
	{
		return c >= 0xD800u && c <= 0xDFFFu;
	}

	struct utf8_decoder
	{
		// utf8_decoder based on this: https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
		// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

		uint_least32_t state{};
		char32_t codepoint{};

		static constexpr uint8_t state_table[]{
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,
			1,	1,	1,	1,	1,	1,	1,	1,	1,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	7,	7,
			7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
			7,	7,	7,	8,	8,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
			2,	2,	2,	2,	2,	2,	2,	2,	10, 3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	4,	3,	3,	11, 6,	6,
			6,	5,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,
			0,	12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0,	12,
			12, 12, 12, 12, 0,	12, 0,	12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12,
			12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36, 12,
			36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12
		};

		TOML_PURE_INLINE_GETTER
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool has_codepoint() const noexcept
		{
			return state == uint_least32_t{};
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool needs_more_input() const noexcept
		{
			return !has_codepoint() && !error();
		}

		constexpr void operator()(uint8_t byte) noexcept
		{
			TOML_ASSERT_ASSUME(!error());

			const auto type = state_table[byte];

			codepoint = static_cast<char32_t>(has_codepoint() ? (uint_least32_t{ 255u } >> type) & byte
															  : (byte & uint_least32_t{ 63u })
																	| (static_cast<uint_least32_t>(codepoint) << 6));

			state = state_table[state + uint_least32_t{ 256u } + type];
		}

		TOML_ALWAYS_INLINE
		constexpr void operator()(char c) noexcept
		{
			operator()(static_cast<uint8_t>(c));
		}

		TOML_ALWAYS_INLINE
		constexpr void reset() noexcept
		{
			state = {};
		}
	};

	TOML_PURE_GETTER
	TOML_ATTR(nonnull)
	bool is_ascii(const char* str, size_t len) noexcept;
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/parse_error.hpp  **************************************************************************************

#if TOML_ENABLE_PARSER

//********  impl/std_except.hpp  ***************************************************************************************

TOML_DISABLE_WARNINGS;
#if TOML_EXCEPTIONS
#include <stdexcept>
#endif
TOML_ENABLE_WARNINGS;

//********  impl/parse_error.hpp  **************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

#if TOML_MSVC
// MSVC's std::runtime_error is implemented inline without a DLL annotation.
#pragma warning(disable : 4275)
#endif

#if TOML_DOXYGEN || !TOML_EXCEPTIONS
#define TOML_PARSE_ERROR_BASE
#else
#define TOML_PARSE_ERROR_BASE	 : public std::runtime_error
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	class TOML_EXPORTED_CLASS parse_error TOML_PARSE_ERROR_BASE
	{
	  private:
#if !TOML_EXCEPTIONS
		std::string description_;
#endif
		source_region source_;

	  public:
#if TOML_EXCEPTIONS

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, source_region&& src) noexcept //
			: std::runtime_error{ desc },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_region& src) noexcept //
			: parse_error{ desc, source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ desc, source_region{ position, position, path } }
		{}

#else

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, source_region&& src) noexcept //
			: description_{ std::move(desc) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_region& src) noexcept //
			: parse_error{ std::move(desc), source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ std::move(desc), source_region{ position, position, path } }
		{}

#endif

		TOML_NODISCARD
		std::string_view description() const noexcept
		{
#if TOML_EXCEPTIONS
			return std::string_view{ what() };
#else
			return description_;
#endif
		}

		TOML_NODISCARD
		const source_region& source() const noexcept
		{
			return source_;
		}

		friend std::ostream& operator<<(std::ostream& lhs, const parse_error& rhs)
		{
			impl::print_to_stream(lhs, rhs.description());
			impl::print_to_stream(lhs, "\n\t(error occurred at "sv);
			impl::print_to_stream(lhs, rhs.source());
			impl::print_to_stream(lhs, ")"sv);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#undef TOML_PARSE_ERROR_BASE

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/parse_result.hpp  *************************************************************************************

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_START(noex);

	class parse_result
	{
	  private:
		struct storage_t
		{
			static constexpr size_t size =
				(sizeof(toml::table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(toml::table));
			static constexpr size_t align =
				(alignof(toml::table) < alignof(parse_error) ? alignof(parse_error) : alignof(toml::table));

			alignas(align) unsigned char bytes[size];
		};

		storage_t storage_;
		bool err_;

		template <typename Type>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static Type* get_as(storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<Type*>(s.bytes));
		}

		template <typename Type>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static const Type* get_as(const storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<const Type*>(s.bytes));
		}

		void destroy() noexcept
		{
			if (err_)
				get_as<parse_error>(storage_)->~parse_error();
			else
				get_as<toml::table>(storage_)->~table();
		}

	  public:

		TOML_NODISCARD_CTOR
		parse_result() noexcept //
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::string{}, source_region{} };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(toml::table&& tbl) noexcept //
			: err_{ false }
		{
			::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(tbl) };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(parse_error&& err) noexcept //
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(err) };
		}

		TOML_NODISCARD_CTOR
		parse_result(parse_result&& res) noexcept //
			: err_{ res.err_ }
		{
			if (err_)
				::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(res).error() };
			else
				::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(res).table() };
		}

		parse_result& operator=(parse_result&& rhs) noexcept
		{
			if (err_ != rhs.err_)
			{
				destroy();
				err_ = rhs.err_;
				if (err_)
					::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(rhs).error() };
				else
					::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(rhs).table() };
			}
			else
			{
				if (err_)
					error() = std::move(rhs).error();
				else
					table() = std::move(rhs).table();
			}
			return *this;
		}

		~parse_result() noexcept
		{
			destroy();
		}

		TOML_NODISCARD
		bool succeeded() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		bool failed() const noexcept
		{
			return err_;
		}

		TOML_NODISCARD
		explicit operator bool() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		toml::table& table() & noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<toml::table>(storage_);
		}

		TOML_NODISCARD
		toml::table&& table() && noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return static_cast<toml::table&&>(*get_as<toml::table>(storage_));
		}

		TOML_NODISCARD
		const toml::table& table() const& noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<const toml::table>(storage_);
		}

		TOML_NODISCARD
		/* implicit */ operator toml::table&() noexcept
		{
			return table();
		}

		TOML_NODISCARD
		/* implicit */ operator toml::table&&() noexcept
		{
			return std::move(table());
		}

		TOML_NODISCARD
		/* implicit */ operator const toml::table&() const noexcept
		{
			return table();
		}

		TOML_NODISCARD
		parse_error& error() & noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<parse_error>(storage_);
		}

		TOML_NODISCARD
		parse_error&& error() && noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return static_cast<parse_error&&>(*get_as<parse_error>(storage_));
		}

		TOML_NODISCARD
		const parse_error& error() const& noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<const parse_error>(storage_);
		}

		TOML_NODISCARD
		explicit operator parse_error&() noexcept
		{
			return error();
		}

		TOML_NODISCARD
		explicit operator parse_error&&() noexcept
		{
			return std::move(error());
		}

		TOML_NODISCARD
		explicit operator const parse_error&() const noexcept
		{
			return error();
		}

		using iterator = table_iterator;

		using const_iterator = const_table_iterator;

		TOML_NODISCARD
		table_iterator begin() noexcept
		{
			return err_ ? table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator begin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator cbegin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cbegin();
		}

		TOML_NODISCARD
		table_iterator end() noexcept
		{
			return err_ ? table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator end() const noexcept
		{
			return err_ ? const_table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator cend() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cend();
		}

		TOML_NODISCARD
		node_view<node> at_path(std::string_view path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::string_view path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<node> at_path(const toml::path& path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(const toml::path& path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> at_path(std::wstring_view path)
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::wstring_view path) const
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#endif

		TOML_NODISCARD
		node_view<node> operator[](const toml::path& path) noexcept
		{
			return err_ ? node_view<node>{} : table()[path];
		}

		TOML_NODISCARD
		node_view<const node> operator[](const toml::path& path) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[path];
		}

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key)
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const parse_result& rhs)
		{
			return rhs.err_ ? (lhs << rhs.error()) : (lhs << rhs.table());
		}

#endif
	};

	TOML_ABI_NAMESPACE_END;
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

//********  impl/parser.hpp  *******************************************************************************************

#if TOML_ENABLE_PARSER

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_INLINE_GETTER
	constexpr std::string_view source_path_or_empty(const char* source_path) noexcept
	{
		return source_path ? std::string_view{ source_path } : std::string_view{};
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	parse_result parse(std::string_view doc, const char* source_path)
	{
		return parse(doc, impl::source_path_or_empty(source_path));
	}

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::string_view file_path);

#if TOML_HAS_CHAR8

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	parse_result parse(std::u8string_view doc, const char* source_path)
	{
		return parse(doc, impl::source_path_or_empty(source_path));
	}

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::u8string_view file_path);

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::wstring_view file_path);

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::wstring_view source_path);

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string && source_path);

	TOML_NODISCARD
	TOML_ALWAYS_INLINE
	parse_result parse(std::istream & doc, const char* source_path)
	{
		return parse(doc, impl::source_path_or_empty(source_path));
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	inline namespace literals
	{
		TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, lit_ex, lit_noex);

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		parse_result operator""_toml(const char* str, size_t len)
		{
			return parse(std::string_view{ str, len });
		}

#if TOML_HAS_CHAR8

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		parse_result operator""_toml(const char8_t* str, size_t len)
		{
			return parse(std::u8string_view{ str, len });
		}

#endif // TOML_HAS_CHAR8

		TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/formatter.hpp  ****************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	struct formatter_constants
	{
		format_flags mandatory_flags;
		format_flags ignored_flags;

		std::string_view float_pos_inf;
		std::string_view float_neg_inf;
		std::string_view float_nan;

		std::string_view bool_true;
		std::string_view bool_false;
	};

	struct formatter_config
	{
		format_flags flags;
		std::string_view indent;
	};

	enum class TOML_CLOSED_ENUM string_dialect : uint8_t
	{
		toml,
		json,
		yaml
	};

	class formatter_buffer;

	class TOML_EXPORTED_CLASS formatter
	{
	  private:
		friend class formatter_buffer;
		const node* source_;
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		const parse_result* result_;
#endif
		const formatter_constants* constants_;
		formatter_config config_;
		size_t indent_columns_;
		format_flags int_format_mask_;
		std::ostream* stream_; //
		int indent_;		   // these are set in attach()
		bool naked_newline_;   //

	  protected:
		TOML_PURE_INLINE_GETTER
		const node& source() const noexcept
		{
			return *source_;
		}

		TOML_PURE_INLINE_GETTER
		std::ostream& stream() const noexcept
		{
			return *stream_;
		}

		TOML_PURE_INLINE_GETTER
		format_flags effective_flags() const noexcept
		{
			return config_.flags;
		}

		TOML_PURE_INLINE_GETTER
		int indent() const noexcept
		{
			return indent_;
		}

		void indent(int level) noexcept
		{
			indent_ = level;
		}

		void increase_indent() noexcept
		{
			indent_++;
		}

		void decrease_indent() noexcept
		{
			indent_--;
		}

		class indent_scope
		{
		  private:
			formatter* formatter_;
			int previous_;

		  public:
			TOML_NODISCARD_CTOR
			explicit indent_scope(formatter& fmt, bool enabled = true) noexcept //
				: formatter_{ &fmt },
				  previous_{ fmt.indent_ }
			{
				if (enabled)
					fmt.indent_++;
			}

			// restores instead of decrementing, as the parser's scopes do (#288)
			~indent_scope() noexcept
			{
				formatter_->indent_ = previous_;
			}

			TOML_DELETE_DEFAULTS(indent_scope);
		};

		class attach_scope
		{
		  private:
			formatter* formatter_;

		  public:
			TOML_NODISCARD_CTOR
			explicit attach_scope(formatter& fmt, std::ostream& stream) noexcept //
				: formatter_{ &fmt }
			{
				fmt.attach(stream);
			}

			~attach_scope() noexcept
			{
				formatter_->detach();
			}

			TOML_DELETE_DEFAULTS(attach_scope);
		};

		TOML_PURE_INLINE_GETTER
		size_t indent_columns() const noexcept
		{
			return indent_columns_;
		}

		TOML_PURE_INLINE_GETTER
		bool indent_array_elements() const noexcept
		{
			return !!(config_.flags & format_flags::indent_array_elements);
		}

		TOML_PURE_INLINE_GETTER
		bool indent_sub_tables() const noexcept
		{
			return !!(config_.flags & format_flags::indent_sub_tables);
		}

		TOML_PURE_INLINE_GETTER
		bool literal_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_literal_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool multi_line_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_multi_line_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool real_tabs_in_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_real_tabs_in_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool unicode_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_unicode_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool terse_kvps() const noexcept
		{
			return !!(config_.flags & format_flags::terse_key_value_pairs);
		}

		TOML_PURE_INLINE_GETTER
		bool force_multi_line_arrays() const noexcept
		{
			return !!(config_.flags & format_flags::force_multi_line_arrays);
		}

		TOML_PURE_INLINE_GETTER
		bool relaxed_float_precision() const noexcept
		{
			return !!(config_.flags & format_flags::relaxed_float_precision);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void attach(std::ostream& stream) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void detach() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_newline(bool force = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_indent();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(char);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(std::string_view);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_string(std::string_view str,
						  bool allow_multi_line			= true,
						  bool allow_bare				= false,
						  bool allow_literal_whitespace = true);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_string(std::string_view str,
						  bool allow_multi_line,
						  bool allow_bare,
						  bool allow_literal_whitespace,
						  string_dialect dialect);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<int64_t>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<double>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<bool>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date_time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_value(const node&, node_type);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool dump_failed_parse_result();

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		formatter(const node*, const parse_result*, const formatter_constants&, const formatter_config&) noexcept;
	};
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/toml_formatter.hpp  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS toml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;
		std::vector<const key*> key_path_;
		bool pending_table_separator_ = {};

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_pending_table_separator();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const key&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_equals();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_inline(const toml::node&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_inline(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_key_path();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_sub_table(const key&, const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_table_array(const key&, const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		TOML_EXPORTED_CONSTANT
		static constexpr impl::formatter_constants constants = { format_flags::none, // mandatory
																 format_flags::none, // ignored
																 "inf"sv,
																 "-inf"sv,
																 "nan"sv,
																 "true"sv,
																 "false"sv };

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags				   //
													| format_flags::allow_literal_strings	   //
													| format_flags::allow_multi_line_strings   //
													| format_flags::allow_unicode_strings	   //
													| format_flags::allow_real_tabs_in_strings //
													| format_flags::allow_binary_integers	   //
													| format_flags::allow_octal_integers	   //
													| format_flags::allow_hexadecimal_integers //
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter& rhs)
		{
			const attach_scope attached{ rhs, lhs };
			rhs.key_path_.clear();
			rhs.pending_table_separator_ = false;
			rhs.print();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/json_formatter.hpp  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS json_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_json_string(std::string_view);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_node(const toml::node&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		TOML_EXPORTED_CONSTANT
		static constexpr impl::formatter_constants constants = {
			format_flags::quote_dates_and_times | format_flags::quote_infinities_and_nans,		 // mandatory
			format_flags::allow_literal_strings | format_flags::allow_multi_line_strings		 //
				| format_flags::allow_real_tabs_in_strings | format_flags::allow_binary_integers //
				| format_flags::allow_octal_integers | format_flags::allow_hexadecimal_integers, // ignored
			"Infinity"sv,
			"-Infinity"sv,
			"NaN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags			  //
													| format_flags::allow_unicode_strings //
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter& rhs)
		{
			const attach_scope attached{ rhs, lhs };
			rhs.print();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/yaml_formatter.hpp  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	class TOML_EXPORTED_CLASS yaml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_yaml_scalar(std::string_view);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_yaml_string(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_yaml_float(const value<double>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_node(const toml::node&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		TOML_EXPORTED_CONSTANT
		static constexpr impl::formatter_constants constants = {
			//
			format_flags::quote_dates_and_times | format_flags::indentation,				// mandatory
			format_flags::allow_multi_line_strings | format_flags::terse_key_value_pairs	//
				| format_flags::allow_binary_integers | format_flags::allow_octal_integers, // ignored
			".inf"sv,
			"-.inf"sv,
			".NAN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags			  //
													| format_flags::allow_literal_strings //
													| format_flags::allow_unicode_strings //
													| format_flags::allow_hexadecimal_integers;

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "  "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "  "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, yaml_formatter& rhs)
		{
			const attach_scope attached{ rhs, lhs };
			rhs.print();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, yaml_formatter&& rhs)
		{
			return lhs << rhs; // as lvalue
		}
	};
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

#if TOML_IMPLEMENTATION

//********  impl/std_string.inl  ***************************************************************************************

#if TOML_WINDOWS

#ifndef _WINDOWS_
#if TOML_INCLUDE_WINDOWS_H
#include <windows.h>
#else

extern "C" __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int CodePage,
																   unsigned long dwFlags,
																   const wchar_t* lpWideCharStr,
																   int cchWideChar,
																   char* lpMultiByteStr,
																   int cbMultiByte,
																   const char* lpDefaultChar,
																   int* lpUsedDefaultChar);

extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int CodePage,
																   unsigned long dwFlags,
																   const char* lpMultiByteStr,
																   int cbMultiByte,
																   wchar_t* lpWideCharStr,
																   int cchWideChar);

#endif // TOML_INCLUDE_WINDOWS_H
#endif // _WINDOWS_

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	// CP_UTF8
	inline constexpr unsigned int utf8_code_page = 65001u;

	TOML_EXTERNAL_LINKAGE
	std::string narrow(std::wstring_view str)
	{
		// the win32 conversions take an int length; a longer string converts to nothing
		const auto str_len = static_cast<int>(str.length());
		if (str.empty() || static_cast<size_t>(str_len) != str.length())
			return {};

		std::string s;
		const auto len = ::WideCharToMultiByte(utf8_code_page, 0, str.data(), str_len, nullptr, 0, nullptr, nullptr);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::WideCharToMultiByte(utf8_code_page, 0, str.data(), str_len, s.data(), len, nullptr, nullptr);
		}
		return s;
	}

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::string_view str)
	{
		const auto str_len = static_cast<int>(str.length());
		if (str.empty() || static_cast<size_t>(str_len) != str.length())
			return {};

		std::wstring s;
		const auto len = ::MultiByteToWideChar(utf8_code_page, 0, str.data(), str_len, nullptr, 0);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::MultiByteToWideChar(utf8_code_page, 0, str.data(), str_len, s.data(), len);
		}
		return s;
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::u8string_view str)
	{
		if (str.empty())
			return {};

		return widen(std::string_view{ reinterpret_cast<const char*>(str.data()), str.length() });
	}

#endif // TOML_HAS_CHAR8
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_WINDOWS

//********  impl/print_to_stream.inl  **********************************************************************************

TOML_DISABLE_WARNINGS;
#include <ostream>
#if TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_FLOAT_CHARCONV
#include <cstdio>
#include <cstdlib>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_wide_integer_to_stream(std::ostream & stream, T val, value_flags format, size_t min_digits)
	{
		static constexpr auto value_flags_mask =
			value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;
		format &= value_flags_mask;

		// zero for decimal
		unsigned bits_per_digit = {};
		if (val > T{})
		{
			switch (format)
			{
				case value_flags::format_as_binary: bits_per_digit = 1; break;
				case value_flags::format_as_octal: bits_per_digit = 3; break;
				case value_flags::format_as_hexadecimal: bits_per_digit = 4; break;
				default: break;
			}
		}

		using unsigned_type = std::make_unsigned_t<T>;
		auto magnitude		= static_cast<unsigned_type>(val);
		if (val < T{})
			magnitude = static_cast<unsigned_type>(unsigned_type{} - magnitude);

		static constexpr size_t max_padding = 16;
		char buf[max_padding + (sizeof(T) * CHAR_BIT)];
		char* const last = buf + sizeof(buf);
		char* digits	 = last;
		if (bits_per_digit)
		{
			const auto mask = static_cast<unsigned_type>((unsigned_type{ 1 } << bits_per_digit) - 1u);
			do
			{
				*--digits = "0123456789ABCDEF"[magnitude & mask];
				magnitude = static_cast<unsigned_type>(magnitude >> bits_per_digit);
			}
			while (magnitude);
		}
		else
		{
			do
			{
				*--digits = static_cast<char>('0' + (magnitude % 10u));
				magnitude = static_cast<unsigned_type>(magnitude / 10u);
			}
			while (magnitude);
		}

		const auto len = static_cast<size_t>(last - digits);
		size_t padding = min_digits > len ? min_digits - len : 0u;
		if (val < T{})
			impl::print_to_stream(stream, '-');
		for (; padding > max_padding; padding--)
			impl::print_to_stream(stream, '0');
		std::memset(digits - padding, '0', padding);
		impl::print_to_stream(stream, digits - padding, len + padding);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_integer_to_stream(std::ostream & stream, T val, value_flags format, size_t min_digits)
	{
		using wide_type = std::conditional_t<std::is_signed_v<T>, long long, unsigned long long>;
		print_wide_integer_to_stream(stream, static_cast<wide_type>(val), format, min_digits);
	}

	TOML_INTERNAL_LINKAGE
	char* write_decimal(char* out, uint_least32_t val, size_t min_digits) noexcept
	{
		char digits[10];
		size_t len = {};
		do
		{
			digits[len++] = static_cast<char>('0' + (val % 10u));
			val /= 10u;
		}
		while (val);
		for (; min_digits > len; min_digits--)
			*out++ = '0';
		while (len)
			*out++ = digits[--len];
		return out;
	}

	TOML_INTERNAL_LINKAGE
	char* write_date(char* out, toml::date val) noexcept
	{
		out	   = write_decimal(out, val.year, 4u);
		*out++ = '-';
		out	   = write_decimal(out, val.month, 2u);
		*out++ = '-';
		return write_decimal(out, val.day, 2u);
	}

	TOML_INTERNAL_LINKAGE
	char* write_time(char* out, toml::time val) noexcept
	{
		out	   = write_decimal(out, val.hour, 2u);
		*out++ = ':';
		out	   = write_decimal(out, val.minute, 2u);
		*out++ = ':';
		out	   = write_decimal(out, val.second, 2u);
		if (!val.nanosecond || val.nanosecond > 999999999u)
			return out;

		*out++		  = '.';
		auto ns		  = val.nanosecond;
		size_t digits = 9u;
		while (ns % 10u == 0u)
		{
			ns /= 10u;
			digits--;
		}
		return write_decimal(out, ns, digits);
	}

	TOML_INTERNAL_LINKAGE
	char* write_time_offset(char* out, toml::time_offset val) noexcept
	{
		if (!val.minutes)
		{
			*out++ = 'Z';
			return out;
		}

		auto mins = static_cast<int>(val.minutes);
		*out++	  = mins < 0 ? '-' : '+';
		if (mins < 0)
			mins = -mins;
		out	   = write_decimal(out, static_cast<uint_least32_t>(mins / 60), 2u);
		*out++ = ':';
		return write_decimal(out, static_cast<uint_least32_t>(mins % 60), 2u);
	}

	TOML_INTERNAL_LINKAGE
	char* write_date_time(char* out, const toml::date_time& val) noexcept
	{
		out	   = write_date(out, val.date);
		*out++ = 'T';
		out	   = write_time(out, val.time);
		if (val.offset)
			out = write_time_offset(out, *val.offset);
		return out;
	}

	struct float_digits
	{
		char digits[std::numeric_limits<double>::max_digits10];
		size_t count;
		int exponent;
	};

	// printf takes its decimal point from setlocale(), so non-digits are skipped
	TOML_INTERNAL_LINKAGE
	float_digits read_scientific(const char* text) noexcept
	{
		float_digits result{};
		bool in_exponent	   = {};
		bool negative_exponent = {};
		for (; *text; text++)
		{
			const char c = *text;
			if (c == 'e')
				in_exponent = true;
			else if (c == '-')
				negative_exponent = true;
			else if (c < '0' || c > '9')
				continue;
			else if (in_exponent)
				result.exponent = (result.exponent * 10) + (c - '0');
			else if (result.count < sizeof(result.digits))
				result.digits[result.count++] = c;
		}
		if (negative_exponent)
			result.exponent = -result.exponent;
		return result;
	}

	// with denormals-are-zero set, every subnormal compares equal to zero
	template <typename T>
	TOML_INTERNAL_LINKAGE
	bool same_bits(T a, T b) noexcept
	{
		using bits_type = std::conditional_t<sizeof(T) == sizeof(uint32_t), uint32_t, uint64_t>;
		static_assert(sizeof(T) == sizeof(bits_type));

		bits_type a_bits = {};
		bits_type b_bits = {};
		std::memcpy(&a_bits, &a, sizeof(T));
		std::memcpy(&b_bits, &b, sizeof(T));
		return a_bits == b_bits;
	}

#if TOML_FLOAT_CHARCONV

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits rounded_digits(T val, int precision) noexcept
	{
		char buf[33];
		*std::to_chars(buf, buf + sizeof(buf) - 1, val, std::chars_format::scientific, precision).ptr = '\0';
		return read_scientific(buf);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits shortest_digits(T val) noexcept
	{
		char buf[33];
		*std::to_chars(buf, buf + sizeof(buf) - 1, val, std::chars_format::scientific).ptr = '\0';
		return read_scientific(buf);
	}

#else

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits rounded_digits(T val, int precision) noexcept
	{
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.*e", precision, static_cast<double>(val)); // NOLINT(*-pro-type-vararg)
		return read_scientific(buf);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	T read_back(const float_digits& val) noexcept
	{
		// strtod takes its decimal point from setlocale(), so the digits go in without one
		char text[32];
		std::memcpy(text, val.digits, val.count);
		char* out	  = text + val.count;
		*out++		  = 'e';
		auto exponent = val.exponent - static_cast<int>(val.count - 1u);
		if (exponent < 0)
		{
			*out++	 = '-';
			exponent = -exponent;
		}
		*write_decimal(out, static_cast<uint_least32_t>(exponent), 1u) = '\0';

		if constexpr (std::is_same_v<T, float>)
			return std::strtof(text, nullptr);
		else
			return std::strtod(text, nullptr);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits shortest_digits(T val) noexcept
	{
		using limits = std::numeric_limits<T>;

		for (int precision = val < (limits::min)() ? 0 : limits::digits10 - 1;; precision++)
		{
			auto result = rounded_digits(val, precision);
			if (precision + 1 >= limits::max_digits10)
				return result;

			const auto back = read_back<T>(result);
			if (same_bits(back, val))
				return result;

			// the gap below a power of two is half the one above it, so past digits10 the nearest candidate can
			// fall outside while the next one up reads back
			if (precision >= limits::digits10 && back < val)
			{
				size_t i = result.count;
				while (i && result.digits[i - 1u] == '9')
					result.digits[--i] = '0';
				if (i)
					result.digits[i - 1u]++;
				else
				{
					result.digits[0] = '1';
					result.exponent++;
				}
				if (same_bits(read_back<T>(result), val))
					return result;
			}
		}
	}

#endif

	// floats print in fixed form inside this window, given here as values and as decimal exponents
	TOML_INTERNAL_LINKAGE
	constexpr double fixed_form_min = 1e-4;
	TOML_INTERNAL_LINKAGE
	constexpr double fixed_form_max = 1e16;
	TOML_INTERNAL_LINKAGE
	constexpr int fixed_form_min_exponent = -4;
	TOML_INTERNAL_LINKAGE
	constexpr int fixed_form_max_exponent = 16;

	// the significant digits of %g, which relaxed precision follows
	TOML_INTERNAL_LINKAGE
	constexpr int relaxed_digits = 6;

#if TOML_FLOAT_CHARCONV

	// val is positive and not zero
	TOML_INTERNAL_LINKAGE
	char* write_shortest_double(char* out, char* end, double val, bool point_before_exponent) noexcept
	{
		// the shortest text reads back, so comparing the value with the bounds matches comparing the text
		const bool fixed  = val >= fixed_form_min && val < fixed_form_max;
		char* const first = out;
		out = std::to_chars(first, end, val, fixed ? std::chars_format::fixed : std::chars_format::scientific).ptr;
		if (fixed)
		{
			if (!std::memchr(first, '.', static_cast<size_t>(out - first)))
			{
				*out++ = '.';
				*out++ = '0';
			}
			return out;
		}

		if (point_before_exponent && first[1] == 'e')
		{
			std::memmove(first + 3, first + 1, static_cast<size_t>(out - first - 1));
			first[1] = '.';
			first[2] = '0';
			out += 2;
		}
		return out;
	}

#endif

	TOML_INTERNAL_LINKAGE
	char* write_scientific(char* out, const float_digits& val, bool point_before_exponent) noexcept
	{
		*out++ = val.digits[0];
		if (val.count > 1u)
		{
			*out++ = '.';
			std::memcpy(out, val.digits + 1, val.count - 1u);
			out += val.count - 1u;
		}
		else if (point_before_exponent)
		{
			*out++ = '.';
			*out++ = '0';
		}
		*out++ = 'e';
		*out++ = val.exponent < 0 ? '-' : '+';
		return write_decimal(out, static_cast<uint_least32_t>(val.exponent < 0 ? -val.exponent : val.exponent), 2u);
	}

	TOML_INTERNAL_LINKAGE
	char* write_fixed(char* out, const float_digits& val) noexcept
	{
		if (val.exponent < 0)
		{
			*out++ = '0';
			*out++ = '.';
			for (int i = val.exponent + 1; i < 0; i++)
				*out++ = '0';
			std::memcpy(out, val.digits, val.count);
			return out + val.count;
		}

		const auto whole = static_cast<size_t>(val.exponent) + 1u;
		for (size_t i = 0; i < whole; i++)
			*out++ = i < val.count ? val.digits[i] : '0';
		*out++ = '.';
		if (whole >= val.count)
		{
			*out++ = '0';
			return out;
		}
		std::memcpy(out, val.digits + whole, val.count - whole);
		return out + (val.count - whole);
	}

	// python's repr() layout, or %g's when relaxed
	template <typename T, size_t N>
	TOML_INTERNAL_LINKAGE
	char* write_float(char (&buf)[N], T val, bool relaxed_precision, bool point_before_exponent) noexcept
	{
		static_assert(N >= 32u);

		char* out = buf;
		if (impl::signbit(val))
		{
			*out++ = '-';
			val	   = -val;
		}
		if (same_bits(val, T{}))
		{
			*out++ = '0';
			*out++ = '.';
			*out++ = '0';
			return out;
		}

#if TOML_FLOAT_CHARCONV
		// fixed form keeps integer digits the shortest text drops once the gap between values passes 2,
		// which happens below 1e16 for floats
		if constexpr (std::is_same_v<T, double>)
		{
			if (!relaxed_precision)
				return write_shortest_double(out, buf + N, val, point_before_exponent);
		}
#endif

#if !TOML_FLOAT_CHARCONV
		[[maybe_unused]] const impl::default_fenv fenv;
#endif
		auto decimal = shortest_digits(val);
		if (relaxed_precision && decimal.count > static_cast<size_t>(relaxed_digits))
			decimal = rounded_digits(val, relaxed_digits - 1);
		while (decimal.count > 1u && decimal.digits[decimal.count - 1u] == '0')
			decimal.count--;

		if (decimal.exponent < fixed_form_min_exponent
			|| decimal.exponent >= (relaxed_precision ? relaxed_digits : fixed_form_max_exponent))
			return write_scientific(out, decimal, point_before_exponent);

		return write_fixed(out, decimal);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_floating_point_to_stream(std::ostream & stream, T val, bool relaxed_precision)
	{
		switch (impl::fpclassify(static_cast<double>(val)))
		{
			case impl::fp_class::neg_inf: impl::print_to_stream(stream, "-inf"sv); break;

			case impl::fp_class::pos_inf: impl::print_to_stream(stream, "inf"sv); break;

			case impl::fp_class::nan: impl::print_to_stream(stream, "nan"sv); break;

			case impl::fp_class::ok:
			{
				char buf[32];
				const auto end = write_float(buf, val, relaxed_precision, false);
				impl::print_to_stream(stream, buf, static_cast<size_t>(end - buf));
			}
			break;

			default: TOML_UNREACHABLE;
		}
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(nonnull)
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const char* val, size_t len)
	{
		stream.write(val, static_cast<std::streamsize>(len));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, std::string_view val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const std::string& val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, char val)
	{
		stream.put(val);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed char val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed short val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed int val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed long val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream,
									   signed long long val,
									   value_flags format,
									   size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned char val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned short val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned int val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned long val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream,
									   unsigned long long val,
									   value_flags format,
									   size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, float val, value_flags, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, double val, value_flags, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, bool val)
	{
		print_to_stream(stream, val ? "true"sv : "false"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::date& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_date(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::time& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_time(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::time_offset& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_time_offset(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::date_time& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_date_time(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const source_position& val)
	{
		print_to_stream(stream, "line "sv);
		print_to_stream(stream, val.line);
		print_to_stream(stream, ", column "sv);
		print_to_stream(stream, val.column);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const source_region& val)
	{
		print_to_stream(stream, val.begin);
		if (val.begin != val.end)
		{
			print_to_stream(stream, " to "sv);
			print_to_stream(stream, val.end);
		}
		if (val.path)
		{
			print_to_stream(stream, " of '"sv);
			print_to_stream(stream, *val.path);
			print_to_stream(stream, '\'');
		}
	}

#if TOML_ENABLE_FORMATTERS

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const array& arr)
	{
		stream << toml_formatter{ arr };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const table& tbl)
	{
		stream << toml_formatter{ tbl };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<std::string>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<int64_t>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<double>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<bool>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<date>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<time>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<date_time>& val)
	{
		stream << toml_formatter{ val };
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/node.inl  *********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node::node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::~node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::node(node && other) noexcept //
		: source_{ std::exchange(other.source_, {}) }
	{}

	TOML_EXTERNAL_LINKAGE
	node::node(const node& /*other*/) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(const node& rhs) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577

		if (&rhs == this)
			return *this;

		source_ = {};
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(node&& rhs) noexcept
	{
		if (&rhs != this)
			source_ = std::exchange(rhs.source_, {});
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::string_view path) noexcept
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::string_view path) const noexcept
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(const path& p) noexcept
	{
		return toml::at_path(*this, p);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(const path& p) const noexcept
	{
		return toml::at_path(*this, p);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::wstring_view path)
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::wstring_view path) const
	{
		return toml::at_path(*this, path);
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::operator[](const path& p) noexcept
	{
		return toml::at_path(*this, p);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::operator[](const path& p) const noexcept
	{
		return toml::at_path(*this, p);
	}
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV node_deep_equality(const node* lhs, const node* rhs) noexcept
	{
		// both same or both null
		if (lhs == rhs)
			return true;

		// lhs null != rhs null or different types
		if ((!lhs != !rhs) || lhs->type() != rhs->type())
			return false;

		return lhs->visit(equal_to_same_type{ rhs });
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/at_path.inl  ******************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	// reads the array indexer starting at path[pos] and moves pos past it
	TOML_INTERNAL_LINKAGE
	bool parse_path_array_indexer(std::string_view path, size_t& pos, size_t& index) noexcept
	{
		static constexpr auto whitespace = " \t"sv;
		const auto end					 = path.length();

		// find first digit in index
		const auto index_start = path.find_first_not_of(whitespace, pos + 1u);
		if (index_start == std::string_view::npos || path[index_start] < '0' || path[index_start] > '9')
			return false;

		// find end of index (first non-digit character)
		const auto index_end = impl::min(path.find_first_not_of("0123456789"sv, index_start), end);

		// move pos to after indexer (char after closing ']' or permissively EOL/subkey '.'/next opening '[')
		pos = impl::min(path.find_first_not_of(whitespace, index_end), end);
		if (pos < end)
		{
			if (path[pos] == ']')
				pos++;
			else if (path[pos] != '.' && path[pos] != '[')
				return false;
		}

		static constexpr auto max_index = static_cast<size_t>(-1);
		index							= {};
		for (const auto digit : path.substr(index_start, index_end - index_start))
		{
			const auto value = static_cast<size_t>(digit - '0');
			if (index > (max_index - value) / 10u)
				return false;
			index = (index * 10u) + value;
		}
		return true;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV parse_path(const std::string_view path,
								  void* const data,
								  const parse_path_callback<std::string_view> on_key,
								  const parse_path_callback<size_t> on_index)
	{
		// a blank string is a valid path; it's just one component representing the "" key
		if (path.empty())
			return on_key(data, ""sv);

		enum class component
		{
			dot,
			array_indexer,
			key
		};

		size_t pos	   = 0;
		const auto end = path.length();
		auto prev	   = component::dot; // invisible root 'dot'

		while (pos < end)
		{
			// start of an array indexer
			if (path[pos] == '[')
			{
				size_t index = {};
				if (!TOML_ANON_NAMESPACE::parse_path_array_indexer(path, pos, index))
					return false;

				prev = component::array_indexer;

				if (!on_index(data, index))
					return false;
			}

			// start of a new table child
			else if (path[pos] == '.')
			{
				// a dot immediately following another dot (or at the beginning of the string) is as if we'd asked
				// for an empty child in between, e.g.
				//
				//     foo..bar
				//
				// is equivalent to
				//
				//     "foo".""."bar"
				//
				if (prev == component::dot && !on_key(data, ""sv))
					return false;

				pos++;
				prev = component::dot;
			}

			// an errant closing ']'
			else if TOML_UNLIKELY(path[pos] == ']')
				return false;

			// some regular subkey
			else
			{
				const auto subkey_start = pos;
				const auto subkey_len	= impl::min(path.find_first_of(".[]"sv, subkey_start + 1u), end) - subkey_start;
				const auto subkey		= path.substr(subkey_start, subkey_len);
				pos += subkey_len;

				// a regular subkey segment immediately after an array indexer is OK if it was all whitespace, e.g.:
				//
				//     "foo[0]  .bar"
				//            ^^ skip this
				//
				// otherwise its an error (since it would have to be preceded by a dot)
				if (prev == component::array_indexer)
				{
					if (subkey.find_first_not_of(" \t"sv) != std::string_view::npos)
						return false;

					prev = component::key;
					continue;
				}

				prev = component::key;

				if (!on_key(data, subkey))
					return false;
			}
		}

		// Last character was a '.', which implies an empty string key at the end of the path
		if (prev == component::dot && !on_key(data, ""sv))
			return false;

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV has_children(const node& n) noexcept
	{
		if (const auto tbl = n.as_table())
			return !tbl->empty();
		if (const auto arr = n.as_array())
			return !arr->empty();
		return false;
	}

	TOML_EXTERNAL_LINKAGE
	node* TOML_CALLCONV child_at(node & n, std::string_view key) noexcept
	{
		const auto tbl = n.as<table>();
		return tbl ? tbl->get(key) : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	node* TOML_CALLCONV child_at(node & n, size_t index) noexcept
	{
		const auto arr = n.as<array>();
		return arr ? arr->get(index) : nullptr;
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::string_view path) noexcept
	{
		if (!impl::has_children(root))
			return {};

		node* current = &root;

		static constexpr auto on_key = [](void* data, std::string_view key) noexcept -> bool
		{
			auto& curr = *static_cast<node**>(data);
			TOML_ASSERT_ASSUME(curr);

			curr = impl::child_at(*curr, key);
			return curr != nullptr;
		};

		static constexpr auto on_index = [](void* data, size_t index) noexcept -> bool
		{
			auto& curr = *static_cast<node**>(data);
			TOML_ASSERT_ASSUME(curr);

			curr = impl::child_at(*curr, index);
			return curr != nullptr;
		};

		if (!impl::parse_path(path, static_cast<void*>(&current), on_key, on_index))
			current = nullptr;

		return node_view{ current };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::string_view path) noexcept
	{
		return node_view<const node>{ at_path(const_cast<node&>(root), path).node() };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::wstring_view path)
	{
		// the narrow-string version does this check too;
		// it's hoisted up here to avoid doing the wide -> narrow conversion where it would not be necessary
		// (avoids an allocation)
		if (!impl::has_children(root))
			return {};

		return at_path(root, impl::narrow(path));
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::wstring_view path)
	{
		return node_view<const node>{ at_path(const_cast<node&>(root), path).node() };
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/path.inl  *********************************************************************************************

TOML_DISABLE_WARNINGS;
#include <sstream>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	path_component::path_component() //
		: type_{ path_component_type::key }
	{
		store_key(std::string_view{}, value_storage_);
	}

	TOML_EXTERNAL_LINKAGE
	path_component::path_component(size_t index) noexcept //
		: type_(path_component_type::array_index)
	{
		store_index(index, value_storage_);
	}

	TOML_EXTERNAL_LINKAGE
	path_component::path_component(std::string_view key) //
		: type_(path_component_type::key)
	{
		store_key(key, value_storage_);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path_component::path_component(std::wstring_view key) //
		: path_component(impl::narrow(key))
	{}

#endif

	TOML_EXTERNAL_LINKAGE
	path_component::path_component(const path_component& pc) //
		: type_{ pc.type_ }
	{
		if (type_ == path_component_type::array_index)
			store_index(pc.index(), value_storage_);
		else
			store_key(pc.key(), value_storage_);
	}

	TOML_EXTERNAL_LINKAGE
	path_component::path_component(path_component && pc) noexcept //
		: type_{ pc.type_ }
	{
		if (type_ == path_component_type::array_index)
			store_index(pc.index_ref(), value_storage_);
		else
			store_key(std::move(pc.key_ref()), value_storage_);
	}

	TOML_EXTERNAL_LINKAGE
	path_component& path_component::operator=(const path_component& rhs)
	{
		if (rhs.type_ == path_component_type::array_index)
			return *this = rhs.index();
		return *this = std::string_view{ rhs.key() };
	}

	TOML_EXTERNAL_LINKAGE
	path_component& path_component::operator=(path_component&& rhs) noexcept
	{
		if (type_ != rhs.type_)
		{
			destroy();

			type_ = rhs.type_;
			if (type_ == path_component_type::array_index)
				store_index(rhs.index(), value_storage_);
			else
				store_key(std::move(rhs.key_ref()), value_storage_);
		}
		else
		{
			if (type_ == path_component_type::array_index)
				index_ref() = rhs.index();
			else
				key_ref() = std::move(rhs.key_ref());
		}
		return *this;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV path_component::equal(const path_component& lhs, const path_component& rhs) noexcept
	{
		// Different comparison depending on contents
		if (lhs.type_ != rhs.type_)
			return false;

		if (lhs.type_ == path_component_type::array_index)
			return lhs.index() == rhs.index();
		// path_component_type::key
		return lhs.key() == rhs.key();
	}

	TOML_EXTERNAL_LINKAGE
	path_component& path_component::operator=(size_t rhs) noexcept
	{
		// If currently a key, string will need to be destroyed regardless
		destroy();

		type_ = path_component_type::array_index;
		store_index(rhs, value_storage_);

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path_component& path_component::operator=(std::string_view rhs)
	{
		if (type_ == path_component_type::key)
			key_ref() = rhs;
		else
		{
			store_key(rhs, value_storage_);
			type_ = path_component_type::key;
		}

		return *this;
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path_component& path_component::operator=(std::wstring_view rhs)
	{
		return *this = std::string_view{ impl::narrow(rhs) };
	}

#endif
}
TOML_NAMESPACE_END;

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	bool parse_path_into(std::string_view path_str, std::vector<path_component> & components)
	{
		const auto original_size = components.size();

		static constexpr auto on_key = [](void* data, std::string_view key) -> bool
		{
			auto& comps = *static_cast<std::vector<path_component>*>(data);
			comps.emplace_back(key);
			return true;
		};

		static constexpr auto on_index = [](void* data, size_t index) -> bool
		{
			auto& comps = *static_cast<std::vector<path_component>*>(data);
			comps.emplace_back(index);
			return true;
		};

		if (!impl::parse_path(path_str, &components, on_key, on_index))
		{
			components.resize(original_size);
			return false;
		}

		return true;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void path::print_to(std::ostream & os) const
	{
		bool root = true;
		for (const auto& component : components_)
		{
			if (component.type() == path_component_type::key) // key
			{
				if (!root)
					impl::print_to_stream(os, '.');
				impl::print_to_stream(os, component.key());
			}
			else // array
			{
				impl::print_to_stream(os, '[');
				impl::print_to_stream(os, component.index());
				impl::print_to_stream(os, ']');
			}
			root = false;
		}
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV path::equal(const path& lhs, const path& rhs) noexcept
	{
		return lhs.components_ == rhs.components_;
	}

	TOML_EXTERNAL_LINKAGE
	path::path(std::string_view str) //
	{
		TOML_ANON_NAMESPACE::parse_path_into(str, components_);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path::path(std::wstring_view str) //
		: path(impl::narrow(str))
	{}

#endif

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(std::string_view rhs)
	{
		// rhs may view one of this path's keys
		return *this = path{ rhs };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(std::wstring_view rhs)
	{
		return *this = impl::narrow(rhs);
	}

#endif

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(const path& rhs)
	{
		// vector::insert can't take a range from the same vector
		if (&rhs == this)
			return *this += path{ rhs };

		components_.insert(components_.end(), rhs.components_.begin(), rhs.components_.end());
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(path&& rhs)
	{
		if (&rhs == this)
			return *this += path{ rhs };

		components_.insert(components_.end(),
						   std::make_move_iterator(rhs.components_.begin()),
						   std::make_move_iterator(rhs.components_.end()));
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::string_view rhs)
	{
		// rhs may view one of this path's keys
		return *this += path{ rhs };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::wstring_view rhs)
	{
		return *this += impl::narrow(rhs);
	}

#endif

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(const path& source)
	{
		if (&source == this)
			return prepend(path{ source });

		components_.insert(components_.begin(), source.components_.begin(), source.components_.end());
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(path && source)
	{
		if (&source == this)
			return prepend(path{ source });

		components_.insert(components_.begin(),
						   std::make_move_iterator(source.components_.begin()),
						   std::make_move_iterator(source.components_.end()));
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(std::string_view source)
	{
		return prepend(path{ source });
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(std::wstring_view source)
	{
		return prepend(impl::narrow(source));
	}

#endif

	TOML_EXTERNAL_LINKAGE
	std::string path::str() const
	{
		if (empty())
			return {};

		std::ostringstream ss;
		print_to(ss);
		return std::move(ss).str();
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	std::wstring path::wide_str() const
	{
		return impl::widen(str());
	}

#endif

	TOML_EXTERNAL_LINKAGE
	void path::clear() noexcept
	{
		components_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	path& path::truncate(size_t n)
	{
		n = impl::min(n, components_.size());

		const auto it_end = components_.end();
		components_.erase(it_end - static_cast<ptrdiff_t>(n), it_end);

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path path::truncated(size_t n) const
	{
		return subpath(0u, components_.size() - impl::min(n, components_.size()));
	}

	TOML_EXTERNAL_LINKAGE
	path path::parent() const
	{
		return truncated(1);
	}

	TOML_EXTERNAL_LINKAGE
	path path::leaf(size_t n) const
	{
		n = impl::min(n, components_.size());
		return subpath(components_.size() - n, n);
	}

	TOML_EXTERNAL_LINKAGE
	path path::subpath(const_iterator start, const_iterator end) const
	{
		if (start >= end)
			return {};

		path result;
		result.components_.insert(result.components_.begin(), start, end);
		return result;
	}

	TOML_EXTERNAL_LINKAGE
	path path::subpath(size_t start, size_t length) const
	{
		if (start >= components_.size() || length == 0)
			return {};
		const size_t count = impl::min(length, components_.size() - start);
		return subpath(begin() + static_cast<ptrdiff_t>(start), begin() + static_cast<ptrdiff_t>(start + count));
	}
}
TOML_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, const toml::path& p) noexcept
	{
		if (!impl::has_children(root))
			return {};

		node* current = &root;

		for (const auto& component : p)
		{
			current = component.type() == path_component_type::array_index ? impl::child_at(*current, component.index())
																		   : impl::child_at(*current, component.key());

			if (!current)
				return {}; // not found, or current is not the kind of container the component selects from
		}

		return node_view{ current };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, const toml::path& p) noexcept
	{
		return node_view<const node>{ at_path(const_cast<node&>(root), p).node() };
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/array.inl  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	array::array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::~array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const impl::array_init_elem* first, const impl::array_init_elem* last)
	{
		TOML_ASSERT_ASSUME(first);
		TOML_ASSERT_ASSUME(last);
		TOML_ASSERT_ASSUME(first <= last);

		size_t cap{};
		for (auto it = first; it != last; it++)
		{
			if (it->value)
				cap++;
		}

		elems_.reserve(cap);
		for (; first != last; first++)
		{
			if (first->value)
				elems_.push_back(std::move(first->value));
		}

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const array& other) //
		: node(other)
	{
		elems_.reserve(other.elems_.size());
		for (const auto& elem : other)
			elems_.emplace_back(impl::make_node(elem));

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	array::array(array && other) noexcept //
		: node(std::move(other)),
		  elems_(std::move(other.elems_))
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(const array& rhs)
	{
		if (&rhs == this)
			return *this;

		vector_type copy;
		copy.reserve(rhs.elems_.size());
		for (const auto& elem : rhs)
			copy.emplace_back(impl::make_node(elem));
		node::operator=(rhs);
		elems_.swap(copy);
		return *this;
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	array& array::operator=(array&& rhs) noexcept
	{
		if (&rhs == this)
			return *this;

		node::operator=(std::move(rhs));
		vector_type stolen = std::move(rhs.elems_);
		elems_.swap(stolen);
		return *this;
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_EXTERNAL_LINKAGE
	void array::preinsertion_resize(size_t idx, size_t count)
	{
		TOML_ASSERT(idx <= elems_.size());
		TOML_ASSERT_ASSUME(count >= 1u);
		const auto old_size = elems_.size();
		const auto new_size = old_size + count;
		elems_.resize(new_size);
		for (size_t left = old_size, right = new_size - 1u; left-- > idx; right--)
			elems_[right] = std::move(elems_[left]);
	}

	TOML_EXTERNAL_LINKAGE
	void array::insert_at_back(impl::node_ptr && elem)
	{
		TOML_ASSERT(elem);
		elems_.push_back(std::move(elem));
	}

	TOML_EXTERNAL_LINKAGE
	array::vector_iterator array::insert_at(const_vector_iterator pos, impl::node_ptr && elem)
	{
		return elems_.insert(pos, std::move(elem));
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype) const noexcept
	{
		const node* first_nonmatch = {};
		return is_homogeneous(ntype, first_nonmatch);
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (elems_.empty())
		{
			first_nonmatch = {};
			return false;
		}
		if (ntype == node_type::none)
			ntype = elems_[0]->type();
		for (const auto& val : elems_)
		{
			if (val->type() != ntype)
			{
				first_nonmatch = val.get();
				return false;
			}
		}
		return true;
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* nonmatch	  = {};
		const auto result = const_cast<array&>(*this).is_homogeneous(ntype, nonmatch);
		first_nonmatch	  = nonmatch;
		return result;
	}

	TOML_EXTERNAL_LINKAGE
	node& array::at(size_t index)
	{
#if TOML_COMPILER_HAS_EXCEPTIONS

		return *elems_.at(index);

#else

		const auto n = get(index);
		TOML_ASSERT_ASSUME(n && "element index not found in array");
		return *n;

#endif
	}

	TOML_EXTERNAL_LINKAGE
	void array::reserve(size_t new_capacity)
	{
		elems_.reserve(new_capacity);
	}

	TOML_EXTERNAL_LINKAGE
	void array::shrink_to_fit()
	{
		elems_.shrink_to_fit();
	}

	TOML_EXTERNAL_LINKAGE
	void array::truncate(size_t new_size)
	{
		if (new_size < elems_.size())
			elems_.resize(new_size);
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ pos }) };
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ first }, const_vector_iterator{ last }) };
	}

	TOML_EXTERNAL_LINKAGE
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (const auto& elem : elems_)
		{
			const auto arr = elem->as_array();
			leaves += arr ? arr->total_leaf_count() : size_t{ 1 };
		}
		return leaves;
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten_child(array && child, size_t& dest_index) noexcept
	{
		for (auto& elem : child.elems_)
		{
			if (const auto arr = elem->as_array())
			{
				if (!arr->empty())
					flatten_child(std::move(*arr), dest_index);
			}
			else
				elems_[dest_index++] = std::move(elem);
		}
	}

	TOML_EXTERNAL_LINKAGE
	array& array::flatten() &
	{
		bool has_child_arrays = {};
		for (const auto& elem : elems_)
		{
			if (elem->is_array())
			{
				has_child_arrays = true;
				break;
			}
		}
		if (!has_child_arrays)
			return *this;

		array flattened;
		flattened.elems_.resize(total_leaf_count());

		size_t dest_index = {};
		flattened.flatten_child(std::move(*this), dest_index);
		elems_ = std::move(flattened.elems_);
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	array& array::prune(bool recursive) & noexcept
	{
		size_t kept = {};
		for (size_t i = 0, e = elems_.size(); i < e; i++)
		{
			bool is_empty = {};
			if (const auto arr = elems_[i]->as_array())
			{
				if (recursive)
					arr->prune(true);
				is_empty = arr->empty();
			}
			else if (const auto tbl = elems_[i]->as_table())
			{
				if (recursive)
					tbl->prune(true);
				is_empty = tbl->empty();
			}
			if (is_empty)
				continue;

			if (kept != i)
				elems_[kept] = std::move(elems_[i]);
			kept++;
		}
		elems_.erase(elems_.cbegin() + static_cast<ptrdiff_t>(kept), elems_.cend());

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void array::pop_back() noexcept
	{
		elems_.pop_back();
	}

	TOML_EXTERNAL_LINKAGE
	void array::clear() noexcept
	{
		elems_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV array::equal(const array& lhs, const array& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.elems_.size() != rhs.elems_.size())
			return false;
		for (size_t i = 0, e = lhs.elems_.size(); i < e; i++)
		{
			const node& lhs_elem = *lhs.elems_[i];
			const node& rhs_elem = *rhs.elems_[i];
			if (lhs_elem.type() != rhs_elem.type() || !lhs_elem.visit(impl::equal_to_same_type{ &rhs_elem }))
				return false;
		}
		return true;
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/table.inl  ********************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	table::table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::~table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const impl::table_init_pair* b, const impl::table_init_pair* e)
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif

		if TOML_UNLIKELY(b == e)
			return;

		TOML_ASSERT_ASSUME(b);
		TOML_ASSERT_ASSUME(e);
		TOML_ASSERT_ASSUME(b <= e);

		for (; b != e; b++)
		{
			if (!b->value) // empty node_views
				continue;

			map_.insert_or_assign(std::move(b->key), std::move(b->value));
		}
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const table& other) //
		: node(other),
		  inline_{ other.inline_ }
	{
		for (auto&& [k, v] : other.map_)
			map_.emplace_hint(map_.end(), k, impl::make_node(std::as_const(*v)));

#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	table::table(table && other) noexcept //
		: node(std::move(other)),
		  map_{ std::move(other.map_) },
		  inline_{ other.inline_ }
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(const table& rhs)
	{
		if (&rhs == this)
			return *this;

		map_type copy;
		for (auto&& [k, v] : rhs.map_)
			copy.emplace_hint(copy.end(), k, impl::make_node(std::as_const(*v)));
		node::operator=(rhs);
		inline_ = rhs.inline_;
		map_.swap(copy);
		return *this;
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	table& table::operator=(table&& rhs) noexcept
	{
		if (&rhs == this)
			return *this;

		node::operator=(std::move(rhs));
		inline_			= rhs.inline_;
		map_type stolen = std::move(rhs.map_);
		map_.swap(stolen);
		return *this;
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype) const noexcept
	{
		const node* first_nonmatch = {};
		return is_homogeneous(ntype, first_nonmatch);
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (map_.empty())
		{
			first_nonmatch = {};
			return false;
		}
		if (ntype == node_type::none)
			ntype = map_.cbegin()->second->type();
		for (const auto& [k, v] : map_)
		{
			TOML_UNUSED(k);
			if (v->type() != ntype)
			{
				first_nonmatch = v.get();
				return false;
			}
		}
		return true;
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* nonmatch	  = {};
		const auto result = const_cast<table&>(*this).is_homogeneous(ntype, nonmatch);
		first_nonmatch	  = nonmatch;
		return result;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	node* table::get(std::string_view key) noexcept
	{
		if (auto it = map_.find(key); it != map_.end())
			return it->second.get();
		return nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	node& table::at(std::string_view key)
	{
		const auto n = get(key);

#if TOML_COMPILER_HAS_EXCEPTIONS

		if (!n)
		{
			auto err = "key '"s;
			err.append(key);
			err.append("' not found in table"sv);
			throw std::out_of_range{ err };
		}

#else

		TOML_ASSERT_ASSUME(n && "key not found in table");

#endif

		return *n;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::get_lower_bound(std::string_view key) noexcept
	{
		return map_.lower_bound(key);
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::iterator table::find(std::string_view key) noexcept
	{
		return iterator{ map_.find(key) };
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::const_iterator table::find(std::string_view key) const noexcept
	{
		return const_iterator{ map_.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator pos) noexcept
	{
		return map_.erase(pos);
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator begin, const_map_iterator end) noexcept
	{
		return map_.erase(begin, end);
	}

	TOML_EXTERNAL_LINKAGE
	size_t table::erase(std::string_view key) noexcept
	{
		if (auto it = map_.find(key); it != map_.end())
		{
			map_.erase(it);
			return size_t{ 1 };
		}
		return size_t{};
	}

	TOML_EXTERNAL_LINKAGE
	table& table::prune(bool recursive) & noexcept
	{
		for (auto it = map_.begin(); it != map_.end();)
		{
			if (auto arr = it->second->as_array())
			{
				if (recursive)
					arr->prune(true);

				if (arr->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
			else if (auto tbl = it->second->as_table())
			{
				if (recursive)
					tbl->prune(true);

				if (tbl->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
			it++;
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void table::clear() noexcept
	{
		map_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::insert_with_hint(const_iterator hint, key && k, impl::node_ptr && v)
	{
		return map_.emplace_hint(const_map_iterator{ hint }, std::move(k), std::move(v));
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV table::equal(const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.map_.size() != rhs.map_.size())
			return false;

		for (auto l = lhs.map_.begin(), r = rhs.map_.begin(), e = lhs.map_.end(); l != e; l++, r++)
		{
			if (l->first != r->first)
				return false;

			const node& lhs_val = *l->second;
			const node& rhs_val = *r->second;
			if (lhs_val.type() != rhs_val.type() || !lhs_val.visit(impl::equal_to_same_type{ &rhs_val }))
				return false;
		}
		return true;
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/simd.hpp  *********************************************************************************************

#if TOML_ENABLE_SIMD

#if defined(__SSE2__)                                                                                                  \
	|| (defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)))
#define TOML_HAS_SSE2 1
#endif

#endif // TOML_ENABLE_SIMD

#ifndef TOML_HAS_SSE2
#define TOML_HAS_SSE2 0
#endif

TOML_DISABLE_WARNINGS;
#if TOML_HAS_SSE2
#include <emmintrin.h>
#endif
TOML_ENABLE_WARNINGS;

//********  impl/unicode.inl  ******************************************************************************************

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_ATTR(nonnull)
	TOML_EXTERNAL_LINKAGE
	bool is_ascii(const char* str, size_t len) noexcept
	{
		const char* const end = str + len;

#if TOML_HAS_SSE2
		{
			constexpr size_t chars_per_vector = 16;

			if (const size_t simdable = len - (len % chars_per_vector))
			{
				__m128i mask = _mm_setzero_si128();
				for (const char* const e = str + simdable; str < e; str += chars_per_vector)
				{
					const __m128i current_bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str));
					mask						= _mm_or_si128(mask, current_bytes);
				}
				if (_mm_movemask_epi8(mask))
					return false;
			}
		}
#endif

		for (; str < end; str++)
			if (static_cast<unsigned char>(*str) > 127u)
				return false;

		return true;
	}
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

//********  impl/parser.inl  *******************************************************************************************

#if TOML_ENABLE_PARSER

TOML_DISABLE_WARNINGS;
#include <istream>
#include <fstream>
#if TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#include <cstdlib>
#include <exception>
#if defined(__GLIBCXX__)
#include <cxxabi.h>
#endif
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_DISABLE_MSVC_WARNING(4702);

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	TOML_INTERNAL_LINKAGE
	constexpr size_t utf8_byte_order_mark_length = utf8_byte_order_mark.length();

	class utf8_byte_stream
	{
	  private:
		static constexpr size_t block_size = 32u;

		std::istream* source_ = {};
		// bytes read for the bom check, kept because seeking back to reread them fails on pipes
		char prefix_[utf8_byte_order_mark_length] = {};
		size_t prefix_begin_					  = {};
		size_t prefix_end_						  = {};
		bool started_							  = {};
#if TOML_COMPILER_HAS_EXCEPTIONS
		// rethrown where the error for a failed read would be reported
		std::exception_ptr failure_;
#endif

		TOML_PURE_INLINE_GETTER
		bool has_prefix() const noexcept
		{
			return prefix_begin_ < prefix_end_;
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		TOML_PURE_INLINE_GETTER
		bool reached_end() const noexcept
		{
			return (source_->rdstate() & (std::istream::badbit | std::istream::eofbit)) == std::istream::eofbit;
		}

#endif

		// false if the stream call threw; the exception is kept in failure_
		template <typename Func>
		bool guarded([[maybe_unused]] bool end_is_failure, Func&& stream_call) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			try
			{
				static_cast<Func&&>(stream_call)();
				return true;
			}
#if defined(__GLIBCXX__)
			// glibc cancels threads by unwinding with this, and aborts the process if it's swallowed
			catch (abi::__forced_unwind&)
			{
				throw;
			}
#endif
			catch (...)
			{
				if (!failure_ && (end_is_failure || !reached_end()))
					failure_ = std::current_exception();
				return false;
			}
#else
			static_cast<Func&&>(stream_call)();
			return true;
#endif
		}

		TOML_NODISCARD
		size_t read(char* dest, size_t num) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			guarded(false, [&]() { source_->read(dest, static_cast<std::streamsize>(num)); });
			return static_cast<size_t>(source_->gcount());
		}

		TOML_NODISCARD
		std::istream::int_type peek() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			auto next = std::istream::traits_type::eof();
			guarded(false, [&]() { next = source_->peek(); });
			return next;
		}

		// in_avail() can call the streambuf's showmanyc(), which may throw
		TOML_NODISCARD
		bool available(std::streamsize& held) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			return guarded(true, [&]() { held = source_->rdbuf()->in_avail(); });
		}

		// istream::read keeps nothing of a read the streambuf throws in, so this takes only what the stream holds,
		// or the rest of the block from one that holds nothing even after a peek
		TOML_NODISCARD
		size_t take(char* dest, size_t num, size_t block) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			if (failure_)
				return {};
#endif
			if (source_->good())
			{
				std::streamsize held = {};
				if (!available(held))
					return {};
				// msvc's peek returns 0 when the streambuf throws
				if (held <= 0 && (peek() == std::istream::traits_type::eof() || !source_->good() || !available(held)))
					return {};
				const auto limit = held > 0 ? held : static_cast<std::streamsize>(block);
				num				 = static_cast<size_t>(impl::min(static_cast<std::streamsize>(num), limit));
			}
			return read(dest, num);
		}

	  public:
		TOML_NODISCARD_CTOR
		utf8_byte_stream() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit utf8_byte_stream(std::istream& stream) noexcept //
			: source_{ &stream }
		{}

		TOML_PURE_INLINE_GETTER
		bool error() const noexcept
		{
			return !!(source_->rdstate() & std::istream::badbit);
		}

		TOML_PURE_INLINE_GETTER
		bool eof() const noexcept
		{
			return started_ && !has_prefix() && !!(source_->rdstate() & std::istream::eofbit);
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		void rethrow_failure() const
		{
			if (failure_)
				std::rethrow_exception(failure_);
		}

#endif

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(char* dest, size_t num, uint64_t offset) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!started_)
			{
				started_   = true;
				size_t got = {};
				do
				{
					got = take(prefix_ + prefix_end_,
							   utf8_byte_order_mark_length - prefix_end_,
							   utf8_byte_order_mark_length - prefix_end_);
					prefix_end_ += got;
				}
				while (got && prefix_end_ < utf8_byte_order_mark_length && source_->good());
				if (prefix_end_ == utf8_byte_order_mark_length
					&& std::memcmp(utf8_byte_order_mark.data(), prefix_, utf8_byte_order_mark_length) == 0)
					prefix_end_ = 0u;
			}

			if (has_prefix())
			{
				const auto count = impl::min(num, prefix_end_ - prefix_begin_);
				std::memcpy(dest, prefix_ + prefix_begin_, count);
				prefix_begin_ += count;
				return count;
			}

			if (source_->rdstate() & (std::istream::badbit | std::istream::eofbit))
				return {};
			return take(dest, num, block_size - static_cast<size_t>(offset % block_size));
		}
	};

	struct utf8_codepoint
	{
		char32_t value;
		char bytes[4];
		size_t count;

		TOML_PURE_INLINE_GETTER
		constexpr operator char32_t() const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivially_default_constructible_v<utf8_codepoint>
				  && std::is_trivially_copyable_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	struct lookahead_pos
	{
		uint64_t offset;
		size_t count;
		char32_t value;
	};

	TOML_INTERNAL_LINKAGE
	constexpr char32_t eof_mark = static_cast<char32_t>(0x110000u);

	TOML_INTERNAL_LINKAGE
	constexpr char32_t wall_mark = static_cast<char32_t>(0x110001u);

	enum class TOML_CLOSED_ENUM wall_kind : unsigned char
	{
		invalid,
		cut_sequence,
		zero_bytes,
		io_error
	};

	enum class TOML_CLOSED_FLAGS_ENUM run_class : unsigned char
	{
		comment		   = 1,
		basic_string   = 2,
		literal_string = 4,
		bare_key	   = 8
	};

	struct run_table
	{
		unsigned char classes[256];
	};

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr run_table make_run_table() noexcept
	{
		run_table table{};
		for (unsigned i = 0; i < 0x80u; i++)
		{
			const auto c	 = static_cast<char32_t>(i);
			unsigned classes = {};
			if (!impl::is_ascii_vertical_whitespace(c) && !impl::is_nontab_control_character(c))
			{
				classes |= static_cast<unsigned>(run_class::comment);
				if (c != U'"' && c != U'\\')
					classes |= static_cast<unsigned>(run_class::basic_string);
				if (c != U'\'')
					classes |= static_cast<unsigned>(run_class::literal_string);
			}
			if (impl::is_ascii_bare_key_character(c))
				classes |= static_cast<unsigned>(run_class::bare_key);
			table.classes[i] = static_cast<unsigned char>(classes);
		}
		for (unsigned i = 0x80u; i < 0x100u; i++)
			table.classes[i] = static_cast<unsigned char>(static_cast<unsigned>(run_class::comment)
														  | static_cast<unsigned>(run_class::basic_string)
														  | static_cast<unsigned>(run_class::literal_string));
		return table;
	}

	TOML_INTERNAL_LINKAGE
	constexpr run_table run_bytes = make_run_table();

	TOML_INTERNAL_LINKAGE
	constexpr uint64_t word_high_bits = 0x8080808080808080ull;

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t word_of(uint64_t byte) noexcept
	{
		return 0x0101010101010101ull * byte;
	}

	// the first byte is the lowest, whatever the byte order
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	uint64_t load_word(const char* bytes) noexcept
	{
		uint64_t word = {};
		std::memcpy(&word, bytes, 8u);
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		word = __builtin_bswap64(word);
#endif
		return word;
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	unsigned trailing_zeros(uint64_t bits) noexcept
	{
#if TOML_GCC_LIKE
		return static_cast<unsigned>(__builtin_ctzll(bits));
#else
		static constexpr unsigned char positions[64] = {
			0,	1,	48, 2,	57, 49, 28, 3,	61, 58, 50, 42, 38, 29, 17, 4,	62, 55, 59, 36, 53, 51,
			43, 22, 45, 39, 33, 30, 24, 18, 12, 5,	63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21,
			44, 32, 23, 11, 46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,	13, 8,	7,	6
		};
		return positions[((bits & (~bits + 1u)) * 0x03F79D71B4CB0A89ull) >> 58];
#endif
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr size_t count_high_bits(uint64_t bits) noexcept
	{
		return static_cast<size_t>(((bits >> 7) * word_of(1u)) >> 56);
	}

	// for words of ascii bytes: the bytes equal to c, as their high bits
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t bytes_equal(uint64_t ascii, unsigned c) noexcept
	{
		return ~((ascii ^ word_of(c)) + word_of(0x7Fu)) & word_high_bits;
	}

	// the bytes of a word that end a comment or string run, as their high bits
	template <run_class Class>
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t run_stops(uint64_t word) noexcept
	{
		const auto ascii = word & ~word_high_bits;
		auto delimiters	 = bytes_equal(ascii, 0x7Fu);
		if constexpr (Class == run_class::basic_string)
			delimiters |= bytes_equal(ascii, '"') | bytes_equal(ascii, '\\');
		if constexpr (Class == run_class::literal_string)
			delimiters |= bytes_equal(ascii, '\'');
		const auto controls = ~((ascii + word_of(0x60u)) | word | bytes_equal(ascii, '\t')) & word_high_bits;
		return controls | (delimiters & ~word);
	}

#if TOML_HAS_SSE2

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr unsigned count_bits(unsigned bits) noexcept
	{
		bits = bits - ((bits >> 1) & 0x55555555u);
		bits = (bits & 0x33333333u) + ((bits >> 2) & 0x33333333u);
		return (((bits + (bits >> 4)) & 0x0F0F0F0Fu) * 0x01010101u) >> 24;
	}

	// the bytes of a block that end a comment or string run, as a movemask
	template <run_class Class>
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	unsigned run_stops(__m128i bytes) noexcept
	{
		const auto equal = [&](char c) noexcept { return _mm_cmpeq_epi8(bytes, _mm_set1_epi8(c)); };

		// bytes from 0x80 compare as negative
		const auto controls =
			_mm_andnot_si128(equal('\t'),
							 _mm_and_si128(_mm_cmpgt_epi8(bytes, _mm_set1_epi8(static_cast<char>(-1))),
										   _mm_cmplt_epi8(bytes, _mm_set1_epi8(0x20))));
		auto stops = _mm_or_si128(controls, equal(0x7F));
		if constexpr (Class == run_class::basic_string)
			stops = _mm_or_si128(stops, _mm_or_si128(equal('"'), equal('\\')));
		if constexpr (Class == run_class::literal_string)
			stops = _mm_or_si128(stops, equal('\''));
		return static_cast<unsigned>(_mm_movemask_epi8(stops));
	}

#endif

	// how many ascii bytes the first n start with
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	size_t ascii_length(const char* bytes, size_t n) noexcept
	{
		size_t i = {};
#if TOML_HAS_SSE2
		for (; n - i >= 16u; i += 16u)
			if (const auto high = static_cast<unsigned>(
					_mm_movemask_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i*>(bytes + i)))))
				return i + trailing_zeros(high);
#endif
		while (n - i >= 8u && !(load_word(bytes + i) & word_high_bits))
			i += 8u;
		while (i < n && static_cast<unsigned char>(bytes[i]) < 0x80u)
			i++;
		return i;
	}

	struct run_span
	{
		const char* data;
		size_t length;
		size_t continuation;
	};

	class source_reader
	{
	  private:
		static constexpr uint64_t no_wall	   = static_cast<uint64_t>(-1);
		static constexpr size_t string_chunk   = 4096;
		static constexpr size_t initial_window = 256;

		// pos_, valid_ and filled_ index data_; base_, line_start_ and wall_ are offsets into the document, 64-bit
		// because a stream can outgrow size_t
		utf8_codepoint current_ = {};
		const char* data_		= {};
		size_t pos_				= {};
		size_t valid_			= {};
		size_t filled_			= {};
		uint64_t base_			= {};
		uint64_t line_start_	= {};
		size_t continuation_	= {};
		uint64_t wall_			= no_wall;
		source_index line_		= 1;
		source_index lf_column_ = {};
		wall_kind wall_kind_	= {};
		bool ended_				= {};
		utf8_byte_stream stream_;
		std::unique_ptr<char[]> window_;
		size_t capacity_ = {};
		source_path_ptr source_path_;

		template <typename String>
		TOML_NODISCARD
		static source_path_ptr make_source_path(String&& source_path)
		{
			if (source_path.empty())
				return {};
			return std::make_shared<const std::string>(static_cast<String&&>(source_path));
		}

		void set_wall(wall_kind kind) noexcept
		{
			wall_	   = base_ + valid_;
			wall_kind_ = kind;
		}

		void validate(size_t limit) noexcept
		{
			auto i = valid_;
			while (i < limit)
			{
				i += ascii_length(data_ + i, limit - i);
				if (i == limit)
					break;

				impl::utf8_decoder decoder;
				auto end = i;
				do
				{
					if (end == filled_)
					{
						valid_ = i;
						return;
					}
					decoder(data_[end++]);
					if (decoder.error())
					{
						valid_ = i;
						set_wall(wall_kind::invalid);
						return;
					}
				}
				while (!decoder.has_codepoint());
				i = end;
			}
			valid_ = i;
		}

		void make_room()
		{
			const auto live = filled_ - pos_;
			auto capacity	= capacity_;
			while (live > capacity / 2u)
				capacity *= 2u;
			if (!pos_ && capacity == capacity_)
				return;

#ifdef NDEBUG
			const bool move = capacity != capacity_;
#else
			// moving the window on every compaction lets asan catch a pointer kept into the old one
			const bool move = true;
#endif
			if (move)
			{
				std::unique_ptr<char[]> window{ new char[capacity] };
				std::memcpy(window.get(), window_.get() + pos_, live);
				window_	  = std::move(window);
				capacity_ = capacity;
			}
			else
				std::memmove(window_.get(), window_.get() + pos_, live);

			data_ = window_.get();
			base_ += pos_;
			valid_ -= pos_;
			filled_ -= pos_;
			pos_ = {};
		}

		TOML_NEVER_INLINE
		void extend() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!window_)
			{
				const auto limit = filled_ - valid_ > string_chunk ? valid_ + string_chunk : filled_;
				validate(limit);
				if (wall_ == no_wall && valid_ < limit)
					set_wall(wall_kind::cut_sequence);
				return;
			}

			make_room();
			TOML_ASSERT_ASSUME(filled_ < capacity_);
			const auto got = stream_(window_.get() + filled_, capacity_ - filled_, base_ + filled_);
			filled_ += got;
			validate(filled_);
			if (wall_ != no_wall)
				return;
			if (got)
			{
				if (stream_.error())
					set_wall(wall_kind::io_error);
			}
			else if (!stream_.eof())
				set_wall(base_ + filled_ ? wall_kind::io_error : wall_kind::zero_bytes);
			else if (valid_ < filled_)
				set_wall(wall_kind::cut_sequence);
			else
				ended_ = true;
		}

		TOML_NODISCARD
		bool ensure(uint64_t at) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			while (at >= base_ + valid_)
			{
				if (at >= wall_ || (ended_ && valid_ == filled_))
					return false;
				extend();
			}
			return true;
		}

		TOML_NODISCARD
		size_t decode(size_t i, char32_t& value) const noexcept
		{
			const auto lead = static_cast<unsigned char>(data_[i]);
			if (lead < 0x80u)
			{
				value = lead;
				return 1u;
			}
			const size_t count = lead < 0xE0u ? 2u : (lead < 0xF0u ? 3u : 4u);
			uint_least32_t cp  = lead & (0x7Fu >> count);
			for (size_t k = 1; k < count; k++)
				cp = (cp << 6) | (static_cast<unsigned char>(data_[i + k]) & 0x3Fu);
			value = static_cast<char32_t>(cp);
			return count;
		}

		TOML_NODISCARD
		const utf8_codepoint* land() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (pos_ < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[pos_]);
				if TOML_LIKELY(byte < 0x80u)
				{
					current_.value	  = byte;
					current_.bytes[0] = data_[pos_];
					current_.count	  = 1u;
					return &current_;
				}
			}
			return land_slow();
		}

		TOML_NEVER_INLINE
		const utf8_codepoint* land_slow() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!ensure(base_ + pos_))
				return nullptr;

			current_.count = decode(pos_, current_.value);
			std::memcpy(current_.bytes, data_ + pos_, current_.count);
			return &current_;
		}

		TOML_NEVER_INLINE
		void step_slow(lookahead_pos& la) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!ensure(la.offset))
			{
				la.value = la.offset == wall_ ? wall_mark : eof_mark;
				la.count = {};
				return;
			}
			la.count = decode(static_cast<size_t>(la.offset - base_), la.value);
		}

	  public:
		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::string_view doc, String&& source_path) //
			: data_{ doc.data() },
			  filled_{ doc.length() },
			  ended_{ true },
			  source_path_{ make_source_path(static_cast<String&&>(source_path)) }
		{
			if (filled_ >= utf8_byte_order_mark_length
				&& std::memcmp(utf8_byte_order_mark.data(), data_, utf8_byte_order_mark_length) == 0)
			{
				data_ += utf8_byte_order_mark_length;
				filled_ -= utf8_byte_order_mark_length;
			}
		}

#if TOML_HAS_CHAR8

		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::u8string_view doc, String&& source_path) //
			: source_reader{ std::string_view{ reinterpret_cast<const char*>(doc.data()), doc.length() },
							 static_cast<String&&>(source_path) }
		{}

#endif

		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::istream& doc, String&& source_path) //
			: stream_{ doc },
			  window_{ new char[initial_window] },
			  capacity_{ initial_window },
			  source_path_{ make_source_path(static_cast<String&&>(source_path)) }
		{
			data_ = window_.get();
		}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept
		{
			return source_path_;
		}

		// the wall is the first byte that can't be read: invalid utf-8, a cut sequence or a failed read
		TOML_PURE_INLINE_GETTER
		bool at_wall() const noexcept
		{
			return base_ + pos_ == wall_;
		}

		TOML_PURE_GETTER
		const char* wall_reason() const noexcept
		{
			TOML_ASSERT(at_wall());

			if (wall_kind_ == wall_kind::invalid)
				return "Encountered invalid UTF-8 sequence";
			if (wall_kind_ == wall_kind::cut_sequence)
				return "Encountered end-of-file during incomplete UTF-8 code point sequence";
			if (wall_kind_ == wall_kind::zero_bytes)
				return "Reading from the underlying stream failed - zero bytes read";
			return "An I/O error occurred while reading from the underlying stream";
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		void rethrow_stream_failure() const
		{
			if (wall_kind_ == wall_kind::zero_bytes || wall_kind_ == wall_kind::io_error)
				stream_.rethrow_failure();
		}

#endif

		TOML_PURE_INLINE_GETTER
		source_position position() const noexcept
		{
			return { line_, static_cast<source_index>(base_ + pos_ - line_start_ - continuation_ + 1u) };
		}

		TOML_PURE_GETTER
		source_position eof_anchor() const noexcept
		{
			const auto at = base_ + pos_;
			if (!at)
				return { 1, 1 };
			if (at == line_start_)
				return { line_ - 1u, lf_column_ };
			return { line_, static_cast<source_index>(at - line_start_ - continuation_) };
		}

		TOML_PURE_INLINE_GETTER
		lookahead_pos lookahead() const noexcept
		{
			return { base_ + pos_, current_.count, current_.value };
		}

		TOML_NODISCARD
		const utf8_codepoint* first() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			return land_slow();
		}

		TOML_NODISCARD
		const utf8_codepoint* next() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (current_.value == U'\n')
			{
				lf_column_ = position().column;
				line_++;
				line_start_	  = base_ + pos_ + 1u;
				continuation_ = {};
			}
			else
				continuation_ += current_.count - 1u;
			pos_ += current_.count;
			return land();
		}

		template <run_class Class>
		TOML_NODISCARD
		run_span run() const noexcept
		{
			static constexpr auto mask		= static_cast<unsigned>(Class);
			static constexpr bool non_ascii = (run_bytes.classes[0x80u] & mask) != 0u;
			TOML_ASSERT(pos_ < valid_);

			auto end			= pos_ + current_.count;
			size_t continuation = current_.count - 1u;
			if constexpr (non_ascii)
			{
#if TOML_HAS_SSE2
				while (valid_ - end >= 16u)
				{
					const auto bytes   = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data_ + end));
					const auto stops   = run_stops<Class>(bytes);
					auto continuations = static_cast<unsigned>(
						_mm_movemask_epi8(_mm_cmplt_epi8(bytes, _mm_set1_epi8(static_cast<char>(-64)))));
					if (stops)
					{
						continuations &= (stops & (~stops + 1u)) - 1u;
						return { data_ + pos_,
								 end + trailing_zeros(stops) - pos_,
								 continuation + count_bits(continuations) };
					}
					if (continuations)
						continuation += count_bits(continuations);
					end += 16u;
				}
#endif
				while (valid_ - end >= 8u)
				{
					const auto word	   = load_word(data_ + end);
					const auto stops   = run_stops<Class>(word);
					auto continuations = word & ~(word << 1) & word_high_bits;
					if (stops)
					{
						continuations &= (stops & (~stops + 1u)) - 1u;
						return { data_ + pos_,
								 end + (trailing_zeros(stops) / 8u) - pos_,
								 continuation + count_high_bits(continuations) };
					}
					continuation += count_high_bits(continuations);
					end += 8u;
				}
			}
			while (end < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[end]);
				if (!(run_bytes.classes[byte] & mask))
					break;
				continuation += non_ascii && (byte & 0xC0u) == 0x80u;
				end++;
			}
			return { data_ + pos_, end - pos_, continuation };
		}

		TOML_NODISCARD
		const utf8_codepoint* skip(const run_span& span) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			TOML_ASSERT(span.data == data_ + pos_);
			TOML_ASSERT(span.length >= current_.count);
#ifndef NDEBUG
			size_t continuation = {};
			for (size_t i = pos_, end = pos_ + span.length; i < end;)
			{
				char32_t value	 = {};
				const auto count = decode(i, value);
				TOML_ASSERT(value != U'\n');
				continuation += count - 1u;
				i += count;
				TOML_ASSERT(i <= end);
			}
			TOML_ASSERT(continuation == span.continuation);
#endif
			continuation_ += span.continuation;
			pos_ += span.length;
			return land();
		}

		void step(lookahead_pos& la) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			la.offset += la.count;
			const auto at = static_cast<size_t>(la.offset - base_);
			if (at < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[at]);
				if (byte < 0x80u)
				{
					la.value = byte;
					la.count = 1u;
					return;
				}
			}
			step_slow(la);
		}
	};
}
TOML_ANON_NAMESPACE_END;

#if TOML_EXCEPTIONS
#define TOML_RETURNS_BY_THROWING [[noreturn]]
#else
#define TOML_RETURNS_BY_THROWING
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename... T>
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr bool is_match(char32_t codepoint, T... vals) noexcept
	{
		static_assert((std::is_same_v<char32_t, T> && ...));
		return ((codepoint == vals) || ...);
	}

	template <uint64_t>
	struct parse_integer_traits;
	template <>
	struct parse_integer_traits<2>
	{
		static constexpr auto scope_qualifier  = "binary integer"sv;
		static constexpr auto digit_name	   = "binary digit"sv;
		static constexpr auto is_digit		   = impl::is_binary_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 63;
		static constexpr auto prefix_codepoint = U'b';
		static constexpr auto prefix		   = "b"sv;
		static constexpr auto full_prefix	   = "0b"sv;
	};
	template <>
	struct parse_integer_traits<8>
	{
		static constexpr auto scope_qualifier  = "octal integer"sv;
		static constexpr auto digit_name	   = "octal digit"sv;
		static constexpr auto is_digit		   = impl::is_octal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 21; // strlen("777777777777777777777")
		static constexpr auto prefix_codepoint = U'o';
		static constexpr auto prefix		   = "o"sv;
		static constexpr auto full_prefix	   = "0o"sv;
	};
	template <>
	struct parse_integer_traits<10>
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto digit_name	  = "decimal digit"sv;
		static constexpr auto is_digit		  = impl::is_decimal_digit;
		static constexpr auto is_signed		  = true;
		static constexpr auto max_digits	  = 19; // strlen("9223372036854775807")
		static constexpr auto full_prefix	  = ""sv;
	};
	template <>
	struct parse_integer_traits<16>
	{
		static constexpr auto scope_qualifier  = "hexadecimal integer"sv;
		static constexpr auto digit_name	   = "hexadecimal digit"sv;
		static constexpr auto is_digit		   = impl::is_hexadecimal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 16; // strlen("7FFFFFFFFFFFFFFF")
		static constexpr auto prefix_codepoint = U'x';
		static constexpr auto prefix		   = "x"sv;
		static constexpr auto full_prefix	   = "0x"sv;
	};

	// false if the value doesn't fit in an int64_t
	template <uint64_t Base>
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	bool digits_to_int64(std::string_view digits, bool negative, int64_t& result) noexcept
	{
		using traits = parse_integer_traits<Base>;

		const auto leading_zeroes = digits.find_first_not_of('0');
		digits.remove_prefix(leading_zeroes == std::string_view::npos ? digits.length() : leading_zeroes);
		if TOML_UNLIKELY(digits.length() > static_cast<size_t>(traits::max_digits))
			return false;

		uint64_t magnitude = {};
		for (const char digit : digits)
		{
			if constexpr (Base == 16)
				magnitude = (magnitude * Base) + impl::hex_to_dec(digit);
			else
				magnitude = (magnitude * Base) + static_cast<uint64_t>(digit - '0');
		}

		static constexpr auto i64_max = static_cast<uint64_t>((std::numeric_limits<int64_t>::max)());
		if TOML_UNLIKELY(magnitude > i64_max + (negative ? 1u : 0u))
			return false;

		if (!negative)
			result = static_cast<int64_t>(magnitude);
		else if (magnitude > i64_max)
			result = (std::numeric_limits<int64_t>::min)();
		else
			result = -static_cast<int64_t>(magnitude);
		return true;
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(node_type val) noexcept
	{
		return impl::node_type_friendly_names[impl::unwrap_enum(val)];
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const std::string& str) noexcept
	{
		return std::string_view{ str };
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(bool val) noexcept
	{
		return val ? "true"sv : "false"sv;
	}

	// code points would otherwise convert to bool
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint&) = delete;
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint*) = delete;

	// what the longest uint64_t takes
	TOML_INTERNAL_LINKAGE
	constexpr size_t max_decimal_digits = 20u;

	// writes the digits so that they stop at end, and returns where they start
	TOML_NODISCARD
	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	char* write_decimal_digits(char* end, uint64_t val) noexcept
	{
		do
		{
			*--end = static_cast<char>('0' + (val % 10u));
			val /= 10u;
		}
		while (val);
		return end;
	}

	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, const char* const buf_end, std::string_view text) noexcept
	{
		if TOML_UNLIKELY(write_pos >= buf_end)
			return;

		const auto max_chars = static_cast<size_t>(buf_end - write_pos);
		auto len			 = max_chars < text.length() ? max_chars : text.length();

		// don't split a code point when truncating
		while (len && len < text.length() && (static_cast<unsigned char>(text[len]) & 0xC0u) == 0x80u)
			len--;

		std::memcpy(write_pos, text.data(), len);
		write_pos += len;
	}

	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, const char* const buf_end, uint64_t magnitude) noexcept
	{
		char text[max_decimal_digits];
		char* const text_end		 = text + sizeof(text);
		const char* const text_begin = write_decimal_digits(text_end, magnitude);
		concatenate(write_pos, buf_end, std::string_view{ text_begin, static_cast<size_t>(text_end - text_begin) });
	}

	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, const char* const buf_end, const utf8_codepoint& cp) noexcept
	{
		if (cp.value <= U'\x1F')
			concatenate(write_pos, buf_end, impl::control_char_escapes[cp.value]);
		else if (cp.value == U'\x7F')
			concatenate(write_pos, buf_end, "\\u007F"sv);
		else if (cp.value <= U'\x7F'
				 || (cp.value > U'\x9F' && !impl::is_whitespace(cp.value) && !impl::is_default_ignorable(cp.value)))
			concatenate(write_pos, buf_end, std::string_view{ cp.bytes, cp.count });
		else
		{
			auto val			= static_cast<uint_least32_t>(cp.value);
			const auto digits	= val > 0xFFFFu ? 8u : 4u;
			constexpr auto mask = uint_least32_t{ 0xFu };
			char buf[10]		= { '\\', digits > 4 ? 'U' : 'u' };
			for (auto i = 2u + digits; i-- > 2u;)
			{
				const auto hexdig = val & mask;
				buf[i]			  = static_cast<char>(hexdig >= 0xAu ? ('A' + (hexdig - 0xAu)) : ('0' + hexdig));
				val >>= 4;
			}
			concatenate(write_pos, buf_end, std::string_view{ buf, digits + 2u });
		}
	}

	// anything else would convert to one of the three
	template <typename T>
	void concatenate(char*&, const char*, const T&) = delete;

	struct error_arg
	{
		enum class kinds : unsigned char
		{
			string,
			signed_integer,
			unsigned_integer,
			codepoint
		};

		const void* ptr = {};
		uint64_t val	= {};
		kinds kind		= {};
	};

	template <typename T>
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	error_arg make_error_arg(const T& arg) noexcept
	{
		constexpr bool is_integer =
			std::is_integral_v<T> && !impl::is_one_of<T, bool, char, wchar_t, char16_t, char32_t>;
		if constexpr (std::is_same_v<T, std::string_view>)
			return { arg.data(), static_cast<uint64_t>(arg.length()), error_arg::kinds::string };
		else if constexpr (std::is_same_v<T, utf8_codepoint>)
			return { &arg, {}, error_arg::kinds::codepoint };
		else if constexpr (std::is_same_v<T, const utf8_codepoint*>)
			return { arg, {}, error_arg::kinds::codepoint };
		else if constexpr (is_integer && std::is_signed_v<T>)
			return { {}, static_cast<uint64_t>(static_cast<int64_t>(arg)), error_arg::kinds::signed_integer };
		else if constexpr (is_integer)
			return { {}, static_cast<uint64_t>(arg), error_arg::kinds::unsigned_integer };
		else
			static_assert(impl::always_false<T>,
						  "error inputs are limited to std::string_views, integers and code points");
	}

	struct error_builder
	{
		static constexpr size_t buf_size = 512;
		char buf[buf_size];
		char* write_pos			  = buf;
		char* const max_write_pos = buf + (buf_size - size_t{ 1 }); // allow for null terminator

		TOML_NODISCARD_CTOR
		explicit error_builder(std::string_view scope) noexcept
		{
			concatenate(write_pos, max_write_pos, "Error while parsing "sv);
			concatenate(write_pos, max_write_pos, scope);
			concatenate(write_pos, max_write_pos, ": "sv);
		}

		void append(const error_arg& arg) noexcept
		{
			switch (arg.kind)
			{
				case error_arg::kinds::string:
					concatenate(write_pos,
								max_write_pos,
								std::string_view{ static_cast<const char*>(arg.ptr), static_cast<size_t>(arg.val) });
					break;
				case error_arg::kinds::signed_integer:
					if (static_cast<int64_t>(arg.val) < 0)
					{
						concatenate(write_pos, max_write_pos, "-"sv);
						concatenate(write_pos, max_write_pos, 0u - arg.val);
						break;
					}
					[[fallthrough]];
				case error_arg::kinds::unsigned_integer: concatenate(write_pos, max_write_pos, arg.val); break;
				case error_arg::kinds::codepoint:
					if (arg.ptr)
						concatenate(write_pos, max_write_pos, *static_cast<const utf8_codepoint*>(arg.ptr));
					break;
				default: TOML_UNREACHABLE;
			}
		}

		TOML_RETURNS_BY_THROWING
#if !TOML_EXCEPTIONS
		TOML_NODISCARD
#endif
		auto finish(source_position pos, const source_path_ptr& source_path)
		{
			*write_pos = '\0';

#if TOML_EXCEPTIONS
			throw parse_error{ buf, pos, source_path };
#else
			return parse_error{ std::string(buf, static_cast<size_t>(write_pos - buf)), pos, source_path };
#endif
		}

		TOML_DELETE_DEFAULTS(error_builder);
	};

	class parse_scope
	{
	  private:
		std::string_view& storage_;
		std::string_view parent_;

	  public:
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

		TOML_DELETE_DEFAULTS(parse_scope);
	};
#define push_parse_scope_2(scope, line) const parse_scope ps_##line(current_scope, scope)
#define push_parse_scope_1(scope, line) push_parse_scope_2(scope, line)
#define push_parse_scope(scope)			push_parse_scope_1(scope, __LINE__)

	struct parse_key_buffer
	{
		struct segment
		{
			size_t offset;
			size_t length;
			source_position begin;
			source_position end;
		};

		std::string buffer;
		std::vector<segment> segments;

		void clear() noexcept
		{
			buffer.clear();
			segments.clear();
		}

		void push_back(std::string_view text, source_position begin, source_position end)
		{
			segments.push_back({ buffer.length(), text.length(), begin, end });
			buffer.append(text);
		}

		TOML_PURE_INLINE_GETTER
		std::string_view operator[](size_t i) const noexcept
		{
			return std::string_view{ buffer.c_str() + segments[i].offset, segments[i].length };
		}

		TOML_PURE_INLINE_GETTER
		std::string_view back() const noexcept
		{
			return (*this)[segments.size() - 1u];
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return segments.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return segments.size();
		}
	};

	class depth_counter_scope
	{
	  private:
		size_t& depth_;
		size_t previous_;

	  public:
		TOML_NODISCARD_CTOR
		explicit depth_counter_scope(size_t& depth, size_t amount = 1u) noexcept //
			: depth_{ depth },
			  previous_{ depth }
		{
			depth_ += amount;
		}

		// restores instead of decrementing, so msvc destroying it twice when built as a module is harmless (#288)
		~depth_counter_scope() noexcept
		{
			depth_ = previous_;
		}

		TOML_DELETE_DEFAULTS(depth_counter_scope);
	};

	struct parsed_string
	{
		std::string_view value;
		bool was_multi_line;
	};

	class node_set
	{
	  private:
		static constexpr size_t max_pending	   = 16;
		static constexpr unsigned initial_bits = 6;

		std::vector<const node*> pending_;
		std::vector<const node*> slots_;
		size_t count_  = {};
		unsigned bits_ = {};

		TOML_PURE_INLINE_GETTER
		size_t home(const node* n) const noexcept
		{
			const auto address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(n));
			return static_cast<size_t>((address * 0x9E3779B97F4A7C15ull) >> (64u - bits_));
		}

		TOML_PURE_GETTER
		size_t slot_of(const node* n) const noexcept
		{
			const auto mask = slots_.size() - 1u;
			auto i			= home(n);
			while (slots_[i] && slots_[i] != n)
				i = (i + 1u) & mask;
			return i;
		}

		void flush()
		{
			auto bits = slots_.empty() ? initial_bits : bits_;
			while ((count_ + pending_.size()) * 2u > (size_t{ 1 } << bits))
				bits++;
			if (bits != bits_)
			{
				std::vector<const node*> old(size_t{ 1 } << bits);
				old.swap(slots_);
				bits_ = bits;
				for (auto n : old)
					if (n)
						slots_[slot_of(n)] = n;
			}

			for (auto n : pending_)
			{
				auto& slot = slots_[slot_of(n)];
				if (!slot)
					count_++;
				slot = n;
			}
			pending_.clear();
		}

	  public:
		void insert(const node* n)
		{
			TOML_ASSERT(n);
			pending_.push_back(n);
		}

		TOML_NODISCARD
		bool contains(const node* n)
		{
			TOML_ASSERT(n);
			if (pending_.size() > max_pending)
				flush();
			for (auto p : pending_)
				if (p == n)
					return true;
			return !slots_.empty() && slots_[slot_of(n)] == n;
		}

		void erase(const node* n) noexcept
		{
			for (auto& p : pending_)
			{
				if (p == n)
				{
					p = pending_.back();
					pending_.pop_back();
					return;
				}
			}
			if (slots_.empty())
				return;
			auto hole = slot_of(n);
			if (slots_[hole] != n)
				return;

			// entries after the hole move back into it unless that would put them before their home slot
			const auto mask = slots_.size() - 1u;
			for (auto i = (hole + 1u) & mask; slots_[i]; i = (i + 1u) & mask)
			{
				if (((i - home(slots_[i])) & mask) >= ((i - hole) & mask))
				{
					slots_[hole] = slots_[i];
					hole		 = i;
				}
			}
			slots_[hole] = nullptr;
			count_--;
		}
	};

	template <size_t N>
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	bool decimal_to_double(const char (&chars)[N], size_t length, double& result) noexcept
	{
		// 'e', '-', thirteen exponent digits and '\0'
		static constexpr size_t extra_chars		= 16u;
		static constexpr int64_t exponent_clamp = 1'000'000'000;
		// a digit can follow the clamp, and each fraction digit moves the exponent by one
		static_assert((exponent_clamp * 10) + 9 + static_cast<int64_t>(N) < 10'000'000'000'000);

		// strtod takes its decimal point from setlocale(), so the digits go in without one
		char text[N + extra_chars];
		size_t text_length = {};
		int64_t exponent   = {};
		bool in_fraction   = {};
		size_t i		   = {};
		for (; i < length && chars[i] != 'e' && chars[i] != 'E'; i++)
		{
			if (chars[i] == '.')
				in_fraction = true;
			else
			{
				text[text_length++] = chars[i];
				if (in_fraction)
					exponent--;
			}
		}

		if (i < length)
		{
			i++;
			const bool negative = chars[i] == '-';
			if (chars[i] == '+' || chars[i] == '-')
				i++;
			int64_t written = {};
			for (; i < length; i++)
				if (written < exponent_clamp)
					written = (written * 10) + (chars[i] - '0');
			exponent += negative ? -written : written;
		}

		text[text_length++] = 'e';
		if (exponent < 0)
		{
			text[text_length++] = '-';
			exponent			= -exponent;
		}
		char exponent_text[max_decimal_digits];
		char* const exponent_end = exponent_text + sizeof(exponent_text);
		for (const char* digit = write_decimal_digits(exponent_end, static_cast<uint64_t>(exponent));
			 digit != exponent_end;
			 digit++)
			text[text_length++] = *digit;
		text[text_length] = '\0';

		char* end = {};
		result	  = std::strtod(text, &end);
		return end == text + text_length && impl::fpclassify(result) != impl::fp_class::pos_inf;
	}

#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

	struct hex_float_fragment
	{
		char chars[24];
		size_t length;
	};

	// the fragments are the mantissa's whole part, its fractional part and the exponent.
	// infinity if the value is too large for a double
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	double hex_fragments_to_double(const hex_float_fragment(&fragments)[3], bool negative_exponent) noexcept
	{
		uint64_t mantissa	= {};
		int binary_exponent = {};
		bool inexact		= {};
		for (size_t fragment_idx = 0; fragment_idx < 2u; fragment_idx++)
		{
			const auto& f = fragments[fragment_idx];
			for (size_t i = 0; i < f.length; i++)
			{
				if (mantissa >> 60)
				{
					inexact = inexact || f.chars[i] != '0';
					if (fragment_idx == 0u)
						binary_exponent += 4;
					continue;
				}
				mantissa = (mantissa << 4) | impl::hex_to_dec(f.chars[i]);
				if (fragment_idx == 1u)
					binary_exponent -= 4;
			}
		}
		if (inexact)
			mantissa |= 1u;

		// far past any exponent that leaves a finite double, and small enough that the loop can't overflow an int
		static constexpr int exponent_clamp = 100000;
		int exponent						= {};
		for (size_t i = 0; i < fragments[2].length && exponent < exponent_clamp; i++)
			exponent = (exponent * 10) + (fragments[2].chars[i] - '0');
		binary_exponent += negative_exponent ? -exponent : exponent;

		// round once, to 53 bits or the subnormal grid, so the ldexp below is exact;
		// converting to double first rounds subnormals twice
		if (mantissa)
		{
			constexpr int digits		= std::numeric_limits<double>::digits;
			constexpr int lowest_binary = std::numeric_limits<double>::min_exponent - digits;

			int msb = 63;
			while (!(mantissa >> msb))
				msb--;
			const int drop =
				msb + 1 - digits > lowest_binary - binary_exponent ? msb + 1 - digits : lowest_binary - binary_exponent;
			if (drop > 64)
				mantissa = 0u;
			else if (drop > 0)
			{
				const uint64_t rem	= drop == 64 ? mantissa : mantissa & ((uint64_t{ 1 } << drop) - 1u);
				const uint64_t half = uint64_t{ 1 } << (drop - 1);
				mantissa			= drop == 64 ? 0u : mantissa >> drop;
				if (rem > half || (rem == half && (mantissa & 1u)))
					mantissa++;
			}
			if (drop > 0)
				binary_exponent += drop;
		}

		return impl::ldexp(static_cast<double>(mantissa), binary_exponent);
	}

#endif // TOML_LANG_UNRELEASED
}
TOML_ANON_NAMESPACE_END;

// Q: "what the fuck is this? MACROS????"
// A: The parser needs to work in exceptionless mode (returning error objects directly)
//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
//    These macros encapsulate the differences between the two modes so I can write code
//    as though I was only targeting one mode and not want to yeet myself into the sun.
//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
//    of toml++.

#define is_eof()		 (!cp)
#define assert_not_eof() TOML_ASSERT_ASSUME(cp != nullptr)
// clang-format off
#define return_if_eof(...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
// clang-format on

#if TOML_EXCEPTIONS
#define is_error()					false
#define return_after_error(...)		TOML_UNREACHABLE
#define assert_not_error()			static_assert(true)
#define return_if_error(...)		static_assert(true)
#define return_if_error_or_eof(...) return_if_eof(__VA_ARGS__)
#else
#define is_error()				(!!err)
#define return_after_error(...) return __VA_ARGS__
#define assert_not_error()		TOML_ASSERT(!is_error())
// clang-format off
#define return_if_error(...)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_error())                                                                                   \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
#define return_if_error_or_eof(...)                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof() || is_error())                                                                       \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
// clang-format on
#endif

#if defined(TOML_BREAK_AT_PARSE_ERRORS) && TOML_BREAK_AT_PARSE_ERRORS
#if defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
#define parse_error_break() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define parse_error_break() __debugbreak()
#endif
#endif
#ifndef parse_error_break
#if TOML_MSVC || TOML_ICC
#define parse_error_break() __debugbreak()
#else
#define parse_error_break() TOML_ASSERT(false)
#endif
#endif
#else
#define parse_error_break() static_assert(true)
#endif

#define set_error_and_return_default(...)                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!is_error())                                                                                               \
			set_error(__VA_ARGS__);                                                                                    \
		return_after_error({});                                                                                        \
	}                                                                                                                  \
	while (false)

#define set_error_at_and_return_default(pos, ...)                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		set_error_at(pos, __VA_ARGS__);                                                                                \
		return_after_error({});                                                                                        \
	}                                                                                                                  \
	while (false)

// clang-format off
#define set_error_and_return_if_eof(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
		{                                                                                                              \
			if (!is_error())                                                                                           \
				set_eof_error();                                                                                       \
			return_after_error(__VA_ARGS__);                                                                           \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)
// clang-format on

#define advance_and_return_if_error(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
	}                                                                                                                  \
	while (false)

#define advance_and_return_if_error_or_unexpected_eof(...)                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
		set_error_and_return_if_eof(__VA_ARGS__);                                                                      \
	}                                                                                                                  \
	while (false)

TOML_IMPL_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);

	class parser
	{
	  private:
		static constexpr size_t max_nested_values = TOML_MAX_NESTED_VALUES;
		static constexpr size_t max_value_length  = 128;
		// how far parse_value() looks ahead to work out a value's type
		static constexpr size_t max_value_scan_length = max_value_length - 1u;

		// NOLINTBEGIN(readability-identifier-naming)
		source_reader reader;
		table root;
		const utf8_codepoint* cp = {};
		node_set implicit_tables;
		node_set dotted_key_tables;
		node_set table_arrays;
		parse_key_buffer key_buffer;
		std::string string_buffer;
		std::string recording_buffer; // for diagnostics
		bool recording = false, recording_whitespace = true;
		std::string_view current_scope;
		size_t nested_values = {};
#if !TOML_EXCEPTIONS
		mutable optional<parse_error> err;
#endif
		// NOLINTEND(readability-identifier-naming)

		TOML_NODISCARD
		source_position current_position(source_index fallback_offset = 0) const noexcept
		{
			if (!is_eof())
				return reader.position();
			auto pos = reader.eof_anchor();
			pos.column += fallback_offset;
			return pos;
		}

		// a stream's exception is rethrown instead, as a parse_error when exceptions are enabled
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_wall_error() const
		{
			const auto pos = reader.position();
#if TOML_EXCEPTIONS
			try
			{
				reader.rethrow_stream_failure();
			}
			catch (const std::exception& exc)
			{
				throw parse_error{ exc.what(), pos, reader.source_path() };
			}
			catch (...)
			{
				throw parse_error{ "An unspecified error occurred", pos, reader.source_path() };
			}
			throw parse_error{ reader.wall_reason(), pos, reader.source_path() };
#else
#if TOML_COMPILER_HAS_EXCEPTIONS
			reader.rethrow_stream_failure();
#endif
			err.emplace(reader.wall_reason(), pos, reader.source_path());
#endif
		}

		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at_impl(source_position pos, const error_arg* args, size_t count) const
		{
			return_if_error();

			error_builder builder{ current_scope };
			for (size_t i = 0; i < count; i++)
				builder.append(args[i]);

			parse_error_break();

#if TOML_EXCEPTIONS
			builder.finish(pos, reader.source_path());
#else
			err.emplace(builder.finish(pos, reader.source_path()));
#endif
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at(source_position pos, const T&... reason) const
		{
			static_assert(sizeof...(T) > 0);
			const error_arg args[] = { make_error_arg(reason)... };
			set_error_at_impl(pos, args, sizeof...(T));
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		void set_error(const T&... reason) const
		{
			set_error_at(current_position(1), reason...);
		}

		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_eof_error() const
		{
			set_error("encountered end-of-file"sv);
		}

		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_depth_error(source_position pos) const
		{
			set_error_at(pos,
						 "exceeded maximum nested value depth of "sv,
						 max_nested_values,
						 " (TOML_MAX_NESTED_VALUES)"sv);
		}

		void land(const utf8_codepoint* next)
		{
			cp = next;
			if TOML_UNLIKELY(!cp)
			{
				if (reader.at_wall())
					set_wall_error();
				return;
			}
			if (recording && (recording_whitespace || !is_whitespace(*cp)))
				recording_buffer.append(cp->bytes, cp->count);
		}

		void advance()
		{
			return_if_error();
			assert_not_eof();

			land(reader.next());
		}

		template <run_class Class>
		void advance_by(const run_span& span)
		{
			return_if_error();
			assert_not_eof();

			if (recording && span.length > cp->count)
			{
				// a run mixing whitespace with other code points can only be recorded whole
				TOML_ASSERT(Class == run_class::bare_key || recording_whitespace);
				if (Class == run_class::bare_key || recording_whitespace)
					recording_buffer.append(span.data + cp->count, span.length - cp->count);
			}

			land(reader.skip(span));
		}

		void start_recording() noexcept
		{
			return_if_error();

			recording			 = true;
			recording_whitespace = true;
			recording_buffer.clear();
			if (!is_eof())
				recording_buffer.append(cp->bytes, cp->count);
		}

		void stop_recording() noexcept
		{
			return_if_error();

			recording = false;
		}

		void stop_recording_key() noexcept
		{
			stop_recording();
			if (!is_error() && !recording_buffer.empty())
				recording_buffer.pop_back();
			while (!recording_buffer.empty() && (recording_buffer.back() == ' ' || recording_buffer.back() == '\t'))
				recording_buffer.pop_back();
		}

		struct recording_scope
		{
			parser& parser_;

			TOML_NODISCARD_CTOR
			explicit recording_scope(parser& p) noexcept //
				: parser_{ p }
			{
				parser_.start_recording();
			}

			~recording_scope() noexcept
			{
				parser_.stop_recording();
			}

			TOML_DELETE_DEFAULTS(recording_scope);
		};

		TOML_NODISCARD
		std::string_view recorded_before_current() const noexcept
		{
			TOML_ASSERT_ASSUME(cp);
			TOML_ASSERT(recording_buffer.length() >= cp->count);
			return std::string_view{ recording_buffer }.substr(0u, recording_buffer.length() - cp->count);
		}

		bool consume_whitespace()
		{
			return_if_error_or_eof({});

			bool consumed = {};
			while (!is_eof() && is_horizontal_whitespace(*cp))
			{
				if TOML_UNLIKELY(!is_ascii_horizontal_whitespace(*cp))
					set_error_and_return_default("expected space or tab, saw '"sv, *cp, "'"sv);

				consumed = true;
				advance_and_return_if_error({});
			}
			return consumed;
		}

		bool consume_line_break()
		{
			return_if_error_or_eof({});

			if TOML_UNLIKELY(is_match(*cp, U'\v', U'\f'))
				set_error_and_return_default(
					R"(vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);

			if (*cp == U'\r')
			{
				advance_and_return_if_error({}); // skip \r

				if TOML_UNLIKELY(is_eof())
					set_error_and_return_default("expected '\\n' after '\\r', saw end-of-file"sv);

				if TOML_UNLIKELY(*cp != U'\n')
					set_error_and_return_default("expected '\\n' after '\\r', saw '"sv, *cp, "'"sv);
			}
			else if (*cp != U'\n')
				return false;

			advance_and_return_if_error({}); // skip \n
			return true;
		}

		bool consume_comment()
		{
			return_if_error_or_eof({});

			if (*cp != U'#')
				return false;

			push_parse_scope("comment"sv);

			advance_and_return_if_error({}); // skip the '#'

			while (!is_eof())
			{
				if (consume_line_break())
					return true;
				return_if_error({});

				// toml/issues/567 (disallow non-TAB control characters in comments)
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are prohibited in comments, saw '"sv,
						*cp,
						"'"sv);

				advance_by<run_class::comment>(reader.run<run_class::comment>());
				return_if_error({});
			}

			return true;
		}

		bool consume_rest_of_line()
		{
			return_if_error({});

			consume_whitespace();
			return_if_error({});
			if (!is_eof() && !consume_comment() && !consume_line_break())
				set_error_and_return_default("expected a comment or line break, saw '"sv, *cp, "'"sv);
			return true;
		}

		TOML_NODISCARD
		bool consume_expected_sequence(std::u32string_view seq)
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
		TOML_NODISCARD
		bool consume_digit_sequence(T* digits, size_t len)
		{
			return_if_error({});
			TOML_ASSERT_ASSUME(digits);
			TOML_ASSERT_ASSUME(len);

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

		TOML_NODISCARD
		uint32_t consume_two_digits(std::string_view name, uint32_t min, uint32_t max)
		{
			return_if_error({});

			const auto pos = current_position();
			uint32_t digits[2];
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit "sv, name, ", saw '"sv, *cp, "'"sv);
			const auto value = digits[1] + (digits[0] * 10u);
			if (value < min || value > max)
				set_error_at_and_return_default(pos,
												"expected "sv,
												name,
												" between "sv,
												min,
												" and "sv,
												max,
												" (inclusive), saw "sv,
												value);
			return value;
		}

		template <typename T>
		TOML_NODISCARD
		size_t consume_variable_length_digit_sequence(T* buffer, size_t max_len)
		{
			return_if_error({});
			TOML_ASSERT_ASSUME(buffer);
			TOML_ASSERT_ASSUME(max_len);

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

		// true if the delimiters closed the string; the ones that are text go into string_buffer
		TOML_NODISCARD
		bool consume_multi_line_string_delimiters(char delimiter)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == static_cast<char32_t>(delimiter));

			size_t lookaheads			  = {};
			size_t consecutive_delimiters = 1;
			do
			{
				advance_and_return_if_error({});
				lookaheads++;
				if (!is_eof() && *cp == static_cast<char32_t>(delimiter))
					consecutive_delimiters++;
				else
					break;
			}
			while (lookaheads < 4u);

			switch (consecutive_delimiters)
			{
				// """ " (one quote somewhere in a ML string)
				case 1: string_buffer += delimiter; return false;

				// """ "" (two quotes somewhere in a ML string)
				case 2: string_buffer.append(2u, delimiter); return false;

				// """ """ (the end of the string)
				case 3: return true;

				// """ """" (one at the end of the string)
				case 4: string_buffer += delimiter; return true;

				// """ """"" (two quotes at the end of the string)
				case 5:
					string_buffer.append(2u, delimiter);
					advance_and_return_if_error({}); // skip the last delimiter
					return true;

				default: TOML_UNREACHABLE;
			}
		}

		bool parse_unicode_escape(std::string& str)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'x', U'u', U'U'));
			push_parse_scope("unicode scalar sequence"sv);

			auto escape_pos = current_position();
			escape_pos.column--;
			uint32_t place_value = *cp == U'U' ? 0x10000000u : (*cp == U'u' ? 0x1000u : 0x10u);
			advance_and_return_if_error_or_unexpected_eof({});

			uint32_t sequence_value = {};
			while (place_value)
			{
				set_error_and_return_if_eof({});

				if TOML_UNLIKELY(!is_hexadecimal_digit(*cp))
					set_error_and_return_default("expected hexadecimal digit, saw '"sv, *cp, "'"sv);

				sequence_value += place_value * hex_to_dec(*cp);
				place_value /= 16u;
				advance_and_return_if_error({});
			}

			if TOML_UNLIKELY(is_unicode_surrogate(sequence_value))
				set_error_at_and_return_default(escape_pos, "unicode surrogates (U+D800 - U+DFFF) are prohibited"sv);
			if TOML_UNLIKELY(sequence_value > 0x10FFFFu)
				set_error_at_and_return_default(escape_pos, "values greater than U+10FFFF are invalid"sv);

			if (sequence_value < 0x80u)
			{
				str += static_cast<char>(sequence_value);
			}
			else if (sequence_value < 0x800u)
			{
				str += static_cast<char>((sequence_value >> 6) | 0xC0u);
				str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
			}
			else if (sequence_value < 0x10000u)
			{
				str += static_cast<char>((sequence_value >> 12) | 0xE0u);
				str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
				str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
			}
			else
			{
				str += static_cast<char>((sequence_value >> 18) | 0xF0u);
				str += static_cast<char>(((sequence_value >> 12) & 0x3Fu) | 0x80u);
				str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
				str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
			}
			return true;
		}

		// cp is the character that follows the backslash
		bool parse_escape_sequence(std::string& str)
		{
			return_if_error({});
			assert_not_eof();

			switch (*cp)
			{
				// 'regular' escape codes
				case U'b': str += '\b'; break;
				case U'f': str += '\f'; break;
				case U'n': str += '\n'; break;
				case U'r': str += '\r'; break;
				case U't': str += '\t'; break;
				case U'"': str += '"'; break;
				case U'\\': str += '\\'; break;
				case U'e': str += '\x1B'; break;

				// unicode scalar sequences
				case U'x': [[fallthrough]];
				case U'u': [[fallthrough]];
				case U'U':
					return parse_unicode_escape(str);

				// ???
				TOML_UNLIKELY_CASE default:
					set_error_and_return_default("unknown escape sequence '\\"sv, *cp, "'"sv);
			}

			advance_and_return_if_error_or_unexpected_eof({});
			return true;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_basic_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'"');
			push_parse_scope("string"sv);

			// skip the '"'
			advance_and_return_if_error_or_unexpected_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			auto& str = string_buffer;
			str.clear();
			bool skipping_whitespace = {};
			do
			{
				// handle closing delimiters
				if (*cp == U'"')
				{
					if (multi_line)
					{
						if (consume_multi_line_string_delimiters('"'))
							return str;
						return_if_error({});
						skipping_whitespace = false;
						continue;
					}

					advance_and_return_if_error({}); // skip the closing delimiter
					return str;
				}

				// handle escapes
				if (*cp == U'\\')
				{
					advance_and_return_if_error_or_unexpected_eof({}); // skip the '\'
					skipping_whitespace = false;

					// handle 'line ending slashes' in multi-line mode
					if (multi_line && is_whitespace(*cp))
					{
						consume_whitespace();
						return_if_error({});
						set_error_and_return_if_eof({});

						if TOML_UNLIKELY(!consume_line_break())
							set_error_and_return_default(
								"line-ending backslashes must be the last non-whitespace character on the line"sv);

						skipping_whitespace = true;
						return_if_error({});
						continue;
					}

					parse_escape_sequence(str);
					return_if_error({});
					continue;
				}

				// handle line endings in multi-line mode
				if (multi_line && is_ascii_vertical_whitespace(*cp))
				{
					consume_line_break();
					return_if_error({});
					if (!skipping_whitespace)
						str += '\n';
					continue;
				}

				// handle control characters
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"unescaped control characters other than TAB (U+0009) are prohibited, saw '"sv,
						*cp,
						"'"sv);

				if (skipping_whitespace && is_ascii_horizontal_whitespace(*cp))
				{
					advance_and_return_if_error({});
					continue;
				}

				skipping_whitespace = false;
				const auto span		= reader.run<run_class::basic_string>();
				str.append(span.data, span.length);
				advance_by<run_class::basic_string>(span);
				return_if_error({});
			}
			while (!is_eof());

			set_eof_error();
			return_after_error({});
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_literal_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'\'');
			push_parse_scope("literal string"sv);

			// skip the delimiter
			advance_and_return_if_error_or_unexpected_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			auto& str = string_buffer;
			str.clear();
			do
			{
				// handle closing delimiters
				if (*cp == U'\'')
				{
					if (multi_line)
					{
						if (consume_multi_line_string_delimiters('\''))
							return str;
						return_if_error({});
						continue;
					}

					advance_and_return_if_error({}); // skip the closing delimiter
					return str;
				}

				// handle line endings in multi-line mode
				if (multi_line && is_ascii_vertical_whitespace(*cp))
				{
					consume_line_break();
					return_if_error({});
					str += '\n';
					continue;
				}

				// handle control characters
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default("control characters other than TAB (U+0009) are prohibited, saw '"sv,
												 *cp,
												 "'"sv);

				const auto span = reader.run<run_class::literal_string>();
				str.append(span.data, span.length);
				advance_by<run_class::literal_string>(span);
				return_if_error({});
			}
			while (!is_eof());

			set_eof_error();
			return_after_error({});
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		parsed_string parse_string()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp));
			push_parse_scope("string"sv);

			// the single-line parsers start on the opening delimiter, so peek instead of consuming
			const auto first = cp->value;
			auto la			 = reader.lookahead();
			reader.step(la);
			const auto second = la.value;
			if (second < eof_mark)
			{
				reader.step(la);
				if (la.value != eof_mark && (second != first || la.value != first))
					return { first == U'\'' ? parse_literal_string(false) : parse_basic_string(false), false };
			}

			advance_and_return_if_error_or_unexpected_eof({});
			advance_and_return_if_error({});

			// if we were eof at the third character then first and second need to be
			// the same string character (otherwise it's an unterminated string)
			if (is_eof())
			{
				if (second == first)
					return {};

				set_eof_error();
				return_after_error({});
			}

			// otherwise the first three characters are all the same string delimiter, so it's a multi-line string
			return { first == U'\'' ? parse_literal_string(true) : parse_basic_string(true), true };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_bare_key_segment()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp));

			string_buffer.clear();

			while (!is_eof() && is_bare_key_character(*cp))
			{
				const auto span = reader.run<run_class::bare_key>();
				string_buffer.append(span.data, span.length);
				advance_by<run_class::bare_key>(span);
				return_if_error({});
			}

			return string_buffer;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		bool parse_boolean()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U't', U'f', U'T', U'F'));
			push_parse_scope("boolean"sv);

			const recording_scope recording_keyword{ *this };
			const auto result = is_match(*cp, U't', U'T');
			if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				set_error_and_return_default("expected '"sv,
											 to_sv(result),
											 "', saw '"sv,
											 recorded_before_current(),
											 *cp,
											 "'"sv);

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return result;
		}

		TOML_PUSH_WARNINGS;
		TOML_DISABLE_CLANG_WARNING("-Wnan-infinity-disabled");

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_inf_or_nan()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
			push_parse_scope("floating-point"sv);

			const recording_scope recording_keyword{ *this };
			const bool negative = *cp == U'-';
			if (negative || *cp == U'+')
				advance_and_return_if_error_or_unexpected_eof({});

			const bool inf = is_match(*cp, U'i', U'I');
			if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				set_error_and_return_default("expected '"sv,
											 inf ? "inf"sv : "nan"sv,
											 "', saw '"sv,
											 recorded_before_current(),
											 *cp,
											 "'"sv);

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return inf ? (negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity())
					   : std::numeric_limits<double>::quiet_NaN();
		}

		TOML_POP_WARNINGS;

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'+', U'-', U'.') || is_decimal_digit(*cp));
			push_parse_scope("floating-point"sv);
			const auto begin_pos = current_position();

			// sign
			const int sign					 = *cp == U'-' ? -1 : 1;
			const std::string_view sign_text = *cp == U'-' ? "-"sv : (*cp == U'+' ? "+"sv : ""sv);
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_unexpected_eof({});

			// consume value chars
			char chars[max_value_length];
			size_t length			= {};
			char32_t prev			= {};
			bool seen_decimal		= {};
			bool seen_exponent		= {};
			char first_integer_part = {};
			source_position first_integer_pos{};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!is_decimal_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				if TOML_UNLIKELY(prev == U'_' && !is_decimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(length == sizeof(chars))
					set_error_and_return_default("exceeds length limit of "sv,
												 sizeof(chars),
												 " digits"sv,
												 (seen_exponent ? ""sv : " (consider using exponent notation)"sv));
				else if (*cp == U'.')
				{
					// .1
					// -.1
					// +.1 (no integer part)
					if (!first_integer_part)
						set_error_and_return_default("expected decimal digit, saw '.'"sv);

					// 1.0e+.10 (exponent cannot have '.')
					else if (seen_exponent)
						set_error_and_return_default(is_match(prev, U'e', U'E')
														 ? "expected exponent digit or sign, saw '.'"sv
														 : "expected exponent digit, saw '.'"sv);

					// 1.0.e+.10
					// 1..0
					// (multiple '.')
					else if (seen_decimal)
						set_error_and_return_default("expected decimal digit or exponent, saw '.'"sv);

					seen_decimal = true;
				}
				else if (is_match(*cp, U'e', U'E'))
				{
					if (!is_decimal_digit(prev) // +e5, 1.e5
						|| seen_exponent)		// 1.0ee+10 (multiple 'e')
						set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);

					seen_decimal  = true; // implied
					seen_exponent = true;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 1.-0 (sign in mantissa)
					if (!seen_exponent)
						set_error_and_return_default(!seen_decimal	? "expected decimal digit or '.', saw '"sv
													 : prev == U'.' ? "expected decimal digit, saw '"sv
																	: "expected decimal digit or exponent, saw '"sv,
													 *cp,
													 "'"sv);

					// 1.0e1-0 (misplaced exponent sign)
					else if (!is_match(prev, U'e', U'E'))
						set_error_and_return_default("expected exponent digit, saw '"sv, *cp, "'"sv);
				}
				else if (is_decimal_digit(*cp))
				{
					if (!seen_decimal)
					{
						if (!first_integer_part)
						{
							first_integer_part = cp->bytes[0];
							first_integer_pos  = current_position();
						}
						else if (first_integer_part == '0')
							set_error_at_and_return_default(first_integer_pos, "leading zeroes are prohibited"sv);
					}
				}
				else
					set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);

				chars[length++] = cp->bytes[0];
				prev			= *cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (prev == U'_')
				set_error_and_return_default("underscores must be followed by digits"sv);
			else if (is_match(prev, U'e', U'E', U'+', U'-', U'.'))
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);
			}

			// convert to double
			double result = {};
#if TOML_FLOAT_CHARCONV
			if TOML_LIKELY(std::from_chars(chars, chars + length, result).ec == std::errc{})
				return result * sign;
#endif
			if (decimal_to_double(chars, length, result))
				return result * sign;

			set_error_at_and_return_default(begin_pos,
											"'"sv,
											sign_text,
											std::string_view{ chars, length },
											"' is not representable in 64 bits"sv);
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_hex_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'0', U'+', U'-'));
			push_parse_scope("hexadecimal floating-point"sv);

#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

			const auto begin_pos = current_position();
			// the sign, "0x", the three fragments, '.', 'p' and the exponent's sign
			char text[3u + (3u * sizeof(hex_float_fragment::chars)) + 3u];
			size_t text_length = {};

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
			{
				text[text_length++] = cp->bytes[0];
				advance_and_return_if_error_or_unexpected_eof({});
			}

			// '0'
			if (*cp != U'0')
				set_error_and_return_default("expected '0', saw '"sv, *cp, "'"sv);
			text[text_length++] = '0';
			advance_and_return_if_error_or_unexpected_eof({});

			// 'x' or 'X'
			if (!is_match(*cp, U'x', U'X'))
				set_error_and_return_default("expected 'x' or 'X', saw '"sv, *cp, "'"sv);
			text[text_length++] = cp->bytes[0];
			advance_and_return_if_error_or_unexpected_eof({});

			// the forms of a C++ hexadecimal floating literal; one side of the point can be empty:
			// (<HEX DIGITS> [.]? | <HEX DIGITS>? [.] <HEX DIGITS>) [pP] [+-]? <DEC DIGITS>

			// consume value fragments
			hex_float_fragment fragments[] = {
				{}, // mantissa, whole part
				{}, // mantissa, fractional part
				{}	// exponent
			};
			hex_float_fragment* current_fragment = fragments;
			char32_t prev						 = {};
			bool negative_exponent				 = {};
			const auto expected_exponent		 = [&]() noexcept
			{
				return is_match(prev, U'p', U'P') ? "expected exponent digit or sign, saw '"sv
												  : "expected exponent digit, saw '"sv;
			};
			const auto expected_mantissa = [&]() noexcept
			{
				return current_fragment == fragments ? "expected hexadecimal digit or '.', saw '"sv
													 : "expected hexadecimal digit or exponent, saw '"sv;
			};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!is_hexadecimal_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				if (prev == U'_' && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (*cp == U'.')
				{
					// 0x10.0p-.0 (exponent cannot have '.')
					if (current_fragment == fragments + 2)
						set_error_and_return_default(expected_exponent(), "."sv, "'"sv);

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
						set_error_and_return_default(expected_exponent(), *cp, "'"sv);

					// 0x.p-0 (mantissa is just '.')
					else if (fragments[0].length == 0u && fragments[1].length == 0u)
						set_error_and_return_default("expected hexadecimal digit, saw '"sv, *cp, "'"sv);

					else
						current_fragment = fragments + 2;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 0x-10.0p-0 (sign in mantissa)
					if (current_fragment != fragments + 2)
						set_error_and_return_default(expected_mantissa(), *cp, "'"sv);

					// 0x10.0p0- (misplaced exponent sign)
					else if (!is_match(prev, U'p', U'P'))
						set_error_and_return_default("expected exponent digit, saw '"sv, *cp, "'"sv);

					else
						negative_exponent = *cp == U'-';
				}
				else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
					set_error_and_return_default(expected_mantissa(), *cp, "'"sv);
				else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
					set_error_and_return_default(expected_exponent(), *cp, "'"sv);
				else if (current_fragment->length == sizeof(hex_float_fragment::chars))
					set_error_and_return_default("exceeds length limit of "sv,
												 sizeof(hex_float_fragment::chars),
												 " digits"sv);
				else
					current_fragment->chars[current_fragment->length++] = cp->bytes[0];

				text[text_length++] = cp->bytes[0];
				prev				= *cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (current_fragment != fragments + 2 || current_fragment->length == 0u)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("missing exponent"sv);
			}
			else if (prev == U'_')
				set_error_and_return_default("underscores must be followed by digits"sv);

			const double result = hex_fragments_to_double(fragments, negative_exponent);
			if (result > (std::numeric_limits<double>::max)())
				set_error_at_and_return_default(begin_pos,
												"'"sv,
												std::string_view{ text, text_length },
												"' is not representable in 64 bits"sv);
			return result * sign;

#else // !TOML_LANG_UNRELEASED

			set_error_and_return_default("hexadecimal floating-point values are not supported in "
										 "TOML " TOML_MAKE_STRING(TOML_LANG_MAJOR) "." TOML_MAKE_STRING(TOML_LANG_MINOR) "." TOML_MAKE_STRING(TOML_LANG_PATCH) " and earlier"sv);

#endif // !TOML_LANG_UNRELEASED
		}

		template <uint64_t Base>
		TOML_NODISCARD
		TOML_NEVER_INLINE
		int64_t parse_integer()
		{
			return_if_error({});
			assert_not_eof();
			using traits = parse_integer_traits<Base>;
			push_parse_scope(traits::scope_qualifier);

			const auto begin_pos = current_position();

			const std::string_view sign_text =
				traits::is_signed && is_match(*cp, U'+', U'-') ? (*cp == U'-' ? "-"sv : "+"sv) : ""sv;
			const bool negative = traits::is_signed && *cp == U'-';
			if constexpr (traits::is_signed)
			{
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_unexpected_eof({});
			}

			[[maybe_unused]] const auto digits_pos = current_position();

			if constexpr (Base == 10)
			{
				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
			}
			else
			{
				// '0'
				if (*cp != U'0')
					set_error_and_return_default("expected '0', saw '"sv, *cp, "'"sv);
				advance_and_return_if_error_or_unexpected_eof({});

				// 'b', 'o', 'x'
				if (*cp != traits::prefix_codepoint)
					set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, *cp, "'"sv);
				advance_and_return_if_error_or_unexpected_eof({});

				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
			}

			// consume digits
			char digits[max_value_length];
			size_t length = {};
			char32_t prev = {};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!traits::is_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				if TOML_UNLIKELY(prev == U'_' && !traits::is_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
				else if TOML_UNLIKELY(length == sizeof(digits))
					set_error_and_return_default("exceeds length limit of "sv, sizeof(digits), " digits"sv);
				else
					digits[length++] = cp->bytes[0];

				prev = *cp;
				advance_and_return_if_error({});
			}

			// sanity check ending state
			if (prev == U'_')
				set_error_and_return_default("underscores must be followed by digits"sv);

			// decimal integers do not allow leading zeroes (bin, oct and hex do)
			if constexpr (Base == 10)
			{
				if TOML_UNLIKELY(length > 1u && digits[0] == '0')
					set_error_at_and_return_default(digits_pos, "leading zeroes are prohibited"sv);
			}

			int64_t result = {};
			if TOML_UNLIKELY(!digits_to_int64<Base>(std::string_view{ digits, length }, negative, result))
				set_error_at_and_return_default(begin_pos,
												"'"sv,
												sign_text,
												traits::full_prefix,
												std::string_view{ digits, length },
												"' is not representable as a signed 64-bit integer"sv);

			return result;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		date parse_date(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date"sv);

			// "YYYY"
			uint32_t digits[4];
			if (!consume_digit_sequence(digits, 4u))
				set_error_and_return_default("expected 4-digit year, saw '"sv, *cp, "'"sv);
			const auto year			= digits[3] + (digits[2] * 10u) + (digits[1] * 100u) + (digits[0] * 1000u);
			const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "MM"
			const auto month = consume_two_digits("month"sv, 1u, 12u);
			return_if_error({});
			const auto max_days_in_month = month == 2u
											 ? (is_leap_year ? 29u : 28u)
											 : (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u);
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "DD"
			const auto day = consume_two_digits("day"sv, 1u, max_days_in_month);
			return_if_error({});

			if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return { year, month, day };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		time parse_time(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("time"sv);

			static constexpr size_t max_digits = 9;
			uint32_t digits[max_digits];

			// "HH"
			const auto hour = consume_two_digits("hour"sv, 0u, 23u);
			return_if_error({});
			set_error_and_return_if_eof({});

			// ':'
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "MM"
			const auto minute = consume_two_digits("minute"sv, 0u, 59u);
			return_if_error({});
			time result{ hour, minute };

			// ':' (early-exiting is allowed; seconds are optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return result;
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "SS"
			const auto second_pos = current_position();
			const auto second	  = consume_two_digits("second"sv, 0u, 60u);
			return_if_error({});
			if (second == 60u && minute != 59u && !part_of_datetime)
				set_error_at_and_return_default(second_pos,
												"second 60 (a leap second) is only valid at minute 59, saw minute "sv,
												minute);
			result.second = static_cast<decltype(result.second)>(second);

			// '.' (early-exiting is allowed; fractional is optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return result;
			if (*cp != U'.')
				set_error_and_return_default("expected '.', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "FFFFFFFFF"
			const size_t digit_count = consume_variable_length_digit_sequence(digits, max_digits);
			if (!digit_count)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("expected fractional digits, saw '"sv, *cp, "'"sv);
			}
			while (!is_eof() && is_decimal_digit(*cp))
				advance_and_return_if_error({});
			if (!is_eof() && !part_of_datetime && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);
			uint32_t value = {};
			for (size_t i = 0; i < digit_count; i++)
				value = (value * 10u) + digits[i];
			for (auto i = digit_count; i < max_digits; i++) // implicit zeros
				value *= 10u;
			result.nanosecond = value;
			return result;
		}

		TOML_NODISCARD
		time_offset parse_time_offset()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'+', U'-'));
			push_parse_scope("date-time offset"sv);

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			advance_and_return_if_error_or_unexpected_eof({});

			// "HH"
			const auto hour = static_cast<int>(consume_two_digits("hour"sv, 0u, 23u));
			return_if_error({});
			set_error_and_return_if_eof({});

			// ':'
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "MM"
			const auto minute = static_cast<int>(consume_two_digits("minute"sv, 0u, 59u));
			return_if_error({});

			time_offset offset{};
			offset.minutes = static_cast<decltype(offset.minutes)>(((hour * 60) + minute) * sign);
			return offset;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		date_time parse_date_time()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date-time"sv);

			// "YYYY-MM-DD"
			const auto date_part = parse_date(true);
			return_if_error({});
			set_error_and_return_if_eof({});

			// ' ', 'T' or 't'
			if (!is_match(*cp, U' ', U'T', U't'))
				set_error_and_return_default("expected space, 'T' or 't', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// "HH:MM:SS.FFFFFFFFF"
			if TOML_UNLIKELY(!is_decimal_digit(*cp))
				set_error_and_return_default("expected 2-digit hour, saw '"sv, *cp, "'"sv);
			const auto time_pos	 = current_position();
			const auto time_part = parse_time(true);
			return_if_error({});
			const source_position second_pos{ time_pos.line, static_cast<source_index>(time_pos.column + 6u) };

			// no offset
			if (is_eof() || is_value_terminator(*cp))
			{
				if (time_part.second == 60u && time_part.minute != 59u)
					set_error_at_and_return_default(
						second_pos,
						"second 60 (a leap second) is only valid at minute 59, saw minute "sv,
						static_cast<unsigned>(time_part.minute));
				return { date_part, time_part };
			}

			// zero offset ('Z' or 'z')
			time_offset offset{};
			if (is_match(*cp, U'Z', U'z'))
				advance_and_return_if_error({});

			// explicit offset ("+/-HH:MM")
			else if (is_match(*cp, U'+', U'-'))
			{
				offset = parse_time_offset();
				return_if_error({});
			}

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			if (time_part.second == 60u)
			{
				const int utc_minutes = (time_part.hour * 60) + time_part.minute - offset.minutes;
				const int utc_minute  = ((utc_minutes % 60) + 60) % 60;
				if (utc_minute != 59)
					set_error_at_and_return_default(
						second_pos,
						"second 60 (a leap second) is only valid at minute 59 UTC, saw minute "sv,
						utc_minute,
						" UTC"sv);
			}

			return { date_part, time_part, offset };
		}

		TOML_NODISCARD
		node_ptr parse_array();

		TOML_NODISCARD
		node_ptr parse_inline_table();

		TOML_NODISCARD
		node_ptr parse_value_known_prefixes()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_control_character(*cp));
			TOML_ASSERT_ASSUME(*cp != U'_');

			switch (cp->value)
			{
				// arrays
				case U'[': return parse_array();

				// inline tables
				case U'{': return parse_inline_table();

				// floats beginning with '.'
				case U'.': return node_ptr{ new value{ parse_float() } };

				// strings
				case U'"': [[fallthrough]];
				case U'\'': return node_ptr{ new value{ parse_string().value } };

				default:
				{
					const auto cp_upper = static_cast<char32_t>(static_cast<uint_least32_t>(cp->value) & ~0x20u);

					// bools
					if (is_match(cp_upper, U'F', U'T'))
						return node_ptr{ new value{ parse_boolean() } };

					// inf/nan
					if (is_match(cp_upper, U'I', U'N'))
						return node_ptr{ new value{ parse_inf_or_nan() } };

					return nullptr;
				}
			}
			TOML_UNREACHABLE;
		}

		// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
		enum TOML_CLOSED_FLAGS_ENUM value_traits : int
		{
			has_nothing	 = 0,
			has_digits	 = 1,
			has_b		 = 1 << 1, // as second char only (0b)
			has_e		 = 1 << 2, // only float exponents
			has_o		 = 1 << 3, // as second char only (0o)
			has_p		 = 1 << 4, // only hexfloat exponents
			has_t		 = 1 << 5,
			has_x		 = 1 << 6, // as second or third char only (0x, -0x, +0x)
			has_z		 = 1 << 7,
			has_colon	 = 1 << 8,
			has_plus	 = 1 << 9,
			has_minus	 = 1 << 10,
			has_dot		 = 1 << 11,
			begins_sign	 = 1 << 12,
			begins_digit = 1 << 13,
			begins_zero	 = 1 << 14,
			signs_msk  = has_plus | has_minus,
			bdigit_msk = has_digits | begins_digit,
			bzero_msk  = bdigit_msk | begins_zero,
		};

		// what looking ahead from the start of a value found
		struct scanned_value
		{
			value_traits traits = has_nothing;
			char32_t chars[max_value_length];
			size_t char_count		= {};
			size_t advance_count	= {};
			bool eof_while_scanning = {};

			TOML_PURE_INLINE_GETTER
			bool has_any(int t) const noexcept
			{
				return (traits & t) != has_nothing;
			}

			TOML_PURE_INLINE_GETTER
			bool has_none(int t) const noexcept
			{
				return (traits & t) == has_nothing;
			}

			TOML_ALWAYS_INLINE
			void add_trait(int t) noexcept
			{
				traits = static_cast<value_traits>(traits | t);
			}
		};

		// false if the value begins with neither a digit nor a sign
		TOML_NODISCARD
		bool scan_value(scanned_value& scanned)
		{
			assert_not_eof();

			// examine the first character to get the 'begins with' traits
			// (good fail-fast opportunity; all the remaining types begin with numeric digits or signs)
			if (is_decimal_digit(*cp))
			{
				scanned.add_trait(begins_digit);
				if (*cp == U'0')
					scanned.add_trait(begins_zero);
			}
			else if (is_match(*cp, U'+', U'-'))
				scanned.add_trait(begins_sign);
			else
				return false;

			// scan the rest of the value to determine the remaining traits
			auto la				  = reader.lookahead();
			const auto ends_value = [&]() noexcept { return la.value >= eof_mark || is_value_terminator(la.value); };
			const auto scan		  = [&]() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
			{
				if (ends_value())
					return;

				do
				{
					if (const auto c = la.value; c != U'_')
					{
						scanned.chars[scanned.char_count++] = c;

						if (is_decimal_digit(c))
							scanned.add_trait(has_digits);
						else if (is_ascii_letter(c))
						{
							TOML_ASSERT_ASSUME((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'));
							switch (static_cast<char32_t>(c | 32u))
							{
								case U'b':
									if (scanned.char_count == 2u && scanned.has_any(begins_zero))
										scanned.add_trait(has_b);
									break;

								case U'e':
									if (scanned.char_count > 1u
										&& scanned.has_none(has_b | has_o | has_p | has_t | has_x | has_z | has_colon)
										&& (scanned.has_none(has_plus | has_minus) || scanned.has_any(begins_sign)))
										scanned.add_trait(has_e);
									break;

								case U'o':
									if (scanned.char_count == 2u && scanned.has_any(begins_zero))
										scanned.add_trait(has_o);
									break;

								case U'p':
									if (scanned.has_any(has_x))
										scanned.add_trait(has_p);
									break;

								case U'x':
									if ((scanned.char_count == 2u && scanned.has_any(begins_zero))
										|| (scanned.char_count == 3u && scanned.has_any(begins_sign)
											&& scanned.chars[1] == U'0'))
										scanned.add_trait(has_x);
									break;

								case U't': scanned.add_trait(has_t); break;
								case U'z': scanned.add_trait(has_z); break;
								default: break;
							}
						}
						else if (c <= U':')
						{
							TOML_ASSERT_ASSUME(c < U'0' || c > U'9');
							switch (c)
							{
								case U'+': scanned.add_trait(has_plus); break;
								case U'-': scanned.add_trait(has_minus); break;
								case U'.': scanned.add_trait(has_dot); break;
								case U':': scanned.add_trait(has_colon); break;
								default: break;
							}
						}
					}

					reader.step(la);
					scanned.advance_count++;
					scanned.eof_while_scanning = la.value == eof_mark;
				}
				while (scanned.advance_count < max_value_scan_length && !ends_value());
			};
			scan();

			// "YYYY-MM-DD"
			static constexpr size_t date_length = 10u;

			// force further scanning if this could have been a date-time with a space instead of a T
			if (scanned.char_count == date_length							 //
				&& (scanned.traits | begins_zero) == (bzero_msk | has_minus) //
				&& scanned.chars[4] == U'-'									 //
				&& scanned.chars[7] == U'-'									 //
				&& la.value == U' ')
			{
				const auto pre_advance_count		= scanned.advance_count;
				const auto pre_scan_traits			= scanned.traits;
				scanned.chars[scanned.char_count++] = la.value;
				scanned.add_trait(has_t);

				const auto backpedal = [&]() noexcept
				{
					scanned.advance_count = pre_advance_count;
					scanned.traits		  = pre_scan_traits;
					scanned.char_count	  = date_length;
				};

				reader.step(la);
				scanned.advance_count++;

				if (!is_decimal_digit(la.value))
					backpedal();
				else
				{
					scanned.chars[scanned.char_count++] = la.value;

					reader.step(la);
					scanned.advance_count++;

					scan();

					// only the space and that digit were added
					if (scanned.char_count == date_length + 2u)
						backpedal();
				}
			}

			return true;
		}

		TOML_NODISCARD
		node_ptr parse_scanned_value(const scanned_value& scanned)
		{
			return_if_error({});
			assert_not_eof();

			node_ptr val;

			// if after scanning ahead we still only have one value character,
			// the only valid value type is an integer.
			if (scanned.char_count == 1u)
			{
				if (scanned.has_any(begins_digit))
				{
					if (scanned.advance_count == 1u)
					{
						val.reset(new value{ static_cast<int64_t>(scanned.chars[0] - U'0') });
						advance(); // skip the digit
					}
					else
						val.reset(new value{ parse_integer<10>() });
					return val;
				}

				if (scanned.eof_while_scanning && scanned.advance_count == 1u)
				{
					advance_and_return_if_error({}); // skip the lone character, so the error is at the end-of-file
					set_eof_error();
					return_after_error({});
				}
				val.reset(new value{ parse_integer<10>() });
				return val;
			}

			// now things that can be identified from two or more characters
			return_if_error({});
			if (scanned.char_count < 2u)
				set_error_and_return_default("could not determine value type"sv);

			if (scanned.has_any(begins_sign) && scanned.has_none(has_p) && scanned.chars[1] == U'0'
				&& scanned.char_count > 2u)
			{
				const auto prefix = static_cast<char32_t>(scanned.chars[2] | 32u);
				if (is_match(prefix, U'x', U'o', U'b'))
					set_error_and_return_default("signs are prohibited on "sv,
												 prefix == U'x'	  ? "hexadecimal"sv
												 : prefix == U'o' ? "octal"sv
																  : "binary"sv,
												 " integers"sv);
			}

			// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
			// typed parse functions to take over and show better diagnostics if there's an issue
			// (the default case of the switch below would hand these to a decimal parser)
			if (scanned.has_any(has_p))
				val.reset(new value{ parse_hex_float() });
			else if (scanned.has_any(has_x | has_o | has_b))
			{
				int64_t i		  = {};
				value_flags flags = {};
				if (scanned.has_any(has_x))
				{
					i	  = parse_integer<16>();
					flags = value_flags::format_as_hexadecimal;
				}
				else if (scanned.has_any(has_o))
				{
					i	  = parse_integer<8>();
					flags = value_flags::format_as_octal;
				}
				else // has_b
				{
					i	  = parse_integer<2>();
					flags = value_flags::format_as_binary;
				}
				return_if_error({});

				val.reset(new value{ i });
				node::ref_cast<int64_t>(*val).flags(flags);
			}
			else if (scanned.has_any(has_e) || (scanned.has_any(begins_digit) && scanned.chars[1] == U'.'))
				val.reset(new value{ parse_float() });
			else if (scanned.has_any(begins_sign))
			{
				// single-digit signed integers
				if (scanned.char_count == 2u && scanned.has_any(has_digits))
				{
					if (scanned.advance_count == 2u)
					{
						val.reset(new value{ static_cast<int64_t>(scanned.chars[1] - U'0')
											 * (scanned.chars[0] == U'-' ? -1LL : 1LL) });
						advance(); // skip the sign
						advance(); // skip the digit
						return val;
					}
				}

				// simple signed floats (e.g. +1.0)
				else if (is_decimal_digit(scanned.chars[1]) && scanned.chars[2] == U'.')
					val.reset(new value{ parse_float() });

				// signed infinity or nan
				else if (is_match(scanned.chars[1], U'i', U'n', U'I', U'N'))
					val.reset(new value{ parse_inf_or_nan() });
			}

			return_if_error({});
			if (val)
				return val;

			// match trait masks against what they can match exclusively.
			// all correct value parses will come out of this list, so doing this as a switch is likely to
			// be a better friend to the optimizer on the success path (failure path can be slow but that
			// doesn't matter much).
			switch (unwrap_enum(scanned.traits))
			{
				// binary integers
				// 0b10
				case bzero_msk | has_b:
					val.reset(new value{ parse_integer<2>() });
					node::ref_cast<int64_t>(*val).flags(value_flags::format_as_binary);
					break;

				// octal integers
				// 0o10
				case bzero_msk | has_o:
					val.reset(new value{ parse_integer<8>() });
					node::ref_cast<int64_t>(*val).flags(value_flags::format_as_octal);
					break;

				// decimal integers
				// 00
				// 10
				// +10
				// -10
				case bzero_msk: [[fallthrough]];
				case bdigit_msk: [[fallthrough]];
				case begins_sign | has_digits | has_minus: [[fallthrough]];
				case begins_sign | has_digits | has_plus:
				{
					// past the scan limit the value could still be a float, so its type is unknown
					static constexpr size_t max_numeric_value_length = max_value_scan_length - 1u;
					if TOML_UNLIKELY(!scanned.eof_while_scanning && scanned.advance_count > max_numeric_value_length)
						set_error_and_return_default("numeric value too long to identify type - cannot exceed "sv,
													 max_numeric_value_length,
													 " characters"sv);

					val.reset(new value{ parse_integer<10>() });
					break;
				}

				// hexadecimal integers
				// 0x10
				case bzero_msk | has_x:
					val.reset(new value{ parse_integer<16>() });
					node::ref_cast<int64_t>(*val).flags(value_flags::format_as_hexadecimal);
					break;

				// decimal floats
				// 0e1
				// 0e-1
				// 0e+1
				// 0.0
				// 0.0e1
				// 0.0e-1
				// 0.0e+1
				case bzero_msk | has_e: [[fallthrough]];
				case bzero_msk | has_e | has_minus: [[fallthrough]];
				case bzero_msk | has_e | has_plus: [[fallthrough]];
				case bzero_msk | has_dot: [[fallthrough]];
				case bzero_msk | has_dot | has_e: [[fallthrough]];
				case bzero_msk | has_dot | has_e | has_minus: [[fallthrough]];
				case bzero_msk | has_dot | has_e | has_plus: [[fallthrough]];
				// 1e1
				// 1e-1
				// 1e+1
				// 1.0
				// 1.0e1
				// 1.0e-1
				// 1.0e+1
				case bdigit_msk | has_e: [[fallthrough]];
				case bdigit_msk | has_e | has_minus: [[fallthrough]];
				case bdigit_msk | has_e | has_plus: [[fallthrough]];
				case bdigit_msk | has_dot: [[fallthrough]];
				case bdigit_msk | has_dot | has_e: [[fallthrough]];
				case bdigit_msk | has_dot | has_e | has_minus: [[fallthrough]];
				case bdigit_msk | has_dot | has_e | has_plus: [[fallthrough]];
				// +1e1
				// +1.0
				// +1.0e1
				// +1.0e+1
				// +1.0e-1
				// -1.0e+1
				case begins_sign | has_digits | has_e | has_plus: [[fallthrough]];
				case begins_sign | has_digits | has_dot | has_plus: [[fallthrough]];
				case begins_sign | has_digits | has_dot | has_e | has_plus: [[fallthrough]];
				case begins_sign | has_digits | has_dot | has_e | signs_msk: [[fallthrough]];
				// -1e1
				// -1e+1
				// +1e-1
				// -1.0
				// -1.0e1
				// -1.0e-1
				case begins_sign | has_digits | has_e | has_minus: [[fallthrough]];
				case begins_sign | has_digits | has_e | signs_msk: [[fallthrough]];
				case begins_sign | has_digits | has_dot | has_minus: [[fallthrough]];
				case begins_sign | has_digits | has_dot | has_e | has_minus:
					val.reset(new value{ parse_float() });
					break;

				// hexadecimal floats
				// 0x10p0
				// 0x10p-0
				// 0x10p+0
				case bzero_msk | has_x | has_p: [[fallthrough]];
				case bzero_msk | has_x | has_p | has_minus: [[fallthrough]];
				case bzero_msk | has_x | has_p | has_plus: [[fallthrough]];
				// -0x10p0
				// -0x10p-0
				// +0x10p0
				// +0x10p+0
				// -0x10p+0
				// +0x10p-0
				case begins_sign | has_digits | has_x | has_p | has_minus: [[fallthrough]];
				case begins_sign | has_digits | has_x | has_p | has_plus: [[fallthrough]];
				case begins_sign | has_digits | has_x | has_p | signs_msk: [[fallthrough]];
				// 0x10.1p0
				// 0x10.1p-0
				// 0x10.1p+0
				case bzero_msk | has_x | has_dot | has_p: [[fallthrough]];
				case bzero_msk | has_x | has_dot | has_p | has_minus: [[fallthrough]];
				case bzero_msk | has_x | has_dot | has_p | has_plus: [[fallthrough]];
				// -0x10.1p0
				// -0x10.1p-0
				// +0x10.1p0
				// +0x10.1p+0
				// -0x10.1p+0
				// +0x10.1p-0
				case begins_sign | has_digits | has_x | has_dot | has_p | has_minus: [[fallthrough]];
				case begins_sign | has_digits | has_x | has_dot | has_p | has_plus: [[fallthrough]];
				case begins_sign | has_digits | has_x | has_dot | has_p | signs_msk:
					val.reset(new value{ parse_hex_float() });
					break;

				// times
				// HH:MM
				// HH:MM:SS
				// HH:MM:SS.FFFFFF
				case bzero_msk | has_colon: [[fallthrough]];
				case bzero_msk | has_colon | has_dot: [[fallthrough]];
				case bdigit_msk | has_colon: [[fallthrough]];
				case bdigit_msk | has_colon | has_dot: val.reset(new value{ parse_time() }); break;

				// local dates
				// YYYY-MM-DD
				case bzero_msk | has_minus: [[fallthrough]];
				case bdigit_msk | has_minus: val.reset(new value{ parse_date() }); break;

				// date-times
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
				case bzero_msk | has_minus | has_colon | has_t: [[fallthrough]];
				case bzero_msk | signs_msk | has_colon | has_t: [[fallthrough]];
				case bdigit_msk | has_minus | has_colon | has_t: [[fallthrough]];
				case bdigit_msk | signs_msk | has_colon | has_t: [[fallthrough]];
				// YYYY-MM-DDTHH:MM:SS.FFFFFF
				// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
				// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
				// YYYY-MM-DD HH:MM:SS.FFFFFF
				// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
				// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
				case bzero_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
				case bzero_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
				case bdigit_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
				case bdigit_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
				// YYYY-MM-DDTHH:MMZ
				// YYYY-MM-DD HH:MMZ
				// YYYY-MM-DDTHH:MM:SSZ
				// YYYY-MM-DD HH:MM:SSZ
				// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
				// YYYY-MM-DD HH:MM:SS.FFFFFFZ
				case bzero_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
				case bzero_msk | has_minus | has_colon | has_dot | has_z | has_t: [[fallthrough]];
				case bdigit_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
				case bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
					val.reset(new value{ parse_date_time() });
					break;

				// everything valid matched above, so this just picks the parser with the most useful error
				default:
					if (scanned.has_any(begins_digit) && scanned.char_count > 4u && scanned.chars[4] == U'-')
					{
						if (scanned.has_any(has_t | has_colon))
							val.reset(new value{ parse_date_time() });
						else
							val.reset(new value{ parse_date() });
					}
					else if (scanned.has_any(begins_digit) && scanned.has_any(has_colon))
						val.reset(new value{ parse_time() });
					else if (scanned.has_any(has_dot | has_e))
						val.reset(new value{ parse_float() });
					else
						val.reset(new value{ parse_integer<10>() });
					break;
			}

			return val;
		}

		TOML_NODISCARD
		node_ptr parse_value()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_value_terminator(*cp));
			push_parse_scope("value"sv);

			const depth_counter_scope depth_counter{ nested_values };
			if TOML_UNLIKELY(nested_values > max_nested_values)
			{
				set_depth_error(current_position());
				return_after_error({});
			}

			// check if it begins with some control character
			// (note that this will also fail for whitespace but we're assuming we've
			// called consume_whitespace() before calling parse_value())
			if TOML_UNLIKELY(is_control_character(*cp))
				set_error_and_return_default("unexpected control character, saw '"sv, *cp, "'"sv);

			// underscores at the beginning
			if (*cp == U'_')
				set_error_and_return_default("leading underscores are prohibited"sv);

			const auto begin_pos = current_position();

			// detect the value type and parse accordingly,
			// starting with value types that can be detected
			// unambiguously from just one character.
			node_ptr val = parse_value_known_prefixes();
			return_if_error({});

			// value types from here down require more than one character to unambiguously identify
			// so scan ahead and collect a set of value 'traits'.
			if (!val)
			{
				scanned_value scanned;
				if (scan_value(scanned))
				{
					val = parse_scanned_value(scanned);
					return_if_error({});
				}
			}

			if (!val)
				set_error_at_and_return_default(begin_pos, "could not determine value type"sv);

			val->source_ = { begin_pos, current_position(1), reader.source_path() };
			return val;
		}

		TOML_NEVER_INLINE
		bool parse_key()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp) || is_string_delimiter(*cp));
			push_parse_scope("key"sv);

			key_buffer.clear();
			start_recording();
			recording_whitespace = false;

			while (!is_error())
			{
				std::string_view key_segment;
				const auto key_begin = current_position();

				// bare_key_segment
				if (is_bare_key_character(*cp))
					key_segment = parse_bare_key_segment();

				// "quoted key segment"
				else if (is_string_delimiter(*cp))
				{
					recording_whitespace	= true;
					const parsed_string str = parse_string();
					recording_whitespace	= false;
					return_if_error({});

					if (str.was_multi_line)
						set_error_at_and_return_default(key_begin,
														"multi-line strings are prohibited in "sv,
														key_buffer.empty() ? ""sv : "dotted "sv,
														"keys"sv);
					key_segment = str.value;
				}

				// ???
				else
					set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
												 *cp,
												 "'"sv);

				const auto key_end = current_position();

				// whitespace following the key segment
				consume_whitespace();

				// store segment
				key_buffer.push_back(key_segment, key_begin, key_end);

				if TOML_UNLIKELY(nested_values + key_buffer.size() > max_nested_values)
				{
					set_depth_error(key_begin);
					return_after_error({});
				}

				// eof or no more key to come
				if (is_eof() || *cp != U'.')
					break;

				// was a dotted key - go around again
				advance_and_return_if_error_or_unexpected_eof({});
				consume_whitespace();
				set_error_and_return_if_eof({});
			}
			return_if_error({});

			stop_recording_key();
			return true;
		}

		TOML_NODISCARD
		key make_key(size_t segment_index) const
		{
			TOML_ASSERT(key_buffer.size() > segment_index);

			const auto& segment = key_buffer.segments[segment_index];
			return key{ key_buffer[segment_index], source_region{ segment.begin, segment.end, reader.source_path() } };
		}

		struct table_header
		{
			source_region region;
			bool is_table_array;
		};

		// leaves the header's key in key_buffer
		TOML_NODISCARD
		table_header read_table_header()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'[');

			const auto begin_pos = current_position();
			bool is_table_array	 = {};

			// skip first '['
			advance_and_return_if_error_or_unexpected_eof({});

			// skip past any whitespace that followed the '['
			const bool had_leading_whitespace = consume_whitespace();
			return_if_error({});
			set_error_and_return_if_eof({});

			// skip second '[' (if present)
			if (*cp == U'[')
			{
				if (had_leading_whitespace)
					set_error_and_return_default(
						"[[array-of-tables]] brackets must be contiguous (i.e. [ [ this ] ] is prohibited)"sv);

				is_table_array = true;
				advance_and_return_if_error_or_unexpected_eof({});

				// skip past any whitespace that followed the '['
				consume_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			// check for a premature closing ']'
			if (*cp == U']')
				set_error_and_return_default("tables with blank bare keys are prohibited"sv);

			if (!is_bare_key_character(*cp) && !is_string_delimiter(*cp))
				set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
											 *cp,
											 "'"sv);

			// get the actual key
			nested_values = {};
			parse_key();
			return_if_error({});
			set_error_and_return_if_eof({});

			// consume the closing ']'
			if (*cp != U']')
				set_error_and_return_default("expected ']', saw '"sv, *cp, "'"sv);
			if (is_table_array)
			{
				advance_and_return_if_error_or_unexpected_eof({});
				if (*cp != U']')
					set_error_and_return_default("expected ']', saw '"sv, *cp, "'"sv);
			}
			advance_and_return_if_error({});
			const auto end_pos = current_position(1);

			// handle the rest of the line after the header
			consume_rest_of_line();
			return_if_error({});

			return { source_region{ begin_pos, end_pos, reader.source_path() }, is_table_array };
		}

		struct table_header_parent
		{
			table* tbl;
			size_t depth;
		};

		TOML_NODISCARD
		table_header_parent find_or_create_table_header_parent(const source_region& header_region)
		{
			return_if_error({});
			TOML_ASSERT(!key_buffer.empty());

			// check if each parent is a table/table array, or can be created implicitly as a table.
			table* parent = &root;
			size_t depth  = {};
			for (size_t i = 0, e = key_buffer.size() - 1u; i < e; i++)
			{
				const std::string_view segment = key_buffer[i];
				auto pit					   = parent->lower_bound(segment);

				// parent already existed
				if (pit != parent->end() && pit->first == segment)
				{
					node& p = pit->second;

					if (auto tbl = p.as_table())
					{
						// adding to closed inline tables is illegal
						if (tbl->is_inline())
							set_error_at_and_return_default(header_region.begin,
															"cannot insert '"sv,
															to_sv(recording_buffer),
															"' into existing inline table"sv);

						parent = tbl;
						depth++;
					}
					else if (auto arr = p.as_array(); arr && table_arrays.contains(arr))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!arr->empty());
						TOML_ASSERT(arr->back().is_table());
						parent = &node::ref_cast<table>(arr->back());
						depth += 2u; // the array and its element
					}
					else
						set_error_at_and_return_default(key_buffer.segments[i].begin,
														"cannot redefine existing "sv,
														to_sv(p.type()),
														" '"sv,
														segment,
														"' as table"sv);
				}

				// need to create a new implicit table
				else
				{
					pit		  = parent->emplace_hint<table>(pit, make_key(i));
					table& p  = node::ref_cast<table>(pit->second);
					p.source_ = header_region;

					implicit_tables.insert(&p);
					parent = &p;
					depth++;
				}
			}

			return { parent, depth };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		table* parse_table_header()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'[');
			push_parse_scope("table header"sv);

			const auto header = read_table_header();
			return_if_error({});

			const auto parent_and_depth = find_or_create_table_header_parent(header.region);
			return_if_error({});
			table* const parent = parent_and_depth.tbl;

			nested_values = parent_and_depth.depth + (header.is_table_array ? 2u : 1u);
			if TOML_UNLIKELY(nested_values > max_nested_values)
			{
				set_depth_error(header.region.begin);
				return_after_error({});
			}

			const std::string_view last_segment = key_buffer.back();
			auto it								= parent->lower_bound(last_segment);

			// if there was already a matching node some sanity checking is necessary;
			// this is ok if we're making an array and the existing element is already an array (new element)
			// or if we're making a table and the existing element is an implicitly-created table (promote it),
			// otherwise this is a redefinition error.
			if (it != parent->end() && it->first == last_segment)
			{
				node& matching_node = it->second;
				if (auto arr = matching_node.as_array(); header.is_table_array && arr && table_arrays.contains(arr))
				{
					table& tbl	= arr->emplace_back<table>();
					tbl.source_ = header.region;
					return &tbl;
				}

				if (auto tbl = matching_node.as_table(); !header.is_table_array && tbl && implicit_tables.contains(tbl))
				{
					implicit_tables.erase(tbl);
					tbl->source_ = header.region;
					return tbl;
				}

				// if we get here it's a redefinition error.
				if (!header.is_table_array && matching_node.type() == node_type::table)
					set_error_at_and_return_default(header.region.begin,
													"cannot redefine existing table '"sv,
													to_sv(recording_buffer),
													"'"sv);

				set_error_at_and_return_default(header.region.begin,
												"cannot redefine existing "sv,
												to_sv(matching_node.type()),
												" '"sv,
												to_sv(recording_buffer),
												"' as "sv,
												header.is_table_array ? "array-of-tables"sv : "table"sv);
			}

			// there was no matching node, sweet - we can freely instantiate a new table/table array.
			auto last_key = make_key(key_buffer.size() - 1u);

			// if it's an array we need to make the array and its first table element,
			// set the starting regions, and return the table element
			if (header.is_table_array)
			{
				it			   = parent->emplace_hint<array>(it, std::move(last_key));
				array& tbl_arr = node::ref_cast<array>(it->second);
				table_arrays.insert(&tbl_arr);
				tbl_arr.source_ = header.region;

				table& tbl	= tbl_arr.emplace_back<table>();
				tbl.source_ = header.region;
				return &tbl;
			}

			// otherwise we're just making a table

			it			= parent->emplace_hint<table>(it, std::move(last_key));
			table& tbl	= node::ref_cast<table>(it->second);
			tbl.source_ = header.region;
			return &tbl;
		}

		TOML_NODISCARD
		table* find_or_create_dotted_key_parent(table* tbl)
		{
			return_if_error({});
			TOML_ASSERT(!key_buffer.empty());

			for (size_t i = 0, e = key_buffer.size() - 1u; i < e; i++)
			{
				const std::string_view segment = key_buffer[i];
				auto pit					   = tbl->lower_bound(segment);

				// parent already existed
				if (pit != tbl->end() && pit->first == segment)
				{
					table* p = pit->second.as_table();

					if TOML_UNLIKELY(!p || !dotted_key_tables.contains(p))
					{
						// redefinition
						if (!p || !implicit_tables.contains(p))
							set_error_at_and_return_default(key_buffer.segments[i].begin,
															"cannot redefine existing "sv,
															to_sv(pit->second.type()),
															" '"sv,
															segment,
															"' as dotted key-value pair"sv);

						// the dotted key defines the table, so a header can't any more
						implicit_tables.erase(p);
						dotted_key_tables.insert(p);
					}

					tbl = p;
				}

				// need to create a new table for the dotted key
				else
				{
					pit		  = tbl->emplace_hint<table>(pit, make_key(i));
					table& p  = node::ref_cast<table>(pit->second);
					p.source_ = pit->first.source();

					dotted_key_tables.insert(&p);
					tbl = &p;
				}
			}

			return tbl;
		}

		TOML_NEVER_INLINE
		bool parse_key_value_pair_and_insert(table* tbl)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp) || is_bare_key_character(*cp));
			push_parse_scope("key-value pair"sv);

			// read the key into the key buffer
			parse_key();
			return_if_error({});
			TOML_ASSERT(key_buffer.size() >= 1u);
			set_error_and_return_if_eof({});

			// '='
			if (*cp != U'=')
				set_error_and_return_default("expected '=', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_unexpected_eof({});

			// skip past any whitespace that followed the '='
			consume_whitespace();
			return_if_error({});
			set_error_and_return_if_eof({});

			// check that the next character could actually be a value
			if (is_value_terminator(*cp))
				set_error_and_return_default("expected value, saw '"sv, *cp, "'"sv);

			// if it's a dotted kvp we need to spawn the parent sub-tables if necessary,
			// and set the target table to the second-to-last one in the chain
			tbl = find_or_create_dotted_key_parent(tbl);
			return_if_error({});

			// ensure this isn't a redefinition
			const std::string_view last_segment = key_buffer.back();
			auto it								= tbl->lower_bound(last_segment);
			if (it != tbl->end() && it->first == last_segment)
				set_error_at_and_return_default(key_buffer.segments[0].begin,
												"cannot redefine existing "sv,
												to_sv(it->second.type()),
												" '"sv,
												to_sv(recording_buffer),
												"'"sv);

			// create the key first since the key buffer will likely get overwritten during value parsing (inline
			// tables)
			auto last_key = make_key(key_buffer.size() - 1u);

			// now we can actually parse the value
			node_ptr val;
			{
				const depth_counter_scope dotted_key_depth{ nested_values, key_buffer.size() - 1u };
				val = parse_value();
			}
			return_if_error({});

			tbl->emplace_hint<node_ptr>(it, std::move(last_key), std::move(val));
			return true;
		}

		void parse_document()
		{
			assert_not_error();
			assert_not_eof();
			push_parse_scope("root table"sv);

			table* current_table = &root;

			do
			{
				return_if_error();

				// leading whitespace, line endings, comments
				if (consume_whitespace() || consume_line_break() || consume_comment())
					continue;
				return_if_error();

				// [tables]
				// [[table array]]
				if (*cp == U'[')
					current_table = parse_table_header();

				// bare_keys
				// dotted.keys
				// "quoted keys"
				else if (is_bare_key_character(*cp) || is_string_delimiter(*cp))
				{
					push_parse_scope("key-value pair"sv);

					parse_key_value_pair_and_insert(current_table);

					// handle the rest of the line after the kvp
					// (this is not done in parse_key_value_pair_and_insert() because that is also used for inline
					// tables)
					consume_rest_of_line();
					return_if_error();
				}

				else // ???
					set_error("expected keys, tables, whitespace or comments, saw '"sv, *cp, "'"sv);
			}
			while (!is_eof());

			root.source_.end = current_position(1);
		}

		static void update_region_ends(node& nde) noexcept
		{
			const auto type = nde.type();
			if (type > node_type::array)
				return;

			if (type == node_type::table)
			{
				auto& tbl = node::ref_cast<table>(nde);
				if (tbl.is_inline()) // inline tables (and all their inline descendants) are already correctly
									 // terminated
					return;

				// a table's region stays as its header
				for (auto&& [k, v] : tbl)
				{
					TOML_UNUSED(k);
					update_region_ends(v);
				}
			}
			else // arrays
			{
				auto& arr = node::ref_cast<array>(nde);
				auto end  = nde.source_.end;
				for (auto&& v : arr)
				{
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
				nde.source_.end = end;
			}
		}

	  public:
		explicit parser(source_reader&& input) //
			: reader{ std::move(input) }
		{
			root.source_ = { source_position{ 1, 1 }, source_position{ 1, 1 }, reader.source_path() };

			land(reader.first());
			if (cp)
				parse_document();

			update_region_ends(root);
		}

		TOML_NODISCARD
		operator parse_result() && noexcept
		{
#if TOML_EXCEPTIONS

			return parse_result{ std::move(root) };

#else

			if (err)
				return parse_result{ *std::move(err) };
			return parse_result{ std::move(root) };

#endif
		}
	};

	TOML_EXTERNAL_LINKAGE
	node_ptr parser::parse_array()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_unexpected_eof({});

		node_ptr arr_ptr{ new array{} };
		array& arr		 = node::ref_cast<array>(*arr_ptr);
		bool after_value = {};

		while (!is_error())
		{
			while (consume_whitespace() || consume_line_break() || consume_comment())
				continue;
			return_if_error({});
			set_error_and_return_if_eof({});

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (!after_value)
					set_error_and_return_default("expected value or closing ']', saw ','"sv);
				after_value = false;
				advance_and_return_if_error_or_unexpected_eof({});
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
				if (after_value)
					set_error_and_return_default("expected comma or closing ']', saw '"sv, *cp, "'"sv);
				after_value = true;

				// e.g. '}' or a non-ASCII line break; parse_value() requires a value to start here
				if TOML_UNLIKELY(is_value_terminator(*cp))
					set_error_and_return_default("expected value or closing ']', saw '"sv, *cp, "'"sv);

				node_ptr val = parse_value();
				return_if_error({});

				if (!arr.capacity())
					arr.reserve(4u);
				arr.emplace_back<node_ptr>(std::move(val));
			}
		}

		return_if_error({});
		return arr_ptr;
	}

	TOML_EXTERNAL_LINKAGE
	node_ptr parser::parse_inline_table()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_unexpected_eof({});

		node_ptr tbl_ptr{ new table{} };
		table& tbl = node::ref_cast<table>(*tbl_ptr);
		tbl.is_inline(true);
		bool after_kvp = {};

		while (!is_error())
		{
			while (consume_whitespace() || consume_line_break() || consume_comment())
				continue;
			return_if_error({});
			set_error_and_return_if_eof({});

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (!after_kvp)
					set_error_and_return_default("expected key-value pair or closing '}', saw ','"sv);
				after_kvp = false;
				advance_and_return_if_error_or_unexpected_eof({});
			}

			// closing '}'
			else if (*cp == U'}')
			{
				advance_and_return_if_error({});
				break;
			}

			// key-value pair
			else if (is_string_delimiter(*cp) || is_bare_key_character(*cp))
			{
				if (after_kvp)
					set_error_and_return_default("expected comma or closing '}', saw '"sv, *cp, "'"sv);
				after_kvp = true;

				parse_key_value_pair_and_insert(&tbl);
				return_if_error({});
			}

			// ???
			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, *cp, "'"sv);
		}

		return_if_error({});
		return tbl_ptr;
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_IMPL_NAMESPACE_END;

#undef TOML_RETURNS_BY_THROWING
#undef advance_and_return_if_error
#undef advance_and_return_if_error_or_unexpected_eof
#undef assert_not_eof
#undef assert_not_error
#undef is_eof
#undef is_error
#undef parse_error_break
#undef push_parse_scope
#undef push_parse_scope_1
#undef push_parse_scope_2
#undef return_after_error
#undef return_if_eof
#undef return_if_error
#undef return_if_error_or_eof
#undef set_error_and_return_default
#undef set_error_and_return_if_eof
#undef set_error_at_and_return_default

TOML_ANON_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse(source_reader && reader)
	{
		[[maybe_unused]] const impl::default_fenv fenv;
		return impl::parser{ std::move(reader) };
	}

	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse_file(std::string_view file_path)
	{
#if TOML_EXCEPTIONS
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	throw parse_error(msg, source_position{}, std::make_shared<const std::string>(std::move(path)))
#else
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	return parse_result(parse_error(msg, source_position{}, std::make_shared<const std::string>(std::move(path))))
#endif

		std::string file_path_str(file_path);

		std::ifstream file;
#if TOML_WINDOWS && !(defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__))
		file.open(impl::widen(file_path_str).c_str(), std::ifstream::in | std::ifstream::binary);
#else
		file.open(file_path_str, std::ifstream::in | std::ifstream::binary);
#endif
		if (!file.is_open())
			TOML_PARSE_FILE_ERROR("File could not be opened for reading", file_path_str);

		// get size; a pipe has none and is parsed as a stream.
		// opening with ios::ate would close it again when the seek fails
		const auto file_size = file.seekg(0, std::ifstream::end).tellg();
		if (file_size == -1)
		{
			file.clear();
			return parse(file, std::move(file_path_str));
		}
		file.seekg(0, std::ifstream::beg);

		// read the whole file into memory first if the file isn't too large
		static constexpr auto large_file_threshold = 1024 * 1024 * 2; // 2 MiB
		if (file_size <= large_file_threshold)
		{
			const auto size = static_cast<size_t>(file_size);
			const std::unique_ptr<char[]> file_data{ new char[size] };
			file.read(file_data.get(), static_cast<std::streamsize>(file_size));
			if (file.gcount() != static_cast<std::streamsize>(file_size))
				TOML_PARSE_FILE_ERROR("File could not be read", file_path_str);
			return parse(std::string_view{ file_data.get(), size }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		if (file.peek() == std::ifstream::traits_type::eof())
			TOML_PARSE_FILE_ERROR("File could not be read", file_path_str);
		return parse(file, std::move(file_path_str));

#undef TOML_PARSE_FILE_ERROR
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::string_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(file_path);
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::u8string_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(
			std::string_view{ reinterpret_cast<const char*>(file_path.data()), file_path.length() });
	}

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::wstring_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(impl::narrow(file_path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_PARSER

//********  impl/formatter.inl  ****************************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_DISABLE_WARNINGS;
#include <ostream>
#include <streambuf>
TOML_ENABLE_WARNINGS;
TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_IMPL_NAMESPACE_START
{
	TOML_CONST_GETTER
	constexpr bool is_non_ascii_unprintable(char32_t c, string_dialect dialect) noexcept
	{
		return (c >= U'\x80' && c <= U'\x9F') || is_non_ascii_vertical_whitespace(c)
			|| (dialect == string_dialect::yaml && (c == U'\xFEFF' || c == U'\xFFFE' || c == U'\xFFFF'));
	}

	class formatter_buffer
	{
	  private:
		class chunks final : public std::streambuf
		{
		  private:
			std::ostream* target_;
			char chars_[4096];

		  protected:
			int_type overflow(int_type ch) override
			{
				flush();
				if (traits_type::eq_int_type(ch, traits_type::eof()))
					return traits_type::not_eof(ch);
				*pptr() = traits_type::to_char_type(ch);
				pbump(1);
				return ch;
			}

			std::streamsize xsputn(const char* str, std::streamsize len) override
			{
				if (len > epptr() - pptr())
				{
					flush();
					if (len > epptr() - pptr())
					{
						target_->write(str, len);
						return len;
					}
				}
				traits_type::copy(pptr(), str, static_cast<size_t>(len));
				pbump(static_cast<int>(len));
				return len;
			}

		  public:
			explicit chunks(std::ostream& target) //
				: target_{ &target }
			{
				setp(chars_, chars_ + sizeof(chars_));
			}

			void flush()
			{
				const auto len = pptr() - pbase();
				if (!len)
					return;
				setp(chars_, chars_ + sizeof(chars_));
				target_->write(chars_, len);
			}
		};

		formatter& formatter_;
		std::ostream* target_;
		chunks chunks_;
		std::ostream stream_;

	  public:
		explicit formatter_buffer(formatter& f) //
			: formatter_{ f },
			  target_{ f.stream_ },
			  chunks_{ *f.stream_ },
			  stream_{ &chunks_ }
		{
			stream_.exceptions(std::ios::badbit);
			formatter_.stream_ = &stream_;
		}

		~formatter_buffer() noexcept
		{
			formatter_.stream_ = target_;
		}

		void flush()
		{
			chunks_.flush();
		}
	};

	enum class TOML_CLOSED_FLAGS_ENUM formatted_string_traits : unsigned
	{
		none		  = 0,
		line_breaks	  = 1u << 0, // \n
		tabs		  = 1u << 1, // \t
		control_chars = 1u << 2, // also includes the non-ascii code points that are always escaped
		single_quotes = 1u << 3,
		non_bare	  = 1u << 4, // anything not satisfying "is bare key character"
		non_ascii	  = 1u << 5, // any codepoint >= 128
		malformed	  = 1u << 6, // not valid utf-8

		all = (malformed << 1u) - 1u
	};
	TOML_MAKE_FLAGS(formatted_string_traits);
}
TOML_IMPL_NAMESPACE_END;

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	char* write_unicode_escape(char* out, char prefix, uint_least32_t value, int digits) noexcept
	{
		*out++ = '\\';
		*out++ = prefix;
		while (digits-- > 0)
			*out++ = "0123456789ABCDEF"[(value >> (digits * 4)) & 0xFu];
		return out;
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_date_or_time(std::ostream & stream, const T& val, format_flags flags)
	{
		if (!(flags & format_flags::quote_dates_and_times))
		{
			impl::print_to_stream(stream, val);
			return;
		}

		impl::print_to_stream_bookended(stream, val, !!(flags & format_flags::allow_literal_strings) ? '\'' : '"');
	}

	// the format_as flag of the base an integer prints in, or none for decimal
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	value_flags printed_integer_format(int64_t val, value_flags flags, format_flags allowed) noexcept
	{
		static constexpr auto format_mask =
			value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;

		if (val < 0)
			return {};

		const auto format	  = flags & format_mask;
		format_flags required = {};
		switch (format)
		{
			case value_flags::format_as_binary: required = format_flags::allow_binary_integers; break;
			case value_flags::format_as_octal: required = format_flags::allow_octal_integers; break;
			case value_flags::format_as_hexadecimal: required = format_flags::allow_hexadecimal_integers; break;
			default: return {};
		}
		return !!(allowed & required) ? format : value_flags{};
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	impl::formatted_string_traits ascii_traits(char c) noexcept
	{
		using impl::formatted_string_traits;

		switch (c)
		{
			case '\n': return formatted_string_traits::line_breaks;
			case '\t': return formatted_string_traits::tabs;
			case '\'': return formatted_string_traits::single_quotes;
			default: break;
		}

		auto traits = formatted_string_traits::none;
		if TOML_UNLIKELY(impl::is_control_character(c))
			traits |= formatted_string_traits::control_chars;
		if (!impl::is_ascii_bare_key_character(static_cast<char32_t>(c)))
			traits |= formatted_string_traits::non_bare;
		return traits;
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view ascii_escape(char c, bool real_tabs_allowed, bool multi_line) noexcept
	{
		switch (c)
		{
			case '"': return R"(\")"sv;
			case '\\': return R"(\\)"sv;
			case '\x7F': return R"(\u007F)"sv;
			case '\t': return real_tabs_allowed ? ""sv : R"(\t)"sv;
			case '\n': return multi_line ? ""sv : R"(\n)"sv;
			default:
				if TOML_LIKELY(static_cast<unsigned char>(c) > 0x1Fu)
					return {};
				return impl::control_char_escapes[static_cast<unsigned char>(c)];
		}
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	impl::formatted_string_traits scan_string_traits(std::string_view str, impl::string_dialect dialect) noexcept
	{
		using impl::formatted_string_traits;

		formatted_string_traits traits = {};

		// ascii fast path
		if (impl::is_ascii(str.data(), str.length()))
		{
			static constexpr auto all_ascii_traits =
				formatted_string_traits::all
				& ~(formatted_string_traits::non_ascii | formatted_string_traits::malformed);

			for (auto c : str)
			{
				traits |= ascii_traits(c);
				if (traits == all_ascii_traits)
					break;
			}
			return traits;
		}

		// unicode slow path
		traits |= formatted_string_traits::non_ascii;
		impl::utf8_decoder decoder;
		for (auto c : str)
		{
			decoder(c);

			if TOML_UNLIKELY(decoder.error())
				break;

			if (!decoder.has_codepoint())
				continue;

			if (decoder.codepoint <= U'\x7F')
			{
				traits |= ascii_traits(static_cast<char>(decoder.codepoint));
				continue;
			}

			if TOML_UNLIKELY(impl::is_non_ascii_unprintable(decoder.codepoint, dialect))
				traits |= formatted_string_traits::control_chars;

			// U+FEFF can't be bare: at the start of a document it reads back as a BOM
			if (!impl::is_bare_key_character(decoder.codepoint)
				|| impl::is_non_ascii_horizontal_whitespace(decoder.codepoint) || decoder.codepoint == U'\xFEFF')
				traits |= formatted_string_traits::non_bare;
		}

		// if the unicode is malformed just treat the string as a single-line non-literal and
		// escape all non-ascii characters (to keep the output parseable and help with diagnostics)
		if (decoder.error() || decoder.needs_more_input())
		{
			traits &= ~formatted_string_traits::line_breaks;
			traits |= formatted_string_traits::control_chars | formatted_string_traits::non_bare
					| formatted_string_traits::malformed;
		}

		return traits;
	}

	TOML_INTERNAL_LINKAGE
	void print_escaped_ascii(std::ostream & stream, std::string_view str, bool real_tabs_allowed, bool multi_line)
	{
		const char* run		  = str.data();
		const char* const end = str.data() + str.length();
		for (auto it = run; it != end; it++)
		{
			const auto escape = ascii_escape(*it, real_tabs_allowed, multi_line);
			if TOML_LIKELY(escape.empty())
				continue;

			if (run != it)
				impl::print_to_stream(stream, run, static_cast<size_t>(it - run));
			impl::print_to_stream(stream, escape);
			run = it + 1;
		}
		if (run != end)
			impl::print_to_stream(stream, run, static_cast<size_t>(end - run));
	}

	TOML_INTERNAL_LINKAGE
	void print_escaped_unicode(std::ostream & stream,
							   std::string_view str,
							   bool real_tabs_allowed,
							   bool multi_line,
							   bool unicode_allowed,
							   impl::string_dialect dialect)
	{
		impl::utf8_decoder decoder;
		const char* run		 = str.data();
		const char* cp_start = str.data();
		const char* cp_end	 = cp_start;
		const auto print_run = [&]()
		{
			if (run != cp_start)
				impl::print_to_stream(stream, run, static_cast<size_t>(cp_start - run));
			run = cp_end;
		};
		const auto escape_raw_bytes = [&]()
		{
			print_run();
			for (; cp_start != cp_end; cp_start++)
			{
				char escape[6];
				write_unicode_escape(escape, 'u', static_cast<uint8_t>(*cp_start), 4);
				impl::print_to_stream(stream, escape, sizeof(escape));
			}
		};
		for (auto c : str)
		{
			decoder(c);
			cp_end++;

			// if the decoder encounters malformed unicode just escape each byte as \u00XX and carry on
			if (decoder.error())
			{
				escape_raw_bytes();
				decoder.reset();
				continue;
			}

			if (!decoder.has_codepoint())
				continue;

			std::string_view escape;
			char unicode_escape[12];
			const auto cp = static_cast<uint_least32_t>(decoder.codepoint);
			if (cp <= 0x7Fu)
				escape = ascii_escape(static_cast<char>(cp), real_tabs_allowed, multi_line);
			else if (!unicode_allowed || impl::is_non_ascii_unprintable(decoder.codepoint, dialect))
			{
				char* escape_end = unicode_escape;
				if (cp > 0xFFFFu && dialect == impl::string_dialect::json)
				{
					const auto offset = cp - 0x10000u;
					escape_end		  = write_unicode_escape(escape_end, 'u', 0xD800u + (offset >> 10), 4);
					escape_end		  = write_unicode_escape(escape_end, 'u', 0xDC00u + (offset & 0x3FFu), 4);
				}
				else if (cp > 0xFFFFu)
					escape_end = write_unicode_escape(escape_end, 'U', cp, 8);
				else
					escape_end = write_unicode_escape(escape_end, 'u', cp, 4);
				escape = { unicode_escape, static_cast<size_t>(escape_end - unicode_escape) };
			}

			if (!escape.empty())
			{
				print_run();
				impl::print_to_stream(stream, escape);
			}
			cp_start = cp_end;
		}
		escape_raw_bytes();
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	formatter::formatter(const node* source_node,
						 const parse_result* source_pr,
						 const formatter_constants& constants,
						 const formatter_config& config) noexcept //
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		: source_{ source_pr && *source_pr ? &source_pr->table() : source_node },
		  result_{ source_pr },
#else
		: source_{ source_pr ? source_pr : source_node },
#endif
		  constants_{ &constants },
		  config_{ config }
	{
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		TOML_ASSERT_ASSUME(source_ || result_);
#else
		TOML_ASSERT_ASSUME(source_);
#endif

		config_.flags = (config_.flags | constants_->mandatory_flags) & ~constants_->ignored_flags;

		indent_columns_ = config_.indent.length();

		int_format_mask_ = config_.flags
						 & (format_flags::allow_binary_integers | format_flags::allow_octal_integers
							| format_flags::allow_hexadecimal_integers);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::attach(std::ostream & stream) noexcept
	{
		indent_		   = {};
		naked_newline_ = true;
		stream_		   = &stream;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::detach() noexcept
	{
		stream_ = nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_newline(bool force)
	{
		if (naked_newline_ && !force)
			return;

		print_to_stream(*stream_, '\n');
		naked_newline_ = true;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_indent()
	{
		if (indent_ <= 0)
			return;

		char buf[128];
		TOML_ASSERT(config_.indent.length() <= sizeof(buf));
		size_t len = {};
		for (int i = 0; i < indent_; i++)
		{
			if (sizeof(buf) - len < config_.indent.length())
			{
				print_to_stream(*stream_, buf, len);
				len = {};
			}
			std::memcpy(buf + len, config_.indent.data(), config_.indent.length());
			len += config_.indent.length();
		}
		print_to_stream(*stream_, buf, len);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(char c)
	{
		print_to_stream(*stream_, c);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(std::string_view str)
	{
		print_to_stream(*stream_, str);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str,
								 bool allow_multi_line,
								 bool allow_bare,
								 bool allow_literal_whitespace)
	{
		print_string(str, allow_multi_line, allow_bare, allow_literal_whitespace, string_dialect::toml);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str,
								 bool allow_multi_line,
								 bool allow_bare,
								 bool allow_literal_whitespace,
								 string_dialect dialect)
	{
		if (str.empty())
		{
			print_unformatted(literal_strings_allowed() ? "''"sv : "\"\""sv);
			return;
		}

		// pre-scan the string to determine how we should output it
		auto traits				   = TOML_ANON_NAMESPACE::scan_string_traits(str, dialect);
		const auto unicode_allowed = unicode_strings_allowed() && !(traits & formatted_string_traits::malformed);

		// strings with line breaks, tabs, single-quotes or anything that gets escaped can't be bare
		if (!allow_bare
			|| !!(traits
				  & (formatted_string_traits::line_breaks | formatted_string_traits::tabs
					 | formatted_string_traits::single_quotes | formatted_string_traits::control_chars)))
			traits |= formatted_string_traits::non_bare;

		// if the string meets the requirements of being 'bare' we can emit a bare string
		// (bare strings are composed of letters and numbers; no whitespace, control chars, quotes, etc)
		if (!(traits & formatted_string_traits::non_bare)
			&& (!(traits & formatted_string_traits::non_ascii) || unicode_allowed))
		{
			print_unformatted(str);
			return;
		}
		const auto real_tabs_allowed = allow_literal_whitespace && real_tabs_in_strings_allowed();

		// determine if this should be a multi-line string (triple-quotes)
		const auto multi_line = allow_literal_whitespace	 //
							 && allow_multi_line			 //
							 && multi_line_strings_allowed() //
							 && !!(traits & formatted_string_traits::line_breaks);

		// the parser trims a newline immediately following the opening delimiter of a multi-line string
		const auto leading_newline = multi_line && str[0] == '\n';

		// determine if this should be a literal string (single-quotes with no escaping)
		// (three or more consecutive single-quotes can't appear in a multi-line literal string)
		const auto literal = literal_strings_allowed()										  //
						  && !(traits & formatted_string_traits::control_chars)				  //
						  && (!(traits & formatted_string_traits::single_quotes)			  //
							  || (multi_line && str.find("'''"sv) == std::string_view::npos)) //
						  && (!(traits & formatted_string_traits::tabs) || real_tabs_allowed) //
						  && (!(traits & formatted_string_traits::line_breaks) || multi_line) //
						  && (!(traits & formatted_string_traits::non_ascii) || unicode_allowed);

		// literal strings (single quotes, no escape codes)
		if (literal)
		{
			const auto quot = multi_line ? R"(''')"sv : R"(')"sv;
			print_unformatted(quot);
			if (leading_newline)
				print_unformatted('\n');
			print_unformatted(str);
			print_unformatted(quot);
			return;
		}

		// anything from here down is a non-literal string, so requires iteration and escaping.
		const auto quot = multi_line ? R"(""")"sv : R"(")"sv;
		print_unformatted(quot);
		if (leading_newline)
			print_unformatted('\n');

		if (!(traits & formatted_string_traits::non_ascii))
			TOML_ANON_NAMESPACE::print_escaped_ascii(*stream_, str, real_tabs_allowed, multi_line);
		else
			TOML_ANON_NAMESPACE::print_escaped_unicode(*stream_,
													   str,
													   real_tabs_allowed,
													   multi_line,
													   unicode_allowed,
													   dialect);

		print_unformatted(quot);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<std::string>& val)
	{
		print_string(val.get());
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<int64_t>& val)
	{
		const auto format = TOML_ANON_NAMESPACE::printed_integer_format(*val, val.flags(), int_format_mask_);
		switch (format)
		{
			case value_flags::format_as_binary: print_to_stream(*stream_, "0b"sv); break;
			case value_flags::format_as_octal: print_to_stream(*stream_, "0o"sv); break;
			case value_flags::format_as_hexadecimal: print_to_stream(*stream_, "0x"sv); break;
			default: break;
		}
		print_to_stream(*stream_, *val, format);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<double>& val)
	{
		std::string_view inf_nan;
		switch (fpclassify(*val))
		{
			case fp_class::neg_inf: inf_nan = constants_->float_neg_inf; break;
			case fp_class::pos_inf: inf_nan = constants_->float_pos_inf; break;
			case fp_class::nan: inf_nan = constants_->float_nan; break;
			case fp_class::ok:
				print_to_stream(*stream_, *val, value_flags::none, relaxed_float_precision());
				naked_newline_ = false;
				return;
			default: TOML_UNREACHABLE;
		}

		if (!!(config_.flags & format_flags::quote_infinities_and_nans))
			print_to_stream_bookended(*stream_, inf_nan, '"');
		else
			print_to_stream(*stream_, inf_nan);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<bool>& val)
	{
		print_unformatted(*val ? constants_->bool_true : constants_->bool_false);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date>& val)
	{
		TOML_ANON_NAMESPACE::print_date_or_time(*stream_, *val, config_.flags);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<time>& val)
	{
		TOML_ANON_NAMESPACE::print_date_or_time(*stream_, *val, config_.flags);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date_time>& val)
	{
		TOML_ANON_NAMESPACE::print_date_or_time(*stream_, *val, config_.flags);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_value(const node& val_node, node_type type)
	{
		TOML_ASSUME(type > node_type::array);
		switch (type)
		{
			case node_type::string: print(static_cast<const value<std::string>&>(val_node)); break;
			case node_type::integer: print(static_cast<const value<int64_t>&>(val_node)); break;
			case node_type::floating_point: print(static_cast<const value<double>&>(val_node)); break;
			case node_type::boolean: print(static_cast<const value<bool>&>(val_node)); break;
			case node_type::date: print(static_cast<const value<date>&>(val_node)); break;
			case node_type::time: print(static_cast<const value<time>&>(val_node)); break;
			case node_type::date_time: print(static_cast<const value<date_time>&>(val_node)); break;
			default: TOML_UNREACHABLE;
		}
	}

#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

	TOML_EXTERNAL_LINKAGE
	bool formatter::dump_failed_parse_result()
	{
		if (!result_ || *result_)
			return false;

		stream() << result_->error();
		return true;
	}

#else

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(const)
	bool formatter::dump_failed_parse_result()
	{
		return false;
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/toml_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	// counts a float that has at most eight decimals, since printing one to count it costs as much as printing it.
	// returns zero for any other float
	TOML_INTERNAL_LINKAGE
	size_t toml_formatter_count_short_float_columns(double val) noexcept
	{
		static constexpr double scale		 = 1e8;
		static constexpr size_t max_decimals = 8u;

		const bool negative	   = impl::signbit(val);
		const double magnitude = negative ? -val : val;

		// below 1e7 only one text with this many decimals reads back as the value; below 1e-4 the exponent form prints
		if (!(magnitude < 1e7) || (magnitude < fixed_form_min && magnitude > 0.0))
			return {};

		const double scaled = magnitude * scale;
		auto digits			= static_cast<uint64_t>(scaled);
		if (static_cast<double>(digits) != scaled || static_cast<double>(digits) / scale != magnitude)
			return {};

		size_t decimals = max_decimals;
		for (; decimals > 1u && digits % 10u == 0u; digits /= 10u)
			decimals--;
		for (size_t i = 0; i < decimals; i++)
			digits /= 10u;

		size_t weight = (negative ? 2u : 1u) + decimals; // "-" and "."
		do
		{
			weight++;
			digits /= 10u;
		}
		while (digits);
		return weight;
	}

	TOML_INTERNAL_LINKAGE
	size_t toml_formatter_count_inline_columns(const node& n, size_t line_wrap_cols, format_flags flags) noexcept
	{
		switch (n.type())
		{
			case node_type::table:
			{
				auto& tbl = static_cast<const table&>(n);
				if (tbl.empty())
					return 2u;		// "{}"
				size_t weight = 3u; // "{ }"
				for (auto&& [k, v] : tbl)
				{
					// +  ", "
					weight += k.length() + toml_formatter_count_inline_columns(v, line_wrap_cols, flags) + 2u;
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::array:
			{
				auto& arr = static_cast<const array&>(n);
				if (arr.empty())
					return 2u;		// "[]"
				size_t weight = 3u; // "[ ]"
				for (auto& elem : arr)
				{
					weight += toml_formatter_count_inline_columns(elem, line_wrap_cols, flags) + 2u; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::string:
			{
				auto& str = static_cast<const value<std::string>&>(n).get();
				return str.length() + 2u; // + ""
			}

			case node_type::integer:
			{
				auto& integer  = static_cast<const value<int64_t>&>(n);
				const auto val = integer.get();

				unsigned bits_per_digit = {};
				switch (TOML_ANON_NAMESPACE::printed_integer_format(val, integer.flags(), flags))
				{
					case value_flags::format_as_binary: bits_per_digit = 1u; break;
					case value_flags::format_as_octal: bits_per_digit = 3u; break;
					case value_flags::format_as_hexadecimal: bits_per_digit = 4u; break;
					default: break;
				}
				if (bits_per_digit)
				{
					size_t weight = 3u; // "0x" and the first digit
					for (auto rest = static_cast<uint64_t>(val) >> bits_per_digit; rest; rest >>= bits_per_digit)
						weight++;
					return weight;
				}

				size_t weight  = val < 0 ? 2u : 1u; // "-" and the first digit
				auto magnitude = static_cast<uint64_t>(val);
				if (val < 0)
					magnitude = 0u - magnitude;
				for (; magnitude >= 10u; magnitude /= 10u)
					weight++;
				return weight;
			}

			case node_type::floating_point:
			{
				const auto val = static_cast<const value<double>&>(n).get();
				if (const auto cls = impl::fpclassify(val); cls != impl::fp_class::ok)
					return cls == impl::fp_class::neg_inf ? 4u : 3u; // "-inf", or "inf" and "nan"

				const bool relaxed_floats = !!(flags & format_flags::relaxed_float_precision);
				if (!relaxed_floats)
				{
					if (const auto columns = toml_formatter_count_short_float_columns(val))
						return columns;
				}

				char buf[32];
				return static_cast<size_t>(TOML_ANON_NAMESPACE::write_float(buf, val, relaxed_floats, false) - buf);
			}

			case node_type::boolean: return static_cast<const value<bool>&>(n).get() ? 4u : 5u; // "true", "false"

			case node_type::date:
			{
				char buf[64];
				const auto end = TOML_ANON_NAMESPACE::write_date(buf, static_cast<const value<date>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::time:
			{
				char buf[64];
				const auto end = TOML_ANON_NAMESPACE::write_time(buf, static_cast<const value<time>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::date_time:
			{
				char buf[64];
				const auto end =
					TOML_ANON_NAMESPACE::write_date_time(buf, static_cast<const value<date_time>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::none:
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	class toml_formatter_key_path_scope
	{
	  private:
		std::vector<const key*>* path_;
		size_t previous_size_;

	  public:
		TOML_NODISCARD_CTOR
		explicit toml_formatter_key_path_scope(std::vector<const key*>& path, const key& k) //
			: path_{ &path },
			  previous_size_{ path.size() }
		{
			path.push_back(&k);
		}

		// restores instead of popping, as the parser's scopes do (#288)
		~toml_formatter_key_path_scope() noexcept
		{
			path_->resize(previous_size_);
		}

		TOML_DELETE_DEFAULTS(toml_formatter_key_path_scope);
	};

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_is_sub_table(const node& n) noexcept
	{
		const auto tbl = n.as_table();
		return tbl && !tbl->is_inline();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_is_table_array(const node& n) noexcept
	{
		const auto arr = n.as_array();
		if (!arr || !arr->is_array_of_tables())
			return false;

		return !static_cast<const table&>((*arr)[0]).is_inline();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_holds_only_tables(const table& tbl) noexcept
	{
		for (auto&& [_, v] : tbl)
		{
			if (!toml_formatter_is_sub_table(v) && !toml_formatter_is_table_array(v))
				return false;
		}
		return !tbl.empty();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_forces_multiline(const node& n,
										 size_t line_wrap_cols,
										 size_t starting_column_bias,
										 format_flags flags) noexcept
	{
		return (toml_formatter_count_inline_columns(n, line_wrap_cols, flags) + starting_column_bias) >= line_wrap_cols;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_pending_table_separator()
	{
		if (pending_table_separator_)
		{
			print_newline(true);
			print_newline(true);
			pending_table_separator_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const key& k)
	{
		print_string(k.str(), false, true, false);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_equals()
	{
		if (terse_kvps())
			print_unformatted('=');
		else
			print_unformatted(" = "sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_inline(const node& n)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print_inline(static_cast<const table&>(n)); break;
			case node_type::array: print(static_cast<const array&>(n)); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_inline(const table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted("{ "sv);

		bool first = true;
		for (auto&& [k, v] : tbl)
		{
			if (!first)
				print_unformatted(", "sv);
			first = false;

			print(k);
			print_equals();
			print_inline(v);
		}

		print_unformatted(" }"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		static constexpr size_t line_wrap_columns = 120u;

		const auto original_indent = indent();
		const auto multiline	   = force_multi_line_arrays()
								  || TOML_ANON_NAMESPACE::toml_formatter_forces_multiline(
										 arr,
										 line_wrap_columns,
										 indent_columns() * static_cast<size_t>(original_indent < 0 ? 0 : original_indent),
										 effective_flags());

		print_unformatted('[');

		if (multiline)
		{
			if (original_indent < 0)
				indent(0);
			if (indent_array_elements())
				increase_indent();
		}
		else
			print_unformatted(' ');

		bool first = true;
		for (auto& v : arr)
		{
			if (!first)
			{
				print_unformatted(',');
				if (!multiline)
					print_unformatted(' ');
			}
			first = false;

			if (multiline)
			{
				print_newline(true);
				print_indent();
			}

			print_inline(v);
		}
		if (multiline)
		{
			indent(original_indent);
			print_newline(true);
			print_indent();
		}
		else
			print_unformatted(' ');

		print_unformatted(']');
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const table& tbl)
	{
		// sub-tables and arrays of tables print after the values; the counts skip those passes for a table with none
		size_t sub_tables	= {};
		size_t table_arrays = {};

		// values, arrays, and inline tables/table arrays
		for (auto&& [k, v] : tbl)
		{
			if (TOML_ANON_NAMESPACE::toml_formatter_is_sub_table(v))
			{
				sub_tables++;
				continue;
			}
			if (TOML_ANON_NAMESPACE::toml_formatter_is_table_array(v))
			{
				table_arrays++;
				continue;
			}

			pending_table_separator_ = true;
			print_newline();
			print_indent();
			print(k);
			print_equals();
			print_inline(v);
		}

		// non-inline tables
		if (sub_tables)
		{
			for (auto&& [k, v] : tbl)
			{
				if (TOML_ANON_NAMESPACE::toml_formatter_is_sub_table(v))
					print_sub_table(k, static_cast<const table&>(v));
			}
		}

		// table arrays
		if (table_arrays)
		{
			for (auto&& [k, v] : tbl)
			{
				if (TOML_ANON_NAMESPACE::toml_formatter_is_table_array(v))
					print_table_array(k, static_cast<const array&>(v));
			}
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_key_path()
	{
		bool first = true;
		for (const auto k : key_path_)
		{
			if (!first)
				print_unformatted('.');
			first = false;
			print(*k);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_sub_table(const key& k, const table& tbl)
	{
		// we can skip indenting and emitting the headers for tables that only contain other tables
		// (so we don't over-nest)
		const bool skip_self = TOML_ANON_NAMESPACE::toml_formatter_holds_only_tables(tbl);

		const TOML_ANON_NAMESPACE::toml_formatter_key_path_scope key_path{ key_path_, k };
		const indent_scope indented{ *this, !skip_self && indent_sub_tables() };

		if (!skip_self)
		{
			print_pending_table_separator();
			print_indent();
			print_unformatted('[');
			print_key_path();
			print_unformatted(']');
			pending_table_separator_ = true;
		}

		print(tbl);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_table_array(const key& k, const array& arr)
	{
		const indent_scope indented{ *this, indent_sub_tables() };
		const TOML_ANON_NAMESPACE::toml_formatter_key_path_scope key_path{ key_path_, k };

		for (auto& elem : arr)
		{
			print_pending_table_separator();
			print_indent();
			print_unformatted("[["sv);
			print_key_path();
			print_unformatted("]]"sv);
			pending_table_separator_ = true;
			print(static_cast<const table&>(elem));
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		const auto type = source().type();
		if (type != node_type::table && type != node_type::array)
		{
			print_value(source(), type);
			return;
		}

		impl::formatter_buffer buffer{ *this };
		if (type == node_type::array)
			print(static_cast<const array&>(source()));
		else if (auto& tbl = static_cast<const table&>(source()); tbl.is_inline())
			print_inline(tbl);
		else
		{
			decrease_indent(); // so root kvps and tables have the same indent
			print(tbl);
		}
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/json_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void json_formatter::print_json_string(std::string_view str)
	{
		print_string(str, false, false, true, impl::string_dialect::json);
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print_node(const toml::node& n)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print(static_cast<const table&>(n)); break;
			case node_type::array: print(static_cast<const array&>(n)); break;
			case node_type::string: print_json_string(static_cast<const value<std::string>&>(n).get()); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted('{');
		{
			const indent_scope indented{ *this, indent_sub_tables() };

			bool first = true;
			for (auto&& [k, v] : tbl)
			{
				if (!first)
					print_unformatted(',');
				first = false;
				print_newline(true);
				print_indent();

				print_json_string(k.str());
				print_unformatted(terse_kvps() ? ":"sv : " : "sv);

				print_node(v);
			}
		}
		print_newline(true);
		print_indent();
		print_unformatted('}');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		print_unformatted('[');
		{
			const indent_scope indented{ *this, indent_array_elements() };

			bool first = true;
			for (auto& v : arr)
			{
				if (!first)
					print_unformatted(',');
				first = false;
				print_newline(true);
				print_indent();

				print_node(v);
			}
		}
		print_newline(true);
		print_indent();
		print_unformatted(']');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		const auto type = source().type();
		if (type != node_type::table && type != node_type::array)
		{
			print_node(source());
			return;
		}

		impl::formatter_buffer buffer{ *this };
		print_node(source());
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

//********  impl/yaml_formatter.inl  ***********************************************************************************

#if TOML_ENABLE_FORMATTERS

TOML_PUSH_WARNINGS;
#ifdef _MSC_VER
#ifndef __clang__
#pragma inline_recursion(on)
#endif
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#endif

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	bool yaml_block_scalar_preserves(std::string_view str) noexcept
	{
		if (str.empty() || str.back() == '\n')
			return false;

		impl::utf8_decoder decoder;
		bool contains_newline = {};
		bool seen_content	  = {};
		for (auto c : str)
		{
			decoder(c);
			if (decoder.error())
				return false;
			if (!decoder.has_codepoint())
				continue;

			const auto cp = decoder.codepoint;
			if (cp == U'\n')
			{
				contains_newline = true;
				continue;
			}
			// libyaml rejects a block scalar whose content starts with a tab
			if (((cp == U' ' || cp == U'\t') && !seen_content) || impl::is_nontab_control_character(cp)
				|| impl::is_non_ascii_unprintable(cp, impl::string_dialect::yaml))
				return false;
			seen_content = true;
		}
		return contains_newline && !decoder.needs_more_input();
	}

	TOML_INTERNAL_LINKAGE
	bool yaml_plain_scalar_is_string(std::string_view str) noexcept
	{
		static constexpr std::string_view keywords[] = { "y"sv,	  "n"sv,	"yes"sv,   "no"sv,	"on"sv,
														 "off"sv, "true"sv, "false"sv, "null"sv };
		static constexpr size_t max_keyword_length	 = []() constexpr noexcept
		{
			size_t longest = {};
			for (auto keyword : keywords)
				longest = keyword.length() > longest ? keyword.length() : longest;
			return longest;
		}();

		if (str.empty() || str.front() == '-' || str.front() == '+' || (str.front() >= '0' && str.front() <= '9'))
			return false;
		if (str.length() > max_keyword_length)
			return true;

		char lower[max_keyword_length];
		for (size_t i = 0; i < str.length(); i++)
			lower[i] = str[i] >= 'A' && str[i] <= 'Z' ? static_cast<char>(str[i] - 'A' + 'a') : str[i];
		const auto word = std::string_view{ lower, str.length() };

		for (auto keyword : keywords)
			if (word == keyword)
				return false;
		return true;
	}

	// YAML limits a key written as 'key: value' to 1024 characters. the count here is an upper bound
	// on what the key prints as, so it can only err towards the explicit form
	TOML_INTERNAL_LINKAGE
	bool yaml_key_needs_explicit_form(std::string_view key, bool unicode_allowed) noexcept
	{
		static constexpr size_t max_implicit_key_length = 1024;
		static constexpr size_t escape_length			= 6;  // \uXXXX
		static constexpr size_t long_escape_length		= 10; // \UXXXXXXXX
		static constexpr size_t quotes					= 2;

		// fits even with every byte escaped
		if (key.length() <= (max_implicit_key_length - quotes) / escape_length)
			return false;

		size_t length = quotes;
		impl::utf8_decoder decoder;
		for (const auto c : key)
		{
			decoder(c);
			if (decoder.error())
				return true;
			if (!decoder.has_codepoint())
				continue;

			const auto cp = decoder.codepoint;
			if (cp < U'\x80')
				length += cp >= U' ' && cp < U'\x7F' && cp != U'"' && cp != U'\\' ? size_t{ 1 } : escape_length;
			else if (unicode_allowed && !impl::is_non_ascii_unprintable(cp, impl::string_dialect::yaml))
				length++;
			else
				length += cp > U'\xFFFF' ? long_escape_length : escape_length;

			if (length > max_implicit_key_length)
				return true;
		}
		return decoder.needs_more_input();
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_scalar(std::string_view str)
	{
		print_string(str,
					 false,
					 TOML_ANON_NAMESPACE::yaml_plain_scalar_is_string(str),
					 true,
					 impl::string_dialect::yaml);
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_string(const value<std::string>& str)
	{
		if (!TOML_ANON_NAMESPACE::yaml_block_scalar_preserves(*str))
		{
			print_yaml_scalar(*str);
			return;
		}

		print_unformatted("|-"sv);

		const indent_scope indented{ *this };

		std::string_view remaining{ *str };
		while (true)
		{
			const auto line_length = remaining.find('\n');
			print_newline();
			print_indent();
			print_unformatted(remaining.substr(0u, line_length));
			if (line_length == std::string_view::npos)
				break;
			remaining.remove_prefix(line_length + 1u);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_float(const value<double>& val)
	{
		if (impl::fpclassify(*val) != impl::fp_class::ok)
		{
			base::print(val);
			return;
		}

		// yaml 1.1 reads 1e+16 as a string
		char buf[32];
		const auto end = TOML_ANON_NAMESPACE::write_float(buf, *val, relaxed_float_precision(), true);
		print_unformatted(std::string_view{ buf, static_cast<size_t>(end - buf) });
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_node(const toml::node& n, bool parent_is_array)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print(static_cast<const table&>(n), parent_is_array); break;
			case node_type::array: print(static_cast<const array&>(n), parent_is_array); break;
			case node_type::string: print_yaml_string(static_cast<const value<std::string>&>(n)); break;
			case node_type::floating_point: print_yaml_float(static_cast<const value<double>&>(n)); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::table& tbl, bool parent_is_array)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		const indent_scope indented{ *this };

		bool continues_line = parent_is_array;
		for (auto&& [k, v] : tbl)
		{
			if (!continues_line)
			{
				print_newline();
				print_indent();
			}
			continues_line = false;

			const bool explicit_key =
				TOML_ANON_NAMESPACE::yaml_key_needs_explicit_form(k.str(), unicode_strings_allowed());
			if (explicit_key)
				print_unformatted("? "sv);

			print_yaml_scalar(k.str());

			if (explicit_key)
			{
				print_newline();
				print_indent();
			}
			print_unformatted(": "sv);

			print_node(v);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::array& arr, bool parent_is_array)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		const indent_scope indented{ *this };

		bool continues_line = parent_is_array;
		for (auto&& v : arr)
		{
			if (!continues_line)
			{
				print_newline();
				print_indent();
			}
			continues_line = false;

			print_unformatted("- "sv);

			print_node(v, true);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		const auto type = source().type();
		if (type != node_type::table && type != node_type::array)
		{
			print_node(source());
			return;
		}

		impl::formatter_buffer buffer{ *this };
		if (type == node_type::table)
			decrease_indent(); // so root kvps and tables have the same indent
		print_node(source());
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#ifdef _MSC_VER
#pragma pop_macro("min")
#pragma pop_macro("max")
#ifndef __clang__
#pragma inline_recursion(off)
#endif
#endif
TOML_POP_WARNINGS;

#endif // TOML_ENABLE_FORMATTERS

#endif // TOML_IMPLEMENTATION

TOML_POP_WARNINGS;

// macro hygiene
#if TOML_UNDEF_MACROS
#undef TOML_ABI_NAMESPACE_BOOL
#undef TOML_ABI_NAMESPACE_END
#undef TOML_ABI_NAMESPACE_START
#undef TOML_ABI_NAMESPACES
#undef TOML_ABSTRACT_INTERFACE
#undef TOML_ALWAYS_INLINE
#undef TOML_ANON_NAMESPACE
#undef TOML_ANON_NAMESPACE_END
#undef TOML_ANON_NAMESPACE_START
#undef TOML_ARCH_AMD64
#undef TOML_ARCH_ARM
#undef TOML_ARCH_ARM32
#undef TOML_ARCH_ARM64
#undef TOML_ARCH_BITNESS
#undef TOML_ARCH_ITANIUM
#undef TOML_ARCH_X64
#undef TOML_ARCH_X86
#undef TOML_ASSERT
#undef TOML_ASSERT_ASSUME
#undef TOML_ASSUME
#undef TOML_ASYMMETRICAL_EQUALITY_OPS
#undef TOML_ATTR
#undef TOML_CLANG
#undef TOML_CLOSED_ENUM
#undef TOML_CLOSED_FLAGS_ENUM
#undef TOML_COMPILER_HAS_EXCEPTIONS
#undef TOML_COMPILER_HAS_RTTI
#undef TOML_CONST
#undef TOML_CONST_GETTER
#undef TOML_CONST_INLINE_GETTER
#undef TOML_CONSTRAINED_TEMPLATE
#undef TOML_CPP
#undef TOML_DECLSPEC
#undef TOML_DELETE_DEFAULTS
#undef TOML_DISABLE_ANALYSIS_WARNINGS
#undef TOML_DISABLE_ARITHMETIC_WARNINGS
#undef TOML_DISABLE_CLANG_WARNING
#undef TOML_DISABLE_CODE_ANALYSIS_WARNINGS
#undef TOML_DISABLE_GCC_WARNING
#undef TOML_DISABLE_MSVC_WARNING
#undef TOML_DISABLE_SPAM_WARNINGS
#undef TOML_DISABLE_SUGGEST_ATTR_WARNINGS
#undef TOML_DISABLE_SWITCH_WARNINGS
#undef TOML_DISABLE_WARNINGS
#undef TOML_DOXYGEN
#undef TOML_EMPTY_BASES
#undef TOML_ENABLE_IF
#undef TOML_ENABLE_WARNINGS
#undef TOML_EVAL_BOOL_0
#undef TOML_EVAL_BOOL_1
#undef TOML_EXPORTED_CONSTANT
#undef TOML_EXTERNAL_LINKAGE
#undef TOML_FLAGS_ENUM
#undef TOML_FLOAT128
#undef TOML_GCC
#undef TOML_GCC_LIKE
#undef TOML_HAS_ATTR
#undef TOML_HAS_BUILTIN
#undef TOML_HAS_CHAR8
#undef TOML_HAS_CPP_ATTR
#undef TOML_HAS_CUSTOM_OPTIONAL_TYPE
#undef TOML_HAS_FEATURE
#undef TOML_HAS_FLOAT_CHARCONV
#undef TOML_HAS_INCLUDE
#undef TOML_HAS_SSE2
#undef TOML_HIDDEN_CONSTRAINT
#undef TOML_ICC
#undef TOML_ICC_CL
#undef TOML_IMPL_NAMESPACE_END
#undef TOML_IMPL_NAMESPACE_START
#undef TOML_IMPLEMENTATION
#undef TOML_INCLUDE_WINDOWS_H
#undef TOML_INLINE_GETTER
#undef TOML_INT128
#undef TOML_INTELLISENSE
#undef TOML_INTERNAL_LINKAGE
#undef TOML_LANG_AT_LEAST
#undef TOML_LANG_EFFECTIVE_VERSION
#undef TOML_LANG_HIGHER_THAN
#undef TOML_LANG_UNRELEASED
#undef TOML_LAUNDER
#undef TOML_LIFETIME_HOOKS
#undef TOML_LIKELY
#undef TOML_LIKELY_CASE
#undef TOML_LINUX
#undef TOML_MAKE_FLAGS
#undef TOML_MAKE_FLAGS_1
#undef TOML_MAKE_FLAGS_2
#undef TOML_MAKE_STRING
#undef TOML_MAKE_STRING_1
#undef TOML_MAKE_VERSION
#undef TOML_MSVC
#undef TOML_MSVC_LIKE
#undef TOML_NAMESPACE
#undef TOML_NEVER_INLINE
#undef TOML_NODISCARD
#undef TOML_NODISCARD_CTOR
#undef TOML_NOEXCEPT_AS
#undef TOML_NVCC
#undef TOML_OPEN_ENUM
#undef TOML_OPEN_FLAGS_ENUM
#undef TOML_PARSER_TYPENAME
#undef TOML_PEDANTIC
#undef TOML_POP_WARNINGS
#undef TOML_PRAGMA_CLANG
#undef TOML_PRAGMA_GCC
#undef TOML_PRAGMA_MSVC
#undef TOML_PURE
#undef TOML_PURE_GETTER
#undef TOML_PURE_INLINE_GETTER
#undef TOML_PUSH_WARNINGS
#undef TOML_REQUIRES
#undef TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN
#undef TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN_MESSAGE
#undef TOML_SA_LIST_BEG
#undef TOML_SA_LIST_END
#undef TOML_SA_LIST_NEW
#undef TOML_SA_LIST_NXT
#undef TOML_SA_LIST_SEP
#undef TOML_SA_NATIVE_VALUE_TYPE_LIST
#undef TOML_SA_NEWLINE
#undef TOML_SA_NODE_TYPE_LIST
#undef TOML_SA_UNWRAPPED_NODE_TYPE_LIST
#undef TOML_SA_VALUE_EXACT_FUNC_MESSAGE
#undef TOML_SA_VALUE_FUNC_MESSAGE
#undef TOML_SA_VALUE_MESSAGE_CHRONO_LOSSLESS
#undef TOML_SA_VALUE_MESSAGE_CHRONO_LOSSY
#undef TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#undef TOML_SA_VALUE_MESSAGE_LOSSLESS
#undef TOML_SA_VALUE_MESSAGE_LOSSY
#undef TOML_SA_VALUE_MESSAGE_STRING_LIKE
#undef TOML_SA_VALUE_MESSAGE_U8STRING
#undef TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
#undef TOML_SA_VALUE_MESSAGE_VIEW
#undef TOML_SA_VALUE_MESSAGE_WSTRING
#undef TOML_SIMPLE_STATIC_ASSERT_MESSAGES
#undef TOML_TRIVIAL_ABI
#undef TOML_UINT128
#undef TOML_UNIX
#undef TOML_UNLIKELY
#undef TOML_UNLIKELY_CASE
#undef TOML_UNREACHABLE
#undef TOML_UNUSED
#undef TOML_WINDOWS
#endif

#endif // TOMLPLUSPLUS_HPP
