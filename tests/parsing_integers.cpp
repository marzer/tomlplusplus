#include "tests.h"

TEST_CASE("parsing - integers (decimal)")
{
	parsing_should_succeed(S(R"(
int1 = +99
int2 = 42
int3 = 0
int4 = -17
int5 = 1_000
int6 = 5_349_221
int7 = 1_2_3_4_5     # VALID but discouraged
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("int1")] == 99);
			CHECK(tbl[S("int2")] == 42);
			CHECK(tbl[S("int3")] == 0);
			CHECK(tbl[S("int4")] == -17);
			CHECK(tbl[S("int5")] == 1000);
			CHECK(tbl[S("int6")] == 5349221);
			CHECK(tbl[S("int7")] == 12345);
		}
	);

	// "Each underscore must be surrounded by at least one digit on each side."
	parsing_should_fail(S("int5 = 1__000"sv));
	parsing_should_fail(S("int5 = _1_000"sv));
	parsing_should_fail(S("int5 = 1_000_"sv));

	// "Leading zeroes are not allowed."
	parsing_should_fail(S("int1 = +099"sv));
	parsing_should_fail(S("int2 = 042"sv));
	parsing_should_fail(S("int3 = 00"sv));
	parsing_should_fail(S("int4 = -017"sv));
	parsing_should_fail(S("int5 = 01_000"sv));
	parsing_should_fail(S("int6 = 05_349_221"sv));
	parsing_should_fail(S("int7 = 01_2_3_4_5"sv));

	// "Integer values -0 and +0 are valid and identical to an unprefixed zero."
	parsing_should_succeed(
		S("zeroes = [-0, +0]"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("zeroes")][0] == 0);
			CHECK(tbl[S("zeroes")][1] == 0);
		}
	);

	parsing_should_fail(S("val = +-1"sv));
	parsing_should_fail(S("val = -+1"sv));
	parsing_should_fail(S("val = ++1"sv));
	parsing_should_fail(S("val = --1"sv));
	parsing_should_fail(S("val = 1-"sv));
	parsing_should_fail(S("val = 1+"sv));
	parsing_should_fail(S("val = -1+"sv));
	parsing_should_fail(S("val = +1-"sv));

	// value tests
	parse_expected_value(        "1234"sv,       1234 );
	parse_expected_value(       "+1234"sv,       1234 );
	parse_expected_value(       "-1234"sv,      -1234 );
	parse_expected_value(           "0"sv,          0 );
	parse_expected_value(     "1_2_3_4"sv,       1234 );
	parse_expected_value(    "+1_2_3_4"sv,       1234 );
	parse_expected_value(    "-1_2_3_4"sv,      -1234 );
	parse_expected_value( "123_456_789"sv,  123456789 );
}

TEST_CASE("parsing - integers (hex, bin, oct)")
{
	parsing_should_succeed(S(R"(
# hexadecimal with prefix `0x`
hex1 = 0xDEADBEEF
hex2 = 0xdeadbeef
hex3 = 0xdead_beef

# octal with prefix `0o`
oct1 = 0o01234567
oct2 = 0o755 # useful for Unix file permissions

# binary with prefix `0b`
bin1 = 0b11010110
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("hex1")] == 0xDEADBEEF);
			CHECK(tbl[S("hex2")] == 0xDEADBEEF);
			CHECK(tbl[S("hex3")] == 0xDEADBEEF);
			CHECK(tbl[S("oct1")] == 01234567);
			CHECK(tbl[S("oct2")] == 0755);
			CHECK(tbl[S("bin1")] == 0b11010110);
		}
	);

	// "leading + is not allowed"
	parsing_should_fail(S("hex1 = +0xDEADBEEF"sv));
	parsing_should_fail(S("hex2 = +0xdeadbeef"sv));
	parsing_should_fail(S("hex3 = +0xdead_beef"sv));
	parsing_should_fail(S("oct1 = +0o01234567"sv));
	parsing_should_fail(S("oct2 = +0o7550"sv));
	parsing_should_fail(S("int6 = +05_349_221"sv));
	parsing_should_fail(S("bin1 = +0b11010110"sv));

	// "leading zeros are allowed (after the prefix)"
	parsing_should_succeed(S(R"(
hex1 = 0x000DEADBEEF
hex2 = 0x00000deadbeef
hex3 = 0x0dead_beef
oct1 = 0o0001234567
oct2 = 0o000755
bin1 = 0b0000011010110
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("hex1")] == 0xDEADBEEF);
			CHECK(tbl[S("hex2")] == 0xDEADBEEF);
			CHECK(tbl[S("hex3")] == 0xDEADBEEF);
			CHECK(tbl[S("oct1")] == 01234567);
			CHECK(tbl[S("oct2")] == 0755);
			CHECK(tbl[S("bin1")] == 0b11010110);
		}
	);

	// "64 bit (signed long) range expected (−9,223,372,036,854,775,808 to 9,223,372,036,854,775,807)."
	parsing_should_fail(S("val = −9223372036854775809"sv));
	parsing_should_fail(S("val = 9223372036854775808"sv));

	// "***Non-negative*** integer values may also be expressed in hexadecimal, octal, or binary"
	parsing_should_fail(S("val = -0o1"sv));
	parsing_should_fail(S("val = -0b1"sv));
	parsing_should_fail(S("val = -0x1"sv));

	// value tests
    parse_expected_value(  "0xDEADBEEF"sv, 0xDEADBEEF );
    parse_expected_value(  "0xdeadbeef"sv, 0xDEADBEEF );
    parse_expected_value(  "0xDEADbeef"sv, 0xDEADBEEF );
    parse_expected_value( "0xDEAD_BEEF"sv, 0xDEADBEEF );
    parse_expected_value( "0xdead_beef"sv, 0xDEADBEEF );
    parse_expected_value( "0xdead_BEEF"sv, 0xDEADBEEF );
    parse_expected_value(        "0xFF"sv,       0xFF );
    parse_expected_value(      "0x00FF"sv,       0xFF );
    parse_expected_value(    "0x0000FF"sv,       0xFF );
    parse_expected_value(       "0o777"sv,       0777 );
    parse_expected_value(     "0o7_7_7"sv,       0777 );
    parse_expected_value(       "0o007"sv,       0007 );
    parse_expected_value(     "0b10000"sv,    0b10000 );
    parse_expected_value(    "0b010000"sv,    0b10000 );
    parse_expected_value(  "0b01_00_00"sv,    0b10000 );
    parse_expected_value(    "0b111111"sv,   0b111111 );
}
