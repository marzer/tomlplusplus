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

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path(L"").string() == "");
		CHECK(toml::path(L"[1]").string() == "[1]");
		CHECK(toml::path(L"[1][2]").string() == "[1][2]");
		CHECK(toml::path(L"  [1][2]").string() == "  [1][2]");
		CHECK(toml::path(L"a.  .b").string() == "a.  .b");
		CHECK(toml::path(L"test[23]").string() == "test[23]");
		CHECK(toml::path(L"[ 120		]").string() == "[120]");
		CHECK(toml::path(L"test.value").string() == "test.value");
		CHECK(toml::path(L"test[0].value").string() == "test[0].value");
		CHECK(toml::path(L"test[1][2]\t .value").string() == "test[1][2].value");
		CHECK(toml::path(L"test[1]\t[2].value").string() == "test[1][2].value");
		CHECK(toml::path(L".test[1][2]\t ..value").string() == ".test[1][2]..value");

#endif // TOML_ENABLE_WINDOWS_COMPAT

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

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(!toml::path(L"test[][2].value"));
		CHECK(!toml::path(L"test[      "));
		CHECK(!toml::path(L"test[1]a.b"));
		CHECK(!toml::path(L"test[1]   a.b"));
		CHECK(!toml::path(L"test[1a]"));
		CHECK(!toml::path(L"test[a1]"));
		CHECK(!toml::path(L"test[1!]"));
		CHECK(!toml::path(L"test[!1]"));
		CHECK(!toml::path(L"test[1 2]"));
		CHECK(!toml::path(L"test[1.2]"));
		CHECK(!toml::path(L"test[0.2]"));

#endif // TOML_ENABLE_WINDOWS_COMPAT

	}
}

TEST_CASE("path - manipulating")
{
	SECTION("parent_node and truncation")
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

		toml::path p6("test.key1.key2.key3.key4");
		CHECK(p6.truncated(0).string() == "test.key1.key2.key3.key4");
		CHECK(p6.truncated(1).string() == "test.key1.key2.key3");
		CHECK(p6.truncated(4).string() == "test");
		CHECK(p6.truncated(5).string() == "");
		CHECK(p6.truncated(20).string() == "");
		CHECK(p6.string() == "test.key1.key2.key3.key4");

		p6.truncate(0);
		CHECK(p6.string() == "test.key1.key2.key3.key4");
		p6.truncate(2);
		CHECK(p6.string() == "test.key1.key2");
		p6.truncate(3);
		CHECK(p6.string() == "");

	}

	SECTION("leaf")
	{
		toml::path p0("one.two.three.four.five");
		CHECK(p0.leaf(0).string() == "");
		CHECK(p0.leaf().string() == "five");
		CHECK(p0.leaf(3).string() == "three.four.five");
		CHECK(p0.leaf(5).string() == "one.two.three.four.five");
		CHECK(p0.leaf(10).string() == "one.two.three.four.five");

		toml::path p1("[10][2][30][4][50]");
		CHECK(p1.leaf(0).string() == "");
		CHECK(p1.leaf().string() == "[50]");
		CHECK(p1.leaf(3).string() == "[30][4][50]");
		CHECK(p1.leaf(5).string() == "[10][2][30][4][50]");
		CHECK(p1.leaf(10).string() == "[10][2][30][4][50]");

		toml::path p2("one[1].two.three[3]");
		CHECK(p2.leaf(0).string() == "");
		CHECK(p2.leaf().string() == "[3]");
		CHECK(p2.leaf(3).string() == "two.three[3]");
		CHECK(p2.leaf(4).string() == "[1].two.three[3]");
		CHECK(p2.leaf(10).string() == "one[1].two.three[3]");
	}

	SECTION("append - string")
	{
		toml::path p0("start");
		CHECK(p0.append("middle.end").string() == "start.middle.end");
		CHECK(p0.append("[12]").string() == "start.middle.end[12]");

		toml::path p1("");
		CHECK(p1.append("[1].key").string() == "[1].key");

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p2("start");
		CHECK(p2.append(L"middle.end").string() == "start.middle.end");
		CHECK(p2.append(L"[12]").string() == "start.middle.end[12]");

		toml::path p3("");
		CHECK(p3.append(L"[1].key").string() == "[1].key");

#endif // TOML_ENABLE_WINDOWS_COMPAT

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

	SECTION("prepend - string")
	{
		toml::path p0("start");
		CHECK(p0.prepend("middle.end").string() == "middle.end.start");
		CHECK(p0.prepend("[12]").string() == "[12].middle.end.start");

		toml::path p1("");
		CHECK(p1.prepend("[1].key").string() == "[1].key");

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p2("start");
		CHECK(p2.prepend(L"middle.end").string() == "middle.end.start");
		CHECK(p2.prepend(L"[12]").string() == "[12].middle.end.start");

		toml::path p3("");
		CHECK(p3.prepend(L"[1].key").string() == "[1].key");

#endif // TOML_ENABLE_WINDOWS_COMPAT

	}

	SECTION("prepend - toml::path copy")
	{
		toml::path p0("start");
		toml::path prependee1("middle.end");
		toml::path prependee2("[12]");
		CHECK(p0.prepend(prependee1).string() == "middle.end.start");
		CHECK(p0.prepend(prependee2).string() == "[12].middle.end.start");

		// Ensure copies and not moves
		CHECK(prependee1.string() == "middle.end");
		CHECK(prependee2.string() == "[12]");

		toml::path p1("");
		toml::path prependee3("[1].key");
		CHECK(p1.prepend(prependee3).string() == "[1].key");

		// Ensure copies and not moves
		CHECK(prependee3.string() == "[1].key");
	}

	SECTION("prepend - toml::path move")
	{
		toml::path p0("start");
		CHECK(p0.prepend(toml::path("middle.end")).string() == "middle.end.start");
		CHECK(p0.prepend(toml::path("[12]")).string() == "[12].middle.end.start");

		toml::path p1("");
		CHECK(p1.prepend(toml::path("[1].key")).string() == "[1].key");
	}

	SECTION("alter components")
	{
		toml::path p0("start.mid[1][2].end");

		p0[3].value = std::size_t{ 13 };
		CHECK(p0.string() == "start.mid[1][13].end");

		p0[3].type = path_component_type::key;
		p0[3].value = "newkey";
		CHECK(p0.string() == "start.mid[1].newkey.end");

		p0[0].value = std::size_t{ 2 };
		p0[0].type	 = path_component_type::array_index;
		CHECK(p0.string() == "[2].mid[1].newkey.end");
	}

	SECTION("assign")
	{
		toml::path p0("start.mid[1][2].end");
		p0.assign("test.key[1]");
		CHECK(p0.string() == "test.key[1]");
		p0.assign("");
		CHECK(p0.string() == "");

		toml::path p1("a.test.path[1]");
		p1.assign("invalid[abc]");
		CHECK(!p1);
		CHECK(p1.string() == "");

		toml::path p2("another[1].test.path");
		p2.assign(toml::path("test"));
		CHECK(p2.string() == "test");
		p2.assign(toml::path(""));
		CHECK(p2.string() == "");

		toml::path p3("final.test[1]");
		p3.assign(toml::path("invalid[abc"));
		CHECK(!p3);
		CHECK(p3.string() == "");

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p4("start.mid[1][2].end");
		p4.assign(L"test.key[1]");
		CHECK(p4.string() == "test.key[1]");
		p4.assign("");
		CHECK(p4.string() == "");

		toml::path p5("a.test.path[1]");
		p5.assign("invalid[abc]");
		CHECK(!p5);
		CHECK(p5.string() == "");

#endif // TOML_ENABLE_WINDOWS_COMPAT

	}

}

TEST_CASE("path - operators")
{
	SECTION("object equality")
	{
		CHECK(toml::path("a.b.c") == toml::path("a.b.c"));
		CHECK(toml::path("[1].a") == toml::path("[1].a"));

		CHECK(toml::path("a.b.c") != toml::path("a.b"));
		CHECK(toml::path("[1].b") != toml::path("[1].b.c"));
	}

	SECTION("string equality")
	{
		CHECK(toml::path("a.b.c") == "a.b.c");
		CHECK(toml::path("[1].a") == "[1].a");

		CHECK(toml::path("a.b.c") != "a.b");
		CHECK(toml::path("[1].b") != "[1].b.c");

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path("a.b.c") == L"a.b.c");
		CHECK(toml::path("[1].a") == L"[1].a");

		CHECK(toml::path("a.b.c") != L"a.b");
		CHECK(toml::path("[1].b") != L"[1].b.c");

#endif // TOML_ENABLE_WINDOWS_COMPAT

	}

	SECTION("arithmetic")
	{
		CHECK(toml::path("a.b.c") / "a[1]" == "a.b.c.a[1]");
		CHECK((toml::path("a.b.c") / "a[1]") == "a.b.c.a[1]");

		CHECK(toml::path("a.b.c") + toml::path("a[1]") == "a.b.c.a[1]");
		CHECK(toml::path("a.b.c") / toml::path("a[1]") == "a.b.c.a[1]");

		toml::path p1("a.b");
		toml::path p2("c[1]");
		CHECK((p1 + p2) == "a.b.c[1]");
		CHECK(p1 / p2 == "a.b.c[1]");

		CHECK(p1 + "c[1]" == "a.b.c[1]");
		CHECK(p1 / "c[1]" == "a.b.c[1]");

		CHECK("a.b" + p2 == "a.b.c[1]");
		CHECK("a.b" / p2 == "a.b.c[1]");

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path("a.b.c") / L"a[1]" == "a.b.c.a[1]");
		CHECK((toml::path("a.b.c") / L"a[1]") == "a.b.c.a[1]");

		CHECK(p1 + L"c[1]" == "a.b.c[1]");
		CHECK(p1 / L"c[1]" == "a.b.c[1]");

		CHECK(L"a.b" + p2 == "a.b.c[1]");
		CHECK(L"a.b" / p2 == "a.b.c[1]");

#endif // TOML_ENABLE_WINDOWS_COMPAT
	}
}

TEST_CASE("path - misc")
{

	CHECK(toml::path("a.b.c").string() == "a.b.c");

	CHECK(!toml::path("a").empty());
	CHECK(toml::path("").empty());

	CHECK(static_cast<std::string>(toml::path("a.b[1]")) == "a.b[1]");
	CHECK(static_cast<bool>(toml::path("a.b[1]")));
	CHECK(!static_cast<bool>(toml::path("a.b[a b]")));

#if TOML_ENABLE_WINDOWS_COMPAT

	CHECK(static_cast<std::wstring>(toml::path("a.b[1]")) == L"a.b[1]");

#endif

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

	SECTION("std::variant mismatches")
	{
		toml::path p0("b[2].c");
		p0[1].value = "abc";
		CHECK(!at_path(tbl, p0));

		toml::path p1("b[2].c");
		p1[0].value = std::size_t{ 1 };
		CHECK(!at_path(tbl, p1));
	}
}
