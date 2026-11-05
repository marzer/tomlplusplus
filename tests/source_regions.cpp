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

	const auto check_node = [](const node* n, source_position begin, source_position end)
	{
		REQUIRE(n);
		CHECK(n->source().begin == begin);
		CHECK(n->source().end == end);
	};

	const auto check_key = [](const table& parent, std::string_view name, source_position begin, source_position end)
	{
		const auto it = parent.find(name);
		REQUIRE(it != parent.end());
		CHECK(it->first.source().begin == begin);
		CHECK(it->first.source().end == end);
	};

	SECTION("the root table spans the document")
	{
		check_node(&static_cast<const table&>(tbl), { 1, 1 }, { 17, 8 });
	}

	SECTION("values and inline containers span their text")
	{
		check_key(tbl, "a", { 1, 1 }, { 1, 2 });
		check_node(tbl["a"].node(), { 1, 5 }, { 1, 6 });
		check_node(tbl["b"].node(), { 2, 5 }, { 2, 10 });
		check_node(tbl["c"].node(), { 3, 5 }, { 3, 20 });
		check_node(tbl["c"][0].node(), { 3, 7 }, { 3, 8 });
		check_node(tbl["c"][1].node(), { 3, 10 }, { 3, 18 });
		check_node(tbl["c"][1][1].node(), { 3, 15 }, { 3, 16 });
		check_node(tbl["d"].node(), { 4, 5 }, { 4, 29 });
		check_key(*tbl["d"].as_table(), "f", { 4, 14 }, { 4, 15 });
		check_node(tbl["d"]["f"].node(), { 4, 18 }, { 4, 27 });
		check_node(tbl["d"]["f"]["g"].node(), { 4, 24 }, { 4, 25 });
	}

	SECTION("tables made by dotted keys span the key that made them")
	{
		check_node(tbl["h"].node(), { 5, 1 }, { 5, 2 });
		check_node(tbl["h"]["i"].node(), { 5, 3 }, { 5, 4 });
		check_key(*tbl["h"]["i"].as_table(), "j", { 5, 5 }, { 5, 6 });
		check_node(tbl["h"]["i"]["j"].node(), { 5, 9 }, { 5, 10 });
	}

	SECTION("tables made by headers span the header")
	{
		check_key(tbl, "table", { 7, 2 }, { 7, 7 });
		check_node(tbl["table"].node(), { 7, 1 }, { 7, 8 });
		check_node(tbl["table"]["k"].node(), { 8, 5 }, { 8, 6 });
		check_key(*tbl["table"].as_table(), "sub", { 10, 8 }, { 10, 11 });
		check_node(tbl["table"]["sub"].node(), { 10, 1 }, { 10, 12 });
	}

	SECTION("arrays of tables span their headers")
	{
		check_key(tbl, "aot", { 13, 3 }, { 13, 6 });
		check_node(tbl["aot"].node(), { 13, 1 }, { 16, 8 });
		check_node(tbl["aot"][0].node(), { 13, 1 }, { 13, 8 });
		check_node(tbl["aot"][1].node(), { 16, 1 }, { 16, 8 });
		check_node(tbl["aot"][1]["n"].node(), { 17, 5 }, { 17, 7 });
	}
}
