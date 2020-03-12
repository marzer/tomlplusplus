#include "tests.h"

TEST_CASE("parsing - strings")
{
	parsing_should_succeed(S(R"(
str = "I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."

str1 = """
Roses are red
Violets are blue"""
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("str")] == S("I'm a string. \"You can quote me\". Name\tJos\u00E9\nLocation\tSF."sv));
			CHECK(tbl[S("str1")] == S("Roses are red\nViolets are blue"sv));
		}
	);

	parsing_should_succeed(S(R"(
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
)"sv),
		[](table&& tbl) noexcept
		{
			static constexpr auto quick_brown_fox = S("The quick brown fox jumps over the lazy dog."sv);
			CHECK(tbl[S("str1")] == quick_brown_fox);
			CHECK(tbl[S("str2")] == quick_brown_fox);
			CHECK(tbl[S("str3")] == quick_brown_fox);
			CHECK(tbl[S("str4")] == S(R"(Here are two quotation marks: "". Simple enough.)"sv));
			CHECK(tbl[S("str5")] == S(R"(Here are three quotation marks: """.)"sv));
			CHECK(tbl[S("str6")] == S(R"(Here are fifteen quotation marks: """"""""""""""".)"sv));
			CHECK(tbl[S("str7")] == S(R"("This," she said, "is just a pointless statement.")"sv));
		}
	);

	parsing_should_fail(S(R"(str5 = """Here are three quotation marks: """.""")"sv));

	parsing_should_succeed(S(R"(
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
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("winpath")] == S(R"(C:\Users\nodejs\templates)"sv));
			CHECK(tbl[S("winpath2")] == S(R"(\\ServerX\admin$\system32\)"sv));
			CHECK(tbl[S("quoted")] == S(R"(Tom "Dubs" Preston-Werner)"sv));
			CHECK(tbl[S("regex")] == S(R"(<\i\c*\s*>)"sv));
			CHECK(tbl[S("regex2")] == S(R"(I [dw]on't need \d{2} apples)"sv));
			CHECK(tbl[S("lines")] == S(R"(The first newline is
trimmed in raw strings.
   All other whitespace
   is preserved.
)"sv));
		}
	);

	parsing_should_succeed(S(R"(
quot15 = '''Here are fifteen quotation marks: """""""""""""""'''

# apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID
apos15 = "Here are fifteen apostrophes: '''''''''''''''"

# 'That's still pointless', she said.
str = ''''That's still pointless', she said.'''
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("quot15")] == S(R"(Here are fifteen quotation marks: """"""""""""""")"sv));
			CHECK(tbl[S("apos15")] == S(R"(Here are fifteen apostrophes: ''''''''''''''')"sv));
			CHECK(tbl[S("str")] == S(R"('That's still pointless', she said.)"sv));
		}
	);

	parsing_should_fail(S(R"(apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID)"sv));

	//value tests
	parse_expected_value(
		R"("The quick brown fox jumps over the lazy dog")"sv,
		 S("The quick brown fox jumps over the lazy dog"sv));
	parse_expected_value(
		R"('The quick brown fox jumps over the lazy dog')"sv,
		 S("The quick brown fox jumps over the lazy dog"sv));
	parse_expected_value(
	  R"("""The quick brown fox jumps over the lazy dog""")"sv,
		 S("The quick brown fox jumps over the lazy dog"sv));
	parse_expected_value(
	  R"('''The quick brown fox jumps over the lazy dog''')"sv,
		 S("The quick brown fox jumps over the lazy dog"sv));
	parse_expected_value(
		R"("Ýôú'ℓℓ λáƭè ₥è áƒƭèř ƭλïƨ - #")"sv,
	   S(R"(Ýôú'ℓℓ λáƭè ₥è áƒƭèř ƭλïƨ - #)"sv));
	parse_expected_value(
		R"(" Âñδ ωλèñ \"'ƨ ářè ïñ ƭλè ƨƭřïñϱ, áℓôñϱ ωïƭλ # \"")"sv,
	   S(R"( Âñδ ωλèñ "'ƨ ářè ïñ ƭλè ƨƭřïñϱ, áℓôñϱ ωïƭλ # ")"sv));
	parse_expected_value(
		R"("Ýôú δôñ'ƭ ƭλïñƙ ƨô₥è úƨèř ωôñ'ƭ δô ƭλáƭ?")"sv,
	   S(R"(Ýôú δôñ'ƭ ƭλïñƙ ƨô₥è úƨèř ωôñ'ƭ δô ƭλáƭ?)"sv));
	parse_expected_value(
		R"("\"\u03B1\u03B2\u03B3\"")"sv,
	     S("\"\u03B1\u03B2\u03B3\""sv));

	// toml/issues/622 - escaping alias for spaces
	#if 0 && TOML_LANG_HIGHER_THAN(0, 5, 0)
	parse_expected_value(
		R"("The\squick\sbrown\sfox\sjumps\sover\sthe\slazy\sdog")"sv,
		 S("The quick brown fox jumps over the lazy dog"sv));
	#else
	parsing_should_fail(R"(str = "The\squick\sbrown\sfox\sjumps\sover\sthe\slazy\sdog")"sv);
	#endif

	// toml/pull/709 - \xHH short-form unicode scalars
	#if TOML_LANG_HIGHER_THAN(0, 5, 0)
	parse_expected_value(
		R"("\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\x11\xFF\xEE")"sv,
		 S("\u0000\u0010\u0020\u0030\u0040\u0050\u0060\u0070\u0080\u0090\u0011\u00FF\u00EE"sv));
	#else
	parsing_should_fail(R"(str = "\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90\x11\xFF\xEE")"sv);
	#endif

	//check 8-digit \U scalars with insufficient digits
	parsing_should_fail(R"(str = "\U1234567")"sv);
	parsing_should_fail(R"(str = "\U123456")"sv);
	parsing_should_fail(R"(str = "\U12345")"sv);
	parsing_should_fail(R"(str = "\U1234")"sv);
	parsing_should_fail(R"(str = "\U123")"sv);
	parsing_should_fail(R"(str = "\U12")"sv);
	parsing_should_fail(R"(str = "\U1")"sv);

	//check 4-digit \u scalars with insufficient digits
	parsing_should_fail(R"(str = "\u123")"sv);
	parsing_should_fail(R"(str = "\u12")"sv);
	parsing_should_fail(R"(str = "\u1")"sv);

	//check 2-digit \x scalars with insufficient digits
	parsing_should_fail(R"(str = "\x1")"sv);
}
