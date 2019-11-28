#pragma once

//--------------------------------------------------------------------
// CONFIGURATION
//--------------------------------------------------------------------
// clang-format off

#ifdef TOML17_CONFIG_HEADER
	#include TOML17_CONFIG_HEADER
#endif

#ifndef TOML17_NAMESPACE
	#define TOML17_NAMESPACE toml
#endif

#ifndef TOML17_NONSTANDARD_ATTRIBUTES
	#define TOML17_NONSTANDARD_ATTRIBUTES 0
#endif

//--------------------------------------------------------------------
// COMPILER & ENVIRONMENT STUFF
//--------------------------------------------------------------------

#ifndef __cplusplus
	#error Toml17 is a C++ library.
#endif

#if defined(_MSC_VER)

	#define TOML17_LANG_VERSION _MSVC_LANG

	#ifdef _CPPRTTI
		#define TOML17_RTTI_ENABLED
	#endif

	#ifdef _CPPUNWIND
		#define TOML17_EXCEPTIONS_ENABLED
	#endif

	#define TOML17_DISABLE_WARNINGS		__pragma(warning(push, 0))
	#define TOML17_RESTORE_WARNINGS		__pragma(warning(pop, 0))

	#define TOML17_INTERFACE			__declspec(novtable)
	#define TOML17_EMPTY_BASES			__declspec(empty_bases)
	#define TOML17_ALWAYS_INLINE		_forceinline
	#define TOML17_ASSUME(cond)			_assume(cond)
	#define TOML17_UNREACHABLE			__assume(0)

#elif defined(__clang__)

	#define TOML17_LANG_VERSION __cplusplus

	#if __has_feature(cxx_rtti)
		#define TOML17_RTTI_ENABLED
	#endif

	#ifdef __EXCEPTIONS
		#define TOML17_EXCEPTIONS_ENABLED
	#endif

	#define TOML17_DISABLE_WARNINGS						\
		_Pragma("clang diagnostic push")				\
		_Pragma("clang diagnostic ignored \"-Wall\"")	\
		_Pragma("clang diagnostic ignored \"-Wextra\"")

	#define TOML17_RESTORE_WARNINGS						\
		_Pragma("clang diagnostic pop")

	#define TOML17_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
	#define TOML17_ASSUME(cond)			__builtin_assume(cond)
	#define TOML17_UNREACHABLE			__builtin_unreachable()

#elif defined(__GNUC__)

	#define TOML17_LANG_VERSION __cplusplus

	#ifdef __GXX_RTTI
		#define TOML17_RTTI_ENABLED
	#endif

	#ifdef __EXCEPTIONS
		#define TOML17_EXCEPTIONS_ENABLED
	#endif

	#define TOML17_DISABLE_WARNINGS						\
		Pragma("GCC diagnostic push")					\
		Pragma("GCC diagnostic ignored \"-Wall\"")

	#define TOML17_RESTORE_WARNINGS						\
		Pragma("GCC diagnostic pop")

	#define TOML17_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
	#define TOML17_UNREACHABLE			__builtin_unreachable()

#endif

#if TOML17_LANG_VERSION < 201703L
	#error Toml17 C++17 or higher. For a C++ TOML parser that supports as low as C++11 see https://github.com/skystrife/cpptoml
#endif

// #ifndef TOML17_RTTI_ENABLED
//     #error This fork of cpptoml requires RTTI be enabled. For a version that supports disabling RTTI see https://github.com/skystrife/cpptoml
// #endif

#ifndef TOML17_EXCEPTIONS_ENABLED
	#error Toml17 currently requires that exceptions be enabled.
#endif

#if TOML17_LANG_VERSION >= 202600LL
	#define TOML17_LANG 26
#elif TOML17_LANG_VERSION >= 202300LL
	#define TOML17_LANG 23
#elif TOML17_LANG_VERSION >= 202000LL
	#define TOML17_LANG 20
#else
	#define TOML17_LANG 17
#endif

#if __has_include(<version>)
	TOML17_DISABLE_WARNINGS
	#include <version>
	TOML17_RESTORE_WARNINGS
#endif

#if __has_cpp_attribute(likely)
	#define TOML17_LIKELY [[likely]]
#else
	#define TOML17_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
	#define TOML17_UNLIKELY [[unlikely]]
#else
	#define TOML17_UNLIKELY
#endif

#if __has_cpp_attribute(no_unique_address)
	#define TOML17_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
	#define TOML17_NO_UNIQUE_ADDRESS
#endif

#define TOML17_SPEC_VERSION_MAJOR 0
#define TOML17_SPEC_VERSION_MINOR 5
#define TOML17_SPEC_VERSION_REVISION 0

#ifndef TOML17_DISABLE_WARNINGS
	#define TOML17_DISABLE_WARNINGS
#endif
#ifndef TOML17_RESTORE_WARNINGS
	#define TOML17_RESTORE_WARNINGS
#endif
#ifndef TOML17_INTERFACE
	#define TOML17_INTERFACE
#endif
#ifndef TOML17_EMPTY_BASES
	#define TOML17_EMPTY_BASES
#endif
#ifndef TOML17_ALWAYS_INLINE
	#define TOML17_ALWAYS_INLINE inline
#endif
#ifndef TOML17_ASSUME(cond)
	#define (void)0
#endif
#ifndef TOML17_UNREACHABLE
	#define (void)0
#endif

#define TOML17_NO_DEFAULT_CASE		default: TOML17_UNREACHABLE

//--------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------

TOML17_DISABLE_WARNINGS

#include <cstdint>
#include <optional>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
// include <sstream>
// include <fstream>
// include <sstream>
// include <type_traits>
// include <iomanip>
// #include <unordered_map>

TOML17_RESTORE_WARNINGS

//--------------------------------------------------------------------
// FORWARD DECLARATIONS & TYPEDEFS
//--------------------------------------------------------------------
// clang-format on

namespace TOML17_NAMESPACE
{
	/*class value;
	class array;*/

#if TOML17_LANG >= 20 || (defined(_MSC_VER) && defined(_HAS_CXX20))
	template <typename T>
	using remove_cvref_t = std::remove_cvref_t<T>;
#else
	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
#endif

	template <typename T>
	inline constexpr bool is_value_type =
		std::is_same_v<T, std::string>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, double>
		|| std::is_same_v<T, bool>;


	template <typename T>
	class node;

	template <>
	class node<void>
	{

		virtual ~node() noexcept = default;
	};
}
