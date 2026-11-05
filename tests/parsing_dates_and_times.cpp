// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

TEST_CASE("parsing - dates and times")
{
	parsing_should_succeed(FILE_LINE_ARGS,
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
							   static constexpr auto odt3 =
								   date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000u }, { -7, 0 } };
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
}

TEST_CASE("parsing - dates and times (values)")
{
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
}

TEST_CASE("parsing - dates and times (leap seconds)")
{
	parse_expected_value(FILE_LINE_ARGS, "23:59:60"sv, toml::time{ 23, 59, 60 });
	parse_expected_value(FILE_LINE_ARGS, "12:59:60.5"sv, toml::time{ 12, 59, 60, 500000000u });
	parse_expected_value(FILE_LINE_ARGS, "1990-12-31T23:59:60"sv, date_time{ { 1990, 12, 31 }, { 23, 59, 60 } });
	parse_expected_value(FILE_LINE_ARGS, "1990-12-31T23:59:60Z"sv, date_time{ { 1990, 12, 31 }, { 23, 59, 60 }, {} });
	parse_expected_value(FILE_LINE_ARGS,
						 "1990-12-31T15:59:60-08:00"sv,
						 date_time{ { 1990, 12, 31 }, { 15, 59, 60 }, { -8, 0 } });
	parse_expected_value(FILE_LINE_ARGS,
						 "1991-01-01T05:44:60+05:45"sv,
						 date_time{ { 1991, 1, 1 }, { 5, 44, 60 }, { 5, 45 } });

	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 12:30:60"sv,
		"1:13: Error while parsing time: second 60 (a leap second) is only valid at minute 59, saw minute 30"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 1990-12-31T23:30:60"sv,
		"1:24: Error while parsing date-time: second 60 (a leap second) is only valid at minute 59, saw minute 30"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 1990-12-31T23:30:60Z"sv,
		"1:24: Error while parsing date-time: second 60 (a leap second) is only valid at minute 59 UTC, saw minute 30 UTC"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 1990-12-31T23:59:60+05:45"sv,
		"1:24: Error while parsing date-time: second 60 (a leap second) is only valid at minute 59 UTC, saw minute 14 UTC"sv);
}

TEST_CASE("parsing - dates and times (excess precision is truncated)")
{
	parse_expected_value(FILE_LINE_ARGS, "10:20:30.123456789"sv, toml::time{ 10, 20, 30, 123456789u });
	parse_expected_value(FILE_LINE_ARGS, "10:20:30.9999999999"sv, toml::time{ 10, 20, 30, 999999999u });
	parse_expected_value(FILE_LINE_ARGS, "10:20:30." + std::string(70u, '9'), toml::time{ 10, 20, 30, 999999999u });
	parse_expected_value(FILE_LINE_ARGS,
						 "1987-03-16T10:20:30." + std::string(70u, '9') + "Z",
						 date_time{ { 1987, 3, 16 }, { 10, 20, 30, 999999999u }, {} });
}

TEST_CASE("parsing - dates and times (omitted seconds)")
{
	// toml/issues/671 (allow omission of seconds)
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
}

TEST_CASE("parsing - dates and times (end of input)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-1"sv,
						"1:16: Error while parsing date: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-"sv,
						"1:15: Error while parsing date: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-03"sv, "1:14: Error while parsing date: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-0"sv, "1:13: Error while parsing date: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 1987-"sv, "1:12: Error while parsing date: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:20:30."sv,
						"1:16: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:3"sv, "1:14: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:"sv, "1:13: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:2"sv, "1:11: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:"sv, "1:10: Error while parsing time: encountered end-of-file"sv);

	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30.04-09:3"sv,
						"1:34: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30.04-09:"sv,
						"1:33: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30.04-09"sv,
						"1:32: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30.04-0"sv,
						"1:31: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30.04-"sv,
						"1:30: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:30."sv,
						"1:27: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:3"sv,
						"1:25: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:20:"sv,
						"1:24: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:2"sv,
						"1:22: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10:"sv,
						"1:21: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 10"sv,
						"1:20: Error while parsing time: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 1"sv,
						"1:18: Error while parsing key-value pair: expected a comment or whitespace, saw '1'"sv);
}

TEST_CASE("parsing - dates and times (malformed)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03x16"sv,
						"1:14: Error while parsing date: expected '-', saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 1#"sv,
						"1:18: Error while parsing key-value pair: expected a comment or whitespace, saw '1'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16 1,"sv,
						"1:18: Error while parsing key-value pair: expected a comment or whitespace, saw '1'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T:"sv,
						"1:18: Error while parsing date-time: expected 2-digit hour, saw ':'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16t,"sv,
						"1:18: Error while parsing date-time: expected 2-digit hour, saw ','"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-3-16"sv,
						"1:13: Error while parsing date: expected 2-digit month, saw '-'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-13-16"sv,
						"1:12: Error while parsing date: expected month between 1 and 12 (inclusive), saw 13"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-00-16"sv,
						"1:12: Error while parsing date: expected month between 1 and 12 (inclusive), saw 0"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-1x"sv,
						"1:16: Error while parsing date: expected 2-digit day, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-02-29"sv,
						"1:15: Error while parsing date: expected day between 1 and 28 (inclusive), saw 29"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1900-02-29"sv,
						"1:15: Error while parsing date: expected day between 1 and 28 (inclusive), saw 29"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 2000-02-30"sv,
						"1:15: Error while parsing date: expected day between 1 and 29 (inclusive), saw 30"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-04-31"sv,
						"1:15: Error while parsing date: expected day between 1 and 30 (inclusive), saw 31"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-00"sv,
						"1:15: Error while parsing date: expected day between 1 and 31 (inclusive), saw 0"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-32"sv,
						"1:15: Error while parsing date: expected day between 1 and 31 (inclusive), saw 32"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 24:00:00"sv,
						"1:7: Error while parsing time: expected hour between 0 and 23 (inclusive), saw 24"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T24:00:00"sv,
						"1:18: Error while parsing time: expected hour between 0 and 23 (inclusive), saw 24"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:2x:00"sv,
						"1:11: Error while parsing time: expected 2-digit minute, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:60:00"sv,
						"1:10: Error while parsing time: expected minute between 0 and 59 (inclusive), saw 60"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:20:3x"sv,
						"1:14: Error while parsing time: expected 2-digit second, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:20:61"sv,
						"1:13: Error while parsing time: expected second between 0 and 60 (inclusive), saw 61"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:30x"sv, "1:15: Error while parsing time: expected '.', saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS, "val = 10:20:30z"sv, "1:15: Error while parsing time: expected '.', saw 'z'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:20:30.5x"sv,
						"1:17: Error while parsing time: expected end of value, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16x"sv,
						"1:17: Error while parsing date: expected end of value, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16+"sv,
						"1:17: Error while parsing date: expected end of value, saw '+'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16:10"sv,
						"1:17: Error while parsing date-time: expected space, 'T' or 't', saw ':'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T10:20:30Zx"sv,
						"1:27: Error while parsing date-time: expected end of value, saw 'x'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 10:20:30.x"sv,
						"1:16: Error while parsing time: expected fractional digits, saw 'x'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 1987-03-16T10:20:30+24:00"sv,
		"1:27: Error while parsing date-time offset: expected hour between 0 and 23 (inclusive), saw 24"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T10:20:30+05:3x"sv,
						"1:31: Error while parsing date-time offset: expected 2-digit minute, saw 'x'"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"val = 1987-03-16T10:20:30+05:60"sv,
		"1:30: Error while parsing date-time offset: expected minute between 0 and 59 (inclusive), saw 60"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T10:20:30+05"sv,
						"1:29: Error while parsing date-time offset: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"val = 1987-03-16T10:20:30+5:00"sv,
						"1:28: Error while parsing date-time offset: expected 2-digit hour, saw ':'"sv);
}
