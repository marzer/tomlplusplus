#include "tests.h"

TEST_CASE("parsing - key-value pairs")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			key = "value"
			bare_key = "value"
			bare-key = "value"
			1234 = "value"
			"" = "blank"
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 5);
			CHECK(tbl[S("key")] == S("value"sv));
			CHECK(tbl[S("bare_key")] == S("value"sv));
			CHECK(tbl[S("bare-key")] == S("value"sv));
			CHECK(tbl[S("1234")] == S("value"sv));
			CHECK(tbl[S("")] == S("blank"sv));
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(key = # INVALID)"sv));

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			"127.0.0.1" = "value"
			"character encoding" = "value"
			"ʎǝʞ" = "value"
			'key2' = "value"
			'quoted "value"' = "value"
			'' = 'blank'
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("127.0.0.1")] == S("value"sv));
			CHECK(tbl[S("character encoding")] == S("value"sv));
			CHECK(tbl[S("ʎǝʞ")] == S("value"sv));
			CHECK(tbl[S("key2")] == S("value"sv));
			CHECK(tbl[S("quoted \"value\"")] == S("value"sv));
			CHECK(tbl[S("")] == S("blank"sv));
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(= "no key name")"sv));

	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		# DO NOT DO THIS
		name = "Tom"
		name = "Pradyun"
	)"sv));
}

TEST_CASE("parsing - key-value pairs (dotted)")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			name = "Orange"
			physical.color = "orange"
			physical.shape = "round"
			site."google.com" = true
			3.14159 = "pi"
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 4);
			CHECK(tbl[S("name")] == S("Orange"sv));
			CHECK(tbl[S("physical")][S("color")] == S("orange"sv));
			CHECK(tbl[S("physical")][S("shape")] == S("round"sv));
			CHECK(tbl[S("site")][S("google.com")] == true);
			CHECK(tbl[S("3")][S("14159")] == S("pi"sv));
		}
	);


	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			fruit.apple.smooth = true
			fruit.orange = 2
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("fruit")][S("apple")][S("smooth")] == true);
			CHECK(tbl[S("fruit")][S("orange")] == 2);
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		# THIS IS INVALID
		fruit.apple = 1
		fruit.apple.smooth = true
	)"sv));

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			# VALID BUT DISCOURAGED

			apple.type = "fruit"
			orange.type = "fruit"

			apple.skin = "thin"
			orange.skin = "thick"

			apple.color = "red"
			orange.color = "orange"
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			# RECOMMENDED

			apple.type = "fruit"
			apple.skin = "thin"
			apple.color = "red"

			orange.type = "fruit"
			orange.skin = "thick"
			orange.color = "orange"
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	// toml/issues/644 ('+' in bare keys) & toml/issues/687 (unicode bare keys)
	#if TOML_LANG_UNRELEASED
		parsing_should_succeed(
			FILE_LINE_ARGS,
			S(R"(
				key+1 = 0
				ʎǝʞ2 = 0
			)"sv),
			[](table&& tbl)
			{
				CHECK(tbl.size() == 2);
				CHECK(tbl[S("key+1")] == 0);
				CHECK(tbl[S("ʎǝʞ2")] == 0);
			}
		);
	#else
		parsing_should_fail(FILE_LINE_ARGS, R"(key+1 = 0)"sv);
		parsing_should_fail(FILE_LINE_ARGS, R"(ʎǝʞ2 = 0)"sv);
	#endif
}

TEST_CASE("parsing - key-value pairs (string keys)")
{
	// these are all derived from the discussion at 
	// https://github.com/toml-lang/toml/issues/733.
	
	// whitespace stripped, fail duplicate keys
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		a     = 2
		a = 3
	)"sv));

	// only surrounding whitespace is stripped, fail: illegal key name or syntax error
	parsing_should_fail(FILE_LINE_ARGS, S("a b = 3"sv));

	// whitespace is allowed when quoted, fail duplicate key
	parsing_should_succeed(FILE_LINE_ARGS, S("\"a b\" = 3"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S("'a b' = 3"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		"a b" = 3
		'a b' = 3
	)"sv));

	// whitespace is allowed when quoted, but not collapsed, success
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		"a b" = 3
		'a  b' = 3
	)"sv));

	// whitespace relevant, but fail: duplicate key
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		"a " = 2
		'a ' = 3
	)"sv));

	// whitespace relevant, and not collapsed, success
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		"a " = 2
		"a  " = 3
	)"sv));

	// whitespace can be escaped, success, different keys (whitespace escapes are not normalized)
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		"a\n" = 2
		"a\r" = 3
		"a\t" = 3
		"a\f" = 3
	)"sv));

	// valid keys composed of various string/non-string mixes types
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(a = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('a' = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a" = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(a.b = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('a'.b = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a".b = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(a.'b' = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('a'.'b' = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a".'b' = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(a."b" = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('a'."b" = 3)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a"."b" = 3)"sv));

	// multi-line strings can't be used in keys
	parsing_should_fail(FILE_LINE_ARGS, S(R"('''a''' = 3)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"("""a""" = 3)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(a.'''b''' = 3)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(a."""b""" = 3)"sv));

	// whitespace relevant (success test, values are NOTE equal)
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		a = " to do "
		b = "to do"
	)"sv), [](table&& tbl)
	{
		CHECK(tbl[S("a")] == S(" to do "sv));
		CHECK(tbl[S("b")] == S("to do"sv));
	});

	// values must be quoted, syntax error
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		a = to do
		b = todo
	)"sv));

	// different quotes, fail duplicate keys
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		a = 2
		'a' = 2
	)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		'a' = 2
		"a" = 2
	)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		'a' = 2
		"""a""" = 2
	)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		'''a''' = 2
		"""a""" = 2
	)"sv));

	// success test, capital not equal to small
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		a = 2
		A = 3
	)"sv));

	// inner quotes are not stripped from value, a & b are equal, value surrounded by quotes
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		a = "\"quoted\""
		b = """"quoted""""
	)"sv), [](table&& tbl)
	{
		CHECK(tbl[S("a")] == S("\"quoted\""sv));
		CHECK(tbl[S("b")] == S("\"quoted\""sv));
	});

	// quote correction is not applied, fail syntax error
	parsing_should_fail(FILE_LINE_ARGS, S(R"("a = "test")"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"('a = 'test')"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"("a = 'test")"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"('a = "test')"sv));

	// quotes cannot appear in keys this way, fail syntax error
	parsing_should_fail(FILE_LINE_ARGS, S(R"("a'b = 3)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"("a"b = 3)"sv));

	// escaped quotes and single quotes can appear this way, fail duplicate keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a'b" = 2)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("a\u0027b" = 4)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		"a'b" = 2
		"a\u0027b" = 4
	)"sv));

	// literal strings, escapes are not escaped, success, since keys are valid and not equal
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		'a"b' = 2
		'a\"b' = 4
	)"sv));

	// escapes must be compared after unescaping, fail duplicate key
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(a = 1)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("\u0061" = 2)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		a = 1
		"\u0061" = 2
	)"sv));

	// escaping requires quotes, syntax error
	parsing_should_fail(FILE_LINE_ARGS, S(R"(\u0061 = 2)"sv));

	// empty keys are allowed, but can only appear once, fail duplicate key
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("" = 2)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('' = 3)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		"" = 2
		'' = 3
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, fail duplicate key
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(1234 = 5)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("1234" = 5)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		1234 = 5
		"1234" = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, fail duplicate key
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(1234 = 5)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"('1234' = 5)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		1234 = 5
		'1234' = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, valid, different keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		1234 = 5
		01234 = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, valid, different keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		12e3 = 4
		12000 = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, valid, different keys, one dotted
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		1.2e3 = 4
		1200 = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, success, cause one is dotted
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(
		1.2e3 = 4
		"1.2e3" = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, fail duplicate keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(12e3 = 4)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("12e3" = 5)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		12e3 = 4
		"12e3" = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, fail duplicate dotted keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(1.2e3 = 4)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(1."2e3" = 5)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		1.2e3 = 4
		1."2e3" = 5
	)"sv));

	// bare keys can be numerals, but are interpreted as strings, fail duplicate dotted keys
	parsing_should_succeed(FILE_LINE_ARGS, S(R"(1.2e3 = 4)"sv));
	parsing_should_succeed(FILE_LINE_ARGS, S(R"("1".2e3 = 5)"sv));
	parsing_should_fail(FILE_LINE_ARGS, S(R"(
		1.2e3 = 4
		"1".2e3 = 5
	)"sv));
}
