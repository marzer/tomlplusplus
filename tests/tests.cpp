// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include <optional>

bool parsing_should_succeed(std::string_view test_file,
							uint32_t test_line,
							std::string_view toml_str,
							pss_func&& func,
							std::string_view source_path)
{
	INFO("["sv << test_file << ", line "sv << test_line << "] "sv
			   << "parsing_should_succeed(\""sv << toml_str << "\")"sv);

	constexpr auto validate_table = [](table&& tabl, std::string_view path) -> table&&
	{
		INFO("Validating table source information"sv);
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

#if TOML_EXCEPTIONS

	try
	{
		{
			INFO("Parsing string directly"sv);
			if (func)
				func(validate_table(toml::parse(toml_str, source_path), source_path));
			else
				validate_table(toml::parse(toml_str, source_path), source_path);
		}
		{
			INFO("Parsing from a string stream"sv);
			std::stringstream ss;
			ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
			if (func)
				func(validate_table(toml::parse(ss, source_path), source_path));
			else
				validate_table(toml::parse(ss, source_path), source_path);
		}
	}
	catch (const parse_error& err)
	{
		FORCE_FAIL("Parse error on line "sv << err.source().begin.line << ", column "sv << err.source().begin.column
											<< ":\n"sv << err.description());
		return false;
	}

#else

	{
		INFO("Parsing string directly"sv);
		parse_result result = toml::parse(toml_str, source_path);
		if (result)
		{
			if (func)
				func(validate_table(std::move(result), source_path));
			else
				validate_table(std::move(result), source_path);
		}
		else
		{
			FORCE_FAIL("Parse error on line "sv << result.error().source().begin.line << ", column "sv
												<< result.error().source().begin.column << ":\n"sv
												<< result.error().description());
			return false;
		}
	}

	{
		INFO("Parsing from a string stream"sv);
		std::stringstream ss;
		ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
		parse_result result = toml::parse(ss, source_path);
		if (result)
		{
			if (func)
				func(validate_table(std::move(result), source_path));
			else
				validate_table(std::move(result), source_path);
		}
		else
		{
			FORCE_FAIL("Parse error on line "sv << result.error().source().begin.line << ", column "sv
												<< result.error().source().begin.column << ":\n"sv
												<< result.error().description());
			return false;
		}
	}

#endif

	return true;
}

namespace
{
	bool check_parse_error(const parse_error& err, std::optional<std::string_view> expected)
	{
		if (!expected)
		{
			SUCCEED();
			return true;
		}

		const auto actual = std::to_string(err.source().begin.line) + ":" + std::to_string(err.source().begin.column)
						  + ": " + std::string{ err.description() };
		CHECK(actual == *expected);
		return actual == *expected;
	}

	bool parsing_should_fail_impl(std::string_view test_file,
								  uint32_t test_line,
								  std::string_view toml_str,
								  std::optional<std::string_view> expected)
	{
		INFO("["sv << test_file << ", line "sv << test_line << "] "sv
				   << "parsing_should_fail(\""sv << toml_str << "\")"sv);

		const auto check = [&](std::string_view mode, auto&& parse) -> bool
		{
			INFO(mode);

#if TOML_EXCEPTIONS
			try
			{
				[[maybe_unused]] auto res = parse();
			}
			catch (const parse_error& err)
			{
				return check_parse_error(err, expected);
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
#else
			if (parse_result res = parse(); !res)
				return check_parse_error(res.error(), expected);
#endif

			FORCE_FAIL("Expected parsing failure"sv);
			return false;
		};

		const bool from_string = check("Parsing string directly"sv, [=]() { return toml::parse(toml_str); });
		const bool from_stream = check("Parsing from a string stream"sv,
									   [=]()
									   {
										   std::stringstream ss;
										   ss.write(toml_str.data(), static_cast<std::streamsize>(toml_str.length()));
										   return toml::parse(ss);
									   });
		return from_string && from_stream;
	}
}

bool parsing_should_fail(std::string_view test_file,
						 uint32_t test_line,
						 std::string_view toml_str,
						 std::string_view expected_error)
{
	return parsing_should_fail_impl(test_file, test_line, toml_str, expected_error);
}

bool parsing_should_fail(std::string_view test_file, uint32_t test_line, std::string_view toml_str)
{
	return parsing_should_fail_impl(test_file, test_line, toml_str, std::nullopt);
}

template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const int&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const unsigned int&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const bool&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const float&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const double&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const std::string_view&);

namespace Catch
{
	template struct StringMaker<node_view<node>>;
	template struct StringMaker<node_view<const node>>;
	template ReusableStringStream& ReusableStringStream::operator<<(node_view<node> const&);
	template ReusableStringStream& ReusableStringStream::operator<<(node_view<const node> const&);
	namespace Detail
	{
		template std::string stringify(const node_view<node>&);
		template std::string stringify(const node_view<const node>&);
	}
}
