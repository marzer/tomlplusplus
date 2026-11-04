// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

TEST_CASE("parsing - dates and times")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
			odt1 = 1979-05-27T07:32:00Z
			odt2 = 1979-05-27T00:32:00-07:00
			odt3 = 1979-05-27T00:32:00.999999-07:00
			odt4 = 1979-05-27 07:32:00Z
			ldt1 = 1979-05-27T07:32:00
			ldt2 = 1979-05-27T00:32:00.999999
			ld1 = 1979-05-27
			lt1 = 07:32:00
			lt2 = 00:32:00.999999
		)"sv,
		[](table&& tbl)
		{
			static constexpr auto odt1 = date_time{ { 1979, 5, 27 }, { 7, 32 }, {} };
			CHECK(tbl["odt1"] == odt1);
			static constexpr auto odt2 = date_time{ { 1979, 5, 27 }, { 0, 32 }, { -7, 0 } };
			CHECK(tbl["odt2"] == odt2);
			static constexpr auto odt3 = date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u }, { -7, 0 } };
			CHECK(tbl["odt3"] == odt3);
			static constexpr auto odt4 = date_time{ { 1979, 5, 27 }, { 7, 32 }, {} };
			CHECK(tbl["odt4"] == odt4);
			static constexpr auto ldt1 = date_time{ { 1979, 5, 27 }, { 7, 32 } };
			CHECK(tbl["ldt1"] == ldt1);
			static constexpr auto ldt2 = date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u } };
			CHECK(tbl["ldt2"] == ldt2);
			static constexpr auto ld1 = date{ 1979, 5, 27 };
			CHECK(tbl["ld1"] == ld1);
			static constexpr auto lt1 = toml::time{ 7, 32 };
			CHECK(tbl["lt1"] == lt1);
			static constexpr auto lt2 = toml::time{ 0, 32, 0, 999999000u };
			CHECK(tbl["lt2"] == lt2);
		});

	// value tests
	parse_expected_value(FILE_LINE_ARGS, "1987-03-16"sv, date{ 1987, 3, 16 });
	parse_expected_value(FILE_LINE_ARGS, "10:20:30"sv, toml::time{ 10, 20, 30 });
	parse_expected_value(FILE_LINE_ARGS, "10:20:30.04"sv, toml::time{ 10, 20, 30, 40000000 });
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, { -9, -30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30-09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30-09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, { 9, 30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30+09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30+09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30.04"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30.04"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30.04"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, { -9, -30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30.04-09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30.04-09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30.04-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, { 9, 30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30.04+09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30.04+09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30.04+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30 }, {} };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30z"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 }, {} };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30.04Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30.04Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30.04Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20:30.04z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16t10:20:30.04z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20:30.04z"sv, val);
	}

// toml/issues/671 (allow omission of seconds)
#if TOML_LANG_UNRELEASED

	parse_expected_value(FILE_LINE_ARGS, "10:20"sv, toml::time{ 10, 20 });
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, { -9, -30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20-09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20-09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, { 9, 30 } };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20+09:30"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20+09:30"sv, val);
	}
	{
		const auto val = date_time{ { 1987, 3, 16 }, { 10, 20 }, {} };
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16T10:20Z"sv, val);
		parse_expected_value(FILE_LINE_ARGS, "1987-03-16 10:20Z"sv, val);
	}

#else

	parsing_should_fail(FILE_LINE_ARGS, "10:20"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16T10:20"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16 10:20"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16T10:20-09:30"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16 10:20-09:30"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16T10:20+09:30"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16 10:20+09:30"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16T10:20Z"sv);
	parsing_should_fail(FILE_LINE_ARGS, "1987-03-16 10:20Z"sv);

#endif

	// eof tests
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-1"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-0"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:30."sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:3"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:"sv);
#if !TOML_LANG_UNRELEASED // toml/issues/671 (allow omission of seconds)
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20"sv);
#endif
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:2"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:"sv);

	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30.04-09:3"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30.04-09:"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30.04-09"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30.04-0"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30.04-"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:30."sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:3"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20:"sv);
#if !TOML_LANG_UNRELEASED // toml/issues/671 (allow omission of seconds)
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:20"sv);
#endif
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:2"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10:"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 10"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 1"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 1#"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16 1,"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16T:"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03-16t,"sv);
}

#if TOML_CPP >= 20
TEST_CASE("chrono conversions - dates and times")
{
	using namespace std::chrono;

	const auto chrono_date = year_month_day{ year{ 1987 }, month{ 3 }, day{ 16 } };
	const auto date_value = toml::date{ chrono_date };
	CHECK(date_value == toml::date{ 1987, 3, 16 });
	CHECK(date_value.to_chrono() == chrono_date);

	const auto chrono_time = hh_mm_ss<nanoseconds>{
		hours{ 10 } + minutes{ 20 } + seconds{ 30 } + nanoseconds{ 40000000 }
	};
	const auto time_value = toml::time{ chrono_time };
	CHECK(time_value == toml::time{ 10, 20, 30, 40000000 });
	CHECK(time_value.to_chrono().to_duration() == chrono_time.to_duration());

	const auto chrono_offset = minutes{ -570 };
	const auto offset_value = toml::time_offset{ chrono_offset };
	CHECK(offset_value == toml::time_offset{ -9, -30 });
	CHECK(offset_value.to_chrono() == chrono_offset);

	const auto chrono_local = local_days{ chrono_date } + chrono_time.to_duration();
	const auto local_value = toml::date_time{ chrono_local };
	CHECK(local_value == toml::date_time{ { 1987, 3, 16 }, { 10, 20, 30, 40000000 } });
	CHECK(local_value.to_chrono() == chrono_local);

	const auto offset_local_value = toml::date_time{ chrono_local, chrono_offset };
	CHECK(offset_local_value == toml::date_time{
		{ 1987, 3, 16 },
		{ 10, 20, 30, 40000000 },
		{ -9, -30 }
	});

	const auto parsed = toml::parse("value = 1987-03-16T10:20:30.04"sv);
	const auto default_time = local_time<nanoseconds>{ local_days{ year_month_day{ year{ 2000 }, month{ 1 }, day{ 1 } } } };
	CHECK(parsed["value"].value_or(default_time) == chrono_local);
	CHECK(parsed["missing"].value_or(default_time) == default_time);

	const auto expected_epoch = chrono_local.time_since_epoch();
	const auto seconds_default = local_time<seconds>{};
	const auto milliseconds_default = local_time<milliseconds>{};
	const auto microseconds_default = local_time<microseconds>{};
	const auto deciseconds_default = local_time<duration<int64_t, std::deci>>{};
	CHECK(parsed["value"].value_or(seconds_default).time_since_epoch() == duration_cast<seconds>(expected_epoch));
	CHECK(parsed["value"].value_or(milliseconds_default).time_since_epoch()
		  == duration_cast<milliseconds>(expected_epoch));
	CHECK(parsed["value"].value_or(microseconds_default).time_since_epoch()
		  == duration_cast<microseconds>(expected_epoch));
	CHECK(parsed["value"].value_or(deciseconds_default).time_since_epoch()
		  == duration_cast<duration<int64_t, std::deci>>(expected_epoch));

	const auto parsed_date = toml::parse("value = 1987-03-16"sv);
	const auto default_date = year_month_day{ year{ 2000 }, month{ 1 }, day{ 1 } };
	CHECK(parsed_date["value"].value_or(default_date) == chrono_date);
	CHECK(parsed_date["missing"].value_or(default_date) == default_date);

	const auto parsed_time = toml::parse("value = 10:20:30.04"sv);
	const auto default_time_of_day = hh_mm_ss<milliseconds>{};
	const auto expected_time_of_day = hh_mm_ss<milliseconds>{ hours{ 10 } + minutes{ 20 } + seconds{ 30 }
															  + milliseconds{ 40 } };
	CHECK(parsed_time["value"].value_or(default_time_of_day).to_duration() == expected_time_of_day.to_duration());
	CHECK(parsed_time["missing"].value_or(default_time_of_day).to_duration()
		  == default_time_of_day.to_duration());
}
#endif
