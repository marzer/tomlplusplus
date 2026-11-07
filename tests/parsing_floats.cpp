// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

TEST_CASE("parsing - floats")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								# fractional
								flt1 = +1.0
								flt2 = 3.1415
								flt3 = -0.01

								# exponent
								flt4 = 5e+22
								flt5 = 1e06
								flt6 = -2E-2

								# both
								flt7 = 6.626e-34

								flt8 = 224_617.445_991_228
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["flt1"] == 1.0);
							   CHECK(tbl["flt2"] == 3.1415);
							   CHECK(tbl["flt3"] == -0.01);
							   CHECK(tbl["flt4"].as<double>()->get() == 5e+22_a);
							   CHECK(tbl["flt5"].as<double>()->get() == 1e6_a);
							   CHECK(tbl["flt6"] == -2E-2);
							   CHECK(tbl["flt7"].as<double>()->get() == 6.626e-34_a);
							   CHECK(tbl["flt8"].as<double>()->get() == 224617.445991228_a);
						   });
}

TEST_CASE("parsing - floats (underscores)")
{
	// "Each underscore must be surrounded by at least one digit on each side."
	parsing_should_fail(FILE_LINE_ARGS,
						"flt8 = 224_617.445_991_228_"sv,
						"1:28: Error while parsing floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt8 = _224_617.445_991_228"sv,
						"1:8: Error while parsing value: values may not begin with underscores"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt8 = 224__617.445_991_228"sv,
						"1:12: Error while parsing floating-point: underscores may only follow digits"sv);
}

TEST_CASE("parsing - floats (long values with underscores)")
{
	for (const auto key : { "a"sv, "ab"sv })
	{
		for (size_t pairs = 60; pairs <= 110; pairs++)
		{
			std::string doc = std::string{ key } + " = 1.";
			for (size_t i = 0; i < pairs; i++)
				doc += "0_";

			parsing_should_succeed(
				FILE_LINE_ARGS,
				doc + "1e5",
				[&](table&& tbl)
				{
					CHECK(tbl[key] == 100000.0);
					REQUIRE(tbl[key].node());
					const auto& region = tbl[key].node()->source();
					CHECK(region.begin == source_position{ 1, static_cast<source_index>(key.length() + 4u) });
					CHECK(region.end == source_position{ 1, static_cast<source_index>(doc.length() + 4u) });
				});

			parsing_should_fail(FILE_LINE_ARGS,
								doc + "1_\n",
								"1:" + std::to_string(doc.length() + 3u)
									+ ": Error while parsing floating-point: underscores must be followed by digits");
		}
	}

#if TOML_LANG_UNRELEASED
	std::string value = "0x";
	for (size_t i = 0; i < 23u; i++)
		value += "1_";
	value += "1.";
	for (size_t i = 0; i < 23u; i++)
		value += "1_";
	value += "1p+";
	for (size_t i = 0; i < 18u; i++)
		value += "0_";
	value += "1_";

	for (size_t key_length = 1; key_length <= 40u; key_length++)
	{
		const auto doc = std::string(key_length, 'k') + " = " + value;
		parsing_should_fail(
			FILE_LINE_ARGS,
			doc,
			"1:" + std::to_string(doc.length() + 1u)
				+ ": Error while parsing hexadecimal floating-point: underscores must be followed by digits");
	}
#endif
}

TEST_CASE("parsing - floats (signed zeroes)")
{
	// "Float values -0.0 and +0.0 are valid and should map according to IEEE 754."
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(zeroes = [-0.0, +0.0])"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["zeroes"][0] == -0.0);
							   CHECK(tbl["zeroes"][1] == +0.0);
							   CHECK(std::signbit(*tbl["zeroes"][0].value<double>()));
							   CHECK(!std::signbit(*tbl["zeroes"][1].value<double>()));
						   });
}

TEST_CASE("parsing - floats (integer part)")
{
	// "A float consists of an integer part followed by a fractional part and/or an exponent part"
	// (i.e. omitting the leading digits before the '.' is not legal in TOML)
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = .1"sv,
						"1:7: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +.1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -.1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = .1e1"sv,
						"1:7: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = .1e+1"sv,
						"1:7: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = .1e-1"sv,
						"1:7: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +.1e1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +.1e+1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +.1e-1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -.1e1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -.1e+1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -.1e-1"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw '.'"sv);
}

TEST_CASE("parsing - floats (fractional part)")
{
	// likewise, so is omitting digits _after_ the '.'
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1."sv,
						"1:9: Error while parsing floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +1."sv,
						"1:10: Error while parsing floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -1."sv,
						"1:10: Error while parsing floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.e1"sv,
						"1:9: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.e+1"sv,
						"1:9: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.e-1"sv,
						"1:9: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +1.e1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +1.e+1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +1.e-1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -1.e1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -1.e+1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -1.e-1"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
}

TEST_CASE("parsing - floats (values)")
{
	parse_expected_value(FILE_LINE_ARGS, "1e1"sv, 1e1);
	parse_expected_value(FILE_LINE_ARGS, "1e+1"sv, 1e+1);
	parse_expected_value(FILE_LINE_ARGS, "1e-1"sv, 1e-1);
	parse_expected_value(FILE_LINE_ARGS, "1.0"sv, 1.0);
	parse_expected_value(FILE_LINE_ARGS, "1.0e1"sv, 1.0e1);
	parse_expected_value(FILE_LINE_ARGS, "1.0e+1"sv, 1.0e+1);
	parse_expected_value(FILE_LINE_ARGS, "1.0e-1"sv, 1.0e-1);
	parse_expected_value(FILE_LINE_ARGS, "+1e1"sv, +1e1);
	parse_expected_value(FILE_LINE_ARGS, "+1.0"sv, +1.0);
	parse_expected_value(FILE_LINE_ARGS, "+1.0e1"sv, +1.0e1);
	parse_expected_value(FILE_LINE_ARGS, "+1.0e+1"sv, +1.0e+1);
	parse_expected_value(FILE_LINE_ARGS, "+1.0e-1"sv, +1.0e-1);
	parse_expected_value(FILE_LINE_ARGS, "-1.0e+1"sv, -1.0e+1);
	parse_expected_value(FILE_LINE_ARGS, "-1e1"sv, -1e1);
	parse_expected_value(FILE_LINE_ARGS, "-1.0"sv, -1.0);
	parse_expected_value(FILE_LINE_ARGS, "-1.0e1"sv, -1.0e1);
	parse_expected_value(FILE_LINE_ARGS, "-1.0e-1"sv, -1.0e-1);
	parse_expected_value(FILE_LINE_ARGS, "0.1"sv, 0.1);
	parse_expected_value(FILE_LINE_ARGS, "0.001"sv, 0.001);
	parse_expected_value(FILE_LINE_ARGS, "0.100"sv, 0.100);
	parse_expected_value(FILE_LINE_ARGS, "+3.14"sv, +3.14);
	parse_expected_value(FILE_LINE_ARGS, "-3.14"sv, -3.14);
	parse_expected_value(FILE_LINE_ARGS, "3.1415_9265_3589"sv, 3.141592653589);
	parse_expected_value(FILE_LINE_ARGS, "+3.1415_9265_3589"sv, +3.141592653589);
	parse_expected_value(FILE_LINE_ARGS, "-3.1415_9265_3589"sv, -3.141592653589);
	parse_expected_value(FILE_LINE_ARGS, "123_456.789"sv, 123456.789);
	parse_expected_value(FILE_LINE_ARGS, "+123_456.789"sv, +123456.789);
	parse_expected_value(FILE_LINE_ARGS, "-123_456.789"sv, -123456.789);
	parse_expected_value(FILE_LINE_ARGS, "+0.0"sv, +0.0);
	parse_expected_value(FILE_LINE_ARGS, "-0.0"sv, -0.0);
	parse_expected_value(FILE_LINE_ARGS, "1e10"sv, 1e10);
	parse_expected_value(FILE_LINE_ARGS, "1e+10"sv, 1e+10);
	parse_expected_value(FILE_LINE_ARGS, "1e-10"sv, 1e-10);
	parse_expected_value(FILE_LINE_ARGS, "+1e10"sv, +1e10);
	parse_expected_value(FILE_LINE_ARGS, "+1e+10"sv, +1e+10);
	parse_expected_value(FILE_LINE_ARGS, "+1e-10"sv, +1e-10);
	parse_expected_value(FILE_LINE_ARGS, "-1e10"sv, -1e10);
	parse_expected_value(FILE_LINE_ARGS, "-1e+10"sv, -1e+10);
	parse_expected_value(FILE_LINE_ARGS, "-1e-10"sv, -1e-10);
	parse_expected_value(FILE_LINE_ARGS, "123e-10"sv, 123e-10);
	parse_expected_value(FILE_LINE_ARGS, "1E10"sv, 1E10);
	parse_expected_value(FILE_LINE_ARGS, "1E+10"sv, 1E+10);
	parse_expected_value(FILE_LINE_ARGS, "1E-10"sv, 1E-10);
	parse_expected_value(FILE_LINE_ARGS, "123E-10"sv, 123E-10);
	parse_expected_value(FILE_LINE_ARGS, "1_2_3E-10"sv, 123E-10);
	parse_expected_value(FILE_LINE_ARGS, "1_2_3E-1_0"sv, 123E-10);
	parse_expected_value(FILE_LINE_ARGS, "+0e0"sv, +0e0);
	parse_expected_value(FILE_LINE_ARGS, "-0e0"sv, -0e0);
	parse_expected_value(FILE_LINE_ARGS, "1_2_3E-01"sv, 123E-01);
	parse_expected_value(FILE_LINE_ARGS, "1_2_3E-0_1"sv, 123E-01);
	parse_expected_value(FILE_LINE_ARGS, "6.02e23"sv, 6.02e23);
	parse_expected_value(FILE_LINE_ARGS, "6.02e+23"sv, 6.02e+23);
	parse_expected_value(FILE_LINE_ARGS, "1.112_650_06e-17"sv, 1.11265006e-17);
	parse_expected_value(FILE_LINE_ARGS, "0.010284358729827818"sv, 0.010284358729827818);
	parse_expected_value(FILE_LINE_ARGS, "0.010284358729827818"sv, 0.010284358729827818);
	parse_expected_value(FILE_LINE_ARGS, "0.0102"sv, 0.0102);
	parse_expected_value(FILE_LINE_ARGS, "10.0102"sv, 10.0102);
	parse_expected_value(FILE_LINE_ARGS, "10.010284358729828"sv, 10.010284358729828);
	parse_expected_value(FILE_LINE_ARGS, "10.0"sv, 10.0);
}

TEST_CASE("parsing - floats (subnormal values)")
{
	parse_expected_value(FILE_LINE_ARGS, "4.9406564584124654e-324"sv, std::numeric_limits<double>::denorm_min());
	parse_expected_value(FILE_LINE_ARGS, "-4.9406564584124654e-324"sv, -std::numeric_limits<double>::denorm_min());
	parse_expected_value(FILE_LINE_ARGS, "2.2250738585072009e-308"sv, 2.2250738585072009e-308);
	parse_expected_value(FILE_LINE_ARGS, "1e-320"sv, 1e-320);
}

TEST_CASE("parsing - floats (exact values)")
{
	static constexpr auto bits = [](double val) noexcept
	{
		uint64_t out;
		std::memcpy(&out, &val, sizeof(out));
		return out;
	};

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
								smallest_subnormal = 5e-324
								rounds_up_to_smallest_subnormal = 3e-324
								just_over_half_smallest_subnormal = 2.4703282292062328e-324
								rounds_down_to_largest_subnormal = 2.2250738585072011e-308
								rounds_up_to_smallest_normal = 2.2250738585072012e-308
								negative_rounds_up_to_smallest_normal = -2.2250738585072012e-308
								smallest_normal = 2.2250738585072014e-308
								tie_to_even = 9007199254740993.0
								hard_to_round = 1e23
								tenth = 0.1
								largest = 1.7976931348623157e308
								rounds_down_to_largest = 1.7976931348623158e308
								fraction_and_exponent = 123.456e-2
							)"sv,
		[](table&& tbl)
		{
			CHECK(bits(*tbl["smallest_subnormal"].value<double>()) == 0x0000000000000001u);
			CHECK(bits(*tbl["rounds_up_to_smallest_subnormal"].value<double>()) == 0x0000000000000001u);
			CHECK(bits(*tbl["just_over_half_smallest_subnormal"].value<double>()) == 0x0000000000000001u);
			CHECK(bits(*tbl["rounds_down_to_largest_subnormal"].value<double>()) == 0x000FFFFFFFFFFFFFu);
			CHECK(bits(*tbl["rounds_up_to_smallest_normal"].value<double>()) == 0x0010000000000000u);
			CHECK(bits(*tbl["negative_rounds_up_to_smallest_normal"].value<double>()) == 0x8010000000000000u);
			CHECK(bits(*tbl["smallest_normal"].value<double>()) == 0x0010000000000000u);
			CHECK(bits(*tbl["tie_to_even"].value<double>()) == 0x4340000000000000u);
			CHECK(bits(*tbl["hard_to_round"].value<double>()) == 0x44B52D02C7E14AF6u);
			CHECK(bits(*tbl["tenth"].value<double>()) == 0x3FB999999999999Au);
			CHECK(bits(*tbl["largest"].value<double>()) == 0x7FEFFFFFFFFFFFFFu);
			CHECK(bits(*tbl["rounds_down_to_largest"].value<double>()) == 0x7FEFFFFFFFFFFFFFu);
			CHECK(bits(*tbl["fraction_and_exponent"].value<double>()) == 0x3FF3C0C1FC8F3238u);
		});
}

TEST_CASE("parsing - floats (underflow to zero)")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								positive = [
									1e-999,
									2e-324,
									2.4703282292062327e-324,
									1e-99999999999999999999,
									1234567890123456789012345678901234567890e-400,
									0.000_001e-400,
								]
								negative = [
									-1e-999,
									-2e-324,
									-1e-99999999999999999999,
								]
							)"sv,
						   [](table&& tbl)
						   {
							   for (auto&& [arr, negative] : { std::pair{ tbl["positive"].as_array(), false },
															   std::pair{ tbl["negative"].as_array(), true } })
							   {
								   REQUIRE(arr);
								   for (auto&& elem : *arr)
								   {
									   const auto val = elem.value<double>();
									   REQUIRE(val);
									   CHECK(*val == 0.0);
									   CHECK(std::signbit(*val) == negative);
								   }
							   }
						   });
}

TEST_CASE("parsing - floats (hexadecimal)")
{
// toml/issues/562 (hexfloats)
#if TOML_LANG_UNRELEASED
	parse_expected_value(FILE_LINE_ARGS, "       0x1.2p3"sv, 0x1.2p3);
	parse_expected_value(FILE_LINE_ARGS, "        0x10p1"sv, 0x10p1);
	parse_expected_value(FILE_LINE_ARGS, "       0x10p-1"sv, 0x10p-1);
	parse_expected_value(FILE_LINE_ARGS, "       0x10p+1"sv, 0x10p+1);
	parse_expected_value(FILE_LINE_ARGS, "       -0x10p1"sv, -0x10p1);
	parse_expected_value(FILE_LINE_ARGS, "      -0x10p-1"sv, -0x10p-1);
	parse_expected_value(FILE_LINE_ARGS, "       +0x10p1"sv, +0x10p1);
	parse_expected_value(FILE_LINE_ARGS, "      +0x10p+1"sv, +0x10p+1);
	parse_expected_value(FILE_LINE_ARGS, "      -0x10p+1"sv, -0x10p+1);
	parse_expected_value(FILE_LINE_ARGS, "      +0x10p-1"sv, +0x10p-1);
	parse_expected_value(FILE_LINE_ARGS, "      0x10.1p1"sv, 0x10.1p1);
	parse_expected_value(FILE_LINE_ARGS, "     0x10.1p-1"sv, 0x10.1p-1);
	parse_expected_value(FILE_LINE_ARGS, "     0x10.1p+1"sv, 0x10.1p+1);
	parse_expected_value(FILE_LINE_ARGS, "     -0x10.1p1"sv, -0x10.1p1);
	parse_expected_value(FILE_LINE_ARGS, "    -0x10.1p-1"sv, -0x10.1p-1);
	parse_expected_value(FILE_LINE_ARGS, "     +0x10.1p1"sv, +0x10.1p1);
	parse_expected_value(FILE_LINE_ARGS, "    +0x10.1p+1"sv, +0x10.1p+1);
	parse_expected_value(FILE_LINE_ARGS, "    -0x10.1p+1"sv, -0x10.1p+1);
	parse_expected_value(FILE_LINE_ARGS, "    +0x10.1p-1"sv, +0x10.1p-1);
	parse_expected_value(FILE_LINE_ARGS, "      0x01.8p1"sv, 0x1.8p1);
	parse_expected_value(FILE_LINE_ARGS, "      0x1.08p1"sv, 0x1.08p1);
	parse_expected_value(FILE_LINE_ARGS, "        0x1p01"sv, 0x1p1);
	parse_expected_value(FILE_LINE_ARGS, "      0x00.1p4"sv, 0x0.1p4);
	parse_expected_value(FILE_LINE_ARGS, "         0x0p0"sv, 0.0);
	parse_expected_value(FILE_LINE_ARGS, "0x123456789p0"sv, 0x123456789p0);
	parse_expected_value(FILE_LINE_ARGS, "0xFFFFFFFFFp0"sv, 0xFFFFFFFFFp0);
	parse_expected_value(FILE_LINE_ARGS, "0x1.123456789abcdefp0"sv, 0x1.123456789abcdefp0);
	parse_expected_value(FILE_LINE_ARGS, "0x123456789abcdef0123p0"sv, 0x123456789abcdef0123p0);
	parse_expected_value(FILE_LINE_ARGS, "0x0.000000000000000000000001p0"sv, 0x1p-96);
	parse_expected_value(FILE_LINE_ARGS, "0x1.00000000000008p0"sv, 1.0);
	parse_expected_value(FILE_LINE_ARGS, "0x1.000000000000080000001p0"sv, 0x1.0000000000001p0);
	parse_expected_value(FILE_LINE_ARGS, "0x1p000000000000000000010"sv, 1024.0);
	parse_expected_value(FILE_LINE_ARGS, "0x1p-4294967297"sv, 0.0);
	parse_expected_value(FILE_LINE_ARGS, "0xA000000000000001p-1136"sv, 0x3p-1074);
	parse_expected_value(FILE_LINE_ARGS, "0x1.0000000000000000000001p-1075"sv, 0x1p-1074);
	parse_expected_value(FILE_LINE_ARGS, "0x0.8000000000000000000001p-1074"sv, 0x1p-1074);
	parse_expected_value(FILE_LINE_ARGS, "-0x1.0000000000000000000001p-1075"sv, -0x1p-1074);
	parse_expected_value(FILE_LINE_ARGS, "0x1.fffffffffffffp1023"sv, 0x1.fffffffffffffp1023);
	parse_expected_value(FILE_LINE_ARGS, "-0x1.fffffffffffffp1023"sv, -0x1.fffffffffffffp1023);
#else
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =     0x10p1"sv,
		"1:12: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =    0x10p-1"sv,
		"1:11: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =    0x10p+1"sv,
		"1:11: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =    -0x10p1"sv,
		"1:11: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =   -0x10p-1"sv,
		"1:10: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =    +0x10p1"sv,
		"1:11: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =   +0x10p+1"sv,
		"1:10: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =   -0x10p+1"sv,
		"1:10: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =   +0x10p-1"sv,
		"1:10: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =   0x10.1p1"sv,
		"1:10: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =  0x10.1p-1"sv,
		"1:9: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =  0x10.1p+1"sv,
		"1:9: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =  -0x10.1p1"sv,
		"1:9: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val = -0x10.1p-1"sv,
		"1:8: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val =  +0x10.1p1"sv,
		"1:9: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val = +0x10.1p+1"sv,
		"1:8: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val = -0x10.1p+1"sv,
		"1:8: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		" val = +0x10.1p-1"sv,
		"1:8: Error while parsing hexadecimal floating-point: hexadecimal floating-point values are not supported in TOML 1.1.0 and earlier"sv);
#endif
}

TEST_CASE("parsing - inf and nan")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								# infinity
								sf1 = inf  # positive infinity
								sf2 = +inf # positive infinity
								sf3 = -inf # negative infinity

								# not a number
								sf4 = nan  # actual sNaN/qNaN encoding is implementation specific
								sf5 = +nan # same as `nan`
								sf6 = -nan # valid, actual encoding is implementation specific
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(impl::fpclassify(**tbl["sf1"].as<double>()) == impl::fp_class::pos_inf);
							   CHECK(impl::fpclassify(**tbl["sf2"].as<double>()) == impl::fp_class::pos_inf);
							   CHECK(impl::fpclassify(**tbl["sf3"].as<double>()) == impl::fp_class::neg_inf);
							   CHECK(impl::fpclassify(**tbl["sf4"].as<double>()) == impl::fp_class::nan);
							   CHECK(impl::fpclassify(**tbl["sf5"].as<double>()) == impl::fp_class::nan);
							   CHECK(impl::fpclassify(**tbl["sf6"].as<double>()) == impl::fp_class::nan);
						   });
}

TEST_CASE("parsing - inf and nan (malformed)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						" val =     NaN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw 'N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =     Nan "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw 'N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =     NAN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw 'N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    +NaN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '+N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    +Nan "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '+N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    +NAN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '+N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    -NaN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '-N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    -Nan "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '-N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    -NAN "sv,
						"1:12: Error while parsing floating-point: expected 'nan', saw '-N'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =   nan.1 "sv,
						"1:13: Error while parsing floating-point: expected end of value, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =   nan,1 "sv,
						"1:13: Error while parsing key-value pair: expected a comment or whitespace, saw ','"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    nan. "sv,
						"1:14: Error while parsing floating-point: expected end of value, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    nan, "sv,
						"1:14: Error while parsing key-value pair: expected a comment or whitespace, saw ','"sv);

	parsing_should_fail(FILE_LINE_ARGS,
						" val =     Inf "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw 'I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =     INF "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw 'I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    +Inf "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw '+I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    +INF "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw '+I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    -Inf "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw '-I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    -INF "sv,
						"1:12: Error while parsing floating-point: expected 'inf', saw '-I'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =   inf.1 "sv,
						"1:13: Error while parsing floating-point: expected end of value, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =   inf,1 "sv,
						"1:13: Error while parsing key-value pair: expected a comment or whitespace, saw ','"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    inf. "sv,
						"1:14: Error while parsing floating-point: expected end of value, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						" val =    inf, "sv,
						"1:14: Error while parsing key-value pair: expected a comment or whitespace, saw ','"sv);
}

TEST_CASE("parsing - floats (malformed)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1_.5"sv,
						"1:9: Error while parsing floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.5_e3"sv,
						"1:11: Error while parsing floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.5e_3"sv,
						"1:11: Error while parsing floating-point: underscores may only follow digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.5_ "sv,
						"1:11: Error while parsing floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e.5"sv,
						"1:9: Error while parsing floating-point: expected exponent decimal digit or sign, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e5.3"sv,
						"1:10: Error while parsing floating-point: expected exponent decimal digit, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.2.3"sv,
						"1:10: Error while parsing floating-point: expected decimal digit or exponent, saw '.'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1.5x"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -12.5z"sv,
						"1:12: Error while parsing floating-point: expected decimal digit, saw 'z'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 12.5tons"sv,
						"1:11: Error while parsing floating-point: expected decimal digit, saw 't'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e+x"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +e5"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -E+1_0"sv,
						"1:8: Error while parsing floating-point: expected decimal digit, saw 'E'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"arr = [ -e-5 ]"sv,
						"1:10: Error while parsing floating-point: expected decimal digit, saw 'e'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e+"sv,
						"1:10: Error while parsing floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e999"sv,
						"1:7: Error while parsing floating-point: '1e999' is not representable in 64 bits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = -1e999"sv,
						"1:7: Error while parsing floating-point: '-1e999' is not representable in 64 bits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = +1e999"sv,
						"1:7: Error while parsing floating-point: '+1e999' is not representable in 64 bits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 1.7976931348623159e308"sv,
		"1:7: Error while parsing floating-point: '1.7976931348623159e308' is not representable in 64 bits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = -1e99999999999999999999"sv,
		"1:7: Error while parsing floating-point: '-1e99999999999999999999' is not representable in 64 bits"sv);
}

#if TOML_LANG_UNRELEASED

TEST_CASE("parsing - hexadecimal floats (malformed)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1p"sv,
						"1:11: Error while parsing hexadecimal floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1p+"sv,
						"1:12: Error while parsing hexadecimal floating-point: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1p "sv,
						"1:11: Error while parsing hexadecimal floating-point: missing exponent"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1p+ "sv,
						"1:12: Error while parsing hexadecimal floating-point: missing exponent"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1p1_ "sv,
		"1:13: Error while parsing hexadecimal floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1.8p_1"sv,
						"1:13: Error while parsing hexadecimal floating-point: underscores may only follow digits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1_.8p1"sv,
		"1:11: Error while parsing hexadecimal floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1.8_p1"sv,
		"1:13: Error while parsing hexadecimal floating-point: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 0x1.8p1.5"sv,
						"1:14: Error while parsing hexadecimal floating-point: expected exponent digit, saw '.'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1.8p.5"sv,
		"1:13: Error while parsing hexadecimal floating-point: expected exponent digit or sign, saw '.'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1.8.8p1"sv,
		"1:12: Error while parsing hexadecimal floating-point: expected hexadecimal digit or exponent, saw '.'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1pg"sv,
		"1:11: Error while parsing hexadecimal floating-point: expected exponent digit or sign, saw 'g'"sv);
}

TEST_CASE("parsing - hexadecimal floats (out of range)")
{
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1p99999"sv,
		"1:7: Error while parsing hexadecimal floating-point: '0x1p99999' is not representable in 64 bits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = -0x1p1024"sv,
		"1:7: Error while parsing hexadecimal floating-point: '-0x1p1024' is not representable in 64 bits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = +0X1_0.8P+1023"sv,
		"1:7: Error while parsing hexadecimal floating-point: '+0X10.8P+1023' is not representable in 64 bits"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1.fffffffffffff8p1023"sv,
		"1:7: Error while parsing hexadecimal floating-point: '0x1.fffffffffffff8p1023' is not representable in 64 bits"sv);
}

#endif

TEST_CASE("parsing - floats (implementation limits)")
{
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 1." + std::string(200u, '1'),
		"1:135: Error while parsing floating-point: exceeds length limit of 128 digits (consider using exponent notation)"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"flt = 1e" + std::string(200u, '1'),
						"1:135: Error while parsing floating-point: exceeds length limit of 128 digits"sv);
#if TOML_LANG_UNRELEASED
	parsing_should_fail(
		FILE_LINE_ARGS,
		"flt = 0x1." + std::string(70u, '1') + "p1",
		"1:35: Error while parsing hexadecimal floating-point: fragment exceeds maximum length of 24 characters"sv);
#endif
}
