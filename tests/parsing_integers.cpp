// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

TEST_CASE("parsing - integers (decimal)")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   BOM_PREFIX R"(
								int1 = +99
								int2 = 42
								int3 = 0
								int4 = -17
								int5 = 1_000
								int6 = 5_349_221
								int7 = 1_2_3_4_5     # VALID but discouraged
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["int1"] == 99);
							   CHECK(tbl["int2"] == 42);
							   CHECK(tbl["int3"] == 0);
							   CHECK(tbl["int4"] == -17);
							   CHECK(tbl["int5"] == 1000);
							   CHECK(tbl["int6"] == 5349221);
							   CHECK(tbl["int7"] == 12345);
						   });
}

TEST_CASE("parsing - integers (decimal, underscores)")
{
	// "Each underscore must be surrounded by at least one digit on each side."
	parsing_should_fail(FILE_LINE_ARGS,
						"int5 = 1__000"sv,
						"1:10: Error while parsing decimal integer: underscores may only follow digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int5 = _1_000"sv,
						"1:8: Error while parsing value: values may not begin with underscores"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int5 = 1_000_"sv,
						"1:14: Error while parsing decimal integer: underscores must be followed by digits"sv);
}

TEST_CASE("parsing - integers (decimal, leading zeroes)")
{
	// "Leading zeroes are not allowed."
	parsing_should_fail(FILE_LINE_ARGS,
						"int1 = +099"sv,
						"1:9: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int2 = 042"sv,
						"1:8: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int3 = 00"sv,
						"1:8: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int4 = -017"sv,
						"1:9: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int5 = 01_000"sv,
						"1:8: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int6 = 05_349_221"sv,
						"1:8: Error while parsing decimal integer: leading zeroes are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int7 = 01_2_3_4_5"sv,
						"1:8: Error while parsing decimal integer: leading zeroes are prohibited"sv);
}

TEST_CASE("parsing - integers (decimal, signed zeroes)")
{
	// "Integer values -0 and +0 are valid and identical to an unprefixed zero."
	parsing_should_succeed(FILE_LINE_ARGS,
						   "zeroes = [-0, +0]"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["zeroes"][0] == 0);
							   CHECK(tbl["zeroes"][1] == 0);
						   });
}

TEST_CASE("parsing - integers (decimal, range)")
{
	// "64 bit (signed long) range expected (−9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)."
	parse_expected_value(FILE_LINE_ARGS, "9223372036854775807"sv, INT64_MAX);
	parse_expected_value(FILE_LINE_ARGS, "-9223372036854775808"sv, INT64_MIN);
	// INT64_MAX + 1
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val =  9223372036854775808"sv,
		"1:8: Error while parsing decimal integer: '9223372036854775808' is not representable as a signed 64-bit integer"sv);
	// INT64_MIN - 1
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = -9223372036854775809"sv,
		"1:7: Error while parsing decimal integer: '-9223372036854775809' is not representable as a signed 64-bit integer"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = +9223372036854775808"sv,
		"1:7: Error while parsing decimal integer: '+9223372036854775808' is not representable as a signed 64-bit integer"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = +100000000000000000000"sv,
		"1:7: Error while parsing decimal integer: '+100000000000000000000' is not representable as a signed 64-bit integer"sv);
}

TEST_CASE("parsing - integers (decimal, misplaced signs)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"val = +-1"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw '-'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -+1"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw '+'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = ++1"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw '+'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = --1"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw '-'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1-"sv,
						"1:8: Error while parsing date: expected 4-digit year, saw '-'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1+"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw '+'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -1+"sv,
						"1:9: Error while parsing decimal integer: expected digit, saw '+'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = +1-"sv,
						"1:9: Error while parsing decimal integer: expected digit, saw '-'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = + 1"sv,
						"1:8: Error while parsing decimal integer: expected digit, saw ' '"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 12t"sv,
						"1:9: Error while parsing decimal integer: expected digit, saw 't'"sv);
}

TEST_CASE("parsing - integers (decimal, values)")
{
	parse_expected_value(FILE_LINE_ARGS, "0"sv, 0);
	parse_expected_value(FILE_LINE_ARGS, "1"sv, 1);
	parse_expected_value(FILE_LINE_ARGS, "+1"sv, 1);
	parse_expected_value(FILE_LINE_ARGS, "-1"sv, -1);
	parse_expected_value(FILE_LINE_ARGS, "1234"sv, 1234);
	parse_expected_value(FILE_LINE_ARGS, "+1234"sv, 1234);
	parse_expected_value(FILE_LINE_ARGS, "-1234"sv, -1234);
	parse_expected_value(FILE_LINE_ARGS, "1_2_3_4"sv, 1234);
	parse_expected_value(FILE_LINE_ARGS, "+1_2_3_4"sv, 1234);
	parse_expected_value(FILE_LINE_ARGS, "-1_2_3_4"sv, -1234);
	parse_expected_value(FILE_LINE_ARGS, "123_456_789"sv, 123456789);
}

TEST_CASE("parsing - integers (hex, bin, oct)")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								# hexadecimal with prefix `0x`
								hex1 = 0xDEADBEEF
								hex2 = 0xdeadbeef
								hex3 = 0xdead_beef

								# octal with prefix `0o`
								oct1 = 0o01234567
								oct2 = 0o755 # useful for Unix file permissions

								# binary with prefix `0b`
								bin1 = 0b11010110
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["hex1"] == 0xDEADBEEF);
							   CHECK(tbl["hex2"] == 0xDEADBEEF);
							   CHECK(tbl["hex3"] == 0xDEADBEEF);
							   CHECK(tbl["oct1"] == 01234567);
							   CHECK(tbl["oct2"] == 0755);
							   CHECK(tbl["bin1"] == 0b11010110);
						   });
}

TEST_CASE("parsing - integers (hex, bin, oct, signs)")
{
	// "leading + is not allowed"
	parsing_should_fail(FILE_LINE_ARGS,
						"hex1 = +0xDEADBEEF"sv,
						"1:8: Error while parsing value: signs are not allowed on hexadecimal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"hex2 = +0xdeadbeef"sv,
						"1:8: Error while parsing value: signs are not allowed on hexadecimal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"hex3 = +0xdead_beef"sv,
						"1:8: Error while parsing value: signs are not allowed on hexadecimal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"oct1 = +0o01234567"sv,
						"1:8: Error while parsing value: signs are not allowed on octal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"oct2 = +0o7550"sv,
						"1:8: Error while parsing value: signs are not allowed on octal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bin1 = +0b11010110"sv,
						"1:8: Error while parsing value: signs are not allowed on binary integers"sv);
}

TEST_CASE("parsing - integers (hex, bin, oct, leading zeroes)")
{
	// "leading zeros are allowed (after the prefix)"
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								hex1 = 0x000DEADBEEF
								hex2 = 0x00000deadbeef
								hex3 = 0x0dead_beef
								oct1 = 0o0001234567
								oct2 = 0o000755
								bin1 = 0b0000011010110
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["hex1"] == 0xDEADBEEF);
							   CHECK(tbl["hex2"] == 0xDEADBEEF);
							   CHECK(tbl["hex3"] == 0xDEADBEEF);
							   CHECK(tbl["oct1"] == 01234567);
							   CHECK(tbl["oct2"] == 0755);
							   CHECK(tbl["bin1"] == 0b11010110);
						   });
}

TEST_CASE("parsing - integers (hex, bin, oct, negative values)")
{
	// "***Non-negative*** integer values may also be expressed in hexadecimal, octal, or binary"
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -0x1"sv,
						"1:7: Error while parsing value: signs are not allowed on hexadecimal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -0o1"sv,
						"1:7: Error while parsing value: signs are not allowed on octal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -0b1"sv,
						"1:7: Error while parsing value: signs are not allowed on binary integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = +0X1"sv,
						"1:7: Error while parsing value: signs are not allowed on hexadecimal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -0O7"sv,
						"1:7: Error while parsing value: signs are not allowed on octal integers"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = -0B1"sv,
						"1:7: Error while parsing value: signs are not allowed on binary integers"sv);
}

TEST_CASE("parsing - integers (hex, bin, oct, range)")
{
	// "64 bit (signed long) range expected (−9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)."
	// (ignoring INT64_MIN because toml doesn't allow these forms to represent negative values)
	parse_expected_value(FILE_LINE_ARGS, "0x7FFFFFFFFFFFFFFF"sv, INT64_MAX);
	parse_expected_value(FILE_LINE_ARGS, "0o777777777777777777777"sv, INT64_MAX);
	parse_expected_value(FILE_LINE_ARGS,
						 "0b111111111111111111111111111111111111111111111111111111111111111"sv,
						 INT64_MAX);
	// INT64_MAX + 1
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val =       0x8000000000000000"sv,
		"1:13: Error while parsing hexadecimal integer: '0x8000000000000000' is not representable as a signed 64-bit integer"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 0o1000000000000000000000"sv,
		"1:7: Error while parsing octal integer: '0o1000000000000000000000' is not representable as a signed 64-bit integer"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 0b1000000000000000000000000000000000000000000000000000000000000000"sv,
		"1:7: Error while parsing binary integer: '0b1000000000000000000000000000000000000000000000000000000000000000' is not representable as a signed 64-bit integer"sv);
}

TEST_CASE("parsing - integers (hex, bin, oct, missing digits)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 0x "sv,
						"1:9: Error while parsing hexadecimal integer: expected hexadecimal digit, saw ' '"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 0o "sv,
						"1:9: Error while parsing octal integer: expected octal digit, saw ' '"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 0b "sv,
						"1:9: Error while parsing binary integer: expected binary digit, saw ' '"sv);
}

TEST_CASE("parsing - integers (hex, bin, oct, values)")
{
	parse_expected_value(FILE_LINE_ARGS, "0xDEADBEEF"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xdeadbeef"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xDEADbeef"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xDEAD_BEEF"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xdead_beef"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xdead_BEEF"sv, 0xDEADBEEF);
	parse_expected_value(FILE_LINE_ARGS, "0xFF"sv, 0xFF);
	parse_expected_value(FILE_LINE_ARGS, "0x00FF"sv, 0xFF);
	parse_expected_value(FILE_LINE_ARGS, "0x0000FF"sv, 0xFF);
	parse_expected_value(FILE_LINE_ARGS, "0o777"sv, 0777);
	parse_expected_value(FILE_LINE_ARGS, "0o7_7_7"sv, 0777);
	parse_expected_value(FILE_LINE_ARGS, "0o007"sv, 0007);
	parse_expected_value(FILE_LINE_ARGS, "0b10000"sv, 0b10000);
	parse_expected_value(FILE_LINE_ARGS, "0b010000"sv, 0b10000);
	parse_expected_value(FILE_LINE_ARGS, "0b01_00_00"sv, 0b10000);
	parse_expected_value(FILE_LINE_ARGS, "0b111111"sv, 0b111111);
}

TEST_CASE("parsing - integers (malformed)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 1_ "sv,
						"1:9: Error while parsing decimal integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 1_"sv,
						"1:9: Error while parsing decimal integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = +1_ "sv,
						"1:10: Error while parsing decimal integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0x1_ "sv,
						"1:11: Error while parsing hexadecimal integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0o1_ "sv,
						"1:11: Error while parsing octal integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0b1_ "sv,
						"1:11: Error while parsing binary integer: underscores must be followed by digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0x_1"sv,
						"1:9: Error while parsing hexadecimal integer: expected hexadecimal digit, saw '_'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0o_1"sv,
						"1:9: Error while parsing octal integer: expected octal digit, saw '_'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0b_1"sv,
						"1:9: Error while parsing binary integer: expected binary digit, saw '_'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0x1__2"sv,
						"1:11: Error while parsing hexadecimal integer: underscores may only follow digits"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0o8"sv,
						"1:9: Error while parsing octal integer: expected octal digit, saw '8'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0b2"sv,
						"1:9: Error while parsing binary integer: expected binary digit, saw '2'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0xG"sv,
						"1:9: Error while parsing hexadecimal integer: expected hexadecimal digit, saw 'G'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"int = 0o17_"sv,
						"1:12: Error while parsing octal integer: underscores must be followed by digits"sv);
}
