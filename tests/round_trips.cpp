// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_WARNINGS;
#include <cmath>
#include <cstring>
#include <random>
TOML_ENABLE_WARNINGS;

namespace
{
	std::string print_toml(const node& n, format_flags flags)
	{
		std::ostringstream ss;
		ss << toml_formatter{ n, flags };
		return std::move(ss).str();
	}

	std::optional<table> parse_quietly(std::string_view str)
	{
#if TOML_EXCEPTIONS
		try
		{
			return toml::parse(str);
		}
		catch (const parse_error&)
		{
			return {};
		}
#else
		auto result = toml::parse(str);
		if (!result)
			return {};
		return std::move(result).table();
#endif
	}

	std::string escaped(std::string_view str)
	{
		std::string out;
		for (const auto c : str)
		{
			const auto u = static_cast<unsigned char>(c);
			if (u == '\\')
				out += "\\\\";
			else if (u >= 0x20u && u < 0x7Fu)
				out += c;
			else
			{
				static constexpr auto digits = "0123456789ABCDEF"sv;
				out += "\\x";
				out += digits[u >> 4];
				out += digits[u & 0xFu];
			}
		}
		return out;
	}

	std::string key_for(size_t index)
	{
		return "k" + std::to_string(index);
	}

	std::string utf8(uint32_t cp)
	{
		std::string out;
		if (cp < 0x80u)
			out += static_cast<char>(cp);
		else if (cp < 0x800u)
		{
			out += static_cast<char>(0xC0u | (cp >> 6));
			out += static_cast<char>(0x80u | (cp & 0x3Fu));
		}
		else if (cp < 0x10000u)
		{
			out += static_cast<char>(0xE0u | (cp >> 12));
			out += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
			out += static_cast<char>(0x80u | (cp & 0x3Fu));
		}
		else
		{
			out += static_cast<char>(0xF0u | (cp >> 18));
			out += static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
			out += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
			out += static_cast<char>(0x80u | (cp & 0x3Fu));
		}
		return out;
	}

	std::vector<std::string> all_strings(std::string_view alphabet, size_t max_length)
	{
		std::vector<std::string> strings{ std::string{} };
		std::vector<std::string> frontier{ std::string{} };
		for (size_t len = 1; len <= max_length; len++)
		{
			std::vector<std::string> next;
			next.reserve(frontier.size() * alphabet.size());
			for (const auto& prefix : frontier)
				for (const auto c : alphabet)
					next.push_back(prefix + c);
			strings.insert(strings.end(), next.begin(), next.end());
			frontier = std::move(next);
		}
		return strings;
	}

	std::vector<std::string> all_strings(const std::vector<std::string_view>& alphabet, size_t max_length)
	{
		std::vector<std::string> strings{ std::string{} };
		std::vector<std::string> frontier{ std::string{} };
		for (size_t len = 1; len <= max_length; len++)
		{
			std::vector<std::string> next;
			next.reserve(frontier.size() * alphabet.size());
			for (const auto& prefix : frontier)
				for (const auto c : alphabet)
					next.push_back(prefix + std::string{ c });
			strings.insert(strings.end(), next.begin(), next.end());
			frontier = std::move(next);
		}
		return strings;
	}

	std::vector<format_flags> ascii_string_flag_combinations()
	{
		static constexpr format_flags string_flags[] = { format_flags::allow_literal_strings,
														 format_flags::allow_multi_line_strings,
														 format_flags::allow_real_tabs_in_strings };

		std::vector<format_flags> combinations;
		for (unsigned mask = 0; mask < 8u; mask++)
		{
			auto flags = format_flags::none;
			for (unsigned bit = 0; bit < 3u; bit++)
				if (mask & (1u << bit))
					flags |= string_flags[bit];
			combinations.push_back(flags);
		}
		return combinations;
	}

	template <typename T, typename Equal>
	void check_values_round_trip(const std::vector<T>& values,
								 format_flags flags,
								 Equal&& equal,
								 value_flags val_flags = value_flags::none)
	{
		toml::table tbl;
		for (size_t i = 0; i < values.size(); i++)
			tbl.insert(key_for(i), values[i], val_flags);

		const auto printed	= print_toml(tbl, flags);
		const auto reparsed = parse_quietly(printed);

		std::vector<std::string> failures;
		for (size_t i = 0; i < values.size() && failures.size() < 5u; i++)
		{
			const auto key = key_for(i);
			if (reparsed)
			{
				const auto val = (*reparsed)[key].template value_exact<T>();
				if (val && equal(*val, values[i]))
					continue;
			}

			table single;
			single.insert(key, values[i], val_flags);
			const auto single_printed = print_toml(single, flags);
			if (!reparsed && parse_quietly(single_printed))
				continue;
			failures.push_back(escaped(single_printed));
		}

		INFO("format flags: " << static_cast<uint64_t>(flags));
		CAPTURE(failures);
		CHECK(reparsed);
		CHECK(failures.empty());
	}
}

TEST_CASE("round trips - strings")
{
	const auto equal = [](const std::string& a, const std::string& b) { return a == b; };

	SECTION("every ascii string up to four characters with every combination of string flags")
	{
		const auto strings = all_strings("a'\"\n\\\t "sv, 4u);
		for (const auto flags : ascii_string_flag_combinations())
			check_values_round_trip(strings, flags, equal);
	}

	SECTION("short strings with other awkward characters")
	{
		auto strings = all_strings(
			{ "a"sv, "'"sv, "\""sv, "\n"sv, "\\"sv, "\t"sv, " "sv, "\r"sv, "\x7F"sv, "\xC3\xA9"sv, "\xE2\x80\xA8"sv },
			3u);
		for (const auto special : { "\r\n"sv,
									"\na\r\nb"sv,
									"é\n'''"sv,
									"\né"sv,
									"a\x01\n'"sv,
									"'''\n"sv,
									"\n'''"sv,
									"\"\"\"\n"sv,
									"\n\"\"\""sv,
									"''\n''"sv,
									"\n\n"sv,
									"'\n'"sv,
									"a\n'''\n[injected]\nb = 1\nc = '''\n"sv,
									"😀\n\U0010FFFF"sv,
									"\xC2\x85\xE2\x80\xA9"sv })
			strings.emplace_back(special);

		for (const auto flags : { format_flags::none,
								  format_flags::allow_unicode_strings,
								  format_flags::allow_literal_strings | format_flags::allow_multi_line_strings
									  | format_flags::allow_real_tabs_in_strings,
								  toml_formatter::default_flags })
			check_values_round_trip(strings, flags, equal);
	}

	SECTION("code points")
	{
		std::vector<uint32_t> code_points;
		for (uint32_t cp = 0; cp < 0x300u; cp++)
			code_points.push_back(cp);
		for (uint32_t cp = 0x2000u; cp < 0x2070u; cp++)
			code_points.push_back(cp);
		for (const uint32_t cp : { 0xD7FFu, 0xE000u, 0xFEFFu, 0xFFFDu, 0xFFFEu, 0xFFFFu, 0x10000u, 0x10FFFFu })
			code_points.push_back(cp);
		for (uint32_t cp = 0x300u; cp < 0x110000u; cp += 509u)
			if (cp < 0xD800u || cp > 0xDFFFu)
				code_points.push_back(cp);

		std::vector<std::string> strings;
		for (const auto cp : code_points)
		{
			strings.push_back(utf8(cp));
			strings.push_back("x" + utf8(cp) + "\n" + utf8(cp));
		}

		check_values_round_trip(strings, toml_formatter::default_flags, equal);
		check_values_round_trip(strings, format_flags::none, equal);
	}
}

TEST_CASE("round trips - keys")
{
	const auto keys = all_strings({ "a"sv, "-"sv, "."sv, " "sv, "'"sv, "\""sv, "\\"sv, "\t"sv, "\n"sv, "é"sv }, 3u);

	const auto check_round_trip = [](const table& tbl)
	{
		for (const auto flags : { toml_formatter::default_flags, format_flags::none })
		{
			INFO("format flags: " << static_cast<uint64_t>(flags));
			const auto reparsed = parse_quietly(print_toml(tbl, flags));
			REQUIRE(reparsed);
			CHECK(*reparsed == tbl);
		}
	};

	SECTION("key-value pairs")
	{
		toml::table tbl;
		for (size_t i = 0; i < keys.size(); i++)
			tbl.insert(keys[i], static_cast<int64_t>(i));
		check_round_trip(tbl);
	}

	SECTION("table headers")
	{
		toml::table tbl;
		for (size_t i = 0; i < keys.size(); i++)
			tbl.insert(keys[i], toml::table{ { keys[keys.size() - i - 1u], static_cast<int64_t>(i) } });
		check_round_trip(tbl);
	}

	SECTION("array of tables headers")
	{
		toml::table tbl;
		for (size_t i = 0; i < keys.size(); i++)
			tbl.insert(keys[i],
					   toml::array{ toml::table{ { "a", toml::table{ { keys[i], static_cast<int64_t>(i) } } } },
									toml::table{} });
		check_round_trip(tbl);
	}

	SECTION("inline tables")
	{
		toml::table inner;
		for (size_t i = 0; i < keys.size(); i++)
			inner.insert(keys[i], static_cast<int64_t>(i));
		inner.is_inline(true);
		check_round_trip(toml::table{ { "inner", inner } });
	}
}

TEST_CASE("round trips - integers")
{
	std::vector<int64_t> values{ 0,
								 1,
								 -1,
								 (std::numeric_limits<int64_t>::min)(),
								 (std::numeric_limits<int64_t>::max)() };
	for (int bit = 0; bit < 63; bit++)
	{
		const auto pow2 = int64_t{ 1 } << bit;
		for (const auto val : { pow2, pow2 - 1, pow2 + 1 })
		{
			values.push_back(val);
			values.push_back(-val);
		}
	}
	int64_t pow10 = 1;
	for (int exponent = 1; exponent <= 18; exponent++)
	{
		pow10 *= 10;
		for (const auto val : { pow10, pow10 - 1, pow10 + 1 })
		{
			values.push_back(val);
			values.push_back(-val);
		}
	}
	std::mt19937_64 rng{ 0x70B1u };
	for (int i = 0; i < 200; i++)
		values.push_back(static_cast<int64_t>(rng()));

	const auto equal	  = [](int64_t a, int64_t b) { return a == b; };
	const auto base_flags = format_flags::allow_binary_integers | format_flags::allow_octal_integers
						  | format_flags::allow_hexadecimal_integers;

	for (const auto val_flags : { value_flags::none,
								  value_flags::format_as_binary,
								  value_flags::format_as_octal,
								  value_flags::format_as_hexadecimal })
	{
		INFO("value flags: " << static_cast<unsigned>(val_flags));
		check_values_round_trip(values, toml_formatter::default_flags, equal, val_flags);
		check_values_round_trip(values, format_flags::none, equal, val_flags);

		toml::table tbl;
		for (size_t i = 0; i < values.size(); i++)
			tbl.insert(key_for(i), values[i], val_flags);

		const auto reparsed = parse_quietly(print_toml(tbl, base_flags));
		REQUIRE(reparsed);
		size_t base_mismatches{};
		for (size_t i = 0; i < values.size(); i++)
		{
			const auto expected = values[i] >= 0 ? val_flags : value_flags::none;
			const auto parsed	= (*reparsed)[key_for(i)].as_integer();
			if (!parsed || parsed->flags() != expected)
				base_mismatches++;
		}
		CHECK(base_mismatches == 0u);
	}
}

TEST_CASE("round trips - floats")
{
	using limits = std::numeric_limits<double>;

	std::vector<double> values{ 0.0,
								-0.0,
								1.0,
								-1.0,
								0.1,
								0.2,
								0.1 + 0.2,
								1.0 / 3.0,
								2.0 / 3.0,
								limits::epsilon(),
								(limits::min)(),
								(limits::max)(),
								limits::lowest(),
								9007199254740991.0,
								9007199254740992.0,
								9007199254740994.0,
								std::nextafter(1.0, 0.0),
								std::nextafter(1.0, 2.0),
								std::nextafter((limits::max)(), 0.0),
								std::nextafter((limits::min)(), 1.0) };
	for (int exp = -307; exp <= 308; exp++)
	{
		values.push_back(std::pow(10.0, exp));
		values.push_back(-std::pow(10.0, exp));
	}
	for (int exp = -1022; exp <= 1023; exp++)
		values.push_back(std::ldexp(1.0, exp));
	for (int i = 0; i <= 100; i++)
		values.push_back(i / 10.0);

	std::mt19937_64 rng{ 0x70B1u };
	while (values.size() < 6000u)
	{
		const auto bits = rng();
		double val;
		std::memcpy(&val, &bits, sizeof(val));
		if (std::isnormal(val))
			values.push_back(val);
	}

	const auto bitwise_equal = [](double a, double b) { return std::memcmp(&a, &b, sizeof(double)) == 0; };

	SECTION("finite")
	{
		check_values_round_trip(values, toml_formatter::default_flags, bitwise_equal);
	}

	SECTION("denormals")
	{
		std::vector<double> denormals{ limits::denorm_min(),
									   -limits::denorm_min(),
									   std::nextafter((limits::min)(), 0.0),
									   (limits::min)() / 3.0 };
		check_values_round_trip(denormals, toml_formatter::default_flags, bitwise_equal);
	}

#if TOML_ENABLE_FLOAT16
	SECTION("float16")
	{
		const _Float16 halves[] = { static_cast<_Float16>(0.1f),
									static_cast<_Float16>(-2.5f),
									static_cast<_Float16>(65504.0f),
									static_cast<_Float16>(6.103515625e-05f),
									static_cast<_Float16>(5.9604644775390625e-08f) };

		toml::table tbl;
		for (size_t i = 0; i < std::size(halves); i++)
			tbl.insert(key_for(i), halves[i]);

		const auto reparsed = parse_quietly(print_toml(tbl, toml_formatter::default_flags));
		REQUIRE(reparsed);
		for (size_t i = 0; i < std::size(halves); i++)
		{
			const auto val = (*reparsed)[key_for(i)].value_exact<double>();
			REQUIRE(val);
			CHECK(*val == static_cast<double>(halves[i]));
		}
	}
#endif

	SECTION("infinities and nans")
	{
		const std::vector<double> specials{ limits::infinity(), -limits::infinity(), limits::quiet_NaN() };
		check_values_round_trip(specials,
								toml_formatter::default_flags,
								[](double a, double b) { return (std::isnan(a) && std::isnan(b)) || a == b; });
	}
}

TEST_CASE("round trips - dates and times")
{
	std::vector<toml::date> dates;
	for (const int year : { 0, 1, 999, 1000, 1969, 1970, 1999, 2000, 2024, 9999 })
	{
		const auto leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
		for (int month = 1; month <= 12; month++)
		{
			static constexpr int month_lengths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
			const auto last_day					 = month_lengths[month - 1] + (month == 2 && leap ? 1 : 0);
			for (const int day : { 1, 10, last_day })
				dates.push_back(toml::date{ year, month, day });
		}
	}

	std::vector<toml::time> times;
	for (const int hour : { 0, 1, 9, 10, 23 })
		for (const int minute : { 0, 9, 10, 59 })
			for (const int second : { 0, 9, 10, 59 })
				for (const uint32_t nanosecond :
					 { 0u, 1u, 10u, 1000u, 120000000u, 123456789u, 500000000u, 999999999u, 100000000u, 999000u })
					times.push_back(toml::time{ hour, minute, second, nanosecond });

	std::vector<toml::date_time> date_times;
	for (int minutes = -(23 * 60 + 59); minutes <= 23 * 60 + 59; minutes++)
		date_times.push_back(toml::date_time{ dates[static_cast<size_t>(minutes + 1439) % dates.size()],
											  times[static_cast<size_t>(minutes + 1439) % times.size()],
											  toml::time_offset{ 0, minutes } });
	for (size_t i = 0; i < times.size(); i++)
		date_times.push_back(toml::date_time{ dates[i % dates.size()], times[i] });

	const auto equal = [](const auto& a, const auto& b) { return a == b; };
	check_values_round_trip(dates, toml_formatter::default_flags, equal);
	check_values_round_trip(times, toml_formatter::default_flags, equal);
	check_values_round_trip(date_times, toml_formatter::default_flags, equal);
}

TEST_CASE("round trips - documents")
{
	const auto document = GENERATE(R"(
		# This is a TOML document

		title = "TOML Example"

		[owner]
		name = "Tom Preston-Werner"
		dob = 1979-05-27T07:32:00-08:00

		[database]
		enabled = true
		ports = [ 8000, 8001, 8002 ]
		data = [ ["delta", "phi"], [3.14] ]
		temp_targets = { cpu = 79.5, case = 72.0 }

		[servers]

		[servers.alpha]
		ip = "10.0.0.1"
		role = "frontend"

		[servers.beta]
		ip = "10.0.0.2"
		role = "backend"
	)"sv,
								   R"(
		[[fruits]]
		name = "apple"

		[fruits.physical]
		color = "red"
		shape = "round"

		[[fruits.varieties]]
		name = "red delicious"

		[[fruits.varieties]]
		name = "granny smith"

		[[fruits]]
		name = "banana"

		[[fruits.varieties]]
		name = "plantain"
	)"sv,
								   R"(
		"key with spaces" = 1
		'quoted "key"' = 'C:\path'
		a.b.c = 1
		a.d = { e = [ 1, 2.5, "three", 1979-05-27, 07:32:00.25, 1979-05-27T07:32:00Z ], f = {} }
		ints = [ 0x1F, 0o17, 0b101, -9223372036854775808, 9223372036854775807 ]
		floats = [ inf, -inf, nan, -0.0, 6.02e23, 1e-300 ]
		strings = [ """multi
		line""", '''literal
		multi''', "tab\there", "unicode é 😀", "", "it's" ]
		empty = []
		nested = [ [ [ ] ], [ { x = 1 } ], [ 1, [ 2, [ 3 ] ] ] ]
		long = [ "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ]

		[[aot]]

		[[aot]]
		x = 1

		[aot.sub]
		y = 2

		[[aot.sub.deeper]]
		z = 3
	)"sv);

	const auto original = parse_quietly(document);
	REQUIRE(original);

	for (const auto flags : { toml_formatter::default_flags,
							  format_flags::none,
							  format_flags::indentation,
							  toml_formatter::default_flags | format_flags::force_multiline_arrays,
							  toml_formatter::default_flags | format_flags::terse_key_value_pairs,
							  format_flags::force_multiline_arrays | format_flags::terse_key_value_pairs })
	{
		const auto printed = print_toml(*original, flags);
		INFO("format flags: " << static_cast<uint64_t>(flags));
		INFO(printed);
		const auto reparsed = parse_quietly(printed);
		REQUIRE(reparsed);
		CHECK(*reparsed == *original);
		CHECK(print_toml(*reparsed, flags) == printed);
	}
}
