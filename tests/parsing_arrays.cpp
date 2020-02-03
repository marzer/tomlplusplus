#include "tests.h"

TEST_CASE("parsing - arrays")
{
	parsing_should_succeed(S(R"(
integers = [ 1, 2, 3 ]
integers2 = [
  1, 2, 3
]
integers3 = [
  1,
  2, # this is ok
]
colors = [ "red", "yellow", "green" ]
nested_array_of_int = [ [ 1, 2 ], [3, 4, 5] ]
nested_mixed_array = [ [ 1, 2 ], ["a", "b", "c"] ]
string_array = [ "all", 'strings', """are the same""", '''type''' ]
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("integers")].as<array>());
			CHECK(tbl[S("integers")].as<array>()->is_homogeneous());
			CHECK(tbl[S("integers")].as<array>()->size() == 3);
			CHECK(tbl[S("integers")][0] == 1);
			CHECK(tbl[S("integers")][1] == 2);
			CHECK(tbl[S("integers")][2] == 3);

			REQUIRE(tbl[S("integers2")].as<array>());
			CHECK(tbl[S("integers2")].as<array>()->is_homogeneous());
			CHECK(tbl[S("integers2")].as<array>()->size() == 3);
			CHECK(tbl[S("integers2")][0] == 1);
			CHECK(tbl[S("integers2")][1] == 2);
			CHECK(tbl[S("integers2")][2] == 3);

			REQUIRE(tbl[S("integers3")].as<array>());
			CHECK(tbl[S("integers3")].as<array>()->is_homogeneous());
			CHECK(tbl[S("integers3")].as<array>()->size() == 2);
			CHECK(tbl[S("integers3")][0] == 1);
			CHECK(tbl[S("integers3")][1] == 2);

			REQUIRE(tbl[S("colors")].as<array>());
			CHECK(tbl[S("colors")].as<array>()->is_homogeneous());
			CHECK(tbl[S("colors")].as<array>()->size() == 3);
			CHECK(tbl[S("colors")][0] == S("red"sv));
			CHECK(tbl[S("colors")][1] == S("yellow"sv));
			CHECK(tbl[S("colors")][2] == S("green"sv));

			REQUIRE(tbl[S("nested_array_of_int")].as<array>());
			CHECK(tbl[S("nested_array_of_int")].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_array_of_int")].as<array>()->size() == 2);
			REQUIRE(tbl[S("nested_array_of_int")][0].as<array>());
			CHECK(tbl[S("nested_array_of_int")][0].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_array_of_int")][0].as<array>()->size() == 2);
			CHECK(tbl[S("nested_array_of_int")][0][0] == 1);
			CHECK(tbl[S("nested_array_of_int")][0][1] == 2);
			REQUIRE(tbl[S("nested_array_of_int")][1].as<array>());
			CHECK(tbl[S("nested_array_of_int")][1].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_array_of_int")][1].as<array>()->size() == 3);
			CHECK(tbl[S("nested_array_of_int")][1][0] == 3);
			CHECK(tbl[S("nested_array_of_int")][1][1] == 4);
			CHECK(tbl[S("nested_array_of_int")][1][2] == 5);

			REQUIRE(tbl[S("nested_mixed_array")].as<array>());
			CHECK(tbl[S("nested_mixed_array")].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_mixed_array")].as<array>()->size() == 2);
			REQUIRE(tbl[S("nested_mixed_array")][0].as<array>());
			CHECK(tbl[S("nested_mixed_array")][0].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_mixed_array")][0].as<array>()->size() == 2);
			CHECK(tbl[S("nested_mixed_array")][0][0] == 1);
			CHECK(tbl[S("nested_mixed_array")][0][1] == 2);
			REQUIRE(tbl[S("nested_mixed_array")][1].as<array>());
			CHECK(tbl[S("nested_mixed_array")][1].as<array>()->is_homogeneous());
			CHECK(tbl[S("nested_mixed_array")][1].as<array>()->size() == 3);
			CHECK(tbl[S("nested_mixed_array")][1][0] == S("a"sv));
			CHECK(tbl[S("nested_mixed_array")][1][1] == S("b"sv));
			CHECK(tbl[S("nested_mixed_array")][1][2] == S("c"sv));

			REQUIRE(tbl[S("string_array")].as<array>());
			CHECK(tbl[S("string_array")].as<array>()->is_homogeneous());
			CHECK(tbl[S("string_array")].as<array>()->size() == 4);
			CHECK(tbl[S("string_array")][0] == S("all"sv));
			CHECK(tbl[S("string_array")][1] == S("strings"sv));
			CHECK(tbl[S("string_array")][2] == S("are the same"sv));
			CHECK(tbl[S("string_array")][3] == S("type"sv));
			REQUIRE(tbl[S("integers")].as<array>());
			CHECK(tbl[S("integers")].as<array>()->is_homogeneous());
			CHECK(tbl[S("integers")].as<array>()->size() == 3);
			CHECK(tbl[S("integers")][0] == 1);
			CHECK(tbl[S("integers")][1] == 2);
			CHECK(tbl[S("integers")][2] == 3);
		}
	);

	// toml/issues/665 - heterogeneous arrays
	#if TOML_LANG_HIGHER_THAN(0, 5, 0)

	parsing_should_succeed(S(R"(
# Mixed-type arrays are allowed
numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]
contributors = [
  "Foo Bar <foo@example.com>",
  { name = "Baz Qux", email = "bazqux@example.com", url = "https://example.com/bazqux" }
]
)"sv),
		[](table&& tbl) noexcept
		{
			REQUIRE(tbl[S("numbers")].as<array>());
			CHECK(!tbl[S("numbers")].as<array>()->is_homogeneous());
			CHECK(tbl[S("numbers")].as<array>()->size() == 6);
			CHECK(tbl[S("numbers")][0].as<double>());
			CHECK(tbl[S("numbers")][1].as<double>());
			CHECK(tbl[S("numbers")][2].as<double>());
			CHECK(tbl[S("numbers")][3].as<int64_t>());
			CHECK(tbl[S("numbers")][4].as<int64_t>());
			CHECK(tbl[S("numbers")][5].as<int64_t>());
			CHECK(tbl[S("numbers")][0] == 0.1);
			CHECK(tbl[S("numbers")][1] == 0.2);
			CHECK(tbl[S("numbers")][2] == 0.5);
			CHECK(tbl[S("numbers")][3] == 1);
			CHECK(tbl[S("numbers")][4] == 2);
			CHECK(tbl[S("numbers")][5] == 5);

			REQUIRE(tbl[S("contributors")].as<array>());
			CHECK(!tbl[S("contributors")].as<array>()->is_homogeneous());
			CHECK(tbl[S("contributors")].as<array>()->size() == 2);
			CHECK(tbl[S("contributors")][0].as<string>());
			CHECK(tbl[S("contributors")][1].as<table>());
			CHECK(tbl[S("contributors")][0] == S("Foo Bar <foo@example.com>"sv));
			CHECK(tbl[S("contributors")][1][S("name")] == S("Baz Qux"sv));
			CHECK(tbl[S("contributors")][1][S("email")] == S("bazqux@example.com"sv));
			CHECK(tbl[S("contributors")][1][S("url")] == S("https://example.com/bazqux"sv));
		}
	);

	#else

	parsing_should_fail(S("numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]"sv));

	#endif
}
