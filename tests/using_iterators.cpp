#include "tests.hpp"

TOML_DISABLE_WARNINGS;
#include <algorithm>
TOML_ENABLE_WARNINGS;

TEST_CASE("using iterators")
{
	constexpr auto data = R"(array=[1,"Foo",true]
string="Bar"
number=5)"sv;
	parsing_should_succeed(
		FILE_LINE_ARGS,
		data,
		[](auto&& tbl)
		{
			const auto tbl_begin = tbl.begin();
			const auto tbl_end	 = tbl.end();

			auto count_table_lambda = [tbl_begin, tbl_end](node_type type) noexcept
			{
				return std::count_if(tbl_begin,
									 tbl_end,
									 [type](const auto& pair) noexcept { return pair.second.type() == type; });
			};

			CHECK(std::distance(tbl_begin, tbl_end) == 3);
			CHECK(count_table_lambda(node_type::table) == 0);
			CHECK(count_table_lambda(node_type::integer) == 1);
			CHECK(count_table_lambda(node_type::string) == 1);
			CHECK(std::next(tbl_begin, 3) == tbl_end);
			CHECK(std::prev(tbl_end, 3) == tbl_begin);

			const auto arr_iter =
				std::find_if(tbl_begin, tbl_end, [](const auto& pair) noexcept { return pair.second.is_array(); });

			REQUIRE(arr_iter != tbl_end);
			CHECK(arr_iter->first == "array"sv);
			const auto& arr		 = arr_iter->second.as_array();
			const auto arr_begin = arr->begin();
			const auto arr_end	 = arr->end();

			auto count_array_lambda = [arr_begin, arr_end](node_type type) noexcept
			{
				return std::count_if(arr_begin,
									 arr_end,
									 [type](const auto& node) noexcept { return node.type() == type; });
			};

			CHECK(std::distance(arr_begin, arr_end) == 3);
			CHECK(count_array_lambda(node_type::table) == 0);
			CHECK(count_array_lambda(node_type::integer) == 1);
			CHECK(count_array_lambda(node_type::string) == 1);
			CHECK(std::next(arr_begin, 2) != arr_end);
		});
}

TEST_CASE("tables - iterators")
{
	table tbl{ { "b", "two" }, { "c", 3.0 }, { "a", 1 } };
	const table& ctbl = tbl;

	SECTION("traits")
	{
		static_assert(
			std::is_same_v<std::iterator_traits<table::iterator>::iterator_category, std::bidirectional_iterator_tag>);
		static_assert(std::is_same_v<std::iterator_traits<table::const_iterator>::iterator_category,
									 std::bidirectional_iterator_tag>);
		static_assert(std::is_convertible_v<table::iterator, table::const_iterator>);
		static_assert(!std::is_convertible_v<table::const_iterator, table::iterator>);
		static_assert(std::is_same_v<decltype(tbl.begin()->first), const toml::key&>);
		static_assert(std::is_same_v<decltype(tbl.begin()->second), node&>);
		static_assert(std::is_same_v<decltype(ctbl.begin()->first), const toml::key&>);
		static_assert(std::is_same_v<decltype(ctbl.begin()->second), const node&>);
	}

	SECTION("key order")
	{
		std::vector<std::string> keys;
		for (auto it = tbl.begin(); it != tbl.end(); it++)
			keys.emplace_back(it->first.str());
		CHECK(keys == std::vector<std::string>{ "a", "b", "c" });

		keys.clear();
		for (auto it = ctbl.cbegin(); it != ctbl.cend(); ++it)
			keys.emplace_back((*it).first.str());
		CHECK(keys == std::vector<std::string>{ "a", "b", "c" });
	}

	SECTION("increment and decrement")
	{
		auto it				= tbl.begin();
		const auto post_inc = it++;
		CHECK(post_inc == tbl.begin());
		CHECK(it->first == "b"sv);

		auto& pre_inc = ++it;
		CHECK(&pre_inc == &it);
		CHECK(it->first == "c"sv);
		CHECK(++it == tbl.end());

		const auto post_dec = it--;
		CHECK(post_dec == tbl.end());
		CHECK(it->first == "c"sv);

		auto& pre_dec = --it;
		CHECK(&pre_dec == &it);
		CHECK(it->first == "b"sv);
		CHECK(--it == tbl.begin());
	}

	SECTION("dereferencing")
	{
		const auto it = tbl.begin();
		auto& kvp	  = *it;
		CHECK(kvp.first == "a"sv);
		CHECK(&kvp.second == tbl.get("a"));
		CHECK(&it->first == &kvp.first);
		CHECK(&it->second == &kvp.second);

		const auto cit = ctbl.begin();
		CHECK((*cit).first == "a"sv);
		CHECK(&cit->second == tbl.get("a"));
	}

	SECTION("structured bindings")
	{
		std::vector<std::string> keys;
		std::vector<const node*> values;
		for (auto&& [k, v] : ctbl)
		{
			keys.emplace_back(k.str());
			values.push_back(&v);
		}
		CHECK(keys == std::vector<std::string>{ "a", "b", "c" });
		CHECK(values == std::vector<const node*>{ tbl.get("a"), tbl.get("b"), tbl.get("c") });
	}

	SECTION("modifying values")
	{
		for (auto&& [k, v] : tbl)
		{
			if (auto i = v.as_integer())
				i->get() = 10;
		}
		CHECK(tbl == table{ { "a", 10 }, { "b", "two" }, { "c", 3.0 } });

		*tbl.begin()->second.as_integer() = 20;
		CHECK(tbl["a"] == 20);
	}

	SECTION("copies and assignment")
	{
		auto it = tbl.begin();
		CHECK(it->first == "a"sv);

		auto copy = it;
		CHECK(copy->first == "a"sv);
		++copy;
		CHECK(copy->first == "b"sv);
		CHECK(it->first == "a"sv);

		it = copy;
		CHECK(it->first == "b"sv);

		table::iterator assigned;
		assigned = tbl.end();
		CHECK(assigned == tbl.end());
	}

	SECTION("conversion to const_iterator")
	{
		table::const_iterator cit = tbl.begin();
		CHECK(cit == ctbl.begin());
		CHECK(cit == tbl.begin());
		CHECK(cit->first == "a"sv);

		cit = tbl.end();
		CHECK(cit == ctbl.end());
	}

	SECTION("an empty table")
	{
		table empty;
		CHECK(empty.begin() == empty.end());
		CHECK(empty.cbegin() == empty.cend());
		CHECK(std::as_const(empty).begin() == std::as_const(empty).end());
	}
}

TEST_CASE("arrays - iterators")
{
	array arr{ 1, "two", 3.0, 4 };
	const array& carr = arr;

	SECTION("traits")
	{
		static_assert(
			std::is_same_v<std::iterator_traits<array::iterator>::iterator_category, std::random_access_iterator_tag>);
		static_assert(std::is_same_v<std::iterator_traits<array::const_iterator>::iterator_category,
									 std::random_access_iterator_tag>);
		static_assert(std::is_convertible_v<array::iterator, array::const_iterator>);
		static_assert(!std::is_convertible_v<array::const_iterator, array::iterator>);
		static_assert(std::is_same_v<decltype(*arr.begin()), node&>);
		static_assert(std::is_same_v<decltype(*carr.begin()), const node&>);
		static_assert(std::is_same_v<decltype(arr.begin()[0]), node&>);
		static_assert(std::is_same_v<decltype(carr.begin()[0]), const node&>);
	}

	SECTION("increment and decrement")
	{
		auto it				= arr.begin();
		const auto post_inc = it++;
		CHECK(post_inc == arr.begin());
		CHECK(&*it == &arr[1]);

		auto& pre_inc = ++it;
		CHECK(&pre_inc == &it);
		CHECK(&*it == &arr[2]);

		const auto post_dec = it--;
		CHECK(&*post_dec == &arr[2]);
		CHECK(&*it == &arr[1]);

		auto& pre_dec = --it;
		CHECK(&pre_dec == &it);
		CHECK(it == arr.begin());
	}

	SECTION("arithmetic")
	{
		auto it = arr.begin();
		CHECK(&*(it + 2) == &arr[2]);
		CHECK(&*(2 + it) == &arr[2]);
		CHECK(&*(arr.end() - 1) == &arr[3]);
		CHECK(arr.end() - it == 4);
		CHECK(it - arr.end() == -4);

		it += 3;
		CHECK(&*it == &arr[3]);
		it -= 2;
		CHECK(&*it == &arr[1]);
		CHECK(it->is_string());
		CHECK(&it[1] == &arr[2]);
		CHECK(&it[-1] == &arr[0]);

		auto cit = carr.begin();
		cit += 2;
		CHECK(&*cit == &arr[2]);
		CHECK(cit - carr.begin() == 2);
		CHECK(&cit[-2] == &arr[0]);
	}

	SECTION("comparison")
	{
		const auto first  = arr.begin();
		const auto second = arr.begin() + 1;
		CHECK(first == arr.begin());
		CHECK(first != second);
		CHECK(first < second);
		CHECK(!(second < first));
		CHECK(first <= second);
		CHECK(first <= first);
		CHECK(!(second <= first));
		CHECK(second > first);
		CHECK(!(first > second));
		CHECK(second >= first);
		CHECK(second >= second);
		CHECK(!(first >= second));
	}

	SECTION("conversion to const_iterator")
	{
		array::const_iterator cit = arr.begin();
		CHECK(cit == carr.begin());
		CHECK(cit == arr.cbegin());
		CHECK(&*cit == &arr[0]);

		cit = arr.end();
		CHECK(cit == carr.end());
		CHECK(carr.end() - carr.begin() == 4);
	}

	SECTION("modifying elements")
	{
		for (auto& elem : arr)
		{
			if (auto i = elem.as_integer())
				i->get() *= 10;
		}
		CHECK(arr == array{ 10, "two", 3.0, 40 });
	}

	SECTION("standard algorithms")
	{
		CHECK(std::distance(arr.begin(), arr.end()) == 4);
		CHECK(std::count_if(carr.begin(), carr.end(), [](const node& n) { return n.is_number(); }) == 3);

		std::vector<const node*> reversed;
		for (auto it = std::make_reverse_iterator(carr.end()); it != std::make_reverse_iterator(carr.begin()); it++)
			reversed.push_back(&*it);
		CHECK(reversed == std::vector<const node*>{ &arr[3], &arr[2], &arr[1], &arr[0] });
	}

	SECTION("an empty array")
	{
		array empty;
		CHECK(empty.begin() == empty.end());
		CHECK(empty.cbegin() == empty.cend());
		CHECK(empty.end() - empty.begin() == 0);
	}
}
