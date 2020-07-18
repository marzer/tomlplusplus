// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

TEST_CASE("tables - moving")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(test = { val1 = "foo" })"sv,
		[&](table&& tbl)
		{
			CHECK(tbl.source().begin == source_position{ 1, 1 });
			CHECK(tbl.source().end == source_position{ 1, 24 });
			CHECK(tbl.source().path);
			CHECK(*tbl.source().path == filename);
			CHECK(tbl.size() == 1_sz);

			REQUIRE(tbl["test"].as<table>());
			CHECK(tbl["test"].as<table>()->size() == 1_sz);
			CHECK(tbl["test"].as<table>()->source().begin == source_position{ 1, 8 });
			CHECK(tbl["test"].as<table>()->source().end == source_position{ 1, 24 });
			CHECK(tbl["test"]["val1"] == "foo");

			table tbl2 = std::move(tbl);
			CHECK(tbl2.source().begin == source_position{ 1, 1 });
			CHECK(tbl2.source().end == source_position{ 1, 24 });
			CHECK(tbl2.source().path);
			CHECK(*tbl2.source().path == filename);
			CHECK(tbl2.size() == 1_sz);
			REQUIRE(tbl2["test"].as<table>());
			CHECK(tbl2["test"].as<table>()->size() == 1_sz);
			CHECK(tbl2["test"]["val1"] == "foo"sv);

			CHECK(tbl.source().begin == source_position{});
			CHECK(tbl.source().end == source_position{});
			CHECK(!tbl.source().path);
			CHECK(tbl.size() == 0_sz);
			CHECK(!tbl["test"].as<table>());
		},
		filename
	);
}

TEST_CASE("tables - construction")
{
	{
		table tbl;
		CHECK(tbl.size() == 0_sz);
		CHECK(tbl.empty());
		CHECK(tbl.begin() == tbl.end());
		CHECK(tbl.cbegin() == tbl.cend());
		CHECK(tbl.source().begin == source_position{});
		CHECK(tbl.source().end == source_position{});
		CHECK(!tbl.source().path);
	}

	{
		table tbl{{
			{ "foo"sv, 42 }
		}};
		CHECK(tbl.size() == 1_sz);
		CHECK(!tbl.empty());
		CHECK(tbl.begin() != tbl.end());
		CHECK(tbl.cbegin() != tbl.cend());
		REQUIRE(tbl.get_as<int64_t>("foo"sv));
		CHECK(*tbl.get_as<int64_t>("foo"sv) == 42);
	}

	{
		table tbl{{
			{ "foo"sv, 42 },
			{ "bar"sv, 10.0 },
			{ "kek"sv, false },
			{ "qux"sv, array{ 1 } }
		}};
		CHECK(tbl.size() == 4_sz);
		CHECK(!tbl.empty());
		REQUIRE(tbl.get_as<int64_t>("foo"sv));
		CHECK(*tbl.get_as<int64_t>("foo"sv) == 42);
		REQUIRE(tbl.get_as<double>("bar"sv));
		CHECK(*tbl.get_as<double>("bar"sv) == 10.0);
		REQUIRE(tbl.get_as<bool>("kek"sv));
		CHECK(*tbl.get_as<bool>("kek"sv) == false);
		REQUIRE(tbl.get_as<array>("qux"sv));
		CHECK(*tbl.get_as<array>("qux"sv) == array{ 1 });
	}

	#if TOML_WINDOWS_COMPAT
	{
		table tbl{ {
			{ L"foo", L"test1" },
			{ L"bar"sv, L"test2"sv },
			{ L"kek"s, L"test3"sv },
			{ L"qux"sv.data(), L"test4"sv.data() }
		} };
		CHECK(tbl.size() == 4_sz);
		CHECK(!tbl.empty());
		REQUIRE(tbl.get_as<std::string>("foo"sv));
		CHECK(*tbl.get_as<std::string>("foo"sv) == "test1"sv);
		REQUIRE(tbl.get_as<std::string>("bar"sv));
		CHECK(*tbl.get_as<std::string>("bar"sv) == "test2"sv);
		REQUIRE(tbl.get_as<std::string>("kek"sv));
		CHECK(*tbl.get_as<std::string>("kek"sv) == "test3"sv);
		REQUIRE(tbl.get_as<std::string>("qux"sv));
		CHECK(*tbl.get_as<std::string>("qux"sv) == "test4"sv);
	}
	#endif // TOML_WINDOWS_COMPAT
}

TEST_CASE("tables - equality")
{
	static constexpr const char* one = "one";

	table tbl1{{
		{ one, 1 },
		{ "two", 2 },
		{ "three", 3 }
	}};
	CHECK(tbl1 == tbl1);

	table tbl2{{
		{ "one"sv, 1 },
		{ "two"sv, 2 },
		{ "three"sv, 3 }
	}};
	CHECK(tbl1 == tbl2);

	table tbl3{{
		{ "one"sv, 1 },
		{ "two"sv, 2 }
	}};
	CHECK(tbl1 != tbl3);

	table tbl4{{
		{ "one"sv, 1 },
		{ "two"sv, 2 },
		{ "three"sv, 3 },
		{ "four"sv, 4 }
	}};
	CHECK(tbl1 != tbl4);

	table tbl5{{
		{ "one"sv, 1 },
		{ "two"sv, 2 },
		{ "three"sv, 3.0 }
	}};
	CHECK(tbl1 != tbl5);

	table tbl6{};
	CHECK(tbl1 != tbl6);
	CHECK(tbl6 == tbl6);

	table tbl7{};
	CHECK(tbl6 == tbl7);
}

namespace
{
	template <typename T>
	static auto advance(T iter, ptrdiff_t offset)
	{
		while (offset > 0)
		{
			iter++;
			offset--;
		}
		while (offset < 0)
		{
			iter--;
			offset++;
		}
		return iter;
	}
}

TEST_CASE("tables - insertion and erasure")
{
	table tbl;
	auto res = tbl.insert("a", 42);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true);
	CHECK(tbl.size() == 1_sz);
	CHECK(!tbl.empty());
	REQUIRE(tbl.get_as<int64_t>("a"sv));
	CHECK(*tbl.get_as<int64_t>("a"sv) == 42);
	REQUIRE(tbl == table{{ { "a"sv, 42 } }});

	res = tbl.insert("a", 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false);
	CHECK(tbl.size() == 1_sz);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 42);
	REQUIRE(tbl == table{{ { "a"sv, 42 } }});

	static constexpr const char* a = "a";
	res = tbl.insert_or_assign(a, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false); // should assign
	CHECK(tbl.size() == 1_sz);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{{ { "a"sv, 69 } }});

	res = tbl.insert_or_assign("b", "kek");
	CHECK(res.first == advance(tbl.begin(), 1));
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 2_sz);
	REQUIRE(tbl.get_as<std::string>("b"));
	CHECK(*tbl.get_as<std::string>("b") == "kek"sv);
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek" } }});

	res = tbl.emplace<array>("c", 1, 2, 3);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == true);
	CHECK(tbl.size() == 3_sz);
	REQUIRE(tbl.get_as<array>("c"));
	CHECK(*tbl.get_as<array>("c") == array{ 1, 2, 3 });
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek"sv }, { "c"sv, array{ 1, 2, 3 } } }});

	res = tbl.emplace<int64_t>("c", 1);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == false);
	CHECK(tbl.size() == 3_sz);
	REQUIRE(!tbl.get_as<int64_t>("c"));
	REQUIRE(tbl.get_as<array>("c"));
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek"s }, { "c"sv, array{ 1, 2, 3 } } }});

	auto it = tbl.erase(tbl.cbegin());
	REQUIRE(tbl == table{{ { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } }});
	CHECK(it == tbl.begin());
	CHECK(tbl.size() == 2_sz);

	res = tbl.insert_or_assign("a"sv, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 3_sz);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } }});

	it = tbl.erase(advance(tbl.cbegin(), 1), advance(tbl.cbegin(), 3));
	REQUIRE(tbl == table{{ { "a"sv, 69 } }});
	CHECK(it == tbl.end());
	CHECK(tbl.size() == 1_sz);

	tbl.clear();
	REQUIRE(tbl == table{});
	CHECK(tbl.size() == 0_sz);
	CHECK(tbl.empty());


	#if TOML_WINDOWS_COMPAT

	tbl.insert(L"a", L"test1");
	REQUIRE(*tbl.get_as<std::string>(L"a"sv) == "test1"sv);
	tbl.insert_or_assign(L"a"sv, L"test2");
	REQUIRE(*tbl.get_as<std::string>(L"a"sv) == "test2"sv);
	tbl.emplace<std::string>(L"b", L"test3");
	REQUIRE(*tbl.get_as<std::string>(L"b"sv) == "test3"sv);
	CHECK(tbl.size() == 2_sz);
	tbl.erase(L"b");
	CHECK(tbl.size() == 1_sz);
	tbl.erase(L"a"s);
	CHECK(tbl.size() == 0_sz);

	#endif // TOML_WINDOWS_COMPAT
}
