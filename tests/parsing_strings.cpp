#include "tests.h"

TEST_CASE("parsing strings")
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
}
