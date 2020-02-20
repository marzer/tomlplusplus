#include "tests.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_INIT_WARNINGS

TEST_CASE("parsing - dates and times")
{
	parsing_should_succeed(S(R"(
odt1 = 1979-05-27T07:32:00Z
odt2 = 1979-05-27T00:32:00-07:00
odt3 = 1979-05-27T00:32:00.999999-07:00
odt4 = 1979-05-27 07:32:00Z
ldt1 = 1979-05-27T07:32:00
ldt2 = 1979-05-27T00:32:00.999999
ld1 = 1979-05-27
lt1 = 07:32:00
lt2 = 00:32:00.999999
)"sv),
		[](table&& tbl) noexcept
		{
			static constexpr auto odt1 = date_time{ { 1979, 5, 27 }, { 7, 32 }, {} };
			CHECK(tbl[S("odt1")] == odt1);
			static constexpr auto odt2 = date_time{ { 1979, 5, 27 }, { 0, 32 }, { -7, 0 } };
			CHECK(tbl[S("odt2")] == odt2);
			static constexpr auto odt3 = date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u }, { -7, 0 } };
			CHECK(tbl[S("odt3")] == odt3);
			static constexpr auto odt4 = date_time{ { 1979, 5, 27 }, { 7, 32 }, {} };
			CHECK(tbl[S("odt4")] == odt4);
			static constexpr auto ldt1 = date_time{ { 1979, 5, 27 }, { 7, 32 } };
			CHECK(tbl[S("ldt1")] == ldt1);
			static constexpr auto ldt2 = date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u } };
			CHECK(tbl[S("ldt2")] == ldt2);
			static constexpr auto ld1 = date{ 1979, 5, 27 };
			CHECK(tbl[S("ld1")] == ld1);
			static constexpr auto lt1 = toml::time{ 7, 32 };
			CHECK(tbl[S("lt1")] == lt1);
			static constexpr auto lt2 = toml::time{ 0, 32, 0, 999999000u };
			CHECK(tbl[S("lt2")] == lt2);
		}
	);

	//value tests
	parse_expected_value(  "1987-03-16"sv,                date{ 1987, 3, 16 } );
	parse_expected_value(    "10:20:30"sv,           toml::time{ 10, 20, 30 } );
	parse_expected_value( "10:20:30.04"sv, toml::time{ 10, 20, 30, 40000000 } );
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 } };
		parse_expected_value("1987-03-16T10:20:30"sv, val);
		parse_expected_value("1987-03-16 10:20:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, { -9, -30 } };
		parse_expected_value("1987-03-16T10:20:30-09:30"sv, val);
		parse_expected_value("1987-03-16 10:20:30-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, { 9, 30 } };
		parse_expected_value("1987-03-16T10:20:30+09:30"sv, val);
		parse_expected_value("1987-03-16 10:20:30+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 } };
		parse_expected_value("1987-03-16T10:20:30.04"sv, val);
		parse_expected_value("1987-03-16 10:20:30.04"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, { -9, -30 } };
		parse_expected_value("1987-03-16T10:20:30.04-09:30"sv, val);
		parse_expected_value("1987-03-16 10:20:30.04-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, { 9, 30 } };
		parse_expected_value("1987-03-16T10:20:30.04+09:30"sv, val);
		parse_expected_value("1987-03-16 10:20:30.04+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, {} };
		parse_expected_value("1987-03-16T10:20:30Z"sv, val);
		parse_expected_value("1987-03-16 10:20:30Z"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, {} };
		parse_expected_value("1987-03-16T10:20:30.04Z"sv, val);
		parse_expected_value("1987-03-16 10:20:30.04Z"sv, val);
	}

	// toml/issues/671 - omitting seconds
	#if TOML_LANG_HIGHER_THAN(0, 5, 0)

	parse_expected_value(       "10:20"sv,               toml::time{ 10, 20 } );
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 } };
		parse_expected_value("1987-03-16T10:20"sv, val );
		parse_expected_value("1987-03-16 10:20"sv, val );
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, { -9, -30 } };
		parse_expected_value("1987-03-16T10:20-09:30"sv, val);
		parse_expected_value("1987-03-16 10:20-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, { 9, 30 } };
		parse_expected_value("1987-03-16T10:20+09:30"sv, val);
		parse_expected_value("1987-03-16 10:20+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, {} };
		parse_expected_value("1987-03-16T10:20Z"sv, val);
		parse_expected_value("1987-03-16 10:20Z"sv, val);
	}

	#else

	parsing_should_fail("10:20"sv);
	parsing_should_fail("1987-03-16T10:20"sv);
	parsing_should_fail("1987-03-16 10:20"sv);
	parsing_should_fail("1987-03-16T10:20-09:30"sv);
	parsing_should_fail("1987-03-16 10:20-09:30"sv);
	parsing_should_fail("1987-03-16T10:20+09:30"sv);
	parsing_should_fail("1987-03-16 10:20+09:30"sv);
	parsing_should_fail("1987-03-16T10:20Z"sv);
	parsing_should_fail("1987-03-16 10:20Z"sv);


	#endif
}

TOML_POP_WARNINGS
