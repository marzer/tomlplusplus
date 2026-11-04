// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

TEST_CASE("array::for_each")
{
	toml::array arr{ 0, 1, 2, 3.0, "four", "five", 6 };

	// check lvalue propagates correctly
	static_cast<array&>(arr).for_each(
		[](auto&& elem, size_t) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<array&>(arr).for_each(
		[](size_t, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<array&>(arr).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check rvalue propagates correctly
	static_cast<array&&>(arr).for_each(
		[](auto&& elem, size_t) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<array&&>(arr).for_each(
		[](size_t, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<array&&>(arr).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check const lvalue propagates correctly
	static_cast<const array&>(arr).for_each(
		[](auto&& elem, size_t) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const array&>(arr).for_each(
		[](size_t, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const array&>(arr).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check const rvalue propagates correctly
	static_cast<const array&&>(arr).for_each(
		[](auto&& elem, size_t) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const array&&>(arr).for_each(
		[](size_t, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const array&&>(arr).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check noexcept - func(elem, i)
	{
		static constexpr auto throwing_visitor	   = [](auto&&, size_t) noexcept(false) {};
		static constexpr auto non_throwing_visitor = [](auto&&, size_t) noexcept(true) {};
		static_assert(!noexcept(static_cast<array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<array&&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&&>(arr).for_each(throwing_visitor)));
		static_assert(noexcept(static_cast<array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<array&&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&&>(arr).for_each(non_throwing_visitor)));
	}

	// check noexcept - func(i, elem)
	{
		static constexpr auto throwing_visitor	   = [](size_t, auto&&) noexcept(false) {};
		static constexpr auto non_throwing_visitor = [](size_t, auto&&) noexcept(true) {};
		static_assert(!noexcept(static_cast<array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<array&&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&&>(arr).for_each(throwing_visitor)));
		static_assert(noexcept(static_cast<array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<array&&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&&>(arr).for_each(non_throwing_visitor)));
	}

	// check noexcept - func(elem)
	{
		static constexpr auto throwing_visitor	   = [](auto&&) noexcept(false) {};
		static constexpr auto non_throwing_visitor = [](auto&&) noexcept(true) {};
		static_assert(!noexcept(static_cast<array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<array&&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&>(arr).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const array&&>(arr).for_each(throwing_visitor)));
		static_assert(noexcept(static_cast<array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<array&&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&>(arr).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const array&&>(arr).for_each(non_throwing_visitor)));
	}

	// check that the iteration actually does what it says on the box
	{
		toml::array arr2;
		arr.for_each([&](const auto& val) { arr2.push_back(val); });
		CHECK(arr == arr2);
	}

	// check that visitation works for a specific type
	{
		toml::array arr2;
		arr.for_each([&](const toml::value<int64_t>& val) { arr2.push_back(val); });
		CHECK(arr2 == toml::array{ 0, 1, 2, 6 });
	}

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

	// check that early-stopping works
	{
		toml::array arr2;
		arr.for_each(
			[&](const auto& val)
			{
				if constexpr (!toml::is_number<decltype(val)>)
					return false;
				else
				{
					arr2.push_back(val);
					return true;
				}
			});
		CHECK(arr2 == toml::array{ 0, 1, 2, 3.0 });
	}

#endif
}

TEST_CASE("table::for_each")
{
	table tbl{ { "zero", 0 },	   //
			   { "one", 1 },	   //
			   { "two", 2 },	   //
			   { "three", 3.0 },   //
			   { "four", "four" }, //
			   { "five", "five" }, //
			   { "six", 6 } };

	// check lvalue propagates correctly
	static_cast<table&>(tbl).for_each(
		[](const toml::key&, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<table&>(tbl).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check rvalue propagates correctly
	static_cast<table&&>(tbl).for_each(
		[](const toml::key&, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<table&&>(tbl).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(!std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check const lvalue propagates correctly
	static_cast<const table&>(tbl).for_each(
		[](const toml::key&, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const table&>(tbl).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_lvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check const rvalue propagates correctly
	static_cast<const table&&>(tbl).for_each(
		[](const toml::key&, auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});
	static_cast<const table&&>(tbl).for_each(
		[](auto&& elem) noexcept
		{
			using elem_ref_type = decltype(elem);
			static_assert(std::is_rvalue_reference_v<elem_ref_type>);

			using elem_type = std::remove_reference_t<elem_ref_type>;
			static_assert(std::is_const_v<elem_type>);
			static_assert(!std::is_volatile_v<elem_type>);
		});

	// check noexcept - func(key, value)
	{
		static constexpr auto throwing_visitor	   = [](const toml::key&, auto&&) noexcept(false) {};
		static constexpr auto non_throwing_visitor = [](const toml::key&, auto&&) noexcept(true) {};
		static_assert(!noexcept(static_cast<table&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<table&&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const table&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const table&&>(tbl).for_each(throwing_visitor)));
		static_assert(noexcept(static_cast<table&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<table&&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const table&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const table&&>(tbl).for_each(non_throwing_visitor)));
	}

	// check noexcept - func(value)
	{
		static constexpr auto throwing_visitor	   = [](auto&&) noexcept(false) {};
		static constexpr auto non_throwing_visitor = [](auto&&) noexcept(true) {};
		static_assert(!noexcept(static_cast<table&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<table&&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const table&>(tbl).for_each(throwing_visitor)));
		static_assert(!noexcept(static_cast<const table&&>(tbl).for_each(throwing_visitor)));
		static_assert(noexcept(static_cast<table&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<table&&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const table&>(tbl).for_each(non_throwing_visitor)));
		static_assert(noexcept(static_cast<const table&&>(tbl).for_each(non_throwing_visitor)));
	}

	// check that the iteration actually does what it says on the box
	{
		toml::table tbl2;
		tbl.for_each([&](auto&& key, auto&& val) { tbl2.insert_or_assign(key, val); });
		CHECK(tbl == tbl2);
	}

	// check that visitation works for a specific type
	{
		toml::table tbl2;
		tbl.for_each([&](auto&& key, const toml::value<int64_t>& val) { tbl2.insert_or_assign(key, val); });
		CHECK(tbl2
			  == table{ { "zero", 0 }, //
						{ "one", 1 },  //
						{ "two", 2 },  //
						{ "six", 6 } });
	}

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

	// check that early-stopping works
	{
		toml::table tbl2;
		size_t added{};
		tbl.for_each(
			[&](auto&& key, const auto& val)
			{
				tbl2.insert_or_assign(key, val);
				added++;
				return added < 3u;
			});
		CHECK(tbl2.size() == 3u);
	}

#endif
}

TEST_CASE("arrays - for_each visits every element in order")
{
	array arr{ 1, "two", 3.0, table{}, array{ 5 } };
	const std::vector<const node*> expected_elems{ &arr[0], &arr[1], &arr[2], &arr[3], &arr[4] };
	const std::vector<size_t> expected_indices{ 0u, 1u, 2u, 3u, 4u };

	std::vector<const node*> elems;
	std::vector<size_t> indices;
	const auto elem_and_index = [&](auto&& elem, size_t i) noexcept
	{
		elems.push_back(&elem);
		indices.push_back(i);
	};
	const auto index_and_elem = [&](size_t i, auto&& elem) noexcept
	{
		elems.push_back(&elem);
		indices.push_back(i);
	};
	const auto elem_only = [&](auto&& elem) noexcept { elems.push_back(&elem); };

	const auto visit_all = [&](auto&& target)
	{
		using target_type = decltype(target);

		elems.clear();
		indices.clear();
		static_cast<target_type>(target).for_each(elem_and_index);
		CHECK(elems == expected_elems);
		CHECK(indices == expected_indices);

		elems.clear();
		indices.clear();
		static_cast<target_type>(target).for_each(index_and_elem);
		CHECK(elems == expected_elems);
		CHECK(indices == expected_indices);

		elems.clear();
		static_cast<target_type>(target).for_each(elem_only);
		CHECK(elems == expected_elems);
	};

	SECTION("lvalue")
	{
		visit_all(arr);
	}

	SECTION("const lvalue")
	{
		visit_all(std::as_const(arr));
	}

	SECTION("rvalue")
	{
		visit_all(std::move(arr));
	}

	SECTION("const rvalue")
	{
		visit_all(std::move(std::as_const(arr)));
	}
}

TEST_CASE("arrays - for_each can modify the elements of a non-const array")
{
	array arr{ 1, "two", 3 };
	arr.for_each([](toml::value<int64_t>& elem) noexcept { *elem *= 10; });
	CHECK(arr == array{ 10, "two", 30 });

	arr.for_each([](size_t i, toml::value<std::string>& elem) { elem = std::to_string(i); });
	CHECK(arr == array{ 10, "1", 30 });
}

TEST_CASE("arrays - for_each on an rvalue array passes elements that can be moved from")
{
	array arr{ 1, array{ 2, 3 }, table{ { "a", 4 } } };
	std::vector<const node*> visited;
	array taken_array;
	table taken_table;
	std::move(arr).for_each(
		[&](auto&& elem)
		{
			using elem_type = std::remove_reference_t<decltype(elem)>;
			static_assert(std::is_rvalue_reference_v<decltype(elem)>);
			visited.push_back(&elem);
			if constexpr (std::is_same_v<elem_type, array>)
				taken_array = std::move(elem);
			else if constexpr (std::is_same_v<elem_type, table>)
				taken_table = std::move(elem);
		});
	CHECK(taken_array == array{ 2, 3 });
	CHECK(taken_table == table{ { "a", 4 } });
	CHECK(visited == std::vector<const node*>{ arr.get(0), arr.get(1), arr.get(2) });
}

TEST_CASE("arrays - for_each returns the array")
{
	array arr{ 1 };
	const auto visitor = [](auto&&) noexcept {};
	CHECK(&arr.for_each(visitor) == &arr);
	CHECK(&std::as_const(arr).for_each(visitor) == &arr);

	array&& moved = std::move(arr).for_each(visitor);
	CHECK(&moved == &arr);

	const array&& const_moved = std::move(std::as_const(arr)).for_each(visitor);
	CHECK(&const_moved == &arr);
}

TEST_CASE("arrays - for_each on an empty array never calls the visitor")
{
	array arr;
	bool called = false;
	arr.for_each([&](auto&&) noexcept { called = true; });
	std::as_const(arr).for_each([&](size_t, auto&&) noexcept { called = true; });
	CHECK(!called);
}

TEST_CASE("arrays - for_each noexcept with visitors for specific types")
{
	array arr{ 1, "two", 3 };
	const auto nothrow_ints	 = [](const toml::value<int64_t>&) noexcept {};
	const auto throwing_ints = [](const toml::value<int64_t>&) noexcept(false) {};
	static_assert(noexcept(arr.for_each(nothrow_ints)));
	static_assert(noexcept(std::as_const(arr).for_each(nothrow_ints)));
	static_assert(noexcept(std::move(arr).for_each(nothrow_ints)));
	static_assert(noexcept(std::move(std::as_const(arr)).for_each(nothrow_ints)));
	static_assert(!noexcept(arr.for_each(throwing_ints)));
	static_assert(!noexcept(std::as_const(arr).for_each(throwing_ints)));
	static_assert(!noexcept(std::move(arr).for_each(throwing_ints)));
	static_assert(!noexcept(std::move(std::as_const(arr)).for_each(throwing_ints)));
}

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

TEST_CASE("arrays - for_each stops when the visitor returns false")
{
	array arr{ 1, 2, 3, 4 };
	int visits{};

	SECTION("convertible to bool")
	{
		std::move(arr).for_each([&](size_t, auto&&) noexcept { return ++visits < 3 ? 1 : 0; });
		CHECK(visits == 3);
	}

	SECTION("not convertible to bool")
	{
		std::as_const(arr).for_each(
			[&](auto&&, size_t)
			{
				++visits;
				return std::string{};
			});
		CHECK(visits == 4);
	}
}

#endif

TEST_CASE("tables - for_each visits every key-value pair in order")
{
	table tbl{ { "c", 3.0 }, { "a", 1 }, { "e", array{ 5 } }, { "b", "two" }, { "d", table{} } };
	const std::vector<std::string> expected_keys{ "a", "b", "c", "d", "e" };
	std::vector<const node*> expected_values;
	for (const auto& k : expected_keys)
		expected_values.push_back(tbl.get(k));

	std::vector<std::string> keys;
	std::vector<const node*> values;
	const auto key_and_value = [&](const toml::key& k, auto&& v) noexcept
	{
		keys.emplace_back(k.str());
		values.push_back(&v);
	};
	const auto value_only = [&](auto&& v) noexcept { values.push_back(&v); };

	const auto visit_all = [&](auto&& target)
	{
		using target_type = decltype(target);

		keys.clear();
		values.clear();
		static_cast<target_type>(target).for_each(key_and_value);
		CHECK(keys == expected_keys);
		CHECK(values == expected_values);

		values.clear();
		static_cast<target_type>(target).for_each(value_only);
		CHECK(values == expected_values);
	};

	SECTION("lvalue")
	{
		visit_all(tbl);
	}

	SECTION("const lvalue")
	{
		visit_all(std::as_const(tbl));
	}

	SECTION("rvalue")
	{
		visit_all(std::move(tbl));
	}

	SECTION("const rvalue")
	{
		visit_all(std::move(std::as_const(tbl)));
	}
}

TEST_CASE("tables - for_each can modify the values of a non-const table")
{
	table tbl{ { "a", 1 }, { "b", "two" }, { "c", 3 } };
	tbl.for_each([](toml::value<int64_t>& val) noexcept { *val *= 10; });
	CHECK(tbl == table{ { "a", 10 }, { "b", "two" }, { "c", 30 } });

	tbl.for_each([](const toml::key& key, toml::value<std::string>& val) { val = key.str(); });
	CHECK(tbl == table{ { "a", 10 }, { "b", "b" }, { "c", 30 } });
}

TEST_CASE("tables - for_each on an rvalue table passes values that can be moved from")
{
	table tbl{ { "a", 1 }, { "b", array{ 2, 3 } }, { "c", table{ { "d", 4 } } } };
	std::vector<const node*> visited;
	array taken_array;
	table taken_table;
	std::move(tbl).for_each(
		[&](const toml::key&, auto&& val)
		{
			using value_type = std::remove_reference_t<decltype(val)>;
			static_assert(std::is_rvalue_reference_v<decltype(val)>);
			visited.push_back(&val);
			if constexpr (std::is_same_v<value_type, array>)
				taken_array = std::move(val);
			else if constexpr (std::is_same_v<value_type, table>)
				taken_table = std::move(val);
		});
	CHECK(taken_array == array{ 2, 3 });
	CHECK(taken_table == table{ { "d", 4 } });
	CHECK(visited == std::vector<const node*>{ tbl.get("a"), tbl.get("b"), tbl.get("c") });
}

TEST_CASE("tables - for_each returns the table")
{
	table tbl{ { "a", 1 } };
	const auto visitor = [](auto&&) noexcept {};
	CHECK(&tbl.for_each(visitor) == &tbl);
	CHECK(&std::as_const(tbl).for_each(visitor) == &tbl);

	table&& moved = std::move(tbl).for_each(visitor);
	CHECK(&moved == &tbl);

	const table&& const_moved = std::move(std::as_const(tbl)).for_each(visitor);
	CHECK(&const_moved == &tbl);
}

TEST_CASE("tables - for_each on an empty table never calls the visitor")
{
	table tbl;
	bool called = false;
	tbl.for_each([&](auto&&) noexcept { called = true; });
	std::as_const(tbl).for_each([&](const toml::key&, auto&&) noexcept { called = true; });
	CHECK(!called);
}

TEST_CASE("tables - for_each noexcept with visitors for specific types")
{
	table tbl{ { "a", 1 }, { "b", "two" }, { "c", 3 } };
	const auto nothrow_ints	 = [](const toml::key&, const toml::value<int64_t>&) noexcept {};
	const auto throwing_ints = [](const toml::key&, const toml::value<int64_t>&) noexcept(false) {};
	static_assert(noexcept(tbl.for_each(nothrow_ints)));
	static_assert(noexcept(std::as_const(tbl).for_each(nothrow_ints)));
	static_assert(noexcept(std::move(tbl).for_each(nothrow_ints)));
	static_assert(noexcept(std::move(std::as_const(tbl)).for_each(nothrow_ints)));
	static_assert(!noexcept(tbl.for_each(throwing_ints)));
	static_assert(!noexcept(std::as_const(tbl).for_each(throwing_ints)));
	static_assert(!noexcept(std::move(tbl).for_each(throwing_ints)));
	static_assert(!noexcept(std::move(std::as_const(tbl)).for_each(throwing_ints)));
}

#if !TOML_RETURN_BOOL_FROM_FOR_EACH_BROKEN

TEST_CASE("tables - for_each stops when the visitor returns false")
{
	table tbl{ { "a", 1 }, { "b", 2 }, { "c", 3 }, { "d", 4 } };
	int visits{};

	SECTION("convertible to bool")
	{
		std::move(tbl).for_each([&](const toml::key&, auto&&) noexcept { return ++visits < 3 ? 1 : 0; });
		CHECK(visits == 3);
	}

	SECTION("not convertible to bool")
	{
		std::as_const(tbl).for_each(
			[&](const toml::key&, auto&&)
			{
				++visits;
				return std::string{};
			});
		CHECK(visits == 4);
	}
}

#endif
