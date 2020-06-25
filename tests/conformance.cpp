#include "tests.h"
using namespace toml::impl;

#if !TOML_UNRELEASED_FEATURES // todo: improve conformance script to remove this

namespace // invalid test data for BurntSushi/toml-test
{
	 static constexpr auto datetime_malformed_no_leads = S(R"(no-leads = 1987-7-05T17:45:00Z)"sv);
	 static constexpr auto datetime_malformed_no_secs = S(R"(no-secs = 1987-07-05T17:45Z)"sv);
	 static constexpr auto datetime_malformed_no_t = S(R"(no-t = 1987-07-0517:45:00Z)"sv);
	 static constexpr auto datetime_malformed_with_milli = S(R"(with-milli = 1987-07-5T17:45:00.12Z)"sv);
	 static constexpr auto duplicate_key_table = S(R"([fruit]
type = "apple"

[fruit.type]
apple = "yes")"sv);
	 static constexpr auto duplicate_keys = S(R"(dupe = false
dupe = true)"sv);
	 static constexpr auto duplicate_tables = S(R"([a]
[a])"sv);
	 static constexpr auto empty_implicit_table = S(R"([naughty..naughty])"sv);
	 static constexpr auto empty_table = S(R"([])"sv);
	 static constexpr auto float_leading_zero_neg = S(R"(leading-zero = -03.14)"sv);
	 static constexpr auto float_leading_zero_pos = S(R"(leading-zero = +03.14)"sv);
	 static constexpr auto float_leading_zero = S(R"(leading-zero = 03.14)"sv);
	 static constexpr auto float_no_leading_zero = S(R"(answer = .12345
neganswer = -.12345)"sv);
	 static constexpr auto float_no_trailing_digits = S(R"(answer = 1.
neganswer = -1.)"sv);
	 static constexpr auto float_underscore_after_point = S(R"(bad = 1._2)"sv);
	 static constexpr auto float_underscore_after = S(R"(bad = 1.2_)"sv);
	 static constexpr auto float_underscore_before_point = S(R"(bad = 1_.2)"sv);
	 static constexpr auto float_underscore_before = S(R"(bad = _1.2)"sv);
	 static constexpr auto inline_table_linebreak = S(R"(simple = { a = 1 
})"sv);
	 static constexpr auto integer_leading_zero_neg = S(R"(leading-zero = -012)"sv);
	 static constexpr auto integer_leading_zero_pos = S(R"(leading-zero = +012)"sv);
	 static constexpr auto integer_leading_zero = S(R"(leading-zero = 012)"sv);
	 static constexpr auto integer_underscore_after = S(R"(bad = 123_)"sv);
	 static constexpr auto integer_underscore_before = S(R"(bad = _123)"sv);
	 static constexpr auto integer_underscore_double = S(R"(bad = 1__23)"sv);
	 static constexpr auto key_after_array = S(R"([[agencies]] owner = "S Cjelli")"sv);
	 static constexpr auto key_after_table = S(R"([error] this = "should not be here")"sv);
	 static constexpr auto key_empty = S(R"(= 1)"sv);
	 static constexpr auto key_hash = S(R"(a# = 1)"sv);
	 static constexpr auto key_newline = S(R"(a
= 1)"sv);
	 static constexpr auto key_no_eol = S(R"(a = 1 b = 2)"sv);
	 static constexpr auto key_open_bracket = S(R"([abc = 1)"sv);
	 static constexpr auto key_single_open_bracket = S(R"([)"sv);
	 static constexpr auto key_space = S(R"(a b = 1)"sv);
	 static constexpr auto key_start_bracket = S(R"([a]
[xyz = 5
[b])"sv);
	 static constexpr auto key_two_equals = S(R"(key= = 1)"sv);
	 static constexpr auto llbrace = S(R"([ [table]])"sv);
	 static constexpr auto multi_line_inline_table = S(R"(json_like = {
          first = "Tom",
          last = "Preston-Werner"
})"sv);
	 static constexpr auto multi_line_string_no_close = S(R"(invalid = """
    this will fail)"sv);
	 static constexpr auto rrbrace = S(R"([[table] ])"sv);
	 static constexpr auto string_bad_byte_escape = S(R"(naughty = "\xAg")"sv);
	 static constexpr auto string_bad_codepoint = S(R"(invalid-codepoint = "This string contains a non scalar unicode codepoint \uD801")"sv);
	 static constexpr auto string_bad_escape = S(R"(invalid-escape = "This string has a bad \a escape character.")"sv);
	 static constexpr auto string_bad_slash_escape = S(R"(invalid-escape = "This string has a bad \/ escape character.")"sv);
	 static constexpr auto string_bad_uni_esc = S(R"(str = "val\ue")"sv);
	 static constexpr auto string_byte_escapes = S(R"(answer = "\x33")"sv);
	 static constexpr auto string_no_close = S(R"(no-ending-quote = "One time, at band camp)"sv);
	 static constexpr auto table_array_implicit = S(R"(# This test is a bit tricky. It should fail because the first use of
# `[[albums.songs]]` without first declaring `albums` implies that `albums`
# must be a table. The alternative would be quite weird. Namely, it wouldn't
# comply with the TOML spec: "Each double-bracketed sub-table will belong to 
# the most *recently* defined table element *above* it."
#
# This is in contrast to the *valid* test, table-array-implicit where
# `[[albums.songs]]` works by itself, so long as `[[albums]]` isn't declared
# later. (Although, `[albums]` could be.)
[[albums.songs]]
name = "Glory Days"

[[albums]]
name = "Born in the USA")"sv);
	 static constexpr auto table_array_malformed_bracket = S(R"([[albums]
name = "Born to Run")"sv);
	 static constexpr auto table_array_malformed_empty = S(R"([[]]
name = "Born to Run")"sv);
	 static constexpr auto table_empty = S(R"([])"sv);
	 static constexpr auto table_nested_brackets_close = S(R"([a]b]
zyx = 42)"sv);
	 static constexpr auto table_nested_brackets_open = S(R"([a[b]
zyx = 42)"sv);
	 static constexpr auto table_whitespace = S(R"([invalid key])"sv);
	 static constexpr auto table_with_pound = S(R"([key#group]
answer = 42)"sv);
	 static constexpr auto text_after_array_entries = S(R"(array = [
  "Is there life after an array separator?", No
  "Entry"
])"sv);
	 static constexpr auto text_after_integer = S(R"(answer = 42 the ultimate answer?)"sv);
	 static constexpr auto text_after_string = S(R"(string = "Is there life after strings?" No.)"sv);
	 static constexpr auto text_after_table = S(R"([error] this shouldn't be here)"sv);
	 static constexpr auto text_before_array_separator = S(R"(array = [
  "Is there life before an array separator?" No,
  "Entry"
])"sv);
	 static constexpr auto text_in_array = S(R"(array = [
  "Entry 1",
  I don't belong,
  "Entry 2",
])"sv);
}

TEST_CASE("conformance - invalid inputs from BurntSushi/toml-test")
{
	parsing_should_fail(FILE_LINE_ARGS, datetime_malformed_no_leads);
	parsing_should_fail(FILE_LINE_ARGS, datetime_malformed_no_secs);
	parsing_should_fail(FILE_LINE_ARGS, datetime_malformed_no_t);
	parsing_should_fail(FILE_LINE_ARGS, datetime_malformed_with_milli);
	parsing_should_fail(FILE_LINE_ARGS, duplicate_key_table);
	parsing_should_fail(FILE_LINE_ARGS, duplicate_keys);
	parsing_should_fail(FILE_LINE_ARGS, duplicate_tables);
	parsing_should_fail(FILE_LINE_ARGS, empty_implicit_table);
	parsing_should_fail(FILE_LINE_ARGS, empty_table);
	parsing_should_fail(FILE_LINE_ARGS, float_leading_zero_neg);
	parsing_should_fail(FILE_LINE_ARGS, float_leading_zero_pos);
	parsing_should_fail(FILE_LINE_ARGS, float_leading_zero);
	parsing_should_fail(FILE_LINE_ARGS, float_no_leading_zero);
	parsing_should_fail(FILE_LINE_ARGS, float_no_trailing_digits);
	parsing_should_fail(FILE_LINE_ARGS, float_underscore_after_point);
	parsing_should_fail(FILE_LINE_ARGS, float_underscore_after);
	parsing_should_fail(FILE_LINE_ARGS, float_underscore_before_point);
	parsing_should_fail(FILE_LINE_ARGS, float_underscore_before);
	parsing_should_fail(FILE_LINE_ARGS, inline_table_linebreak);
	parsing_should_fail(FILE_LINE_ARGS, integer_leading_zero_neg);
	parsing_should_fail(FILE_LINE_ARGS, integer_leading_zero_pos);
	parsing_should_fail(FILE_LINE_ARGS, integer_leading_zero);
	parsing_should_fail(FILE_LINE_ARGS, integer_underscore_after);
	parsing_should_fail(FILE_LINE_ARGS, integer_underscore_before);
	parsing_should_fail(FILE_LINE_ARGS, integer_underscore_double);
	parsing_should_fail(FILE_LINE_ARGS, key_after_array);
	parsing_should_fail(FILE_LINE_ARGS, key_after_table);
	parsing_should_fail(FILE_LINE_ARGS, key_empty);
	parsing_should_fail(FILE_LINE_ARGS, key_hash);
	parsing_should_fail(FILE_LINE_ARGS, key_newline);
	parsing_should_fail(FILE_LINE_ARGS, key_no_eol);
	parsing_should_fail(FILE_LINE_ARGS, key_open_bracket);
	parsing_should_fail(FILE_LINE_ARGS, key_single_open_bracket);
	parsing_should_fail(FILE_LINE_ARGS, key_space);
	parsing_should_fail(FILE_LINE_ARGS, key_start_bracket);
	parsing_should_fail(FILE_LINE_ARGS, key_two_equals);
	parsing_should_fail(FILE_LINE_ARGS, llbrace);
	parsing_should_fail(FILE_LINE_ARGS, multi_line_inline_table);
	parsing_should_fail(FILE_LINE_ARGS, multi_line_string_no_close);
	parsing_should_fail(FILE_LINE_ARGS, rrbrace);
	parsing_should_fail(FILE_LINE_ARGS, string_bad_byte_escape);
	parsing_should_fail(FILE_LINE_ARGS, string_bad_codepoint);
	parsing_should_fail(FILE_LINE_ARGS, string_bad_escape);
	parsing_should_fail(FILE_LINE_ARGS, string_bad_slash_escape);
	parsing_should_fail(FILE_LINE_ARGS, string_bad_uni_esc);
	parsing_should_fail(FILE_LINE_ARGS, string_byte_escapes);
	parsing_should_fail(FILE_LINE_ARGS, string_no_close);
	parsing_should_fail(FILE_LINE_ARGS, table_array_implicit);
	parsing_should_fail(FILE_LINE_ARGS, table_array_malformed_bracket);
	parsing_should_fail(FILE_LINE_ARGS, table_array_malformed_empty);
	parsing_should_fail(FILE_LINE_ARGS, table_empty);
	parsing_should_fail(FILE_LINE_ARGS, table_nested_brackets_close);
	parsing_should_fail(FILE_LINE_ARGS, table_nested_brackets_open);
	parsing_should_fail(FILE_LINE_ARGS, table_whitespace);
	parsing_should_fail(FILE_LINE_ARGS, table_with_pound);
	parsing_should_fail(FILE_LINE_ARGS, text_after_array_entries);
	parsing_should_fail(FILE_LINE_ARGS, text_after_integer);
	parsing_should_fail(FILE_LINE_ARGS, text_after_string);
	parsing_should_fail(FILE_LINE_ARGS, text_after_table);
	parsing_should_fail(FILE_LINE_ARGS, text_before_array_separator);
	parsing_should_fail(FILE_LINE_ARGS, text_in_array);
}

namespace // invalid test data for iarna/toml-spec-tests
{
	 static constexpr auto array_of_tables_1 = S(R"(# INVALID TOML DOC
fruit = []

[[fruit]] # Not allowed)"sv);
	 static constexpr auto array_of_tables_2 = S(R"(# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [[fruit.variety]]
    name = "red delicious"

  # This table conflicts with the previous table
  [fruit.variety]
    name = "granny smith")"sv);
	 static constexpr auto bare_key_1 = S(R"(bare!key = 123)"sv);
	 static constexpr auto bare_key_2 = S(R"(barekey
   = 123)"sv);
	 static constexpr auto bare_key_3 = S(R"(barekey =)"sv);
	 static constexpr auto inline_table_imutable_1 = S(R"([product]
type = { name = "Nail" }
type.edible = false  # INVALID)"sv);
	 static constexpr auto inline_table_imutable_2 = S(R"([product]
type.name = "Nail"
type = { edible = false }  # INVALID)"sv);
	 static constexpr auto inline_table_trailing_comma = S(R"(abc = { abc = 123, })"sv);
	 static constexpr auto int_0_padded = S(R"(int = 0123)"sv);
	 static constexpr auto int_signed_bin = S(R"(bin = +0b10)"sv);
	 static constexpr auto int_signed_hex = S(R"(hex = +0xab)"sv);
	 static constexpr auto int_signed_oct = S(R"(oct = +0o23)"sv);
	 static constexpr auto key_value_pair_1 = S(R"(key = # INVALID)"sv);
	 static constexpr auto key_value_pair_2 = S(R"(first = "Tom" last = "Preston-Werner" # INVALID)"sv);
	 static constexpr auto multiple_dot_key = S(R"(# THE FOLLOWING IS INVALID

# This defines the value of fruit.apple to be an integer.
fruit.apple = 1

# But then this treats fruit.apple like it's a table.
# You can't turn an integer into a table.
fruit.apple.smooth = true)"sv);
	 static constexpr auto multiple_key = S(R"(# DO NOT DO THIS
name = "Tom"
name = "Pradyun")"sv);
	 static constexpr auto no_key_name = S(R"(= "no key name"  # INVALID)"sv);
	 static constexpr auto string_basic_multiline_invalid_backslash = S(R"(a = """
  foo \ \n
  bar""")"sv);
	 static constexpr auto string_basic_multiline_out_of_range_unicode_escape_1 = S(R"(a = """\UFFFFFFFF""")"sv);
	 static constexpr auto string_basic_multiline_out_of_range_unicode_escape_2 = S(R"(a = """\U00D80000""")"sv);
	 static constexpr auto string_basic_multiline_quotes = S(R"(str5 = """Here are three quotation marks: """.""")"sv);
	 static constexpr auto string_basic_multiline_unknown_escape = S(R"(a = """\@""")"sv);
	 static constexpr auto string_basic_out_of_range_unicode_escape_1 = S(R"(a = "\UFFFFFFFF")"sv);
	 static constexpr auto string_basic_out_of_range_unicode_escape_2 = S(R"(a = "\U00D80000")"sv);
	 static constexpr auto string_basic_unknown_escape = S(R"(a = "\@")"sv);
	 static constexpr auto string_literal_multiline_quotes = S(R"(apos15 = '''Here are fifteen apostrophes: ''''''''''''''''''  # INVALID)"sv);
	 static constexpr auto table_1 = S(R"(# DO NOT DO THIS

[fruit]
apple = "red"

[fruit]
orange = "orange")"sv);
	 static constexpr auto table_2 = S(R"(# DO NOT DO THIS EITHER

[fruit]
apple = "red"

[fruit.apple]
texture = "smooth")"sv);
	 static constexpr auto table_3 = S(R"([fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple]  # INVALID)"sv);
	 static constexpr auto table_4 = S(R"([fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple.taste]  # INVALID)"sv);
	 static constexpr auto table_invalid_1 = S(R"([fruit.physical]  # subtable, but to which parent element should it belong?
  color = "red"
  shape = "round"

[[fruit]]  # parser must throw an error upon discovering that "fruit" is
           # an array rather than a table
  name = "apple")"sv);
	 static constexpr auto table_invalid_2 = S(R"(# INVALID TOML DOC
fruit = []

[[fruit]] # Not allowed)"sv);
	 static constexpr auto table_invalid_3 = S(R"(# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [[fruit.variety]]
    name = "red delicious"

  # INVALID: This table conflicts with the previous array of tables
  [fruit.variety]
    name = "granny smith"

  [fruit.physical]
    color = "red"
    shape = "round")"sv);
	 static constexpr auto table_invalid_4 = S(R"(# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [[fruit.variety]]
    name = "red delicious"

  [fruit.physical]
    color = "red"
    shape = "round"

  # INVALID: This array of tables conflicts with the previous table
  [[fruit.physical]]
    color = "green")"sv);
}

TEST_CASE("conformance - invalid inputs from iarna/toml-spec-tests")
{
	parsing_should_fail(FILE_LINE_ARGS, array_of_tables_1);
	parsing_should_fail(FILE_LINE_ARGS, array_of_tables_2);
	parsing_should_fail(FILE_LINE_ARGS, bare_key_1);
	parsing_should_fail(FILE_LINE_ARGS, bare_key_2);
	parsing_should_fail(FILE_LINE_ARGS, bare_key_3);
	parsing_should_fail(FILE_LINE_ARGS, inline_table_imutable_1);
	parsing_should_fail(FILE_LINE_ARGS, inline_table_imutable_2);
	parsing_should_fail(FILE_LINE_ARGS, inline_table_trailing_comma);
	parsing_should_fail(FILE_LINE_ARGS, int_0_padded);
	parsing_should_fail(FILE_LINE_ARGS, int_signed_bin);
	parsing_should_fail(FILE_LINE_ARGS, int_signed_hex);
	parsing_should_fail(FILE_LINE_ARGS, int_signed_oct);
	parsing_should_fail(FILE_LINE_ARGS, key_value_pair_1);
	parsing_should_fail(FILE_LINE_ARGS, key_value_pair_2);
	parsing_should_fail(FILE_LINE_ARGS, multiple_dot_key);
	parsing_should_fail(FILE_LINE_ARGS, multiple_key);
	parsing_should_fail(FILE_LINE_ARGS, no_key_name);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_multiline_invalid_backslash);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_multiline_out_of_range_unicode_escape_1);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_multiline_out_of_range_unicode_escape_2);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_multiline_quotes);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_multiline_unknown_escape);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_out_of_range_unicode_escape_1);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_out_of_range_unicode_escape_2);
	parsing_should_fail(FILE_LINE_ARGS, string_basic_unknown_escape);
	parsing_should_fail(FILE_LINE_ARGS, string_literal_multiline_quotes);
	parsing_should_fail(FILE_LINE_ARGS, table_1);
	parsing_should_fail(FILE_LINE_ARGS, table_2);
	parsing_should_fail(FILE_LINE_ARGS, table_3);
	parsing_should_fail(FILE_LINE_ARGS, table_4);
	parsing_should_fail(FILE_LINE_ARGS, table_invalid_1);
	parsing_should_fail(FILE_LINE_ARGS, table_invalid_2);
	parsing_should_fail(FILE_LINE_ARGS, table_invalid_3);
	parsing_should_fail(FILE_LINE_ARGS, table_invalid_4);
}


#endif // !TOML_UNRELEASED_FEATURES
