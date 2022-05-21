// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"
TOML_DISABLE_SPAM_WARNINGS;

TEST_CASE("path - parsing")
{

	SECTION("parsing")
	{
		CHECK(toml::path("").string()						== "");
		CHECK(toml::path("[1]").string()					== "[1]");
		CHECK(toml::path("[1][2]").string()					== "[1][2]");
		CHECK(toml::path("  [1][2]").string()				== "  [1][2]");
		CHECK(toml::path("a.  .b").string()					== "a.  .b");
		CHECK(toml::path("test[23]").string()				== "test[23]");
		CHECK(toml::path("[ 120		]").string()			== "[120]");
		CHECK(toml::path("test.value").string()				== "test.value");
		CHECK(toml::path("test[0].value").string()			== "test[0].value");
		CHECK(toml::path("test[1][2]\t .value").string()	== "test[1][2].value");
		CHECK(toml::path("test[1]\t[2].value").string()		== "test[1][2].value");
		CHECK(toml::path(".test[1][2]\t ..value").string()	== ".test[1][2]..value");
	}

	SECTION("parsing - errors")
	{
		CHECK(!toml::path("test[][2].value"));
		CHECK(!toml::path("test[      "));
		CHECK(!toml::path("test[1]a.b"));
		CHECK(!toml::path("test[1]   a.b"));
		CHECK(!toml::path("test[1a]"));
		CHECK(!toml::path("test[a1]"));
		CHECK(!toml::path("test[1!]"));
		CHECK(!toml::path("test[!1]"));
		CHECK(!toml::path("test[1 2]"));
		CHECK(!toml::path("test[1.2]"));
		CHECK(!toml::path("test[0.2]"));
	}
}

TEST_CASE("path - manipulating")
{
	SECTION("parent_nodes")
	{
		toml::path p0("");
		CHECK(p0.parent_path().string() == "");

		toml::path p1("start.middle.end");
		CHECK(p1.parent_path().string() == "start.middle");
		CHECK(p1.parent_path().parent_path().string() == "start");
		CHECK(p1.parent_path().parent_path().parent_path().string() == "");
		CHECK(p1.parent_path().parent_path().parent_path().parent_path().string() == "");

		toml::path p2("[1][2][3]");
		CHECK(p2.parent_path().string() == "[1][2]");
		CHECK(p2.parent_path().parent_path().string() == "[1]");
		CHECK(p2.parent_path().parent_path().parent_path().string() == "");

		toml::path p3(".test");
		CHECK(p3.parent_path().string() == "");

		toml::path p4("test..");
		CHECK(p4.parent_path().string() == "test.");
		CHECK(p4.parent_path().parent_path().string() == "test");
		CHECK(p4.parent_path().parent_path().parent_path().string() == "");

		toml::path p5("test.key[12].subkey");
		CHECK(p5.parent_path().string() == "test.key[12]");
		CHECK(p5.parent_path().parent_path().string() == "test.key");
		CHECK(p5.parent_path().parent_path().parent_path().string() == "test");
		CHECK(p5.parent_path().parent_path().parent_path().parent_path().string() == "");
	}

	SECTION("append - string")
	{
		toml::path p0("start");
		CHECK(p0.append("middle.end").string() == "start.middle.end");
		CHECK(p0.append("[12]").string() == "start.middle.end[12]");

		toml::path p1("");
		CHECK(p1.append("[1].key").string() == "[1].key");
	}

	SECTION("append - toml::path copy")
	{
		toml::path p0("start");
		toml::path appendee1("middle.end");
		toml::path appendee2("[12]");
		CHECK(p0.append(appendee1).string() == "start.middle.end");
		CHECK(p0.append(appendee2).string() == "start.middle.end[12]");

		// Ensure copies and not moves
		CHECK(appendee1.string() == "middle.end");
		CHECK(appendee2.string() == "[12]");

		toml::path p1("");
		toml::path appendee3("[1].key");
		CHECK(p1.append(appendee3).string() == "[1].key");

		// Ensure copies and not moves
		CHECK(appendee3.string() == "[1].key");
	}

	SECTION("append - toml::path move")
	{
		toml::path p0("start");
		CHECK(p0.append(toml::path("middle.end")).string() == "start.middle.end");
		CHECK(p0.append(toml::path("[12]")).string() == "start.middle.end[12]");

		toml::path p1("");
		CHECK(p1.append(toml::path("[1].key")).string() == "[1].key");
	}

	SECTION("alter components")
	{
		toml::path p0("start.mid[1][2].end");

		p0[3]->value = std::size_t{ 13 };
		CHECK(p0.string() == "start.mid[1][13].end");

		p0[3]->type = path_component_type::KEY;
		p0[3]->value = "newkey";
		CHECK(p0.string() == "start.mid[1].newkey.end");

		p0[0]->value = std::size_t{ 2 };
		p0[0]->type	 = path_component_type::ARRAY_INDEX;
		CHECK(p0.string() == "[2].mid[1].newkey.end");
	}

}

TEST_CASE("path - comparison")
{
	SECTION("equality")
	{
		CHECK(toml::path("a.b.c") == toml::path("a.b.c"));
		CHECK(toml::path("[1].a") == toml::path("[1].a"));

		CHECK(toml::path("a.b.c") != toml::path("a.b"));
		CHECK(toml::path("[1].b") != toml::path("[1].b.c"));
	}
}

TEST_CASE("path - accessing")
{

	// clang-format off
	
	const auto tbl = table
	{
		{ ""sv, 0 }, // blank key
		{ "a"sv, 1 },
		{
			"b"sv,
			array
			{
				2,
				array{ 3 },
				table { { "c", 4 } }
			},
		},
		{ "d", table{ {"e", 5, }, {""sv, -1 } } }
	};

	// clang-format on

	/*

	# equivalent to the following TOML:

	"" = 0
	a = 1
	b = [
			2,
			[ 3 ],
			{ "c" = 4 }
	]
	d = { "e" = 5, "" = -1 }

	*/

	SECTION("table")
	{
		// this section uses the free function version of at_path

		CHECK(tbl[""]);
		CHECK(tbl[""] == at_path(tbl, toml::path("")));

		CHECK(tbl["a"]);
		CHECK(tbl["a"] == at_path(tbl, toml::path("a")));
		CHECK(tbl["a"] != at_path(tbl, toml::path(".a"))); // equivalent to ""."a"
		CHECK(!at_path(tbl, toml::path(".a")));

		CHECK(tbl["b"]);
		CHECK(tbl["b"] == at_path(tbl, toml::path("b")));

		CHECK(tbl["b"][0]);
		CHECK(tbl["b"][0] == at_path(tbl, toml::path("b[0]")));
		CHECK(tbl["b"][0] == at_path(tbl, toml::path("b[0]     ")));
		CHECK(tbl["b"][0] == at_path(tbl, toml::path("b[ 0\t]"))); // whitespace is allowed inside array indexer

		CHECK(tbl["b"][1]);
		CHECK(tbl["b"][1] != tbl["b"][0]);
		CHECK(tbl["b"][1] == at_path(tbl, toml::path("b[1]")));

		CHECK(tbl["b"][1][0]);
		CHECK(tbl["b"][1][0] == at_path(tbl, toml::path("b[1][0]")));
		CHECK(tbl["b"][1][0] == at_path(tbl, toml::path("b[1]    \t   [0]"))); // whitespace is allowed after array
																   // indexers

		CHECK(tbl["b"][2]["c"]);
		CHECK(tbl["b"][2]["c"] == at_path(tbl, toml::path("b[2].c")));
		CHECK(tbl["b"][2]["c"] == at_path(tbl, toml::path("b[2]   \t.c"))); // whitespace is allowed after array indexers

		CHECK(tbl["d"]);
		CHECK(tbl["d"] == at_path(tbl, toml::path("d")));

		CHECK(tbl["d"]["e"]);
		CHECK(tbl["d"]["e"] == at_path(tbl, toml::path("d.e")));
		CHECK(tbl["d"]["e"] != at_path(tbl, toml::path("d. e"))); // equivalent to "d"." e"
		CHECK(!at_path(tbl, toml::path("d. e")));

		CHECK(tbl["d"][""]);
		CHECK(tbl["d"][""] == at_path(tbl, toml::path("d.")));
	}

	SECTION("array")
	{
		// this section uses the node_view member function version of at_path

		auto arr = tbl["b"];

		CHECK(tbl["b"][0]);
		CHECK(tbl["b"][0] == arr.at_path(toml::path("[0]")));
		CHECK(tbl["b"][0] == arr.at_path(toml::path("[0]     ")));
		CHECK(tbl["b"][0] == arr.at_path(toml::path("[ 0\t]"))); // whitespace is allowed inside array indexer

		CHECK(tbl["b"][1]);
		CHECK(tbl["b"][1].node() != arr[0].node());
		CHECK(tbl["b"][1] == arr.at_path(toml::path("[1]")));

		CHECK(tbl["b"][1][0]);
		CHECK(tbl["b"][1][0] == arr.at_path(toml::path("[1][0]")));
		CHECK(tbl["b"][1][0] == arr.at_path(toml::path("[1]    \t   [0]"))); // whitespace is allowed after array
																 // indexers

		CHECK(tbl["b"][2]["c"]);
		CHECK(tbl["b"][2]["c"] == arr.at_path(toml::path("[2].c")));
		CHECK(tbl["b"][2]["c"] == arr.at_path(toml::path("[2]   \t.c"))); // whitespace is allowed after array indexers
	}
}
