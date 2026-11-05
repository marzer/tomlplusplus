// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

#ifdef _WIN32
TOML_DISABLE_WARNINGS;
#include <windows.h>
TOML_ENABLE_WARNINGS;
#endif

template <typename T>
static constexpr T one = static_cast<T>(1);

TEST_CASE("values - construction")
{
	static constexpr auto check_value = [](const auto& init_value, auto expected_native_type_tag)
	{
		using init_type			   = impl::remove_cvref<decltype(init_value)>;
		using native_type		   = impl::native_type_of<init_type>;
		using expected_native_type = typename decltype(expected_native_type_tag)::type;
		static_assert(std::is_same_v<native_type, expected_native_type>);

		auto v			 = value{ init_value };
		using value_type = decltype(v);
		static_assert(std::is_same_v<value_type, value<native_type>>);

		if constexpr (std::is_same_v<std::string, native_type>)
		{
#if TOML_HAS_CHAR8
			using char8_type = char8_t;
#else
			using char8_type = char;
#endif

			using init_char_type = impl::remove_cvref<decltype(init_value[0])>;
			using init_view_type = std::basic_string_view<init_char_type>;
			static_assert(impl::is_one_of<init_char_type, char, wchar_t, char8_type>);

			const auto init_view = init_view_type{ init_value };
			if constexpr (impl::is_one_of<init_char_type, char, char8_type>)
			{
				const auto coerced_view =
					std::string_view{ reinterpret_cast<const char*>(init_view.data()), init_view.length() };

				CHECK(v == coerced_view);
				CHECK(coerced_view == v);
			}
#if TOML_ENABLE_WINDOWS_COMPAT
			else if constexpr (impl::is_one_of<init_char_type, wchar_t>)
			{
				const auto narrowed_string = impl::narrow(init_view);

				CHECK(v == narrowed_string);
				CHECK(narrowed_string == v);
			}
#endif
			else
			{
				static_assert(impl::always_false<init_char_type>, "evaluated unreachable branch");
			}
		}
		else if constexpr (impl::is_one_of<native_type, int64_t, double, bool>)
		{
			CHECK(v == static_cast<native_type>(init_value));
			CHECK(static_cast<native_type>(init_value) == v);
		}
		else // dates + times
		{
			CHECK(v == init_value);
			CHECK(init_value == v);
		}

		static constexpr auto expected_node_type = impl::node_type_of<native_type>;

		CHECK(v.is_homogeneous());
		CHECK(v.template is_homogeneous<native_type>());
		CHECK(v.is_homogeneous(expected_node_type));

		// sanity check the virtual type checks
		CHECK(v.type() == expected_node_type);
		CHECK(!v.is_table());
		CHECK(!v.is_array());
		CHECK(!v.is_array_of_tables());
		CHECK(v.is_value());
		CHECK(v.is_string() == (expected_node_type == node_type::string));
		CHECK(v.is_integer() == (expected_node_type == node_type::integer));
		CHECK(v.is_floating_point() == (expected_node_type == node_type::floating_point));
		CHECK(v.is_number()
			  == (expected_node_type == node_type::integer || expected_node_type == node_type::floating_point));
		CHECK(v.is_boolean() == (expected_node_type == node_type::boolean));
		CHECK(v.is_date() == (expected_node_type == node_type::date));
		CHECK(v.is_time() == (expected_node_type == node_type::time));
		CHECK(v.is_date_time() == (expected_node_type == node_type::date_time));

		// sanity check the virtual type casts (non-const)
		CHECK(!v.as_table());
		CHECK(!v.as_array());
		if constexpr (expected_node_type == node_type::string)
			CHECK(v.as_string() == &v);
		else
			CHECK(!v.as_string());
		if constexpr (expected_node_type == node_type::integer)
			CHECK(v.as_integer() == &v);
		else
			CHECK(!v.as_integer());
		if constexpr (expected_node_type == node_type::floating_point)
			CHECK(v.as_floating_point() == &v);
		else
			CHECK(!v.as_floating_point());
		if constexpr (expected_node_type == node_type::boolean)
			CHECK(v.as_boolean() == &v);
		else
			CHECK(!v.as_boolean());
		if constexpr (expected_node_type == node_type::date)
			CHECK(v.as_date() == &v);
		else
			CHECK(!v.as_date());
		if constexpr (expected_node_type == node_type::time)
			CHECK(v.as_time() == &v);
		else
			CHECK(!v.as_time());
		if constexpr (expected_node_type == node_type::date_time)
			CHECK(v.as_date_time() == &v);
		else
			CHECK(!v.as_date_time());

		// sanity check the virtual type casts (const)
		const auto& cv = std::as_const(v);
		CHECK(!cv.as_table());
		CHECK(!cv.as_array());
		if constexpr (expected_node_type == node_type::string)
			CHECK(cv.as_string() == &v);
		else
			CHECK(!cv.as_string());
		if constexpr (expected_node_type == node_type::integer)
			CHECK(cv.as_integer() == &v);
		else
			CHECK(!cv.as_integer());
		if constexpr (expected_node_type == node_type::floating_point)
			CHECK(cv.as_floating_point() == &v);
		else
			CHECK(!cv.as_floating_point());
		if constexpr (expected_node_type == node_type::boolean)
			CHECK(cv.as_boolean() == &v);
		else
			CHECK(!cv.as_boolean());
		if constexpr (expected_node_type == node_type::date)
			CHECK(cv.as_date() == &v);
		else
			CHECK(!cv.as_date());
		if constexpr (expected_node_type == node_type::time)
			CHECK(cv.as_time() == &v);
		else
			CHECK(!cv.as_time());
		if constexpr (expected_node_type == node_type::date_time)
			CHECK(cv.as_date_time() == &v);
		else
			CHECK(!cv.as_date_time());
	};

	check_value(one<signed char>, type_tag<int64_t>{});
	check_value(one<signed short>, type_tag<int64_t>{});
	check_value(one<signed int>, type_tag<int64_t>{});
	check_value(one<signed long>, type_tag<int64_t>{});
	check_value(one<signed long long>, type_tag<int64_t>{});
	check_value(one<unsigned char>, type_tag<int64_t>{});
	check_value(one<unsigned short>, type_tag<int64_t>{});
	check_value(one<unsigned int>, type_tag<int64_t>{});
	check_value(one<unsigned long>, type_tag<int64_t>{});
	check_value(one<unsigned long long>, type_tag<int64_t>{});
	check_value(true, type_tag<bool>{});
	check_value(false, type_tag<bool>{});
	check_value(one<float>, type_tag<double>{});
	check_value(one<double>, type_tag<double>{});
	check_value(date{ 2024, 1, 2 }, type_tag<date>{});
	check_value(toml::time{ 3, 4, 5 }, type_tag<toml::time>{});
	check_value(date_time{ date{ 2024, 1, 2 }, toml::time{ 3, 4, 5 } }, type_tag<date_time>{});
	check_value("kek", type_tag<std::string>{});
	check_value("kek"s, type_tag<std::string>{});
	check_value("kek"sv, type_tag<std::string>{});
	check_value("kek"sv.data(), type_tag<std::string>{});
#if TOML_HAS_CHAR8
	check_value(u8"kek", type_tag<std::string>{});
	check_value(u8"kek"s, type_tag<std::string>{});
	check_value(u8"kek"sv, type_tag<std::string>{});
	check_value(u8"kek"sv.data(), type_tag<std::string>{});
#endif

#ifdef _WIN32
	check_value(one<BOOL>, type_tag<int64_t>{});
	check_value(one<SHORT>, type_tag<int64_t>{});
	check_value(one<INT>, type_tag<int64_t>{});
	check_value(one<LONG>, type_tag<int64_t>{});
	check_value(one<INT_PTR>, type_tag<int64_t>{});
	check_value(one<LONG_PTR>, type_tag<int64_t>{});
	check_value(one<USHORT>, type_tag<int64_t>{});
	check_value(one<UINT>, type_tag<int64_t>{});
	check_value(one<ULONG>, type_tag<int64_t>{});
	check_value(one<UINT_PTR>, type_tag<int64_t>{});
	check_value(one<ULONG_PTR>, type_tag<int64_t>{});
	check_value(one<WORD>, type_tag<int64_t>{});
	check_value(one<DWORD>, type_tag<int64_t>{});
	check_value(one<DWORD32>, type_tag<int64_t>{});
	check_value(one<DWORD64>, type_tag<int64_t>{});
	check_value(one<DWORDLONG>, type_tag<int64_t>{});

#if TOML_ENABLE_WINDOWS_COMPAT

	check_value(L"kek", type_tag<std::string>{});
	check_value(L"kek"s, type_tag<std::string>{});
	check_value(L"kek"sv, type_tag<std::string>{});
	check_value(L"kek"sv.data(), type_tag<std::string>{});

#endif // TOML_ENABLE_WINDOWS_COMPAT

#endif
}

TEST_CASE("values - toml_formatter")
{
	static constexpr auto print_value = [](auto&& raw)
	{
		auto val = toml::value{ std::forward<decltype(raw)>(raw) };
		std::stringstream ss;
		ss << val;
		return ss.str();
	};

	CHECK(print_value(1) == "1");
	CHECK(print_value(1.0f) == "1.0");
	CHECK(print_value(1.0) == "1.0");

	CHECK(print_value(1.5f) == "1.5");
	CHECK(print_value(1.5) == "1.5");

	CHECK(print_value(10) == "10");
	CHECK(print_value(10.0f) == "10.0");
	CHECK(print_value(10.0) == "10.0");

	CHECK(print_value(100) == "100");
	CHECK(print_value(100.0f) == "100.0");
	CHECK(print_value(100.0) == "100.0");

	CHECK(print_value(1000) == "1000");
	CHECK(print_value(1000.0f) == "1000.0");
	CHECK(print_value(1000.0) == "1000.0");

	CHECK(print_value(10000) == "10000");
	CHECK(print_value(10000.0f) == "10000.0");
	CHECK(print_value(10000.0) == "10000.0");

	CHECK(print_value(std::numeric_limits<double>::infinity()) == "inf");
	CHECK(print_value(-std::numeric_limits<double>::infinity()) == "-inf");
	CHECK(print_value(std::numeric_limits<double>::quiet_NaN()) == "nan");

	// only integers for large values;
	// large floats might get output as scientific notation and that's fine
	CHECK(print_value(10000000000) == "10000000000");
	CHECK(print_value(100000000000000) == "100000000000000");
}

TEST_CASE("nodes - value() int/float/bool conversions")
{
#define CHECK_VALUE_PASS(type, v) CHECK(n.value<type>() == static_cast<type>(v))
#define CHECK_VALUE_FAIL(type)	  CHECK(!n.value<type>())

	// bools
	{
		value val{ false };
		const node& n = val;
		CHECK_VALUE_PASS(bool, false);
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = true;
		CHECK_VALUE_PASS(bool, true);
		CHECK_VALUE_PASS(int8_t, 1);
		CHECK_VALUE_PASS(uint8_t, 1);
		CHECK_VALUE_PASS(int16_t, 1);
		CHECK_VALUE_PASS(uint16_t, 1);
		CHECK_VALUE_PASS(int32_t, 1);
		CHECK_VALUE_PASS(uint32_t, 1);
		CHECK_VALUE_PASS(int64_t, 1);
		CHECK_VALUE_PASS(uint64_t, 1);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}

	// ints
	{
		value val{ 0 };
		const node& n = val;
		CHECK_VALUE_PASS(bool, false); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_PASS(float, 0);
		CHECK_VALUE_PASS(double, 0);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 100;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, 100);
		CHECK_VALUE_PASS(uint8_t, 100);
		CHECK_VALUE_PASS(int16_t, 100);
		CHECK_VALUE_PASS(uint16_t, 100);
		CHECK_VALUE_PASS(int32_t, 100);
		CHECK_VALUE_PASS(uint32_t, 100);
		CHECK_VALUE_PASS(int64_t, 100);
		CHECK_VALUE_PASS(uint64_t, 100);
		CHECK_VALUE_PASS(float, 100);
		CHECK_VALUE_PASS(double, 100);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -100;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, -100);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -100);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -100);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -100);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -100);
		CHECK_VALUE_PASS(double, -100);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 1000;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, 1000);
		CHECK_VALUE_PASS(uint16_t, 1000);
		CHECK_VALUE_PASS(int32_t, 1000);
		CHECK_VALUE_PASS(uint32_t, 1000);
		CHECK_VALUE_PASS(int64_t, 1000);
		CHECK_VALUE_PASS(uint64_t, 1000);
		CHECK_VALUE_PASS(float, 1000);
		CHECK_VALUE_PASS(double, 1000);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1000;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -1000);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -1000);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -1000);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1000);
		CHECK_VALUE_PASS(double, -1000);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = (std::numeric_limits<int64_t>::max)();
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, (std::numeric_limits<int64_t>::max)());
		CHECK_VALUE_PASS(uint64_t, (std::numeric_limits<int64_t>::max)());
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = (std::numeric_limits<int64_t>::min)();
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, (std::numeric_limits<int64_t>::min)());
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}

	// floats
	{
		value val{ 0.0 };
		const node& n = val;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_PASS(float, 0);
		CHECK_VALUE_PASS(double, 0);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 1.0;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, 1);
		CHECK_VALUE_PASS(uint8_t, 1);
		CHECK_VALUE_PASS(int16_t, 1);
		CHECK_VALUE_PASS(uint16_t, 1);
		CHECK_VALUE_PASS(int32_t, 1);
		CHECK_VALUE_PASS(uint32_t, 1);
		CHECK_VALUE_PASS(int64_t, 1);
		CHECK_VALUE_PASS(uint64_t, 1);
		CHECK_VALUE_PASS(float, 1);
		CHECK_VALUE_PASS(double, 1);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1.0;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, -1);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -1);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -1);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -1);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1);
		CHECK_VALUE_PASS(double, -1);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 1.5;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, 1.5);
		CHECK_VALUE_PASS(double, 1.5);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1.5;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1.5);
		CHECK_VALUE_PASS(double, -1.5);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = std::numeric_limits<double>::infinity();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, std::numeric_limits<float>::infinity());
		CHECK_VALUE_PASS(double, std::numeric_limits<double>::infinity());
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -std::numeric_limits<double>::infinity();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -std::numeric_limits<float>::infinity());
		CHECK_VALUE_PASS(double, -std::numeric_limits<double>::infinity());
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = std::numeric_limits<double>::quiet_NaN();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		{
			auto fval = n.value<float>();
			REQUIRE(fval.has_value());
			CHECK(impl::fpclassify(*fval) == impl::fp_class::nan);
		}
		{
			auto fval = n.value<double>();
			REQUIRE(fval.has_value());
			CHECK(impl::fpclassify(*fval) == impl::fp_class::nan);
		}
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}
}

namespace string_like_types
{
	struct code_point_counted_string
	{
		std::string text;

		explicit code_point_counted_string(std::string_view sv) //
			: text{ sv }
		{}

		const char* data() const noexcept
		{
			return text.data();
		}

		size_t length() const noexcept
		{
			size_t count = {};
			for (const auto c : text)
				if ((static_cast<unsigned char>(c) & 0xC0u) != 0x80u)
					count++;
			return count;
		}
	};

	struct convertible_to_string : code_point_counted_string
	{
		using code_point_counted_string::code_point_counted_string;

		operator std::string() const
		{
			return text;
		}
	};

	struct convertible_to_string_view : code_point_counted_string
	{
		using code_point_counted_string::code_point_counted_string;

		operator std::string_view() const noexcept
		{
			return text;
		}
	};

	struct convertible_to_c_string : code_point_counted_string
	{
		using code_point_counted_string::code_point_counted_string;

		operator const char*() const noexcept
		{
			return text.c_str();
		}
	};

	struct derived_from_string : std::string
	{
		using std::string::string;
	};

	struct retrievable_string
	{
		std::string text;

		explicit retrievable_string(std::string_view sv) //
			: text{ sv }
		{}

		const char* data() const noexcept
		{
			return text.data();
		}

		size_t length() const noexcept
		{
			return text.length();
		}
	};

	struct initialiser_only_string : retrievable_string
	{
		explicit initialiser_only_string(const char* str) //
			: retrievable_string{ str }
		{}
	};

	struct move_only_string : retrievable_string
	{
		using retrievable_string::retrievable_string;

		move_only_string(move_only_string&&)	  = default;
		move_only_string(const move_only_string&) = delete;
	};

	struct potentially_throwing_copy_string
	{
		std::string_view text;

		explicit potentially_throwing_copy_string(std::string_view sv) noexcept //
			: text{ sv }
		{}

		potentially_throwing_copy_string(const potentially_throwing_copy_string& other) noexcept(false) //
			: text{ other.text }
		{}

		const char* data() const noexcept
		{
			return text.data();
		}

		size_t length() const noexcept
		{
			return text.length();
		}
	};

	template <typename T>
	void check_existing_conversion_is_used()
	{
		static constexpr auto text = "h\xC3\xA9llo w\xC3\xB6rld"sv;

		const T str{ text };
		REQUIRE(str.length() < text.length());
		REQUIRE(std::string_view{ str.data(), text.length() } == text);

		CHECK(value<std::string>{ str }.get() == text);
		CHECK(value<std::string>{ T{ text } }.get() == text);

		array arr;
		arr.emplace_back<std::string>(str);
		arr.push_back(T{ text });
		CHECK(*arr.get_as<std::string>(0) == text);
		CHECK(*arr.get_as<std::string>(1) == text);

		table tbl;
		tbl.emplace<std::string>("k", str);
		tbl.insert("inserted", T{ text });
		CHECK(tbl["k"] == text);
		CHECK(tbl["inserted"] == text);
	}
}

using namespace string_like_types;

TEST_CASE("string-like types - construction uses existing conversions to std::string")
{
	check_existing_conversion_is_used<convertible_to_string>();
	check_existing_conversion_is_used<convertible_to_string_view>();
	check_existing_conversion_is_used<convertible_to_c_string>();
}

TEST_CASE("string-like types - construction moves from types derived from std::string when std::string does")
{
	static_assert(std::is_nothrow_constructible_v<value<std::string>, derived_from_string>
				  == std::is_nothrow_constructible_v<std::string, derived_from_string>);

	if constexpr (std::is_nothrow_constructible_v<std::string, derived_from_string>)
	{
		derived_from_string str{ "a string long enough that no implementation can store it in a small-string buffer" };
		const auto buffer = std::as_const(str).data();

		const value<std::string> val{ std::move(str) };
		CHECK(val.get().data() == buffer);
	}
}

TEST_CASE("string-like types - lossless conversion implies equality comparison")
{
	table tbl{ { "k", "x"sv } };
	CHECK(tbl["k"] == retrievable_string{ "x" });
	CHECK(tbl["k"] != retrievable_string{ "y" });
	CHECK(tbl["missing"] != retrievable_string{ "x" });

	array arr{ "x"sv };
	CHECK(arr == std::vector<retrievable_string>{ retrievable_string{ "x" } });
	CHECK(arr != std::vector<retrievable_string>{ retrievable_string{ "y" } });
}

TEST_CASE("string-like types - lossless conversion implies insertion")
{
	static_assert(std::is_same_v<inserted_type_of<initialiser_only_string>, value<std::string>>);

	table tbl;
	tbl.insert("a", initialiser_only_string{ "x" });
	tbl.insert_or_assign("b", initialiser_only_string{ "x" });
	tbl.emplace("c", initialiser_only_string{ "x" });
	CHECK(tbl["a"] == "x"sv);
	CHECK(tbl["b"] == "x"sv);
	CHECK(tbl["c"] == "x"sv);

	array arr;
	arr.push_back(initialiser_only_string{ "x" });
	arr.emplace_back(initialiser_only_string{ "x" });
	CHECK(arr == array{ "x"sv, "x"sv });
}

TEST_CASE("string-like types - value_or() supports move-only retrieval targets")
{
	table tbl{ { "k", "x"sv } };
	CHECK(tbl["k"].value_or(move_only_string{ "y" }).text == "x"sv);
	CHECK(tbl["missing"].value_or(move_only_string{ "y" }).text == "y"sv);
}

TEST_CASE("string-like types - retrieval is not noexcept when copying can throw")
{
	static_assert(!noexcept(std::declval<const node&>().value<potentially_throwing_copy_string>()));
	static_assert(!noexcept(std::declval<const node&>().value_exact<potentially_throwing_copy_string>()));
	static_assert(!noexcept(std::declval<const node&>().value_or(std::declval<potentially_throwing_copy_string>())));
}

TEST_CASE("string-like types - retrieval works when copying can throw")
{
	table tbl{ { "k", "x"sv }, { "i", 1 } };
	const node& str		= *tbl.get("k");
	const node& integer = *tbl.get("i");

	CHECK(str.value<potentially_throwing_copy_string>()->text == "x"sv);
	CHECK(str.value_exact<potentially_throwing_copy_string>()->text == "x"sv);
	CHECK(str.value_or(potentially_throwing_copy_string{ "y"sv }).text == "x"sv);
	CHECK(!integer.value<potentially_throwing_copy_string>());
	CHECK(!integer.value_exact<potentially_throwing_copy_string>());
	CHECK(integer.value_or(potentially_throwing_copy_string{ "y"sv }).text == "y"sv);
}

#if TOML_CPP >= 20

namespace
{
	template <typename T>
	constexpr bool comparable_with_nodes = requires(const node_view<const node>& v, const T& t) { v == t; };
}

TEST_CASE("chrono types - retrieval")
{
	using namespace std::chrono;

	const auto time_of_day = hours{ 10 } + minutes{ 20 } + milliseconds{ 30040 };
	const auto local	   = local_days{ year{ 1987 } / 3 / 16 } + time_of_day;
	const table tbl{ { "dt", date_time{ local } },
					 { "odt", date_time{ local, minutes{ -570 } } },
					 { "d", date{ 1987, 3, 16 } },
					 { "t", toml::time{ 10, 20, 30, 40000000 } } };

	SECTION("local_time")
	{
		CHECK(tbl["dt"].value<local_time<nanoseconds>>() == local);
		CHECK(tbl["dt"].value<local_time<seconds>>() == time_point_cast<seconds>(local));
		CHECK(tbl["dt"].value_exact<local_time<nanoseconds>>() == local);
		CHECK(tbl["dt"].value_or(local_time<seconds>{}) == time_point_cast<seconds>(local));
		CHECK(tbl["d"].value_or(local_time<seconds>{}) == local_time<seconds>{});
		CHECK(tbl["missing"].value_or(local_time<seconds>{}) == local_time<seconds>{});
	}

	SECTION("local_time from an offset date-time")
	{
		CHECK(tbl["odt"].value<local_time<nanoseconds>>() == local);
		CHECK(!tbl["odt"].value_exact<local_time<nanoseconds>>());
	}

	SECTION("year_month_day")
	{
		CHECK(tbl["d"].value<year_month_day>() == year{ 1987 } / 3 / 16);
		CHECK(tbl["d"].value_exact<year_month_day>() == year{ 1987 } / 3 / 16);
		CHECK(!tbl["dt"].value<year_month_day>());
	}

	SECTION("hh_mm_ss")
	{
		CHECK(tbl["t"].value_or(hh_mm_ss<milliseconds>{}).to_duration() == time_of_day);
		CHECK(tbl["t"].value_or(hh_mm_ss<seconds>{}).to_duration() == seconds{ 37230 });
		CHECK(tbl["dt"].value_or(hh_mm_ss<seconds>{}).to_duration() == seconds{});

		const auto exact = tbl["t"].value_exact<hh_mm_ss<nanoseconds>>();
		REQUIRE(exact);
		CHECK(exact->to_duration() == time_of_day);
	}

	SECTION("out of range for the duration")
	{
		const table limits{ { "dt", date_time{ date{ 2038, 1, 19 }, toml::time{ 3, 14, 8 } } },
							{ "t", toml::time{ 0, 2, 8 } } };
		CHECK(!limits["dt"].value<local_time<duration<int32_t>>>());
		CHECK(limits["dt"].value_or(local_time<duration<int32_t>>{}) == local_time<duration<int32_t>>{});
		CHECK(!limits["t"].value<hh_mm_ss<duration<int8_t>>>());
		CHECK(limits["t"].value_or(hh_mm_ss<duration<int8_t>>{}).to_duration() == seconds{});
	}
}

TEST_CASE("chrono types - lossless conversion implies insertion and equality comparison")
{
	using namespace std::chrono;

	const auto local = local_days{ year{ 1987 } / 3 / 16 } + hours{ 10 } + minutes{ 20 } + seconds{ 30 };
	const auto ymd	 = year{ 1987 } / 3 / 16;
	const auto hms	 = hh_mm_ss{ hours{ 10 } + minutes{ 20 } + milliseconds{ 30500 } };

	static_assert(std::is_same_v<inserted_type_of<decltype(local)>, value<date_time>>);
	static_assert(std::is_same_v<inserted_type_of<decltype(ymd)>, value<date>>);
	static_assert(std::is_same_v<inserted_type_of<decltype(hms)>, value<toml::time>>);

	table tbl;
	tbl.insert("dt", local);
	tbl.insert_or_assign("d", ymd);
	tbl.emplace("t", hms);
	CHECK(tbl["dt"] == date_time{ date{ 1987, 3, 16 }, toml::time{ 10, 20, 30 } });
	CHECK(tbl["d"] == date{ 1987, 3, 16 });
	CHECK(tbl["t"] == toml::time{ 10, 20, 30, 500000000 });

	CHECK(tbl["dt"] == local);
	CHECK(tbl["dt"] != local + seconds{ 1 });
	CHECK(tbl["d"] == ymd);
	CHECK(tbl["t"] == hms);

	array arr;
	arr.push_back(local);
	arr.emplace_back(ymd);
	CHECK(arr == array{ date_time{ date{ 1987, 3, 16 }, toml::time{ 10, 20, 30 } }, date{ 1987, 3, 16 } });
	CHECK(array{ local } == std::vector{ local });
}

TEST_CASE("chrono types - conversion is lossless only for integral durations that convert exactly")
{
	using namespace std::chrono;
	using picoseconds		= duration<int64_t, std::pico>;
	using float_nanoseconds = duration<double, std::nano>;

	static_assert(comparable_with_nodes<local_time<seconds>>);
	static_assert(comparable_with_nodes<local_time<nanoseconds>>);
	static_assert(!comparable_with_nodes<local_time<picoseconds>>);
	static_assert(!comparable_with_nodes<local_time<float_nanoseconds>>);
	static_assert(comparable_with_nodes<hh_mm_ss<milliseconds>>);
	static_assert(!comparable_with_nodes<hh_mm_ss<picoseconds>>);

	static_assert(impl::can_represent_native<local_time<nanoseconds>>);
	static_assert(impl::can_represent_native<local_time<picoseconds>>);
	static_assert(!impl::can_represent_native<local_time<microseconds>>);
	static_assert(!impl::can_represent_native<local_time<float_nanoseconds>>);
	static_assert(impl::can_represent_native<hh_mm_ss<nanoseconds>>);
	static_assert(!impl::can_represent_native<hh_mm_ss<seconds>>);
}

#endif
