#pragma once
#define TOML_UNDEF_MACROS 0
#if !defined(_MSC_VER) || !defined(_M_IX86)
	#define TOML_ALL_INLINE 0
#endif
#include "../include/toml++/toml_preprocessor.h"

#if TOML_COMPILER_EXCEPTIONS != TOML_EXCEPTIONS
	#error TOML_EXCEPTIONS does not match TOML_COMPILER_EXCEPTIONS (default behaviour should be to match)
#endif

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#ifdef TARTANLLAMA_OPTIONAL
	#if __has_include(<tloptional/include/tl/optional.hpp>)
		#include <tloptional/include/tl/optional.hpp>
	#else
		#error  TartanLlama/optional is missing! You probably need to fetch submodules ("git submodule update --init extern/tloptional")
	#endif
	#define TOML_OPTIONAL_TYPE tl::optional
#endif
#include "catch2.h"
#include <sstream>
namespace toml {}
using namespace Catch::literals;
using namespace toml;
TOML_POP_WARNINGS

#include "../include/toml++/toml.h"

#define FILE_LINE_ARGS	std::string_view{ __FILE__ }, __LINE__
#define S(str)			TOML_STRING_PREFIX(str)
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

TOML_PUSH_WARNINGS
TOML_DISABLE_FLOAT_WARNINGS


template <typename Char, typename Func = std::false_type>
inline bool parsing_should_succeed(
	std::string_view test_file,
	uint32_t test_line,
	std::basic_string_view<Char> toml_str,
	Func&& func = {},
	std::string_view source_path = {})
{
	INFO(
		"["sv << test_file << ", line "sv << test_line << "] "sv
		<< "parsing_should_succeed(\""sv << std::string_view(reinterpret_cast<const char*>(toml_str.data()), toml_str.length()) << "\")"sv
	)

	constexpr auto validate_table = [](table&& tabl, std::string_view path)  -> table&&
	{
		INFO("Validating table source information"sv)
		CHECK(tabl.source().begin != source_position{});
		CHECK(tabl.source().end != source_position{});
		if (path.empty())
			CHECK(tabl.source().path == nullptr);
		else
		{
			REQUIRE(tabl.source().path != nullptr);
			CHECK(*tabl.source().path == path);
		}
		return std::move(tabl);
	};

	static constexpr auto is_functor = !std::is_same_v<impl::remove_cvref_t<Func>, std::false_type>;

	#if TOML_EXCEPTIONS

	try
	{
		{
			INFO("Parsing string directly"sv)
			if constexpr (is_functor)
				std::forward<Func>(func)(validate_table(toml::parse(toml_str, source_path), source_path));
			else
				validate_table(toml::parse(toml_str, source_path), source_path);
		}
		{
			INFO("Parsing from a string stream"sv)
			std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> ss;
			ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
			if constexpr (is_functor)
				std::forward<Func>(func)(validate_table(toml::parse(ss, source_path), source_path));
			else
				validate_table(toml::parse(ss, source_path), source_path);
		}
	}
	catch (const parse_error& err)
	{
		FORCE_FAIL(
			"Parse error on line "sv << err.source().begin.line
			<< ", column "sv << err.source().begin.column
			<< ":\n"sv << err.description()
		);
		return false;
	}

	#else

	{
		INFO("Parsing string directly"sv)
		parse_result result = toml::parse(toml_str, source_path);
		if (result)
		{
			if constexpr (is_functor)
				std::forward<Func>(func)(validate_table(std::move(result), source_path));
			else
				validate_table(std::move(result), source_path);
		}
		else
		{
			FORCE_FAIL(
				"Parse error on line "sv << result.error().source().begin.line
				<< ", column "sv << result.error().source().begin.column
				<< ":\n"sv << result.error().description()
			);
		}
	}

	{
		INFO("Parsing from a string stream"sv)
		std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> ss;
		ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
		parse_result result = toml::parse(ss, source_path);
		if (result)
		{
			if constexpr (is_functor)
				std::forward<Func>(func)(validate_table(std::move(result), source_path));
			else
				validate_table(std::move(result), source_path);
		}
		else
		{
			FORCE_FAIL(
				"Parse error on line "sv << result.error().source().begin.line
				<< ", column "sv << result.error().source().begin.column
				<< ":\n"sv << result.error().description()
			);
		}
	}

	#endif

	return true;
}

template <typename Char>
inline bool parsing_should_fail(
	std::string_view test_file,
	uint32_t test_line,
	std::basic_string_view<Char> toml_str)
{
	INFO(
		"["sv << test_file << ", line "sv << test_line << "] "sv
		<< "parsing_should_fail(\""sv << std::string_view(reinterpret_cast<const char*>(toml_str.data()), toml_str.length()) << "\")"sv
	)

	#if TOML_EXCEPTIONS

	static constexpr auto run_tests = [](auto&& fn)
	{
		try
		{
			fn();
		}
		catch (const parse_error&)
		{
			SUCCEED("parse_error thrown OK"sv);
			return true;
		}
		catch (const std::exception& exc)
		{
			FORCE_FAIL("Expected parsing failure, saw exception: "sv << exc.what());
			return false;
		}
		catch (...)
		{
			FORCE_FAIL("Expected parsing failure, saw unspecified exception"sv);
			return false;
		}

		FORCE_FAIL("Expected parsing failure"sv);
		return false;
	};

	return run_tests([=]() { (void)toml::parse(toml_str); })
		&& run_tests([=]()
		{
			std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> ss;
			ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
			(void)toml::parse(ss);
		});

	#else

	static constexpr auto run_tests = [](auto&& fn)
	{
		if (parse_result result = fn(); !result)
		{
			SUCCEED("parse_error generated OK"sv);
			return true;
		}

		FORCE_FAIL("Expected parsing failure"sv);
	};

	return run_tests([=]() { return toml::parse(toml_str); })
		&& run_tests([=]()
		{
			std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> ss;
			ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
			return toml::parse(ss);
		});

	#endif
}

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
		if constexpr (std::is_same_v<string, impl::promoted<T>>)
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

	using value_type = impl::promoted<impl::remove_cvref_t<T>>;
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
				auto nv = tbl[S("val"sv)];
				REQUIRE(nv);
				REQUIRE(nv.as<value_type>());
				REQUIRE(nv.get()->type() == impl::node_type_of<T>);

				// check the raw value
				REQUIRE(nv.get()->value<value_type>() == expected);
				REQUIRE(nv.get()->value_or(T{}) == expected);
				REQUIRE(nv.as<value_type>()->get() == expected);
				REQUIRE(nv.value<value_type>() == expected);
				REQUIRE(nv.value_or(T{}) == expected);
				REQUIRE(nv.ref<value_type>() == expected);
				REQUIRE(nv.get()->ref<value_type>() == expected);

				// check the table relops
				REQUIRE(tbl == table{ { { S("val"sv), expected } } });
				REQUIRE(!(tbl != table{ { { S("val"sv), expected } } }));

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
				REQUIRE(nv.get()->source().begin == begin);
				REQUIRE(nv.get()->source().end == end);

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
				auto tbl = table{ { { S("val"sv), *val_parsed } } };
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
					auto nv = tbl[S("val"sv)];
					REQUIRE(nv);
					REQUIRE(nv.as<value_type>());
					REQUIRE(nv.get()->type() == impl::node_type_of<T>);

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

// manually instantiate some templates to reduce test compilation time (chosen using ClangBuildAnalyzer)
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const int&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const unsigned int&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const bool&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const float&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const double&);
extern template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const toml::string_view&);
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

TOML_POP_WARNINGS // TOML_DISABLE_FLOAT_WARNINGS
