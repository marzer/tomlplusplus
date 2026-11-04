// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	constexpr date sample_date{ 2024, 1, 2 };
	constexpr toml::time sample_time{ 3, 4, 5 };
	constexpr date_time sample_date_time{ sample_date, sample_time };

	constexpr node_type all_types[] = { node_type::none,	 node_type::table,	 node_type::array,
										node_type::string,	 node_type::integer, node_type::floating_point,
										node_type::boolean,	 node_type::date,	 node_type::time,
										node_type::date_time };

	template <typename Func>
	void for_each_sample_value(Func&& func)
	{
		value<std::string> str{ "hello"sv };
		value<int64_t> integer{ 42 };
		value<double> floating_point{ 0.5 };
		value<bool> boolean{ true };
		value<date> dt{ sample_date };
		value<toml::time> tm{ sample_time };
		value<date_time> dtm{ sample_date_time };
		func(str, node_type::string);
		func(integer, node_type::integer);
		func(floating_point, node_type::floating_point);
		func(boolean, node_type::boolean);
		func(dt, node_type::date);
		func(tm, node_type::time);
		func(dtm, node_type::date_time);
	}
}

TEST_CASE("values - default construction value-initialises")
{
	CHECK(value<std::string>{}.get().empty());
	CHECK(value<int64_t>{}.get() == 0);
	CHECK(value<double>{}.get() == 0.0);
	CHECK(value<bool>{}.get() == false);
	CHECK(value<date>{}.get() == date{});
	CHECK(value<toml::time>{}.get() == toml::time{});
	CHECK(value<date_time>{}.get() == date_time{});
	CHECK(value<int64_t>{}.flags() == value_flags::none);
}

TEST_CASE("values - constructor arguments are forwarded to the underlying value")
{
	CHECK(*value<std::string>{ 3u, 'x' } == "xxx"sv);
	CHECK(*value<std::string>{ "abcdef"sv, 1u, 3u } == "bcd"sv);
	CHECK(*value<double>{ 1 } == 1.0);
	CHECK(*value<date>{ 2024, 1, 2 } == sample_date);
	CHECK(*value<toml::time>{ 3, 4, 5 } == sample_time);
	CHECK(*value<date_time>{ sample_date, sample_time } == sample_date_time);
}

TEST_CASE("values - constructing from a raw value and flags")
{
	const value<int64_t> hex{ 42, value_flags::format_as_hexadecimal };
	CHECK(*hex == 42);
	CHECK(hex.flags() == value_flags::format_as_hexadecimal);

	std::ostringstream ss;
	ss << hex;
	CHECK(ss.str() == "0x2A"sv);

	const toml::value deduced{ 42, value_flags::format_as_octal };
	static_assert(std::is_same_v<decltype(deduced), const value<int64_t>>);
	CHECK(*deduced == 42);
	CHECK(deduced.flags() == value_flags::format_as_octal);

	const toml::value str{ "abc"sv, value_flags::none };
	static_assert(std::is_same_v<decltype(str), const value<std::string>>);
	CHECK(*str == "abc"sv);

	const value<double> converted{ 1, value_flags::none };
	CHECK(*converted == 1.0);

	const value<int64_t> no_source{ 42, preserve_source_value_flags };
	CHECK(no_source.flags() == value_flags::none);
}

TEST_CASE("values - copying and moving keep the value and flags")
{
	value<int64_t> original{ 42 };
	original.flags(value_flags::format_as_hexadecimal);

	SECTION("copy construction")
	{
		const value<int64_t> copy{ original };
		CHECK(*copy == 42);
		CHECK(copy.flags() == value_flags::format_as_hexadecimal);
		CHECK(*original == 42);
	}

	SECTION("copy construction with a flags override")
	{
		const value<int64_t> octal{ original, value_flags::format_as_octal };
		CHECK(*octal == 42);
		CHECK(octal.flags() == value_flags::format_as_octal);

		const value<int64_t> preserved{ original, preserve_source_value_flags };
		CHECK(*preserved == 42);
		CHECK(preserved.flags() == value_flags::format_as_hexadecimal);
	}

	SECTION("move construction")
	{
		const value<int64_t> moved{ std::move(original) };
		CHECK(*moved == 42);
		CHECK(moved.flags() == value_flags::format_as_hexadecimal);
	}

	SECTION("move construction with a flags override")
	{
		value<int64_t> first{ original };
		const value<int64_t> binary{ std::move(first), value_flags::format_as_binary };
		CHECK(*binary == 42);
		CHECK(binary.flags() == value_flags::format_as_binary);

		value<int64_t> second{ original };
		const value<int64_t> preserved{ std::move(second), preserve_source_value_flags };
		CHECK(*preserved == 42);
		CHECK(preserved.flags() == value_flags::format_as_hexadecimal);
	}

	SECTION("copy assignment")
	{
		value<int64_t> target{ 1 };
		target = original;
		CHECK(*target == 42);
		CHECK(target.flags() == value_flags::format_as_hexadecimal);
	}

	SECTION("move assignment")
	{
		value<int64_t> target{ 1 };
		value<int64_t> source{ original };
		target = std::move(source);
		CHECK(*target == 42);
		CHECK(target.flags() == value_flags::format_as_hexadecimal);
	}

	SECTION("strings")
	{
		value<std::string> str{ "a string long enough that no implementation can keep it in a small buffer"sv };
		const value<std::string> copy{ str };
		CHECK(copy == str);
		const auto buffer = str.get().data();
		const value<std::string> moved{ std::move(str) };
		CHECK(moved == copy);
		CHECK(moved.get().data() == buffer);
	}
}

TEST_CASE("values - flags() sets the metadata and returns the value")
{
	value<int64_t> val{ 42 };
	CHECK(val.flags() == value_flags::none);
	CHECK(&val.flags(value_flags::format_as_octal) == &val);
	CHECK(val.flags() == value_flags::format_as_octal);
	CHECK(*val == 42);
}

TEST_CASE("values - assigning raw values")
{
	value<std::string> str;
	str = "view"sv;
	CHECK(*str == "view"sv);
	str = std::string{ "owned" };
	CHECK(*str == "owned"sv);

	value<int64_t> integer;
	integer = 42;
	CHECK(*integer == 42);

	value<double> floating_point;
	floating_point = 0.25;
	CHECK(*floating_point == 0.25);

	value<bool> boolean;
	boolean = true;
	CHECK(*boolean);

	value<date> dt;
	dt = sample_date;
	CHECK(*dt == sample_date);

	value<toml::time> tm;
	tm = sample_time;
	CHECK(*tm == sample_time);

	value<date_time> dtm;
	dtm = sample_date_time;
	CHECK(*dtm == sample_date_time);
}

TEST_CASE("values - accessors refer to the underlying value")
{
	value<std::string> val{ "hello"sv };
	const auto& cval	   = val;
	const auto* underlying = &val.get();

	SECTION("get()")
	{
		static_assert(std::is_same_v<decltype(val.get()), std::string&>);
		static_assert(std::is_same_v<decltype(std::move(val).get()), std::string&&>);
		static_assert(std::is_same_v<decltype(cval.get()), const std::string&>);
		static_assert(std::is_same_v<decltype(std::move(cval).get()), const std::string&&>);

		CHECK(&val.get() == underlying);
		CHECK(&cval.get() == underlying);
		std::string&& rref			   = std::move(val).get();
		const std::string&& const_rref = std::move(cval).get();
		CHECK(&rref == underlying);
		CHECK(&const_rref == underlying);
	}

	SECTION("operator*")
	{
		static_assert(std::is_same_v<decltype(*val), std::string&>);
		static_assert(std::is_same_v<decltype(*std::move(val)), std::string&&>);
		static_assert(std::is_same_v<decltype(*cval), const std::string&>);
		static_assert(std::is_same_v<decltype(*std::move(cval)), const std::string&&>);

		CHECK(&*val == underlying);
		CHECK(&*cval == underlying);
		std::string&& rref			   = *std::move(val);
		const std::string&& const_rref = *std::move(cval);
		CHECK(&rref == underlying);
		CHECK(&const_rref == underlying);
	}

	SECTION("explicit conversions")
	{
		CHECK(&static_cast<std::string&>(val) == underlying);
		CHECK(&static_cast<const std::string&>(cval) == underlying);
		std::string&& rref			   = static_cast<std::string&&>(std::move(val));
		const std::string&& const_rref = static_cast<const std::string&&>(std::move(val));
		const std::string&& from_const = static_cast<const std::string&&>(std::move(cval));
		CHECK(&rref == underlying);
		CHECK(&const_rref == underlying);
		CHECK(&from_const == underlying);

		value<int64_t> integer{ 42 };
		CHECK(static_cast<int64_t>(std::move(integer)) == 42);
	}

	SECTION("operator->")
	{
		CHECK(val->size() == 5u);
		CHECK(cval->data() == underlying->data());

		value<date> dt{ sample_date };
		CHECK(dt->year == 2024);
		dt->year = 2025;
		CHECK(std::as_const(dt)->year == 2025);
	}

	SECTION("writes through accessors are visible")
	{
		val.get() += "!";
		*val += "?";
		static_cast<std::string&>(val) += ".";
		CHECK(*cval == "hello!?."sv);
	}
}

TEST_CASE("values - comparisons with raw values")
{
	SECTION("integers")
	{
		const value<int64_t> val{ 42 };
		CHECK_SYMMETRIC_EQUAL(val, int64_t{ 42 });
		CHECK_SYMMETRIC_INEQUAL(val, int64_t{ 43 });
		CHECK(val < int64_t{ 43 });
		CHECK(!(val < int64_t{ 42 }));
		CHECK(int64_t{ 41 } < val);
		CHECK(!(int64_t{ 42 } < val));
		CHECK(val <= int64_t{ 42 });
		CHECK(!(val <= int64_t{ 41 }));
		CHECK(int64_t{ 42 } <= val);
		CHECK(!(int64_t{ 43 } <= val));
		CHECK(val > int64_t{ 41 });
		CHECK(!(val > int64_t{ 42 }));
		CHECK(int64_t{ 43 } > val);
		CHECK(!(int64_t{ 42 } > val));
		CHECK(val >= int64_t{ 42 });
		CHECK(!(val >= int64_t{ 43 }));
		CHECK(int64_t{ 42 } >= val);
		CHECK(!(int64_t{ 41 } >= val));
	}

	SECTION("strings")
	{
		const value<std::string> val{ "b"sv };
		CHECK_SYMMETRIC_EQUAL(val, "b"sv);
		CHECK_SYMMETRIC_INEQUAL(val, "c"sv);
		CHECK(val < "c"sv);
		CHECK("a"sv < val);
		CHECK(val <= "b"sv);
		CHECK("b"sv <= val);
		CHECK(val > "a"sv);
		CHECK("c"sv > val);
		CHECK(val >= "b"sv);
		CHECK("b"sv >= val);
	}

	SECTION("floating-point")
	{
		const value<double> val{ 0.5 };
		CHECK_SYMMETRIC_EQUAL(val, 0.5);
		CHECK_SYMMETRIC_INEQUAL(val, 1.5);
		CHECK_SYMMETRIC_INEQUAL(val, std::numeric_limits<double>::quiet_NaN());
		CHECK(val < 1.0);
		CHECK(0.25 < val);
		CHECK(val <= 0.5);
		CHECK(0.5 <= val);
		CHECK(val > 0.25);
		CHECK(1.0 > val);
		CHECK(val >= 0.5);
		CHECK(0.5 >= val);

		const value<double> nan{ std::numeric_limits<double>::quiet_NaN() };
		CHECK_SYMMETRIC_EQUAL(nan, std::numeric_limits<double>::quiet_NaN());
		CHECK_SYMMETRIC_INEQUAL(nan, 0.5);
	}

	SECTION("booleans")
	{
		const value<bool> val{ true };
		CHECK_SYMMETRIC_EQUAL(val, true);
		CHECK_SYMMETRIC_INEQUAL(val, false);
		CHECK(false < val);
		CHECK(val > false);
		CHECK(val >= true);
		CHECK(val <= true);
	}

	SECTION("dates and times")
	{
		const value<date> dt{ sample_date };
		CHECK_SYMMETRIC_EQUAL(dt, sample_date);
		CHECK_SYMMETRIC_INEQUAL(dt, (date{ 2024, 1, 3 }));
		CHECK(dt < date{ 2024, 1, 3 });
		CHECK(date{ 2024, 1, 1 } < dt);
		CHECK(dt <= sample_date);
		CHECK(dt > date{ 2023, 12, 31 });
		CHECK(dt >= sample_date);

		const value<toml::time> tm{ sample_time };
		CHECK_SYMMETRIC_EQUAL(tm, sample_time);
		CHECK(tm < toml::time{ 3, 4, 6 });
		CHECK(toml::time{ 3, 4, 4 } <= tm);
		CHECK(tm > toml::time{ 3, 4, 4 });
		CHECK(toml::time{ 3, 4, 5 } >= tm);

		const value<date_time> dtm{ sample_date_time };
		CHECK_SYMMETRIC_EQUAL(dtm, sample_date_time);
		CHECK_SYMMETRIC_INEQUAL(dtm, date_time{ sample_date });
		CHECK(date_time{ sample_date } < dtm);
		CHECK(dtm > date_time{ sample_date });
	}
}

TEST_CASE("values - comparisons between values")
{
	SECTION("same type compares the values")
	{
		const value<int64_t> one{ 1 };
		const value<int64_t> two{ 2 };
		CHECK_SYMMETRIC_EQUAL(one, value<int64_t>{ 1 });
		CHECK_SYMMETRIC_INEQUAL(one, two);
		CHECK(one < two);
		CHECK(!(two < one));
		CHECK(one <= two);
		CHECK(one <= value<int64_t>{ 1 });
		CHECK(!(two <= one));
		CHECK(two > one);
		CHECK(!(one > two));
		CHECK(two >= one);
		CHECK(two >= value<int64_t>{ 2 });
		CHECK(!(one >= two));
	}

	SECTION("different types are never equal and order by node type")
	{
		const value<std::string> str{ "42"sv };
		const value<int64_t> integer{ 42 };
		const value<date_time> dtm{ sample_date_time };
		CHECK_SYMMETRIC_INEQUAL(str, integer);
		CHECK_SYMMETRIC_INEQUAL(integer, dtm);
		CHECK(str < integer);
		CHECK(!(integer < str));
		CHECK(str <= integer);
		CHECK(!(integer <= str));
		CHECK(integer > str);
		CHECK(!(str > integer));
		CHECK(dtm >= integer);
		CHECK(!(integer >= dtm));
	}
}

TEST_CASE("values - is_homogeneous() is true only for the value's own type")
{
	for_each_sample_value(
		[](auto& val, node_type own_type)
		{
			using value_type = typename std::remove_reference_t<decltype(val)>::value_type;
			INFO("value type: " << own_type);

			CHECK(val.is_homogeneous());
			CHECK(val.template is_homogeneous<value_type>());
			CHECK(val.template is_homogeneous<value<value_type>>());
			CHECK(!val.template is_homogeneous<table>());
			CHECK(!val.template is_homogeneous<array>());

			const node& as_node = val;
			CHECK(as_node.is_homogeneous());
			CHECK(as_node.is_homogeneous<value_type>());

			for (const auto type : all_types)
			{
				INFO("checked against " << type);
				const bool expected			  = type == node_type::none || type == own_type;
				const node* expected_nonmatch = expected ? nullptr : &val;
				CHECK(val.is_homogeneous(type) == expected);
				CHECK(as_node.is_homogeneous(type) == expected);

				node* nonmatch = {};
				CHECK(val.is_homogeneous(type, nonmatch) == expected);
				CHECK(nonmatch == expected_nonmatch);

				const node* const_nonmatch = {};
				CHECK(std::as_const(val).is_homogeneous(type, const_nonmatch) == expected);
				CHECK(const_nonmatch == expected_nonmatch);
			}
		});
}
