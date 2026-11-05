// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	template <typename... T>
	struct type_list
	{};

	using node_types = type_list<table, array, std::string, int64_t, double, bool, date, toml::time, date_time>;

	template <typename... T, typename Func>
	void for_each_type(type_list<T...>, Func&& func)
	{
		(func(type_tag<T>{}), ...);
	}

	constexpr date sample_date{ 2024, 1, 2 };
	constexpr toml::time sample_time{ 3, 4, 5 };
	constexpr date_time sample_date_time{ sample_date, sample_time };

	struct sample_nodes
	{
		table tbl{ { "a"sv, 1 } };
		array arr{ 1, 2 };
		value<std::string> str{ "hello"sv };
		value<int64_t> integer{ 42 };
		value<double> floating_point{ 0.5 };
		value<bool> boolean{ true };
		value<date> dt{ sample_date };
		value<toml::time> tm{ sample_time };
		value<date_time> dtm{ sample_date_time };
	};

	template <typename T>
	impl::wrap_node<T>& sample_of(sample_nodes& s) noexcept
	{
		if constexpr (std::is_same_v<T, table>)
			return s.tbl;
		else if constexpr (std::is_same_v<T, array>)
			return s.arr;
		else if constexpr (std::is_same_v<T, std::string>)
			return s.str;
		else if constexpr (std::is_same_v<T, int64_t>)
			return s.integer;
		else if constexpr (std::is_same_v<T, double>)
			return s.floating_point;
		else if constexpr (std::is_same_v<T, bool>)
			return s.boolean;
		else if constexpr (std::is_same_v<T, date>)
			return s.dt;
		else if constexpr (std::is_same_v<T, toml::time>)
			return s.tm;
		else
			return s.dtm;
	}

	template <typename T>
	std::string printed(const T& obj)
	{
		std::ostringstream ss;
		ss << obj;
		return std::move(ss).str();
	}

	template <typename T>
	const volatile void* underlying_of(sample_nodes& s) noexcept
	{
		auto& n = sample_of<T>(s);
		if constexpr (is_container<T>)
			return &n;
		else
			return &n.get();
	}

	template <typename T, typename Node>
	void check_ref_qualifiers(Node&& n, const volatile void* expected)
	{
		const auto check = [=](auto&& ref)
		{
			const bool same_object = static_cast<const volatile void*>(std::addressof(ref)) == expected;
			CHECK(same_object);
		};
		check(static_cast<Node&&>(n).template ref<T>());
		check(static_cast<Node&&>(n).template ref<const T>());
		check(static_cast<Node&&>(n).template ref<volatile T>());
		check(static_cast<Node&&>(n).template ref<const volatile T>());
		check(static_cast<Node&&>(n).template ref<T&>());
		check(static_cast<Node&&>(n).template ref<const T&>());
		check(static_cast<Node&&>(n).template ref<volatile T&>());
		check(static_cast<Node&&>(n).template ref<const volatile T&>());
		check(static_cast<Node&&>(n).template ref<T&&>());
		check(static_cast<Node&&>(n).template ref<const T&&>());
		check(static_cast<Node&&>(n).template ref<volatile T&&>());
		check(static_cast<Node&&>(n).template ref<const volatile T&&>());
	}

	template <typename Receiver, typename Make, typename Expected>
	void check_value_or_categories(const Receiver& r, const Make& make, const Expected& expected)
	{
		CHECK(r.value_or(make()) == expected);
		{
			auto def = make();
			CHECK(r.value_or(def) == expected);
		}
		{
			const auto def = make();
			CHECK(r.value_or(def) == expected);
			CHECK(r.value_or(std::move(def)) == expected);
		}
	}

	template <typename Make, typename Expected>
	void check_value_or(node& n, const Make& make, const Expected& expected)
	{
		check_value_or_categories(std::as_const(n), make, expected);
		check_value_or_categories(node_view<node>{ n }, make, expected);
		check_value_or_categories(node_view<const node>{ n }, make, expected);
		check_value_or_categories(node_view<node>{}, make, make());
		check_value_or_categories(node_view<const node>{}, make, make());
	}

	template <typename Receiver, typename Array>
	void check_value_or_array(const Receiver& r, Array& arr, const void* expected)
	{
		const void* from_lvalue = r.value_or(arr);
		const void* from_rvalue = r.value_or(std::move(arr));
		CHECK(from_lvalue == expected);
		CHECK(from_rvalue == expected);
	}

	template <typename Array>
	void check_value_or_arrays(value<std::string>& str, node& non_string, Array& arr)
	{
		const void* str_data	= str.get().c_str();
		const void* arr_data	= &arr[0];
		const node& const_str	= str;
		const node& const_other = non_string;
		check_value_or_array(const_str, arr, str_data);
		check_value_or_array(const_other, arr, arr_data);
		check_value_or_array(node_view<node>{ str }, arr, str_data);
		check_value_or_array(node_view<const node>{ str }, arr, str_data);
		check_value_or_array(node_view<node>{ non_string }, arr, arr_data);
		check_value_or_array(node_view<node>{}, arr, arr_data);
		check_value_or_array(node_view<const node>{}, arr, arr_data);
	}
}

TEST_CASE("nodes - is() and as() identify the concrete node type")
{
	sample_nodes s;
	for_each_type(node_types{},
				  [&](auto actual_tag)
				  {
					  using actual	 = typename decltype(actual_tag)::type;
					  auto& concrete = sample_of<actual>(s);
					  node& n		 = concrete;
					  INFO("node type: " << n.type());

					  for_each_type(node_types{},
									[&](auto tested_tag)
									{
										using tested				   = typename decltype(tested_tag)::type;
										static constexpr bool expected = std::is_same_v<actual, tested>;
										INFO("tested type: " << impl::node_type_of<tested>);

										CHECK(n.is<tested>() == expected);
										CHECK(n.is<const tested>() == expected);
										CHECK(n.is<volatile tested>() == expected);
										CHECK(n.is<const volatile tested>() == expected);
										CHECK(n.is<tested&>() == expected);
										CHECK(n.is<impl::wrap_node<tested>>() == expected);

										auto* ptr		 = n.as<tested>();
										const auto* cptr = std::as_const(n).as<tested>();
										CHECK((ptr != nullptr) == expected);
										CHECK((cptr != nullptr) == expected);
										if constexpr (expected)
										{
											CHECK(ptr == &concrete);
											CHECK(cptr == &concrete);
											CHECK(n.as<impl::wrap_node<tested>>() == &concrete);
										}
									});
				  });
}

TEST_CASE("nodes - ref() refers to the underlying object for every qualifier")
{
	sample_nodes s;
	for_each_type(node_types{},
				  [&](auto tag)
				  {
					  using T = typename decltype(tag)::type;
					  node& n = sample_of<T>(s);
					  INFO("node type: " << n.type());

					  const auto expected = underlying_of<T>(s);
					  check_ref_qualifiers<T>(n, expected);
					  check_ref_qualifiers<T>(std::move(n), expected);
					  check_ref_qualifiers<T>(std::as_const(n), expected);
					  check_ref_qualifiers<T>(std::move(std::as_const(n)), expected);
					  check_ref_qualifiers<T>(node_view<node>{ n }, expected);
					  check_ref_qualifiers<T>(node_view<const node>{ n }, expected);
				  });

	SECTION("writes through a reference are visible in the node")
	{
		node& n			 = s.integer;
		n.ref<int64_t>() = 7;
		CHECK(*s.integer == 7);
		node_view<node>{ s.str }.ref<std::string>() = "changed";
		CHECK(*s.str == "changed"sv);
	}
}

TEST_CASE("nodes - visit() passes the concrete node to the visitor")
{
	sample_nodes s;
	for_each_type(node_types{},
				  [&](auto tag)
				  {
					  using T		 = typename decltype(tag)::type;
					  auto& concrete = sample_of<T>(s);
					  node& n		 = concrete;
					  INFO("node type: " << n.type());

					  const auto address_of = [](auto&& visited) noexcept -> const void* { return &visited; };
					  CHECK(n.visit(address_of) == &concrete);
					  CHECK(std::move(n).visit(address_of) == &concrete);
					  CHECK(std::as_const(n).visit(address_of) == &concrete);
					  CHECK(std::move(std::as_const(n)).visit(address_of) == &concrete);

					  const auto type_of = [](auto&& visited) noexcept
					  { return impl::node_type_of<decltype(visited)>; };
					  CHECK(n.visit(type_of) == impl::node_type_of<T>);
					  CHECK(node_view<node>{ n }.visit(type_of) == impl::node_type_of<T>);
					  CHECK(node_view<const node>{ n }.visit(type_of) == impl::node_type_of<T>);
				  });
}

TEST_CASE("nodes - visit() with a non-exhaustive visitor")
{
	sample_nodes s;
	const auto integer_only = [](const value<int64_t>& v) noexcept { return *v + 1; };

	SECTION("returns the default-constructed result for other node types")
	{
		for_each_type(node_types{},
					  [&](auto tag)
					  {
						  using T = typename decltype(tag)::type;
						  node& n = sample_of<T>(s);
						  INFO("node type: " << n.type());

						  const int64_t expected = std::is_same_v<T, int64_t> ? 43 : 0;
						  CHECK(n.visit(integer_only) == expected);
						  CHECK(std::move(n).visit(integer_only) == expected);
						  CHECK(std::as_const(n).visit(integer_only) == expected);
						  CHECK(std::move(std::as_const(n)).visit(integer_only) == expected);
					  });
	}

	SECTION("void visitors are only invoked for matching node types")
	{
		int calls = {};
		for_each_type(node_types{},
					  [&](auto tag)
					  {
						  node& n = sample_of<typename decltype(tag)::type>(s);
						  n.visit(
							  [&](value<std::string>& str) noexcept
							  {
								  calls++;
								  str = "visited"sv;
							  });
					  });
		CHECK(calls == 1);
		CHECK(*s.str == "visited"sv);
	}
}

TEST_CASE("nodes - value_exact() only returns values of the exact type")
{
	sample_nodes s;
	for_each_type(node_types{},
				  [&](auto actual_tag)
				  {
					  using actual = typename decltype(actual_tag)::type;
					  node& n	   = sample_of<actual>(s);
					  INFO("node type: " << n.type());

					  CHECK(n.value_exact<std::string>().has_value() == std::is_same_v<actual, std::string>);
					  CHECK(n.value_exact<std::string_view>().has_value() == std::is_same_v<actual, std::string>);
					  CHECK(n.value_exact<const char*>().has_value() == std::is_same_v<actual, std::string>);
					  CHECK(n.value_exact<int64_t>().has_value() == std::is_same_v<actual, int64_t>);
					  CHECK(n.value_exact<double>().has_value() == std::is_same_v<actual, double>);
					  CHECK(n.value_exact<bool>().has_value() == std::is_same_v<actual, bool>);
					  CHECK(n.value_exact<date>().has_value() == std::is_same_v<actual, date>);
					  CHECK(n.value_exact<toml::time>().has_value() == std::is_same_v<actual, toml::time>);
					  CHECK(n.value_exact<date_time>().has_value() == std::is_same_v<actual, date_time>);
				  });

	CHECK(s.str.value_exact<std::string>() == "hello"s);
	CHECK(s.str.value_exact<std::string_view>() == "hello"sv);
	CHECK(s.str.value_exact<const char*>() == s.str.get().c_str());
	CHECK(s.integer.value_exact<int64_t>() == 42);
	CHECK(s.floating_point.value_exact<double>() == 0.5);
	CHECK(s.boolean.value_exact<bool>() == true);
	CHECK(s.dt.value_exact<date>() == sample_date);
	CHECK(s.tm.value_exact<toml::time>() == sample_time);
	CHECK(s.dtm.value_exact<date_time>() == sample_date_time);

#if TOML_HAS_CHAR8
	CHECK(s.str.value_exact<std::u8string>() == u8"hello"s);
	CHECK(s.str.value_exact<std::u8string_view>() == u8"hello"sv);
	CHECK(s.str.value_exact<const char8_t*>() == reinterpret_cast<const char8_t*>(s.str.get().c_str()));
	CHECK(!s.integer.value_exact<std::u8string>());
	CHECK(!s.integer.value_exact<std::u8string_view>());
	CHECK(!s.integer.value_exact<const char8_t*>());
#endif

	SECTION("no conversions between value types")
	{
		CHECK(!s.integer.value_exact<double>());
		CHECK(!s.floating_point.value_exact<int64_t>());
		CHECK(!s.boolean.value_exact<int64_t>());
		CHECK(!s.integer.value_exact<bool>());
	}

	SECTION("types that can losslessly represent the native type")
	{
#ifdef TOML_INT128
		CHECK((s.integer.value_exact<TOML_INT128>() == TOML_INT128{ 42 }));
		CHECK(!s.floating_point.value_exact<TOML_INT128>());
#endif
		CHECK(s.floating_point.value_exact<long double>() == 0.5L);
		CHECK(!s.integer.value_exact<long double>());
	}
}

TEST_CASE("nodes - value() conversions to wider and narrower types")
{
	const value<int64_t> integer_value{ 42 };
	const value<int64_t> beyond_float_value{ (int64_t{ 1 } << 24) + 1 };
	const value<double> whole_value{ 3.0 };
	const value<double> fractional_value{ 3.5 };
	const value<double> huge_value{ 1e300 };
	const value<bool> boolean_value{ true };
	const value<std::string> str_value{ "hello"sv };
	const value<date> dt_value{ sample_date };
	const table tbl;
	const array arr;

	const node& integer		 = integer_value;
	const node& beyond_float = beyond_float_value;
	const node& whole		 = whole_value;
	const node& fractional	 = fractional_value;
	const node& huge		 = huge_value;
	const node& boolean		 = boolean_value;
	const node& str			 = str_value;
	const node& dt			 = dt_value;

	SECTION("long long")
	{
		CHECK(integer.value<long long>() == 42LL);
		CHECK(whole.value<long long>() == 3LL);
		CHECK(!fractional.value<long long>());
		CHECK(boolean.value<long long>() == 1LL);
		CHECK(!str.value<long long>());
	}

#ifdef TOML_INT128
	SECTION("128-bit integers")
	{
		const value<int64_t> negative_value{ -42 };
		const node& negative = negative_value;
		CHECK((integer.value<TOML_INT128>() == TOML_INT128{ 42 }));
		CHECK((negative.value<TOML_INT128>() == TOML_INT128{ -42 }));
		CHECK((whole.value<TOML_INT128>() == TOML_INT128{ 3 }));
		CHECK(!fractional.value<TOML_INT128>());
		CHECK((boolean.value<TOML_INT128>() == TOML_INT128{ 1 }));
		CHECK(!str.value<TOML_INT128>());

		CHECK((integer.value<TOML_UINT128>() == TOML_UINT128{ 42u }));
		CHECK(!negative.value<TOML_UINT128>());
		CHECK((whole.value<TOML_UINT128>() == TOML_UINT128{ 3u }));
		CHECK(!fractional.value<TOML_UINT128>());
		CHECK((boolean.value<TOML_UINT128>() == TOML_UINT128{ 1u }));
		CHECK(!str.value<TOML_UINT128>());
	}
#endif

	SECTION("float")
	{
		CHECK(integer.value<float>() == 42.0f);
		CHECK(!beyond_float.value<float>());
		CHECK(whole.value<float>() == 3.0f);
		CHECK(!huge.value<float>());
		CHECK(!boolean.value<float>());
		CHECK(!str.value<float>());
		CHECK(!dt.value<float>());
	}

	SECTION("long double")
	{
		CHECK(integer.value<long double>() == 42.0L);
		CHECK(huge.value<long double>() == static_cast<long double>(1e300));
		CHECK(!str.value<long double>());
	}

#if TOML_ENABLE_FLOAT16
	SECTION("_Float16")
	{
		const value<int64_t> beyond_half_value{ 2049 };
		const node& beyond_half = beyond_half_value;
		CHECK((integer.value<_Float16>() == static_cast<_Float16>(42)));
		CHECK(!beyond_half.value<_Float16>());
		CHECK((fractional.value<_Float16>() == static_cast<_Float16>(3.5)));
		CHECK((whole.value<_Float16>() == static_cast<_Float16>(3)));
		CHECK(!huge.value<_Float16>());
		CHECK(!str.value<_Float16>());
	}
#endif

#ifdef TOML_FLOAT128
	SECTION("128-bit floats")
	{
		CHECK((integer.value<TOML_FLOAT128>() == TOML_FLOAT128{ 42 }));
		CHECK((fractional.value<TOML_FLOAT128>() == TOML_FLOAT128{ 3.5 }));
		CHECK((huge.value<TOML_FLOAT128>() == TOML_FLOAT128{ 1e300 }));
		CHECK(!str.value<TOML_FLOAT128>());
	}
#endif

	SECTION("strings")
	{
		CHECK(str.value<const char*>() == str_value.get().c_str());
		CHECK(!integer.value<const char*>());
#if TOML_HAS_CHAR8
		CHECK(str.value<std::u8string>() == u8"hello"s);
		CHECK(str.value<std::u8string_view>() == u8"hello"sv);
		CHECK(str.value<const char8_t*>() == reinterpret_cast<const char8_t*>(str_value.get().c_str()));
		CHECK(!integer.value<std::u8string>());
		CHECK(!integer.value<std::u8string_view>());
		CHECK(!integer.value<const char8_t*>());
#endif
	}

	SECTION("nodes that are not numbers or booleans convert to nothing")
	{
		for (const node* n : { &str, &dt, static_cast<const node*>(&tbl), static_cast<const node*>(&arr) })
		{
			INFO("node type: " << n->type());
			CHECK(!n->value<int>());
			CHECK(!n->value<unsigned>());
			CHECK(!n->value<int64_t>());
			CHECK(!n->value<double>());
			CHECK(!n->value<float>());
			CHECK(!n->value<bool>());
		}
		CHECK(!tbl.value<std::string>());
		CHECK(!arr.value<date>());
	}
}

TEST_CASE("nodes - value() conversions match the documented examples")
{
	auto res = toml::parse(R"(
		int  = -10
		flt  = 25.0
		pi   = 3.14159
		bool = false
		huge = 9223372036854775807
		str  = "foo"
	)"sv);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	const table& tbl = res;

	SECTION("integer")
	{
		const node& n = *tbl.get("int");
		CHECK(n.value<bool>() == true);
		CHECK(n.value<int>() == -10);
		CHECK(!n.value<unsigned int>());
		CHECK(n.value<long long>() == -10LL);
		CHECK(n.value<float>() == -10.0f);
		CHECK(n.value<double>() == -10.0);
		CHECK(!n.value<std::string>());
		CHECK(!n.value<std::string_view>());
		CHECK(!n.value<const char*>());
	}

	SECTION("whole floating-point")
	{
		const node& n = *tbl.get("flt");
		CHECK(!n.value<bool>());
		CHECK(n.value<int>() == 25);
		CHECK(n.value<unsigned int>() == 25u);
		CHECK(n.value<long long>() == 25LL);
		CHECK(n.value<float>() == 25.0f);
		CHECK(n.value<double>() == 25.0);
		CHECK(!n.value<std::string>());
		CHECK(!n.value<std::string_view>());
		CHECK(!n.value<const char*>());
	}

	SECTION("fractional floating-point")
	{
		const node& n = *tbl.get("pi");
		CHECK(!n.value<bool>());
		CHECK(!n.value<int>());
		CHECK(!n.value<unsigned int>());
		CHECK(!n.value<long long>());
		CHECK(n.value<float>() == 3.14159f);
		CHECK(n.value<double>() == 3.14159);
		CHECK(!n.value<std::string>());
		CHECK(!n.value<std::string_view>());
		CHECK(!n.value<const char*>());
	}

	SECTION("boolean")
	{
		const node& n = *tbl.get("bool");
		CHECK(n.value<bool>() == false);
		CHECK(n.value<int>() == 0);
		CHECK(n.value<unsigned int>() == 0u);
		CHECK(n.value<long long>() == 0LL);
		CHECK(!n.value<float>());
		CHECK(!n.value<double>());
		CHECK(!n.value<std::string>());
		CHECK(!n.value<std::string_view>());
		CHECK(!n.value<const char*>());
	}

	SECTION("integer too large for most types")
	{
		const node& n = *tbl.get("huge");
		CHECK(n.value<bool>() == true);
		CHECK(!n.value<int>());
		CHECK(!n.value<unsigned int>());
		CHECK(n.value<long long>() == 9223372036854775807LL);
		CHECK(!n.value<float>());
		CHECK(!n.value<double>());
		CHECK(!n.value<std::string>());
		CHECK(!n.value<std::string_view>());
		CHECK(!n.value<const char*>());
	}

	SECTION("string")
	{
		const node& n = *tbl.get("str");
		CHECK(!n.value<bool>());
		CHECK(!n.value<int>());
		CHECK(!n.value<unsigned int>());
		CHECK(!n.value<long long>());
		CHECK(!n.value<float>());
		CHECK(!n.value<double>());
		CHECK(n.value<std::string>() == "foo"s);
		CHECK(n.value<std::string_view>() == "foo"sv);
		CHECK(n.value<const char*>() == "foo"sv);
	}
}

TEST_CASE("nodes - value() rejects floating-point values outside the range of int64_t")
{
	const auto check = [](double d, optional<int64_t> expected)
	{
		INFO("value: " << d);
		const value<double> val{ d };
		const node& n = val;
		CHECK(n.value<int64_t>() == expected);
		CHECK(n.value<int>() == (expected && *expected == 0 ? optional<int>{ 0 } : optional<int>{}));
		CHECK(n.value_or(int64_t{ -1 }) == expected.value_or(-1));
	};
	check(0.0, int64_t{ 0 });
	check(-9223372036854775808.0, (std::numeric_limits<int64_t>::min)());
	check(9223372036854774784.0, int64_t{ 9223372036854774784 });
	check(9223372036854775808.0, {});
	check(-9223372036854777856.0, {});
	check(1e300, {});
	check(-1e300, {});
}

TEST_CASE("nodes - value_or() returns the value or the default")
{
	value<std::string> str{ "hello"sv };
	value<int64_t> integer{ 42 };
	value<double> whole{ 3.0 };
	value<double> fractional{ 0.5 };
	value<double> huge{ 1e300 };
	value<bool> boolean{ true };
	value<date> dt{ sample_date };
	value<toml::time> tm{ sample_time };
	value<date_time> dtm{ sample_date_time };

	SECTION("bool")
	{
		const auto make = [] { return false; };
		check_value_or(boolean, make, true);
		check_value_or(integer, make, true);
		check_value_or(str, make, false);
	}

	SECTION("int")
	{
		const auto make = [] { return -7; };
		check_value_or(integer, make, 42);
		check_value_or(whole, make, 3);
		check_value_or(fractional, make, -7);
		check_value_or(boolean, make, 1);
		check_value_or(str, make, -7);
	}

	SECTION("int64_t")
	{
		const auto make = [] { return int64_t{ -7 }; };
		check_value_or(integer, make, int64_t{ 42 });
		check_value_or(whole, make, int64_t{ 3 });
		check_value_or(fractional, make, int64_t{ -7 });
		check_value_or(boolean, make, int64_t{ 1 });
		check_value_or(str, make, int64_t{ -7 });
	}

#ifdef TOML_INT128
	SECTION("128-bit integers")
	{
		value<int64_t> negative{ -1 };
		const auto make_signed = [] { return TOML_INT128{ -7 }; };
		check_value_or(integer, make_signed, TOML_INT128{ 42 });
		check_value_or(negative, make_signed, TOML_INT128{ -1 });
		check_value_or(whole, make_signed, TOML_INT128{ 3 });
		check_value_or(str, make_signed, TOML_INT128{ -7 });

		const auto make_unsigned = [] { return TOML_UINT128{ 7u }; };
		check_value_or(integer, make_unsigned, TOML_UINT128{ 42u });
		check_value_or(negative, make_unsigned, TOML_UINT128{ 7u });
		check_value_or(whole, make_unsigned, TOML_UINT128{ 3u });
		check_value_or(str, make_unsigned, TOML_UINT128{ 7u });
	}
#endif

	SECTION("float")
	{
		const auto make = [] { return -7.0f; };
		check_value_or(fractional, make, 0.5f);
		check_value_or(integer, make, 42.0f);
		check_value_or(huge, make, -7.0f);
		check_value_or(str, make, -7.0f);
	}

	SECTION("double")
	{
		const auto make = [] { return -7.0; };
		check_value_or(fractional, make, 0.5);
		check_value_or(integer, make, 42.0);
		check_value_or(boolean, make, -7.0);
		check_value_or(str, make, -7.0);
	}

#ifdef TOML_FLOAT128
	SECTION("128-bit floats")
	{
		const auto make = [] { return TOML_FLOAT128{ -7 }; };
		check_value_or(fractional, make, TOML_FLOAT128{ 0.5 });
		check_value_or(integer, make, TOML_FLOAT128{ 42 });
		check_value_or(str, make, TOML_FLOAT128{ -7 });
	}
#endif

	SECTION("dates and times")
	{
		const auto make_date = [] { return date{ 1999, 12, 31 }; };
		check_value_or(dt, make_date, sample_date);
		check_value_or(dtm, make_date, date{ 1999, 12, 31 });

		const auto make_time = [] { return toml::time{ 23, 59 }; };
		check_value_or(tm, make_time, sample_time);
		check_value_or(dtm, make_time, toml::time{ 23, 59 });

		const auto make_date_time = [] { return date_time{ date{ 1999, 12, 31 } }; };
		check_value_or(dtm, make_date_time, sample_date_time);
		check_value_or(dt, make_date_time, date_time{ date{ 1999, 12, 31 } });
	}

	SECTION("std::string and std::string_view")
	{
		const auto make_string = [] { return "fallback"s; };
		check_value_or(str, make_string, "hello"s);
		check_value_or(integer, make_string, "fallback"s);

		const auto make_view = [] { return "fallback"sv; };
		check_value_or(str, make_view, "hello"sv);
		check_value_or(integer, make_view, "fallback"sv);
	}

	SECTION("character pointers")
	{
		static constexpr const char* fallback = "fallback";
		const auto make_const				  = [] { return fallback; };
		check_value_or(str, make_const, str.get().c_str());
		check_value_or(integer, make_const, fallback);

		char buffer[]			= "fallback";
		const auto make_mutable = [&] { return buffer; };
		check_value_or(str, make_mutable, str.get().c_str());
		check_value_or(integer, make_mutable, static_cast<const char*>(buffer));
	}

	SECTION("character arrays")
	{
		char buffer[]			  = "x";
		const char const_buffer[] = "x";
		check_value_or_arrays(str, integer, buffer);
		check_value_or_arrays(str, integer, const_buffer);
	}

#if TOML_HAS_CHAR8
	SECTION("char8_t strings")
	{
		const auto make_string = [] { return u8"fallback"s; };
		check_value_or(str, make_string, u8"hello"s);
		check_value_or(integer, make_string, u8"fallback"s);

		const auto make_view = [] { return u8"fallback"sv; };
		check_value_or(str, make_view, u8"hello"sv);
		check_value_or(integer, make_view, u8"fallback"sv);

		const auto str_data						 = reinterpret_cast<const char8_t*>(str.get().c_str());
		static constexpr const char8_t* fallback = u8"fallback";
		const auto make_const					 = [] { return fallback; };
		check_value_or(str, make_const, str_data);
		check_value_or(integer, make_const, fallback);

		char8_t buffer[]		= u8"fallback";
		const auto make_mutable = [&] { return buffer; };
		check_value_or(str, make_mutable, str_data);
		check_value_or(integer, make_mutable, static_cast<const char8_t*>(buffer));

		char8_t array_buffer[]			   = u8"x";
		const char8_t const_array_buffer[] = u8"x";
		check_value_or_arrays(str, integer, array_buffer);
		check_value_or_arrays(str, integer, const_array_buffer);
	}
#endif
}

TEST_CASE("nodes - node_type can be written to streams")
{
	const auto narrow = [](node_type type)
	{
		std::ostringstream ss;
		ss << type;
		return std::move(ss).str();
	};
	CHECK(narrow(node_type::none) == "none"sv);
	CHECK(narrow(node_type::table) == "table"sv);
	CHECK(narrow(node_type::array) == "array"sv);
	CHECK(narrow(node_type::string) == "string"sv);
	CHECK(narrow(node_type::integer) == "integer"sv);
	CHECK(narrow(node_type::floating_point) == "floating-point"sv);
	CHECK(narrow(node_type::boolean) == "boolean"sv);
	CHECK(narrow(node_type::date) == "date"sv);
	CHECK(narrow(node_type::time) == "time"sv);
	CHECK(narrow(node_type::date_time) == "date-time"sv);

	std::wostringstream wide;
	wide << node_type::floating_point;
	CHECK(std::move(wide).str() == L"floating-point"sv);
}

TEST_CASE("nodes - copies do not keep the source region but moves do")
{
	auto res = toml::parse("val = 42\nother = 7"sv);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	table& tbl = res;

	auto& parsed			 = *tbl.get_as<int64_t>("val");
	auto& other				 = *tbl.get_as<int64_t>("other");
	const auto parsed_source = parsed.source();
	REQUIRE(parsed_source.begin == source_position{ 1, 7 });
	REQUIRE(other.source().begin == source_position{ 2, 9 });

	value<int64_t> copied{ parsed };
	CHECK(*copied == 42);
	CHECK(!copied.source().begin);

	value<int64_t> assigned{ 0 };
	assigned = parsed;
	CHECK(*assigned == 42);
	CHECK(!assigned.source().begin);

	other = parsed;
	CHECK(*other == 42);
	CHECK(!other.source().begin);

	value<int64_t> moved{ std::move(parsed) };
	CHECK(*moved == 42);
	CHECK(moved.source().begin == parsed_source.begin);
	CHECK(moved.source().end == parsed_source.end);

	value<int64_t> move_assigned{ 0 };
	move_assigned = std::move(moved);
	CHECK(*move_assigned == 42);
	CHECK(move_assigned.source().begin == parsed_source.begin);
}

TEST_CASE("nodes - at_path() and operator[]")
{
	table tbl{ { "a"sv, table{ { "b"sv, array{ 1, 2, 3 } } } } };
	node& n		   = tbl;
	const node& cn = tbl;

	CHECK(n.at_path("a.b[1]"sv) == 2);
	CHECK(cn.at_path("a.b[1]"sv) == 2);
	CHECK(n.at_path(toml::path{ "a.b[2]"sv }) == 3);
	CHECK(cn.at_path(toml::path{ "a.b[2]"sv }) == 3);
	CHECK(n[toml::path{ "a.b[0]"sv }] == 1);
	CHECK(cn[toml::path{ "a.b[0]"sv }] == 1);

	CHECK(n.at_path("a.b[1]"sv).node() == tbl["a"]["b"][1].node());
	CHECK(!n.at_path("a.c"sv));
	CHECK(!cn.at_path(toml::path{ "a.b[3]"sv }));
	CHECK(!n[toml::path{ "x" }]);
}

TEST_CASE("nodes - at_path() documented examples")
{
	auto res = toml::parse(R"(
		[foo]
		bar = [ 0, 1, 2, [ 3 ], { kek = 4 } ]
	)"sv);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	table& config = res;

	CHECK(config.at_path("foo.bar[2]") == 2);
	CHECK(config.at_path("foo.bar[3][0]") == 3);
	CHECK(config.at_path("foo.bar[4].kek") == 4);

	const toml::path path1{ "foo.bar[2]" };
	const toml::path path2{ "foo.bar[4].kek" };
	CHECK(config.at_path(path1) == 2);
	CHECK(printed(config.at_path(path1.parent())) == "[ 0, 1, 2, [ 3 ], { kek = 4 } ]"sv);
	CHECK(config.at_path(path2) == 4);
	CHECK(printed(config.at_path(path2.parent())) == "{ kek = 4 }"sv);
}

TEST_CASE("nodes - is_homogeneous() documented examples")
{
	auto res = toml::parse("arr = [ 1, 2, 3, 4.0 ]"sv);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	table& cfg = res;
	node& arr  = *cfg.get("arr");

	node* nonmatch = {};
	CHECK(!arr.is_homogeneous(node_type::integer, nonmatch));
	REQUIRE(nonmatch);
	CHECK(nonmatch->type() == node_type::floating_point);
	CHECK(nonmatch->source().begin == source_position{ 1, 18 });
	CHECK(nonmatch->source().end == source_position{ 1, 21 });

	const node* const_nonmatch = {};
	CHECK(!std::as_const(arr).is_homogeneous(node_type::integer, const_nonmatch));
	CHECK(const_nonmatch == nonmatch);

	const auto ints		  = array{ 1, 2, 3 };
	const node& ints_node = ints;
	CHECK(ints_node.is_homogeneous(node_type::none));
	CHECK(!ints_node.is_homogeneous(node_type::floating_point));
	CHECK(!ints_node.is_homogeneous(node_type::array));
	CHECK(ints_node.is_homogeneous(node_type::integer));
	CHECK(ints_node.is_homogeneous());
	CHECK(!ints_node.is_homogeneous<double>());
	CHECK(!ints_node.is_homogeneous<toml::array>());
	CHECK(ints_node.is_homogeneous<int64_t>());

	const auto empty	   = array{};
	const node& empty_node = empty;
	CHECK(!empty_node.is_homogeneous());
	CHECK(!empty_node.is_homogeneous(node_type::integer));
}

TEST_CASE("nodes - explicit conversion to node_view")
{
	value<int64_t> val{ 42 };
	node& n = val;

	const auto view = n.operator node_view<node>();
	CHECK(view.node() == &n);
	CHECK(view == 42);

	const auto const_view = std::as_const(n).operator node_view<const node>();
	CHECK(const_view.node() == &n);
	CHECK(const_view == 42);
}

TEST_CASE("nodes - inserting a node copies or moves it into the container")
{
	sample_nodes s;
	for_each_type(node_types{},
				  [&](auto tag)
				  {
					  using T		 = typename decltype(tag)::type;
					  auto& concrete = sample_of<T>(s);
					  INFO("node type: " << concrete.type());

					  array arr;
					  arr.push_back(concrete);
					  arr.push_back(std::as_const(concrete));
					  arr.push_back(static_cast<node&>(concrete));
					  arr.push_back(static_cast<const node&>(concrete));
					  arr.push_back(node_view<node>{ concrete });
					  arr.push_back(node_view<const node>{ concrete });
					  {
						  auto moved_from = concrete;
						  arr.push_back(std::move(moved_from));
					  }
					  {
						  auto moved_from = concrete;
						  arr.push_back(static_cast<node&&>(moved_from));
					  }

					  REQUIRE(arr.size() == 8u);
					  for (auto& elem : arr)
					  {
						  CHECK(elem.type() == concrete.type());
						  CHECK(node_view<node>{ elem } == node_view<node>{ concrete });
					  }
				  });
}

TEST_CASE("nodes - inserting raw values creates value nodes")
{
	array arr;
	arr.push_back(static_cast<signed char>(-1));
	arr.push_back(static_cast<short>(-2));
	arr.push_back(static_cast<unsigned char>(3u));
	arr.push_back(static_cast<unsigned short>(4u));
	arr.push_back(5u);
	arr.push_back(static_cast<const char*>("six"));
	const float seven = 7.5f;
	arr.push_back(seven);
	arr.push_back(sample_date_time);

	REQUIRE(arr.size() == 8u);
	CHECK(arr[0].value_exact<int64_t>() == -1);
	CHECK(arr[1].value_exact<int64_t>() == -2);
	CHECK(arr[2].value_exact<int64_t>() == 3);
	CHECK(arr[3].value_exact<int64_t>() == 4);
	CHECK(arr[4].value_exact<int64_t>() == 5);
	CHECK(arr[5].value_exact<std::string_view>() == "six"sv);
	CHECK(arr[6].value_exact<double>() == 7.5);
	CHECK(arr[7].value_exact<date_time>() == sample_date_time);
}
