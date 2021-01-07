// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
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
			// sanity-check initial state of a freshly-parsed table
			REQUIRE(tbl["test"].as<table>());
			CHECK(tbl["test"].as<table>()->size() == 1u);
			CHECK(tbl["test"].as<table>()->source().begin == source_position{ 1, 8 });
			CHECK(tbl["test"].as<table>()->source().end == source_position{ 1, 24 });
			CHECK(tbl["test"]["val1"] == "foo");

			// sanity-check initial state of default-constructed table
			table tbl2;
			CHECK(tbl2.source().begin == source_position{});
			CHECK(tbl2.source().end == source_position{});
			CHECK(!tbl2.source().path);
			CHECK(tbl2.size() == 0u);

			// check the results of move-assignment
			tbl2 = std::move(tbl);
			CHECK(tbl2.source().begin == source_position{ 1, 1 });
			CHECK(tbl2.source().end == source_position{ 1, 24 });
			CHECK(tbl2.source().path);
			CHECK(*tbl2.source().path == filename);
			CHECK(tbl2.size() == 1u);
			REQUIRE(tbl2["test"].as<table>());
			CHECK(tbl2["test"].as<table>()->size() == 1u);
			CHECK(tbl2["test"]["val1"] == "foo"sv);

			// check that moved-from table is now the same as default-constructed
			CHECK(tbl.source().begin == source_position{});
			CHECK(tbl.source().end == source_position{});
			CHECK(!tbl.source().path);
			CHECK(tbl.size() == 0u);
			CHECK(!tbl["test"].as<table>());

			// check the results of move-construction
			table tbl3{ std::move(tbl2) };
			CHECK(tbl3.source().begin == source_position{ 1, 1 });
			CHECK(tbl3.source().end == source_position{ 1, 24 });
			CHECK(tbl3.source().path);
			CHECK(*tbl3.source().path == filename);
			CHECK(tbl3.size() == 1u);
			REQUIRE(tbl3["test"].as<table>());
			CHECK(tbl3["test"].as<table>()->size() == 1u);
			CHECK(tbl3["test"]["val1"] == "foo"sv);

			// check that moved-from table is now the same as default-constructed
			CHECK(tbl2.source().begin == source_position{});
			CHECK(tbl2.source().end == source_position{});
			CHECK(!tbl2.source().path);
			CHECK(tbl2.size() == 0u);
			CHECK(!tbl2["test"].as<table>());
		},
		filename
	);
}

TEST_CASE("tables - copying")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(test = { val1 = "foo" })"sv,
		[&](table&& tbl)
		{
			// sanity-check initial state of a freshly-parsed table
			REQUIRE(tbl["test"].as<table>());
			CHECK(tbl["test"].as<table>()->size() == 1u);
			CHECK(tbl["test"].as<table>()->source().begin == source_position{ 1, 8 });
			CHECK(tbl["test"].as<table>()->source().end == source_position{ 1, 24 });
			CHECK(tbl["test"]["val1"] == "foo");

			// sanity-check initial state of default-constructed table
			table tbl2;
			CHECK(tbl2.source().begin == source_position{});
			CHECK(tbl2.source().end == source_position{});
			CHECK(!tbl2.source().path);
			CHECK(tbl2.size() == 0u);

			// check the results of copy-assignment
			tbl2 = tbl;
			CHECK(tbl2.source().begin == source_position{});
			CHECK(tbl2.source().end == source_position{});
			CHECK(!tbl2.source().path);
			CHECK(tbl2.size() == 1u);
			REQUIRE(tbl2["test"].as<table>());
			CHECK(tbl2["test"].as<table>()->size() == 1u);
			CHECK(tbl2["test"]["val1"] == "foo"sv);
			CHECK(tbl2 == tbl);

			// check the results of copy-construction
			table tbl3{ tbl2 };
			CHECK(tbl3.source().begin == source_position{});
			CHECK(tbl3.source().end == source_position{});
			CHECK(!tbl3.source().path);
			CHECK(tbl3.size() == 1u);
			REQUIRE(tbl3["test"].as<table>());
			CHECK(tbl3["test"].as<table>()->size() == 1u);
			CHECK(tbl3["test"]["val1"] == "foo"sv);
			CHECK(tbl3 == tbl2);
			CHECK(tbl3 == tbl);
		},
		filename
	);
}

TEST_CASE("tables - construction")
{
	{
		table tbl;
		CHECK(tbl.size() == 0u);
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
		CHECK(tbl.size() == 1u);
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
		CHECK(tbl.size() == 4u);
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
		CHECK(tbl.size() == 4u);
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
	CHECK(tbl.size() == 1u);
	CHECK(!tbl.empty());
	REQUIRE(tbl.get_as<int64_t>("a"sv));
	CHECK(*tbl.get_as<int64_t>("a"sv) == 42);
	REQUIRE(tbl == table{{ { "a"sv, 42 } }});

	res = tbl.insert("a", 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false);
	CHECK(tbl.size() == 1u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 42);
	REQUIRE(tbl == table{{ { "a"sv, 42 } }});

	static constexpr const char* a = "a";
	res = tbl.insert_or_assign(a, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false); // should assign
	CHECK(tbl.size() == 1u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{{ { "a"sv, 69 } }});

	res = tbl.insert_or_assign("b", "kek");
	CHECK(res.first == advance(tbl.begin(), 1));
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 2u);
	REQUIRE(tbl.get_as<std::string>("b"));
	CHECK(*tbl.get_as<std::string>("b") == "kek"sv);
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek" } }});

	res = tbl.emplace<array>("c", 1, 2, 3);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == true);
	CHECK(tbl.size() == 3u);
	REQUIRE(tbl.get_as<array>("c"));
	CHECK(*tbl.get_as<array>("c") == array{ 1, 2, 3 });
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek"sv }, { "c"sv, array{ 1, 2, 3 } } }});

	res = tbl.emplace<int64_t>("c", 1);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == false);
	CHECK(tbl.size() == 3u);
	REQUIRE(!tbl.get_as<int64_t>("c"));
	REQUIRE(tbl.get_as<array>("c"));
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek"s }, { "c"sv, array{ 1, 2, 3 } } }});

	auto it = tbl.erase(tbl.cbegin());
	REQUIRE(tbl == table{{ { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } }});
	CHECK(it == tbl.begin());
	CHECK(tbl.size() == 2u);

	res = tbl.insert_or_assign("a"sv, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 3u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{{ { "a"sv, 69 }, { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } }});

	it = tbl.erase(advance(tbl.cbegin(), 1), advance(tbl.cbegin(), 3));
	REQUIRE(tbl == table{{ { "a"sv, 69 } }});
	CHECK(it == tbl.end());
	CHECK(tbl.size() == 1u);

	tbl.clear();
	REQUIRE(tbl == table{});
	CHECK(tbl.size() == 0u);
	CHECK(tbl.empty());

	// void insert(Iter first, Iter last)
	{
		std::vector<std::pair<std::string, std::string>> vals{
			{ "foo", "foo" },
			{ "bar", "bar" },
			{ "kek", "kek" }
		};
		tbl.insert(vals.begin(), vals.end());
		CHECK(tbl.size() == 3u);
		REQUIRE(tbl.get_as<std::string>("foo"));
		CHECK(*tbl.get_as<std::string>("foo") == "foo");
		REQUIRE(tbl.get_as<std::string>("bar"));
		CHECK(*tbl.get_as<std::string>("bar") == "bar");
		REQUIRE(tbl.get_as<std::string>("kek"));
		CHECK(*tbl.get_as<std::string>("kek") == "kek");

		REQUIRE(vals.size() == 3u);
		CHECK(vals[0].first == "foo");
		CHECK(vals[0].second == "foo");
		CHECK(vals[1].first == "bar");
		CHECK(vals[1].second == "bar");
		CHECK(vals[2].first == "kek");
		CHECK(vals[2].second == "kek");

		tbl.clear();
	}

	// void insert(Iter first, Iter last) (with move iterators)
	{
		std::vector<std::pair<std::string, std::string>> vals{
			{ "foo", "foo" },
			{ "bar", "bar" },
			{ "kek", "kek" }
		};
		tbl.insert(std::make_move_iterator(vals.begin()), std::make_move_iterator(vals.end()));
		CHECK(tbl.size() == 3u);
		REQUIRE(tbl.get_as<std::string>("foo"));
		CHECK(*tbl.get_as<std::string>("foo") == "foo");
		REQUIRE(tbl.get_as<std::string>("bar"));
		CHECK(*tbl.get_as<std::string>("bar") == "bar");
		REQUIRE(tbl.get_as<std::string>("kek"));
		CHECK(*tbl.get_as<std::string>("kek") == "kek");

		REQUIRE(vals.size() == 3u);
		CHECK(vals[0].first == "");
		CHECK(vals[0].second == "");
		CHECK(vals[1].first == "");
		CHECK(vals[1].second == "");
		CHECK(vals[2].first == "");
		CHECK(vals[2].second == "");

		tbl.clear();
	}


	#if TOML_WINDOWS_COMPAT

	tbl.insert(L"a", L"test1");
	REQUIRE(*tbl.get_as<std::string>(L"a"sv) == "test1"sv);
	tbl.insert_or_assign(L"a"sv, L"test2");
	REQUIRE(*tbl.get_as<std::string>(L"a"sv) == "test2"sv);
	tbl.emplace<std::string>(L"b", L"test3");
	REQUIRE(*tbl.get_as<std::string>(L"b"sv) == "test3"sv);
	CHECK(tbl.size() == 2u);
	tbl.erase(L"b");
	CHECK(tbl.size() == 1u);
	tbl.erase(L"a"s);
	CHECK(tbl.size() == 0u);

	#endif // TOML_WINDOWS_COMPAT
}

TEST_CASE("tables - printing")
{
	static constexpr auto to_string = [](std::string_view some_toml)
	{
		auto val = toml::parse(some_toml);
		std::stringstream ss;
		ss << val;
		return ss.str();
	};

	{
		static constexpr auto some_toml = R"(val1 = 1
val2 = 2
val3 = 3)"sv;

		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = R"([a_table]
a = 1
b = 2
c = 3)"sv;

		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = R"(val1 = 1
val2 = 2
val3 = 3

[a_table]
a = 1
b = 2
c = 3)"sv;

		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = "[a]\n\n[b]\n\n[c]"sv;
		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = "[a]\nkey = 1\n\n[b]\n\n[c]"sv;
		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = "key = 1\n\n[a]\nkey = 1\n\n[b]\n\n[c]"sv;
		CHECK(to_string(some_toml) == some_toml);
	}

	{
		static constexpr auto some_toml = "key = 1\n\n[a]\nkey = 1\n\n[b]\n\n[[c]]\n\n[[c]]"sv;
		CHECK(to_string(some_toml) == some_toml);
	}
}

