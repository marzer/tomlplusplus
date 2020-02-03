#include "tests.h"

TEST_CASE("tables - moving")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		S(R"(test = { val1 = "foo" })"sv),
		[&](table&& tbl) noexcept
		{
			CHECK(tbl.source().begin == source_position{ 1, 1 });
			CHECK(tbl.source().end == source_position{ 1, 24 });
			CHECK(tbl.source().path);
			CHECK(*tbl.source().path == filename);
			CHECK(tbl.size() == 1_sz);

			REQUIRE(tbl[S("test")].as<table>());
			CHECK(tbl[S("test")].as<table>()->size() == 1_sz);
			CHECK(tbl[S("test")].as<table>()->source().begin == source_position{ 1, 8 });
			CHECK(tbl[S("test")].as<table>()->source().end == source_position{ 1, 24 });
			CHECK(tbl[S("test")][S("val1")] == S("foo"sv));

			table tbl2 = std::move(tbl);
			CHECK(tbl2.source().begin == source_position{ 1, 1 });
			CHECK(tbl2.source().end == source_position{ 1, 24 });
			CHECK(tbl2.source().path);
			CHECK(*tbl2.source().path == filename);
			CHECK(tbl2.size() == 1_sz);
			REQUIRE(tbl2[S("test")].as<table>());
			CHECK(tbl2[S("test")].as<table>()->size() == 1_sz);
			CHECK(tbl2[S("test")][S("val1")] == S("foo"sv));

			CHECK(tbl.source().begin == source_position{});
			CHECK(tbl.source().end == source_position{});
			CHECK(!tbl.source().path);
			CHECK(tbl.size() == 0_sz);
			CHECK(!tbl[S("test")].as<table>());
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
			{ S("foo"sv), 42 }
		}};
		CHECK(tbl.size() == 1_sz);
		CHECK(!tbl.empty());
		CHECK(tbl.begin() != tbl.end());
		CHECK(tbl.cbegin() != tbl.cend());
		REQUIRE(tbl.get_as<int64_t>(S("foo"sv)));
		CHECK(*tbl.get_as<int64_t>(S("foo"sv)) == 42);
	}

	{
		table tbl{{
			{ S("foo"sv), 42 },
			{ S("bar"sv), 10.0 },
			{ S("kek"sv), false },
			{ S("qux"sv), array{ 1 } }
		}};
		CHECK(tbl.size() == 4_sz);
		CHECK(!tbl.empty());
		REQUIRE(tbl.get_as<int64_t>(S("foo"sv)));
		CHECK(*tbl.get_as<int64_t>(S("foo"sv)) == 42);
		REQUIRE(tbl.get_as<double>(S("bar"sv)));
		CHECK(*tbl.get_as<double>(S("bar"sv)) == 10.0);
		REQUIRE(tbl.get_as<bool>(S("kek"sv)));
		CHECK(*tbl.get_as<bool>(S("kek"sv)) == false);
		REQUIRE(tbl.get_as<array>(S("qux"sv)));
		CHECK(*tbl.get_as<array>(S("qux"sv)) == array{ 1 });
	}
}

TEST_CASE("tables - equality")
{
	static constexpr const string_char* one = S("one");

	table tbl1{{
		{ one, 1 },
		{ S("two"), 2 },
		{ S("three"), 3 }
	}};
	CHECK(tbl1 == tbl1);

	table tbl2{{
		{ S("one"sv), 1 },
		{ S("two"sv), 2 },
		{ S("three"sv), 3 }
	}};
	CHECK(tbl1 == tbl2);

	table tbl3{{
		{ S("one"sv), 1 },
		{ S("two"sv), 2 }
	}};
	CHECK(tbl1 != tbl3);

	table tbl4{{
		{ S("one"sv), 1 },
		{ S("two"sv), 2 },
		{ S("three"sv), 3 },
		{ S("four"sv), 4 }
	}};
	CHECK(tbl1 != tbl4);

	table tbl5{{
		{ S("one"sv), 1 },
		{ S("two"sv), 2 },
		{ S("three"sv), 3.0 }
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
	static auto advance(T iter, ptrdiff_t offset) noexcept
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
	auto res = tbl.insert(S("a"), 42);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true);
	CHECK(tbl.size() == 1_sz);
	CHECK(!tbl.empty());
	REQUIRE(tbl.get_as<int64_t>(S("a"sv)));
	CHECK(*tbl.get_as<int64_t>(S("a"sv)) == 42);
	REQUIRE(tbl == table{{ { S("a"sv), 42 } }});

	res = tbl.insert(S("a"), 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false);
	CHECK(tbl.size() == 1_sz);
	REQUIRE(tbl.get_as<int64_t>(S("a")));
	CHECK(*tbl.get_as<int64_t>(S("a")) == 42);
	REQUIRE(tbl == table{{ { S("a"sv), 42 } }});

	static constexpr const string_char* a = S("a");
	res = tbl.insert_or_assign(a, 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == false); //should assign
	CHECK(tbl.size() == 1_sz);
	REQUIRE(tbl.get_as<int64_t>(S("a")));
	CHECK(*tbl.get_as<int64_t>(S("a")) == 69);
	REQUIRE(tbl == table{{ { S("a"sv), 69 } }});

	res = tbl.insert_or_assign(S("b"), S("kek"));
	CHECK(res.first == advance(tbl.begin(), 1));
	CHECK(res.second == true); //should insert
	CHECK(tbl.size() == 2_sz);
	REQUIRE(tbl.get_as<string>(S("b")));
	CHECK(*tbl.get_as<string>(S("b")) == S("kek"sv));
	REQUIRE(tbl == table{{ { S("a"sv), 69 }, { S("b"sv), S("kek") } }});

	res = tbl.emplace<array>(S("c"), 1, 2, 3);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == true);
	CHECK(tbl.size() == 3_sz);
	REQUIRE(tbl.get_as<array>(S("c")));
	CHECK(*tbl.get_as<array>(S("c")) == array{ 1, 2, 3 });
	REQUIRE(tbl == table{{ { S("a"sv), 69 }, { S("b"sv), S("kek"sv) }, { S("c"sv), array{ 1, 2, 3 } } }});

	res = tbl.emplace<int64_t>(S("c"), 1);
	CHECK(res.first == advance(tbl.begin(), 2));
	CHECK(res.second == false);
	CHECK(tbl.size() == 3_sz);
	REQUIRE(!tbl.get_as<int64_t>(S("c")));
	REQUIRE(tbl.get_as<array>(S("c")));
	REQUIRE(tbl == table{{ { S("a"sv), 69 }, { S("b"sv), S("kek"s) }, { S("c"sv), array{ 1, 2, 3 } } }});

	auto it = tbl.erase(tbl.cbegin());
	REQUIRE(tbl == table{{ { S("b"sv), S("kek") }, { S("c"sv), array{ 1, 2, 3 } } }});
	CHECK(it == tbl.begin());
	CHECK(tbl.size() == 2_sz);

	res = tbl.insert_or_assign(S("a"sv), 69);
	CHECK(res.first == tbl.begin());
	CHECK(res.second == true); //should insert
	CHECK(tbl.size() == 3_sz);
	REQUIRE(tbl.get_as<int64_t>(S("a")));
	CHECK(*tbl.get_as<int64_t>(S("a")) == 69);
	REQUIRE(tbl == table{{ { S("a"sv), 69 }, { S("b"sv), S("kek") }, { S("c"sv), array{ 1, 2, 3 } } }});

	it = tbl.erase(advance(tbl.cbegin(), 1), advance(tbl.cbegin(), 3));
	REQUIRE(tbl == table{{ { S("a"sv), 69 } }});
	CHECK(it == tbl.end());
	CHECK(tbl.size() == 1_sz);

	tbl.clear();
	REQUIRE(tbl == table{});
	CHECK(tbl.size() == 0_sz);
	CHECK(tbl.empty());
}
