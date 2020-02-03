#include "tests.h"

TEST_CASE("parsing - key-value pairs")
{
	parsing_should_succeed(S(R"(
key = "value"
bare_key = "value"
bare-key = "value"
1234 = "value"
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl.size() == 4);
			CHECK(tbl[S("key")] == S("value"sv));
			CHECK(tbl[S("bare_key")] == S("value"sv));
			CHECK(tbl[S("bare-key")] == S("value"sv));
			CHECK(tbl[S("1234")] == S("value"sv));
		}
	);

	parsing_should_fail(S(R"(key = # INVALID)"sv));

	parsing_should_succeed(S(R"(
"127.0.0.1" = "value"
"character encoding" = "value"
"ʎǝʞ" = "value"
'key2' = "value"
'quoted "value"' = "value"
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("127.0.0.1")] == S("value"sv));
			CHECK(tbl[S("character encoding")] == S("value"sv));
			CHECK(tbl[S("ʎǝʞ")] == S("value"sv));
			CHECK(tbl[S("key2")] == S("value"sv));
			CHECK(tbl[S("quoted \"value\"")] == S("value"sv));
		}
	);

	parsing_should_fail(S(R"(= "no key name")"sv));

	parsing_should_fail(S(R"(
# DO NOT DO THIS
name = "Tom"
name = "Pradyun"
)"sv)
	);

}

TEST_CASE("parsing - key-value pairs (dotted)")
{
	parsing_should_succeed(S(R"(
name = "Orange"
physical.color = "orange"
physical.shape = "round"
site."google.com" = true
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl.size() == 3);
			CHECK(tbl[S("name")] == S("Orange"sv));
			CHECK(tbl[S("physical")][S("color")] == S("orange"sv));
			CHECK(tbl[S("physical")][S("shape")] == S("round"sv));
			CHECK(tbl[S("site")][S("google.com")] == true);
		}
	);


	parsing_should_succeed(S(R"(
fruit.apple.smooth = true
fruit.orange = 2
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("fruit")][S("apple")][S("smooth")] == true);
			CHECK(tbl[S("fruit")][S("orange")] == 2);
		}
	);

	parsing_should_fail(S(R"(
# THIS IS INVALID
fruit.apple = 1
fruit.apple.smooth = true
)"sv)
	);

	parsing_should_succeed(S(R"(
# VALID BUT DISCOURAGED

apple.type = "fruit"
orange.type = "fruit"

apple.skin = "thin"
orange.skin = "thick"

apple.color = "red"
orange.color = "orange"
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	parsing_should_succeed(S(R"(
# RECOMMENDED

apple.type = "fruit"
apple.skin = "thin"
apple.color = "red"

orange.type = "fruit"
orange.skin = "thick"
orange.color = "orange"
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	// allow + in bare keys - toml/issues/644
	// allow unicode in bare keys - toml/issues/687
	#if TOML_LANG_HIGHER_THAN(0, 5, 0)
	parsing_should_succeed(S(R"(
key+1 = 0
ʎǝʞ2 = 0
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl.size() == 2);
			CHECK(tbl[S("key+1")] == 0);
			CHECK(tbl[S("ʎǝʞ2")] == 0);
		}
	);
	#else
	parsing_should_fail(R"(key+1 = 0)"sv);
	parsing_should_fail(R"(ʎǝʞ2 = 0)"sv);
	#endif
}
