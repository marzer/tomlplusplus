// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

TEST_CASE("parsing - strings")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
str = "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."

str1 = """
Roses are red
Violets are blue"""

str2 = """

Roses are red
Violets are blue"""
)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["str"]
									 == "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."sv);
							   CHECK(tbl["str1"] == "Roses are red\nViolets are blue"sv);
							   CHECK(tbl["str2"] == "\nRoses are red\nViolets are blue"sv);
						   });

	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
# The following strings are byte-for-byte equivalent:
str1 = "The quick brown fox jumps over the lazy dog."

str2 = """
The quick brown \


  fox jumps over \
    the lazy dog."""

str3 = """\
       The quick brown \
       fox jumps over \
       the lazy dog.\
       """

str4 = """Here are two quotation marks: "". Simple enough."""
# str5 = """Here are three quotation marks: """."""  # INVALID
str5 = """Here are three quotation marks: ""\"."""
str6 = """Here are fifteen quotation marks: ""\"""\"""\"""\"""\"."""

# "This," she said, "is just a pointless statement."
str7 = """"This," she said, "is just a pointless statement.""""
)"sv,
						   [](table&& tbl)
						   {
							   static constexpr auto quick_brown_fox = "The quick brown fox jumps over the lazy dog."sv;
							   CHECK(tbl["str1"] == quick_brown_fox);
							   CHECK(tbl["str2"] == quick_brown_fox);
							   CHECK(tbl["str3"] == quick_brown_fox);
							   CHECK(tbl["str4"] == R"(Here are two quotation marks: "". Simple enough.)"sv);
							   CHECK(tbl["str5"] == R"(Here are three quotation marks: """.)"sv);
							   CHECK(tbl["str6"] == R"(Here are fifteen quotation marks: """"""""""""""".)"sv);
							   CHECK(tbl["str7"] == R"("This," she said, "is just a pointless statement.")"sv);
						   });

	parsing_should_fail(FILE_LINE_ARGS,
						R"(str5 = """Here are three quotation marks: """.""")"sv,
						"1:46: Error while parsing key-value pair: expected a comment or line break, saw '.'"sv);

	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
# What you see is what you get.
winpath  = 'C:\Users\nodejs\templates'
winpath2 = '\\ServerX\admin$\system32\'
quoted   = 'Tom "Dubs" Preston-Werner'
regex    = '<\i\c*\s*>'
regex2 = '''I [dw]on't need \d{2} apples'''
lines  = '''
The first newline is
trimmed in raw strings.
   All other whitespace
   is preserved.
'''
lines2  = '''

The first newline is
trimmed in raw strings.
   All other whitespace
   is preserved.
'''
)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["winpath"] == R"(C:\Users\nodejs\templates)"sv);
							   CHECK(tbl["winpath2"] == R"(\\ServerX\admin$\system32\)"sv);
							   CHECK(tbl["quoted"] == R"(Tom "Dubs" Preston-Werner)"sv);
							   CHECK(tbl["regex"] == R"(<\i\c*\s*>)"sv);
							   CHECK(tbl["regex2"] == R"(I [dw]on't need \d{2} apples)"sv);
							   CHECK(tbl["lines"] == R"(The first newline is
trimmed in raw strings.
   All other whitespace
   is preserved.
)"sv);
							   CHECK(tbl["lines2"] == R"(
The first newline is
trimmed in raw strings.
   All other whitespace
   is preserved.
)"sv);
						   });

	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
quot15 = '''Here are fifteen quotation marks: """""""""""""""'''

# apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID
apos15 = "Here are fifteen apostrophes: '''''''''''''''"

# 'That's still pointless', she said.
str = ''''That's still pointless', she said.'''
)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["quot15"] == R"(Here are fifteen quotation marks: """"""""""""""")"sv);
							   CHECK(tbl["apos15"] == R"(Here are fifteen apostrophes: ''''''''''''''')"sv);
							   CHECK(tbl["str"] == R"('That's still pointless', she said.)"sv);
						   });

	parsing_should_fail(FILE_LINE_ARGS,
						R"(apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID)"sv,
						"1:48: Error while parsing key-value pair: expected a comment or line break, saw '''"sv);
}

TEST_CASE("parsing - strings (values)")
{
	parse_expected_value(FILE_LINE_ARGS,
						 R"("The quick brown fox jumps over the lazy dog")"sv,
						 "The quick brown fox jumps over the lazy dog"sv);
	parse_expected_value(FILE_LINE_ARGS,
						 R"('The quick brown fox jumps over the lazy dog')"sv,
						 "The quick brown fox jumps over the lazy dog"sv);
	parse_expected_value(FILE_LINE_ARGS,
						 R"("""The quick brown fox jumps over the lazy dog""")"sv,
						 "The quick brown fox jumps over the lazy dog"sv);
	parse_expected_value(FILE_LINE_ARGS,
						 R"('''The quick brown fox jumps over the lazy dog''')"sv,
						 "The quick brown fox jumps over the lazy dog"sv);

#if UNICODE_LITERALS_OK
	parse_expected_value(FILE_LINE_ARGS, R"("Ýôú'ℓℓ λáƭè ₥è áƒƭèř ƭλïƨ - #")"sv, R"(Ýôú'ℓℓ λáƭè ₥è áƒƭèř ƭλïƨ - #)"sv);
	parse_expected_value(FILE_LINE_ARGS,
						 R"(" Âñδ ωλèñ \"'ƨ ářè ïñ ƭλè ƨƭřïñϱ, áℓôñϱ ωïƭλ # \"")"sv,
						 R"( Âñδ ωλèñ "'ƨ ářè ïñ ƭλè ƨƭřïñϱ, áℓôñϱ ωïƭλ # ")"sv);
	parse_expected_value(FILE_LINE_ARGS,
						 R"("Ýôú δôñ'ƭ ƭλïñƙ ƨô₥è úƨèř ωôñ'ƭ δô ƭλáƭ?")"sv,
						 R"(Ýôú δôñ'ƭ ƭλïñƙ ƨô₥è úƨèř ωôñ'ƭ δô ƭλáƭ?)"sv);
#endif // UNICODE_LITERALS_OK

	parse_expected_value(FILE_LINE_ARGS, R"("\"\u03B1\u03B2\u03B3\"")"sv, "\"\u03B1\u03B2\u03B3\""sv);
}

TEST_CASE("parsing - strings (escape sequences)")
{
	// toml/pull/796 (\xHH unicode scalars)
	parse_expected_value(FILE_LINE_ARGS,
						 R"("\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\x11\xFF\xEE")"sv,
						 "\u0000\u0010\u0020\u0030\u0040\u0050\u0060\u0070\u0080\u0090\u0011\u00FF\u00EE"sv);

	// toml/pull/790
	parse_expected_value(FILE_LINE_ARGS, R"("\e[31mfoo\e[0m")"sv, "\x1B[31mfoo\x1B[0m"sv);

	// check 8-digit \U scalars with insufficient digits
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U1234567")"sv,
						R"(1:17: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U123456")"sv,
						R"(1:16: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U12345")"sv,
						R"(1:15: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U1234")"sv,
						R"(1:14: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U123")"sv,
						R"(1:13: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U12")"sv,
						R"(1:12: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U1")"sv,
						R"(1:11: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);

	// check 4-digit \u scalars with insufficient digits
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\u123")"sv,
						R"(1:13: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\u12")"sv,
						R"(1:12: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\u1")"sv,
						R"(1:11: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);

	// check 2-digit \x scalars with insufficient digits
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\x1")"sv,
						R"(1:11: Error while parsing unicode scalar sequence: expected hexadecimal digit, saw '"')"sv);
}

TEST_CASE("parsing - strings (quotes next to multi-line delimiters)")
{
	// ML string examples from https://github.com/toml-lang/toml/issues/725
	parse_expected_value(FILE_LINE_ARGS, R"( """ """          )"sv, R"( )"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """ """"         )"sv, R"( ")"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """ """""        )"sv, R"( "")"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(v= """ """"""       )"sv,
						R"(1:13: Error while parsing key-value pair: expected a comment or line break, saw '"')"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( ''' '''          )"sv, R"( )"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( ''' ''''         )"sv, R"( ')"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( ''' '''''        )"sv, R"( '')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(v= ''' ''''''       )"sv,
						"1:13: Error while parsing key-value pair: expected a comment or line break, saw '''"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """"""           )"sv, R"()"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """" """         )"sv, R"(" )"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """"" """        )"sv, R"("" )"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(v= """""" """       )"sv,
						R"(1:11: Error while parsing key-value pair: expected a comment or line break, saw '"')"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( ''''''           )"sv, R"()"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( '''' '''         )"sv, R"(' )"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( ''''' '''        )"sv, R"('' )"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(v= '''''' '''       )"sv,
						"1:11: Error while parsing key-value pair: expected a comment or line break, saw '''"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """""\""""""     )"sv, R"(""""")"sv);
	parse_expected_value(FILE_LINE_ARGS, R"( """""\"""\"""""" )"sv, R"("""""""")"sv);
}

TEST_CASE("parsing - strings (line-ending backslashes only trim ASCII whitespace)")
{
	for (const auto cp :
		 { "\xC2\xA0"sv, "\xE1\x9A\x80"sv, "\xE2\x80\x8B"sv, "\xE2\x80\xA8"sv, "\xE3\x80\x80"sv, "\xEF\xBB\xBF"sv })
	{
		const auto str		= std::string{ cp };
		const auto expected = "x" + str + " \ty";
		parse_expected_value(FILE_LINE_ARGS, "\"\"\"x\\\n" + str + " \ty\"\"\"", std::string_view{ expected });
		parse_expected_value(FILE_LINE_ARGS,
							 "\"\"\"x\\ \t\n \t\n\t " + str + " \ty\"\"\"",
							 std::string_view{ expected });
	}
}

TEST_CASE("parsing - strings (malformed)")
{
	parsing_should_fail(
		FILE_LINE_ARGS,
		R"(str = """foo \ x""")"sv,
		"1:16: Error while parsing string: line-ending backslashes must be the last non-whitespace character on the line"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		R"(str = "\uD800")"sv,
		"1:8: Error while parsing unicode scalar sequence: unicode surrogates (U+D800 - U+DFFF) are prohibited"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		R"(str = "\uDFFF")"sv,
		"1:8: Error while parsing unicode scalar sequence: unicode surrogates (U+D800 - U+DFFF) are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\U00110000")"sv,
						"1:8: Error while parsing unicode scalar sequence: values greater than U+10FFFF are invalid"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		R"(str = "\U0000D800")"sv,
		"1:8: Error while parsing unicode scalar sequence: unicode surrogates (U+D800 - U+DFFF) are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "\q")"sv,
						R"(1:9: Error while parsing string: unknown escape sequence '\q')"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = """\q""")"sv,
						R"(1:11: Error while parsing string: unknown escape sequence '\q')"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		R"(str = """\uD800""")"sv,
		"1:10: Error while parsing unicode scalar sequence: unicode surrogates (U+D800 - U+DFFF) are prohibited"sv);
	parsing_should_fail(FILE_LINE_ARGS, "str = \"\xED\xA0\x80\""sv, "1:8: Encountered invalid UTF-8 sequence"sv);
	parsing_should_fail(FILE_LINE_ARGS, "str = '\xED\xA0\x80'"sv, "1:8: Encountered invalid UTF-8 sequence"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = "abc)"sv,
						"1:11: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = 'abc)"sv,
						"1:11: Error while parsing literal string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = """abc)"sv,
						"1:13: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						R"(str = '''abc)"sv,
						"1:13: Error while parsing literal string: encountered end-of-file"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"str = 'a\x01'"sv,
		R"(1:9: Error while parsing literal string: control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"str = '''a\x01'''"sv,
		R"(1:11: Error while parsing literal string: control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"str = \"a\x01\""sv,
		R"(1:9: Error while parsing string: unescaped control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
}

TEST_CASE("parsing - strings (at the end of the document)")
{
	parsing_should_fail(FILE_LINE_ARGS, "a = '"sv, "1:6: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, R"(a = ")"sv, "1:6: Error while parsing string: encountered end-of-file"sv);
	parse_expected_value(FILE_LINE_ARGS, "''"sv, ""sv);
	parse_expected_value(FILE_LINE_ARGS, R"("")"sv, ""sv);
	parsing_should_fail(FILE_LINE_ARGS, "a = 'a"sv, "1:7: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, R"(a = "a)"sv, "1:7: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = '''"sv,
						"1:8: Error while parsing literal string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, R"(a = """)"sv, "1:8: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS, "a = '\n"sv, "1:7: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"a = \"\"\"x\\ \t"sv,
						"1:12: Error while parsing string: encountered end-of-file"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"a = 'a\n"sv,
		R"(1:7: Error while parsing literal string: control characters other than TAB (U+0009) are prohibited, saw '\n')"sv);
}

TEST_CASE("parsing - strings (bytes after long stretches)")
{
	std::string accented;
	for (size_t i = 0; i < 40u; i++)
		accented += "\xC3\xA9";

	SECTION("every ascii byte")
	{
		for (const auto& filler : { std::string(40u, 'x'), accented })
		{
			for (unsigned byte = 0; byte < 0x80u; byte++)
			{
				INFO("byte " << byte);
				const auto c	   = static_cast<char>(byte);
				const bool control = c != '\t' && (byte < 0x20u || byte == 0x7Fu);
				const auto value   = filler + c + "y";
				auto check		   = [&](table&& tbl) { CHECK(tbl["a"] == std::string_view{ value }); };

				const auto basic = "a = \"" + filler + c + "y\"";
				if (c == '"')
					parsing_should_fail(
						FILE_LINE_ARGS,
						basic,
						"1:47: Error while parsing key-value pair: expected a comment or line break, saw 'y'"sv);
				else if (c == '\\')
					parsing_should_fail(FILE_LINE_ARGS,
										basic,
										R"(1:47: Error while parsing string: unknown escape sequence '\y')"sv);
				else if (control)
					parsing_should_fail(FILE_LINE_ARGS,
										basic,
										"1:46: Error while parsing string: unescaped control characters other than TAB "
										"(U+0009) are prohibited, saw "
											+ quoted_control_character(c));
				else
					parsing_should_succeed(FILE_LINE_ARGS, basic, check);

				const auto literal = "a = '" + filler + c + "y'";
				if (c == '\'')
					parsing_should_fail(
						FILE_LINE_ARGS,
						literal,
						"1:47: Error while parsing key-value pair: expected a comment or line break, saw 'y'"sv);
				else if (control)
					parsing_should_fail(FILE_LINE_ARGS,
										literal,
										"1:46: Error while parsing literal string: control characters other than TAB "
										"(U+0009) are prohibited, saw "
											+ quoted_control_character(c));
				else
					parsing_should_succeed(FILE_LINE_ARGS, literal, check);

				const auto multi_basic = "a = \"\"\"" + filler + c + "y\"\"\"";
				if (c == '\\')
					parsing_should_fail(FILE_LINE_ARGS,
										multi_basic,
										R"(1:49: Error while parsing string: unknown escape sequence '\y')"sv);
				else if (c == '\r')
					parsing_should_fail(FILE_LINE_ARGS,
										multi_basic,
										R"(1:49: Error while parsing string: expected '\n' after '\r', saw 'y')"sv);
				else if (c == '\v' || c == '\f')
					parsing_should_fail(
						FILE_LINE_ARGS,
						multi_basic,
						R"(1:48: Error while parsing string: vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);
				else if (control && c != '\n')
					parsing_should_fail(FILE_LINE_ARGS,
										multi_basic,
										"1:48: Error while parsing string: unescaped control characters other than TAB "
										"(U+0009) are prohibited, saw "
											+ quoted_control_character(c));
				else
					parsing_should_succeed(FILE_LINE_ARGS, multi_basic, check);

				const auto multi_literal = "a = '''" + filler + c + "y'''";
				if (c == '\r')
					parsing_should_fail(
						FILE_LINE_ARGS,
						multi_literal,
						R"(1:49: Error while parsing literal string: expected '\n' after '\r', saw 'y')"sv);
				else if (c == '\v' || c == '\f')
					parsing_should_fail(
						FILE_LINE_ARGS,
						multi_literal,
						R"(1:48: Error while parsing literal string: vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);
				else if (control && c != '\n')
					parsing_should_fail(FILE_LINE_ARGS,
										multi_literal,
										"1:48: Error while parsing literal string: control characters other than TAB "
										"(U+0009) are prohibited, saw "
											+ quoted_control_character(c));
				else
					parsing_should_succeed(FILE_LINE_ARGS, multi_literal, check);
			}
		}
	}

	SECTION("multi-byte code points before an error")
	{
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = \"\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\x01\""sv,
			R"(1:12: Error while parsing string: unescaped control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = '\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\x01'"sv,
			R"(1:12: Error while parsing literal string: control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = \"\"\"\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\x01\"\"\""sv,
			R"(1:11: Error while parsing string: unescaped control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = '''\xC3\xA9\xE2\x82\xAC\xF0\x9F\x98\x82\x01'''"sv,
			R"(1:11: Error while parsing literal string: control characters other than TAB (U+0009) are prohibited, saw '\u0001')"sv);
	}
}

TEST_CASE("parsing - strings (bytes within a 16-byte block)")
{
	struct kind
	{
		std::string_view open;
		std::string_view close;
		std::string_view scope;
	};

	for (const auto& k : { kind{ "\"", "\"", "string" },
						   kind{ "'", "'", "literal string" },
						   kind{ "\"\"\"", "\"\"\"", "string" },
						   kind{ "'''", "'''", "literal string" } })
	{
		const bool basic	  = k.close[0] == '"';
		const bool multi_line = k.close.length() == 3u;
		const auto error_at	  = [](size_t column, std::string_view scope)
		{ return "1:" + std::to_string(column) + ": Error while parsing " + std::string{ scope } + ": "; };

		const auto check = [&](const std::string& filler, size_t columns, char c, std::string_view tail)
		{
			INFO(k.open << ", byte " << static_cast<unsigned>(c) << " after " << filler.length() << " bytes");
			const auto column = 5u + k.open.length() + columns;
			const auto doc = "a = " + std::string{ k.open } + filler + c + std::string{ tail } + std::string{ k.close };
			const auto value   = std::string{ k.open.substr(multi_line ? 3u : 1u) } + filler + c + std::string{ tail };
			const bool control = c != '\t' && (static_cast<unsigned char>(c) < 0x20u || c == '\x7F');

			if (!multi_line && c == k.close[0])
				parsing_should_fail(FILE_LINE_ARGS,
									doc,
									error_at(column + 1u, "key-value pair")
										+ "expected a comment or line break, saw 'y'");
			else if (basic && c == '\\')
				parsing_should_fail(FILE_LINE_ARGS,
									doc,
									error_at(column + 1u, k.scope) + R"(unknown escape sequence '\y')");
			else if (multi_line && c == '\r')
				parsing_should_fail(FILE_LINE_ARGS,
									doc,
									error_at(column + 1u, k.scope) + R"(expected '\n' after '\r', saw 'y')");
			else if (multi_line && (c == '\v' || c == '\f'))
				parsing_should_fail(
					FILE_LINE_ARGS,
					doc,
					error_at(column, k.scope)
						+ R"(vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)");
			else if (control && !(multi_line && c == '\n'))
				parsing_should_fail(FILE_LINE_ARGS,
									doc,
									error_at(column, k.scope) + (basic ? "unescaped " : "")
										+ "control characters other than TAB (U+0009) are prohibited, saw "
										+ quoted_control_character(c));
			else
				parsing_should_succeed(FILE_LINE_ARGS,
									   doc,
									   [&](table&& tbl) { CHECK(tbl["a"] == std::string_view{ value }); });
		};

		static constexpr auto ys	   = "yyyyyyyyyyyyyyyy"sv;
		static constexpr auto accented = "\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9"sv;
		for (unsigned byte = 0; byte < 0x80u; byte++)
		{
			check(std::string(5u, 'x'), 5u, static_cast<char>(byte), ys);
			check("\xC3\xA9" + std::string(3u, 'x'), 4u, static_cast<char>(byte), ys);
		}

		for (size_t length = 0; length < 18u; length++)
		{
			for (const auto& [c, tail] : { std::pair{ '\x01', accented },
										   std::pair{ '\x7F', accented },
										   std::pair{ '\\', ys },
										   std::pair{ k.close[0], ys } })
			{
				check(std::string(length, 'x'), length, c, tail);
				if (length >= 2u)
					check("\xC3\xA9" + std::string(length - 2u, 'x'), length - 1u, c, tail);
			}
		}
	}
}
