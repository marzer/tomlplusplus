#pragma once

//--------------------------------------------------------------------
// CONFIGURATION
//--------------------------------------------------------------------
// clang-format off

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
#endif

#ifndef TOML_NAMESPACE
	#define TOML_NAMESPACE toml
#endif

#ifndef TOML_USE_CHAR_8_IF_AVAILABLE
	#define TOML_USE_CHAR_8_IF_AVAILABLE 1
#endif

#ifndef TOML_ASSERT
	#define TOML_DEFAULT_ASSERT
#endif

//--------------------------------------------------------------------
// COMPILER & ENVIRONMENT STUFF
//--------------------------------------------------------------------

#ifndef __cplusplus
	#error Toml17 is a C++ library.
#endif

#if defined(_MSC_VER)

	#define TOML_LANG_VERSION _MSVC_LANG

	#ifdef _CPPRTTI
		#define TOML_RTTI_ENABLED
	#endif

	#ifdef _CPPUNWIND
		#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_DISABLE_WARNINGS		__pragma(warning(push, 0))
	#define TOML_RESTORE_WARNINGS		__pragma(warning(pop, 0))

	#define TOML_INTERFACE				__declspec(novtable)
	#define TOML_EMPTY_BASES			__declspec(empty_bases)
	#define TOML_ALWAYS_INLINE			__forceinline
	#define TOML_ASSUME(cond)			__assume(cond)
	#define TOML_UNREACHABLE			__assume(0)

#elif defined(__clang__)

	#define TOML_LANG_VERSION __cplusplus

	#if __has_feature(cxx_rtti)
		#define TOML_RTTI_ENABLED
	#endif

	#ifdef __EXCEPTIONS
		#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_DISABLE_WARNINGS						\
		_Pragma("clang diagnostic push")				\
		_Pragma("clang diagnostic ignored \"-Wall\"")	\
		_Pragma("clang diagnostic ignored \"-Wextra\"")

	#define TOML_RESTORE_WARNINGS						\
		_Pragma("clang diagnostic pop")

	#define TOML_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
	#define TOML_ASSUME(cond)			__builtin_assume(cond)
	#define TOML_UNREACHABLE			__builtin_unreachable()

#elif defined(__GNUC__)

	#define TOML_LANG_VERSION __cplusplus

	#ifdef __GXX_RTTI
		#define TOML_RTTI_ENABLED
	#endif

	#ifdef __EXCEPTIONS
		#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_DISABLE_WARNINGS						\
		Pragma("GCC diagnostic push")					\
		Pragma("GCC diagnostic ignored \"-Wall\"")

	#define TOML_RESTORE_WARNINGS						\
		Pragma("GCC diagnostic pop")

	#define TOML_ALWAYS_INLINE		__attribute__((__always_inline__)) inline
	#define TOML_UNREACHABLE			__builtin_unreachable()

#endif

#if TOML_LANG_VERSION < 201703L
	#error Toml17 requires C++17 or higher. For a C++ TOML parser that supports as low as C++11 see https://github.com/skystrife/cpptoml
#endif

// #ifndef TOML_RTTI_ENABLED
//     #error Toml17 requires requires RTTI be enabled. For a C++ TOML parser that supports disabling RTTI see https://github.com/skystrife/cpptoml
// #endif

#ifndef TOML_EXCEPTIONS_ENABLED
	#error Toml17 currently requires that exceptions be enabled.
#endif

#if TOML_LANG_VERSION >= 202600LL
	#define TOML_LANG 26
#elif TOML_LANG_VERSION >= 202300LL
	#define TOML_LANG 23
#elif TOML_LANG_VERSION >= 202000LL
	#define TOML_LANG 20
#else
	#define TOML_LANG 17
#endif

#ifndef TOML_DISABLE_WARNINGS
	#define TOML_DISABLE_WARNINGS
#endif
#ifndef TOML_RESTORE_WARNINGS
	#define TOML_RESTORE_WARNINGS
#endif

#ifdef TOML_DEFAULT_ASSERT
	TOML_DISABLE_WARNINGS
	#include <cassert>
	TOML_RESTORE_WARNINGS
	#define TOML_ASSERT(cond)	assert(cond)
#endif

#if __has_include(<version>)
	TOML_DISABLE_WARNINGS
	#include <version>
	TOML_RESTORE_WARNINGS
#endif

#if __has_cpp_attribute(likely)
	#define TOML_LIKELY [[likely]]
#else
	#define TOML_LIKELY
#endif

#if __has_cpp_attribute(unlikely)
	#define TOML_UNLIKELY [[unlikely]]
#else
	#define TOML_UNLIKELY
#endif

#if __has_cpp_attribute(no_unique_address)
	#define TOML_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
	#define TOML_NO_UNIQUE_ADDRESS
#endif

#if TOML_LANG >= 20
	#define TOML_CONSTEVAL	consteval
#else
	#define TOML_CONSTEVAL	constexpr
#endif

#define TOML_SPEC_VERSION_MAJOR		0
#define TOML_SPEC_VERSION_MINOR		5
#define TOML_SPEC_VERSION_REVISION	0

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
	#define TOML_ASSUME(cond)		(void)0
#endif
#ifndef TOML_UNREACHABLE
	#define TOML_UNREACHABLE		(void)0
#endif

#define TOML_NO_DEFAULT_CASE		default: TOML_UNREACHABLE

#define TOML_NOT_IMPLEMENTED_YET	TOML_ASSERT(false && "Implement me")

//--------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------

TOML_DISABLE_WARNINGS

#include <cstdint>
#include <optional>
#include <memory>
#include <string_view>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <iosfwd>
#include <stdexcept>
#include <filesystem>

TOML_RESTORE_WARNINGS

//--------------------------------------------------------------------
// FORWARD DECLARATIONS & TYPEDEFS
//--------------------------------------------------------------------
// clang-format on

namespace TOML_NAMESPACE
{
#	if TOML_LANG >= 20 || (defined(_MSC_VER) && defined(_HAS_CXX20))

	template <typename T>
	using remove_cvref_t = std::remove_cvref_t<T>;

	#else

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	#endif

	namespace impl
	{
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

		class parser;
	}

	struct date final
	{
		uint16_t year;
		uint8_t month;
		uint8_t day;
	};

	struct time final
	{
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint32_t microsecond;
	};

	struct time_offset final
	{
		int8_t hours;
		int8_t minutes;
	};

	struct datetime final
	{
		std::optional<date> date;
		std::optional<time> time;
		std::optional<time_offset> time_offset;
	};

	#if defined(__cpp_lib_char8_t) && TOML_USE_CHAR_8_IF_AVAILABLE

	using string = std::u8string;
	using string_view = std::u8string_view;

	#else

	using string = std::string;
	using string_view = std::string_view;

	#endif

	using path = std::filesystem::path;

	template <typename T>
	inline constexpr bool is_value_type =
		std::is_same_v<T, string>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, double>
		|| std::is_same_v<T, bool>
		|| std::is_same_v<T, datetime>;

	template <typename T>	class value;
	template <>				class value<string>;
	template <>				class value<int64_t>;
	template <>				class value<double>;
	template <>				class value<bool>;
	template <>				class value<datetime>;
	class array;
	class table;
	class table_array;

	struct document_position
	{
		size_t line;	//begins at 1
		size_t column;	//begins at 1
		size_t index;	//begins at 0
	};

	struct document_region
	{
		document_position begin;
		document_position end;
		std::shared_ptr<const path> source_path;
	};

	class TOML_INTERFACE node
		: public std::enable_shared_from_this<node>
	{
		private:
			friend class impl::parser;
			document_region region_;

			[[nodiscard]] TOML_ALWAYS_INLINE value<string>* reinterpret_as_string() noexcept { return reinterpret_cast<value<string>*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE value<int64_t>* reinterpret_as_int() noexcept	{ return reinterpret_cast<value<int64_t>*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE value<double>* reinterpret_as_float() noexcept { return reinterpret_cast<value<double>*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE value<bool>* reinterpret_as_bool() noexcept { return reinterpret_cast<value<bool>*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE value<datetime>* reinterpret_as_datetime() noexcept { return reinterpret_cast<value<datetime>*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE array* reinterpret_as_array() noexcept { return reinterpret_cast<array*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE table* reinterpret_as_table() noexcept { return reinterpret_cast<table*>(this); }
			[[nodiscard]] TOML_ALWAYS_INLINE table_array* reinterpret_as_table_array() noexcept { return reinterpret_cast<table_array*>(this); }

		public:

			[[nodiscard]] virtual bool is_value() const noexcept = 0;
			[[nodiscard]] virtual bool is_string() const noexcept = 0;
			[[nodiscard]] virtual bool is_int() const noexcept = 0;
			[[nodiscard]] virtual bool is_float() const noexcept = 0;
			[[nodiscard]] virtual bool is_bool() const noexcept = 0;
			[[nodiscard]] virtual bool is_datetime() const noexcept = 0;
			[[nodiscard]] virtual bool is_array() const noexcept = 0;
			[[nodiscard]] virtual bool is_table() const noexcept = 0;
			[[nodiscard]] virtual bool is_table_array() const noexcept = 0;

			[[nodiscard]] virtual std::shared_ptr<value<string>> as_string() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<value<int64_t>> as_int() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<value<double>> as_float() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<value<bool>> as_bool() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<value<datetime>> as_datetime() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<array> as_array() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<table> as_table() noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<table_array> as_table_array() noexcept = 0;

			[[nodiscard]] virtual std::shared_ptr<const value<string>> as_string() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const value<int64_t>> as_int() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const value<double>> as_float() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const value<bool>> as_bool() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const value<datetime>> as_datetime() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const array> as_array() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const table> as_table() const noexcept = 0;
			[[nodiscard]] virtual std::shared_ptr<const table_array> as_table_array() const noexcept = 0;

			[[nodiscard]]
			const document_region& region() const noexcept
			{
				return region_;
			}

			virtual ~node() noexcept = default;
	};
}
