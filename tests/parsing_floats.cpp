#include "tests.h"

TEST_CASE("parsing - floats")
{
	parsing_should_succeed(S(R"(
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
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("flt1")] == 1.0);
			CHECK(tbl[S("flt2")] == 3.1415);
			CHECK(tbl[S("flt3")] == -0.01);
			CHECK(tbl[S("flt4")].as<double>()->get() == 5e+22_a);
			CHECK(tbl[S("flt5")].as<double>()->get() == 1e6_a);
			CHECK(tbl[S("flt6")] == -2E-2);
			CHECK(tbl[S("flt7")].as<double>()->get() == 6.626e-34_a);
			CHECK(tbl[S("flt8")].as<double>()->get() == 224617.445991228_a);
		}
	);

	// "Each underscore must be surrounded by at least one digit on each side."
	parsing_should_fail(S("flt8 = 224_617.445_991_228_"sv));
	parsing_should_fail(S("flt8 = _224_617.445_991_228"sv));
	parsing_should_fail(S("flt8 = 224__617.445_991_228"sv));

	// "Float values -0.0 and +0.0 are valid and should map according to IEEE 754."
	parsing_should_succeed(
		S(R"(zeroes = [-0.0, +0.0])"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("zeroes")][0] == -0.0);
			CHECK(tbl[S("zeroes")][1] == +0.0);
		}
	);

	//value tests
	parse_expected_value(               "1e1"sv,             1e1 );
	parse_expected_value(              "1e-1"sv,            1e-1 );
	parse_expected_value(              "1e+1"sv,            1e+1 );
	parse_expected_value(               "1.0"sv,             1.0 );
	parse_expected_value(             "1.0e1"sv,           1.0e1 );
	parse_expected_value(            "1.0e-1"sv,          1.0e-1 );
	parse_expected_value(            "1.0e+1"sv,          1.0e+1 );
	parse_expected_value(              "+1e1"sv,            +1e1 );
	parse_expected_value(              "+1.0"sv,            +1.0 );
	parse_expected_value(            "+1.0e1"sv,          +1.0e1 );
	parse_expected_value(           "+1.0e+1"sv,         +1.0e+1 );
	parse_expected_value(           "+1.0e-1"sv,         +1.0e-1 );
	parse_expected_value(           "-1.0e+1"sv,         -1.0e+1 );
	parse_expected_value(              "-1e1"sv,            -1e1 );
	parse_expected_value(              "-1.0"sv,            -1.0 );
	parse_expected_value(            "-1.0e1"sv,          -1.0e1 );
	parse_expected_value(           "-1.0e-1"sv,         -1.0e-1 );
	parse_expected_value(               "1.0"sv,             1.0 );
	parse_expected_value(               "0.1"sv,             0.1 );
	parse_expected_value(             "0.001"sv,           0.001 );
	parse_expected_value(             "0.100"sv,           0.100 );
	parse_expected_value(             "+3.14"sv,           +3.14 );
	parse_expected_value(             "-3.14"sv,           -3.14 );
	parse_expected_value(  "3.1415_9265_3589"sv,  3.141592653589 );
	parse_expected_value( "+3.1415_9265_3589"sv, +3.141592653589 );
	parse_expected_value( "-3.1415_9265_3589"sv, -3.141592653589 );
	parse_expected_value(       "123_456.789"sv,      123456.789 );
	parse_expected_value(      "+123_456.789"sv,     +123456.789 );
	parse_expected_value(      "-123_456.789"sv,     -123456.789 );
	parse_expected_value(              "+0.0"sv,            +0.0 );
	parse_expected_value(              "-0.0"sv,            -0.0 );
	parse_expected_value(              "1e10"sv,            1e10 );
	parse_expected_value(             "1e+10"sv,           1e+10 );
	parse_expected_value(             "1e-10"sv,           1e-10 );
	parse_expected_value(             "+1e10"sv,           +1e10 );
	parse_expected_value(            "+1e+10"sv,          +1e+10 );
	parse_expected_value(            "+1e-10"sv,          +1e-10 );
	parse_expected_value(             "-1e10"sv,           -1e10 );
	parse_expected_value(            "-1e+10"sv,          -1e+10 );
	parse_expected_value(            "-1e-10"sv,          -1e-10 );
	parse_expected_value(           "123e-10"sv,         123e-10 );
	parse_expected_value(              "1E10"sv,            1E10 );
	parse_expected_value(             "1E+10"sv,           1E+10 );
	parse_expected_value(             "1E-10"sv,           1E-10 );
	parse_expected_value(           "123E-10"sv,         123E-10 );
	parse_expected_value(         "1_2_3E-10"sv,         123E-10 );
	parse_expected_value(        "1_2_3E-1_0"sv,         123E-10 );
	parse_expected_value(              "+0e0"sv,            +0e0 );
	parse_expected_value(              "-0e0"sv,            -0e0 );
	parse_expected_value(         "1_2_3E-01"sv,         123E-01 );
	parse_expected_value(        "1_2_3E-0_1"sv,         123E-01 );
	parse_expected_value(           "6.02e23"sv,         6.02e23 );
	parse_expected_value(          "6.02e+23"sv,        6.02e+23 );
	parse_expected_value(  "1.112_650_06e-17"sv,  1.11265006e-17 );

	//toml/issues/562 - hexfloat literals
	#if TOML_LANG_HIGHER_THAN(0, 5, 0)
	parse_expected_value("          0x10.1p0"sv,        0x10.1p0 );
	parse_expected_value("          0x0.3p10"sv,        0x0.3p10 );
	parse_expected_value("          0x12.2P2"sv,        0x12.2P2 );
	#else
	parsing_should_fail(S("val = 0x10.1p0"sv));
	parsing_should_fail(S("val = 0x0.3p10"sv));
	parsing_should_fail(S("val = 0x12.2P2"sv));
	#endif
}

TEST_CASE("parsing - inf and nan")
{
	parsing_should_succeed(S(R"(
# infinity
sf1 = inf  # positive infinity
sf2 = +inf # positive infinity
sf3 = -inf # negative infinity

# not a number
sf4 = nan  # actual sNaN/qNaN encoding is implementation specific
sf5 = +nan # same as `nan`
sf6 = -nan # valid, actual encoding is implementation specific
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("sf1")] == std::numeric_limits<double>::infinity());
			CHECK(tbl[S("sf2")] == std::numeric_limits<double>::infinity());
			CHECK(tbl[S("sf3")] == -std::numeric_limits<double>::infinity());
			CHECK(std::isnan(tbl[S("sf4")].as<double>()->get()));
			CHECK(std::isnan(tbl[S("sf5")].as<double>()->get()));
			CHECK(std::isnan(tbl[S("sf6")].as<double>()->get()));
		}
	);
}
