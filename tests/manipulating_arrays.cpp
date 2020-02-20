#include "tests.h"

TEST_CASE("arrays - moving")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		S(R"(test = [ "foo" ])"sv),
		[&](table&& tbl) noexcept
		{
			CHECK(tbl.source().begin == source_position{ 1, 1 });
			CHECK(tbl.source().end == source_position{ 1, 17 });
			CHECK(tbl.source().path);
			CHECK(*tbl.source().path == filename);
			CHECK(tbl.size() == 1_sz);

			auto arr1 = tbl[S("test")].as<array>();
			REQUIRE(arr1);
			CHECK(arr1->size() == 1_sz);
			CHECK(arr1->source().begin == source_position{ 1, 8 });
			CHECK(arr1->source().end == source_position{ 1, 17 });
			CHECK(arr1->source().path);
			CHECK(*arr1->source().path == filename);
			REQUIRE(arr1->get_as<string>(0_sz));
			CHECK(*arr1->get_as<string>(0_sz) == S("foo"sv));

			array arr2;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0_sz);

			arr2 = std::move(*arr1);
			CHECK(arr2.source().begin == source_position{ 1, 8 });
			CHECK(arr2.source().end == source_position{ 1, 17 });
			CHECK(arr2.source().path);
			CHECK(*arr2.source().path == filename);
			CHECK(arr2.size() == 1_sz);
			REQUIRE(arr2.get_as<string>(0_sz));
			CHECK(*arr2.get_as<string>(0_sz) == S("foo"sv));

			CHECK(arr1->source().begin == source_position{});
			CHECK(arr1->source().end == source_position{});
			CHECK(!arr1->source().path);
			CHECK(arr1->size() == 0_sz);
		},
		filename
	);

}

TEST_CASE("arrays - construction")
{
	{
		array arr;
		CHECK(arr.size() == 0_sz);
		CHECK(arr.empty());
		CHECK(arr.begin() == arr.end());
		CHECK(arr.cbegin() == arr.cend());
		CHECK(arr.source().begin == source_position{});
		CHECK(arr.source().end == source_position{});
		CHECK(!arr.source().path);
		CHECK(!arr.is_homogeneous());
	}
	
	{
		array arr{ 42 };
		CHECK(arr.size() == 1_sz);
		CHECK(!arr.empty());
		CHECK(arr.begin() != arr.end());
		CHECK(arr.cbegin() != arr.cend());
		REQUIRE(arr.get_as<int64_t>(0_sz));
		CHECK(*arr.get_as<int64_t>(0_sz) == 42);
		CHECK(arr.is_homogeneous());
		CHECK(arr.is_homogeneous<int64_t>());
	}

	{
		array arr{ 42, S("test"sv), 10.0f, array{}, value{ 3 } };
		CHECK(arr.size() == 5_sz);
		CHECK(!arr.empty());
		REQUIRE(arr.get_as<int64_t>(0_sz));
		CHECK(*arr.get_as<int64_t>(0_sz) == 42);
		REQUIRE(arr.get_as<string>(1_sz));
		CHECK(*arr.get_as<string>(1_sz) == S("test"sv));
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 10.0);
		REQUIRE(arr.get_as<array>(3_sz));
		REQUIRE(arr.get_as<int64_t>(4_sz));
		CHECK(*arr.get_as<int64_t>(4_sz) == 3);
		CHECK(!arr.is_homogeneous());
	}
}

TEST_CASE("arrays - equality")
{
	array arr1{ 1, 2, 3 };
	CHECK(arr1 == arr1);
	{
		auto ilist = { 1, 2, 3 };
		CHECK(arr1 == ilist);
		CHECK(ilist == arr1);

		ilist = { 2, 3, 4 };
		CHECK(arr1 != ilist);
		CHECK(ilist != arr1);

		auto ivec = std::vector{ 1, 2, 3 };
		CHECK(arr1 == ivec);
		CHECK(ivec == arr1);

		ivec = std::vector{ 2, 3, 4 };
		CHECK(arr1 != ivec);
		CHECK(ivec != arr1);
	}

	array arr2{ 1, 2, 3 };
	CHECK(arr1 == arr2);

	array arr3{ 1, 2 };
	CHECK(arr1 != arr3);

	array arr4{ 1, 2, 3, 4 };
	CHECK(arr1 != arr4);

	array arr5{ 1, 2, 3.0 };
	CHECK(arr1 != arr5);

	array arr6{};
	CHECK(arr1 != arr6);
	CHECK(arr6 == arr6);

	array arr7{};
	CHECK(arr6 == arr7);
}

TEST_CASE("arrays - insertion and erasure")
{
	array arr;
	auto it = arr.insert(arr.cbegin(), 42);
	CHECK(it == arr.begin());
	CHECK(arr.size() == 1_sz);
	CHECK(!arr.empty());
	REQUIRE(arr.get_as<int64_t>(0_sz));
	CHECK(*arr.get_as<int64_t>(0_sz) == 42);
	REQUIRE(arr == array{ 42 });

	it = arr.insert(arr.cend(), 3, 10.0f);
	CHECK(it == arr.begin() + 1);
	CHECK(arr.size() == 4_sz);
	REQUIRE(arr.get_as<double>(1_sz));
	CHECK(*arr.get_as<double>(1_sz) == 10.0);
	REQUIRE(arr.get_as<double>(2_sz));
	CHECK(*arr.get_as<double>(2_sz) == 10.0);
	REQUIRE(arr.get_as<double>(3_sz));
	CHECK(*arr.get_as<double>(3_sz) == 10.0);
	REQUIRE(arr == array{ 42, 10.0, 10.0, 10.0 });

	it = arr.emplace<array>(arr.cbegin(), 1, 2, 3);
	CHECK(it == arr.begin());
	CHECK(arr.size() == 5_sz);
	REQUIRE(arr.get_as<array>(0_sz));
	CHECK(arr.get_as<array>(0_sz)->size() == 3_sz);
	REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0 });

	{
		decltype(auto) val = arr.push_back(S("test"sv));
		CHECK(arr.size() == 6_sz);
		REQUIRE(arr.get_as<string>(5_sz));
		CHECK(*arr.get_as<string>(5_sz) == S("test"sv));
		CHECK(val == S("test"sv));
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, S("test"sv) });
	}

	{
		decltype(auto) val = arr.emplace_back<string>(S("test2"sv));
		CHECK(arr.size() == 7_sz);
		REQUIRE(arr.get_as<string>(6_sz));
		CHECK(*arr.get_as<string>(6_sz) == S("test2"sv));
		CHECK(val == S("test2"sv));
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, S("test"sv), S("test2"sv) });
	}

	it = arr.erase(arr.cbegin());
	REQUIRE(arr == array{ 42, 10.0, 10.0, 10.0, S("test"sv), S("test2"sv) });
	CHECK(it == arr.begin());
	CHECK(arr.size() == 6_sz);


	it = arr.erase(arr.cbegin() + 2, arr.cbegin() + 4);
	REQUIRE(arr == array{ 42, 10.0, S("test"sv), S("test2"sv) });
	CHECK(it == arr.begin() + 2);
	CHECK(arr.size() == 4_sz);

	arr.pop_back();
	REQUIRE(arr == array{ 42, 10.0, S("test"sv) });
	CHECK(arr.size() == 3_sz);

	arr.clear();
	REQUIRE(arr == array{});
	CHECK(arr.size() == 0_sz);
	CHECK(arr.empty());
}

TEST_CASE("arrays - flattening")
{
	{
		array arr{
			1, 2, 3,
			array{ 4, 5 },
			6,
			array{},
			array{ 7, array{ 8, array{ 9 }, 10, array{}, }, 11 },
		};
		arr.flatten();
		REQUIRE(arr == array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
	}

	{
		array arr{
			array{},
			array{array{}},
			array{array{},array{array{},array{}},array{}},
			array{array{array{array{array{array{ 1 }}}}}}
		};
		arr.flatten();
		REQUIRE(arr == array{ 1 });
	}
}
