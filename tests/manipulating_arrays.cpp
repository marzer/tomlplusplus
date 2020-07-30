// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

TEST_CASE("arrays - moving")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(test = [ "foo" ])"sv,
		[&](table&& tbl)
		{
			// sanity-check initial state of a freshly-parsed array
			auto arr1 = tbl["test"].as<array>();
			REQUIRE(arr1);
			CHECK(arr1->size() == 1_sz);
			CHECK(arr1->source().begin == source_position{ 1, 8 });
			CHECK(arr1->source().end == source_position{ 1, 17 });
			CHECK(arr1->source().path);
			CHECK(*arr1->source().path == filename);
			REQUIRE(arr1->get_as<std::string>(0_sz));
			CHECK(*arr1->get_as<std::string>(0_sz) == "foo"sv);

			// sanity-check initial state of default-constructed array
			array arr2;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0_sz);

			// check the results of move-assignment
			arr2 = std::move(*arr1);
			CHECK(arr2.source().begin == source_position{ 1, 8 });
			CHECK(arr2.source().end == source_position{ 1, 17 });
			CHECK(arr2.source().path);
			CHECK(*arr2.source().path == filename);
			CHECK(arr2.size() == 1_sz);
			REQUIRE(arr2.get_as<std::string>(0_sz));
			CHECK(*arr2.get_as<std::string>(0_sz) == "foo"sv);

			// check that moved-from array is now the same as default-constructed
			CHECK(arr1->source().begin == source_position{});
			CHECK(arr1->source().end == source_position{});
			CHECK(!arr1->source().path);
			CHECK(arr1->size() == 0_sz);

			// check the results of move-construction
			array arr3{ std::move(arr2) };
			CHECK(arr3.source().begin == source_position{ 1, 8 });
			CHECK(arr3.source().end == source_position{ 1, 17 });
			CHECK(arr3.source().path);
			CHECK(*arr3.source().path == filename);
			CHECK(arr3.size() == 1_sz);
			REQUIRE(arr3.get_as<std::string>(0_sz));
			CHECK(*arr3.get_as<std::string>(0_sz) == "foo"sv);

			// check that moved-from array is now the same as default-constructed
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0_sz);
		},
		filename
	);
}

TEST_CASE("arrays - copying")
{
	static constexpr auto filename = "foo.toml"sv;

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(test = [ "foo" ])"sv,
		[&](table&& tbl)
		{
			// sanity-check initial state of a freshly-parsed array
			auto arr1 = tbl["test"].as<array>();
			REQUIRE(arr1);
			CHECK(arr1->size() == 1_sz);
			CHECK(arr1->source().begin == source_position{ 1, 8 });
			CHECK(arr1->source().end == source_position{ 1, 17 });
			CHECK(arr1->source().path);
			CHECK(*arr1->source().path == filename);
			REQUIRE(arr1->get_as<std::string>(0_sz));
			CHECK(*arr1->get_as<std::string>(0_sz) == "foo"sv);

			// sanity-check initial state of default-constructed array
			array arr2;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0_sz);

			// check the results of copy-assignment
			arr2 = *arr1;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 1_sz);
			REQUIRE(arr2.get_as<std::string>(0_sz));
			CHECK(*arr2.get_as<std::string>(0_sz) == "foo"sv);
			CHECK(arr2 == *arr1);

			// check the results of copy-construction
			array arr3{ arr2 };
			CHECK(arr3.source().begin == source_position{});
			CHECK(arr3.source().end == source_position{});
			CHECK(!arr3.source().path);
			CHECK(arr3.size() == 1_sz);
			REQUIRE(arr3.get_as<std::string>(0_sz));
			CHECK(*arr3.get_as<std::string>(0_sz) == "foo"sv);
			CHECK(arr3 == *arr1);
			CHECK(arr3 == arr2);
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
		CHECK(!arr.is_homogeneous<double>());
	}

	{
		array arr{ 42, "test"sv, 10.0f, array{}, value{ 3 } };
		CHECK(arr.size() == 5_sz);
		CHECK(!arr.empty());
		REQUIRE(arr.get_as<int64_t>(0_sz));
		CHECK(*arr.get_as<int64_t>(0_sz) == 42);
		REQUIRE(arr.get_as<std::string>(1_sz));
		CHECK(*arr.get_as<std::string>(1_sz) == "test"sv);
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 10.0);
		REQUIRE(arr.get_as<array>(3_sz));
		REQUIRE(arr.get_as<int64_t>(4_sz));
		CHECK(*arr.get_as<int64_t>(4_sz) == 3);
		CHECK(!arr.is_homogeneous());
	}

	#if TOML_WINDOWS_COMPAT
	{
		array arr{ "mixed", "string"sv, L"test", L"kek"sv };
		CHECK(arr.size() == 4_sz);
		CHECK(arr.is_homogeneous());
		CHECK(arr.is_homogeneous<std::string>());
		CHECK(*arr.get_as<std::string>(0) == "mixed"sv);
		CHECK(*arr.get_as<std::string>(1) == "string"sv);
		CHECK(*arr.get_as<std::string>(2) == "test"sv);
		CHECK(*arr.get_as<std::string>(3) == "kek"sv);
	}
	#endif // TOML_WINDOWS_COMPAT
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

	// insert(const_iterator pos, ElemType&& val)
	auto it = arr.insert(arr.cbegin(), 42);
	CHECK(it == arr.begin());
	CHECK(arr.size() == 1_sz);
	CHECK(!arr.empty());
	REQUIRE(arr.get_as<int64_t>(0_sz));
	CHECK(*arr.get_as<int64_t>(0_sz) == 42);
	REQUIRE(arr == array{ 42 });

	// insert(const_iterator pos, size_t count, ElemType&& val)
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

	// emplace(const_iterator pos, Args &&... args) noexcept
	it = arr.emplace<array>(arr.cbegin(), 1, 2, 3);
	CHECK(it == arr.begin());
	CHECK(arr.size() == 5_sz);
	REQUIRE(arr.get_as<array>(0_sz));
	CHECK(arr.get_as<array>(0_sz)->size() == 3_sz);
	REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0 });

	// push_back(ElemType&& val) noexcept
	{
		arr.push_back("test"sv);
		auto& val = *arr.back().as_string();
		CHECK(arr.size() == 6_sz);
		REQUIRE(arr.get_as<std::string>(5_sz));
		CHECK(*arr.get_as<std::string>(5_sz) == "test"sv);
		CHECK(val == "test"sv);
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, "test"sv });
	}

	// decltype(auto) emplace_back(Args&&... args) noexcept
	{
		decltype(auto) val = arr.emplace_back<std::string>("test2"sv);
		CHECK(arr.size() == 7_sz);
		REQUIRE(arr.get_as<std::string>(6_sz));
		CHECK(*arr.get_as<std::string>(6_sz) == "test2"sv);
		CHECK(val == "test2"sv);
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, "test"sv, "test2"sv });
	}

	// erase(const_iterator pos) noexcept;
	it = arr.erase(arr.cbegin());
	REQUIRE(arr == array{ 42, 10.0, 10.0, 10.0, "test"sv, "test2"sv });
	CHECK(it == arr.begin());
	CHECK(arr.size() == 6_sz);

	// erase(const_iterator first, const_iterator last) noexcept;
	it = arr.erase(arr.cbegin() + 2, arr.cbegin() + 4);
	REQUIRE(arr == array{ 42, 10.0, "test"sv, "test2"sv });
	CHECK(it == arr.begin() + 2);
	CHECK(arr.size() == 4_sz);

	arr.pop_back();
	REQUIRE(arr == array{ 42, 10.0, "test"sv });
	CHECK(arr.size() == 3_sz);

	arr.clear();
	REQUIRE(arr == array{});
	CHECK(arr.size() == 0_sz);
	CHECK(arr.empty());

	// insert(const_iterator pos, Iter first, Iter last)
	{
		auto vals = std::vector{ 1.0, 2.0, 3.0 };
		arr.insert(arr.cbegin(), vals.begin(), vals.end());
		CHECK(arr.size() == 3_sz);
		REQUIRE(arr.get_as<double>(0_sz));
		CHECK(*arr.get_as<double>(0_sz) == 1.0);
		REQUIRE(arr.get_as<double>(1_sz));
		CHECK(*arr.get_as<double>(1_sz) == 2.0);
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 3.0);

		arr.insert(arr.cbegin() + 1, vals.begin(), vals.end());
		CHECK(arr.size() == 6_sz);
		REQUIRE(arr.get_as<double>(0_sz));
		CHECK(*arr.get_as<double>(0_sz) == 1.0);
		REQUIRE(arr.get_as<double>(1_sz));
		CHECK(*arr.get_as<double>(1_sz) == 1.0);
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 2.0);
		REQUIRE(arr.get_as<double>(3_sz));
		CHECK(*arr.get_as<double>(3_sz) == 3.0);
		REQUIRE(arr.get_as<double>(4_sz));
		CHECK(*arr.get_as<double>(4_sz) == 2.0);
		REQUIRE(arr.get_as<double>(5_sz));
		CHECK(*arr.get_as<double>(5_sz) == 3.0);
	}

	// iterator insert(const_iterator pos, std::initializer_list<ElemType> ilist) noexcept
	{
		arr.clear();

		arr.insert(arr.cbegin(), { 1.0, 2.0, 3.0 });
		CHECK(arr.size() == 3_sz);
		REQUIRE(arr.get_as<double>(0_sz));
		CHECK(*arr.get_as<double>(0_sz) == 1.0);
		REQUIRE(arr.get_as<double>(1_sz));
		CHECK(*arr.get_as<double>(1_sz) == 2.0);
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 3.0);

		arr.insert(arr.cbegin() + 1, { 1.0, 2.0, 3.0 });
		CHECK(arr.size() == 6_sz);
		REQUIRE(arr.get_as<double>(0_sz));
		CHECK(*arr.get_as<double>(0_sz) == 1.0);
		REQUIRE(arr.get_as<double>(1_sz));
		CHECK(*arr.get_as<double>(1_sz) == 1.0);
		REQUIRE(arr.get_as<double>(2_sz));
		CHECK(*arr.get_as<double>(2_sz) == 2.0);
		REQUIRE(arr.get_as<double>(3_sz));
		CHECK(*arr.get_as<double>(3_sz) == 3.0);
		REQUIRE(arr.get_as<double>(4_sz));
		CHECK(*arr.get_as<double>(4_sz) == 2.0);
		REQUIRE(arr.get_as<double>(5_sz));
		CHECK(*arr.get_as<double>(5_sz) == 3.0);
	}

	#if TOML_WINDOWS_COMPAT

	arr.clear();
	it = arr.insert(arr.cbegin(), L"test");
	REQUIRE(*arr.get_as<std::string>(0_sz) == "test"sv);
	
	it = arr.emplace<std::string>(arr.cbegin(), L"test2"sv);
	REQUIRE(*arr.get_as<std::string>(0_sz) == "test2"sv);

	arr.push_back(L"test3"s);
	REQUIRE(*arr.back().as_string() == "test3"sv);

	arr.emplace_back<std::string>(L"test4");
	REQUIRE(*arr.back().as_string() == "test4"sv);

	#endif // TOML_WINDOWS_COMPAT
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
			array{inserter{array{}}},
			array{array{},array{array{},array{}},array{}},
			array{array{array{array{array{array{ 1 }}}}}}
		};
		arr.flatten();
		REQUIRE(arr == array{ 1 });
	}
}

TEST_CASE("arrays - resizing and truncation")
{
	array arr{ 1, 2, 3, 4, 5 };
	REQUIRE(arr.size() == 5_sz);

	// truncate with no change
	arr.truncate(5_sz);
	REQUIRE(arr.size() == 5_sz);
	REQUIRE(arr == array{ 1, 2, 3, 4, 5 });

	// truncate down to three elements
	arr.truncate(3_sz);
	REQUIRE(arr.size() == 3_sz);
	REQUIRE(arr == array{ 1, 2, 3 });

	// resize down to two elements
	arr.resize(2_sz, 42);
	REQUIRE(arr.size() == 2_sz);
	REQUIRE(arr == array{ 1, 2 });

	// resize with no change
	arr.resize(2_sz, 42);
	REQUIRE(arr.size() == 2_sz);
	REQUIRE(arr == array{ 1, 2 });

	//resize up to six elements
	arr.resize(6_sz, 42);
	REQUIRE(arr.size() == 6_sz);
	REQUIRE(arr == array{ 1, 2, 42, 42, 42, 42 });
}
