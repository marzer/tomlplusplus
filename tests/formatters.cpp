// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_WARNINGS;
#include <cmath>
#include <stdexcept>
#include <streambuf>
TOML_ENABLE_WARNINGS;

namespace
{
	template <typename Formatter, typename T>
	static auto format_to_string(const T& obj,
								 format_flags flags			= Formatter::default_flags,
								 format_flags exclude_flags = format_flags::none)
	{
		std::stringstream ss;
		ss << "*****\n" << Formatter{ obj, flags & ~(exclude_flags) } << "\n*****";
		return ss.str();
	}

	struct char32_printer
	{
		char32_t value;

		friend std::ostream& operator<<(std::ostream& os, const char32_printer& p)
		{
			if (p.value <= U'\x1F')
				return os << '\'' << impl::control_char_escapes[static_cast<size_t>(p.value)] << '\'';
			else if (p.value == U'\x7F')
				return os << "'\\u007F'"sv;
			else if (p.value < 127u)
				return os << '\'' << static_cast<char>(static_cast<uint8_t>(p.value)) << '\'';
			else
				return os << static_cast<uint_least32_t>(p.value);
		}
	};

	struct string_difference
	{
		source_position position;
		size_t index;
		char32_t a, b;

		friend std::ostream& operator<<(std::ostream& os, const string_difference& diff)
		{
			if (diff.a && diff.b && diff.a != diff.b)
				os << char32_printer{ diff.a } << " vs "sv << char32_printer{ diff.b } << " at "sv;
			return os << diff.position << ", index "sv << diff.index;
		}
	};

	static optional<string_difference> find_first_difference(std::string_view str_a, std::string_view str_b) noexcept
	{
		string_difference diff{ { 1u, 1u } };
		impl::utf8_decoder a, b;

		for (size_t i = 0, e = std::min(str_a.length(), str_b.length()); i < e; i++, diff.index++)
		{
			a(static_cast<uint8_t>(str_a[i]));
			b(static_cast<uint8_t>(str_b[i]));
			if (a.has_codepoint() != b.has_codepoint() || a.error() != b.error())
				return diff;

			if (a.error())
			{
				a.reset();
				b.reset();
				continue;
			}

			if (!a.has_codepoint())
				continue;

			if (a.codepoint != b.codepoint)
			{
				diff.a = a.codepoint;
				diff.b = b.codepoint;
				return diff;
			}

			if (a.codepoint == U'\n')
			{
				diff.position.line++;
				diff.position.column = 1u;
			}
			else
				diff.position.column++;
		}
		if (str_a.length() != str_b.length())
			return diff;
		return {};
	}

	template <typename Formatter, typename T>
	static std::string print_with(const T& obj, format_flags flags = Formatter::default_flags)
	{
		std::ostringstream ss;
		ss << Formatter{ obj, flags };
		return std::move(ss).str();
	}

	static std::string_view skip_leading_newline(std::string_view str) noexcept
	{
		if (!str.empty() && str.front() == '\n')
			str.remove_prefix(1);
		return str;
	}

	static std::optional<table> parse_printed(std::string_view str)
	{
#if TOML_EXCEPTIONS
		try
		{
			return toml::parse(str);
		}
		catch (const parse_error& err)
		{
			UNSCOPED_INFO(err);
			return {};
		}
#else
		auto result = toml::parse(str);
		if (!result)
		{
			UNSCOPED_INFO(result.error());
			return {};
		}
		return std::move(result).table();
#endif
	}

	template <typename Formatter, typename T>
	static void check_output(const T& data, format_flags flags, std::string_view expected)
	{
		CHECK(print_with<Formatter>(data, flags) == skip_leading_newline(expected));
	}

	static void check_toml_round_trip(const table& data, format_flags flags, std::string_view expected)
	{
		const auto printed = print_with<toml_formatter>(data, flags);
		CHECK(printed == skip_leading_newline(expected));
		const auto reparsed = parse_printed(printed);
		REQUIRE(reparsed);
		CHECK(*reparsed == data);
	}

	static constexpr auto f64_inf = std::numeric_limits<double>::infinity();
	static constexpr auto f64_nan = std::numeric_limits<double>::quiet_NaN();
	static constexpr auto i64_min = (std::numeric_limits<int64_t>::min)();
	static constexpr auto i64_max = (std::numeric_limits<int64_t>::max)();

	static std::string letters(size_t length)
	{
		std::string str(length, '\0');
		for (size_t i = 0; i < length; i++)
			str[i] = static_cast<char>('a' + i % 26u);
		return str;
	}

	static void check_same(const std::string& printed, const std::string& expected)
	{
		if (const auto diff = find_first_difference(printed, expected))
			FORCE_FAIL("string mismatch: "sv << *diff);
	}

	struct limited_buf : std::streambuf
	{
		std::string received;
		size_t limit;
		bool throws;

		explicit limited_buf(size_t max_bytes, bool throw_at_limit = false) //
			: limit{ max_bytes },
			  throws{ throw_at_limit }
		{}

	  protected:
		int_type overflow(int_type ch) override
		{
			if (traits_type::eq_int_type(ch, traits_type::eof()))
				return traits_type::not_eof(ch);
			const auto c = traits_type::to_char_type(ch);
			return xsputn(&c, 1) == 1 ? ch : traits_type::eof();
		}

		std::streamsize xsputn(const char* str, std::streamsize len) override
		{
			const auto accepted = (std::min)(static_cast<size_t>(len), limit - received.size());
			received.append(str, accepted);
#if TOML_EXCEPTIONS
			if (throws && accepted < static_cast<size_t>(len))
				throw std::runtime_error{ "the disk is full" };
#endif
			return static_cast<std::streamsize>(accepted);
		}
	};
}

#define CHECK_FORMATTER(formatter, data, expected)                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		const auto str	= format_to_string<formatter>(data);                                                           \
		const auto diff = find_first_difference(str, expected);                                                        \
		if (diff)                                                                                                      \
			FORCE_FAIL("string mismatch: "sv << *diff);                                                                \
	}                                                                                                                  \
	while (false)

#define CHECK_OUTPUT(formatter, ...)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		INFO("checked at line " << __LINE__);                                                                          \
		check_output<formatter>(__VA_ARGS__);                                                                          \
	}                                                                                                                  \
	while (false)

#define CHECK_TOML_ROUND_TRIP(...)                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		INFO("checked at line " << __LINE__);                                                                          \
		check_toml_round_trip(__VA_ARGS__);                                                                            \
	}                                                                                                                  \
	while (false)

TEST_CASE("formatters")
{
	const auto data_date = toml::date{ 2021, 11, 2 };
	const auto data_time = toml::time{ 20, 33, 0 };
	const auto data		 = toml::table{
		{ "integers"sv,
		  toml::table{ { "zero"sv, 0 },
					   { "one"sv, 1 },
					   { "dec"sv, 10 },
					   { "bin"sv, 10, toml::value_flags::format_as_binary },
					   { "oct"sv, 10, toml::value_flags::format_as_octal },
					   { "hex"sv, 10, toml::value_flags::format_as_hexadecimal } } },
		{ "floats"sv,
		  toml::table{ { "pos_zero"sv, +0.0 },
					   { "neg_zero"sv, -0.0 },
					   { "one"sv, 1.0 },
					   { "pos_inf"sv, +std::numeric_limits<double>::infinity() },
					   { "neg_inf"sv, -std::numeric_limits<double>::infinity() },
					   { "pos_nan"sv, +std::numeric_limits<double>::quiet_NaN() },
					   { "neg_nan"sv, -std::numeric_limits<double>::quiet_NaN() }

		  } },

		{ "dates and times"sv,
		  toml::table{

			  { "dates"sv, toml::table{ { "val"sv, data_date } } },

			  { "times"sv, toml::table{ { "val"sv, data_time } } },

			  { "date-times"sv,
				toml::table{

					{ "local"sv, toml::table{ { "val"sv, toml::date_time{ data_date, data_time } } } },
					{ "offset"sv,
					  toml::table{
						  { "val"sv, toml::date_time{ data_date, data_time, toml::time_offset{} } } } } } } } },

		{ "bools"sv,
		  toml::table{ { "true"sv, true }, //
					   { "false"sv, false } } },

		{
			"strings"sv,
			toml::array{ R"()"sv,
						 R"(string)"sv,
						 R"(string with a single quote in it: ')"sv,
						 R"(string with a double quote in it: ")"sv,
						 "string with a tab: \t"sv,
						 R"(a long string to force the array over multiple lines)"sv },
		},

		{ "a"sv,
		  toml::table{ { "val", true },
					   { "b"sv, toml::table{ { "val", true }, { "c"sv, toml::table{ { "val", true } } } } } } }

	};

	SECTION("toml_formatter")
	{
		static constexpr auto expected = R"(*****
strings = [
    '',
    'string',
    "string with a single quote in it: '",
    'string with a double quote in it: "',
    'string with a tab: 	',
    'a long string to force the array over multiple lines'
]

[a]
val = true

    [a.b]
    val = true

        [a.b.c]
        val = true

[bools]
false = false
true = true

['dates and times'.date-times.local]
val = 2021-11-02T20:33:00

['dates and times'.date-times.offset]
val = 2021-11-02T20:33:00Z

['dates and times'.dates]
val = 2021-11-02

['dates and times'.times]
val = 20:33:00

[floats]
neg_inf = -inf
neg_nan = nan
neg_zero = -0.0
one = 1.0
pos_inf = inf
pos_nan = nan
pos_zero = 0.0

[integers]
bin = 0b1010
dec = 10
hex = 0xA
oct = 0o12
one = 1
zero = 0
*****)"sv;

		CHECK_FORMATTER(toml_formatter, data, expected);
	}

	SECTION("json_formatter")
	{
		static constexpr auto expected = R"(*****
{
    "a" : {
        "b" : {
            "c" : {
                "val" : true
            },
            "val" : true
        },
        "val" : true
    },
    "bools" : {
        "false" : false,
        "true" : true
    },
    "dates and times" : {
        "date-times" : {
            "local" : {
                "val" : "2021-11-02T20:33:00"
            },
            "offset" : {
                "val" : "2021-11-02T20:33:00Z"
            }
        },
        "dates" : {
            "val" : "2021-11-02"
        },
        "times" : {
            "val" : "20:33:00"
        }
    },
    "floats" : {
        "neg_inf" : "-Infinity",
        "neg_nan" : "NaN",
        "neg_zero" : -0.0,
        "one" : 1.0,
        "pos_inf" : "Infinity",
        "pos_nan" : "NaN",
        "pos_zero" : 0.0
    },
    "integers" : {
        "bin" : 10,
        "dec" : 10,
        "hex" : 10,
        "oct" : 10,
        "one" : 1,
        "zero" : 0
    },
    "strings" : [
        "",
        "string",
        "string with a single quote in it: '",
        "string with a double quote in it: \"",
        "string with a tab: \t",
        "a long string to force the array over multiple lines"
    ]
}
*****)"sv;

		CHECK_FORMATTER(json_formatter, data, expected);
	}

	SECTION("yaml_formatter")
	{
		static constexpr auto expected = R"(*****
a: 
  b: 
    c: 
      val: true
    val: true
  val: true
bools: 
  'false': false
  'true': true
'dates and times': )"
										 R"(
  date-times: 
    local: 
      val: '2021-11-02T20:33:00'
    offset: 
      val: '2021-11-02T20:33:00Z'
  dates: 
    val: '2021-11-02'
  times: 
    val: '20:33:00'
floats: 
  neg_inf: -.inf
  neg_nan: .NAN
  neg_zero: -0.0
  one: 1.0
  pos_inf: .inf
  pos_nan: .NAN
  pos_zero: 0.0
integers: 
  bin: 10
  dec: 10
  hex: 0xA
  oct: 10
  one: 1
  zero: 0
strings: 
  - ''
  - string
  - "string with a single quote in it: '"
  - 'string with a double quote in it: "'
  - "string with a tab: \t"
  - 'a long string to force the array over multiple lines'
*****)"sv;

		CHECK_FORMATTER(yaml_formatter, data, expected);
	}
}

TEST_CASE("formatters - multi-line strings")
{
	// strings that can't be emitted verbatim between multi-line delimiters
	const auto data = toml::table{ { "triple_single_quotes"sv, "a\n'''\n[injected]\nb = 1\nc = '''\n"sv },
								   { "leading_newline"sv, "\nfo'o"sv },
								   { "leading_newline_escaped"sv, "\nfo\"o\x7F"sv } };

	SECTION("toml_formatter")
	{
		static constexpr auto expected = R"(*****
leading_newline = '''

fo'o'''
leading_newline_escaped = """

fo\"o\u007F"""
triple_single_quotes = """a
'''
[injected]
b = 1
c = '''
"""
*****)"sv;

		CHECK_FORMATTER(toml_formatter, data, expected);
	}

	SECTION("round-trip")
	{
		parse_expected_value(FILE_LINE_ARGS,
							 R"("a\n'''\n[injected]\nb = 1\nc = '''\n")"sv,
							 "a\n'''\n[injected]\nb = 1\nc = '''\n"sv);
		parse_expected_value(FILE_LINE_ARGS, R"("\nfo'o")"sv, "\nfo'o"sv);
		parse_expected_value(FILE_LINE_ARGS, R"("\nfo\"o\u007F")"sv, "\nfo\"o\x7F"sv);
	}
}

TEST_CASE("formatters - toml_formatter string flags")
{
	const auto data =
		toml::table{ { "apostrophe", "it's"sv },	{ "lines", "one\ntwo"sv }, { "path", R"(C:\Users\marzer)"sv },
					 { "quoted", R"(say "hi")"sv }, { "tab", "a\tb"sv },	   { "unicode", "café 😀"sv } };

	SECTION("default flags")
	{
		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags,
							  R"(
apostrophe = "it's"
lines = '''one
two'''
path = 'C:\Users\marzer'
quoted = 'say "hi"'
tab = 'a)"
							  "\t"
							  R"(b'
unicode = 'café 😀')"sv);
	}

	SECTION("no flags")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::none, R"(
apostrophe = "it's"
lines = "one\ntwo"
path = "C:\\Users\\marzer"
quoted = "say \"hi\""
tab = "a\tb"
unicode = "caf\u00E9 \U0001F600")"sv);
	}

	SECTION("literal strings")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_literal_strings, R"(
apostrophe = "it's"
lines = "one\ntwo"
path = 'C:\Users\marzer'
quoted = 'say "hi"'
tab = "a\tb"
unicode = "caf\u00E9 \U0001F600")"sv);
	}

	SECTION("multi-line strings")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_multi_line_strings, R"(
apostrophe = "it's"
lines = """one
two"""
path = "C:\\Users\\marzer"
quoted = "say \"hi\""
tab = "a\tb"
unicode = "caf\u00E9 \U0001F600")"sv);
	}

	SECTION("real tabs")
	{
		CHECK_TOML_ROUND_TRIP(data,
							  format_flags::allow_real_tabs_in_strings,
							  R"(
apostrophe = "it's"
lines = "one\ntwo"
path = "C:\\Users\\marzer"
quoted = "say \"hi\""
tab = "a)"
							  "\t"
							  R"(b"
unicode = "caf\u00E9 \U0001F600")"sv);
	}

	SECTION("unicode")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_unicode_strings, R"(
apostrophe = "it's"
lines = "one\ntwo"
path = "C:\\Users\\marzer"
quoted = "say \"hi\""
tab = "a\tb"
unicode = "café 😀")"sv);
	}

	SECTION("multi-line strings with tabs")
	{
		const auto tabbed = toml::table{ { "val", "a\tb\nit's"sv } };
		CHECK_TOML_ROUND_TRIP(tabbed, toml_formatter::default_flags, "val = '''a\tb\nit's'''"sv);
		CHECK_TOML_ROUND_TRIP(tabbed, format_flags::allow_multi_line_strings, R"(val = """a\tb
it's""")"sv);
	}
}

TEST_CASE("formatters - toml_formatter finds non-ASCII text anywhere in a long string")
{
	const auto data = toml::table{ { "ascii", "0123456789 0123456789 0123456789 cafe"sv },
								   { "first_block", "caf\u00E9 0123456789 0123456789 0123456789"sv },
								   { "second_block", "0123456789 01234 caf\u00E9 0123456789 012"sv },
								   { "tail", "0123456789 0123456789 0123456789 caf\u00E9"sv } };

	CHECK_TOML_ROUND_TRIP(data, format_flags::none, R"(
ascii = "0123456789 0123456789 0123456789 cafe"
first_block = "caf\u00E9 0123456789 0123456789 0123456789"
second_block = "0123456789 01234 caf\u00E9 0123456789 012"
tail = "0123456789 0123456789 0123456789 caf\u00E9")"sv);
}

TEST_CASE("formatters - toml_formatter string escapes")
{
	const auto data = toml::table{ { "backslash_apostrophe", "a\\b'c"sv },
								   { "c1_controls", "a\\b\xC2\x80z\xC2\x9F"sv },
								   { "controls", "bell\x07 esc\x1B del\x7F"sv },
								   { "crlf", "a\r\nb"sv },
								   { "every_ascii_trait", "\n\t'\x01 x"sv },
								   { "line_separator", "a\u2028z"sv },
								   { "next_line", "a\u0085z"sv },
								   { "unicode_controls", "é\\\x7F\"\t\x01"sv },
								   { "unicode_lines", "é\n'"sv },
								   { "unicode_lines_controls", "é\n\x01"sv } };

	SECTION("default flags")
	{
		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags,
							  R"(
backslash_apostrophe = "a\\b'c"
c1_controls = "a\\b\u0080z\u009F"
controls = "bell\u0007 esc\u001B del\u007F"
crlf = """a\r
b"""
every_ascii_trait = """

)"
							  "\t"
							  R"('\u0001 x"""
line_separator = "a\u2028z"
next_line = "a\u0085z"
unicode_controls = "é\\\u007F\")"
							  "\t"
							  R"(\u0001"
unicode_lines = '''é
''''
unicode_lines_controls = """é
\u0001""")"sv);
	}

	SECTION("no flags")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::none, R"(
backslash_apostrophe = "a\\b'c"
c1_controls = "a\\b\u0080z\u009F"
controls = "bell\u0007 esc\u001B del\u007F"
crlf = "a\r\nb"
every_ascii_trait = "\n\t'\u0001 x"
line_separator = "a\u2028z"
next_line = "a\u0085z"
unicode_controls = "\u00E9\\\u007F\"\t\u0001"
unicode_lines = "\u00E9\n'"
unicode_lines_controls = "\u00E9\n\u0001")"sv);
	}
}

TEST_CASE("formatters - malformed UTF-8")
{
	SECTION("a sequence cut short at the end is escaped like other invalid bytes")
	{
		const auto data = toml::table{ { "three", "a\xE2\x82"sv }, { "two", "a\xC3"sv } };

		const auto printed = print_with<toml_formatter>(data);
		CHECK(printed == R"(three = "a\u00E2\u0082"
two = "a\u00C3")"sv);
		const auto reparsed = parse_printed(printed);
		REQUIRE(reparsed);
		CHECK((*reparsed)["three"] == "a\xC3\xA2\xC2\x82"sv);
		CHECK((*reparsed)["two"] == "a\xC3\x83"sv);

		CHECK(print_with<json_formatter>(toml::value{ "a\xC3"s }) == R"("a\u00C3")"sv);
	}

	SECTION("invalid bytes are escaped one by one")
	{
		const auto data = toml::table{ { "invalid_byte", "\xFF"sv },
									   { "invalid_continuation", "a\xC3\x28z"sv },
									   { "newline_then_invalid", "x\n\xFF"sv } };

		static constexpr auto expected = R"(
invalid_byte = "\u00FF"
invalid_continuation = "a\u00C3\u0028z"
newline_then_invalid = "x\n\u00FF")"sv;

		for (auto flags : { toml_formatter::default_flags, format_flags::none })
		{
			const auto printed = print_with<toml_formatter>(data, flags);
			CHECK(printed == skip_leading_newline(expected));
			const auto reparsed = parse_printed(printed);
			REQUIRE(reparsed);
			CHECK((*reparsed)["invalid_byte"] == "\xC3\xBF"sv);
			CHECK((*reparsed)["invalid_continuation"] == "a\xC3\x83(z"sv);
			CHECK((*reparsed)["newline_then_invalid"] == "x\n\xC3\xBF"sv);
		}
	}
}

TEST_CASE("formatters - toml_formatter integers")
{
	const auto data = toml::table{ { "bin", 10, value_flags::format_as_binary },
								   { "dec", 10 },
								   { "hex", 255, value_flags::format_as_hexadecimal },
								   { "max_bin", i64_max, value_flags::format_as_binary },
								   { "max_hex", i64_max, value_flags::format_as_hexadecimal },
								   { "max_oct", i64_max, value_flags::format_as_octal },
								   { "min_hex", i64_min, value_flags::format_as_hexadecimal },
								   { "negative_bin", -10, value_flags::format_as_binary },
								   { "negative_hex", -255, value_flags::format_as_hexadecimal },
								   { "negative_oct", -8, value_flags::format_as_octal },
								   { "oct", 8, value_flags::format_as_octal },
								   { "zero_hex", 0, value_flags::format_as_hexadecimal } };

	SECTION("all bases allowed")
	{
		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
bin = 0b1010
dec = 10
hex = 0xFF
max_bin = 0b111111111111111111111111111111111111111111111111111111111111111
max_hex = 0x7FFFFFFFFFFFFFFF
max_oct = 0o777777777777777777777
min_hex = -9223372036854775808
negative_bin = -10
negative_hex = -255
negative_oct = -8
oct = 0o10
zero_hex = 0x0)"sv);
	}

	SECTION("no bases allowed")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::none, R"(
bin = 10
dec = 10
hex = 255
max_bin = 9223372036854775807
max_hex = 9223372036854775807
max_oct = 9223372036854775807
min_hex = -9223372036854775808
negative_bin = -10
negative_hex = -255
negative_oct = -8
oct = 8
zero_hex = 0)"sv);
	}

	SECTION("binary only")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_binary_integers, R"(
bin = 0b1010
dec = 10
hex = 255
max_bin = 0b111111111111111111111111111111111111111111111111111111111111111
max_hex = 9223372036854775807
max_oct = 9223372036854775807
min_hex = -9223372036854775808
negative_bin = -10
negative_hex = -255
negative_oct = -8
oct = 8
zero_hex = 0)"sv);
	}

	SECTION("octal only")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_octal_integers, R"(
bin = 10
dec = 10
hex = 255
max_bin = 9223372036854775807
max_hex = 9223372036854775807
max_oct = 0o777777777777777777777
min_hex = -9223372036854775808
negative_bin = -10
negative_hex = -255
negative_oct = -8
oct = 0o10
zero_hex = 0)"sv);
	}

	SECTION("hexadecimal only")
	{
		CHECK_TOML_ROUND_TRIP(data, format_flags::allow_hexadecimal_integers, R"(
bin = 10
dec = 10
hex = 0xFF
max_bin = 9223372036854775807
max_hex = 0x7FFFFFFFFFFFFFFF
max_oct = 9223372036854775807
min_hex = -9223372036854775808
negative_bin = -10
negative_hex = -255
negative_oct = -8
oct = 8
zero_hex = 0x0)"sv);
	}
}

TEST_CASE("formatters - toml_formatter floats")
{
	const auto exact = toml::table{ { "half", 0.5 },
									{ "inf", f64_inf },
									{ "integral", 1.0 },
									{ "lowest", std::numeric_limits<double>::lowest() },
									{ "max", (std::numeric_limits<double>::max)() },
									{ "min_normal", (std::numeric_limits<double>::min)() },
									{ "mixed", 123456.789 },
									{ "nan", f64_nan },
									{ "neg_inf", -f64_inf },
									{ "neg_zero", -0.0 },
									{ "pow2_53", 9007199254740992.0 },
									{ "pow10_17", 1e17 },
									{ "zero", 0.0 } };

	const auto inexact = toml::table{ { "big", 1e300 },		{ "epsilon", std::numeric_limits<double>::epsilon() },
									  { "pow10_16", 1e16 }, { "pow10_minus_5", 1e-5 },
									  { "tenth", 0.1 },		{ "third", 1.0 / 3.0 } };

	SECTION("full precision")
	{
		CHECK_TOML_ROUND_TRIP(exact, toml_formatter::default_flags, R"(
half = 0.5
inf = inf
integral = 1.0
lowest = -1.7976931348623157e+308
max = 1.7976931348623157e+308
min_normal = 2.2250738585072014e-308
mixed = 123456.789
nan = nan
neg_inf = -inf
neg_zero = -0.0
pow10_17 = 1e+17
pow2_53 = 9007199254740992.0
zero = 0.0)"sv);

		const auto reparsed = parse_printed(print_with<toml_formatter>(exact));
		REQUIRE(reparsed);
		CHECK(std::signbit(*(*reparsed)["neg_zero"].value<double>()));
		CHECK(!std::signbit(*(*reparsed)["zero"].value<double>()));
	}

	SECTION("full precision of inexact values")
	{
		CHECK_TOML_ROUND_TRIP(inexact, toml_formatter::default_flags, R"(
big = 1e+300
epsilon = 2.220446049250313e-16
pow10_16 = 1e+16
pow10_minus_5 = 1e-05
tenth = 0.1
third = 0.3333333333333333)"sv);
	}

	SECTION("fixed notation from 1e-4 up to 1e16")
	{
		const auto boundaries = toml::table{ { "fixed_max", std::nextafter(1e16, 0.0) },
											 { "fixed_min", 1e-4 },
											 { "just_below_fixed_min", std::nextafter(1e-4, 0.0) },
											 { "pow10_15", 1e15 },
											 { "pow10_5", 100000.0 },
											 { "pow2_minus_1017", std::ldexp(1.0, -1017) },
											 { "seventeen_digits", 123456789012345680.0 } };

		CHECK_TOML_ROUND_TRIP(boundaries, toml_formatter::default_flags, R"(
fixed_max = 9999999999999998.0
fixed_min = 0.0001
just_below_fixed_min = 9.999999999999999e-05
pow10_15 = 1000000000000000.0
pow10_5 = 100000.0
pow2_minus_1017 = 7.120236347223045e-307
seventeen_digits = 1.2345678901234568e+17)"sv);
	}

	SECTION("relaxed precision")
	{
		CHECK_OUTPUT(toml_formatter, exact, format_flags::relaxed_float_precision, R"(
half = 0.5
inf = inf
integral = 1.0
lowest = -1.79769e+308
max = 1.79769e+308
min_normal = 2.22507e-308
mixed = 123457.0
nan = nan
neg_inf = -inf
neg_zero = -0.0
pow10_17 = 1e+17
pow2_53 = 9.0072e+15
zero = 0.0)"sv);

		CHECK_OUTPUT(toml_formatter, inexact, format_flags::relaxed_float_precision, R"(
big = 1e+300
epsilon = 2.22045e-16
pow10_16 = 1e+16
pow10_minus_5 = 1e-05
tenth = 0.1
third = 0.333333)"sv);

		const auto boundaries = toml::table{ { "denorm_min", std::numeric_limits<double>::denorm_min() },
											 { "fixed_max", 999999.0 },
											 { "rounds_to_fixed_min", std::nextafter(1e-4, 0.0) },
											 { "rounds_up_out_of_fixed", 999999.5 },
											 { "seven_digits", 1234567.0 } };

		CHECK_OUTPUT(toml_formatter, boundaries, format_flags::relaxed_float_precision, R"(
denorm_min = 5e-324
fixed_max = 999999.0
rounds_to_fixed_min = 0.0001
rounds_up_out_of_fixed = 1e+06
seven_digits = 1.23457e+06)"sv);
	}

	SECTION("quoted infinities and nans")
	{
		const auto specials = toml::table{ { "inf", f64_inf }, { "nan", f64_nan }, { "neg_inf", -f64_inf } };
		const auto printed	= print_with<toml_formatter>(specials, format_flags::quote_infinities_and_nans);
		CHECK(printed == R"(inf = "inf"
nan = "nan"
neg_inf = "-inf")"sv);

		const auto reparsed = parse_printed(printed);
		REQUIRE(reparsed);
		CHECK(*reparsed == toml::table{ { "inf", "inf"sv }, { "nan", "nan"sv }, { "neg_inf", "-inf"sv } });
	}

	SECTION("denormals")
	{
		const auto denormals = toml::table{ { "denorm_min", std::numeric_limits<double>::denorm_min() },
											{ "neg_denorm_min", -std::numeric_limits<double>::denorm_min() } };
		CHECK_TOML_ROUND_TRIP(denormals, toml_formatter::default_flags, R"(
denorm_min = 5e-324
neg_denorm_min = -5e-324)"sv);
	}
}

TEST_CASE("formatters - toml_formatter dates and times")
{
	const auto data = toml::table{
		{ "date", toml::date{ 1979, 5, 27 } },
		{ "date_max", toml::date{ 9999, 12, 31 } },
		{ "date_min", toml::date{ 0, 1, 1 } },
		{ "local", toml::date_time{ { 1979, 5, 27 }, { 7, 32, 0 } } },
		{ "negative_offset", toml::date_time{ { 1979, 5, 27 }, { 0, 32, 0, 999999000 }, { -7, 0 } } },
		{ "negative_sub_hour_offset", toml::date_time{ { 1979, 5, 27 }, { 0, 32, 0 }, { 0, -45 } } },
		{ "positive_offset", toml::date_time{ { 1979, 5, 27 }, { 0, 32, 0 }, { 5, 30 } } },
		{ "sub_hour_offset", toml::date_time{ { 1979, 5, 27 }, { 0, 32, 0 }, { 0, 30 } } },
		{ "time", toml::time{ 7, 32, 0 } },
		{ "time_max", toml::time{ 23, 59, 59, 999999999 } },
		{ "time_nanosecond", toml::time{ 0, 0, 0, 1 } },
		{ "time_tenth", toml::time{ 10, 20, 34, 500000000 } },
		{ "utc", toml::date_time{ { 1979, 5, 27 }, { 7, 32, 0 }, {} } },
	};

	SECTION("unquoted")
	{
		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
date = 1979-05-27
date_max = 9999-12-31
date_min = 0000-01-01
local = 1979-05-27T07:32:00
negative_offset = 1979-05-27T00:32:00.999999-07:00
negative_sub_hour_offset = 1979-05-27T00:32:00-00:45
positive_offset = 1979-05-27T00:32:00+05:30
sub_hour_offset = 1979-05-27T00:32:00+00:30
time = 07:32:00
time_max = 23:59:59.999999999
time_nanosecond = 00:00:00.000000001
time_tenth = 10:20:34.5
utc = 1979-05-27T07:32:00Z)"sv);
	}

	SECTION("quoted")
	{
		const auto subset = toml::table{ { "date", toml::date{ 1979, 5, 27 } },
										 { "date_time", toml::date_time{ { 1979, 5, 27 }, { 7, 32, 0 }, { 5, 30 } } },
										 { "time", toml::time{ 7, 32, 0 } } };
		const auto as_strings = toml::table{ { "date", "1979-05-27"sv },
											 { "date_time", "1979-05-27T07:32:00+05:30"sv },
											 { "time", "07:32:00"sv } };

		CHECK_TOML_ROUND_TRIP(
			as_strings,
			toml_formatter::default_flags,
			print_with<toml_formatter>(subset, toml_formatter::default_flags | format_flags::quote_dates_and_times));
		CHECK_OUTPUT(toml_formatter,
					 subset,
					 toml_formatter::default_flags | format_flags::quote_dates_and_times,
					 R"(
date = '1979-05-27'
date_time = '1979-05-27T07:32:00+05:30'
time = '07:32:00')"sv);
		CHECK_OUTPUT(toml_formatter, subset, format_flags::quote_dates_and_times, R"(
date = "1979-05-27"
date_time = "1979-05-27T07:32:00+05:30"
time = "07:32:00")"sv);
	}
}

TEST_CASE("formatters - toml_formatter keys")
{
	SECTION("quoting")
	{
		const auto data = toml::table{ { "", 1 },	 { "\"quoted\"", 2 },  { "a b", 3 },  { "a\nb", 4 }, { "a\tb", 5 },
									   { "a.b", 6 }, { "bare-key_09", 7 }, { "café", 8 }, { "it's", 9 } };

#if TOML_LANG_UNRELEASED
		static constexpr auto unicode_key = "café = 8"sv;
#else
		static constexpr auto unicode_key = "'café' = 8"sv;
#endif

		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags,
							  R"('' = 1
'"quoted"' = 2
"a\tb" = 5
"a\nb" = 4
'a b' = 3
'a.b' = 6
bare-key_09 = 7
)"s + std::string{ unicode_key } + R"(
"it's" = 9)");

		CHECK_TOML_ROUND_TRIP(data,
							  format_flags::none,
							  R"(
"" = 1
"\"quoted\"" = 2
"a\tb" = 5
"a\nb" = 4
"a b" = 3
"a.b" = 6
bare-key_09 = 7
)"
							  "\"caf\\u00E9\" = 8"
							  R"(
"it's" = 9)"sv);
	}

	SECTION("keys containing U+1680 or U+FEFF are quoted")
	{
		for (const auto key : { "\xE1\x9A\x80"sv,
								"\xEF\xBB\xBF"sv,
								"\xEF\xBB\xBF"
								"a"sv,
								"a\xEF\xBB\xBF"sv })
		{
			const auto data = toml::table{ { key, 1 } };
			CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, "'" + std::string{ key } + "' = 1");
		}
	}

#if TOML_LANG_UNRELEASED
	SECTION("U+180E can be bare (toml/pull/891)")
	{
		CHECK_TOML_ROUND_TRIP(toml::table{ { "\xE1\xA0\x8E"sv, 1 } },
							  toml_formatter::default_flags,
							  "\xE1\xA0\x8E = 1"sv);
	}
#endif

	SECTION("table headers")
	{
		const auto data = toml::table{
			{ "a b", toml::table{ { "c.d", toml::table{ { "val", 1 } } } } },
			{ "it's", toml::array{ toml::table{ { "val", 2 } }, toml::table{ { "val", 3 } } } },
		};

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
['a b'.'c.d']
val = 1

[["it's"]]
val = 2

[["it's"]]
val = 3)"sv);
	}
}

TEST_CASE("formatters - toml_formatter tables")
{
	SECTION("empty")
	{
		CHECK_TOML_ROUND_TRIP(toml::table{}, toml_formatter::default_flags, ""sv);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "empty", toml::table{} } }, toml_formatter::default_flags, "[empty]"sv);

		auto inline_empty = toml::table{ { "empty", toml::table{} } };
		inline_empty["empty"].as_table()->is_inline(true);
		CHECK_TOML_ROUND_TRIP(inline_empty, toml_formatter::default_flags, "empty = {}"sv);
	}

	SECTION("inline")
	{
		auto data = toml::table{
			{ "inl",
			  toml::table{ { "arr", toml::array{ 1, 2 } }, { "num", 1 }, { "sub", toml::table{ { "str", "x"sv } } } } }
		};
		data["inl"].as_table()->is_inline(true);

		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags,
							  "inl = { arr = [ 1, 2 ], num = 1, sub = { str = 'x' } }"sv);
		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags | format_flags::terse_key_value_pairs,
							  "inl={ arr=[ 1, 2 ], num=1, sub={ str='x' } }"sv);
	}

	SECTION("only sub-tables")
	{
		const auto data = toml::table{
			{ "a", toml::table{ { "b", toml::table{ { "c", toml::table{ { "val", 1 } } } } } } },
			{ "x", toml::table{ { "aot", toml::array{ toml::table{ { "val", 2 } } } } } },
		};

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
[a.b.c]
val = 1

[[x.aot]]
val = 2)"sv);
	}

	SECTION("inline tables and arrays of tables as children")
	{
		auto data = toml::table{ { "outer",
								   toml::table{ { "aot", toml::array{ toml::table{ { "b", 2 } } } },
												{ "inl", toml::table{ { "a", 1 } } } } } };
		data["outer"]["inl"].as_table()->is_inline(true);

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
[outer]
inl = { a = 1 }

    [[outer.aot]]
    b = 2)"sv);
	}

	SECTION("arrays of tables")
	{
		const auto data = toml::table{
			{ "fruit",
			  toml::array{ toml::table{ { "name", "apple"sv },
										{ "physical", toml::table{ { "color", "red"sv } } },
										{ "variety",
										  toml::array{ toml::table{ { "name", "red delicious"sv } },
													   toml::table{ { "name", "granny smith"sv } } } } },
						   toml::table{ { "name", "banana"sv } } } },
		};

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
[[fruit]]
name = 'apple'

    [fruit.physical]
    color = 'red'

    [[fruit.variety]]
    name = 'red delicious'

    [[fruit.variety]]
    name = 'granny smith'

[[fruit]]
name = 'banana')"sv);

		CHECK_TOML_ROUND_TRIP(data, format_flags::none, R"(
[[fruit]]
name = "apple"

[fruit.physical]
color = "red"

[[fruit.variety]]
name = "red delicious"

[[fruit.variety]]
name = "granny smith"

[[fruit]]
name = "banana")"sv);
	}

	SECTION("indentation of nested tables")
	{
		const auto data = toml::table{
			{ "val", 0 },
			{ "a", toml::table{ { "val", 1 }, { "b", toml::table{ { "val", 2 }, { "arr", toml::array{ 1, 2 } } } } } },
		};

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags | format_flags::force_multi_line_arrays, R"(
val = 0

[a]
val = 1

    [a.b]
    arr = [
        1,
        2
    ]
    val = 2)"sv);

		CHECK_TOML_ROUND_TRIP(data, format_flags::indent_sub_tables | format_flags::force_multi_line_arrays, R"(
val = 0

[a]
val = 1

    [a.b]
    arr = [
    1,
    2
    ]
    val = 2)"sv);

		CHECK_TOML_ROUND_TRIP(data, format_flags::indent_array_elements | format_flags::force_multi_line_arrays, R"(
val = 0

[a]
val = 1

[a.b]
arr = [
    1,
    2
]
val = 2)"sv);
	}

	SECTION("terse key-value pairs")
	{
		const auto data = toml::table{ { "a", 1 }, { "b", toml::table{ { "c", "d"sv } } } };
		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags | format_flags::terse_key_value_pairs, R"(
a=1

[b]
c='d')"sv);
	}
}

TEST_CASE("formatters - toml_formatter arrays")
{
	SECTION("inline")
	{
		auto data = toml::table{
			{ "bools", toml::array{ true, false } },
			{ "empty", toml::array{} },
			{ "floats", toml::array{ 1.5, -2.5, 0.0 } },
			{ "inline_tables", toml::array{ toml::table{ { "a", 1 } }, toml::table{} } },
			{ "ints", toml::array{ 1, -2, 0 } },
			{ "mixed", toml::array{ 1, "two"sv, toml::table{ { "three", 3 } } } },
			{ "nested", toml::array{ toml::array{}, toml::array{ 1, toml::array{ 2 } } } },
			{ "temporal",
			  toml::array{ toml::date{ 2021, 1, 2 },
						   toml::time{ 3, 4, 5 },
						   toml::date_time{ { 2021, 1, 2 }, { 3, 4, 5 }, { 1, 0 } } } },
		};
		data["inline_tables"][0].as_table()->is_inline(true);

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, R"(
bools = [ true, false ]
empty = []
floats = [ 1.5, -2.5, 0.0 ]
inline_tables = [ { a = 1 }, {} ]
ints = [ 1, -2, 0 ]
mixed = [ 1, 'two', { three = 3 } ]
nested = [ [], [ 1, [ 2 ] ] ]
temporal = [ 2021-01-02, 03:04:05, 2021-01-02T03:04:05+01:00 ])"sv);
	}

	SECTION("extremes stay inline")
	{
		const auto data = toml::table{ { "arr", toml::array{ i64_min, i64_max, -f64_inf, f64_inf, f64_nan } } };
		CHECK_TOML_ROUND_TRIP(data,
							  toml_formatter::default_flags,
							  "arr = [ -9223372036854775808, 9223372036854775807, -inf, inf, nan ]"sv);
	}

	SECTION("long arrays wrap")
	{
		toml::array arr;
		for (int i = 0; i < 30; i++)
			arr.push_back(1000 + i);
		const auto data = toml::table{ { "arr", arr } };

		std::string expected = "arr = [";
		for (int i = 0; i < 30; i++)
		{
			expected += i ? ",\n    " : "\n    ";
			expected += std::to_string(1000 + i);
		}
		expected += "\n]";

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags, expected);
	}

	SECTION("integers take the width they print at")
	{
		toml::array arr;
		std::string inline_expected	 = "arr = [ ";
		std::string wrapped_expected = "arr = [";
		for (int i = 0; i < 5; i++)
		{
			arr.push_back(999999999999999999);
			inline_expected += "999999999999999999, ";
			wrapped_expected += "\n    999999999999999999,";
		}

		arr.push_back(12345678901234);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  inline_expected + "12345678901234 ]");

		arr.pop_back();
		arr.push_back(123456789012345);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  wrapped_expected + "\n    123456789012345\n]");
	}

	SECTION("integers take the width of the base they print in")
	{
		toml::array arr;
		std::string inline_expected	 = "arr = [ ";
		std::string wrapped_expected = "arr = [";
		for (int i = 0; i < 9; i++)
		{
			arr.push_back(255, value_flags::format_as_binary);
			inline_expected += "0b11111111, ";
			wrapped_expected += "\n    0b11111111,";
		}

		arr.push_back(4095, value_flags::format_as_octal);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  inline_expected + "0o7777 ]");

		arr.pop_back();
		arr.push_back(32767, value_flags::format_as_octal);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  wrapped_expected + "\n    0o77777\n]");
	}

	SECTION("a hexadecimal integer can take fewer columns than its decimal digits")
	{
		toml::array arr;
		std::string inline_expected	 = "arr = [ ";
		std::string wrapped_expected = "arr = [";
		for (int i = 0; i < 5; i++)
		{
			arr.push_back(i64_max, value_flags::format_as_hexadecimal);
			inline_expected += "0x7FFFFFFFFFFFFFFF, ";
			wrapped_expected += "\n    0x7FFFFFFFFFFFFFFF,";
		}

		arr.push_back(12345678901234);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  inline_expected + "12345678901234 ]");

		arr.pop_back();
		arr.push_back(123456789012345);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags,
							  wrapped_expected + "\n    123456789012345\n]");
	}

	SECTION("an integer that prints in decimal despite its flags takes its decimal width")
	{
		toml::array arr;
		std::string inline_expected	 = "arr = [ ";
		std::string wrapped_expected = "arr = [";
		for (int i = 0; i < 5; i++)
		{
			arr.push_back(999999999999999999, value_flags::format_as_binary);
			inline_expected += "999999999999999999, ";
			wrapped_expected += "\n    999999999999999999,";
		}

		// the formatter doesn't allow the format
		arr.push_back(12345678901234);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags & ~format_flags::allow_binary_integers,
							  inline_expected + "12345678901234 ]");

		// negative integers are always decimal
		arr.pop_back();
		arr.push_back(-12345678901234, value_flags::format_as_hexadecimal);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", arr } },
							  toml_formatter::default_flags & ~format_flags::allow_binary_integers,
							  wrapped_expected + "\n    -12345678901234\n]");
	}

	SECTION("floats take the width they print at")
	{
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", toml::array{ 1e300, -1e300, 1e-300, 5e-324, 1e16 } } },
							  toml_formatter::default_flags,
							  "arr = [ 1e+300, -1e+300, 1e-300, 5e-324, 1e+16 ]"sv);

		toml::array six;
		for (int i = 0; i < 6; i++)
			six.push_back(0.123456789012345);
		toml::array seven = six;
		seven.push_back(0.123456789012345);

		std::string inline_expected	 = "arr = [ ";
		std::string wrapped_expected = "arr = [";
		for (int i = 0; i < 7; i++)
		{
			if (i < 6)
				inline_expected += i ? ", 0.123456789012345" : "0.123456789012345";
			wrapped_expected += i ? ",\n    0.123456789012345" : "\n    0.123456789012345";
		}
		inline_expected += " ]";
		wrapped_expected += "\n]";

		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", six } }, toml_formatter::default_flags, inline_expected);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", seven } }, toml_formatter::default_flags, wrapped_expected);
	}

	SECTION("short floats take the width they print at")
	{
		// 119 columns stay on one line and 120 wrap: "[ ]" is 3, and each element is its text plus ", "
		toml::array fits;
		std::string fits_expected  = "arr = [ ";
		std::string wraps_expected = "arr = [";
		for (int i = 0; i < 15; i++)
		{
			fits.push_back(12.25);
			fits_expected += "12.25, ";
			wraps_expected += "\n    12.25,";
		}
		fits.push_back(-0.5);
		toml::array wraps = fits;
		fits.push_back(0.0);
		wraps.push_back(1.25);
		fits_expected += "-0.5, 0.0 ]";
		wraps_expected += "\n    -0.5,\n    1.25\n]";

		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", fits } }, toml_formatter::default_flags, fits_expected);
		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", wraps } }, toml_formatter::default_flags, wraps_expected);
	}

	SECTION("dates and times take the width they print at")
	{
		const auto repeated = [](const auto& val, size_t count)
		{
			toml::array arr;
			for (size_t i = 0; i < count; i++)
				arr.push_back(val);
			return toml::table{ { "arr", arr } };
		};
		const auto printed = [](std::string_view text, size_t count, bool wrapped)
		{
			std::string expected = wrapped ? "arr = [" : "arr = [ ";
			for (size_t i = 0; i < count; i++)
			{
				if (wrapped)
					expected += i ? ",\n    " : "\n    ";
				else if (i)
					expected += ", ";
				expected += text;
			}
			return expected + (wrapped ? "\n]" : " ]");
		};

		const toml::time short_time{ 7, 32, 0 };
		const toml::time long_time{ 7, 32, 0, 123456789 };
		CHECK_TOML_ROUND_TRIP(repeated(short_time, 11), toml_formatter::default_flags, printed("07:32:00", 11, false));
		CHECK_TOML_ROUND_TRIP(repeated(short_time, 12), toml_formatter::default_flags, printed("07:32:00", 12, true));
		CHECK_TOML_ROUND_TRIP(repeated(long_time, 5),
							  toml_formatter::default_flags,
							  printed("07:32:00.123456789", 5, false));
		CHECK_TOML_ROUND_TRIP(repeated(long_time, 6),
							  toml_formatter::default_flags,
							  printed("07:32:00.123456789", 6, true));

		const toml::date date{ 1979, 5, 27 };
		CHECK_TOML_ROUND_TRIP(repeated(date, 9), toml_formatter::default_flags, printed("1979-05-27", 9, false));
		CHECK_TOML_ROUND_TRIP(repeated(date, 10), toml_formatter::default_flags, printed("1979-05-27", 10, true));

		const toml::date_time local{ date, short_time };
		const toml::date_time offset{ date, long_time, toml::time_offset{ -8, 0 } };
		CHECK_TOML_ROUND_TRIP(repeated(local, 5),
							  toml_formatter::default_flags,
							  printed("1979-05-27T07:32:00", 5, false));
		CHECK_TOML_ROUND_TRIP(repeated(local, 6),
							  toml_formatter::default_flags,
							  printed("1979-05-27T07:32:00", 6, true));
		CHECK_TOML_ROUND_TRIP(repeated(offset, 3),
							  toml_formatter::default_flags,
							  printed("1979-05-27T07:32:00.123456789-08:00", 3, false));
		CHECK_TOML_ROUND_TRIP(repeated(offset, 4),
							  toml_formatter::default_flags,
							  printed("1979-05-27T07:32:00.123456789-08:00", 4, true));

		CHECK_TOML_ROUND_TRIP(repeated(true, 19), toml_formatter::default_flags, printed("true", 19, false));
		CHECK_TOML_ROUND_TRIP(repeated(true, 20), toml_formatter::default_flags, printed("true", 20, true));
	}

	SECTION("arrays holding long inline tables wrap")
	{
		toml::table inner;
		std::string expected = "arr = [\n    { ";
		for (int i = 10; i < 30; i++)
		{
			const auto key = "key_" + std::to_string(i);
			inner.insert(key, i);
			expected += (i > 10 ? ", " : "") + key + " = " + std::to_string(i);
		}
		expected += " }\n]";
		inner.is_inline(true);

		CHECK_TOML_ROUND_TRIP(toml::table{ { "arr", toml::array{ inner } } }, toml_formatter::default_flags, expected);
	}

	SECTION("forced multi-line")
	{
		static_assert(format_flags::force_multiline_arrays == format_flags::force_multi_line_arrays,
					  "the flag's first spelling stays until v4");

		auto data = toml::table{ { "arr", toml::array{ 1, toml::array{ 2, 3 }, toml::array{} } },
								 { "inl", toml::table{ { "arr", toml::array{ 4 } } } } };
		data["inl"].as_table()->is_inline(true);

		CHECK_TOML_ROUND_TRIP(data, toml_formatter::default_flags | format_flags::force_multi_line_arrays, R"(
arr = [
    1,
    [
        2,
        3
    ],
    []
]
inl = { arr = [
    4
] })"sv);

		CHECK_TOML_ROUND_TRIP(data, format_flags::force_multi_line_arrays, R"(
arr = [
1,
[
2,
3
],
[]
]
inl = { arr = [
4
] })"sv);
	}
}

TEST_CASE("formatters - toml_formatter root nodes")
{
	auto inline_table = toml::table{ { "a", 1 }, { "b", toml::array{ 2 } } };
	inline_table.is_inline(true);

	CHECK_OUTPUT(toml_formatter, inline_table, toml_formatter::default_flags, "{ a = 1, b = [ 2 ] }"sv);
	CHECK_OUTPUT(toml_formatter, toml::array{ 1, "two"sv }, toml_formatter::default_flags, "[ 1, 'two' ]"sv);
	CHECK_OUTPUT(toml_formatter, toml::array{}, toml_formatter::default_flags, "[]"sv);
	CHECK_OUTPUT(toml_formatter,
				 toml::array{ 1, 2 },
				 toml_formatter::default_flags | format_flags::force_multi_line_arrays,
				 "[\n    1,\n    2\n]"sv);

	SECTION("documented examples")
	{
		const auto doc = toml::table{ { "description", "This is some TOML, yo." },
									  { "fruit", toml::array{ "apple", "orange", "pear" } },
									  { "numbers", toml::array{ 1, 2, 3, 4, 5 } },
									  { "table", toml::table{ { "foo", "bar" } } } };
		CHECK(print_with<toml_formatter>(doc) == R"(description = 'This is some TOML, yo.'
fruit = [ 'apple', 'orange', 'pear' ]
numbers = [ 1, 2, 3, 4, 5 ]

[table]
foo = 'bar')"sv);

		auto tbl = toml::table{ { "a", 1 }, { "b", 2 }, { "c", 3 }, { "d", toml::table{ { "e", 4 } } } };
		CHECK(print_with<toml_formatter>(tbl) == "a = 1\nb = 2\nc = 3\n\n[d]\ne = 4"sv);

		tbl.is_inline(true);
		CHECK(print_with<toml_formatter>(tbl) == "{ a = 1, b = 2, c = 3, d = { e = 4 } }"sv);
	}
}

TEST_CASE("formatters - printing a formatter twice gives the same output")
{
	const auto data =
		toml::table{ { "a", toml::table{ { "b", 1 } } }, { "c", toml::array{ toml::table{ { "d", 2 } } } } };

	const auto check_reuse = [&](auto&& formatter, std::string_view expected)
	{
		std::ostringstream first, second;
		first << formatter;
		second << formatter;
		CHECK(first.str() == skip_leading_newline(expected));
		CHECK(second.str() == skip_leading_newline(expected));
	};

	SECTION("toml_formatter")
	{
		toml_formatter formatter{ data };
		check_reuse(formatter, "[a]\nb = 1\n\n[[c]]\nd = 2"sv);
	}

	SECTION("json_formatter")
	{
		json_formatter formatter{ data };
		check_reuse(formatter, R"(
{
    "a" : {
        "b" : 1
    },
    "c" : [
        {
            "d" : 2
        }
    ]
})"sv);
	}

	SECTION("yaml_formatter")
	{
		yaml_formatter formatter{ data };
		check_reuse(formatter, "a: \n  b: 1\nc: \n  - d: 2"sv);
	}
}

TEST_CASE("formatters - long output")
{
	toml::array numbers;
	for (int64_t i = 0; i < 2000; i++)
		numbers.push_back(i);
	toml::array strings;
	for (const size_t length : { 1000u, 3000u, 4095u, 4096u, 4097u, 10000u })
		strings.push_back(letters(length));
	const auto data = toml::table{ { "numbers", numbers }, { "strings", strings } };

	std::string toml_expected = "numbers = [";
	std::string json_expected = "{\n    \"numbers\" : [";
	std::string yaml_expected = "numbers: ";
	for (size_t i = 0; i < numbers.size(); i++)
	{
		toml_expected += (i ? ",\n    " : "\n    ") + std::to_string(i);
		json_expected += (i ? ",\n        " : "\n        ") + std::to_string(i);
		yaml_expected += "\n  - " + std::to_string(i);
	}
	toml_expected += "\n]\nstrings = [";
	json_expected += "\n    ],\n    \"strings\" : [";
	yaml_expected += "\nstrings: ";
	for (size_t i = 0; i < strings.size(); i++)
	{
		const auto& str = strings[i].ref<std::string>();
		toml_expected += (i ? ",\n    '" : "\n    '") + str + "'";
		json_expected += (i ? ",\n        \"" : "\n        \"") + str + "\"";
		yaml_expected += "\n  - " + str;
	}
	toml_expected += "\n]";
	json_expected += "\n    ]\n}";

	const auto toml_printed = print_with<toml_formatter>(data);
	check_same(toml_printed, toml_expected);
	const auto reparsed = parse_printed(toml_printed);
	REQUIRE(reparsed);
	CHECK(*reparsed == data);
	check_same(print_with<json_formatter>(data), json_expected);
	check_same(print_with<yaml_formatter>(data), yaml_expected);
}

TEST_CASE("formatters - deep indentation")
{
	static constexpr size_t depth = 50;
	toml::array root;
	auto innermost = &root;
	for (size_t i = 1; i < depth; i++)
	{
		innermost->push_back(toml::array{});
		innermost = innermost->back().as_array();
	}
	innermost->push_back(1);

	std::string expected;
	for (size_t level = 1; level <= depth; level++)
		expected += "[\n" + std::string(level * 4u, ' ');
	expected += '1';
	for (size_t level = depth; level-- > 0u;)
		expected += '\n' + std::string(level * 4u, ' ') + ']';

	check_same(print_with<json_formatter>(root), expected);
}

TEST_CASE("formatters - a stream that fails partway")
{
	toml::array numbers;
	for (int64_t i = 0; i < 1000; i++)
		numbers.push_back(i);
	toml::array strings;
	for (const size_t length : { 100u, 3000u, 5000u })
		strings.push_back(letters(length));
	const auto data = toml::table{ { "numbers", numbers }, { "strings", strings } };

	const auto check_failures = [](auto& formatter)
	{
		std::ostringstream ss;
		ss << formatter;
		const auto full = ss.str();
		REQUIRE(full.size() > 8192u);

		for (const size_t limit : { size_t{},
									size_t{ 1 },
									size_t{ 100 },
									size_t{ 4095 },
									size_t{ 4096 },
									size_t{ 4097 },
									full.size() - 1u,
									full.size(),
									full.size() + 1u })
		{
			CAPTURE(limit);
			const auto delivered = full.substr(0, (std::min)(limit, full.size()));
			const auto fails	 = limit < full.size();

			limited_buf refusing{ limit };
			std::ostream refusing_stream{ &refusing };
			refusing_stream << formatter;
			CHECK(refusing.received == delivered);
			CHECK(refusing_stream.bad() == fails);

#if TOML_EXCEPTIONS
			limited_buf throwing{ limit, true };
			std::ostream throwing_stream{ &throwing };
			throwing_stream << formatter;
			CHECK(throwing.received == delivered);
			CHECK(throwing_stream.bad() == fails);

			limited_buf rethrowing{ limit, true };
			std::ostream rethrowing_stream{ &rethrowing };
			rethrowing_stream.exceptions(std::ios::badbit);
			if (fails)
				CHECK_THROWS_WITH(rethrowing_stream << formatter, "the disk is full");
			else
				CHECK_NOTHROW(rethrowing_stream << formatter);
			CHECK(rethrowing.received == delivered);
#endif
		}

		std::ostringstream again;
		again << formatter;
		CHECK(again.str() == full);
	};

	SECTION("toml_formatter")
	{
		toml_formatter formatter{ data };
		check_failures(formatter);
	}

	SECTION("json_formatter")
	{
		json_formatter formatter{ data };
		check_failures(formatter);
	}

	SECTION("yaml_formatter")
	{
		yaml_formatter formatter{ data };
		check_failures(formatter);
	}
}

static constexpr auto documented_fruit = R"(
	[fruit]
	apple.color = "red"
	apple.taste.sweet = true

	[fruit.apple.texture]
	smooth = true
)"sv;

TEST_CASE("formatters - json_formatter")
{
	SECTION("documented example")
	{
		const auto data = parse_printed(documented_fruit);
		REQUIRE(data);
		CHECK_OUTPUT(json_formatter, *data, json_formatter::default_flags, R"(
{
    "fruit" : {
        "apple" : {
            "color" : "red",
            "taste" : {
                "sweet" : true
            },
            "texture" : {
                "smooth" : true
            }
        }
    }
})"sv);
	}

	const auto data = toml::table{
		{ "arr", toml::array{ 1, toml::array{ 2 }, toml::table{ { "x", 3 } }, toml::array{}, toml::table{} } },
		{ "date", toml::date{ 2021, 1, 2 } },
		{ "empty", toml::table{} },
		{ "hex", 255, value_flags::format_as_hexadecimal },
		{ "nan", f64_nan },
		{ "neg_inf", -f64_inf },
		{ "str", "it's \"x\"\n\tcafé"sv },
	};

	SECTION("default flags")
	{
		CHECK_OUTPUT(json_formatter, data, json_formatter::default_flags, R"(
{
    "arr" : [
        1,
        [
            2
        ],
        {
            "x" : 3
        },
        [],
        {}
    ],
    "date" : "2021-01-02",
    "empty" : {},
    "hex" : 255,
    "nan" : "NaN",
    "neg_inf" : "-Infinity",
    "str" : "it's \"x\"\n\tcafé"
})"sv);
	}

	SECTION("no flags")
	{
		auto finite = data;
		finite.erase("nan");
		finite.erase("neg_inf");
		CHECK_OUTPUT(json_formatter, finite, format_flags::none, R"(
{
"arr" : [
1,
[
2
],
{
"x" : 3
},
[],
{}
],
"date" : "2021-01-02",
"empty" : {},
"hex" : 255,
"str" : "it's \"x\"\n\tcaf\u00E9"
})"sv);
	}

	SECTION("control characters are escaped")
	{
		std::string controls;
		for (int c = 0; c < 0x20; c++)
			controls += static_cast<char>(c);
		controls += '\x7F';

		std::string c1_controls;
		for (int c = 0x80; c < 0xA0; c++)
		{
			c1_controls += '\xC2';
			c1_controls += static_cast<char>(c);
		}

		CHECK_OUTPUT(json_formatter, toml::array{ controls, c1_controls }, json_formatter::default_flags, R"(
[
    "\u0000\u0001\u0002\u0003\u0004\u0005\u0006\u0007\b\t\n\u000B\f\r\u000E\u000F\u0010\u0011\u0012\u0013\u0014\u0015\u0016\u0017\u0018\u0019\u001A\u001B\u001C\u001D\u001E\u001F\u007F",
    "\u0080\u0081\u0082\u0083\u0084\u0085\u0086\u0087\u0088\u0089\u008A\u008B\u008C\u008D\u008E\u008F\u0090\u0091\u0092\u0093\u0094\u0095\u0096\u0097\u0098\u0099\u009A\u009B\u009C\u009D\u009E\u009F"
])"sv);
	}

	SECTION("toml-only string flags are ignored")
	{
		CHECK(print_with<json_formatter>(data,
										 json_formatter::default_flags | format_flags::allow_literal_strings
											 | format_flags::allow_multi_line_strings)
			  == print_with<json_formatter>(data));
	}

	SECTION("flags that would make invalid JSON are ignored")
	{
		const auto all_flags = json_formatter::default_flags | format_flags::allow_real_tabs_in_strings
							 | format_flags::allow_binary_integers | format_flags::allow_octal_integers
							 | format_flags::allow_hexadecimal_integers;
		CHECK(print_with<json_formatter>(data, all_flags) == print_with<json_formatter>(data));

		const auto tricky =
			toml::table{ { "hex", 255, value_flags::format_as_hexadecimal }, { "nan", f64_nan }, { "tab", "a\tb"sv } };
		CHECK_OUTPUT(json_formatter,
					 tricky,
					 format_flags::allow_hexadecimal_integers | format_flags::allow_real_tabs_in_strings,
					 R"(
{
"hex" : 255,
"nan" : "NaN",
"tab" : "a\tb"
})"sv);
	}

	SECTION("characters above U+FFFF are escaped as surrogate pairs")
	{
		const auto face = "\xF0\x9F\x98\x80"sv;
		CHECK_OUTPUT(json_formatter,
					 toml::table{ { face, toml::array{ face } } },
					 format_flags::none,
					 R"(
{
"\uD83D\uDE00" : [
"\uD83D\uDE00"
]
})"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ std::string{ face } }, format_flags::none, R"("\uD83D\uDE00")"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ "\xF4\x8F\xBF\xBF"s }, format_flags::none, R"("\uDBFF\uDFFF")"sv);
	}

	SECTION("terse key-value pairs")
	{
		CHECK_OUTPUT(json_formatter,
					 toml::table{ { "a", 1 }, { "b", toml::table{ { "c", true } } } },
					 json_formatter::default_flags | format_flags::terse_key_value_pairs,
					 R"(
{
    "a":1,
    "b":{
        "c":true
    }
})"sv);
	}

	SECTION("indentation flags apply separately")
	{
		const auto nested = toml::table{ { "arr", toml::array{ 1, toml::table{ { "x", 2 } } } } };

		CHECK_OUTPUT(json_formatter, nested, format_flags::indent_sub_tables, R"(
{
    "arr" : [
    1,
    {
        "x" : 2
    }
    ]
})"sv);

		CHECK_OUTPUT(json_formatter, nested, format_flags::indent_array_elements, R"(
{
"arr" : [
    1,
    {
    "x" : 2
    }
]
})"sv);
	}

	SECTION("root nodes")
	{
		CHECK_OUTPUT(json_formatter, toml::table{}, json_formatter::default_flags, "{}"sv);
		CHECK_OUTPUT(json_formatter, toml::array{}, json_formatter::default_flags, "[]"sv);
		CHECK_OUTPUT(json_formatter,
					 toml::array{ 1, "two"sv },
					 json_formatter::default_flags,
					 "[\n    1,\n    \"two\"\n]"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ "two"s }, json_formatter::default_flags, R"("two")"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ 42 }, json_formatter::default_flags, "42"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ 1e16 }, json_formatter::default_flags, "1e+16"sv);
		CHECK_OUTPUT(json_formatter, toml::value{ f64_inf }, json_formatter::default_flags, R"("Infinity")"sv);
		CHECK_OUTPUT(json_formatter,
					 toml::value{ toml::time{ 3, 4, 5 } },
					 json_formatter::default_flags,
					 R"("03:04:05")"sv);
	}
}

TEST_CASE("formatters - yaml_formatter")
{
	SECTION("documented example")
	{
		const auto data = parse_printed(documented_fruit);
		REQUIRE(data);
		CHECK_OUTPUT(yaml_formatter,
					 *data,
					 yaml_formatter::default_flags,
					 "fruit: \n"
					 "  apple: \n"
					 "    color: red\n"
					 "    taste: \n"
					 "      sweet: true\n"
					 "    texture: \n"
					 "      smooth: true"sv);
	}

	SECTION("multi-line strings")
	{
		const auto data = toml::table{
			{ "blank_line", "x\n\ny"sv },		{ "leading_newline", "\nlead"sv },
			{ "lines", "line1\nline2"sv },		{ "nested", toml::table{ { "lines", "a\nb"sv } } },
			{ "seq", toml::array{ "c\nd"sv } },
		};

		CHECK_OUTPUT(yaml_formatter,
					 data,
					 yaml_formatter::default_flags,
					 "blank_line: |-\n"
					 "  x\n"
					 "  \n"
					 "  y\n"
					 "leading_newline: |-\n"
					 "  \n"
					 "  lead\n"
					 "lines: |-\n"
					 "  line1\n"
					 "  line2\n"
					 "nested: \n"
					 "  lines: |-\n"
					 "    a\n"
					 "    b\n"
					 "seq: \n"
					 "  - |-\n"
					 "    c\n"
					 "    d"sv);

		CHECK_OUTPUT(yaml_formatter, toml::value{ "a\nb"s }, yaml_formatter::default_flags, "|-\n  a\n  b"sv);
		CHECK_OUTPUT(yaml_formatter, toml::value{ "a\n\tb"s }, yaml_formatter::default_flags, "|-\n  a\n  \tb"sv);
	}

	SECTION("multi-line strings that block scalars can't hold are quoted")
	{
		const auto data = toml::table{
			{ "bom",
			  "a\n\xEF\xBB\xBF"
			  "b"sv },
			{ "control", "a\x01\nb"sv },
			{ "crlf", "a\r\nb"sv },
			{ "del", "a\x7F\nb"sv },
			{ "indented", "  a\nb"sv },
			{ "indented_after_blank", "\n  a"sv },
			{ "malformed", "a\x80\nb"sv },
			{ "next_line", "a\xC2\x85\nb"sv },
			{ "only_newline", "\n"sv },
			{ "tab_indented", "\ta\nb"sv },
			{ "tab_indented_after_blank", "\n\ta"sv },
			{ "trailing_newline", "a\n"sv },
		};

		CHECK_OUTPUT(yaml_formatter, data, yaml_formatter::default_flags, R"(
bom: "a\n\uFEFFb"
control: "a\u0001\nb"
crlf: "a\r\nb"
del: "a\u007F\nb"
indented: "  a\nb"
indented_after_blank: "\n  a"
malformed: "a\u0080\nb"
next_line: "a\u0085\nb"
only_newline: "\n"
tab_indented: "\ta\nb"
tab_indented_after_blank: "\n\ta"
trailing_newline: "a\n")"sv);
	}

	SECTION("characters YAML can't hold raw are escaped")
	{
		const auto data = toml::table{
			{ "c1",
			  "a\xC2\x80"
			  "b"sv },
			{ "feff", "\xEF\xBB\xBF"sv },
			{ "fffe", "\xEF\xBF\xBE"sv },
			{ "ffff", "\xEF\xBF\xBF"sv },
			{ "multi_line", "a\n\xC2\x9F"sv },
		};

		CHECK_OUTPUT(yaml_formatter, data, yaml_formatter::default_flags, R"(
c1: "a\u0080b"
feff: "\uFEFF"
fffe: "\uFFFE"
ffff: "\uFFFF"
multi_line: "a\n\u009F")"sv);
		CHECK_OUTPUT(yaml_formatter,
					 toml::table{ { "a\xC2\x80", 1 } },
					 yaml_formatter::default_flags,
					 R"("a\u0080": 1)"sv);
		CHECK_OUTPUT(yaml_formatter,
					 toml::table{ { "a\xEF\xBB\xBF", 1 } },
					 yaml_formatter::default_flags,
					 R"("a\uFEFF": 1)"sv);
	}

	SECTION("single-line strings")
	{
		const auto data = toml::table{
			{ "a b", "plain"sv },	 { "apostrophe", "it's"sv }, { "empty", ""sv },
			{ "path", R"(C:\x)"sv }, { "spaced", "a b"sv },		 { "tab", "a\tb"sv },
		};

		CHECK_OUTPUT(yaml_formatter, data, yaml_formatter::default_flags, R"(
'a b': plain
apostrophe: "it's"
empty: ''
path: 'C:\x'
spaced: 'a b'
tab: "a\tb")"sv);

		CHECK_OUTPUT(yaml_formatter, data, format_flags::none, R"(
"a b": plain
apostrophe: "it's"
empty: ""
path: "C:\\x"
spaced: "a b"
tab: "a\tb")"sv);

		CHECK_OUTPUT(yaml_formatter, toml::value{ "plain"s }, yaml_formatter::default_flags, "plain"sv);
		CHECK_OUTPUT(yaml_formatter, toml::value{ "a b"s }, yaml_formatter::default_flags, "'a b'"sv);
	}

	SECTION("strings and keys YAML would read as something else are quoted")
	{
		const auto data = toml::table{
			{ "123", "-"sv },  { "hex", "0x1F"sv }, { "null", "NULL"sv }, { "num", "1e5"sv },
			{ "off", "On"sv }, { "plain", "-a"sv }, { "true", "yes"sv },  { "word", "nullable"sv },
		};

		CHECK_OUTPUT(yaml_formatter, data, yaml_formatter::default_flags, R"(
'123': '-'
hex: '0x1F'
'null': 'NULL'
num: '1e5'
'off': 'On'
plain: '-a'
'true': 'yes'
word: nullable)"sv);

		CHECK_OUTPUT(yaml_formatter, toml::value{ "false"s }, format_flags::none, R"("false")"sv);
		CHECK_OUTPUT(yaml_formatter, toml::array{ "42"sv, "~"sv, "y"sv }, format_flags::none, R"(
  - "42"
  - "~"
  - "y")"sv);
	}

	SECTION("keys too long for the 'key: value' form are written as explicit keys")
	{
		const std::string fits(1022u, 'k');
		const std::string too_long(1023u, 'k');
		const std::string escaped(171u, '\x01');
		const auto data = toml::table{
			{ escaped, 0 },
			{ fits, 1 },
			{ too_long, 2 },
			{ "nested", toml::table{ { too_long, toml::array{ 3 } } } },
			{ "seq", toml::array{ toml::table{ { too_long, "x\ny"sv }, { "z", 4 } } } },
		};

		std::string escaped_text;
		for (size_t i = 0; i < escaped.length(); i++)
			escaped_text += "\\u0001";

		CHECK_OUTPUT(yaml_formatter,
					 data,
					 yaml_formatter::default_flags,
					 "? \"" + escaped_text + "\"\n: 0\n"  //
						 + fits + ": 1\n"				  //
						 + "? " + too_long + "\n: 2\n"	  //
						 + "nested: \n"					  //
						 + "  ? " + too_long + "\n  : \n" //
						 + "    - 3\n"					  //
						 + "seq: \n"					  //
						 + "  - ? " + too_long + "\n"	  //
						 + "    : |-\n"					  //
						 + "      x\n"					  //
						 + "      y\n"					  //
						 + "    z: 4");
	}

	SECTION("a key's length is counted in the characters it prints as")
	{
		const auto repeat = [](std::string_view text, size_t count)
		{
			std::string repeated;
			for (size_t i = 0; i < count; i++)
				repeated += text;
			return repeated;
		};
		const auto with_key	  = [](const std::string& key) { return toml::table{ { key, 1 } }; };
		const auto unicode	  = yaml_formatter::default_flags;
		const auto no_unicode = unicode & ~format_flags::allow_unicode_strings;

		// U+00D7 is two bytes, and is not a bare key character with or without unreleased features
		static constexpr auto times = "\xC3\x97"sv;
		CHECK_OUTPUT(yaml_formatter, with_key(repeat(times, 1022u)), unicode, "'" + repeat(times, 1022u) + "': 1");
		CHECK_OUTPUT(yaml_formatter, with_key(repeat(times, 1023u)), unicode, "? '" + repeat(times, 1023u) + "'\n: 1");

		static constexpr auto escaped_times = R"(\u00D7)"sv;
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(times, 170u)),
					 no_unicode,
					 "\"" + repeat(escaped_times, 170u) + "\": 1");
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(times, 171u)),
					 no_unicode,
					 "? \"" + repeat(escaped_times, 171u) + "\"\n: 1");

		static constexpr auto emoji			= "\xF0\x9F\x98\x80"sv;
		static constexpr auto escaped_emoji = R"(\U0001F600)"sv;
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(emoji, 102u)),
					 no_unicode,
					 "\"" + repeat(escaped_emoji, 102u) + "\": 1");
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(emoji, 103u)),
					 no_unicode,
					 "? \"" + repeat(escaped_emoji, 103u) + "\"\n: 1");

		// U+0085 is escaped whatever the flags
		static constexpr auto next_line			= "\xC2\x85"sv;
		static constexpr auto escaped_next_line = R"(\u0085)"sv;
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(next_line, 170u)),
					 unicode,
					 "\"" + repeat(escaped_next_line, 170u) + "\": 1");
		CHECK_OUTPUT(yaml_formatter,
					 with_key(repeat(next_line, 171u)),
					 unicode,
					 "? \"" + repeat(escaped_next_line, 171u) + "\"\n: 1");
	}

	SECTION("flags that would make YAML read a value back as something else are ignored")
	{
		const auto data = toml::table{
			{ "a", 1 },
			{ "bin", 10, value_flags::format_as_binary },
			{ "oct", 10, value_flags::format_as_octal },
		};

		CHECK_OUTPUT(yaml_formatter,
					 data,
					 yaml_formatter::default_flags | format_flags::terse_key_value_pairs
						 | format_flags::allow_binary_integers | format_flags::allow_octal_integers,
					 R"(
a: 1
bin: 10
oct: 10)"sv);
	}

	SECTION("values")
	{
		const auto data = toml::table{
			{ "bin", 10, value_flags::format_as_binary },
			{ "date", toml::date{ 2021, 1, 2 } },
			{ "hex", 10, value_flags::format_as_hexadecimal },
			{ "inf", f64_inf },
			{ "nan", f64_nan },
			{ "oct", 10, value_flags::format_as_octal },
		};

		CHECK_OUTPUT(yaml_formatter, data, yaml_formatter::default_flags, R"(
bin: 10
date: '2021-01-02'
hex: 0xA
inf: .inf
nan: .NAN
oct: 10)"sv);

		CHECK_OUTPUT(yaml_formatter, data, format_flags::quote_infinities_and_nans, R"(
bin: 10
date: "2021-01-02"
hex: 10
inf: ".inf"
nan: ".NAN"
oct: 10)"sv);

		CHECK_OUTPUT(yaml_formatter, toml::value{ 42 }, yaml_formatter::default_flags, "42"sv);
		CHECK_OUTPUT(yaml_formatter, toml::value{ -f64_inf }, yaml_formatter::default_flags, "-.inf"sv);
	}

	SECTION("floats in exponent notation keep a decimal point for YAML 1.1")
	{
		const auto data =
			toml::table{ { "exponent", 1e16 },	{ "exponent_with_fraction", 1.5e300 },
						 { "fixed", 100000.0 }, { "in_array", toml::array{ 1e-5, 0.5 } },
						 { "neg_zero", -0.0 },	{ "subnormal", -std::numeric_limits<double>::denorm_min() } };

		CHECK_OUTPUT(yaml_formatter,
					 data,
					 yaml_formatter::default_flags,
					 "exponent: 1.0e+16\n"
					 "exponent_with_fraction: 1.5e+300\n"
					 "fixed: 100000.0\n"
					 "in_array: \n"
					 "  - 1.0e-05\n"
					 "  - 0.5\n"
					 "neg_zero: -0.0\n"
					 "subnormal: -5.0e-324"sv);

		CHECK_OUTPUT(yaml_formatter, toml::value{ 1e16 }, yaml_formatter::default_flags, "1.0e+16"sv);
		CHECK_OUTPUT(yaml_formatter,
					 toml::value{ 999999.5 },
					 yaml_formatter::default_flags | format_flags::relaxed_float_precision,
					 "1.0e+06"sv);
	}

	SECTION("containers")
	{
		const auto data = toml::table{
			{ "empty_array", toml::array{} },
			{ "empty_table", toml::table{} },
			{ "seq",
			  toml::array{ 1,
						   toml::array{ 2, toml::array{ 3 } },
						   toml::table{ { "a", 4 }, { "b", toml::table{ { "c", 5 } } } },
						   toml::array{},
						   toml::table{},
						   true } },
		};

		CHECK_OUTPUT(yaml_formatter,
					 data,
					 yaml_formatter::default_flags,
					 "empty_array: []\n"
					 "empty_table: {}\n"
					 "seq: \n"
					 "  - 1\n"
					 "  - - 2\n"
					 "    - - 3\n"
					 "  - a: 4\n"
					 "    b: \n"
					 "      c: 5\n"
					 "  - []\n"
					 "  - {}\n"
					 "  - true"sv);

		CHECK_OUTPUT(yaml_formatter, toml::array{ 1, toml::array{ 2 } }, yaml_formatter::default_flags, R"(
  - 1
  - - 2)"sv);
		CHECK_OUTPUT(yaml_formatter, toml::array{}, yaml_formatter::default_flags, "[]"sv);
		CHECK_OUTPUT(yaml_formatter, toml::table{}, yaml_formatter::default_flags, "{}"sv);
	}
}

#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

TEST_CASE("formatters - parse results")
{
	static constexpr auto error_message =
		"Error while parsing key-value pair: encountered end-of-file\n\t(error occurred at line 1, column 5)"sv;

	SECTION("toml_formatter")
	{
		CHECK(print_with<toml_formatter>(toml::parse("a = 'b'"sv)) == "a = 'b'"sv);
		CHECK(print_with<toml_formatter>(toml::parse("a = "sv)) == error_message);
	}

	SECTION("json_formatter")
	{
		CHECK(print_with<json_formatter>(toml::parse("a = 'b'"sv)) == "{\n    \"a\" : \"b\"\n}"sv);
		CHECK(print_with<json_formatter>(toml::parse("a = "sv)) == error_message);
	}

	SECTION("yaml_formatter")
	{
		CHECK(print_with<yaml_formatter>(toml::parse("a = 'b'"sv)) == "a: b"sv);
		CHECK(print_with<yaml_formatter>(toml::parse("a = "sv)) == error_message);
	}
}

#endif
