#pragma once
#include "settings.h"

#if USE_TARTANLLAMA_OPTIONAL
	#include "tloptional.h"
#endif

#include "evil_macros.h"

#if USE_SINGLE_HEADER
	#include "../toml.hpp"
#else
	#include "../include/toml++/toml.h"
#endif

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS

#include "catch2.h"
#include <sstream>
namespace toml {}
using namespace Catch::literals;
using namespace toml;

TOML_POP_WARNINGS

#define FILE_LINE_ARGS	std::string_view{ __FILE__ }, __LINE__
#define BOM_PREFIX "\xEF\xBB\xBF"

#if TOML_EXCEPTIONS
	#define FORCE_FAIL(...) FAIL(__VA_ARGS__)
#else
	#define FORCE_FAIL(...)		\
	do							\
	{							\
		FAIL(__VA_ARGS__);		\
		std::exit(-1);			\
		TOML_UNREACHABLE;		\
	}							\
	while (false)
#endif

[[nodiscard]]
TOML_ATTR(const)
inline double make_infinity(int sign = 1) noexcept
{
	constexpr uint64_t pos_inf = 0b0111111111110000000000000000000000000000000000000000000000000000ull;
	constexpr uint64_t neg_inf = 0b1111111111110000000000000000000000000000000000000000000000000000ull;
	double val;
	std::memcpy(&val, sign >= 0 ? &pos_inf : &neg_inf, sizeof(double));
	return val;
}

[[nodiscard]]
TOML_ATTR(const)
inline double make_nan() noexcept
{
	constexpr uint64_t qnan = 0b0111111111111000000000000000000000000000000000000000000000000000ull;
	double val;
	std::memcpy(&val, &qnan, sizeof(double));
	return val;
}

// function_view - adapted from here: https://vittorioromeo.info/index/blog/passing_functions_to_functions.html
template <typename Func>
class function_view;
template <typename R, typename... P>
class function_view<R(P...)> final
{
	private:
		using func_type = R(P...);
		using eraser_func_type = R(void*, P&&...);

		mutable void* ptr_ = {};
		mutable eraser_func_type* eraser = {};

	public:

		function_view() noexcept = default;

		template <typename T>
		function_view(T&& x) noexcept
			: ptr_{ reinterpret_cast<void*>(std::addressof(x)) }
		{
			eraser = [](void* ptr, P&&... xs) -> R
			{
				return (*reinterpret_cast<std::add_pointer_t<std::remove_reference_t<T>>>(ptr))(std::forward<P>(xs)...);
			};
		}

		decltype(auto) operator()(P&&... xs) const
		{
			return eraser(ptr_, std::forward<P>(xs)...);
		}

		[[nodiscard]] operator bool() const noexcept { return !!ptr_; }
};

using pss_func = function_view<void(toml::table&&)>;

bool parsing_should_succeed(
	std::string_view test_file,
	uint32_t test_line,
	std::string_view toml_str,
	pss_func&& func = {},
	std::string_view source_path = {});

bool parsing_should_fail(
	std::string_view test_file,
	uint32_t test_line,
	std::string_view toml_str);

TOML_PUSH_WARNINGS
TOML_DISABLE_ARITHMETIC_WARNINGS

template <typename T>
inline bool parse_expected_value(
	std::string_view test_file,
	uint32_t test_line,
	std::string_view value_str,
	const T& expected)
{
	INFO("["sv << test_file << ", line "sv << test_line << "] "sv << "parse_expected_value(\""sv << value_str << "\")"sv)

	std::string val;
	static constexpr auto key = "val = "sv;
	val.reserve(key.length() + value_str.length());
	val.append(key);
	val.append(value_str);

	static constexpr auto is_val = [](char32_t codepoint)
	{
		if constexpr (impl::node_type_of<T> == node_type::string)
			return codepoint == U'"' || codepoint == U'\'';
		else
			return !impl::is_whitespace(codepoint);
	};

	source_position pos{ 1,  static_cast<source_index>(key.length()) };
	source_position begin{}, end{};
	{
		impl::utf8_decoder decoder;
		for (auto c : value_str)
		{
			decoder(static_cast<uint8_t>(c));
			if (!decoder.has_code_point())
				continue;

			if (impl::is_line_break(decoder.codepoint))
			{
				if (decoder.codepoint != U'\r')
				{
					pos.line++;
					pos.column = source_index{ 1 };
				}
				continue;
			}

			pos.column++;
			if (is_val(decoder.codepoint))
			{
				if (!begin)
					begin = pos;
				else
					end = pos;
			}
		}
		if (!end)
			end = begin;
		end.column++;
	}

	using value_type = impl::native_type_of<impl::remove_cvref_t<T>>;
	value<value_type> val_parsed;
	{
		INFO("["sv << test_file << ", line "sv << test_line << "] "sv << "parse_expected_value: Checking initial parse"sv)

		bool stolen_value = false; // parsing_should_succeed invokes the functor more than once
		const auto result = parsing_should_succeed(
			test_file,
			test_line,
			std::string_view{ val },
			[&](table&& tbl)
			{
				REQUIRE(tbl.size() == 1);
				auto nv = tbl["val"sv];
				REQUIRE(nv);
				REQUIRE(nv.is<value_type>());
				REQUIRE(nv.as<value_type>());
				REQUIRE(nv.type() == impl::node_type_of<T>);
				REQUIRE(nv.node());
				REQUIRE(nv.node()->is<value_type>());
				REQUIRE(nv.node()->as<value_type>());
				REQUIRE(nv.node()->type() == impl::node_type_of<T>);

				// check the raw value
				REQUIRE(nv.node()->value<value_type>() == expected);
				REQUIRE(nv.node()->value_or(T{}) == expected);
				REQUIRE(nv.as<value_type>()->get() == expected);
				REQUIRE(nv.value<value_type>() == expected);
				REQUIRE(nv.value_or(T{}) == expected);
				REQUIRE(nv.ref<value_type>() == expected);
				REQUIRE(nv.node()->ref<value_type>() == expected);

				// check the table relops
				REQUIRE(tbl == table{ { { "val"sv, expected } } });
				REQUIRE(!(tbl != table{ { { "val"sv, expected } } }));

				// check the value relops
				REQUIRE(*nv.as<value_type>() == expected);
				REQUIRE(expected == *nv.as<value_type>());
				REQUIRE(!(*nv.as<value_type>() != expected));
				REQUIRE(!(expected != *nv.as<value_type>()));

				// check the node_view relops
				REQUIRE(nv == expected);
				REQUIRE(expected == nv);
				REQUIRE(!(nv != expected));
				REQUIRE(!(expected != nv));

				// make sure source info is correct
				REQUIRE(nv.node()->source().begin == begin);
				REQUIRE(nv.node()->source().end == end);

				// steal the val for round-trip tests
				if (!stolen_value)
				{
					val_parsed = std::move(*nv.as<value_type>());
					stolen_value = true;
				}
			}
		);

		if (!result)
			return false;
	}

	// check round-tripping
	{
		INFO("["sv << test_file << ", line "sv << test_line << "] "sv << "parse_expected_value: Checking round-trip"sv)
		{
			std::string str;
			{
				auto tbl = table{ { { "val"sv, *val_parsed } } };
				std::ostringstream ss;
				ss << tbl;
				str = std::move(ss).str();
			}

			bool value_ok = true;
			const auto parse_ok = parsing_should_succeed(
				test_file,
				test_line,
				std::string_view{ str },
				[&](table&& tbl)
				{
					REQUIRE(tbl.size() == 1);
					auto nv = tbl["val"sv];
					REQUIRE(nv);
					REQUIRE(nv.as<value_type>());
					REQUIRE(nv.node()->type() == impl::node_type_of<T>);

					if (value_ok && nv.ref<value_type>() != expected)
					{
						value_ok = false;
						FORCE_FAIL("Value was not the same after round-tripping"sv);
					}
				}
			);

			if (!parse_ok || value_ok)
				return false;
		}
	}

	return true;
}

// manually instantiate some templates to reduce obj bloat and test compilation time

extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const int&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const unsigned int&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const bool&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const float&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const double&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const std::string_view&);
namespace std
{
	extern template class unique_ptr<const Catch::IExceptionTranslator>;
}
namespace Catch
{
	extern template struct StringMaker<node_view<node>>;
	extern template struct StringMaker<node_view<const node>>;
	extern template ReusableStringStream& ReusableStringStream::operator << (node_view<node> const&);
	extern template ReusableStringStream& ReusableStringStream::operator << (node_view<const node> const&);
	namespace Detail
	{
		extern template std::string stringify(const node_view<node>&);
		extern template std::string stringify(const node_view<const node>&);
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_ARITHMETIC_WARNINGS
