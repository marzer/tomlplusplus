// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	void check_ordering(source_position lhs, source_position rhs, int expected)
	{
		INFO(lhs << " vs " << rhs);
		CHECK((lhs == rhs) == (expected == 0));
		CHECK((lhs != rhs) == (expected != 0));
		CHECK((lhs < rhs) == (expected < 0));
		CHECK((lhs <= rhs) == (expected <= 0));
		CHECK((lhs > rhs) == (expected > 0));
		CHECK((lhs >= rhs) == (expected >= 0));
		CHECK((rhs == lhs) == (expected == 0));
		CHECK((rhs != lhs) == (expected != 0));
		CHECK((rhs < lhs) == (expected > 0));
		CHECK((rhs <= lhs) == (expected >= 0));
		CHECK((rhs > lhs) == (expected < 0));
		CHECK((rhs >= lhs) == (expected <= 0));
	}

	template <typename T>
	std::string streamed(const T& val)
	{
		std::ostringstream ss;
		ss << val;
		return std::move(ss).str();
	}

	constexpr auto max_index = std::numeric_limits<source_index>::max();

	void check_region(const node* n, source_position begin, source_position end)
	{
		REQUIRE(n);
		CHECK(n->source().begin == begin);
		CHECK(n->source().end == end);
	}

	void check_key_region(const table& parent, std::string_view name, source_position begin, source_position end)
	{
		INFO(name);
		const auto it = parent.find(name);
		REQUIRE(it != parent.end());
		CHECK(it->first.source().begin == begin);
		CHECK(it->first.source().end == end);
	}
}

TEST_CASE("source positions - validity")
{
	static_assert(!source_position{ 0, 0 });
	static_assert(source_position{ 1, 1 });

	const auto [pos, valid] = GENERATE(Catch::Generators::table<source_position, bool>({
		{ source_position{ 0, 0 }, false },
		{ source_position{ 1, 0 }, false },
		{ source_position{ 0, 1 }, false },
		{ source_position{ 1, 1 }, true },
		{ source_position{ max_index, max_index }, true },
	}));

	CHECK(static_cast<bool>(pos) == valid);
}

TEST_CASE("source positions - comparison")
{
	static_assert(source_position{ 3, 7 } == source_position{ 3, 7 });
	static_assert(source_position{ 3, 7 } < source_position{ 3, 8 });

	SECTION("equal")
	{
		check_ordering({ 1, 1 }, { 1, 1 }, 0);
		check_ordering({ max_index, max_index }, { max_index, max_index }, 0);
	}

	SECTION("column")
	{
		check_ordering({ 3, 7 }, { 3, 8 }, -1);
		check_ordering({ 3, max_index }, { 3, 1 }, 1);
	}

	SECTION("line")
	{
		check_ordering({ 3, 7 }, { 4, 7 }, -1);
		check_ordering({ max_index, 1 }, { 1, 1 }, 1);
	}

	SECTION("line outranks column")
	{
		check_ordering({ 1, max_index }, { 2, 1 }, -1);
		check_ordering({ max_index, 1 }, { max_index - 1u, max_index }, 1);
	}
}

TEST_CASE("source positions - stream output")
{
	CHECK(streamed(source_position{ 3, 7 }) == "line 3, column 7");
	CHECK(streamed(source_position{ 1, 1 }) == "line 1, column 1");
	CHECK(streamed(source_position{ max_index, max_index }) == "line 4294967295, column 4294967295");
}

TEST_CASE("source regions - stream output")
{
	const auto path = std::make_shared<const std::string>("config.toml");

	SECTION("a single position")
	{
		CHECK(streamed(source_region{ { 1, 7 }, { 1, 7 }, nullptr }) == "line 1, column 7");
		CHECK(streamed(source_region{ { 1, 7 }, { 1, 7 }, path }) == "line 1, column 7 of 'config.toml'");
	}

	SECTION("a range")
	{
		CHECK(streamed(source_region{ { 1, 7 }, { 1, 9 }, nullptr }) == "line 1, column 7 to line 1, column 9");
		CHECK(streamed(source_region{ { 1, 7 }, { 3, 2 }, nullptr }) == "line 1, column 7 to line 3, column 2");
		CHECK(streamed(source_region{ { 1, 7 }, { 1, 9 }, path })
			  == "line 1, column 7 to line 1, column 9 of 'config.toml'");
	}
}

TEST_CASE("source regions - from parsing")
{
	SECTION("with a source path")
	{
		auto tbl = toml::parse("bar = 42"sv, "config.toml"sv);
		REQUIRE(tbl["bar"]);

		const auto& src = tbl["bar"].node()->source();
		CHECK(src.begin == source_position{ 1, 7 });
		CHECK(src.end == source_position{ 1, 9 });
		REQUIRE(src.path);
		CHECK(*src.path == "config.toml"sv);
		CHECK(streamed(src) == "line 1, column 7 to line 1, column 9 of 'config.toml'");
	}

	SECTION("without a source path")
	{
		auto tbl = toml::parse("bar = 42"sv);
		REQUIRE(tbl["bar"]);

		const auto& src = tbl["bar"].node()->source();
		CHECK(!src.path);
		CHECK(streamed(src) == "line 1, column 7 to line 1, column 9");
	}
}

TEST_CASE("source regions - every kind of node")
{
	auto tbl = toml::parse(R"(a = 1
b = "two"
c = [ 1, [ 2, 3 ] ]
d = { e = 4, f = { g = 5 } }
h.i.j = 6

[table]
k = 7

[table.sub]
l = 8

[[aot]]
m = 9

[[aot]]
n = 10
)"sv);

	SECTION("the root table spans the document")
	{
		check_region(&static_cast<const table&>(tbl), { 1, 1 }, { 17, 8 });
	}

	SECTION("values and inline containers span their text")
	{
		check_key_region(tbl, "a", { 1, 1 }, { 1, 2 });
		check_region(tbl["a"].node(), { 1, 5 }, { 1, 6 });
		check_region(tbl["b"].node(), { 2, 5 }, { 2, 10 });
		check_region(tbl["c"].node(), { 3, 5 }, { 3, 20 });
		check_region(tbl["c"][0].node(), { 3, 7 }, { 3, 8 });
		check_region(tbl["c"][1].node(), { 3, 10 }, { 3, 18 });
		check_region(tbl["c"][1][1].node(), { 3, 15 }, { 3, 16 });
		check_region(tbl["d"].node(), { 4, 5 }, { 4, 29 });
		check_key_region(*tbl["d"].as_table(), "f", { 4, 14 }, { 4, 15 });
		check_region(tbl["d"]["f"].node(), { 4, 18 }, { 4, 27 });
		check_region(tbl["d"]["f"]["g"].node(), { 4, 24 }, { 4, 25 });
	}

	SECTION("tables made by dotted keys span the key that made them")
	{
		check_region(tbl["h"].node(), { 5, 1 }, { 5, 2 });
		check_region(tbl["h"]["i"].node(), { 5, 3 }, { 5, 4 });
		check_key_region(*tbl["h"]["i"].as_table(), "j", { 5, 5 }, { 5, 6 });
		check_region(tbl["h"]["i"]["j"].node(), { 5, 9 }, { 5, 10 });
	}

	SECTION("tables made by headers span the header")
	{
		check_key_region(tbl, "table", { 7, 2 }, { 7, 7 });
		check_region(tbl["table"].node(), { 7, 1 }, { 7, 8 });
		check_region(tbl["table"]["k"].node(), { 8, 5 }, { 8, 6 });
		check_key_region(*tbl["table"].as_table(), "sub", { 10, 8 }, { 10, 11 });
		check_region(tbl["table"]["sub"].node(), { 10, 1 }, { 10, 12 });
	}

	SECTION("arrays of tables span their headers")
	{
		check_key_region(tbl, "aot", { 13, 3 }, { 13, 6 });
		check_region(tbl["aot"].node(), { 13, 1 }, { 16, 8 });
		check_region(tbl["aot"][0].node(), { 13, 1 }, { 13, 8 });
		check_region(tbl["aot"][1].node(), { 16, 1 }, { 16, 8 });
		check_region(tbl["aot"][1]["n"].node(), { 17, 5 }, { 17, 7 });
	}
}

TEST_CASE("source regions - after multi-byte code points")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   "'\xC3\xA9' = '\xE2\x82\xAC'\n"
						   "'\xF0\x9F\x98\x82' = ['\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82', 1]\n"
						   "t = { '\xE2\x82\xAC' = 1, b = 2 }"sv,
						   [](table&& tbl)
						   {
							   check_region(&tbl, { 1, 1 }, { 3, 23 });

							   check_key_region(tbl, "\xC3\xA9"sv, { 1, 1 }, { 1, 4 });
							   check_region(tbl["\xC3\xA9"].node(), { 1, 7 }, { 1, 10 });

							   check_key_region(tbl, "\xF0\x9F\x98\x82"sv, { 2, 1 }, { 2, 4 });
							   check_region(tbl["\xF0\x9F\x98\x82"].node(), { 2, 7 }, { 2, 17 });
							   check_region(tbl["\xF0\x9F\x98\x82"][0].node(), { 2, 8 }, { 2, 13 });
							   check_region(tbl["\xF0\x9F\x98\x82"][1].node(), { 2, 15 }, { 2, 16 });

							   REQUIRE(tbl["t"].as_table());
							   check_region(tbl["t"].node(), { 3, 5 }, { 3, 23 });
							   check_key_region(*tbl["t"].as_table(), "\xE2\x82\xAC"sv, { 3, 7 }, { 3, 10 });
							   check_region(tbl["t"]["\xE2\x82\xAC"].node(), { 3, 13 }, { 3, 14 });
							   check_key_region(*tbl["t"].as_table(), "b"sv, { 3, 16 }, { 3, 17 });
							   check_region(tbl["t"]["b"].node(), { 3, 20 }, { 3, 21 });
						   });
}

TEST_CASE("source regions - after a byte order mark")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   BOM_PREFIX "'\xC3\xA9' = 1\nb = '" BOM_PREFIX "'"sv,
						   [](table&& tbl)
						   {
							   check_region(&tbl, { 1, 1 }, { 2, 8 });
							   check_key_region(tbl, "\xC3\xA9"sv, { 1, 1 }, { 1, 4 });
							   check_region(tbl["\xC3\xA9"].node(), { 1, 7 }, { 1, 8 });
							   check_region(tbl["b"].node(), { 2, 5 }, { 2, 8 });
							   CHECK(tbl["b"] == BOM_PREFIX ""sv);
						   });

	parsing_should_succeed(FILE_LINE_ARGS,
						   BOM_PREFIX ""sv,
						   [](table&& tbl)
						   {
							   check_region(&tbl, { 1, 1 }, { 1, 1 });
							   CHECK(tbl.empty());
						   });
}

TEST_CASE("source regions - on CRLF lines")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   "a = 1\r\n"
						   "b = 'two'\r\n"
						   "\r\n"
						   "[t]\r\n"
						   "c = [\r\n"
						   "  3,\r\n"
						   "]\r\n"
						   "d = \"\"\"\r\n"
						   "x\r\n"
						   "y\"\"\"\r\n"sv,
						   [](table&& tbl)
						   {
							   check_region(&tbl, { 1, 1 }, { 10, 7 });
							   check_region(tbl["a"].node(), { 1, 5 }, { 1, 6 });
							   check_region(tbl["b"].node(), { 2, 5 }, { 2, 10 });
							   check_key_region(tbl, "t"sv, { 4, 2 }, { 4, 3 });
							   check_region(tbl["t"].node(), { 4, 1 }, { 4, 4 });
							   check_region(tbl["t"]["c"].node(), { 5, 5 }, { 7, 2 });
							   check_region(tbl["t"]["c"][0].node(), { 6, 3 }, { 6, 4 });
							   check_region(tbl["t"]["d"].node(), { 8, 5 }, { 10, 5 });
							   CHECK(tbl["t"]["d"] == "x\ny"sv);
						   });
}

TEST_CASE("source regions - at the end of the document")
{
	const auto check = [](std::string_view doc,
						  source_position root_end,
						  source_position first_begin = {},
						  source_position first_end	  = {})
	{
		INFO(doc);
		parsing_should_succeed(FILE_LINE_ARGS,
							   doc,
							   [&](table&& tbl)
							   {
								   check_region(&tbl, { 1, 1 }, root_end);
								   if (!first_begin)
									   return;
								   REQUIRE(!tbl.empty());
								   check_region(&tbl.begin()->second, first_begin, first_end);
							   });
	};

	check(""sv, { 1, 1 });
	check("\n"sv, { 1, 2 });
	check("a = 1"sv, { 1, 6 }, { 1, 5 }, { 1, 6 });
	check("a = 1\n"sv, { 1, 7 }, { 1, 5 }, { 1, 6 });
	check("a = 1\r\n"sv, { 1, 8 }, { 1, 5 }, { 1, 6 });
	check("a = 1 # c"sv, { 1, 10 }, { 1, 5 }, { 1, 6 });
	check("a = 1 # c\n"sv, { 1, 11 }, { 1, 5 }, { 1, 6 });
	check("[t]"sv, { 1, 4 }, { 1, 1 }, { 1, 4 });
	check("[t]\n"sv, { 1, 5 }, { 1, 1 }, { 1, 4 });
	check("[[t]]"sv, { 1, 6 }, { 1, 1 }, { 1, 6 });
	check("[[t]]\n"sv, { 1, 7 }, { 1, 1 }, { 1, 6 });
	check("a = [1,\n2]"sv, { 2, 3 }, { 1, 5 }, { 2, 3 });
	check("a = [1,\n2]\n"sv, { 2, 4 }, { 1, 5 }, { 2, 3 });
	check("a = '''x\ny'''"sv, { 2, 5 }, { 1, 5 }, { 2, 5 });
	check("a = '''x\ny'''\n"sv, { 2, 6 }, { 1, 5 }, { 2, 5 });
}
