// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

TEST_CASE("parsing - tables")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
[table]

[table-1]
key1 = "some string"
key2 = 123

[table-2]
key1 = "another string"
key2 = 456

[dog."tater.man"]
type.name = "pug"

[a.b.c]            # this is best practice
[ d.e.f ]          # same as [d.e.f]
[ g .  h  . i ]    # same as [g.h.i]
[ j . "k" . 'l' ]  # same as [j."k".'l']

# [x] you
# [x.y] don't
# [x.y.z] need these
[x.y.z.w] # for this to work

[x] # defining a super-table afterwards is ok

[fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple.texture]  # you can add sub-tables
smooth = true

)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["table"].as<table>());
			CHECK(tbl["table"].as<table>()->size() == 0u);

			REQUIRE(tbl["table-1"].as<table>());
			CHECK(tbl["table-1"].as<table>()->size() == 2u);
			CHECK(tbl["table-1"]["key1"] == "some string"sv);
			CHECK(tbl["table-1"]["key2"] == 123);

			REQUIRE(tbl["table-2"].as<table>());
			CHECK(tbl["table-2"].as<table>()->size() == 2u);
			CHECK(tbl["table-2"]["key1"] == "another string"sv);
			CHECK(tbl["table-2"]["key2"] == 456);

			REQUIRE(tbl["dog"].as<table>());
			CHECK(tbl["dog"].as<table>()->size() == 1u);

			REQUIRE(tbl["dog"]["tater.man"].as<table>());
			CHECK(tbl["dog"]["tater.man"].as<table>()->size() == 1u);
			CHECK(tbl["dog"]["tater.man"]["type"]["name"] == "pug"sv);

			CHECK(tbl["a"].as<table>());
			CHECK(tbl["a"]["b"].as<table>());
			CHECK(tbl["a"]["b"]["c"].as<table>());

			CHECK(tbl["d"].as<table>());
			CHECK(tbl["d"]["e"].as<table>());
			CHECK(tbl["d"]["e"]["f"].as<table>());

			CHECK(tbl["g"].as<table>());
			CHECK(tbl["g"]["h"].as<table>());
			CHECK(tbl["g"]["h"]["i"].as<table>());

			CHECK(tbl["j"].as<table>());
			CHECK(tbl["j"]["k"].as<table>());
			CHECK(tbl["j"]["k"]["l"].as<table>());

			REQUIRE(tbl["fruit"].as<table>());
			CHECK(tbl["fruit"]["apple"]["color"] == "red"sv);
			CHECK(tbl["fruit"]["apple"]["taste"]["sweet"] == true);
			CHECK(tbl["fruit"]["apple"]["texture"]["smooth"] == true);
		}
	);


	parsing_should_fail(FILE_LINE_ARGS, R"(
# DO NOT DO THIS

[fruit]
apple = "red"

[fruit]
orange = "orange"
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
# DO NOT DO THIS EITHER

[fruit]
apple = "red"

[fruit.apple]
texture = "smooth"
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
[fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple]
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
[fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple.taste]
)"sv);

	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
# VALID BUT DISCOURAGED
[fruit.apple]
[animal]
[fruit.orange]
)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["animal"].as<table>());
			CHECK(tbl["animal"].as<table>()->size() == 0u);

			REQUIRE(tbl["fruit"].as<table>());
			CHECK(tbl["fruit"].as<table>()->size() == 2u);

			REQUIRE(tbl["fruit"]["apple"].as<table>());
			REQUIRE(tbl["fruit"]["orange"].as<table>());
		}
	);


	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
# RECOMMENDED
[fruit.apple]
[fruit.orange]
[animal]
)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["animal"].as<table>());
			CHECK(tbl["animal"].as<table>()->size() == 0u);

			REQUIRE(tbl["fruit"].as<table>());
			CHECK(tbl["fruit"].as<table>()->size() == 2u);

			REQUIRE(tbl["fruit"]["apple"].as<table>());
			REQUIRE(tbl["fruit"]["orange"].as<table>());
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, R"([])"sv);
}

TEST_CASE("parsing - inline tables")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
name = { first = "Tom", last = "Preston-Werner" }
point = { x = 1, y = 2 }
animal = { type.name = "pug" }

[product]
type = { name = "Nail" }
)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["name"].as<table>());
			CHECK(tbl["name"].as<table>()->size() == 2u);
			CHECK(tbl["name"]["first"] == "Tom"sv);
			CHECK(tbl["name"]["last"] == "Preston-Werner"sv);

			REQUIRE(tbl["point"].as<table>());
			CHECK(tbl["point"].as<table>()->size() == 2u);
			CHECK(tbl["point"]["x"] == 1);
			CHECK(tbl["point"]["y"] == 2);

			REQUIRE(tbl["animal"].as<table>());
			CHECK(tbl["animal"].as<table>()->size() == 1u);
			REQUIRE(tbl["animal"]["type"].as<table>());
			CHECK(tbl["animal"]["type"].as<table>()->size() == 1u);
			CHECK(tbl["animal"]["type"]["name"] == "pug"sv);

			REQUIRE(tbl["product"].as<table>());
			CHECK(tbl["product"].as<table>()->size() == 1u);
			REQUIRE(tbl["product"]["type"].as<table>());
			CHECK(tbl["product"]["type"].as<table>()->size() == 1u);
			CHECK(tbl["product"]["type"]["name"] == "Nail"sv);
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, R"(
[product]
type = { name = "Nail" }
type.edible = false  # INVALID
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
[product]
type.name = "Nail"
type = { edible = false }  # INVALID
)"sv);

	// "newlines are allowed between the curly braces [if] they are valid within a value."
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
test = { val1 = "foo", val2 = [
	1, 2,
	3
], val3 = "bar" }
)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["test"].as<table>());
			CHECK(tbl["test"].as<table>()->size() == 3u);
			CHECK(tbl["test"]["val1"] == "foo"sv);
			REQUIRE(tbl["test"]["val2"].as<array>());
			CHECK(tbl["test"]["val2"].as<array>()->size() == 3u);
			CHECK(tbl["test"]["val2"][0] == 1);
			CHECK(tbl["test"]["val2"][1] == 2);
			CHECK(tbl["test"]["val2"][2] == 3);
			CHECK(tbl["test"]["val3"] == "bar"sv);
		}
	);

	// toml/issues/516 (newlines/trailing commas in inline tables)
	#if TOML_LANG_UNRELEASED
	{
		parsing_should_succeed(
			FILE_LINE_ARGS,
			R"(
name = {
	first = "Tom",
	last = "Preston-Werner",
}
)"sv,
			[](table&& tbl)
			{
				REQUIRE(tbl["name"].as<table>());
				CHECK(tbl["name"].as<table>()->size() == 2u);
				CHECK(tbl["name"]["first"] == "Tom"sv);
				CHECK(tbl["name"]["last"] == "Preston-Werner"sv);
			}
		);

	}
	#else
	{
		// "A terminating comma (also called trailing comma) is not permitted after the last key/value pair in an inline table."
		parsing_should_fail(FILE_LINE_ARGS, R"(name = { first = "Tom", last = "Preston-Werner", })"sv);

		// "No newlines are allowed between the curly braces unless they are valid within a value."
		parsing_should_fail(FILE_LINE_ARGS, R"(
name = {
	first = "Tom",
	last = "Preston-Werner"
}
)"sv);

	}
	#endif

}

TEST_CASE("parsing - arrays-of-tables")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
points = [ { x = 1, y = 2, z = 3 },
           { x = 7, y = 8, z = 9 },
           { x = 2, y = 4, z = 8 } ]

[[products]]
name = "Hammer"
sku = 738594937

[[products]]

[[products]]
name = "Nail"
sku = 284758393

color = "gray"

[[fruit]]
  name = "apple"

  [fruit.physical]  # subtable
    color = "red"
    shape = "round"

  [[fruit.variety]]  # nested array of tables
    name = "red delicious"

  [[fruit.variety]]
    name = "granny smith"

[[fruit]]
  name = "banana"

  [[fruit.variety]]
    name = "plantain"

)"sv,
		[](table&& tbl)
		{
			REQUIRE(tbl["points"].as<array>());
			CHECK(tbl["points"].as<array>()->size() == 3u);
			CHECK(tbl["points"].as<array>()->is_homogeneous());
			CHECK(tbl["points"].as<array>()->is_array_of_tables());
			CHECK(tbl["points"][0]["x"] == 1);
			CHECK(tbl["points"][0]["y"] == 2);
			CHECK(tbl["points"][0]["z"] == 3);
			CHECK(tbl["points"][1]["x"] == 7);
			CHECK(tbl["points"][1]["y"] == 8);
			CHECK(tbl["points"][1]["z"] == 9);
			CHECK(tbl["points"][2]["x"] == 2);
			CHECK(tbl["points"][2]["y"] == 4);
			CHECK(tbl["points"][2]["z"] == 8);

			REQUIRE(tbl["products"].as<array>());
			CHECK(tbl["products"].as<array>()->size() == 3u);
			CHECK(tbl["products"].as<array>()->is_homogeneous());
			CHECK(tbl["products"].as<array>()->is_array_of_tables());

			REQUIRE(tbl["products"][0].as<table>());
			CHECK(tbl["products"][0].as<table>()->size() == 2u);
			CHECK(tbl["products"][0]["name"] == "Hammer"sv);
			CHECK(tbl["products"][0]["sku"] == 738594937);

			REQUIRE(tbl["products"][1].as<table>());
			CHECK(tbl["products"][1].as<table>()->size() == 0u);

			REQUIRE(tbl["products"][2].as<table>());
			CHECK(tbl["products"][2].as<table>()->size() == 3u);
			CHECK(tbl["products"][2]["name"] == "Nail"sv);
			CHECK(tbl["products"][2]["sku"] == 284758393);
			CHECK(tbl["products"][2]["color"] == "gray"sv);


			REQUIRE(tbl["fruit"].as<array>());
			CHECK(tbl["fruit"].as<array>()->size() == 2u);
			CHECK(tbl["fruit"].as<array>()->is_homogeneous());
			CHECK(tbl["fruit"].as<array>()->is_array_of_tables());

			REQUIRE(tbl["fruit"][0].as<table>());
			CHECK(tbl["fruit"][0].as<table>()->size() == 3u);
			CHECK(tbl["fruit"][0]["name"] == "apple"sv);

			REQUIRE(tbl["fruit"][0]["physical"].as<table>());
			CHECK(tbl["fruit"][0]["physical"].as<table>()->size() == 2u);
			CHECK(tbl["fruit"][0]["physical"]["color"] == "red"sv);
			CHECK(tbl["fruit"][0]["physical"]["shape"] == "round"sv);

			REQUIRE(tbl["fruit"][0]["variety"].as<array>());
			CHECK(tbl["fruit"][0]["variety"].as<array>()->size() == 2u);
			CHECK(tbl["fruit"][0]["variety"].as<array>()->is_homogeneous());
			CHECK(tbl["fruit"][0]["variety"].as<array>()->is_array_of_tables());
			CHECK(tbl["fruit"][0]["variety"][0]["name"] == "red delicious"sv);
			CHECK(tbl["fruit"][0]["variety"][1]["name"] == "granny smith"sv);

			REQUIRE(tbl["fruit"][1].as<table>());
			CHECK(tbl["fruit"][1].as<table>()->size() == 2u);
			CHECK(tbl["fruit"][1]["name"] == "banana"sv);

			REQUIRE(tbl["fruit"][1]["variety"].as<array>());
			CHECK(tbl["fruit"][1]["variety"].as<array>()->size() == 1u);
			CHECK(tbl["fruit"][1]["variety"].as<array>()->is_homogeneous());
			CHECK(tbl["fruit"][1]["variety"].as<array>()->is_array_of_tables());
			CHECK(tbl["fruit"][1]["variety"][0]["name"] == "plantain"sv);
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, R"(
# INVALID TOML DOC
[fruit.physical]  # subtable, but to which parent element should it belong?
  color = "red"
  shape = "round"

[[fruit]]  # parser must throw an error upon discovering that "fruit" is
           # an array rather than a table
  name = "apple"
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
# INVALID TOML DOC
fruit = []

[[fruit]] # Not allowed
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [[fruit.variety]]
    name = "red delicious"

  # INVALID: This table conflicts with the previous array of tables
  [fruit.variety]
    name = "granny smith"
)"sv);

	parsing_should_fail(FILE_LINE_ARGS, R"(
# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [fruit.physical]
    color = "red"
    shape = "round"

  # INVALID: This array of tables conflicts with the previous table
  [[fruit.physical]]
    color = "green"
)"sv);

}
