// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	template <typename... Args>
	parse_error make_error(const char* desc, Args&&... args)
	{
#if TOML_EXCEPTIONS
		return parse_error{ desc, std::forward<Args>(args)... };
#else
		return parse_error{ std::string{ desc }, std::forward<Args>(args)... };
#endif
	}

	std::optional<parse_error> parse_failure(std::string_view doc, std::string_view source_path)
	{
#if TOML_EXCEPTIONS
		try
		{
			static_cast<void>(toml::parse(doc, source_path));
		}
		catch (const parse_error& err)
		{
			return err;
		}
		return {};
#else
		auto result = toml::parse(doc, source_path);
		if (result)
			return {};
		return std::move(result).error();
#endif
	}

	std::string streamed(const parse_error& err)
	{
		std::ostringstream ss;
		ss << err;
		return std::move(ss).str();
	}
}

TEST_CASE("parse errors - construction")
{
	const source_region region{ { 3, 7 }, { 3, 9 }, std::make_shared<const std::string>("bad.toml") };

	SECTION("description and region")
	{
		const auto err = make_error("kaboom", source_region{ region });
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == region.begin);
		CHECK(err.source().end == region.end);
		CHECK(err.source().path == region.path);
	}

	SECTION("description and a copied region")
	{
		const auto err = make_error("kaboom", region);
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == region.begin);
		CHECK(err.source().end == region.end);
		CHECK(err.source().path == region.path);
	}

	SECTION("description and position")
	{
		const auto err = make_error("kaboom", source_position{ 3, 7 });
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == source_position{ 3, 7 });
		CHECK(err.source().end == source_position{ 3, 7 });
		CHECK(!err.source().path);
	}

	SECTION("description, position and path")
	{
		const auto err = make_error("kaboom", source_position{ 3, 7 }, region.path);
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == source_position{ 3, 7 });
		CHECK(err.source().end == source_position{ 3, 7 });
		CHECK(err.source().path == region.path);
	}
}

TEST_CASE("parse errors - description is null-terminated")
{
	const auto err	= make_error("kaboom", source_position{ 1, 1 });
	const auto desc = err.description();
	CHECK(desc == "kaboom"sv);
	CHECK(desc.data()[desc.length()] == '\0');
}

#if TOML_EXCEPTIONS

TEST_CASE("parse errors - are std::runtime_errors when exceptions are enabled")
{
	static_assert(std::is_base_of_v<std::runtime_error, parse_error>);

	const auto err = make_error("kaboom", source_position{ 1, 1 });
	CHECK(std::string_view{ err.what() } == "kaboom"sv);
}

#endif

TEST_CASE("parse errors - stream output")
{
	const auto path = std::make_shared<const std::string>("bad.toml");

	CHECK(streamed(make_error("kaboom", source_position{ 3, 7 })) == "kaboom\n\t(error occurred at line 3, column 7)");
	CHECK(streamed(make_error("kaboom", source_position{ 3, 7 }, path))
		  == "kaboom\n\t(error occurred at line 3, column 7 of 'bad.toml')");
	CHECK(streamed(make_error("kaboom", source_region{ { 3, 7 }, { 3, 9 }, path }))
		  == "kaboom\n\t(error occurred at line 3, column 7 to line 3, column 9 of 'bad.toml')");
}

TEST_CASE("parse errors - from the parser")
{
	const auto err = parse_failure("enabled = trUe"sv, "bad.toml"sv);
	REQUIRE(err);
	CHECK(err->description() == "Error while parsing boolean: expected 'true', saw 'trU'"sv);
	CHECK(err->source().begin == source_position{ 1, 13 });
	CHECK(err->source().end == source_position{ 1, 13 });
	REQUIRE(err->source().path);
	CHECK(*err->source().path == "bad.toml"sv);
	CHECK(streamed(*err)
		  == "Error while parsing boolean: expected 'true', saw 'trU'\n\t(error occurred at line 1, column 13 of "
			 "'bad.toml')");
}

TEST_CASE("parse errors - long descriptions are truncated")
{
	const std::string key(600u, 'k');
	const auto err = parse_failure(key + " = 1\n" + key + " = 2\n", "long.toml"sv);
	REQUIRE(err);
	const auto full = "Error while parsing key-value pair: cannot redefine existing integer '" + key + "'";
	const auto desc = err->description();
	CHECK(desc.length() < full.length());
	CHECK(std::string_view{ full }.substr(0u, desc.length()) == desc);
}

TEST_CASE("parse errors - positions after multi-byte code points")
{
	static constexpr auto saw_x =
		"1:9: Error while parsing key-value pair: expected a comment or whitespace, saw 'x'"sv;
	parsing_should_fail(FILE_LINE_ARGS, "a = '\xC3\xA9' x"sv, saw_x);
	parsing_should_fail(FILE_LINE_ARGS, "a = '\xE2\x82\xAC' x"sv, saw_x);
	parsing_should_fail(FILE_LINE_ARGS, "a = '\xF0\x9F\x98\x82' x"sv, saw_x);

	parsing_should_fail(FILE_LINE_ARGS,
						"'\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82' = 1 x"sv,
						"1:11: Error while parsing key-value pair: expected a comment or whitespace, saw 'x'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"# \xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\x01"sv,
		"1:6: Error while parsing comment: control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = \"\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\\q\""sv,
						R"(1:10: Error while parsing string: unknown escape sequence '\q')"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"a = \"\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\\uD800\""sv,
		"1:9: Error while parsing unicode scalar sequence: unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = '\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82'\nb = x"sv,
						"2:5: Error while parsing value: could not determine value type"sv);
}

TEST_CASE("parse errors - code points")
{
	SECTION("non-ASCII whitespace is escaped")
	{
		parsing_should_fail(FILE_LINE_ARGS,
							"a =\xE2\x80\xA8"
							"1"sv,
							R"(1:4: Error while parsing key-value pair: expected value, saw '\u2028')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 1\xC2\x85"sv,
			R"(1:6: Error while parsing key-value pair: expected a comment or whitespace, saw '\u0085')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a\xE2\x80\xA9 = 1"sv,
							R"(1:2: Error while parsing key-value pair: expected '=', saw '\u2029')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\r\xC2\xA0"sv,
							R"(1:7: Error while parsing key-value pair: expected '\n' after '\r', saw '\u00A0')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 0x\xE3\x80\x80"sv,
			R"(1:7: Error while parsing hexadecimal integer: expected hexadecimal digit, saw '\u3000')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 07:\xE2\x80\x80"sv,
							R"(1:8: Error while parsing time: expected 2-digit minute, saw '\u2000')"sv);
	}

	SECTION("C1 controls and default-ignorable code points are escaped")
	{
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 0x\xC2\x80"sv,
			R"(1:7: Error while parsing hexadecimal integer: expected hexadecimal digit, saw '\u0080')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\xC2\x9B"sv,
							R"(1:6: Error while parsing decimal integer: expected digit, saw '\u009B')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\xE2\x80\x8D"sv,
							R"(1:6: Error while parsing decimal integer: expected digit, saw '\u200D')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\xE2\x80\xAE"sv,
							R"(1:6: Error while parsing decimal integer: expected digit, saw '\u202E')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 0x\xC2\xAD"sv,
			R"(1:7: Error while parsing hexadecimal integer: expected hexadecimal digit, saw '\u00AD')"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\xF3\xA0\x80\x81"sv,
							R"(1:6: Error while parsing decimal integer: expected digit, saw '\U000E0001')"sv);
	}

	SECTION("other non-ASCII code points are printed raw")
	{
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1\r\xC3\xA9"sv,
							"1:7: Error while parsing key-value pair: expected '\\n' after '\\r', saw '\xC3\xA9'"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 0x\xF0\x9F\x98\x82"sv,
			"1:7: Error while parsing hexadecimal integer: expected hexadecimal digit, saw '\xF0\x9F\x98\x82'"sv);
	}
}

TEST_CASE("parse errors - positions after a byte order mark")
{
	parsing_should_fail(FILE_LINE_ARGS,
						BOM_PREFIX "a = 1 x"sv,
						"1:7: Error while parsing key-value pair: expected a comment or whitespace, saw 'x'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		BOM_PREFIX "= 1"sv,
		"1:1: Error while parsing root table: expected keys, tables, whitespace or comments, saw '='"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						BOM_PREFIX "a = 1\nb = x"sv,
						"2:5: Error while parsing value: could not determine value type"sv);
#if TOML_LANG_UNRELEASED
	parsing_should_succeed(FILE_LINE_ARGS,
						   BOM_PREFIX BOM_PREFIX "a = 1"sv,
						   [](table&& tbl) { CHECK(tbl[BOM_PREFIX "a"] == 1); });
#else
	parsing_should_fail(
		FILE_LINE_ARGS,
		BOM_PREFIX BOM_PREFIX "a = 1"sv,
		R"(1:1: Error while parsing root table: expected keys, tables, whitespace or comments, saw '\uFEFF')"sv);
#endif
	parsing_should_fail(FILE_LINE_ARGS,
						"\xEF\xBB"
						"a = 1"sv,
						"1:1: Encountered invalid utf-8 sequence"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"\xEF\xBB"sv,
						"1:1: Encountered EOF during incomplete utf-8 code point sequence"sv);
}

TEST_CASE("parse errors - carriage returns")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"\r"sv,
						R"(1:2: Error while parsing root table: expected '\n' after '\r', saw EOF)"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"\rx"sv,
						R"(1:2: Error while parsing root table: expected '\n' after '\r', saw 'x')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"\r\r\n"sv,
						R"(1:2: Error while parsing root table: expected '\n' after '\r', saw '\r')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = 1\r\nb = 2\rc = 3"sv,
						R"(2:7: Error while parsing key-value pair: expected '\n' after '\r', saw 'c')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = 1 \rx"sv,
						R"(1:8: Error while parsing key-value pair: expected '\n' after '\r', saw 'x')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"# x\ry"sv,
						R"(1:5: Error while parsing comment: expected '\n' after '\r', saw 'y')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"[a]\rx"sv,
						R"(1:5: Error while parsing table header: expected '\n' after '\r', saw 'x')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = [1,\r2]"sv,
						R"(1:9: Error while parsing array: expected '\n' after '\r', saw '2')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a\r = 1"sv,
						R"(1:2: Error while parsing key-value pair: expected '=', saw '\r')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a =\r\n1"sv,
						R"(1:4: Error while parsing key-value pair: expected value, saw '\r')"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"a = \"x\ry\""sv,
		"1:7: Error while parsing string: unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"a = 'x\ry'"sv,
		"1:7: Error while parsing literal string: control characters other than TAB (U+0009) are explicitly prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = \"\"\"x\ry\"\"\""sv,
						R"(1:10: Error while parsing string: expected '\n' after '\r', saw 'y')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = '''x\ry'''"sv,
						R"(1:10: Error while parsing literal string: expected '\n' after '\r', saw 'y')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = \"\"\"\rx\"\"\""sv,
						R"(1:9: Error while parsing string: expected '\n' after '\r', saw 'x')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = \"\"\"x\\\ry\"\"\""sv,
						R"(1:11: Error while parsing string: expected '\n' after '\r', saw 'y')"sv);
}

TEST_CASE("parse errors - at the end of the document")
{
	parsing_should_fail(FILE_LINE_ARGS, "a = ["sv, "1:6: Error while parsing array: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "a = [\n"sv, "1:7: Error while parsing array: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "a = [\r\n"sv, "1:8: Error while parsing array: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = [ # \xE2\x82\xAC"sv,
						"1:10: Error while parsing array: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = [ # \xF0\x9F\x98\x82\n"sv,
						"1:11: Error while parsing array: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = 1\n["sv,
						"2:2: Error while parsing table header: encountered end-of-file"sv);
}
