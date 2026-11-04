// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	template <typename T>
	std::string streamed(const T& val)
	{
		std::ostringstream ss;
		ss << val;
		return std::move(ss).str();
	}

	template <typename T, typename... Args>
	std::string printed(T val, Args&&... args)
	{
		std::ostringstream ss;
		impl::print_to_stream(ss, val, static_cast<Args&&>(args)...);
		return std::move(ss).str();
	}

	template <typename T>
	std::string reference_digits(T val, unsigned base)
	{
		using unsigned_type = std::make_unsigned_t<T>;
		auto magnitude		= static_cast<unsigned_type>(val);
		if (val < T{})
			magnitude = static_cast<unsigned_type>(unsigned_type{} - magnitude);

		std::string digits;
		do
		{
			digits.insert(digits.begin(), "0123456789ABCDEF"[magnitude % base]);
			magnitude = static_cast<unsigned_type>(magnitude / base);
		}
		while (magnitude);

		if (val < T{})
			digits.insert(digits.begin(), '-');
		return digits;
	}
}

TEST_CASE("printing - nodes use the toml formatter")
{
	const auto check_node = [](const auto& n, std::string_view expected)
	{
		CHECK(streamed(n) == expected);
		std::ostringstream ss;
		ss << toml_formatter{ n };
		CHECK(ss.str() == expected);
	};

	check_node(toml::value{ "it's"s }, R"("it's")"sv);
	check_node(toml::value{ 42 }, "42"sv);
	check_node(toml::value{ -0.5 }, "-0.5"sv);
	check_node(toml::value{ true }, "true"sv);
	check_node(toml::value{ toml::date{ 2021, 1, 2 } }, "2021-01-02"sv);
	check_node(toml::value{ toml::time{ 3, 4, 5 } }, "03:04:05"sv);
	check_node(toml::value{ toml::date_time{ { 2021, 1, 2 }, { 3, 4, 5 }, {} } }, "2021-01-02T03:04:05Z"sv);
	check_node(toml::array{ 1, "two"sv, toml::array{ 3.0 } }, "[ 1, 'two', [ 3.0 ] ]"sv);
	check_node(toml::table{ { "a", 1 }, { "b", toml::table{ { "c", false } } } }, "a = 1\n\n[b]\nc = false"sv);

	const auto tbl = toml::table{ { "a", toml::array{ 1, 2 } } };
	CHECK(streamed(tbl["a"]) == "[ 1, 2 ]"sv);
	CHECK(streamed(tbl["a"][1]) == "2"sv);
	CHECK(streamed(tbl["missing"]).empty());
}

TEMPLATE_TEST_CASE("printing - integers",
				   "",
				   signed char,
				   signed short,
				   signed int,
				   signed long,
				   signed long long,
				   unsigned char,
				   unsigned short,
				   unsigned int,
				   unsigned long,
				   unsigned long long)
{
	using limits   = std::numeric_limits<TestType>;
	const auto ten = TestType{ 10 };

	SECTION("zero")
	{
		CHECK(printed(TestType{}) == "0"sv);
		CHECK(printed(TestType{}, value_flags::format_as_hexadecimal) == "0"sv);
		CHECK(printed(TestType{}, value_flags::none, 3u) == "000"sv);
	}

	SECTION("bases")
	{
		CHECK(printed(ten) == "10"sv);
		CHECK(printed(ten, value_flags::format_as_binary) == "1010"sv);
		CHECK(printed(ten, value_flags::format_as_octal) == "12"sv);
		CHECK(printed(ten, value_flags::format_as_hexadecimal) == "A"sv);
		CHECK(printed(TestType{ 127 }, value_flags::format_as_hexadecimal) == "7F"sv);
	}

	SECTION("extremes in every base")
	{
		for (const auto val : { (limits::min)(), (limits::max)(), static_cast<TestType>((limits::max)() / 3) })
		{
			CAPTURE(+val);
			CHECK(printed(val) == reference_digits(val, 10u));
			const auto non_negative = val >= TestType{};
			CHECK(printed(val, value_flags::format_as_binary) == reference_digits(val, non_negative ? 2u : 10u));
			CHECK(printed(val, value_flags::format_as_octal) == reference_digits(val, non_negative ? 8u : 10u));
			CHECK(printed(val, value_flags::format_as_hexadecimal) == reference_digits(val, non_negative ? 16u : 10u));
		}
	}

	if constexpr (std::is_signed_v<TestType>)
	{
		SECTION("negative values ignore bases")
		{
			const auto minus_ten = static_cast<TestType>(-ten);
			CHECK(printed(minus_ten) == "-10"sv);
			CHECK(printed(minus_ten, value_flags::format_as_binary) == "-10"sv);
			CHECK(printed(minus_ten, value_flags::format_as_octal) == "-10"sv);
			CHECK(printed(minus_ten, value_flags::format_as_hexadecimal) == "-10"sv);
		}
	}

	SECTION("minimum digits")
	{
		CHECK(printed(TestType{ 5 }, value_flags::none, 4u) == "0005"sv);
		CHECK(printed(TestType{ 127 }, value_flags::format_as_hexadecimal, 4u) == "007F"sv);
		CHECK(printed(TestType{ 127 }, value_flags::none, 2u) == "127"sv);
		CHECK(printed(ten, value_flags::format_as_binary, 6u) == "001010"sv);
	}
}

TEMPLATE_TEST_CASE("printing - floating-point", "", float, double)
{
	using limits = std::numeric_limits<TestType>;

	SECTION("infinities and nans")
	{
		CHECK(printed(limits::infinity()) == "inf"sv);
		CHECK(printed(-limits::infinity()) == "-inf"sv);
		CHECK(printed(limits::quiet_NaN()) == "nan"sv);
		CHECK(printed(limits::infinity(), value_flags::none, true) == "inf"sv);
	}

	SECTION("integral values get a decimal point")
	{
		CHECK(printed(TestType{ 1 }) == "1.0"sv);
		CHECK(printed(TestType{ -0.0 }) == "-0.0"sv);
		CHECK(printed(TestType{ 16777216 }) == "16777216.0"sv);
		CHECK(printed(TestType{ 1 }, value_flags::none, true) == "1.0"sv);
	}

	SECTION("exactly representable values")
	{
		CHECK(printed(TestType{ 0.5 }) == "0.5"sv);
		CHECK(printed(TestType{ -2.25 }) == "-2.25"sv);
		CHECK(printed(TestType{ 1024 }) == "1024.0"sv);
	}

	SECTION("full precision round-trips")
	{
		for (const auto val : { static_cast<TestType>(0.1),
								TestType{ 1 } / TestType{ 3 },
								(limits::max)(),
								static_cast<TestType>(1e-30) })
		{
			const auto str = printed(val);
			CAPTURE(str);
			std::istringstream ss{ str };
			ss.imbue(std::locale::classic());
			TestType parsed{};
			ss >> parsed;
			CHECK(parsed == val);
		}
	}

	SECTION("relaxed precision")
	{
		CHECK(printed(static_cast<TestType>(0.1), value_flags::none, true) == "0.1"sv);
		CHECK(printed(TestType{ 1 } / TestType{ 3 }, value_flags::none, true) == "0.333333"sv);
		CHECK(printed(TestType{ 123456.75 }, value_flags::none, true) == "123457.0"sv);
	}
}

TEST_CASE("printing - booleans")
{
	CHECK(printed(true) == "true"sv);
	CHECK(printed(false) == "false"sv);
}
