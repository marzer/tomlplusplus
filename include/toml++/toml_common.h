#pragma once

//--------------------------------------------------------------------
// CONFIGURATION
//--------------------------------------------------------------------
// clang-format off

#ifdef TOML_CONFIG_HEADER
	#include TOML_CONFIG_HEADER
	#undef TOML_CONFIG_HEADER
#endif

#ifndef TOML_USE_CHAR_8_IF_AVAILABLE
	#define TOML_USE_CHAR_8_IF_AVAILABLE 0
#endif

#ifndef TOML_STRICT
	#define TOML_STRICT 0
#endif

#ifndef TOML_ASSERT
	#ifdef assert
		#define TOML_ASSERT(expr)	assert(expr)
	#else
		#define TOML_ASSERT(expr)	(void)0
	#endif
#endif

//--------------------------------------------------------------------
// COMPILER & ENVIRONMENT STUFF
//--------------------------------------------------------------------

#ifndef __cplusplus
	#error toml++ is a C++ library.
#endif

#ifdef __clang__

	#ifdef __cpp_exceptions
	#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_PUSH_WARNINGS			_Pragma("clang diagnostic push")
	#define TOML_DISABLE_ALL_WARNINGS	_Pragma("clang diagnostic ignored \"-Wall\"")	\
										_Pragma("clang diagnostic ignored \"-Wextra\"")
	#define TOML_POP_WARNINGS			_Pragma("clang diagnostic pop")
	#define TOML_CPP_VERSION			__cplusplus
	#define TOML_ALWAYS_INLINE			__attribute__((__always_inline__)) inline
	#define TOML_ASSUME(cond)			__builtin_assume(cond)
	#define TOML_UNREACHABLE			__builtin_unreachable()

#elif defined(_MSC_VER)

	#ifdef _CPPUNWIND
		#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_CPP_VERSION			_MSVC_LANG
	#define TOML_PUSH_WARNINGS			__pragma(warning(push))
	#define TOML_DISABLE_ALL_WARNINGS	__pragma(warning(pop))	\
										__pragma(warning(push, 0))
	#define TOML_POP_WARNINGS			__pragma(warning(pop))
	#define TOML_INTERFACE				__declspec(novtable)
	#define TOML_EMPTY_BASES			__declspec(empty_bases)
	#define TOML_ALWAYS_INLINE			__forceinline
	#define TOML_ASSUME(cond)			__assume(cond)
	#define TOML_UNREACHABLE			__assume(0)

#elif defined(__GNUC__)

	#ifdef __cpp_exceptions
		#define TOML_EXCEPTIONS_ENABLED
	#endif

	#define TOML_PUSH_WARNINGS			Pragma("GCC diagnostic push")
	#define TOML_DISABLE_ALL_WARNINGS	Pragma("GCC diagnostic ignored \"-Wall\"")
	#define TOML_POP_WARNINGS			Pragma("GCC diagnostic pop")
	#define TOML_CPP_VERSION			__cplusplus
	#define TOML_ALWAYS_INLINE			__attribute__((__always_inline__)) inline
	#define TOML_UNREACHABLE			__builtin_unreachable()

#endif

#if TOML_CPP_VERSION >= 202600L
	#define TOML_CPP 26
#elif TOML_CPP_VERSION >= 202300L
	#define TOML_CPP 23
#elif TOML_CPP_VERSION >= 202000L
	#define TOML_CPP 20
#elif TOML_CPP_VERSION >= 201703L
	#define TOML_CPP 17
#else
	#error toml++ requires C++17 or higher. For a C++ TOML parser that supports as low as C++11 see https://github.com/skystrife/cpptoml
#endif

#ifndef TOML_EXCEPTIONS_ENABLED
#error toml++ currently requires that exceptions be enabled.
#endif
#undef TOML_EXCEPTIONS_ENABLED

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
	#define TOML_UNREACHABLE	(void)0
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
#if TOML_STRICT
	#define TOML_ALWAYS_INLINE_WHEN_STRICT TOML_ALWAYS_INLINE
#else
	#define TOML_ALWAYS_INLINE_WHEN_STRICT inline
#endif

#define TOML_SPEC_VERSION_MAJOR		0
#define TOML_SPEC_VERSION_MINOR		5
#define TOML_SPEC_VERSION_REVISION	0

//--------------------------------------------------------------------
// INCLUDES
//--------------------------------------------------------------------

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS

#include <cstdint>
#include <optional>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <charconv>
#include <iosfwd>

TOML_POP_WARNINGS

#if TOML_USE_CHAR_8_IF_AVAILABLE && defined(__cpp_lib_char8_t)

	#define TOML_CHAR_8	1
	#define TOML_STRING_PREFIX_1(S) u8##S
	#define TOML_STRING_PREFIX(S) TOML_STRING_PREFIX_1(S)

#else

	#define TOML_CHAR_8	0
	#define TOML_STRING_PREFIX(S) S

#endif

//--------------------------------------------------------------------
// FORWARD DECLARATIONS & TYPEDEFS
//--------------------------------------------------------------------
// clang-format on

namespace toml
{
	#if defined(__cpp_lib_remove_cvref) || (defined(_MSC_VER) && defined(_HAS_CXX20))

	template <typename T>
	using remove_cvref_t = std::remove_cvref_t<T>;

	#else

	template <typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	#endif

	namespace impl
	{
		using namespace std::literals::string_literals;
		using namespace std::literals::string_view_literals;

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
		class writer;
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
		date date;
		time time;
		std::optional<time_offset> time_offset;
	};

	#if TOML_CHAR_8

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

	template <typename T>
	inline constexpr bool is_value_type =
		std::is_same_v<T, string>
		|| std::is_same_v<T, int64_t>
		|| std::is_same_v<T, double>
		|| std::is_same_v<T, bool>
		|| std::is_same_v<T, date>
		|| std::is_same_v<T, time>
		|| std::is_same_v<T, datetime>;

	template <typename T>
	class value;
	class array;
	class table;
	class table_array;

	struct document_position
	{
		uint32_t line;		//begins at 1
		uint32_t column;	//begins at 1
	};

	struct document_region
	{
		document_position begin;
		document_position end;
		std::shared_ptr<const string> source_path;
	};

	enum class node_type : uint8_t
	{
		array,
		table,
		table_array,
		string,
		integer,
		floating_point,
		boolean,
		date,
		time,
		datetime
	};

	class TOML_INTERFACE node
	{
		private:
			friend class impl::parser;
			document_region rgn{};

		public:

			[[nodiscard]] virtual bool is_value() const noexcept = 0;
			[[nodiscard]] virtual bool is_string() const noexcept = 0;
			[[nodiscard]] virtual bool is_int() const noexcept = 0;
			[[nodiscard]] virtual bool is_float() const noexcept = 0;
			[[nodiscard]] virtual bool is_bool() const noexcept = 0;
			[[nodiscard]] virtual bool is_date() const noexcept = 0;
			[[nodiscard]] virtual bool is_time() const noexcept = 0;
			[[nodiscard]] virtual bool is_datetime() const noexcept = 0;
			[[nodiscard]] virtual bool is_array() const noexcept = 0;
			[[nodiscard]] virtual bool is_table() const noexcept = 0;
			[[nodiscard]] virtual bool is_table_array() const noexcept = 0;
			
			[[nodiscard]] virtual value<string>* as_string() noexcept = 0;
			[[nodiscard]] virtual value<int64_t>* as_int() noexcept = 0;
			[[nodiscard]] virtual value<double>* as_float() noexcept = 0;
			[[nodiscard]] virtual value<bool>* as_bool() noexcept = 0;
			[[nodiscard]] virtual value<date>* as_date() noexcept = 0;
			[[nodiscard]] virtual value<time>* as_time() noexcept = 0;
			[[nodiscard]] virtual value<datetime>* as_datetime() noexcept = 0;
			[[nodiscard]] virtual array* as_array() noexcept = 0;
			[[nodiscard]] virtual table* as_table() noexcept = 0;
			[[nodiscard]] virtual table_array* as_table_array() noexcept = 0;

			[[nodiscard]] virtual const value<string>* as_string() const noexcept = 0;
			[[nodiscard]] virtual const value<int64_t>* as_int() const noexcept = 0;
			[[nodiscard]] virtual const value<double>* as_float() const noexcept = 0;
			[[nodiscard]] virtual const value<bool>* as_bool() const noexcept = 0;
			[[nodiscard]] virtual const value<date>* as_date() const noexcept = 0;
			[[nodiscard]] virtual const value<time>* as_time() const noexcept = 0;
			[[nodiscard]] virtual const value<datetime>* as_datetime() const noexcept = 0;
			[[nodiscard]] virtual const array* as_array() const noexcept = 0;
			[[nodiscard]] virtual const table* as_table() const noexcept = 0;
			[[nodiscard]] virtual const table_array* as_table_array() const noexcept = 0;

			[[nodiscard]] virtual node_type type() const noexcept = 0;

			[[nodiscard]]
			const document_region& region() const noexcept
			{
				return rgn;
			}

			virtual ~node() noexcept = default;
	};

	class parse_error final
		: public std::runtime_error
	{
		private:
			document_region region;

		public:
			parse_error(const char* description, document_region&& region_) noexcept
				: std::runtime_error{ description },
				region{ std::move(region_) }
			{}

			parse_error(const char* description, const document_region& region_) noexcept
				: std::runtime_error{ description },
				region{ region_ }
			{}

			parse_error(const char* description, const document_position& position, const std::shared_ptr<const string>& source_path) noexcept
				: std::runtime_error{ description },
				region{ position, position, source_path }
			{}

			parse_error(const char* description, const document_position& position) noexcept
				: std::runtime_error{ description },
				region{ position, position }
			{}

			[[nodiscard]]
			const document_region& where() const noexcept
			{
				return region;
			}
	};
}
