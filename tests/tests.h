#pragma once
#define TOML_UNDEF_MACROS 0
#include "../include/toml++/toml.h"
TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include "catch2.h"
#include <sstream>
using namespace toml;
using namespace Catch::literals;
TOML_POP_WARNINGS

#define S(str) TOML_STRING_PREFIX(str)

template <typename CHAR, typename FUNC>
void parsing_should_succeed(std::basic_string_view<CHAR> toml_str, FUNC&& func, std::string_view source_path = {}) noexcept
{
	constexpr auto validate_table = [](table&& tabl, std::string_view path) noexcept -> table&&
	{
		CHECK(tabl.region().begin != source_position{});
		CHECK(tabl.region().end != source_position{});
		if (path.empty())
			CHECK(tabl.region().path == nullptr);
		else
		{
			REQUIRE(tabl.region().path != nullptr);
			CHECK(*tabl.region().path == path);
		}

		return std::move(tabl);
	};

	#if TOML_EXCEPTIONS

	try
	{
		std::forward<FUNC>(func)(validate_table(toml::parse(toml_str, source_path), source_path));
		{
			std::basic_stringstream<CHAR, std::char_traits<CHAR>, std::allocator<CHAR>> ss;
			ss.write(toml_str.data(), toml_str.length());
			std::forward<FUNC>(func)(validate_table(toml::parse(ss, source_path), source_path));
		}
	}
	catch (const toml::parse_error& err)
	{
		FAIL(
			"Parse error on line "sv << err.where().begin.line
			<< ", column "sv << err.where().begin.column
			<< ":\n"sv << err.what()
		);
	}

	#else

	{
		toml::parse_result result = toml::parse(toml_str, source_path);
		if (result)
			std::forward<FUNC>(func)(validate_table(*std::move(result.root), source_path));
		else
		{
			FAIL(
				"Parse error on line "sv << result.error.where.begin.line
				<< ", column "sv << result.error.where.begin.column
				<< ":\n"sv << result.error.what
			);
			return;
		}
	}

	{
		std::basic_stringstream<CHAR, std::char_traits<CHAR>, std::allocator<CHAR>> ss;
		ss.write(toml_str.data(), toml_str.length());
		toml::parse_result result = toml::parse(ss, source_path);
		if (result)
			std::forward<FUNC>(func)(validate_table(*std::move(result.root), source_path));
		else
		{
			FAIL(
				"Parse error on line "sv << result.error.where.begin.line
				<< ", column "sv << result.error.where.begin.column
				<< ":\n"sv << result.error.what
			);
			return;
		}
	}

	#endif
}

template <typename CHAR>
void parsing_should_fail(std::basic_string_view<CHAR> toml_str) noexcept
{
	#if TOML_EXCEPTIONS

	static constexpr auto run_tests = [](auto&& fn) noexcept
	{
		try
		{
			fn();
		}
		catch (const toml::parse_error&)
		{
			SUCCEED("toml::parse_error thrown OK"sv);
			return true;
		}
		catch (const std::exception& exc)
		{
			FAIL("Expected parsing failure, saw exception: "sv << exc.what());
		}
		catch (...)
		{
			FAIL("Expected parsing failure, saw unspecified error"sv);
		}
		return false;
	};

	if (run_tests([=]() { (void)toml::parse(toml_str); }))
		run_tests([=]()
		{
			std::basic_stringstream<CHAR, std::char_traits<CHAR>, std::allocator<CHAR>> ss;
			ss.write(toml_str.data(), toml_str.length());
			(void)toml::parse(ss);
		});

	#else

	static constexpr auto run_tests = [](auto&& fn) noexcept
	{
		toml::parse_result result = fn();
		if (result)
		{
			FAIL("Expected parsing failure"sv);
			return false;
		}
		else
		{
			SUCCEED("toml::parse_error returned OK"sv);
			return true;
		}
	};

	if (run_tests([=]() noexcept { return toml::parse(toml_str); }))
		run_tests([=]() noexcept
		{
			std::basic_stringstream<CHAR, std::char_traits<CHAR>, std::allocator<CHAR>> ss;
			ss.write(toml_str.data(), toml_str.length());
			return toml::parse(ss);
		});

	#endif
}

template <typename T>
void parse_expected_value(std::string_view value_str, const T& expected) noexcept
{
	std::string value;
	value.reserve(value_str.length() + 6_sz);
	value.append("val = "sv);
	value.append(value_str);

	static constexpr auto is_val = [](char c) noexcept
	{
		if constexpr (std::is_same_v<string, value_of<T>>)
			return c == '"' || c == '\'';
		else
			return c != ' ' && c != '\t';
	};

	size_t begin = 5_sz;
	for (; begin < value.length(); begin++)
		if (is_val(value[begin]))
			break;
	begin++;
	size_t end = value.length();
	for (; end --> 0_sz; )
		if (is_val(value[end]))
			break;
	end += 2_sz;

	parsing_should_succeed(std::string_view{ value }, [&](table&& tbl) noexcept
	{
		CHECK(tbl.size() == 1);
		REQUIRE(tbl[S("val"sv)].as<value_of<T>>());
		CHECK(tbl[S("val"sv)].as<value_of<T>>()->get() == expected);
		CHECK(tbl[S("val"sv)].get()->region().begin == source_position{ 1, static_cast<uint32_t>(begin) });
		CHECK(tbl[S("val"sv)].get()->region().end == source_position{ 1, static_cast<uint32_t>(end) });
	});
}
