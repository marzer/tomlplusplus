// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	template <typename T>
	T sample_value()
	{
		if constexpr (std::is_same_v<T, std::string>)
			return "kek";
		else if constexpr (std::is_same_v<T, int64_t>)
			return 42;
		else if constexpr (std::is_same_v<T, double>)
			return 4.5;
		else if constexpr (std::is_same_v<T, bool>)
			return true;
		else if constexpr (std::is_same_v<T, toml::date>)
			return toml::date{ 2024, 2, 29 };
		else if constexpr (std::is_same_v<T, toml::time>)
			return toml::time{ 13, 37, 42 };
		else
			return toml::date_time{ toml::date{ 2024, 2, 29 }, toml::time{ 13, 37, 42 }, toml::time_offset{ 2, 0 } };
	}

	array one_of_each_node_type()
	{
		return array{ table{ { "a", 1 } },
					  array{ 1, 2 },
					  sample_value<std::string>(),
					  sample_value<int64_t>(),
					  sample_value<double>(),
					  sample_value<bool>(),
					  sample_value<toml::date>(),
					  sample_value<toml::time>(),
					  sample_value<toml::date_time>() };
	}
}

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
			CHECK(tbl["test"].node() == tbl.get("test"sv));
			CHECK(tbl["test"].node() == &tbl.at("test"sv));

			// sanity-check initial state of a freshly-parsed table (const)
			const table& ctbl = tbl;
			REQUIRE(ctbl["test"].as<table>());
			CHECK(ctbl["test"].as<table>()->size() == 1u);
			CHECK(ctbl["test"].as<table>()->source().begin == source_position{ 1, 8 });
			CHECK(ctbl["test"].as<table>()->source().end == source_position{ 1, 24 });
			CHECK(ctbl["test"].node() == ctbl.get("test"sv));
			CHECK(ctbl["test"].node() == &ctbl.at("test"sv));

			// sanity check the virtual type checks
			CHECK(tbl.type() == node_type::table);
			CHECK(tbl.is_table());
			CHECK(!tbl.is_array());
			CHECK(!tbl.is_array_of_tables());
			CHECK(!tbl.is_value());
			CHECK(!tbl.is_string());
			CHECK(!tbl.is_integer());
			CHECK(!tbl.is_floating_point());
			CHECK(!tbl.is_number());
			CHECK(!tbl.is_boolean());
			CHECK(!tbl.is_date());
			CHECK(!tbl.is_time());
			CHECK(!tbl.is_date_time());

			// sanity check the virtual type casts (non-const)
			CHECK(tbl.as_table() == &tbl);
			CHECK(!tbl.as_array());
			CHECK(!tbl.as_string());
			CHECK(!tbl.as_integer());
			CHECK(!tbl.as_floating_point());
			CHECK(!tbl.as_boolean());
			CHECK(!tbl.as_date());
			CHECK(!tbl.as_time());
			CHECK(!tbl.as_date_time());

			// sanity check the virtual type casts (const)
			CHECK(ctbl.as_table() == &ctbl);
			CHECK(!ctbl.as_array());
			CHECK(!ctbl.as_string());
			CHECK(!ctbl.as_integer());
			CHECK(!ctbl.as_floating_point());
			CHECK(!ctbl.as_boolean());
			CHECK(!ctbl.as_date());
			CHECK(!ctbl.as_time());
			CHECK(!ctbl.as_date_time());

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
		filename);
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
		filename);
}

TEST_CASE("tables - assignment between a table and its descendants")
{
	const auto child = table{ { "b", 2 }, { "c", "three" } };

	table tbl{ { "a", 1 }, { "child", child } };
	tbl.get_as<table>("child")->is_inline(true);

	SECTION("copy from a descendant")
	{
		tbl = *tbl.get_as<table>("child");
		CHECK(tbl == child);
		CHECK(tbl.is_inline());
	}

	SECTION("move from a descendant")
	{
		tbl = std::move(*tbl.get_as<table>("child"));
		CHECK(tbl == child);
		CHECK(tbl.is_inline());
	}

	SECTION("copy from an ancestor")
	{
		const table original{ tbl };
		auto& descendant = *tbl.get_as<table>("child");
		descendant		 = tbl;
		CHECK(descendant == original);
		CHECK(!descendant.is_inline());
		CHECK(tbl == table{ { "a", 1 }, { "child", original } });
	}

	SECTION("self-assignment")
	{
		const table original{ tbl };
		table& self = tbl;
		tbl			= self;
		CHECK(tbl == original);
		tbl = std::move(self);
		CHECK(tbl == original);
	}
}

TEST_CASE("tables - construction")
{
	SECTION("default")
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

	SECTION("one key-value pair")
	{
		table tbl{ { "foo"sv, 42 } };
		CHECK(tbl.size() == 1u);
		CHECK(!tbl.empty());
		CHECK(tbl.begin() != tbl.end());
		CHECK(tbl.cbegin() != tbl.cend());
		REQUIRE(tbl.get_as<int64_t>("foo"sv));
		CHECK(*tbl.get_as<int64_t>("foo"sv) == 42);
	}

	SECTION("several key-value pairs")
	{
		table tbl{ { "foo"sv, 42 }, { "bar"sv, 10.0 }, { "kek"sv, false }, { "qux"sv, array{ 1 } } };
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

	SECTION("null node_views are skipped")
	{
		const table src{ { "x", 1 } };
		table tbl{ { "a", src["x"] }, { "b", src["missing"] }, { "c", 3 } };
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
		CHECK(src == table{ { "x", 1 } });
	}

	SECTION("only null node_views")
	{
		const table src;
		table tbl{ { "a", src["missing"] } };
		CHECK(tbl.empty());
	}

	SECTION("empty initialiser list")
	{
		table tbl({});
		CHECK(tbl.empty());
	}

#if TOML_ENABLE_WINDOWS_COMPAT
	SECTION("wide strings")
	{
		table tbl{ { L"foo", L"test1" },
				   { L"bar"sv, L"test2"sv },
				   { L"kek"s, L"test3"sv },
				   { L"qux"sv.data(), L"test4"sv.data() } };
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
#endif // TOML_ENABLE_WINDOWS_COMPAT
}

TEST_CASE("tables - equality")
{
	SECTION("size and value types")
	{
		static constexpr const char* one = "one";

		table tbl1{ { one, 1 }, { "two", 2 }, { "three", 3 } };
		CHECK(tbl1 == tbl1);

		table tbl2{ { "one"sv, 1 }, { "two"sv, 2 }, { "three"sv, 3 } };
		CHECK(tbl1 == tbl2);

		table tbl3{ { "one"sv, 1 }, { "two"sv, 2 } };
		CHECK(tbl1 != tbl3);

		table tbl4{ { "one"sv, 1 }, { "two"sv, 2 }, { "three"sv, 3 }, { "four"sv, 4 } };
		CHECK(tbl1 != tbl4);

		table tbl5{ { "one"sv, 1 }, { "two"sv, 2 }, { "three"sv, 3.0 } };
		CHECK(tbl1 != tbl5);

		table tbl6;
		CHECK(tbl1 != tbl6);
		CHECK(tbl6 == tbl6);

		table tbl7;
		CHECK(tbl6 == tbl7);
	}

	SECTION("same size, different keys")
	{
		const table lhs{ { "a", 1 }, { "b", 2 } };
		const table rhs{ { "a", 1 }, { "c", 2 } };
		CHECK_SYMMETRIC_INEQUAL(lhs, rhs);
	}

	SECTION("same keys and types, different values")
	{
		const table lhs{ { "a", 1 }, { "b", 2 } };
		const table rhs{ { "a", 1 }, { "b", 3 } };
		CHECK_SYMMETRIC_INEQUAL(lhs, rhs);
	}

	SECTION("nested containers are compared by value")
	{
		const table lhs{ { "a", table{ { "b", array{ 1, 2 } } } } };
		table rhs{ lhs };
		CHECK_SYMMETRIC_EQUAL(lhs, rhs);

		rhs["a"]["b"].as_array()->push_back(3);
		CHECK_SYMMETRIC_INEQUAL(lhs, rhs);
	}

	SECTION("inline tables equal non-inline tables")
	{
		const table lhs{ { "a", 1 } };
		table rhs{ { "a", 1 } };
		rhs.is_inline(true);
		CHECK_SYMMETRIC_EQUAL(lhs, rhs);
	}
}

TEST_CASE("tables - is_homogeneous")
{
	const auto samples					   = one_of_each_node_type();
	static constexpr node_type all_types[] = { node_type::none,		node_type::table,	node_type::array,
											   node_type::string,	node_type::integer, node_type::floating_point,
											   node_type::boolean,	node_type::date,	node_type::time,
											   node_type::date_time };

	SECTION("empty tables are never homogeneous")
	{
		table tbl;
		const table& ctbl = tbl;
		CHECK(!ctbl.is_homogeneous());
		CHECK(!ctbl.is_homogeneous<int64_t>());
		for (const auto type : all_types)
		{
			INFO("type: " << type);
			CHECK(!ctbl.is_homogeneous(type));

			node* nonmatch = &tbl;
			CHECK(!tbl.is_homogeneous(type, nonmatch));
			CHECK(nonmatch == nullptr);

			const node* cnonmatch = &tbl;
			CHECK(!ctbl.is_homogeneous(type, cnonmatch));
			CHECK(cnonmatch == nullptr);
		}
	}

	SECTION("values of one type")
	{
		for (const auto& sample : samples)
		{
			INFO("values of type " << sample.type());
			table tbl{ { "a", sample }, { "b", sample } };
			const table& ctbl = tbl;
			CHECK(ctbl.is_homogeneous());
			CHECK(ctbl.is_homogeneous(node_type::none));
			CHECK(ctbl.is_homogeneous(sample.type()));

			node* nonmatch{};
			CHECK(tbl.is_homogeneous(node_type::none, nonmatch));
			CHECK(tbl.is_homogeneous(sample.type(), nonmatch));

			const node* cnonmatch{};
			CHECK(ctbl.is_homogeneous(node_type::none, cnonmatch));
			CHECK(ctbl.is_homogeneous(sample.type(), cnonmatch));

			for (const auto type : all_types)
			{
				if (type == node_type::none || type == sample.type())
					continue;

				INFO("checked against " << type);
				CHECK(!ctbl.is_homogeneous(type));

				nonmatch = {};
				CHECK(!tbl.is_homogeneous(type, nonmatch));
				CHECK(nonmatch == tbl.get("a"));

				cnonmatch = {};
				CHECK(!ctbl.is_homogeneous(type, cnonmatch));
				CHECK(cnonmatch == tbl.get("a"));
			}
		}
	}

	SECTION("values of mixed types")
	{
		for (const auto& first : samples)
		{
			for (const auto& second : samples)
			{
				if (first.type() == second.type())
					continue;

				INFO("values of types " << first.type() << " and " << second.type());
				table tbl{ { "a", first }, { "b", second } };
				const table& ctbl = tbl;
				CHECK(!ctbl.is_homogeneous());

				for (const auto type : all_types)
				{
					INFO("checked against " << type);
					const node* expected_nonmatch =
						(type == node_type::none || type == first.type()) ? tbl.get("b") : tbl.get("a");

					CHECK(!ctbl.is_homogeneous(type));

					node* nonmatch{};
					CHECK(!tbl.is_homogeneous(type, nonmatch));
					CHECK(nonmatch == expected_nonmatch);

					const node* cnonmatch{};
					CHECK(!ctbl.is_homogeneous(type, cnonmatch));
					CHECK(cnonmatch == expected_nonmatch);
				}
			}
		}
	}

	SECTION("typed overloads")
	{
		const table ints{ { "a", 1 }, { "b", 2 } };
		CHECK(ints.is_homogeneous<int64_t>());
		CHECK(ints.is_homogeneous<toml::value<int64_t>>());
		CHECK(!ints.is_homogeneous<double>());
		CHECK(!ints.is_homogeneous<table>());

		const table tables{ { "a", table{} }, { "b", table{ { "c", 1 } } } };
		CHECK(tables.is_homogeneous<table>());
		CHECK(!tables.is_homogeneous<array>());
		CHECK(!tables.is_array_of_tables());
	}

	SECTION("through a node reference")
	{
		table tbl{ { "a", 1 }, { "b", "two" } };
		node& nd		= tbl;
		const node& cnd = tbl;

		node* nonmatch{};
		CHECK(!nd.is_homogeneous(node_type::integer, nonmatch));
		CHECK(nonmatch == tbl.get("b"));

		const node* cnonmatch{};
		CHECK(!cnd.is_homogeneous(node_type::string, cnonmatch));
		CHECK(cnonmatch == tbl.get("a"));

		CHECK(!cnd.is_homogeneous());
		CHECK(!cnd.is_homogeneous<int64_t>());
	}
}

TEST_CASE("tables - lookup")
{
	table tbl{ { "a", 1 }, { "c", "three" }, { "e", 5.0 } };
	const table& ctbl = tbl;

	SECTION("get")
	{
		REQUIRE(tbl.get("a"));
		CHECK(tbl.get("a")->value<int64_t>() == 1);
		CHECK(tbl.get("b") == nullptr);
		CHECK(tbl.get("") == nullptr);
		CHECK(ctbl.get("c") == tbl.get("c"));
		CHECK(ctbl.get("z") == nullptr);
	}

	SECTION("get_as")
	{
		REQUIRE(tbl.get_as<int64_t>("a"));
		CHECK(tbl.get_as<int64_t>("a")->get() == 1);
		CHECK(tbl.get_as<toml::value<int64_t>>("a") == tbl.get_as<int64_t>("a"));
		CHECK(tbl.get_as<double>("a") == nullptr);
		CHECK(tbl.get_as<table>("a") == nullptr);
		CHECK(tbl.get_as<int64_t>("b") == nullptr);
		CHECK(ctbl.get_as<std::string>("c") == tbl.get_as<std::string>("c"));
		CHECK(ctbl.get_as<std::string>("e") == nullptr);
	}

	SECTION("at")
	{
		CHECK(&tbl.at("a") == tbl.get("a"));
		CHECK(&ctbl.at("e") == tbl.get("e"));
#if TOML_COMPILER_HAS_EXCEPTIONS
		CHECK_THROWS_AS(tbl.at("b"), std::out_of_range);
		CHECK_THROWS_AS(ctbl.at("b"), std::out_of_range);
#endif
	}

	SECTION("contains")
	{
		CHECK(ctbl.contains("a"));
		CHECK(ctbl.contains("c"));
		CHECK(!ctbl.contains("b"));
		CHECK(!ctbl.contains(""));
		CHECK(!table{}.contains("a"));
	}

	SECTION("find")
	{
		const auto it = tbl.find("c");
		REQUIRE(it != tbl.end());
		CHECK(it->first == "c"sv);
		CHECK(&it->second == tbl.get("c"));
		CHECK(tbl.find("b") == tbl.end());

		const auto cit = ctbl.find("e");
		REQUIRE(cit != ctbl.cend());
		CHECK(cit->first == "e"sv);
		CHECK(&cit->second == tbl.get("e"));
		CHECK(ctbl.find("z") == ctbl.cend());
	}

	SECTION("lower_bound")
	{
		CHECK(tbl.lower_bound("") == tbl.begin());
		CHECK(tbl.lower_bound("a") == tbl.begin());
		CHECK(tbl.lower_bound("b")->first == "c"sv);
		CHECK(tbl.lower_bound("c")->first == "c"sv);
		CHECK(tbl.lower_bound("f") == tbl.end());

		CHECK(ctbl.lower_bound("a") == ctbl.cbegin());
		CHECK(ctbl.lower_bound("d")->first == "e"sv);
		CHECK(ctbl.lower_bound("e")->first == "e"sv);
		CHECK(ctbl.lower_bound("z") == ctbl.cend());
	}

	SECTION("operator[]")
	{
		CHECK(tbl["a"].node() == tbl.get("a"));
		CHECK(tbl["a"] == 1);
		CHECK(!tbl["b"]);
		CHECK(ctbl["c"].node() == tbl.get("c"));
		CHECK(ctbl["c"] == "three"sv);
		CHECK(!ctbl["d"]);
	}
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
	CHECK(res.first->first == "a"sv);
	CHECK(&res.first->second == tbl.get("a"));
	CHECK(res.second == true);
	CHECK(tbl.size() == 1u);
	CHECK(!tbl.empty());
	REQUIRE(tbl.get_as<int64_t>("a"sv));
	CHECK(*tbl.get_as<int64_t>("a"sv) == 42);
	REQUIRE(tbl == table{ { "a"sv, 42 } });

	res = tbl.insert("a", 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false);
	CHECK(tbl.size() == 1u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 42);
	REQUIRE(tbl == table{ { "a"sv, 42 } });

	static constexpr const char* a = "a";
	res							   = tbl.insert_or_assign(a, 69);
	CHECK(res.first == tbl.begin());
	CHECK(&res.first->second == tbl.get("a"));
	CHECK(res.second == false); // should assign
	CHECK(tbl.size() == 1u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{ { "a"sv, 69 } });

	res = tbl.insert_or_assign("b", "kek");
	CHECK(res.first == advance(tbl.begin(), 1));
	CHECK(res.first->first == "b"sv);
	CHECK(&res.first->second == tbl.get("b"));
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 2u);
	REQUIRE(tbl.get_as<std::string>("b"));
	CHECK(*tbl.get_as<std::string>("b") == "kek"sv);
	REQUIRE(tbl == table{ { "a"sv, 69 }, { "b"sv, "kek" } });

	res = tbl.emplace<array>("c", 1, 2, 3);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == true);
	CHECK(tbl.size() == 3u);
	REQUIRE(tbl.get_as<array>("c"));
	CHECK(*tbl.get_as<array>("c") == array{ 1, 2, 3 });
	REQUIRE(tbl == table{ { "a"sv, 69 }, { "b"sv, "kek"sv }, { "c"sv, array{ 1, 2, 3 } } });

	res = tbl.emplace<int64_t>("c", 1);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == false);
	CHECK(tbl.size() == 3u);
	REQUIRE(!tbl.get_as<int64_t>("c"));
	REQUIRE(tbl.get_as<array>("c"));
	REQUIRE(tbl == table{ { "a"sv, 69 }, { "b"sv, "kek"s }, { "c"sv, array{ 1, 2, 3 } } });

	auto it = tbl.erase(tbl.cbegin());
	REQUIRE(tbl == table{ { { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } } });
	CHECK(it == tbl.begin());
	CHECK(tbl.size() == 2u);

	res = tbl.insert_or_assign("a"sv, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true); // should insert
	CHECK(tbl.size() == 3u);
	REQUIRE(tbl.get_as<int64_t>("a"));
	CHECK(*tbl.get_as<int64_t>("a") == 69);
	REQUIRE(tbl == table{ { "a"sv, 69 }, { "b"sv, "kek" }, { "c"sv, array{ 1, 2, 3 } } });

	it = tbl.erase(advance(tbl.cbegin(), 1), advance(tbl.cbegin(), 3));
	REQUIRE(tbl == table{ { "a"sv, 69 } });
	CHECK(it == tbl.end());
	CHECK(tbl.size() == 1u);

	tbl.clear();
	REQUIRE(tbl == table{});
	CHECK(tbl.size() == 0u);
	CHECK(tbl.empty());
	CHECK(tbl.begin() == tbl.end());

	// void insert(Iter first, Iter last)
	{
		std::vector<std::pair<std::string, std::string>> vals{ { "foo", "foo" }, { "bar", "bar" }, { "kek", "kek" } };
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
		std::vector<std::pair<std::string, std::string>> vals{ { "foo", "foo" }, { "bar", "bar" }, { "kek", "kek" } };
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

#if TOML_ENABLE_WINDOWS_COMPAT

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

#endif // TOML_ENABLE_WINDOWS_COMPAT

	// insert with value_flags
	{
		tbl.clear();

		auto hex = toml::value{ 1 };
		hex.flags(value_flags::format_as_hexadecimal);
		CHECK(hex.flags() == value_flags::format_as_hexadecimal);

		tbl.insert("hex", hex);
		CHECK(hex.flags() == value_flags::format_as_hexadecimal);
		CHECK(tbl["hex"].as_integer()->flags() == value_flags::format_as_hexadecimal);

		tbl.insert("hex2", std::move(hex));
		CHECK(hex.flags() == value_flags{});
		CHECK(tbl["hex2"].as_integer()->flags() == value_flags::format_as_hexadecimal);
	}
}

TEST_CASE("tables - insert")
{
	table tbl{ { "b", 2 } };

	SECTION("keys are kept in order")
	{
		tbl.insert("c", 3);
		tbl.insert("a", 1);
		std::vector<std::string> keys;
		for (auto&& [k, v] : tbl)
			keys.emplace_back(k.str());
		CHECK(keys == std::vector<std::string>{ "a", "b", "c" });
	}

	SECTION("keys of any string type")
	{
		const toml::key k1{ "k1" };
		const std::string k2 = "k2";
		const char* k6		 = "k6";
		tbl.clear();
		tbl.insert(k1, 1);
		tbl.insert(k2, 2);
		tbl.insert(std::string{ "k3" }, 3);
		tbl.insert("k4"sv, 4);
		tbl.insert("k5", 5);
		tbl.insert(k6, 6);
		tbl.insert(toml::key{ "k7" }, 7);
		CHECK(tbl
			  == table{ { "k1", 1 }, { "k2", 2 }, { "k3", 3 }, { "k4", 4 }, { "k5", 5 }, { "k6", 6 }, { "k7", 7 } });
		CHECK(k1 == "k1"sv);
		CHECK(k2 == "k2"sv);
	}

	SECTION("values convertible to native types")
	{
		tbl.clear();
		tbl.insert("float", 1.5f);
		tbl.insert("int", 7);
		tbl.insert("unsigned", 8u);
		tbl.insert("c string", "str");
		tbl.insert("string_view", "sv"sv);
		CHECK(tbl["float"].is_floating_point());
		CHECK(tbl["float"] == 1.5);
		CHECK(tbl["int"].is_integer());
		CHECK(tbl["int"] == 7);
		CHECK(tbl["unsigned"].is_integer());
		CHECK(tbl["unsigned"] == 8);
		CHECK(tbl["c string"].is_string());
		CHECK(tbl["c string"] == "str"sv);
		CHECK(tbl["string_view"].is_string());
		CHECK(tbl["string_view"] == "sv"sv);
	}

	SECTION("nodes and containers")
	{
		const table child{ { "x", 1 } };
		const node& child_node = child;
		tbl.insert("node", child_node);
		tbl.insert("table", child);
		tbl.insert("array", array{ 1, 2 });
		CHECK(tbl["node"] == child);
		CHECK(tbl["table"] == child);
		CHECK(tbl["array"] == array{ 1, 2 });
		CHECK(child == table{ { "x", 1 } });

		table moved_from{ { "y", 2 } };
		tbl.insert("moved", std::move(moved_from));
		CHECK(tbl["moved"] == table{ { "y", 2 } });
	}

	SECTION("node_views")
	{
		const table src{ { "x", "ex" } };
		const auto [it, inserted] = tbl.insert("view", src["x"]);
		CHECK(inserted);
		CHECK(it->first == "view"sv);
		CHECK(tbl["view"] == "ex"sv);
		CHECK(src["x"] == "ex"sv);

		const auto [null_it, null_inserted] = tbl.insert("null", src["missing"]);
		CHECK(!null_inserted);
		CHECK(null_it == tbl.end());
		CHECK(!tbl.contains("null"));
	}

	SECTION("value flags")
	{
		tbl.insert("hex", 255, value_flags::format_as_hexadecimal);
		CHECK(tbl["hex"].as_integer()->flags() == value_flags::format_as_hexadecimal);
	}
}

TEST_CASE("tables - inserting a range")
{
	table tbl{ { "a", 1 } };

	SECTION("an empty range")
	{
		const std::vector<std::pair<std::string, int>> kvps;
		tbl.insert(kvps.begin(), kvps.end());
		CHECK(tbl == table{ { "a", 1 } });
	}

	SECTION("existing keys are not replaced")
	{
		const std::vector<std::pair<std::string, int>> kvps{ { "b", 2 }, { "a", 10 } };
		tbl.insert(kvps.begin(), kvps.end());
		CHECK(tbl == table{ { "a", 1 }, { "b", 2 } });
	}

	SECTION("from another table")
	{
		const table src{ { "b", 2 }, { "c", table{ { "d", 4 } } } };
		tbl.insert(src.begin(), src.end());
		CHECK(tbl == table{ { "a", 1 }, { "b", 2 }, { "c", table{ { "d", 4 } } } });
		CHECK(src == table{ { "b", 2 }, { "c", table{ { "d", 4 } } } });
	}

	SECTION("value flags")
	{
		const std::vector<std::pair<std::string, int>> kvps{ { "b", 2 }, { "c", 3 } };
		tbl.insert(kvps.begin(), kvps.end(), value_flags::format_as_octal);
		CHECK(tbl["a"].as_integer()->flags() == value_flags::none);
		CHECK(tbl["b"].as_integer()->flags() == value_flags::format_as_octal);
		CHECK(tbl["c"].as_integer()->flags() == value_flags::format_as_octal);
	}
}

TEST_CASE("tables - insert_or_assign")
{
	table tbl{ { "a", 1 } };

	SECTION("node_views")
	{
		const table src{ { "x", 2.5 } };

		auto res = tbl.insert_or_assign("a", src["x"]);
		CHECK(!res.second);
		CHECK(tbl["a"] == 2.5);

		res = tbl.insert_or_assign("b", src["x"]);
		CHECK(res.second);
		CHECK(tbl["b"] == 2.5);

		res = tbl.insert_or_assign("a", src["missing"]);
		CHECK(!res.second);
		CHECK(res.first == tbl.end());
		CHECK(tbl["a"] == 2.5);

		res = tbl.insert_or_assign("c", src["missing"]);
		CHECK(!res.second);
		CHECK(res.first == tbl.end());
		CHECK(!tbl.contains("c"));
	}

	SECTION("a descendant of the value being replaced")
	{
		tbl.insert_or_assign("a", table{ { "b", table{ { "c", 3 } } } });
		tbl.insert_or_assign("a", *tbl["a"]["b"].as_table());
		CHECK(tbl == table{ { "a", table{ { "c", 3 } } } });
	}

	SECTION("value flags")
	{
		tbl.insert_or_assign("a", 2, value_flags::format_as_binary);
		tbl.insert_or_assign("b", 3, value_flags::format_as_hexadecimal);
		CHECK(tbl["a"] == 2);
		CHECK(tbl["a"].as_integer()->flags() == value_flags::format_as_binary);
		CHECK(tbl["b"] == 3);
		CHECK(tbl["b"].as_integer()->flags() == value_flags::format_as_hexadecimal);
	}
}

TEST_CASE("tables - emplace")
{
	table tbl{ { "a", 1 } };

	SECTION("constructs a value at a new key")
	{
		const auto [it, emplaced] = tbl.emplace<std::string>("d", "this is not a drill"sv, size_t{ 14 }, size_t{ 5 });
		CHECK(emplaced);
		CHECK(it->first == "d"sv);
		CHECK(tbl["d"] == "drill"sv);
	}

	SECTION("deduces the value type")
	{
		tbl.emplace("b", 2.5);
		tbl.emplace("c", "three"sv);
		CHECK(tbl["b"].is_floating_point());
		CHECK(tbl["b"] == 2.5);
		CHECK(tbl["c"].is_string());
		CHECK(tbl["c"] == "three"sv);
	}
}

TEST_CASE("tables - emplace_hint")
{
	table tbl{ { "a", 1 }, { "c", 3 } };

	SECTION("with an accurate hint")
	{
		const auto it = tbl.emplace_hint<int64_t>(tbl.lower_bound("b"), "b", 2);
		CHECK(it->first == "b"sv);
		CHECK(&it->second == tbl.get("b"));
		CHECK(tbl == table{ { "a", 1 }, { "b", 2 }, { "c", 3 } });
	}

	SECTION("with an inaccurate hint")
	{
		const auto it = tbl.emplace_hint<int64_t>(tbl.cbegin(), "d", 4);
		CHECK(it->first == "d"sv);
		CHECK(std::next(it) == tbl.end());
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 }, { "d", 4 } });
	}

	SECTION("does nothing for an existing key")
	{
		const auto it = tbl.emplace_hint<std::string>(tbl.cend(), "c", "three"sv);
		CHECK(it->first == "c"sv);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
	}

	SECTION("deduces the value type")
	{
		const auto it = tbl.emplace_hint(tbl.cend(), "s", "str"sv);
		CHECK(it->first == "s"sv);
		CHECK(tbl["s"].is_string());
		CHECK(tbl["s"] == "str"sv);
	}

#if TOML_COMPILER_HAS_EXCEPTIONS
	SECTION("leaves the table unchanged if the value constructor throws")
	{
		CHECK_THROWS_AS(tbl.emplace_hint<std::string>(tbl.cend(), "s", "abc"sv, size_t{ 10 }, size_t{ 1 }),
						std::out_of_range);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
	}
#endif
}

TEST_CASE("tables - erasure")
{
	table tbl{ { "a", 1 }, { "b", 2 }, { "c", 3 } };

	SECTION("by iterator")
	{
		const auto it = tbl.erase(std::next(tbl.begin()));
		REQUIRE(it != tbl.end());
		CHECK(it->first == "c"sv);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
	}

	SECTION("by const iterator")
	{
		const auto it = tbl.erase(std::next(tbl.cbegin()));
		REQUIRE(it != tbl.end());
		CHECK(it->first == "c"sv);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
	}

	SECTION("the last key-value pair")
	{
		const auto it = tbl.erase(std::prev(tbl.end()));
		CHECK(it == tbl.end());
		CHECK(tbl == table{ { "a", 1 }, { "b", 2 } });
	}

	SECTION("by range")
	{
		const auto it = tbl.erase(tbl.cbegin(), std::prev(tbl.cend()));
		REQUIRE(it != tbl.end());
		CHECK(it->first == "c"sv);
		CHECK(tbl == table{ { "c", 3 } });
	}

	SECTION("an empty range")
	{
		const auto it = tbl.erase(tbl.cbegin(), tbl.cbegin());
		CHECK(it == tbl.begin());
		CHECK(tbl == table{ { "a", 1 }, { "b", 2 }, { "c", 3 } });
	}

	SECTION("by key")
	{
		CHECK(tbl.erase("b") == 1u);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
		CHECK(tbl.erase("b") == 0u);
		CHECK(tbl.erase("z") == 0u);
		CHECK(tbl == table{ { "a", 1 }, { "c", 3 } });
		CHECK(table{}.erase("a") == 0u);
	}
}

TEST_CASE("tables - pruning")
{
	const table tbl{ { "a", 1 },
					 { "b", array{} },
					 { "c", array{ table{}, array{} } },
					 { "d", table{} },
					 { "e", table{ { "f", table{} } } },
					 { "g", table{ { "h", 2 }, { "i", array{} } } } };

	const table pruned_recursive{ { "a", 1 }, { "g", table{ { "h", 2 } } } };

	const table pruned_flat{ { "a", 1 },
							 { "c", array{ table{}, array{} } },
							 { "e", table{ { "f", table{} } } },
							 { "g", table{ { "h", 2 }, { "i", array{} } } } };

	SECTION("recursive")
	{
		table pruned{ tbl };
		CHECK(&pruned.prune() == &pruned);
		CHECK(pruned == pruned_recursive);
	}

	SECTION("non-recursive")
	{
		table pruned{ tbl };
		CHECK(&pruned.prune(false) == &pruned);
		CHECK(pruned == pruned_flat);
	}

	SECTION("rvalue")
	{
		const table pruned = table{ tbl }.prune();
		CHECK(pruned == pruned_recursive);

		const table pruned2 = table{ tbl }.prune(false);
		CHECK(pruned2 == pruned_flat);
	}

	SECTION("an empty table")
	{
		table empty;
		CHECK(empty.prune().empty());
	}
}

TEMPLATE_TEST_CASE("tables - native values",
				   "",
				   std::string,
				   int64_t,
				   double,
				   bool,
				   toml::date,
				   toml::time,
				   toml::date_time)
{
	const TestType val = sample_value<TestType>();
	const TestType other{};
	REQUIRE(val != other);

	SECTION("insert")
	{
		table tbl;
		TestType copy = val;
		CHECK(tbl.insert("lvalue", copy).second);
		CHECK(copy == val);
		CHECK(tbl.insert("rvalue", std::move(copy)).second);
		CHECK(tbl.insert("value", toml::value<TestType>{ val }).second);
		CHECK(!tbl.insert("lvalue", other).second);
		CHECK(tbl.size() == 3u);
		for (auto&& [k, v] : tbl)
		{
			INFO("key: " << k);
			CHECK(v.template is<TestType>());
			CHECK(v.template value<TestType>() == val);
		}
	}

	SECTION("insert_or_assign")
	{
		table tbl{ { "a", other } };
		CHECK(!tbl.insert_or_assign("a", val).second);
		CHECK(tbl["a"] == val);
		CHECK(tbl.insert_or_assign("b", toml::value<TestType>{ val }).second);
		CHECK(tbl["b"] == val);
	}

	SECTION("emplace")
	{
		table tbl;
		CHECK(tbl.emplace<TestType>("a", val).second);
		CHECK(tbl.emplace("b", val).second);
		CHECK(!tbl.emplace<TestType>("a", other).second);
		CHECK(tbl["a"] == val);
		CHECK(tbl["b"] == val);
		CHECK(tbl.get_as<TestType>("b"));
	}

	SECTION("emplace_hint")
	{
		table tbl;
		const auto it = tbl.emplace_hint<TestType>(tbl.cend(), "a", val);
		CHECK(&it->second == tbl.get("a"));
		CHECK(tbl["a"] == val);
	}

	SECTION("lookup")
	{
		table tbl{ { "a", val } };
		const table& ctbl = tbl;
		REQUIRE(tbl.get_as<TestType>("a"));
		CHECK(tbl.get_as<TestType>("a")->get() == val);
		REQUIRE(ctbl.get_as<TestType>("a"));
		CHECK(ctbl.get_as<TestType>("a")->get() == val);
		CHECK(ctbl.at("a").value<TestType>() == val);
		CHECK(ctbl.find("a")->second.is<TestType>());
		CHECK(ctbl["a"] == val);
	}

	SECTION("equality")
	{
		const table lhs{ { "a", val } };
		const table same{ { "a", val } };
		const table different{ { "a", other } };
		CHECK_SYMMETRIC_EQUAL(lhs, same);
		CHECK_SYMMETRIC_INEQUAL(lhs, different);
	}

	SECTION("is_homogeneous")
	{
		const table tbl{ { "a", val }, { "b", other } };
		CHECK(tbl.is_homogeneous<TestType>());
		CHECK(tbl.is_homogeneous(tbl.get("a")->type()));
		CHECK(!tbl.is_homogeneous<table>());
	}
}

TEST_CASE("tables - toml_formatter")
{
	static constexpr auto to_string = [](std::string_view some_toml,
										 format_flags flags			= toml_formatter::default_flags,
										 format_flags exclude_flags = format_flags::none)
	{
		auto val = toml::parse(some_toml);
		std::stringstream ss;
		ss << toml_formatter{ val, flags & ~(exclude_flags) };
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

	{
		constexpr auto input = R"(key1 = 'val1'
key2 = [ 1, 2, 3, 4, '5' ]
key3 = [ 'this is a really long array', 'and should be split over multiple lines', 'by the formatter', 'unless i dun goofed', 'i guess thats what tests are for' ]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

    [sub2.sub3]
    key6 = 'val'
    key7 = [ 1, 2, 3, 4, '5' ]
    key8 = [ 'this is a really long array', 'and should be split over multiple lines', 'by the formatter', 'unless i dun goofed', 'i guess thats what tests are for' ])"sv;

		constexpr auto expected_default = R"(key1 = 'val1'
key2 = [ 1, 2, 3, 4, '5' ]
key3 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

    [sub2.sub3]
    key6 = 'val'
    key7 = [ 1, 2, 3, 4, '5' ]
    key8 = [
        'this is a really long array',
        'and should be split over multiple lines',
        'by the formatter',
        'unless i dun goofed',
        'i guess thats what tests are for'
    ])"sv;
		CHECK(to_string(input) == expected_default);

		constexpr auto expected_without_indented_subtables = R"(key1 = 'val1'
key2 = [ 1, 2, 3, 4, '5' ]
key3 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

[sub2.sub3]
key6 = 'val'
key7 = [ 1, 2, 3, 4, '5' ]
key8 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
])"sv;
		CHECK(to_string(input, toml_formatter::default_flags, format_flags::indent_sub_tables)
			  == expected_without_indented_subtables);

		constexpr auto expected_without_indented_arrays = R"(key1 = 'val1'
key2 = [ 1, 2, 3, 4, '5' ]
key3 = [
'this is a really long array',
'and should be split over multiple lines',
'by the formatter',
'unless i dun goofed',
'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

    [sub2.sub3]
    key6 = 'val'
    key7 = [ 1, 2, 3, 4, '5' ]
    key8 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
    ])"sv;
		CHECK(to_string(input, toml_formatter::default_flags, format_flags::indent_array_elements)
			  == expected_without_indented_arrays);

		constexpr auto expected_without_indentation = R"(key1 = 'val1'
key2 = [ 1, 2, 3, 4, '5' ]
key3 = [
'this is a really long array',
'and should be split over multiple lines',
'by the formatter',
'unless i dun goofed',
'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

[sub2.sub3]
key6 = 'val'
key7 = [ 1, 2, 3, 4, '5' ]
key8 = [
'this is a really long array',
'and should be split over multiple lines',
'by the formatter',
'unless i dun goofed',
'i guess thats what tests are for'
])"sv;
		CHECK(to_string(input, toml_formatter::default_flags, format_flags::indentation)
			  == expected_without_indentation);

		// forcing multiline arrays: even short arrays become one-per-line (with array elements indented)
		constexpr auto expected_forced_multiline = R"(key1 = 'val1'
key2 = [
    1,
    2,
    3,
    4,
    '5'
]
key3 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

    [sub2.sub3]
    key6 = 'val'
    key7 = [
        1,
        2,
        3,
        4,
        '5'
    ]
    key8 = [
        'this is a really long array',
        'and should be split over multiple lines',
        'by the formatter',
        'unless i dun goofed',
        'i guess thats what tests are for'
    ])"sv;

		CHECK(to_string(input, toml_formatter::default_flags | format_flags::force_multiline_arrays)
			  == expected_forced_multiline);

		// forcing multiline arrays without indenting array elements
		constexpr auto expected_forced_without_indented_arrays = R"(key1 = 'val1'
key2 = [
1,
2,
3,
4,
'5'
]
key3 = [
'this is a really long array',
'and should be split over multiple lines',
'by the formatter',
'unless i dun goofed',
'i guess thats what tests are for'
]

[sub1]
key4 = 'val'

[sub2]
key5 = 'val'

    [sub2.sub3]
    key6 = 'val'
    key7 = [
    1,
    2,
    3,
    4,
    '5'
    ]
    key8 = [
    'this is a really long array',
    'and should be split over multiple lines',
    'by the formatter',
    'unless i dun goofed',
    'i guess thats what tests are for'
    ])"sv;

		CHECK(to_string(input,
						toml_formatter::default_flags | format_flags::force_multiline_arrays,
						format_flags::indent_array_elements)
			  == expected_forced_without_indented_arrays);
	}
}

TEST_CASE("tables - for_each")
{
	const auto tbl = table{ { "a", 1 }, { "b", 2.0 }, { "c", 3 }, { "d", "four" }, { "e", false } };

	SECTION("type checking")
	{
		int count	= 0;
		int ints	= 0;
		int floats	= 0;
		int numbers = 0;
		int strings = 0;
		int bools	= 0;
		tbl.for_each(
			[&](const auto& v) noexcept
			{
				count++;
				if constexpr (toml::is_integer<decltype(v)>)
					ints++;
				if constexpr (toml::is_floating_point<decltype(v)>)
					floats++;
				if constexpr (toml::is_number<decltype(v)>)
					numbers++;
				if constexpr (toml::is_string<decltype(v)>)
					strings++;
				if constexpr (toml::is_boolean<decltype(v)>)
					bools++;
			});
		CHECK(count == 5);
		CHECK(ints == 2);
		CHECK(floats == 1);
		CHECK(numbers == (ints + floats));
		CHECK(strings == 1);
		CHECK(bools == 1);
	}

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

	SECTION("early-exit (key, val)")
	{
		int count = 0;
		tbl.for_each([&](const key& /*k*/, const auto& /*v*/) noexcept -> bool { return ++count < 3; });
		CHECK(count == 3);
	}

	SECTION("early-exit (val)")
	{
		int count = 0;
		tbl.for_each([&](const auto& /*v*/) noexcept -> bool { return ++count < 3; });
		CHECK(count == 3);
	}

#endif
}
