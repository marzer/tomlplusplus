// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	template <typename T>
	void check_ordering(const T& lhs, const T& rhs, int expected)
	{
		INFO(lhs << " vs " << rhs);
		CHECK((lhs == rhs) == (expected == 0));
		CHECK((lhs != rhs) == (expected != 0));
		CHECK((lhs < rhs) == (expected < 0));
		CHECK((lhs <= rhs) == (expected <= 0));
		CHECK((lhs > rhs) == (expected > 0));
		CHECK((lhs >= rhs) == (expected >= 0));
		CHECK((rhs == lhs) == (expected == 0));
		CHECK((rhs != lhs) == (expected != 0));
		CHECK((rhs < lhs) == (expected > 0));
		CHECK((rhs <= lhs) == (expected >= 0));
		CHECK((rhs > lhs) == (expected < 0));
		CHECK((rhs >= lhs) == (expected <= 0));
	}

	template <typename T>
	std::string streamed(const T& val)
	{
		std::ostringstream ss;
		ss << val;
		return std::move(ss).str();
	}
}

TEST_CASE("dates - construction")
{
	constexpr date d{ 1987, 3, 16 };
	static_assert(d.year == 1987);
	static_assert(d.month == 3);
	static_assert(d.day == 16);

	CHECK(date{ 1987ull, static_cast<char>(3), static_cast<short>(16) } == d);
}

TEST_CASE("dates - comparison")
{
	static_assert(date{ 1987, 3, 16 } == date{ 1987, 3, 16 });
	static_assert(date{ 1987, 3, 16 } < date{ 1987, 3, 17 });

	SECTION("equal")
	{
		check_ordering(date{ 1987, 3, 16 }, date{ 1987, 3, 16 }, 0);
	}

	SECTION("year")
	{
		check_ordering(date{ 1987, 3, 16 }, date{ 1988, 3, 16 }, -1);
		check_ordering(date{ 1, 1, 1 }, date{ 0, 1, 1 }, 1);
		check_ordering(date{ 9998, 1, 1 }, date{ 9999, 1, 1 }, -1);
	}

	SECTION("month")
	{
		check_ordering(date{ 1987, 3, 16 }, date{ 1987, 4, 16 }, -1);
		check_ordering(date{ 1987, 12, 1 }, date{ 1987, 1, 1 }, 1);
	}

	SECTION("day")
	{
		check_ordering(date{ 1987, 3, 16 }, date{ 1987, 3, 17 }, -1);
		check_ordering(date{ 1987, 3, 31 }, date{ 1987, 3, 1 }, 1);
	}

	SECTION("year outranks month and day")
	{
		check_ordering(date{ 1987, 12, 31 }, date{ 1988, 1, 1 }, -1);
		check_ordering(date{ 2001, 1, 1 }, date{ 2000, 12, 31 }, 1);
	}

	SECTION("month outranks day")
	{
		check_ordering(date{ 1987, 3, 31 }, date{ 1987, 4, 1 }, -1);
		check_ordering(date{ 1987, 2, 1 }, date{ 1987, 1, 31 }, 1);
	}
}

TEST_CASE("dates - stream output")
{
	const auto [val, expected] = GENERATE(Catch::Generators::table<date, std::string_view>({
		{ date{ 1987, 3, 16 }, "1987-03-16"sv },
		{ date{ 1, 2, 3 }, "0001-02-03"sv },
		{ date{ 5, 1, 2 }, "0005-01-02"sv },
		{ date{ 2000, 10, 10 }, "2000-10-10"sv },
		{ date{ 9999, 12, 31 }, "9999-12-31"sv },
	}));

	CHECK(streamed(val) == expected);
}

TEST_CASE("times - construction")
{
	constexpr toml::time t{ 10, 20, 30, 40 };
	static_assert(t.hour == 10u);
	static_assert(t.minute == 20u);
	static_assert(t.second == 30u);
	static_assert(t.nanosecond == 40u);

	SECTION("seconds and nanoseconds default to zero")
	{
		constexpr toml::time hm{ 10, 20 };
		CHECK(hm.hour == 10u);
		CHECK(hm.minute == 20u);
		CHECK(hm.second == 0u);
		CHECK(hm.nanosecond == 0u);

		constexpr toml::time hms{ 10, 20, 30 };
		CHECK(hms.second == 30u);
		CHECK(hms.nanosecond == 0u);
	}

	SECTION("mixed integral types")
	{
		CHECK(toml::time{ 10ull, static_cast<char>(20), static_cast<short>(30), 40l } == t);
	}
}

TEST_CASE("times - comparison")
{
	static_assert(toml::time{ 10, 20 } == toml::time{ 10, 20, 0, 0 });
	static_assert(toml::time{ 10, 20 } < toml::time{ 10, 21 });

	SECTION("equal")
	{
		check_ordering(toml::time{ 10, 20, 30, 40 }, toml::time{ 10, 20, 30, 40 }, 0);
		check_ordering(toml::time{ 0, 0 }, toml::time{ 0, 0, 0, 0 }, 0);
	}

	SECTION("hour")
	{
		check_ordering(toml::time{ 10, 20, 30, 40 }, toml::time{ 11, 20, 30, 40 }, -1);
		check_ordering(toml::time{ 23, 0 }, toml::time{ 0, 0 }, 1);
	}

	SECTION("minute")
	{
		check_ordering(toml::time{ 10, 20, 30, 40 }, toml::time{ 10, 21, 30, 40 }, -1);
		check_ordering(toml::time{ 10, 59 }, toml::time{ 10, 0 }, 1);
	}

	SECTION("second")
	{
		check_ordering(toml::time{ 10, 20, 30, 40 }, toml::time{ 10, 20, 31, 40 }, -1);
		check_ordering(toml::time{ 10, 20, 59 }, toml::time{ 10, 20, 0 }, 1);
	}

	SECTION("nanosecond")
	{
		check_ordering(toml::time{ 10, 20, 30, 40 }, toml::time{ 10, 20, 30, 41 }, -1);
		check_ordering(toml::time{ 10, 20, 30, 999999999 }, toml::time{ 10, 20, 30, 0 }, 1);
		check_ordering(toml::time{ 10, 20, 30, 999999999 }, toml::time{ 10, 20, 30, 999999998 }, 1);
	}

	SECTION("each field outranks the ones after it")
	{
		check_ordering(toml::time{ 0, 59, 59, 999999999 }, toml::time{ 1, 0, 0, 0 }, -1);
		check_ordering(toml::time{ 0, 0, 59, 999999999 }, toml::time{ 0, 1, 0, 0 }, -1);
		check_ordering(toml::time{ 0, 0, 0, 999999999 }, toml::time{ 0, 0, 1, 0 }, -1);
		check_ordering(toml::time{ 23, 59, 59, 999999999 }, toml::time{ 0, 0, 0, 0 }, 1);
	}
}

TEST_CASE("times - stream output")
{
	const auto [val, expected] = GENERATE(Catch::Generators::table<toml::time, std::string_view>({
		{ toml::time{ 10, 20, 34 }, "10:20:34"sv },
		{ toml::time{ 10, 20, 34, 500000000 }, "10:20:34.5"sv },
		{ toml::time{ 0, 0 }, "00:00:00"sv },
		{ toml::time{ 1, 2, 3 }, "01:02:03"sv },
		{ toml::time{ 1, 2, 3, 120000000 }, "01:02:03.12"sv },
		{ toml::time{ 23, 59, 59, 999999999 }, "23:59:59.999999999"sv },
		{ toml::time{ 23, 59, 59, 1 }, "23:59:59.000000001"sv },
		{ toml::time{ 12, 0, 0, 120000 }, "12:00:00.00012"sv },
		{ toml::time{ 12, 0, 0, 10 }, "12:00:00.00000001"sv },
		{ toml::time{ 12, 0, 0, 100000000 }, "12:00:00.1"sv },
	}));

	CHECK(streamed(val) == expected);
}

TEST_CASE("time offsets - construction")
{
	static_assert(time_offset{ 2, 30 }.minutes == 150);
	static_assert(time_offset{ 2, -30 }.minutes == 90);
	static_assert(time_offset{ -2, 30 }.minutes == -90);
	static_assert(time_offset{ -2, -30 }.minutes == -150);
	static_assert(time_offset{ 0, 0 }.minutes == 0);
	static_assert(time_offset{ 0, -30 }.minutes == -30);
	static_assert(time_offset{ 23, 59 }.minutes == 1439);
	static_assert(time_offset{ -23, -59 }.minutes == -1439);

	SECTION("mixed integral types")
	{
		CHECK(time_offset{ 2u, 30u }.minutes == 150);
		CHECK(time_offset{ static_cast<int8_t>(-2), static_cast<uint8_t>(30) }.minutes == -90);
		CHECK(time_offset{ -5ll, static_cast<short>(-45) }.minutes == -345);
	}
}

TEST_CASE("time offsets - comparison")
{
	static_assert(time_offset{ 0, 0 } == time_offset{ 0, 0 });
	static_assert(time_offset{ 0, -1 } < time_offset{ 0, 0 });

	check_ordering(time_offset{ 0, 0 }, time_offset{ 0, 0 }, 0);
	check_ordering(time_offset{ 2, 30 }, time_offset{ 1, 90 }, 0);
	check_ordering(time_offset{ 0, -1 }, time_offset{ 0, 0 }, -1);
	check_ordering(time_offset{ 0, 1 }, time_offset{ 0, 0 }, 1);
	check_ordering(time_offset{ 0, -1 }, time_offset{ 0, 1 }, -1);
	check_ordering(time_offset{ -23, -59 }, time_offset{ 23, 59 }, -1);
	check_ordering(time_offset{ 5, 45 }, time_offset{ 5, 30 }, 1);
}

TEST_CASE("time offsets - stream output")
{
	const auto [val, expected] = GENERATE(Catch::Generators::table<time_offset, std::string_view>({
		{ time_offset{ 2, 30 }, "+02:30"sv },
		{ time_offset{ 2, -30 }, "+01:30"sv },
		{ time_offset{}, "Z"sv },
		{ time_offset{ 0, 0 }, "Z"sv },
		{ time_offset{ -2, 30 }, "-01:30"sv },
		{ time_offset{ -2, -30 }, "-02:30"sv },
		{ time_offset{ 0, 30 }, "+00:30"sv },
		{ time_offset{ 0, -30 }, "-00:30"sv },
		{ time_offset{ 0, 1 }, "+00:01"sv },
		{ time_offset{ 0, -1 }, "-00:01"sv },
		{ time_offset{ 1, 0 }, "+01:00"sv },
		{ time_offset{ -1, 0 }, "-01:00"sv },
		{ time_offset{ 23, 59 }, "+23:59"sv },
		{ time_offset{ -23, -59 }, "-23:59"sv },
	}));

	CHECK(streamed(val) == expected);
}

TEST_CASE("date-times - construction")
{
	constexpr date d{ 1987, 3, 16 };
	constexpr toml::time t{ 10, 20, 34 };
	constexpr time_offset off{ -2, -30 };

	static_assert(date_time{ d, t }.is_local());
	static_assert(date_time{ d }.is_local());
	static_assert(date_time{ t }.is_local());
	static_assert(!date_time{ d, t, off }.is_local());

	SECTION("date and time")
	{
		const date_time dt{ d, t };
		CHECK(dt.is_local());
		CHECK(dt.date == d);
		CHECK(dt.time == t);
		CHECK(!dt.offset);
	}

	SECTION("date only")
	{
		const date_time dt{ d };
		CHECK(dt.is_local());
		CHECK(dt.date == d);
		CHECK(dt.time == toml::time{ 0, 0 });
		CHECK(!dt.offset);
	}

	SECTION("time only")
	{
		const date_time dt{ t };
		CHECK(dt.is_local());
		CHECK(dt.date == date{ 0, 0, 0 });
		CHECK(dt.time == t);
		CHECK(!dt.offset);
	}

	SECTION("date, time and offset")
	{
		const date_time dt{ d, t, off };
		CHECK(!dt.is_local());
		CHECK(dt.date == d);
		CHECK(dt.time == t);
		REQUIRE(dt.offset);
		CHECK(*dt.offset == off);
	}

	SECTION("a zero offset is not local")
	{
		const date_time dt{ d, t, time_offset{} };
		CHECK(!dt.is_local());
		REQUIRE(dt.offset);
		CHECK(dt.offset->minutes == 0);
	}
}

TEST_CASE("date-times - comparison")
{
	constexpr date d{ 1987, 3, 16 };
	constexpr toml::time t{ 10, 20, 34 };
	constexpr time_offset off{ 5, 45 };

	static_assert(date_time{ d, t, off } == date_time{ d, t, off });
	static_assert(date_time{ d, t } < date_time{ d, toml::time{ 10, 20, 35 } });

	SECTION("equal")
	{
		check_ordering(date_time{ d, t }, date_time{ d, t }, 0);
		check_ordering(date_time{ d, t, off }, date_time{ d, t, off }, 0);
		check_ordering(date_time{ d, t, time_offset{} }, date_time{ d, t, time_offset{ 0, 0 } }, 0);
	}

	SECTION("date outranks time")
	{
		check_ordering(date_time{ date{ 1987, 3, 15 }, toml::time{ 23, 59, 59, 999999999 } },
					   date_time{ d, toml::time{ 0, 0 } },
					   -1);
		check_ordering(date_time{ date{ 1987, 3, 17 }, toml::time{ 0, 0 }, off },
					   date_time{ d, toml::time{ 23, 59 }, off },
					   1);
	}

	SECTION("time decides when the dates match")
	{
		check_ordering(date_time{ d, toml::time{ 10, 20, 34 } }, date_time{ d, toml::time{ 10, 20, 35 } }, -1);
		check_ordering(date_time{ d, toml::time{ 10, 20, 34, 1 }, off }, date_time{ d, t, off }, 1);
	}

	SECTION("local and offset date-times are never equal")
	{
		const date_time local{ d, t };
		const date_time offset{ d, t, off };
		const date_time utc{ d, t, time_offset{} };
		CHECK_SYMMETRIC_INEQUAL(local, offset);
		CHECK_SYMMETRIC_INEQUAL(local, utc);
	}

	SECTION("different offsets are not equal")
	{
		const date_time ahead{ d, t, time_offset{ 1, 0 } };
		const date_time behind{ d, t, time_offset{ -1, 0 } };
		const date_time utc{ d, t, time_offset{} };
		CHECK_SYMMETRIC_INEQUAL(ahead, behind);
		CHECK_SYMMETRIC_INEQUAL(ahead, utc);
		CHECK_SYMMETRIC_INEQUAL(behind, utc);
	}
}

TEST_CASE("date-times - stream output")
{
	const auto [val, expected] = GENERATE(Catch::Generators::table<date_time, std::string_view>({
		{ date_time{ { 1987, 3, 16 }, { 10, 20, 34 } }, "1987-03-16T10:20:34"sv },
		{ date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, { -2, -30 } }, "1987-03-16T10:20:34-02:30"sv },
		{ date_time{ { 1987, 3, 16 }, { 10, 20, 34 }, {} }, "1987-03-16T10:20:34Z"sv },
		{ date_time{ { 1987, 3, 16 }, { 10, 20, 34, 500000000 }, { 5, 45 } }, "1987-03-16T10:20:34.5+05:45"sv },
		{ date_time{ { 1987, 3, 16 }, { 10, 20, 34, 5000 }, { 0, 45 } }, "1987-03-16T10:20:34.000005+00:45"sv },
		{ date_time{ date{ 1, 2, 3 } }, "0001-02-03T00:00:00"sv },
	}));

	CHECK(streamed(val) == expected);
}

#if TOML_CPP >= 20

TEST_CASE("dates - std::chrono conversion")
{
	using namespace std::chrono;

	static_assert(date{ year{ 1987 } / 3 / 16 } == date{ 1987, 3, 16 });
	static_assert(date{ year{ 1987 }, month{ 3 }, day{ 16 } } == date{ 1987, 3, 16 });
	static_assert(date{ 1987, 3, 16 }.to_chrono() == year{ 1987 } / 3 / 16);
}

TEST_CASE("times - std::chrono conversion")
{
	using namespace std::chrono;

	SECTION("from hh_mm_ss")
	{
		static_assert(toml::time{ hh_mm_ss{ hours{ 10 } + minutes{ 20 } + seconds{ 30 } + nanoseconds{ 40 } } }
					  == toml::time{ 10, 20, 30, 40 });
		static_assert(toml::time{ hh_mm_ss{ hours{ 10 } + minutes{ 20 } + milliseconds{ 30040 } } }
					  == toml::time{ 10, 20, 30, 40000000 });
		static_assert(toml::time{ hh_mm_ss{ hours{ 10 } + minutes{ 20 } } } == toml::time{ 10, 20 });
	}

	SECTION("to hh_mm_ss")
	{
		const toml::time t{ 10, 20, 30, 40999999 };
		REQUIRE(t.to_chrono());
		CHECK(t.to_chrono()->to_duration() == hours{ 10 } + minutes{ 20 } + seconds{ 30 } + nanoseconds{ 40999999 });
		REQUIRE(t.to_chrono<milliseconds>());
		CHECK(t.to_chrono<milliseconds>()->to_duration() == hours{ 10 } + minutes{ 20 } + milliseconds{ 30040 });
	}

	SECTION("out of range for the duration")
	{
		using narrow_seconds = duration<int8_t>;
		REQUIRE(toml::time{ 0, 2, 7 }.to_chrono<narrow_seconds>());
		CHECK(toml::time{ 0, 2, 7 }.to_chrono<narrow_seconds>()->to_duration() == seconds{ 127 });
		CHECK(!toml::time{ 0, 2, 8 }.to_chrono<narrow_seconds>());
	}
}

TEST_CASE("time offsets - std::chrono conversion")
{
	using namespace std::chrono;

	static_assert(time_offset{ minutes{ -570 } } == time_offset{ -9, -30 });
	static_assert(time_offset{ hours{ -9 }, minutes{ -30 } } == time_offset{ -9, -30 });
	static_assert(time_offset{ hours{ -2 }, minutes{ 30 } } == time_offset{ -2, 30 });
	static_assert(time_offset{ -9, -30 }.to_chrono() == minutes{ -570 });
}

TEST_CASE("date-times - std::chrono conversion")
{
	using namespace std::chrono;

	constexpr auto ymd	 = year{ 1987 } / 3 / 16;
	constexpr auto local = local_days{ ymd } + hours{ 10 } + minutes{ 20 } + seconds{ 30 } + nanoseconds{ 40 };
	constexpr date d{ 1987, 3, 16 };
	constexpr toml::time t{ 10, 20, 30, 40 };

	SECTION("from local_time")
	{
		static_assert(date_time{ local } == date_time{ d, t });
		static_assert(date_time{ time_point_cast<seconds>(local) } == date_time{ d, toml::time{ 10, 20, 30 } });
		static_assert(date_time{ local, minutes{ -570 } } == date_time{ d, t, time_offset{ -9, -30 } });
	}

	SECTION("from year_month_day and hh_mm_ss")
	{
		constexpr auto hms = hh_mm_ss{ hours{ 10 } + minutes{ 20 } + seconds{ 30 } + nanoseconds{ 40 } };
		static_assert(date_time{ ymd, hms } == date_time{ d, t });
		static_assert(date_time{ ymd, hms, minutes{ 90 } } == date_time{ d, t, time_offset{ 1, 30 } });
		static_assert(date_time{ ymd } == date_time{ d });
	}

	SECTION("to local_time")
	{
		CHECK(date_time{ d, t }.to_chrono() == local);
		CHECK(date_time{ d, t }.to_chrono<seconds>() == time_point_cast<seconds>(local));
		CHECK(date_time{ d, toml::time{ 10, 20, 30, 999999999 } }.to_chrono<seconds>()
			  == local_days{ ymd } + hours{ 10 } + minutes{ 20 } + seconds{ 30 });
	}

	SECTION("to local_time ignores the offset")
	{
		CHECK(date_time{ d, t, time_offset{ -9, -30 } }.to_chrono() == local);
	}

	SECTION("before the epoch")
	{
		const auto before = local_days{ year{ 1969 } / 12 / 31 } + hours{ 23 } + minutes{ 59 } + seconds{ 59 }
						  + nanoseconds{ 999999999 };
		const date_time expected{ date{ 1969, 12, 31 }, toml::time{ 23, 59, 59, 999999999 } };
		CHECK(date_time{ before } == expected);
		CHECK(expected.to_chrono() == before);
		CHECK(expected.to_chrono<seconds>()
			  == local_days{ year{ 1969 } / 12 / 31 } + hours{ 23 } + minutes{ 59 } + seconds{ 59 });
	}

	SECTION("out of range for the duration")
	{
		CHECK(date_time{ date{ 1600, 1, 1 } }.to_chrono<seconds>());
		CHECK(!date_time{ date{ 1600, 1, 1 } }.to_chrono());

		using narrow_seconds = duration<int32_t>;
		CHECK(date_time{ date{ 2038, 1, 19 }, toml::time{ 3, 14, 7 } }.to_chrono<narrow_seconds>());
		CHECK(!date_time{ date{ 2038, 1, 19 }, toml::time{ 3, 14, 8 } }.to_chrono<narrow_seconds>());

		using unsigned_seconds = duration<uint32_t>;
		CHECK(date_time{ date{ 1970, 1, 1 } }.to_chrono<unsigned_seconds>());
		CHECK(!date_time{ date{ 1969, 12, 31 }, toml::time{ 23, 59, 59 } }.to_chrono<unsigned_seconds>());
	}
}

#endif
