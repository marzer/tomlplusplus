#include "tests.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_FIELD_INIT_WARNING

TEST_CASE("parsing dates and times")
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
			static constexpr auto odt1 = date_time{ { 1979, 5, 27 }, { 7, 32 }, time_offset{} };
			CHECK(tbl[S("odt1")] == odt1);
			static constexpr auto odt2 = date_time{ { 1979, 5, 27 }, { 0, 32 }, time_offset{ -7 } };
			CHECK(tbl[S("odt2")] == odt2);
			static constexpr auto odt3 = date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u }, time_offset{ -7 } };
			CHECK(tbl[S("odt3")] == odt3);
			static constexpr auto odt4 = date_time{ { 1979, 5, 27 }, { 7, 32 }, time_offset{} };
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
}

TOML_POP_WARNINGS
