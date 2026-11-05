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

			// sanity check the virtual type checks
			CHECK(arr1->type() == node_type::array);
			CHECK(!arr1->is_table());
			CHECK(arr1->is_array());
			CHECK(!arr1->is_array_of_tables());
			CHECK(!arr1->is_value());
			CHECK(!arr1->is_string());
			CHECK(!arr1->is_integer());
			CHECK(!arr1->is_floating_point());
			CHECK(!arr1->is_number());
			CHECK(!arr1->is_boolean());
			CHECK(!arr1->is_date());
			CHECK(!arr1->is_time());
			CHECK(!arr1->is_date_time());

			// sanity check the virtual type casts (non-const)
			CHECK(!arr1->as_table());
			CHECK(arr1->as_array() == arr1);
			CHECK(!arr1->as_string());
			CHECK(!arr1->as_integer());
			CHECK(!arr1->as_floating_point());
			CHECK(!arr1->as_boolean());
			CHECK(!arr1->as_date());
			CHECK(!arr1->as_time());
			CHECK(!arr1->as_date_time());

			// sanity check the virtual type casts (const)
			const auto carr1 = &std::as_const(*arr1);
			CHECK(!carr1->as_table());
			CHECK(carr1->as_array() == carr1);
			CHECK(!carr1->as_string());
			CHECK(!carr1->as_integer());
			CHECK(!carr1->as_floating_point());
			CHECK(!carr1->as_boolean());
			CHECK(!carr1->as_date());
			CHECK(!carr1->as_time());
			CHECK(!carr1->as_date_time());

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
		filename);
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
		filename);
}

TEST_CASE("arrays - assignment between an array and its descendants")
{
	const auto child = array{ 2, "three", table{ { "four", 4 } } };

	array arr{ 1, child };

	SECTION("copy from a descendant")
	{
		arr = *arr.get_as<array>(1);
		CHECK(arr == child);
	}

	SECTION("move from a descendant")
	{
		arr = std::move(*arr.get_as<array>(1));
		CHECK(arr == child);
	}

	SECTION("copy from an ancestor")
	{
		const array original{ arr };
		auto& descendant = *arr.get_as<array>(1);
		descendant		 = arr;
		CHECK(descendant == original);
		CHECK(arr == array{ 1, original });
	}

	SECTION("self-assignment")
	{
		const array original{ arr };
		array& self = arr;
		arr			= self;
		CHECK(arr == original);
		arr = std::move(self);
		CHECK(arr == original);
	}
}

TEST_CASE("arrays - construction")
{
	SECTION("default")
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

	SECTION("one element")
	{
		array arr{ 42 };
		CHECK(arr.size() == 1u);
		CHECK(!arr.empty());
		CHECK(arr.begin() != arr.end());
		CHECK(arr.cbegin() != arr.cend());
		REQUIRE(arr.get_as<int64_t>(0u));
		CHECK(*arr.get_as<int64_t>(0u) == 42);
		CHECK(arr.get(0u) == &arr[0u]);
		CHECK(arr.is_homogeneous());
		CHECK(arr.is_homogeneous<int64_t>());
		CHECK(!arr.is_homogeneous<double>());
		CHECK(arr.get(0u) == &arr.at(0u));

		const array& carr = arr;
		CHECK(carr.size() == 1u);
		CHECK(!carr.empty());
		CHECK(carr.begin() != carr.end());
		CHECK(carr.cbegin() != carr.cend());
		REQUIRE(carr.get_as<int64_t>(0u));
		CHECK(*carr.get_as<int64_t>(0u) == 42);
		CHECK(carr.get(0u) == &carr[0u]);
		CHECK(carr.is_homogeneous());
		CHECK(carr.is_homogeneous<int64_t>());
		CHECK(!carr.is_homogeneous<double>());
		CHECK(carr.get(0u) == &carr.at(0u));
	}

	SECTION("several elements")
	{
		array arr{ 42, "test"sv, 10.0f, array{}, value{ 3 } };
		CHECK(arr.size() == 5u);
		CHECK(!arr.empty());
		REQUIRE(arr.get_as<int64_t>(0u));
		CHECK(*arr.get_as<int64_t>(0u) == 42);
		CHECK(arr.get(0u) == &arr[0u]);
		REQUIRE(arr.get_as<std::string>(1u));
		CHECK(*arr.get_as<std::string>(1u) == "test"sv);
		CHECK(arr.get(1u) == &arr[1u]);
		REQUIRE(arr.get_as<double>(2u));
		CHECK(*arr.get_as<double>(2u) == 10.0);
		REQUIRE(arr.get_as<array>(3u));
		REQUIRE(arr.get_as<int64_t>(4u));
		CHECK(*arr.get_as<int64_t>(4u) == 3);
		CHECK(!arr.is_homogeneous());
		CHECK(arr.get(0u) == &arr.at(0u));
		CHECK(arr.get(1u) == &arr.at(1u));
	}

	SECTION("null node_views are skipped")
	{
		const table src{ { "x", 1 } };
		const array arr{ src["missing"], src["x"], src["missing"], 2 };
		CHECK(arr == array{ 1, 2 });
		CHECK(src == table{ { "x", 1 } });
	}

	SECTION("only null node_views")
	{
		const table src;
		CHECK(array{ src["missing"] }.empty());
		CHECK(array{ src["a"], src["b"] }.empty());
	}

	SECTION("a single child array needs an inserter")
	{
		const array moved{ array{ 42 } };
		CHECK(moved == array{ 42 });

		const array nested{ inserter{ array{ 42 } } };
		REQUIRE(nested.size() == 1u);
		REQUIRE(nested.get_as<array>(0));
		CHECK(*nested.get_as<array>(0) == array{ 42 });
	}

#if TOML_ENABLE_WINDOWS_COMPAT
	SECTION("wide strings")
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
#endif // TOML_ENABLE_WINDOWS_COMPAT
}

TEST_CASE("arrays - equality")
{
	SECTION("size and element types")
	{
		array arr1{ 1, 2, 3 };
		CHECK(arr1 == arr1);
		{
			auto ilist = { 1, 2, 3 };
			CHECK(arr1 == ilist);
			CHECK(ilist == arr1);

			auto other_ilist = { 2, 3, 4 };
			CHECK(arr1 != other_ilist);
			CHECK(other_ilist != arr1);

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

	SECTION("same size and types, different values")
	{
		const array lhs{ 1, "two", 3.0 };
		const array rhs{ 1, "two", 3.5 };
		CHECK_SYMMETRIC_INEQUAL(lhs, rhs);
	}

	SECTION("nested containers are compared by value")
	{
		const array lhs{ 1, array{ 2, table{ { "a", 3 } } } };
		array rhs{ lhs };
		CHECK_SYMMETRIC_EQUAL(lhs, rhs);

		rhs[1].as_array()->get_as<table>(1)->insert_or_assign("a", 4);
		CHECK_SYMMETRIC_INEQUAL(lhs, rhs);
	}

	SECTION("empty initialiser lists and vectors")
	{
		const array empty;
		const std::vector<int> empty_vec;
		const auto empty_ilist = std::initializer_list<int>{};
		CHECK(empty == empty_vec);
		CHECK(empty_vec == empty);
		CHECK(empty == empty_ilist);
		CHECK(empty_ilist == empty);

		const array arr{ 1 };
		CHECK(arr != empty_vec);
		CHECK(empty_vec != arr);
		CHECK(arr != empty_ilist);
		CHECK(empty_ilist != arr);
	}

	SECTION("vectors of a different size or element type")
	{
		const array arr{ 1, 2, 3 };
		CHECK(arr != std::vector<int>{ 1, 2 });
		CHECK(arr != std::vector<int>{ 1, 2, 3, 4 });
		CHECK(arr != std::vector<double>{ 1.0, 2.0, 3.0 });
		CHECK(std::vector<double>{ 1.0, 2.0, 3.0 } != arr);
	}

	SECTION("vectors of string-like types")
	{
		const array arr{ "a", "b" };
		CHECK(arr == std::vector<std::string>{ "a", "b" });
		CHECK(arr == std::vector<std::string_view>{ "a"sv, "b"sv });
		CHECK(arr == std::vector<const char*>{ "a", "b" });
		CHECK(arr != std::vector<std::string>{ "a", "c" });
		CHECK(arr != std::vector<int>{ 1, 2 });
	}
}

TEST_CASE("arrays - is_homogeneous")
{
	const auto samples					   = one_of_each_node_type();
	static constexpr node_type all_types[] = { node_type::none,		node_type::table,	node_type::array,
											   node_type::string,	node_type::integer, node_type::floating_point,
											   node_type::boolean,	node_type::date,	node_type::time,
											   node_type::date_time };

	SECTION("empty arrays are never homogeneous")
	{
		array arr;
		const array& carr = arr;
		CHECK(!carr.is_homogeneous());
		CHECK(!carr.is_homogeneous<int64_t>());
		CHECK(!carr.is_array_of_tables());
		for (const auto type : all_types)
		{
			INFO("type: " << type);
			CHECK(!carr.is_homogeneous(type));

			node* nonmatch = &arr;
			CHECK(!arr.is_homogeneous(type, nonmatch));
			CHECK(nonmatch == nullptr);

			const node* cnonmatch = &arr;
			CHECK(!carr.is_homogeneous(type, cnonmatch));
			CHECK(cnonmatch == nullptr);
		}
	}

	SECTION("elements of one type")
	{
		for (const auto& sample : samples)
		{
			INFO("elements of type " << sample.type());
			array arr{ sample, sample };
			const array& carr = arr;
			CHECK(carr.is_homogeneous());
			CHECK(carr.is_homogeneous(node_type::none));
			CHECK(carr.is_homogeneous(sample.type()));
			CHECK(carr.is_array_of_tables() == (sample.type() == node_type::table));

			node* nonmatch{};
			CHECK(arr.is_homogeneous(node_type::none, nonmatch));
			CHECK(arr.is_homogeneous(sample.type(), nonmatch));

			const node* cnonmatch{};
			CHECK(carr.is_homogeneous(node_type::none, cnonmatch));
			CHECK(carr.is_homogeneous(sample.type(), cnonmatch));

			for (const auto type : all_types)
			{
				if (type == node_type::none || type == sample.type())
					continue;

				INFO("checked against " << type);
				CHECK(!carr.is_homogeneous(type));

				nonmatch = {};
				CHECK(!arr.is_homogeneous(type, nonmatch));
				CHECK(nonmatch == &arr[0]);

				cnonmatch = {};
				CHECK(!carr.is_homogeneous(type, cnonmatch));
				CHECK(cnonmatch == &arr[0]);
			}
		}
	}

	SECTION("elements of mixed types")
	{
		for (const auto& first : samples)
		{
			for (const auto& second : samples)
			{
				if (first.type() == second.type())
					continue;

				INFO("elements of types " << first.type() << " and " << second.type());
				array arr{ first, second };
				const array& carr = arr;
				CHECK(!carr.is_homogeneous());
				CHECK(!carr.is_array_of_tables());

				for (const auto type : all_types)
				{
					INFO("checked against " << type);
					const node* expected_nonmatch =
						(type == node_type::none || type == first.type()) ? &arr[1] : &arr[0];

					CHECK(!carr.is_homogeneous(type));

					node* nonmatch{};
					CHECK(!arr.is_homogeneous(type, nonmatch));
					CHECK(nonmatch == expected_nonmatch);

					const node* cnonmatch{};
					CHECK(!carr.is_homogeneous(type, cnonmatch));
					CHECK(cnonmatch == expected_nonmatch);
				}
			}
		}
	}

	SECTION("the first non-match is reported")
	{
		array arr{ 1, 2, "three", 4, "five" };
		node* nonmatch{};
		CHECK(!arr.is_homogeneous(node_type::integer, nonmatch));
		CHECK(nonmatch == &arr[2]);
		CHECK(!arr.is_homogeneous(node_type::string, nonmatch));
		CHECK(nonmatch == &arr[0]);
	}

	SECTION("typed overloads")
	{
		const array ints{ 1, 2 };
		CHECK(ints.is_homogeneous<int64_t>());
		CHECK(ints.is_homogeneous<toml::value<int64_t>>());
		CHECK(!ints.is_homogeneous<double>());
		CHECK(!ints.is_homogeneous<array>());

		const array tables{ table{}, table{ { "a", 1 } } };
		CHECK(tables.is_homogeneous<table>());
		CHECK(!tables.is_homogeneous<array>());
		CHECK(tables.is_array_of_tables());
	}

	SECTION("through a node reference")
	{
		array arr{ 1, "two" };
		node& nd		= arr;
		const node& cnd = arr;

		node* nonmatch{};
		CHECK(!nd.is_homogeneous(node_type::integer, nonmatch));
		CHECK(nonmatch == &arr[1]);

		const node* cnonmatch{};
		CHECK(!cnd.is_homogeneous(node_type::string, cnonmatch));
		CHECK(cnonmatch == &arr[0]);

		CHECK(!cnd.is_homogeneous());
		CHECK(!cnd.is_homogeneous<int64_t>());
		CHECK(!cnd.is_array_of_tables());
	}
}

TEST_CASE("arrays - element access")
{
	array arr{ 1, "two", 3.0 };
	const array& carr = arr;

	SECTION("get")
	{
		REQUIRE(arr.get(0));
		CHECK(arr.get(0)->value<int64_t>() == 1);
		CHECK(arr.get(3) == nullptr);
		CHECK(carr.get(2) == arr.get(2));
		CHECK(carr.get(100) == nullptr);
		CHECK(array{}.get(0) == nullptr);
	}

	SECTION("get_as")
	{
		REQUIRE(arr.get_as<std::string>(1));
		CHECK(arr.get_as<std::string>(1)->get() == "two"sv);
		CHECK(arr.get_as<toml::value<std::string>>(1) == arr.get_as<std::string>(1));
		CHECK(arr.get_as<int64_t>(1) == nullptr);
		CHECK(arr.get_as<int64_t>(3) == nullptr);
		CHECK(carr.get_as<double>(2) == arr.get_as<double>(2));
		CHECK(carr.get_as<double>(5) == nullptr);
	}

	SECTION("operator[]")
	{
		CHECK(&arr[0] == arr.get(0));
		CHECK(&carr[2] == arr.get(2));
	}

	SECTION("at")
	{
		CHECK(&arr.at(1) == arr.get(1));
		CHECK(&carr.at(2) == arr.get(2));
#if TOML_COMPILER_HAS_EXCEPTIONS
		CHECK_THROWS_AS(arr.at(3), std::out_of_range);
		CHECK_THROWS_AS(carr.at(3), std::out_of_range);
#endif
	}

	SECTION("front and back")
	{
		CHECK(&arr.front() == arr.get(0));
		CHECK(&carr.front() == arr.get(0));
		CHECK(&arr.back() == arr.get(2));
		CHECK(&carr.back() == arr.get(2));

		const array single{ 42 };
		CHECK(&single.front() == &single.back());
	}
}

TEST_CASE("arrays - capacity")
{
	array arr{ 1, 2, 3 };
	CHECK(!arr.empty());
	CHECK(arr.size() == 3u);
	CHECK(arr.capacity() >= arr.size());
	CHECK(arr.max_size() == std::vector<impl::node_ptr>{}.max_size());

	SECTION("reserve")
	{
		arr.reserve(100u);
		CHECK(arr.capacity() >= 100u);
		CHECK(arr == array{ 1, 2, 3 });
	}

	SECTION("shrink_to_fit")
	{
		arr.reserve(100u);
		arr.shrink_to_fit();
		CHECK(arr == array{ 1, 2, 3 });
	}
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
	CHECK(arr.begin() == arr.end());

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

	// iterator replace(const_iterator pos, ElemType&& elem) noexcept
	{
		arr.clear();
		arr.insert(arr.begin(), { 1, 2, 3 });
		CHECK(arr == array{ 1, 2, 3 });
		it = arr.replace(arr.begin() + 1u, "two"sv);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, "two"sv, 3 });
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	arr.clear();
	it = arr.insert(arr.cbegin(), L"test");
	CHECK(*arr.get_as<std::string>(0u) == "test"sv);

	it = arr.emplace<std::string>(arr.cbegin(), L"test2"sv);
	CHECK(*arr.get_as<std::string>(0u) == "test2"sv);

	arr.push_back(L"test3"s);
	CHECK(*arr.back().as_string() == "test3"sv);

	arr.emplace_back<std::string>(L"test4");
	CHECK(*arr.back().as_string() == "test4"sv);

#endif // TOML_ENABLE_WINDOWS_COMPAT

	// push_back with value_flags
	{
		arr.clear();

		auto hex = toml::value{ 1 };
		hex.flags(value_flags::format_as_hexadecimal);
		CHECK(hex.flags() == value_flags::format_as_hexadecimal);

		arr.push_back(hex);
		CHECK(hex.flags() == value_flags::format_as_hexadecimal);
		CHECK(arr.back().as_integer()->flags() == value_flags::format_as_hexadecimal);

		arr.push_back(std::move(hex));
		CHECK(hex.flags() == value_flags{});
		CHECK(arr.back().as_integer()->flags() == value_flags::format_as_hexadecimal);
	}
}

TEST_CASE("arrays - insert")
{
	array arr{ 1, 3 };

	SECTION("at the front, middle and back")
	{
		auto it = arr.insert(arr.cbegin(), 0);
		CHECK(it == arr.begin());
		it = arr.insert(arr.cbegin() + 2, 2);
		CHECK(it == arr.begin() + 2);
		it = arr.insert(arr.cend(), 4);
		CHECK(it == arr.end() - 1);
		CHECK(arr == array{ 0, 1, 2, 3, 4 });
	}

	SECTION("nodes, containers and node_views")
	{
		const table src{ { "x", "ex" } };
		arr.insert(arr.cend(), src);
		arr.insert(arr.cend(), *src.get("x"));
		arr.insert(arr.cend(), src["x"]);
		CHECK(arr == array{ 1, 3, src, "ex", "ex" });
		CHECK(src == table{ { "x", "ex" } });
	}

	SECTION("a null node_view")
	{
		const table src;
		const auto it = arr.insert(arr.cbegin(), src["missing"]);
		CHECK(it == arr.end());
		CHECK(arr == array{ 1, 3 });
	}

	SECTION("repeated values")
	{
		auto it = arr.insert(arr.cbegin() + 1, size_t{ 0 }, 2);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 3 });

		it = arr.insert(arr.cbegin() + 1, size_t{ 1 }, 2);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 2, 3 });

		it = arr.insert(arr.cbegin() + 1, size_t{ 3 }, "honk"sv);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, "honk"sv, "honk"sv, "honk"sv, 2, 3 });

		it = arr.insert(arr.cend(), size_t{ 2 }, 4);
		CHECK(it == arr.begin() + 6);
		CHECK(arr == array{ 1, "honk"sv, "honk"sv, "honk"sv, 2, 3, 4, 4 });
	}

	SECTION("repeated values with a count of the same type")
	{
		const auto it = arr.insert(arr.cend(), 3, 42);
		CHECK(it == arr.begin() + 2);
		CHECK(arr == array{ 1, 3, 42, 42, 42 });
	}

	SECTION("repeated containers")
	{
		const auto it = arr.insert(arr.cend(), size_t{ 2 }, table{ { "a", 1 } });
		CHECK(it == arr.begin() + 2);
		CHECK(arr == array{ 1, 3, table{ { "a", 1 } }, table{ { "a", 1 } } });
	}

	SECTION("repeated node_views")
	{
		const table src{ { "x", 5 } };
		auto it = arr.insert(arr.cbegin(), size_t{ 2 }, src["x"]);
		CHECK(it == arr.begin());
		CHECK(arr == array{ 5, 5, 1, 3 });
		CHECK(src["x"] == 5);

		it = arr.insert(arr.cbegin(), size_t{ 2 }, src["missing"]);
		CHECK(it == arr.end());
		CHECK(arr == array{ 5, 5, 1, 3 });
	}

	SECTION("repeated copies of the array itself")
	{
		const array original{ arr };
		const auto it = arr.insert(arr.cbegin() + 1, size_t{ 2 }, arr);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, original, original, 3 });
	}

	SECTION("repeated copies of an ancestor")
	{
		table tbl{ { "arr", arr } };
		const table original{ tbl };
		auto& child = *tbl.get_as<array>("arr");
		child.insert(child.cend(), size_t{ 2 }, tbl);
		CHECK(child == array{ 1, 3, original, original });
	}

	SECTION("a range from the array itself")
	{
		const auto it = arr.insert(arr.cbegin() + 1, arr.cbegin(), arr.cend());
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 1, 3, 3 });
	}

	SECTION("a range that contains the array")
	{
		array outer{ arr, 4 };
		const array original{ outer };
		auto& inner = *outer.get_as<array>(0);
		inner.insert(inner.cend(), outer.cbegin(), outer.cend());
		CHECK(inner == array{ 1, 3, original[0], 4 });
	}

	SECTION("a range")
	{
		const std::vector<int> vals{ 10, 20 };
		auto it = arr.insert(arr.cbegin() + 1, vals.begin(), vals.end());
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 10, 20, 3 });

		it = arr.insert(arr.cbegin() + 1, vals.end(), vals.end());
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 10, 20, 3 });
	}

	SECTION("a range from another array")
	{
		const array src{ "a", table{ { "b", 2 } } };
		const auto it = arr.insert(arr.cend(), src.begin(), src.end());
		CHECK(it == arr.begin() + 2);
		CHECK(arr == array{ 1, 3, "a", table{ { "b", 2 } } });
		CHECK(src == array{ "a", table{ { "b", 2 } } });
	}

	SECTION("a range of node_views")
	{
		const table src{ { "x", 5 }, { "y", 6 } };
		const std::vector<node_view<const node>> views{ src["missing"], src["x"], src["missing"], src["y"] };
		auto it = arr.insert(arr.cbegin() + 1, views.begin(), views.end());
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 5, 6, 3 });

		const std::vector<node_view<const node>> nulls{ src["a"], src["b"] };
		it = arr.insert(arr.cbegin() + 1, nulls.begin(), nulls.end());
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 5, 6, 3 });
	}

	SECTION("initialiser lists")
	{
		auto it = arr.insert(arr.cbegin() + 1, { 2.0, 2.5 });
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 2.0, 2.5, 3 });

		it = arr.insert(arr.cbegin(), std::initializer_list<int>{});
		CHECK(it == arr.begin());
		CHECK(arr == array{ 1, 2.0, 2.5, 3 });
	}

	SECTION("value flags")
	{
		const std::vector<int> vals{ 12 };
		arr.insert(arr.cbegin(), 10, value_flags::format_as_hexadecimal);
		arr.insert(arr.cbegin(), size_t{ 2 }, 11, value_flags::format_as_octal);
		arr.insert(arr.cbegin(), vals.begin(), vals.end(), value_flags::format_as_binary);
		CHECK(arr == array{ 12, 11, 11, 10, 1, 3 });
		CHECK(arr[0].as_integer()->flags() == value_flags::format_as_binary);
		CHECK(arr[1].as_integer()->flags() == value_flags::format_as_octal);
		CHECK(arr[2].as_integer()->flags() == value_flags::format_as_octal);
		CHECK(arr[3].as_integer()->flags() == value_flags::format_as_hexadecimal);
		CHECK(arr[4].as_integer()->flags() == value_flags::none);
	}
}

TEST_CASE("arrays - emplace")
{
	array arr{ 1, 2 };

	SECTION("constructs an element in place")
	{
		const auto it = arr.emplace<std::string>(arr.cbegin() + 1, "this is not a drill"sv, size_t{ 14 }, size_t{ 5 });
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, "drill"sv, 2 });
	}

	SECTION("deduces the element type")
	{
		arr.emplace(arr.cend(), 2.5);
		arr.emplace(arr.cbegin(), "zero"sv);
		CHECK(arr == array{ "zero"sv, 1, 2, 2.5 });
	}

	SECTION("containers")
	{
		arr.emplace<table>(arr.cend());
		arr.emplace<array>(arr.cend(), 3, 4);
		CHECK(arr == array{ 1, 2, table{}, array{ 3, 4 } });
	}
}

TEST_CASE("arrays - emplace_back")
{
	array arr{ 1, 2 };

	SECTION("returns the new element")
	{
		auto& elem = arr.emplace_back<array>(3, "four"sv);
		static_assert(std::is_same_v<decltype(elem), array&>);
		CHECK(&elem == &arr.back());
		CHECK(arr == array{ 1, 2, array{ 3, "four"sv } });
	}

	SECTION("deduces the element type")
	{
		auto& elem = arr.emplace_back(2.5);
		static_assert(std::is_same_v<decltype(elem), toml::value<double>&>);
		CHECK(&elem == &arr.back());
		CHECK(arr == array{ 1, 2, 2.5 });
	}

	SECTION("value types")
	{
		auto& elem = arr.emplace_back<toml::value<std::string>>("three"sv);
		static_assert(std::is_same_v<decltype(elem), toml::value<std::string>&>);
		CHECK(elem == "three"sv);
		CHECK(arr == array{ 1, 2, "three"sv });
	}
}

TEST_CASE("arrays - push_back")
{
	array arr{ 1 };

	SECTION("nodes, containers and node_views")
	{
		const table src{ { "x", "ex" } };
		arr.push_back(src);
		arr.push_back(*src.get("x"));
		arr.push_back(src["x"]);
		arr.push_back(array{ 2, 3 });
		CHECK(arr == array{ 1, src, "ex", "ex", array{ 2, 3 } });
		CHECK(src == table{ { "x", "ex" } });
	}

	SECTION("a null node_view does nothing")
	{
		const table src;
		arr.push_back(src["missing"]);
		CHECK(arr == array{ 1 });
	}

	SECTION("value flags")
	{
		toml::value<int64_t> hex{ 255 };
		hex.flags(value_flags::format_as_hexadecimal);
		arr.push_back(hex, preserve_source_value_flags);
		arr.push_back(hex, value_flags::format_as_octal);
		arr.push_back(hex, value_flags::none);
		arr.push_back(toml::value<int64_t>{ hex }, value_flags::format_as_binary);
		arr.push_back(255);
		arr.push_back(255, value_flags::format_as_octal);
		arr.push_back(sample_value<toml::date_time>(), value_flags::none);

		REQUIRE(arr == array{ 1, 255, 255, 255, 255, 255, 255, sample_value<toml::date_time>() });
		CHECK(arr[1].as_integer()->flags() == value_flags::format_as_hexadecimal);
		CHECK(arr[2].as_integer()->flags() == value_flags::format_as_octal);
		CHECK(arr[3].as_integer()->flags() == value_flags::none);
		CHECK(arr[4].as_integer()->flags() == value_flags::format_as_binary);
		CHECK(arr[5].as_integer()->flags() == value_flags::none);
		CHECK(arr[6].as_integer()->flags() == value_flags::format_as_octal);
		CHECK(arr[7].as_date_time()->flags() == value_flags::none);
	}
}

TEST_CASE("arrays - replace")
{
	array arr{ 1, 2, 3 };

	SECTION("with a node_view")
	{
		const table src{ { "x", table{ { "y", 1 } } } };
		const auto it = arr.replace(arr.cbegin(), src["x"]);
		CHECK(it == arr.begin());
		CHECK(arr == array{ table{ { "y", 1 } }, 2, 3 });
	}

	SECTION("with a null node_view")
	{
		const table src;
		const auto it = arr.replace(arr.cbegin(), src["missing"]);
		CHECK(it == arr.end());
		CHECK(arr == array{ 1, 2, 3 });
	}

	SECTION("with a descendant of the element being replaced")
	{
		arr.replace(arr.cbegin(), array{ array{ 4, 5 }, 6 });
		arr.replace(arr.cbegin(), *arr[0].as_array()->get_as<array>(0));
		CHECK(arr == array{ array{ 4, 5 }, 2, 3 });
	}

	SECTION("value flags")
	{
		arr.replace(arr.cbegin(), 10, value_flags::format_as_octal);
		CHECK(arr[0].as_integer()->flags() == value_flags::format_as_octal);
	}
}

TEST_CASE("arrays - erasure")
{
	array arr{ 1, 2, 3, 4 };

	SECTION("one element")
	{
		auto it = arr.erase(arr.cbegin() + 1);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 3, 4 });

		it = arr.erase(arr.begin());
		CHECK(it == arr.begin());
		CHECK(arr == array{ 3, 4 });

		it = arr.erase(arr.cend() - 1);
		CHECK(it == arr.end());
		CHECK(arr == array{ 3 });
	}

	SECTION("a range")
	{
		auto it = arr.erase(arr.cbegin() + 1, arr.cbegin() + 3);
		CHECK(it == arr.begin() + 1);
		CHECK(arr == array{ 1, 4 });

		it = arr.erase(arr.cbegin(), arr.cbegin());
		CHECK(it == arr.begin());
		CHECK(arr == array{ 1, 4 });

		it = arr.erase(arr.cbegin(), arr.cend());
		CHECK(it == arr.end());
		CHECK(arr.empty());
	}
}

TEST_CASE("arrays - flattening")
{
	SECTION("nested arrays")
	{
		array arr{
			1,
			2,
			3,
			array{ 4, 5 },
			6,
			array{},
			array{ 7,
				   array{
					   8,
					   array{ 9 },
					   10,
					   array{},
				   },
				   11 },
		};
		CHECK(&arr.flatten() == &arr);
		CHECK(arr == array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
	}

	SECTION("empty and deeply nested arrays")
	{
		array arr{ array{},
				   array{ inserter{ array{} } },
				   array{ array{}, array{ array{}, array{} }, array{} },
				   array{ array{ array{ array{ array{ array{ 1 } } } } } } };
		arr.flatten();
		CHECK(arr == array{ 1 });
	}

	SECTION("an empty array")
	{
		array arr;
		CHECK(&arr.flatten() == &arr);
		CHECK(arr.empty());
	}

	SECTION("no nested arrays")
	{
		const array original{ 1, "two", table{ { "a", array{ 3, array{ 4 } } } } };
		array arr{ original };
		arr.flatten();
		CHECK(arr == original);
	}

	SECTION("only empty nested arrays")
	{
		array arr{ 1, array{}, array{ inserter{ array{} } }, 2 };
		arr.flatten();
		CHECK(arr == array{ 1, 2 });
	}

	SECTION("arrays inside tables are not flattened")
	{
		array arr{ 1, table{ { "a", array{ 2, array{ 3 } } } }, array{ 4, 5 } };
		arr.flatten();
		CHECK(arr == array{ 1, table{ { "a", array{ 2, array{ 3 } } } }, 4, 5 });
	}

	SECTION("rvalue")
	{
		const array flat = array{ 1, array{ 2, array{ 3, 4 } }, 5 }.flatten();
		CHECK(flat == array{ 1, 2, 3, 4, 5 });
	}
}

TEST_CASE("arrays - pruning")
{
	SECTION("recursive and non-recursive")
	{
		const auto arr = array{
			1, array{ 2, array{}, 3 }, table{ { "4", 5 }, { "6", array{} } }, array{}, 8, array{ table{} }, 9, 10
		};

		const auto pruned_recursive = array{ 1, array{ 2, 3 }, table{ { "4", 5 } }, 8, 9, 10 };
		CHECK(array{ arr }.prune(true) == pruned_recursive);

		const auto pruned_flat =
			array{ 1, array{ 2, array{}, 3 }, table{ { "4", 5 }, { "6", array{} } }, 8, array{ table{} }, 9, 10 };
		CHECK(array{ arr }.prune(false) == pruned_flat);
	}

	SECTION("children that become empty are removed")
	{
		array arr{ 1, array{ inserter{ array{} } }, table{ { "a", table{} } }, 2 };
		CHECK(&arr.prune() == &arr);
		CHECK(arr == array{ 1, 2 });
	}

	SECTION("non-recursive pruning removes empty tables and arrays")
	{
		array arr{ 1, table{}, array{}, table{ { "a", table{} } } };
		CHECK(&arr.prune(false) == &arr);
		CHECK(arr == array{ 1, table{ { "a", table{} } } });
	}

	SECTION("an empty array")
	{
		array arr;
		CHECK(arr.prune().empty());
	}
}

TEST_CASE("arrays - resizing and truncation")
{
	array arr{ 1, 2, 3, 4, 5 };
	CHECK(arr.size() == 5u);

	SECTION("shrinking and growing")
	{
		arr.truncate(5u);
		CHECK(arr.size() == 5u);
		CHECK(arr == array{ 1, 2, 3, 4, 5 });

		arr.truncate(3u);
		CHECK(arr.size() == 3u);
		CHECK(arr == array{ 1, 2, 3 });

		arr.resize(2u, 42);
		CHECK(arr.size() == 2u);
		CHECK(arr == array{ 1, 2 });

		arr.resize(2u, 42);
		CHECK(arr.size() == 2u);
		CHECK(arr == array{ 1, 2 });

		arr.resize(6u, 42);
		CHECK(arr.size() == 6u);
		CHECK(arr == array{ 1, 2, 42, 42, 42, 42 });
	}

	SECTION("truncate past the end does nothing")
	{
		arr.truncate(10u);
		CHECK(arr == array{ 1, 2, 3, 4, 5 });
	}

	SECTION("truncate to zero")
	{
		arr.truncate(0u);
		CHECK(arr.empty());
	}

	SECTION("resize to zero")
	{
		arr.resize(0u, 42);
		CHECK(arr.empty());
	}

	SECTION("resize by one")
	{
		arr.resize(6u, "six"sv);
		CHECK(arr == array{ 1, 2, 3, 4, 5, "six"sv });
	}

	SECTION("resize with containers")
	{
		arr.resize(7u, table{ { "a", 1 } });
		CHECK(arr == array{ 1, 2, 3, 4, 5, table{ { "a", 1 } }, table{ { "a", 1 } } });
	}

	SECTION("resize with copies of the array itself")
	{
		const array original{ arr };
		arr.resize(7u, arr);
		CHECK(arr == array{ 1, 2, 3, 4, 5, original, original });
	}

	SECTION("resize with value flags")
	{
		arr.resize(7u, 8, value_flags::format_as_hexadecimal);
		CHECK(arr == array{ 1, 2, 3, 4, 5, 8, 8 });
		CHECK(arr[4].as_integer()->flags() == value_flags::none);
		CHECK(arr[5].as_integer()->flags() == value_flags::format_as_hexadecimal);
		CHECK(arr[6].as_integer()->flags() == value_flags::format_as_hexadecimal);
	}
}

TEMPLATE_TEST_CASE("arrays - native values",
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

	SECTION("push_back")
	{
		array arr;
		TestType copy = val;
		arr.push_back(copy);
		CHECK(copy == val);
		arr.push_back(std::move(copy));
		arr.push_back(toml::value<TestType>{ val });
		CHECK(arr.size() == 3u);
		for (auto& elem : arr)
		{
			CHECK(elem.is<TestType>());
			CHECK(elem.value<TestType>() == val);
		}
	}

	SECTION("emplace_back")
	{
		array arr;
		auto& explicit_elem = arr.emplace_back<TestType>(val);
		auto& deduced_elem	= arr.emplace_back(val);
		static_assert(std::is_same_v<decltype(explicit_elem), toml::value<TestType>&>);
		static_assert(std::is_same_v<decltype(deduced_elem), toml::value<TestType>&>);
		CHECK(explicit_elem == val);
		CHECK(deduced_elem == val);
		CHECK(&explicit_elem == &arr.front());
		CHECK(&deduced_elem == &arr.back());
	}

	SECTION("insert, emplace and replace")
	{
		array arr{ other };
		arr.insert(arr.cbegin(), val);
		arr.insert(arr.cend(), size_t{ 2 }, val);
		arr.emplace<TestType>(arr.cbegin() + 1, val);
		REQUIRE(arr.size() == 5u);
		CHECK(arr[0].value<TestType>() == val);
		CHECK(arr[1].value<TestType>() == val);
		CHECK(arr[2].value<TestType>() == other);
		CHECK(arr[3].value<TestType>() == val);
		CHECK(arr[4].value<TestType>() == val);

		arr.replace(arr.cbegin() + 2, val);
		CHECK(arr[2].value<TestType>() == val);
	}

	SECTION("resize")
	{
		array arr;
		arr.resize(3u, val);
		REQUIRE(arr.size() == 3u);
		for (auto& elem : arr)
			CHECK(elem.value<TestType>() == val);
	}

	SECTION("element access")
	{
		array arr{ val };
		const array& carr = arr;
		REQUIRE(arr.get_as<TestType>(0));
		CHECK(arr.get_as<TestType>(0)->get() == val);
		REQUIRE(carr.get_as<TestType>(0));
		CHECK(carr.get_as<TestType>(0)->get() == val);
		CHECK(carr.at(0).value<TestType>() == val);
		CHECK(carr.front().value<TestType>() == val);
		CHECK(carr.back().value<TestType>() == val);
	}

	SECTION("equality")
	{
		const array arr{ val, other };
		const array same{ val, other };
		const array different{ other, val };
		CHECK_SYMMETRIC_EQUAL(arr, same);
		CHECK_SYMMETRIC_INEQUAL(arr, different);

		const std::vector<TestType> vec{ val, other };
		const std::vector<TestType> reversed{ other, val };
		CHECK(arr == vec);
		CHECK(vec == arr);
		CHECK(arr != reversed);
		CHECK(reversed != arr);

		const std::initializer_list<TestType> ilist{ val, other };
		const std::initializer_list<TestType> reversed_ilist{ other, val };
		CHECK(arr == ilist);
		CHECK(ilist == arr);
		CHECK(arr != reversed_ilist);
		CHECK(reversed_ilist != arr);
	}

	SECTION("is_homogeneous")
	{
		const array arr{ val, other };
		CHECK(arr.is_homogeneous<TestType>());
		CHECK(arr.is_homogeneous(arr[0].type()));
		CHECK(!arr.is_homogeneous<table>());
	}
}

TEST_CASE("arrays - for_each")
{
	const auto arr = array{ 1, 2.0, 3, "four", false };

	SECTION("type checking")
	{
		int count	= 0;
		int ints	= 0;
		int floats	= 0;
		int numbers = 0;
		int strings = 0;
		int bools	= 0;
		arr.for_each(
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

	SECTION("early-exit (elem, index)")
	{
		int count = 0;
		arr.for_each(
			[&](const auto& elem, size_t /*idx*/) noexcept -> bool
			{
				count++;
				return !toml::is_string<decltype(elem)>;
			});
		CHECK(count == 4);
	}

	SECTION("early-exit (elem)")
	{
		int count = 0;
		arr.for_each(
			[&](const auto& elem) noexcept -> bool
			{
				count++;
				return !toml::is_string<decltype(elem)>;
			});
		CHECK(count == 4);
	}

	SECTION("early-exit (index, elem)")
	{
		int count = 0;
		arr.for_each(
			[&](size_t /*idx*/, const auto& elem) noexcept -> bool
			{
				count++;
				return !toml::is_string<decltype(elem)>;
			});
		CHECK(count == 4);
	}

#endif
}
