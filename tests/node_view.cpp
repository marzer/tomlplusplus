// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	constexpr date sample_date{ 2024, 1, 2 };
	constexpr toml::time sample_time{ 3, 4, 5 };
	constexpr date_time sample_date_time{ sample_date, sample_time };

	template <typename T>
	std::string printed(const T& obj)
	{
		std::ostringstream ss;
		ss << obj;
		return std::move(ss).str();
	}

	table make_sample_table()
	{
		return table{ { "str"sv, "hello"sv },
					  { "int"sv, 42 },
					  { "float"sv, 0.5 },
					  { "bool"sv, true },
					  { "date"sv, sample_date },
					  { "time"sv, sample_time },
					  { "date_time"sv, sample_date_time },
					  { "arr"sv, array{ 1, 2 } },
					  { "tbl"sv, table{ { "a"sv, 1 } } } };
	}

	template <typename View>
	void check_empty_view(View view)
	{
		using viewed_type = typename View::viewed_type;

		CHECK(!view);
		CHECK(view.node() == nullptr);
		CHECK(view.type() == node_type::none);
		CHECK(!view.is_table());
		CHECK(!view.is_array());
		CHECK(!view.is_array_of_tables());
		CHECK(!view.is_value());
		CHECK(!view.is_string());
		CHECK(!view.is_integer());
		CHECK(!view.is_floating_point());
		CHECK(!view.is_number());
		CHECK(!view.is_boolean());
		CHECK(!view.is_date());
		CHECK(!view.is_time());
		CHECK(!view.is_date_time());
		CHECK(!view.template is<int64_t>());
		CHECK(!view.template is<table>());

		CHECK(!view.is_homogeneous());
		CHECK(!view.template is_homogeneous<int64_t>());
		CHECK(!view.is_homogeneous(node_type::none));
		value<int64_t> unrelated{ 1 };
		viewed_type* nonmatch = &unrelated;
		CHECK(!view.is_homogeneous(node_type::none, nonmatch));
		CHECK(nonmatch == nullptr);

		CHECK(view.as_table() == nullptr);
		CHECK(view.as_array() == nullptr);
		CHECK(view.as_string() == nullptr);
		CHECK(view.as_integer() == nullptr);
		CHECK(view.as_floating_point() == nullptr);
		CHECK(view.as_boolean() == nullptr);
		CHECK(view.as_date() == nullptr);
		CHECK(view.as_time() == nullptr);
		CHECK(view.as_date_time() == nullptr);

		CHECK(!view.template value<int64_t>());
		CHECK(!view.template value<std::string_view>());
		CHECK(!view.template value_exact<int64_t>());
		CHECK(!view.template value_exact<std::string>());
		CHECK(view.value_or(7) == 7);

		CHECK(!view["key"sv]);
		CHECK(!view[0]);
		CHECK(!view[toml::path{ "a.b" }]);
		CHECK(!view.at_path("a.b"sv));
		CHECK(!view.at_path(toml::path{ "a.b" }));

		int calls			= {};
		const auto counter	= [&](auto&&) noexcept { calls++; };
		const auto returner = [](auto&&) noexcept { return 42; };
		view.visit(counter);
		CHECK(calls == 0);
		CHECK(view.visit(returner) == 0);

		CHECK(printed(view).empty());
	}

	template <typename View>
	void check_type_queries(View view, node_type expected)
	{
		INFO("node type: " << expected);
		REQUIRE(view);
		CHECK(view.type() == expected);
		CHECK(view.is_table() == (expected == node_type::table));
		CHECK(view.is_array() == (expected == node_type::array));
		CHECK(view.is_value() == (expected > node_type::array));
		CHECK(view.is_string() == (expected == node_type::string));
		CHECK(view.is_integer() == (expected == node_type::integer));
		CHECK(view.is_floating_point() == (expected == node_type::floating_point));
		CHECK(view.is_number() == (expected == node_type::integer || expected == node_type::floating_point));
		CHECK(view.is_boolean() == (expected == node_type::boolean));
		CHECK(view.is_date() == (expected == node_type::date));
		CHECK(view.is_time() == (expected == node_type::time));
		CHECK(view.is_date_time() == (expected == node_type::date_time));
		CHECK(!view.is_array_of_tables());

		const auto* n = view.node();
		CHECK((view.as_table() == n) == (expected == node_type::table));
		CHECK((view.as_array() == n) == (expected == node_type::array));
		CHECK((view.as_string() == n) == (expected == node_type::string));
		CHECK((view.as_integer() == n) == (expected == node_type::integer));
		CHECK((view.as_floating_point() == n) == (expected == node_type::floating_point));
		CHECK((view.as_boolean() == n) == (expected == node_type::boolean));
		CHECK((view.as_date() == n) == (expected == node_type::date));
		CHECK((view.as_time() == n) == (expected == node_type::time));
		CHECK((view.as_date_time() == n) == (expected == node_type::date_time));

		CHECK(view.template is<table>() == (expected == node_type::table));
		CHECK(view.template is<const value<int64_t>&>() == (expected == node_type::integer));
	}
}

TEST_CASE("node_view - empty views")
{
	check_empty_view(node_view<node>{});
	check_empty_view(node_view<const node>{});
	check_empty_view(node_view<node>{ static_cast<node*>(nullptr) });
	check_empty_view(node_view<const node>{ static_cast<const node*>(nullptr) });
}

TEST_CASE("node_view - construction, copying and assignment")
{
	value<int64_t> first{ 1 };
	value<int64_t> second{ 2 };

	static_assert(std::is_same_v<decltype(node_view{ first }), node_view<node>>);
	static_assert(std::is_same_v<decltype(node_view{ &first }), node_view<node>>);
	static_assert(std::is_same_v<decltype(node_view{ std::as_const(first) }), node_view<const node>>);
	static_assert(std::is_same_v<decltype(node_view{ &std::as_const(first) }), node_view<const node>>);

	node_view<node> view{ first };
	CHECK(view);
	CHECK(view.node() == &first);
	CHECK(node_view<node>{ &first }.node() == &first);
	CHECK(node_view<const node>{ std::as_const(first) }.node() == &first);

	auto copy = view;
	CHECK(copy.node() == &first);
	copy = node_view<node>{ second };
	CHECK(copy.node() == &second);
	CHECK(view.node() == &first);

	auto moved = std::move(copy);
	CHECK(moved.node() == &second);
	moved = node_view<node>{};
	CHECK(!moved);
}

TEST_CASE("node_view - type queries and casts")
{
	auto tbl		 = make_sample_table();
	const auto& ctbl = tbl;
	for (auto&& [key, val] : tbl)
	{
		INFO("key: " << key);
		check_type_queries(tbl[key], val.type());
		check_type_queries(ctbl[key], val.type());
	}
	check_type_queries(node_view<node>{ tbl }, node_type::table);
	check_type_queries(node_view<const node>{ ctbl }, node_type::table);

	SECTION("arrays of tables")
	{
		table aot{ { "aot"sv, array{ table{}, table{} } } };
		CHECK(aot["aot"].is_array_of_tables());
		CHECK(std::as_const(aot)["aot"].is_array_of_tables());
	}
}

TEST_CASE("node_view - is_homogeneous()")
{
	table tbl{ { "ints"sv, array{ 1, 2, 3 } }, { "mixed"sv, array{ 1, 2.0 } }, { "empty"sv, array{} } };
	const auto& ctbl = tbl;

	CHECK(tbl["ints"].is_homogeneous());
	CHECK(tbl["ints"].is_homogeneous<int64_t>());
	CHECK(!tbl["ints"].is_homogeneous<double>());
	CHECK(tbl["ints"].is_homogeneous(node_type::integer));
	CHECK(!tbl["mixed"].is_homogeneous());
	CHECK(tbl["mixed"].is_homogeneous(node_type::none) == false);
	CHECK(!tbl["empty"].is_homogeneous());

	node* nonmatch = nullptr;
	CHECK(tbl["ints"].is_homogeneous(node_type::integer, nonmatch));
	CHECK(nonmatch == nullptr);
	CHECK(!tbl["mixed"].is_homogeneous(node_type::integer, nonmatch));
	CHECK(nonmatch == tbl["mixed"][1].node());

	const node* const_nonmatch = nullptr;
	CHECK(ctbl["ints"].is_homogeneous(node_type::integer, const_nonmatch));
	CHECK(const_nonmatch == nullptr);
	CHECK(!ctbl["mixed"].is_homogeneous(node_type::integer, const_nonmatch));
	CHECK(const_nonmatch == ctbl["mixed"][1].node());
}

TEST_CASE("node_view - value retrieval")
{
	auto tbl		 = make_sample_table();
	const auto& ctbl = tbl;

	CHECK(tbl["int"].value<int64_t>() == 42);
	CHECK(tbl["int"].value<double>() == 42.0);
	CHECK(tbl["int"].value_exact<int64_t>() == 42);
	CHECK(!tbl["int"].value_exact<double>());
	CHECK(ctbl["float"].value<double>() == 0.5);
	CHECK(!ctbl["float"].value<int64_t>());
	CHECK(ctbl["str"].value_exact<std::string_view>() == "hello"sv);
	CHECK(!ctbl["str"].value<int64_t>());
	CHECK(tbl["date"].value<date>() == sample_date);
	CHECK(tbl["time"].value_exact<toml::time>() == sample_time);
	CHECK(tbl["date_time"].value_or(date_time{}) == sample_date_time);
	CHECK(tbl["missing"].value_or("fallback"sv) == "fallback"sv);
	CHECK(ctbl["missing"]["deeper"].value_or(1.5) == 1.5);

	CHECK(tbl["int"].ref<int64_t>() == 42);
	CHECK(&ctbl["str"].ref<std::string>() == &tbl["str"].as_string()->get());
	tbl["int"].ref<int64_t>() = 7;
	CHECK(tbl["int"] == 7);
}

TEST_CASE("node_view - subviews pass through missing nodes")
{
	table tbl{ { "a"sv, table{ { "b"sv, array{ 1, table{ { "c"sv, "d"sv } } } } } } };

	const auto check_chains = [](auto view)
	{
		CHECK(view["a"]["b"][1]["c"] == "d"sv);
		CHECK(view["a"]["b"][0] == 1);
		CHECK(!view["x"]["b"][1]["c"]);
		CHECK(!view["a"]["x"][1]["c"]);
		CHECK(!view["a"]["b"][5]["c"]);
		CHECK(!view["a"]["b"][0]["c"]);
		CHECK(!view["a"][0]);
		CHECK(!view["a"]["b"]["c"]);
		CHECK(!view["a"]["b"][0][0]);

		CHECK(view.at_path("a.b[1].c") == "d"sv);
		CHECK(view["a"].at_path("b[1].c") == "d"sv);
		CHECK(view["a"][toml::path{ "b[1].c" }] == "d"sv);
		CHECK(view["a"].at_path(toml::path{ "b[1].c" }) == "d"sv);
		CHECK(!view.at_path("a.b[2]"));
		CHECK(!view["missing"].at_path("b"));
	};
	check_chains(node_view<node>{ tbl });
	check_chains(node_view<const node>{ tbl });

	SECTION("mutable views can modify the viewed nodes")
	{
		node_view<node>{ tbl }["a"]["b"].as_array()->push_back(3);
		CHECK(tbl["a"]["b"][2] == 3);
	}
}

TEST_CASE("node_view - equality")
{
	auto tbl		 = make_sample_table();
	auto other		 = make_sample_table();
	const auto& ctbl = tbl;

	SECTION("views compare the viewed nodes")
	{
		for (auto&& [key, val] : tbl)
		{
			INFO("key: " << key);
			CHECK(tbl[key] == other[key]);
			CHECK(ctbl[key] == other[key]);
			CHECK(!(tbl[key] != other[key]));
			CHECK(tbl[key] != tbl["missing"]);
			CHECK(tbl["missing"] != tbl[key]);
			if (key != "int"sv)
				CHECK(tbl[key] != tbl["int"]);
		}
		CHECK(tbl["missing"] == ctbl["also missing"]);

		other.insert_or_assign("int", 43);
		CHECK(tbl["int"] != other["int"]);
		other.insert_or_assign("str", "goodbye"sv);
		CHECK(tbl["str"] != other["str"]);
	}

	SECTION("tables")
	{
		CHECK(tbl["tbl"] == table{ { "a"sv, 1 } });
		CHECK(table{ { "a"sv, 1 } } == tbl["tbl"]);
		CHECK(tbl["tbl"] != table{});
		CHECK(table{} != tbl["tbl"]);
		CHECK(tbl["int"] != table{});
	}

	SECTION("arrays")
	{
		CHECK(tbl["arr"] == array{ 1, 2 });
		CHECK(array{ 1, 2 } == tbl["arr"]);
		CHECK(tbl["arr"] != array{ 1 });
		CHECK(array{ 1 } != tbl["arr"]);
		CHECK(tbl["int"] != array{});

		CHECK(tbl["arr"] == std::initializer_list<int>{ 1, 2 });
		CHECK(std::initializer_list<int>{ 1, 2 } == tbl["arr"]);
		CHECK(tbl["arr"] != std::initializer_list<int>{ 2, 1 });
		CHECK(std::initializer_list<int>{ 2, 1 } != tbl["arr"]);
		CHECK(tbl["int"] != std::initializer_list<int>{ 42 });

		CHECK(tbl["arr"] == std::vector<int>{ 1, 2 });
		CHECK(std::vector<int>{ 1, 2 } == tbl["arr"]);
		CHECK(tbl["arr"] != std::vector<int>{ 1, 2, 3 });
		CHECK(std::vector<int>{ 1, 2, 3 } != tbl["arr"]);
		CHECK(tbl["missing"] != std::vector<int>{});
	}

	SECTION("value nodes")
	{
		CHECK(tbl["int"] == value<int64_t>{ 42 });
		CHECK(value<int64_t>{ 42 } == tbl["int"]);
		CHECK(tbl["int"] != value<int64_t>{ 43 });
		CHECK(value<int64_t>{ 43 } != tbl["int"]);
		CHECK(tbl["str"] != value<int64_t>{ 42 });
		CHECK(tbl["missing"] != value<int64_t>{ 42 });
	}

	SECTION("raw values")
	{
		CHECK(tbl["int"] == 42);
		CHECK(42 == tbl["int"]);
		CHECK(tbl["int"] != 43);
		CHECK(43 != tbl["int"]);
		CHECK(tbl["str"] != 42);
		CHECK(tbl["missing"] != 42);
		CHECK(tbl["str"] == "hello"sv);
		CHECK(tbl["str"] == "hello"s);
		CHECK(tbl["float"] == 0.5);
		CHECK(tbl["bool"] == true);
		CHECK(tbl["date"] == sample_date);
		CHECK(tbl["time"] == sample_time);
		CHECK(tbl["date_time"] == sample_date_time);
		CHECK(ctbl["date_time"] != date_time{ sample_date });
	}
}

TEST_CASE("node_view - documented example")
{
	auto res = toml::parse(R"(
		title = "my hardware store"

		[[products]]
		name = "Hammer"
		sku = 738594937
		keywords = [ "hammer", "construction", "build" ]

		[[products]]
		name = "Nail"
		sku = 284758393
		color = "gray"
	)"sv);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	table& tbl = res;

	CHECK(printed(tbl["title"]) == "'my hardware store'"sv);
	CHECK(printed(tbl["products"][0]["name"]) == "'Hammer'"sv);
	CHECK(printed(tbl["products"][0]["keywords"]) == "[ 'hammer', 'construction', 'build' ]"sv);
	CHECK(printed(tbl["products"][0]["keywords"][2]) == "'build'"sv);

	tbl["products"][0]["keywords"].as_array()->push_back("heavy");
	CHECK(printed(tbl["products"][0]["keywords"]) == "[ 'hammer', 'construction', 'build', 'heavy' ]"sv);
	CHECK(!tbl["products"][2]);
	CHECK(printed(tbl["products"][2]).empty());
}

TEST_CASE("node_view - printing writes the viewed node")
{
	auto tbl = make_sample_table();
	CHECK(printed(tbl["str"]) == "'hello'"sv);
	CHECK(printed(tbl["int"]) == "42"sv);
	CHECK(printed(tbl["arr"]) == "[ 1, 2 ]"sv);
	CHECK(printed(std::as_const(tbl)["date"]) == "2024-01-02"sv);
	CHECK(printed(tbl["missing"]).empty());
}
