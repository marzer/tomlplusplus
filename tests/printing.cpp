// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_WARNINGS;
#if __has_include(<charconv>)
#include <charconv>
#endif
#include <cmath>
#include <cstring>
#include <random>
#include <tuple>
#if defined(__SSE2__) || defined(_M_X64)
#include <pmmintrin.h>
#endif
TOML_ENABLE_WARNINGS;

#if TOML_HAS_FLOAT_CHARCONV                                                                                            \
	|| (defined(_LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT) && _LIBCPP_AVAILABILITY_HAS_TO_CHARS_FLOATING_POINT)
#define HAS_FLOAT_TO_CHARS 1
#else
#define HAS_FLOAT_TO_CHARS 0
#endif

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

#if HAS_FLOAT_TO_CHARS

	template <typename T>
	std::string reference_float(T val, bool relaxed)
	{
		std::string text = std::signbit(val) ? "-" : "";
		val				 = std::abs(val);
		if (val == T{})
			return text + "0.0";

		const auto scientific = [&](auto... precision)
		{
			char buf[64];
			const auto str = std::string{ buf, std::to_chars(buf, buf + sizeof(buf), val, precision...).ptr };
			const auto e   = str.find('e');
			return std::pair{ str.substr(0, 1) + (e > 1u ? str.substr(2u, e - 2u) : ""s),
							  std::stoi(str.substr(e + 1u)) };
		};

		auto [digits, exponent] = scientific(std::chars_format::scientific);
		if (relaxed && digits.length() > 6u)
			std::tie(digits, exponent) = scientific(std::chars_format::scientific, 5);
		while (digits.length() > 1u && digits.back() == '0')
			digits.pop_back();

		if (exponent < -4 || exponent >= (relaxed ? 6 : 16))
		{
			text += digits.substr(0, 1);
			if (digits.length() > 1u)
				text += "." + digits.substr(1);
			const auto magnitude = std::to_string(exponent < 0 ? -exponent : exponent);
			return text + (exponent < 0 ? "e-" : "e+") + (magnitude.length() < 2u ? "0" : "") + magnitude;
		}
		if (exponent < 0)
			return text + "0." + std::string(static_cast<size_t>(-exponent - 1), '0') + digits;

		const auto whole = static_cast<size_t>(exponent) + 1u;
		if (digits.length() <= whole)
			return text + digits + std::string(whole - digits.length(), '0') + ".0";
		return text + digits.substr(0, whole) + "." + digits.substr(whole);
	}

#endif
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
		CHECK(printed(TestType{}, value_flags::none, 100u) == std::string(100u, '0'));
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
		CHECK(printed(TestType{ 5 }, value_flags::none, 100u) == std::string(99u, '0') + "5");
		CHECK(printed((limits::max)(), value_flags::format_as_binary, 100u)
			  == std::string(100u - reference_digits((limits::max)(), 2u).size(), '0')
					 + reference_digits((limits::max)(), 2u));
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

#if HAS_FLOAT_TO_CHARS

	SECTION("shortest digits in python's repr() layout, or 6 digits in %g's layout when relaxed")
	{
		using bits_type					= std::conditional_t<sizeof(TestType) == sizeof(uint32_t), uint32_t, uint64_t>;
		static constexpr auto from_bits = [](bits_type bits) noexcept
		{
			TestType val;
			std::memcpy(&val, &bits, sizeof(val));
			return val;
		};

		std::vector<TestType> values{ (limits::min)(),
									  (limits::max)(),
									  limits::denorm_min(),
									  limits::epsilon(),
									  std::nextafter(static_cast<TestType>(1e-4), TestType{}),
									  static_cast<TestType>(1e-4),
									  std::nextafter(static_cast<TestType>(1e16), TestType{}),
									  static_cast<TestType>(1e16) };
		for (int exp = limits::min_exponent - limits::digits; exp < limits::max_exponent; exp++)
			values.push_back(std::ldexp(TestType{ 1 }, exp));
		for (int exp = limits::min_exponent10; exp <= limits::max_exponent10; exp++)
			values.push_back(static_cast<TestType>(std::pow(10.0, exp)));

		std::mt19937_64 rng{ 0x5407u };
		static constexpr auto subnormal_count = bits_type{ 1 } << (limits::digits - 1);
		for (size_t i = 0; i < 20000u; i++)
		{
			const auto bits = static_cast<bits_type>(rng());
			if (const auto val = from_bits(bits); std::isfinite(val))
				values.push_back(val);
			const auto subnormal = from_bits(static_cast<bits_type>(bits % subnormal_count));
			values.push_back(i % 2u ? -subnormal : subnormal);
		}

		std::vector<std::string> mismatches;
		for (const auto val : values)
		{
			for (const bool relaxed : { false, true })
			{
				const auto expected = reference_float(val, relaxed);
				const auto str		= printed(val, value_flags::none, relaxed);
				if (str != expected && mismatches.size() < 10u)
					mismatches.push_back(str + " should be " + expected);
			}
		}
		CAPTURE(mismatches);
		CHECK(mismatches.empty());
	}

#endif
}

#if !TOML_FLOAT_CHARCONV && (defined(__SSE2__) || defined(_M_X64))

TEST_CASE("printing - subnormals with denormals-are-zero set")
{
	using limits = std::numeric_limits<double>;

	for (const auto val : { limits::denorm_min(), std::nextafter((limits::min)(), 0.0) })
	{
		const auto node		= toml::value{ val };
		const auto expected = streamed(node);
		const auto csr		= _mm_getcsr();
		_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
		_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
		const auto str = streamed(node);
		_mm_setcsr(csr);
		CHECK(str == expected);
	}
}

#endif

TEST_CASE("printing - booleans")
{
	CHECK(printed(true) == "true"sv);
	CHECK(printed(false) == "false"sv);
}
