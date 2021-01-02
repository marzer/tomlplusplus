// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
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
			CHECK(arr1->size() == 1u);
			CHECK(arr1->source().begin == source_position{ 1, 8 });
			CHECK(arr1->source().end == source_position{ 1, 17 });
			CHECK(arr1->source().path);
			CHECK(*arr1->source().path == filename);
			REQUIRE(arr1->get_as<std::string>(0u));
			CHECK(*arr1->get_as<std::string>(0u) == "foo"sv);

			// sanity-check initial state of default-constructed array
			array arr2;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0u);

			// check the results of move-assignment
			arr2 = std::move(*arr1);
			CHECK(arr2.source().begin == source_position{ 1, 8 });
			CHECK(arr2.source().end == source_position{ 1, 17 });
			CHECK(arr2.source().path);
			CHECK(*arr2.source().path == filename);
			CHECK(arr2.size() == 1u);
			REQUIRE(arr2.get_as<std::string>(0u));
			CHECK(*arr2.get_as<std::string>(0u) == "foo"sv);

			// check that moved-from array is now the same as default-constructed
			CHECK(arr1->source().begin == source_position{});
			CHECK(arr1->source().end == source_position{});
			CHECK(!arr1->source().path);
			CHECK(arr1->size() == 0u);

			// check the results of move-construction
			array arr3{ std::move(arr2) };
			CHECK(arr3.source().begin == source_position{ 1, 8 });
			CHECK(arr3.source().end == source_position{ 1, 17 });
			CHECK(arr3.source().path);
			CHECK(*arr3.source().path == filename);
			CHECK(arr3.size() == 1u);
			REQUIRE(arr3.get_as<std::string>(0u));
			CHECK(*arr3.get_as<std::string>(0u) == "foo"sv);

			// check that moved-from array is now the same as default-constructed
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0u);
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
			CHECK(arr1->size() == 1u);
			CHECK(arr1->source().begin == source_position{ 1, 8 });
			CHECK(arr1->source().end == source_position{ 1, 17 });
			CHECK(arr1->source().path);
			CHECK(*arr1->source().path == filename);
			REQUIRE(arr1->get_as<std::string>(0u));
			CHECK(*arr1->get_as<std::string>(0u) == "foo"sv);

			// sanity-check initial state of default-constructed array
			array arr2;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 0u);

			// check the results of copy-assignment
			arr2 = *arr1;
			CHECK(arr2.source().begin == source_position{});
			CHECK(arr2.source().end == source_position{});
			CHECK(!arr2.source().path);
			CHECK(arr2.size() == 1u);
			REQUIRE(arr2.get_as<std::string>(0u));
			CHECK(*arr2.get_as<std::string>(0u) == "foo"sv);
			CHECK(arr2 == *arr1);

			// check the results of copy-construction
			array arr3{ arr2 };
			CHECK(arr3.source().begin == source_position{});
			CHECK(arr3.source().end == source_position{});
			CHECK(!arr3.source().path);
			CHECK(arr3.size() == 1u);
			REQUIRE(arr3.get_as<std::string>(0u));
			CHECK(*arr3.get_as<std::string>(0u) == "foo"sv);
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
		CHECK(arr.size() == 0u);
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
		CHECK(arr.size() == 1u);
		CHECK(!arr.empty());
		CHECK(arr.begin() != arr.end());
		CHECK(arr.cbegin() != arr.cend());
		REQUIRE(arr.get_as<int64_t>(0u));
		CHECK(*arr.get_as<int64_t>(0u) == 42);
		CHECK(arr.is_homogeneous());
		CHECK(arr.is_homogeneous<int64_t>());
		CHECK(!arr.is_homogeneous<double>());
	}

	{
		array arr{ 42, "test"sv, 10.0f, array{}, value{ 3 } };
		CHECK(arr.size() == 5u);
		CHECK(!arr.empty());
		REQUIRE(arr.get_as<int64_t>(0u));
		CHECK(*arr.get_as<int64_t>(0u) == 42);
		REQUIRE(arr.get_as<std::string>(1u));
		CHECK(*arr.get_as<std::string>(1u) == "test"sv);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 10.0);
		REQUIRE(arr.get_as<array>(3u));
		REQUIRE(arr.get_as<int64_t>(4u));
		CHECK(*arr.get_as<int64_t>(4u) == 3);
		CHECK(!arr.is_homogeneous());
	}

	#if TOML_WINDOWS_COMPAT
	{
		array arr{ "mixed", "string"sv, L"test", L"kek"sv };
		CHECK(arr.size() == 4u);
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
	CHECK(arr.size() == 1u);
	CHECK(!arr.empty());
	REQUIRE(arr.get_as<int64_t>(0u));
	CHECK(*arr.get_as<int64_t>(0u) == 42);
	REQUIRE(arr == array{ 42 });

	// insert(const_iterator pos, size_t count, ElemType&& val)
	it = arr.insert(arr.cend(), 3, 10.0f);
	CHECK(it == arr.begin() + 1);
	CHECK(arr.size() == 4u);
	REQUIRE(arr.get_as<double>(1u));
	CHECK(*arr.get_as<double>(1u) == 10.0);
	REQUIRE(arr.get_as<double>(2u));
	CHECK(*arr.get_as<double>(2u) == 10.0);
	REQUIRE(arr.get_as<double>(3u));
	CHECK(*arr.get_as<double>(3u) == 10.0);
	REQUIRE(arr == array{ 42, 10.0, 10.0, 10.0 });

	// emplace(const_iterator pos, Args &&... args) noexcept
	it = arr.emplace<array>(arr.cbegin(), 1, 2, 3);
	CHECK(it == arr.begin());
	CHECK(arr.size() == 5u);
	REQUIRE(arr.get_as<array>(0u));
	CHECK(arr.get_as<array>(0u)->size() == 3u);
	REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0 });

	// push_back(ElemType&& val) noexcept
	{
		arr.push_back("test"sv);
		auto& val = *arr.back().as_string();
		CHECK(arr.size() == 6u);
		REQUIRE(arr.get_as<std::string>(5u));
		CHECK(*arr.get_as<std::string>(5u) == "test"sv);
		CHECK(val == "test"sv);
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, "test"sv });
	}

	// decltype(auto) emplace_back(Args&&... args) noexcept
	{
		decltype(auto) val = arr.emplace_back<std::string>("test2"sv);
		CHECK(arr.size() == 7u);
		REQUIRE(arr.get_as<std::string>(6u));
		CHECK(*arr.get_as<std::string>(6u) == "test2"sv);
		CHECK(val == "test2"sv);
		CHECK(&val == &arr.back());
		REQUIRE(arr == array{ array{ 1, 2, 3 }, 42, 10.0, 10.0, 10.0, "test"sv, "test2"sv });
	}

	// erase(const_iterator pos) noexcept;
	it = arr.erase(arr.cbegin());
	REQUIRE(arr == array{ 42, 10.0, 10.0, 10.0, "test"sv, "test2"sv });
	CHECK(it == arr.begin());
	CHECK(arr.size() == 6u);

	// erase(const_iterator first, const_iterator last) noexcept;
	it = arr.erase(arr.cbegin() + 2, arr.cbegin() + 4);
	REQUIRE(arr == array{ 42, 10.0, "test"sv, "test2"sv });
	CHECK(it == arr.begin() + 2);
	CHECK(arr.size() == 4u);

	arr.pop_back();
	REQUIRE(arr == array{ 42, 10.0, "test"sv });
	CHECK(arr.size() == 3u);

	arr.clear();
	REQUIRE(arr == array{});
	CHECK(arr.size() == 0u);
	CHECK(arr.empty());

	// insert(const_iterator pos, Iter first, Iter last)
	{
		auto vals = std::vector{ 1.0, 2.0, 3.0 };
		arr.insert(arr.cbegin(), vals.begin(), vals.end());
		CHECK(arr.size() == 3u);
		REQUIRE(arr.get_as<double>(0u));
		CHECK(*arr.get_as<double>(0u) == 1.0);
		REQUIRE(arr.get_as<double>(1u));
		CHECK(*arr.get_as<double>(1u) == 2.0);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 3.0);

		arr.insert(arr.cbegin() + 1, vals.begin(), vals.end());
		CHECK(arr.size() == 6u);
		REQUIRE(arr.get_as<double>(0u));
		CHECK(*arr.get_as<double>(0u) == 1.0);
		REQUIRE(arr.get_as<double>(1u));
		CHECK(*arr.get_as<double>(1u) == 1.0);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 2.0);
		REQUIRE(arr.get_as<double>(3u));
		CHECK(*arr.get_as<double>(3u) == 3.0);
		REQUIRE(arr.get_as<double>(4u));
		CHECK(*arr.get_as<double>(4u) == 2.0);
		REQUIRE(arr.get_as<double>(5u));
		CHECK(*arr.get_as<double>(5u) == 3.0);
	}

	// insert(const_iterator pos, Iter first, Iter last) (with move iterators)
	{
		arr.clear();

		std::vector<std::string> vals{ "foo", "bar", "kek" };
		arr.insert(arr.cbegin(), std::make_move_iterator(vals.begin()), std::make_move_iterator(vals.end()));
		CHECK(arr.size() == 3u);
		REQUIRE(arr.get_as<std::string>(0));
		CHECK(*arr.get_as<std::string>(0) == "foo");
		REQUIRE(arr.get_as<std::string>(1));
		CHECK(*arr.get_as<std::string>(1) == "bar");
		REQUIRE(arr.get_as<std::string>(2));
		CHECK(*arr.get_as<std::string>(2) == "kek");

		REQUIRE(vals.size() == 3u);
		CHECK(vals[0] == "");
		CHECK(vals[1] == "");
		CHECK(vals[2] == "");
	}

	// iterator insert(const_iterator pos, std::initializer_list<ElemType> ilist) noexcept
	{
		arr.clear();

		arr.insert(arr.cbegin(), { 1.0, 2.0, 3.0 });
		CHECK(arr.size() == 3u);
		REQUIRE(arr.get_as<double>(0u));
		CHECK(*arr.get_as<double>(0u) == 1.0);
		REQUIRE(arr.get_as<double>(1u));
		CHECK(*arr.get_as<double>(1u) == 2.0);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 3.0);

		arr.insert(arr.cbegin() + 1, { 1.0, 2.0, 3.0 });
		CHECK(arr.size() == 6u);
		REQUIRE(arr.get_as<double>(0u));
		CHECK(*arr.get_as<double>(0u) == 1.0);
		REQUIRE(arr.get_as<double>(1u));
		CHECK(*arr.get_as<double>(1u) == 1.0);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 2.0);
		REQUIRE(arr.get_as<double>(3u));
		CHECK(*arr.get_as<double>(3u) == 3.0);
		REQUIRE(arr.get_as<double>(4u));
		CHECK(*arr.get_as<double>(4u) == 2.0);
		REQUIRE(arr.get_as<double>(5u));
		CHECK(*arr.get_as<double>(5u) == 3.0);
	}

	#if TOML_WINDOWS_COMPAT

	arr.clear();
	it = arr.insert(arr.cbegin(), L"test");
	REQUIRE(*arr.get_as<std::string>(0u) == "test"sv);
	
	it = arr.emplace<std::string>(arr.cbegin(), L"test2"sv);
	REQUIRE(*arr.get_as<std::string>(0u) == "test2"sv);

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
	REQUIRE(arr.size() == 5u);

	// truncate with no change
	arr.truncate(5u);
	REQUIRE(arr.size() == 5u);
	REQUIRE(arr == array{ 1, 2, 3, 4, 5 });

	// truncate down to three elements
	arr.truncate(3u);
	REQUIRE(arr.size() == 3u);
	REQUIRE(arr == array{ 1, 2, 3 });

	// resize down to two elements
	arr.resize(2u, 42);
	REQUIRE(arr.size() == 2u);
	REQUIRE(arr == array{ 1, 2 });

	// resize with no change
	arr.resize(2u, 42);
	REQUIRE(arr.size() == 2u);
	REQUIRE(arr == array{ 1, 2 });

	//resize up to six elements
	arr.resize(6u, 42);
	REQUIRE(arr.size() == 6u);
	REQUIRE(arr == array{ 1, 2, 42, 42, 42, 42 });
}
