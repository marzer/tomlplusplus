#include "tests.h"

TEST_CASE("parsing - tables")
{
	parsing_should_succeed(S(R"(
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
[ j . "ʞ" . 'l' ]  # same as [j."ʞ".'l']

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

)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("table")].as<table>());
			CHECK(tbl[S("table")].as<table>()->size() == 0_sz);

			REQUIRE(tbl[S("table-1")].as<table>());
			CHECK(tbl[S("table-1")].as<table>()->size() == 2_sz);
			CHECK(tbl[S("table-1")][S("key1")] == S("some string"sv));
			CHECK(tbl[S("table-1")][S("key2")] == 123);

			REQUIRE(tbl[S("table-2")].as<table>());
			CHECK(tbl[S("table-2")].as<table>()->size() == 2_sz);
			CHECK(tbl[S("table-2")][S("key1")] == S("another string"sv));
			CHECK(tbl[S("table-2")][S("key2")] == 456);

			REQUIRE(tbl[S("dog")].as<table>());
			CHECK(tbl[S("dog")].as<table>()->size() == 1_sz);

			REQUIRE(tbl[S("dog")][S("tater.man")].as<table>());
			CHECK(tbl[S("dog")][S("tater.man")].as<table>()->size() == 1_sz);
			CHECK(tbl[S("dog")][S("tater.man")][S("type")][S("name")] == S("pug"sv));

			CHECK(tbl[S("a")].as<table>());
			CHECK(tbl[S("a")][S("b")].as<table>());
			CHECK(tbl[S("a")][S("b")][S("c")].as<table>());

			CHECK(tbl[S("d")].as<table>());
			CHECK(tbl[S("d")][S("e")].as<table>());
			CHECK(tbl[S("d")][S("e")][S("f")].as<table>());

			CHECK(tbl[S("g")].as<table>());
			CHECK(tbl[S("g")][S("h")].as<table>());
			CHECK(tbl[S("g")][S("h")][S("i")].as<table>());

			CHECK(tbl[S("j")].as<table>());
			CHECK(tbl[S("j")][S("ʞ")].as<table>());
			CHECK(tbl[S("j")][S("ʞ")][S("l")].as<table>());

			REQUIRE(tbl[S("fruit")].as<table>());
			CHECK(tbl[S("fruit")][S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("fruit")][S("apple")][S("taste")][S("sweet")] == true);
			CHECK(tbl[S("fruit")][S("apple")][S("texture")][S("smooth")] == true);
		}
	);


	parsing_should_fail(S(R"(
# DO NOT DO THIS

[fruit]
apple = "red"

[fruit]
orange = "orange"
)"sv));

	parsing_should_fail(S(R"(
# DO NOT DO THIS EITHER

[fruit]
apple = "red"

[fruit.apple]
texture = "smooth"
)"sv));

	parsing_should_fail(S(R"(
[fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple]
)"sv));

	parsing_should_fail(S(R"(
[fruit]
apple.color = "red"
apple.taste.sweet = true

[fruit.apple.taste]
)"sv));

	parsing_should_succeed(S(R"(
# VALID BUT DISCOURAGED
[fruit.apple]
[animal]
[fruit.orange]
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("animal")].as<table>());
			CHECK(tbl[S("animal")].as<table>()->size() == 0_sz);

			REQUIRE(tbl[S("fruit")].as<table>());
			CHECK(tbl[S("fruit")].as<table>()->size() == 2_sz);

			REQUIRE(tbl[S("fruit")][S("apple")].as<table>());
			REQUIRE(tbl[S("fruit")][S("orange")].as<table>());
		}
	);


	parsing_should_succeed(S(R"(
# RECOMMENDED
[fruit.apple]
[fruit.orange]
[animal]
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("animal")].as<table>());
			CHECK(tbl[S("animal")].as<table>()->size() == 0_sz);

			REQUIRE(tbl[S("fruit")].as<table>());
			CHECK(tbl[S("fruit")].as<table>()->size() == 2_sz);

			REQUIRE(tbl[S("fruit")][S("apple")].as<table>());
			REQUIRE(tbl[S("fruit")][S("orange")].as<table>());
		}
	);
}

TEST_CASE("parsing - inline tables")
{
	parsing_should_succeed(S(R"(
name = { first = "Tom", last = "Preston-Werner" }
point = { x = 1, y = 2 }
animal = { type.name = "pug" }

[product]
type = { name = "Nail" }
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("name")].as<table>());
			CHECK(tbl[S("name")].as<table>()->size() == 2_sz);
			CHECK(tbl[S("name")][S("first")] == S("Tom"sv));
			CHECK(tbl[S("name")][S("last")] == S("Preston-Werner"sv));

			REQUIRE(tbl[S("point")].as<table>());
			CHECK(tbl[S("point")].as<table>()->size() == 2_sz);
			CHECK(tbl[S("point")][S("x")] == 1);
			CHECK(tbl[S("point")][S("y")] == 2);

			REQUIRE(tbl[S("animal")].as<table>());
			CHECK(tbl[S("animal")].as<table>()->size() == 1_sz);
			REQUIRE(tbl[S("animal")][S("type")].as<table>());
			CHECK(tbl[S("animal")][S("type")].as<table>()->size() == 1_sz);
			CHECK(tbl[S("animal")][S("type")][S("name")] == S("pug"sv));

			REQUIRE(tbl[S("product")].as<table>());
			CHECK(tbl[S("product")].as<table>()->size() == 1_sz);
			REQUIRE(tbl[S("product")][S("type")].as<table>());
			CHECK(tbl[S("product")][S("type")].as<table>()->size() == 1_sz);
			CHECK(tbl[S("product")][S("type")][S("name")] == S("Nail"sv));
		}
	);

	parsing_should_fail(S(R"(
[product]
type = { name = "Nail" }
type.edible = false  # INVALID
)"sv));

	parsing_should_fail(S(R"(
[product]
type.name = "Nail"
type = { edible = false }  # INVALID
)"sv));

	// "newlines are allowed between the curly braces [if] they are valid within a value."
	parsing_should_succeed(S(R"(
test = { val1 = "foo", val2 = [
	1, 2,
	3
], val3 = "bar" }
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("test")].as<table>());
			CHECK(tbl[S("test")].as<table>()->size() == 3_sz);
			CHECK(tbl[S("test")][S("val1")] == S("foo"sv));
			REQUIRE(tbl[S("test")][S("val2")].as<array>());
			CHECK(tbl[S("test")][S("val2")].as<array>()->size() == 3_sz);
			CHECK(tbl[S("test")][S("val2")][0] == 1);
			CHECK(tbl[S("test")][S("val2")][1] == 2);
			CHECK(tbl[S("test")][S("val2")][2] == 3);
			CHECK(tbl[S("test")][S("val3")] == S("bar"sv));
		}
	);

	#if TOML_LANG_HIGHER_THAN(0, 5, 0)
	{
		// toml/issues/516 - allow newlines and trailing commas in inline tables
		parsing_should_succeed(S(R"(
name = {
	first = "Tom",
	last = "Preston-Werner",
}
)"sv),
			[](table&& tbl) noexcept
			{
				REQUIRE(tbl[S("name")].as<table>());
				CHECK(tbl[S("name")].as<table>()->size() == 2_sz);
				CHECK(tbl[S("name")][S("first")] == S("Tom"sv));
				CHECK(tbl[S("name")][S("last")] == S("Preston-Werner"sv));
			}
		);

	}
	#else
	{
		// "A terminating comma (also called trailing comma) is not permitted after the last key/value pair in an inline table."
		parsing_should_fail(S(R"(name = { first = "Tom", last = "Preston-Werner", })"sv));

		// "No newlines are allowed between the curly braces unless they are valid within a value."
		parsing_should_fail(S(R"(
name = {
	first = "Tom",
	last = "Preston-Werner"
}
)"sv));

	}
	#endif

}

TEST_CASE("parsing - arrays-of-tables")
{
	parsing_should_succeed(S(R"(
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

)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("points")].as<array>());
			CHECK(tbl[S("points")].as<array>()->size() == 3_sz);
			CHECK(tbl[S("points")].as<array>()->is_homogeneous());
			CHECK(tbl[S("points")].as<array>()->is_array_of_tables());
			CHECK(tbl[S("points")][0][S("x")] == 1);
			CHECK(tbl[S("points")][0][S("y")] == 2);
			CHECK(tbl[S("points")][0][S("z")] == 3);
			CHECK(tbl[S("points")][1][S("x")] == 7);
			CHECK(tbl[S("points")][1][S("y")] == 8);
			CHECK(tbl[S("points")][1][S("z")] == 9);
			CHECK(tbl[S("points")][2][S("x")] == 2);
			CHECK(tbl[S("points")][2][S("y")] == 4);
			CHECK(tbl[S("points")][2][S("z")] == 8);

			REQUIRE(tbl[S("products")].as<array>());
			CHECK(tbl[S("products")].as<array>()->size() == 3_sz);
			CHECK(tbl[S("products")].as<array>()->is_homogeneous());
			CHECK(tbl[S("products")].as<array>()->is_array_of_tables());

			REQUIRE(tbl[S("products")][0].as<table>());
			CHECK(tbl[S("products")][0].as<table>()->size() == 2_sz);
			CHECK(tbl[S("products")][0][S("name")] == S("Hammer"sv));
			CHECK(tbl[S("products")][0][S("sku")] == 738594937);

			REQUIRE(tbl[S("products")][1].as<table>());
			CHECK(tbl[S("products")][1].as<table>()->size() == 0_sz);

			REQUIRE(tbl[S("products")][2].as<table>());
			CHECK(tbl[S("products")][2].as<table>()->size() == 3_sz);
			CHECK(tbl[S("products")][2][S("name")] == S("Nail"sv));
			CHECK(tbl[S("products")][2][S("sku")] == 284758393);
			CHECK(tbl[S("products")][2][S("color")] == S("gray"sv));


			REQUIRE(tbl[S("fruit")].as<array>());
			CHECK(tbl[S("fruit")].as<array>()->size() == 2_sz);
			CHECK(tbl[S("fruit")].as<array>()->is_homogeneous());
			CHECK(tbl[S("fruit")].as<array>()->is_array_of_tables());

			REQUIRE(tbl[S("fruit")][0].as<table>());
			CHECK(tbl[S("fruit")][0].as<table>()->size() == 3_sz);
			CHECK(tbl[S("fruit")][0][S("name")] == S("apple"sv));

			REQUIRE(tbl[S("fruit")][0][S("physical")].as<table>());
			CHECK(tbl[S("fruit")][0][S("physical")].as<table>()->size() == 2_sz);
			CHECK(tbl[S("fruit")][0][S("physical")][S("color")] == S("red"sv));
			CHECK(tbl[S("fruit")][0][S("physical")][S("shape")] == S("round"sv));

			REQUIRE(tbl[S("fruit")][0][S("variety")].as<array>());
			CHECK(tbl[S("fruit")][0][S("variety")].as<array>()->size() == 2_sz);
			CHECK(tbl[S("fruit")][0][S("variety")].as<array>()->is_homogeneous());
			CHECK(tbl[S("fruit")][0][S("variety")].as<array>()->is_array_of_tables());
			CHECK(tbl[S("fruit")][0][S("variety")][0][S("name")] == S("red delicious"sv));
			CHECK(tbl[S("fruit")][0][S("variety")][1][S("name")] == S("granny smith"sv));

			REQUIRE(tbl[S("fruit")][1].as<table>());
			CHECK(tbl[S("fruit")][1].as<table>()->size() == 2_sz);
			CHECK(tbl[S("fruit")][1][S("name")] == S("banana"sv));

			REQUIRE(tbl[S("fruit")][1][S("variety")].as<array>());
			CHECK(tbl[S("fruit")][1][S("variety")].as<array>()->size() == 1_sz);
			CHECK(tbl[S("fruit")][1][S("variety")].as<array>()->is_homogeneous());
			CHECK(tbl[S("fruit")][1][S("variety")].as<array>()->is_array_of_tables());
			CHECK(tbl[S("fruit")][1][S("variety")][0][S("name")] == S("plantain"sv));
		}
	);

	parsing_should_fail(S(R"(
# INVALID TOML DOC
[fruit.physical]  # subtable, but to which parent element should it belong?
  color = "red"
  shape = "round"

[[fruit]]  # parser must throw an error upon discovering that "fruit" is
           # an array rather than a table
  name = "apple"
)"sv));

	parsing_should_fail(S(R"(
# INVALID TOML DOC
fruit = []

[[fruit]] # Not allowed
)"sv));

	parsing_should_fail(S(R"(
# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [[fruit.variety]]
    name = "red delicious"

  # INVALID: This table conflicts with the previous array of tables
  [fruit.variety]
    name = "granny smith"
)"sv));

	parsing_should_fail(S(R"(
# INVALID TOML DOC
[[fruit]]
  name = "apple"

  [fruit.physical]
    color = "red"
    shape = "round"

  # INVALID: This array of tables conflicts with the previous table
  [[fruit.physical]]
    color = "green"
)"sv));

}
