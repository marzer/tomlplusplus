// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

// these tests pin the path semantics of v3.4.0, quirks included, and v3 keeps them as they are.
// passing means unchanged, not correct; v4 revisits them (docs/pages/v4.md)

TEST_CASE("path - parsing")
{
	SECTION("parsing")
	{
		CHECK(toml::path("").str() == "");
		CHECK(toml::path("[1]").str() == "[1]");
		CHECK(toml::path("[1][2]").str() == "[1][2]");
		CHECK(toml::path("  [1][2]").str() == "  [1][2]");
		CHECK(toml::path("a.  .b").str() == "a.  .b");
		CHECK(toml::path("test[23]").str() == "test[23]");
		CHECK(toml::path("[ 120  ]").str() == "[120]");
		CHECK(toml::path("[ 120\t\t]").str() == "[120]");
		CHECK(toml::path("[\t120]").str() == "[120]");
		CHECK(toml::path("test.value").str() == "test.value");
		CHECK(toml::path("test[0].value").str() == "test[0].value");
		CHECK(toml::path("test[1][2]\t .value").str() == "test[1][2].value");
		CHECK(toml::path("test[1]\t[2].value").str() == "test[1][2].value");
		CHECK(toml::path(".test[1][2]\t ..value").str() == ".test[1][2]..value");

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path(L"").str() == "");
		CHECK(toml::path(L"[1]").str() == "[1]");
		CHECK(toml::path(L"[1][2]").str() == "[1][2]");
		CHECK(toml::path(L"  [1][2]").str() == "  [1][2]");
		CHECK(toml::path(L"a.  .b").str() == "a.  .b");
		CHECK(toml::path(L"test[23]").str() == "test[23]");
		CHECK(toml::path(L"[ 120  ]").str() == "[120]");
		CHECK(toml::path(L"[ 120\t\t]").str() == "[120]");
		CHECK(toml::path(L"test.value").str() == "test.value");
		CHECK(toml::path(L"test[0].value").str() == "test[0].value");
		CHECK(toml::path(L"test[1][2]\t .value").str() == "test[1][2].value");
		CHECK(toml::path(L"test[1]\t[2].value").str() == "test[1][2].value");
		CHECK(toml::path(L".test[1][2]\t ..value").str() == ".test[1][2]..value");

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
		CHECK(!toml::path("test["));
		CHECK(!toml::path("test[ ]"));
		CHECK(!toml::path("test[-1]"));
		CHECK(!toml::path("test[+1]"));
		CHECK(!toml::path("test[99999999999999999999999999]"));
		CHECK(!toml::path("test[1]]"));
		CHECK(!toml::path("test]"));
		CHECK(!toml::path("test]a"));
		CHECK(!toml::path("]"));

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

	SECTION("parsing - unterminated array index")
	{
		CHECK(toml::path("test[1").str() == "test[1]");
		CHECK(toml::path("test[1 ").str() == "test[1]");
		CHECK(toml::path("test[1.a").str() == "test[1].a");
		CHECK(toml::path("test[1[2]").str() == "test[1][2]");
	}

	SECTION("parsing - largest array index")
	{
		const auto str = "[" + std::to_string(std::numeric_limits<size_t>::max()) + "]";
		const toml::path p{ str };
		REQUIRE(p.size() == 1u);
		CHECK(p[0].index() == std::numeric_limits<size_t>::max());
		CHECK(p.str() == str);
	}

	SECTION("parsing from literal")
	{
		auto p0 = "a.b.c[1][12]"_tpath;
		CHECK(p0);
		CHECK(p0.str() == "a.b.c[1][12]");

		CHECK("ab.cd[1]"_tpath == toml::path("ab.cd[1]"));

		CHECK(("an.invalid.path[a1]"_tpath).str() == "");
	}
}

TEST_CASE("path - manipulating")
{
	SECTION("parent_node and truncation")
	{
		toml::path p0("");
		CHECK(p0.parent().str() == "");

		toml::path p1("start.middle.end");
		CHECK(p1.parent().str() == "start.middle");
		CHECK(p1.parent().parent().str() == "start");
		CHECK(p1.parent().parent().parent().str() == "");
		CHECK(p1.parent().parent().parent().parent().str() == "");

		toml::path p2("[1][2][3]");
		CHECK(p2.parent().str() == "[1][2]");
		CHECK(p2.parent().parent().str() == "[1]");
		CHECK(p2.parent().parent().parent().str() == "");

		toml::path p3(".test");
		CHECK(p3.parent().str() == "");

		toml::path p4("test..");
		CHECK(p4.parent().str() == "test.");
		CHECK(p4.parent().parent().str() == "test");
		CHECK(p4.parent().parent().parent().str() == "");

		toml::path p5("test.key[12].subkey");
		CHECK(p5.parent().str() == "test.key[12]");
		CHECK(p5.parent().parent().str() == "test.key");
		CHECK(p5.parent().parent().parent().str() == "test");
		CHECK(p5.parent().parent().parent().parent().str() == "");

		toml::path p6("test.key1.key2.key3.key4");
		CHECK(p6.truncated(0).str() == "test.key1.key2.key3.key4");
		CHECK(p6.truncated(1).str() == "test.key1.key2.key3");
		CHECK(p6.truncated(4).str() == "test");
		CHECK(p6.truncated(5).str() == "");
		CHECK(p6.truncated(20).str() == "");
		CHECK(p6.str() == "test.key1.key2.key3.key4");

		p6.truncate(0);
		CHECK(p6.str() == "test.key1.key2.key3.key4");
		p6.truncate(2);
		CHECK(p6.str() == "test.key1.key2");
		p6.truncate(3);
		CHECK(p6.str() == "");

		toml::path p7("a[1].b");
		CHECK(&p7.truncate(10) == &p7);
		CHECK(p7.empty());
	}

	SECTION("subpath")
	{
		toml::path p0("a.simple[1].path[2].object");

		CHECK(p0.subpath(p0.begin() + 1, p0.begin() + 4).str() == "simple[1].path");
		CHECK(p0.subpath(p0.begin() + 1, p0.end() - 1).str() == "simple[1].path[2]");
		CHECK(p0.subpath(p0.begin(), p0.begin()).str() == "");
		CHECK(p0.subpath(p0.begin(), p0.end() - 5).str() == "a");
		CHECK(p0.subpath(p0.begin() + 2, p0.end() - 1).str() == "[1].path[2]");

		CHECK(p0.subpath(p0.begin() + 5, p0.end() - 5).str() == "");
		CHECK(!p0.subpath(p0.end(), p0.begin()));

		CHECK(p0.subpath(1, 4).str() == "simple[1].path[2]");
		CHECK(p0.subpath(0, 0).str() == "");
		CHECK(p0.subpath(2, 0).str() == "");
		CHECK(p0.subpath(2, 1).str() == "[1]");
		CHECK(p0.subpath(100, 1).str() == "");
		CHECK(p0.subpath(p0.size(), 1).str() == "");
		CHECK(p0.subpath(p0.size(), 0).str() == "");
		CHECK(p0.subpath(p0.size() + 1, 0).str() == "");
		CHECK(p0.subpath(0, p0.size()).str() == p0.str());
		CHECK(p0.subpath(2, 100).str() == "[1].path[2].object");
		CHECK(p0.subpath(0, 100).str() == "a.simple[1].path[2].object");
		CHECK(p0.subpath(0, static_cast<size_t>(-1)).str() == p0.str());
		CHECK(p0.subpath(1, static_cast<size_t>(-1)).str() == "simple[1].path[2].object");
		CHECK(p0.subpath(static_cast<size_t>(-1), 0).str() == "");
		CHECK(p0.subpath(static_cast<size_t>(-1), 1).str() == "");
		CHECK(p0.subpath(static_cast<size_t>(-1), static_cast<size_t>(-1)).str() == "");

		auto sub = p0.subpath(2, 2);
		REQUIRE(sub.size() == 2);
		CHECK(sub[0].type() == toml::path_component_type::array_index);
		CHECK(sub[0].index() == 1);
		CHECK(sub[1].type() == toml::path_component_type::key);
		CHECK(sub[1].key() == "path");

		toml::path p_single("foo");
		CHECK(p_single.subpath(0, 1).str() == "foo");
		CHECK(p_single.subpath(0, 5).str() == "foo");
		CHECK(p_single.subpath(1, 1).str() == "");
		CHECK(p_single.subpath(1, 0).str() == "");

		toml::path p_empty;
		CHECK(p_empty.subpath(0, 0).str() == "");
		CHECK(p_empty.subpath(0, 1).str() == "");
		CHECK(p_empty.subpath(10, 5).str() == "");
		CHECK(p_empty.subpath(static_cast<size_t>(-1), static_cast<size_t>(-1)).str() == "");
	}

	SECTION("leaf")
	{
		toml::path p0("one.two.three.four.five");
		CHECK(p0.leaf(0).str() == "");
		CHECK(p0.leaf().str() == "five");
		CHECK(p0.leaf(3).str() == "three.four.five");
		CHECK(p0.leaf(5).str() == "one.two.three.four.five");
		CHECK(p0.leaf(10).str() == "one.two.three.four.five");

		toml::path p1("[10][2][30][4][50]");
		CHECK(p1.leaf(0).str() == "");
		CHECK(p1.leaf().str() == "[50]");
		CHECK(p1.leaf(3).str() == "[30][4][50]");
		CHECK(p1.leaf(5).str() == "[10][2][30][4][50]");
		CHECK(p1.leaf(10).str() == "[10][2][30][4][50]");

		toml::path p2("one[1].two.three[3]");
		CHECK(p2.leaf(0).str() == "");
		CHECK(p2.leaf().str() == "[3]");
		CHECK(p2.leaf(3).str() == "two.three[3]");
		CHECK(p2.leaf(4).str() == "[1].two.three[3]");
		CHECK(p2.leaf(10).str() == "one[1].two.three[3]");
	}

	SECTION("append - string")
	{
		toml::path p0("start");
		CHECK(p0.size() == 1u);
		CHECK(p0.append("middle.end").str() == "start.middle.end");
		CHECK(p0.append("[12]").str() == "start.middle.end[12]");

		toml::path p1("");
		CHECK(p1.size() == 1u);
		p1.append("[1].key"sv);
		CHECK(p1.size() == 3u);
		CHECK(p1.str() == "[1].key"sv);

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p2("start");
		CHECK(p2.size() == 1u);
		CHECK(p2.append(L"middle.end").str() == "start.middle.end");
		CHECK(p2.append(L"[12]").str() == "start.middle.end[12]");

		toml::path p3("");
		CHECK(p3.append(L"[1].key").str() == "[1].key");

#endif // TOML_ENABLE_WINDOWS_COMPAT

		toml::path p4;
		CHECK(p4.size() == 0u);
		CHECK(p4.append("[1].key").str() == "[1].key");
	}

	SECTION("append - toml::path copy")
	{
		toml::path p0("start");
		toml::path appendee1("middle.end");
		toml::path appendee2("[12]");
		CHECK(p0.append(appendee1).str() == "start.middle.end");
		CHECK(p0.append(appendee2).str() == "start.middle.end[12]");

		// Ensure copies and not moves
		CHECK(appendee1.str() == "middle.end");
		CHECK(appendee2.str() == "[12]");

		toml::path p1("");
		toml::path appendee3("[1].key");
		CHECK(p1.append(appendee3).str() == "[1].key");

		// Ensure copies and not moves
		CHECK(appendee3.str() == "[1].key");
	}

	SECTION("append - toml::path move")
	{
		toml::path p0("start");
		CHECK(p0.append(toml::path{ "middle.end" }).str() == "start.middle.end");
		CHECK(p0.append(toml::path{ "[12]" }).str() == "start.middle.end[12]");

		toml::path p1("");
		CHECK(p1.size() == 1u);
		CHECK(p1.append(toml::path{ "[1].key" }).str() == "[1].key");

		toml::path p2;
		CHECK(p2.size() == 0u);
		CHECK(p2.append(toml::path{ "[1].key" }).str() == "[1].key");
	}

	SECTION("prepend - string")
	{
		toml::path p0("start");
		CHECK(p0.prepend("middle.end").str() == "middle.end.start");
		CHECK(p0.prepend("[12]").str() == "[12].middle.end.start");

		toml::path p1;
		CHECK(p1.prepend("[1].key").str() == "[1].key");

		toml::path p2("");
		CHECK(p2.prepend("[1].key").str() == "[1].key.");

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p3("start");
		CHECK(p3.prepend(L"middle.end").str() == "middle.end.start");
		CHECK(p3.prepend(L"[12]").str() == "[12].middle.end.start");

#endif // TOML_ENABLE_WINDOWS_COMPAT
	}

	SECTION("prepend - toml::path copy")
	{
		toml::path p0("start");
		toml::path prependee1("middle.end");
		toml::path prependee2("[12]");
		CHECK(p0.prepend(prependee1).str() == "middle.end.start");
		CHECK(p0.prepend(prependee2).str() == "[12].middle.end.start");

		// Ensure copies and not moves
		CHECK(prependee1.str() == "middle.end");
		CHECK(prependee2.str() == "[12]");

		toml::path p1;
		toml::path prependee3("[1].key");
		CHECK(p1.prepend(prependee3).str() == "[1].key");

		// Ensure copies and not moves
		CHECK(prependee3.str() == "[1].key");
	}

	SECTION("prepend - toml::path move")
	{
		toml::path p0("start");
		CHECK(p0.prepend(toml::path("middle.end")).str() == "middle.end.start");
		CHECK(p0.prepend(toml::path("[12]")).str() == "[12].middle.end.start");

		toml::path p1;
		CHECK(p1.prepend(toml::path("[1].key")).str() == "[1].key");
	}

	SECTION("alter components")
	{
		toml::path p0("start.mid[1][2].end");

		p0[3] = std::size_t{ 13 };
		CHECK(p0.str() == "start.mid[1][13].end");

		p0[0] = 2u;
		CHECK(p0.str() == "[2].mid[1][13].end");

		p0[0] = 10;
		CHECK(p0.str() == "[10].mid[1][13].end");

		p0[3] = "newkey";
		CHECK(p0.str() == "[10].mid[1].newkey.end");
	}

	SECTION("assign")
	{
		toml::path p0("start.mid[1][2].end");
		p0.assign("test.key[1]");
		CHECK(p0.str() == "test.key[1]");
		p0.assign("");
		CHECK(p0.str() == "");

		toml::path p1("a.test.path[1]");
		p1.assign("invalid[abc]");
		CHECK(!p1);
		CHECK(p1.str() == "");

		toml::path p2("another[1].test.path");
		p2.assign(toml::path("test"));
		CHECK(p2.str() == "test");
		p2.assign(toml::path(""));
		CHECK(p2.str() == "");

		toml::path p3("final.test[1]");
		p3.assign(toml::path("invalid[abc"));
		CHECK(!p3);
		CHECK(p3.str() == "");

		const toml::path source("copied.from[1]");
		toml::path p6("another[1].test.path");
		CHECK(&p6.assign(source) == &p6);
		CHECK(p6 == source);
		CHECK(source.str() == "copied.from[1]");

		toml::path p7("some.path");
		CHECK(&(p7 = "test.key[1]"sv) == &p7);
		CHECK(p7.str() == "test.key[1]");
		p7 = "invalid[abc]"sv;
		CHECK(!p7);
		CHECK(p7.empty());

#if TOML_ENABLE_WINDOWS_COMPAT

		toml::path p4("start.mid[1][2].end");
		p4.assign(L"test.key[1]");
		CHECK(p4.str() == "test.key[1]");
		p4.assign("");
		CHECK(p4.str() == "");

		toml::path p5("a.test.path[1]");
		p5.assign("invalid[abc]");
		CHECK(!p5);
		CHECK(p5.str() == "");

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

		CHECK("a.b.c"sv == toml::path("a.b.c"));
		CHECK(!("a.b"sv == toml::path("a.b.c")));
		CHECK("a.b"sv != toml::path("a.b.c"));
		CHECK(!("[1].a"sv != toml::path("[1].a")));
		CHECK(!(toml::path("a.b.c") == "a.b"sv));
		CHECK(!(toml::path("a.b.c") != "a.b.c"sv));

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path("a.b.c") == L"a.b.c");
		CHECK(toml::path("[1].a") == L"[1].a");

		CHECK(toml::path("a.b.c") != L"a.b");
		CHECK(toml::path("[1].b") != L"[1].b.c");

#endif // TOML_ENABLE_WINDOWS_COMPAT
	}

	SECTION("arithmetic")
	{
		CHECK(toml::path("a.b.c") + "a[1]" == "a.b.c.a[1]");
		CHECK((toml::path("a.b.c") + "a[1]") == "a.b.c.a[1]");

		CHECK(toml::path("a.b.c") + toml::path("a[1]") == "a.b.c.a[1]");

		toml::path p1("a.b");
		toml::path p2("c[1]");
		CHECK(p1 + p2 == "a.b.c[1]");

		CHECK(p1 + "c[1]" == "a.b.c[1]");

		CHECK("a.b" + p2 == "a.b.c[1]");

#if TOML_ENABLE_WINDOWS_COMPAT

		CHECK(toml::path("a.b.c") + L"a[1]" == "a.b.c.a[1]");

		CHECK(p1 + L"c[1]" == "a.b.c[1]");

		CHECK(L"a.b" + p2 == "a.b.c[1]");

#endif // TOML_ENABLE_WINDOWS_COMPAT
	}
}

TEST_CASE("path - misc")
{
	CHECK(toml::path{ "" }.str() == "");
	CHECK(toml::path{ "a" }.str() == "a");
	CHECK(toml::path{ "a.b" }.str() == "a.b");
	CHECK(toml::path{ "a.b.c" }.str() == "a.b.c");
	CHECK(toml::path{ ".a.b.c" }.str() == ".a.b.c");

	CHECK(toml::path{}.empty());
	CHECK(!toml::path{ "" }.empty());
	CHECK(!toml::path{ "a" }.empty());

	CHECK(static_cast<std::string>(toml::path("a.b[1]")) == "a.b[1]");
	CHECK(static_cast<bool>(toml::path("a.b[1]")));
	CHECK(!static_cast<bool>(toml::path("a.b[a b]")));

#if TOML_ENABLE_WINDOWS_COMPAT

	CHECK(static_cast<std::wstring>(toml::path("a.b[1]")) == L"a.b[1]");

#endif
}

TEST_CASE("path - default construction")
{
	const toml::path p;
	CHECK(p.empty());
	CHECK(!p);
	CHECK(p.size() == 0u);
	CHECK(p.begin() == p.end());
	CHECK(p.str().empty());
}

TEST_CASE("path - copying and moving")
{
	const toml::path original{ "a.b[1]" };

	toml::path copied{ original };
	CHECK(copied == original);

	const toml::path moved{ std::move(copied) };
	CHECK(moved == original);

	toml::path assigned{ "x" };
	assigned = original;
	CHECK(assigned == original);

	toml::path move_assigned{ "x" };
	move_assigned = toml::path{ original };
	CHECK(move_assigned == original);
}

TEST_CASE("path - clear")
{
	toml::path p{ "a.b[1]" };
	p.clear();
	CHECK(p.empty());
	CHECK(!p);
	CHECK(p.str().empty());
}

TEST_CASE("path - appending or prepending an unparseable string leaves the path unchanged")
{
	toml::path p{ "a.b" };
	p += "c[x]"sv;
	CHECK(p.str() == "a.b");
	p.append("c[x]"sv);
	CHECK(p.str() == "a.b");
	p.prepend("c[x]"sv);
	CHECK(p.str() == "a.b");
	CHECK(p + "c[x]"sv == p);
	CHECK("c[x]"sv + p == p);
}

TEST_CASE("path - element access")
{
	SECTION("const subscript")
	{
		const toml::path p{ "a[1].b" };
		static_assert(std::is_same_v<decltype(p[0]), const path_component&>);
		REQUIRE(p.size() == 3u);
		CHECK(p[0].key() == "a");
		CHECK(p[1].index() == 1u);
		CHECK(p[2].key() == "b");
	}

	SECTION("iteration")
	{
		static_assert(std::is_same_v<decltype(std::declval<toml::path&>().begin()), toml::path::iterator>);
		static_assert(std::is_same_v<decltype(std::declval<toml::path&>().end()), toml::path::iterator>);
		static_assert(std::is_same_v<decltype(std::declval<const toml::path&>().begin()), toml::path::const_iterator>);
		static_assert(std::is_same_v<decltype(std::declval<const toml::path&>().end()), toml::path::const_iterator>);
		static_assert(std::is_same_v<decltype(std::declval<toml::path&>().cbegin()), toml::path::const_iterator>);
		static_assert(std::is_same_v<decltype(std::declval<toml::path&>().cend()), toml::path::const_iterator>);

		toml::path p{ "a[1].b" };
		for (auto& component : p)
			if (component.type() == path_component_type::key)
				component = "x"sv;
		CHECK(p.str() == "x[1].x");

		const auto& cp = std::as_const(p);
		CHECK(std::distance(cp.begin(), cp.end()) == 3);
		CHECK(p.cbegin() == cp.begin());
		CHECK(p.cend() == cp.end());
		CHECK(std::distance(p.cbegin(), p.cend()) == 3);
	}
}

TEST_CASE("path - stream output")
{
	const auto str = GENERATE("a.b[1].c"sv,
							  "[1][2]"sv,
							  "[1].a"sv,
							  ".a"sv,
							  "a..b"sv,
							  "a."sv,
							  "a[1]."sv,
							  "a. b"sv,
							  "  [1][2]"sv,
							  ""sv);

	const toml::path p{ str };
	std::ostringstream ss;
	ss << p;
	CHECK(ss.str() == str);
	CHECK(p.str() == str);
	CHECK(toml::path{ ss.str() } == p);
}

TEST_CASE("path - stream output of an empty path")
{
	std::ostringstream ss;
	ss << toml::path{};
	CHECK(ss.str().empty());
}

TEST_CASE("path - components")
{
	SECTION("default constructed component is an empty key")
	{
		const path_component pc;
		CHECK(pc.type() == path_component_type::key);
		CHECK(pc.key().empty());
	}

	SECTION("array index")
	{
		const path_component pc{ 42_sz };
		CHECK(pc.type() == path_component_type::array_index);
		CHECK(pc.index() == 42u);
		CHECK(static_cast<size_t>(pc) == 42u);
	}

	SECTION("key")
	{
		const path_component pc{ "kek"sv };
		CHECK(pc.type() == path_component_type::key);
		CHECK(pc.key() == "kek");
		CHECK(static_cast<const std::string&>(pc) == "kek");
		CHECK(&static_cast<const std::string&>(pc) == &pc.key());
	}

	SECTION("equality")
	{
		CHECK_SYMMETRIC_EQUAL(path_component{ "kek"sv }, path_component{ "kek"sv });
		CHECK_SYMMETRIC_INEQUAL(path_component{ "kek"sv }, path_component{ "kok"sv });
		CHECK_SYMMETRIC_EQUAL(path_component{ 1_sz }, path_component{ 1_sz });
		CHECK_SYMMETRIC_INEQUAL(path_component{ 1_sz }, path_component{ 2_sz });
		CHECK_SYMMETRIC_INEQUAL(path_component{ "1"sv }, path_component{ 1_sz });
		CHECK_SYMMETRIC_INEQUAL(path_component{}, path_component{ 0_sz });
	}
}

TEST_CASE("path - moving a component moves its key")
{
	const std::string long_key(100u, 'k');

	SECTION("move construction")
	{
		path_component source{ std::string_view{ long_key } };
		const auto data = source.key().data();
		const path_component moved{ std::move(source) };
		CHECK(moved.key() == long_key);
		CHECK(moved.key().data() == data);
	}

	SECTION("move assignment over an index")
	{
		path_component source{ std::string_view{ long_key } };
		const auto data = source.key().data();
		path_component target{ 7_sz };
		target = std::move(source);
		CHECK(target.key() == long_key);
		CHECK(target.key().data() == data);
	}
}

TEST_CASE("path - component copying and moving")
{
	const auto source = GENERATE(path_component{ "kek"sv }, path_component{ 42_sz });
	const auto target = GENERATE(path_component{ "foo"sv }, path_component{ 7_sz });

	SECTION("copy construction")
	{
		const path_component copied{ source };
		CHECK(copied == source);
	}

	SECTION("move construction")
	{
		path_component temp{ source };
		const path_component moved{ std::move(temp) };
		CHECK(moved == source);
	}

	SECTION("copy assignment")
	{
		path_component pc{ target };
		CHECK(&(pc = source) == &pc);
		CHECK(pc == source);
	}

	SECTION("move assignment")
	{
		path_component pc{ target };
		path_component temp{ source };
		CHECK(&(pc = std::move(temp)) == &pc);
		CHECK(pc == source);
	}
}

TEST_CASE("path - component assignment from an index or a key")
{
	SECTION("index over a key")
	{
		path_component pc{ "kek"sv };
		CHECK(&(pc = 42_sz) == &pc);
		CHECK(pc.type() == path_component_type::array_index);
		CHECK(pc.index() == 42u);
	}

	SECTION("index over an index")
	{
		path_component pc{ 7_sz };
		pc = 42_sz;
		CHECK(pc.type() == path_component_type::array_index);
		CHECK(pc.index() == 42u);
	}

	SECTION("key over an index")
	{
		path_component pc{ 7_sz };
		CHECK(&(pc = "kek"sv) == &pc);
		CHECK(pc.type() == path_component_type::key);
		CHECK(pc.key() == "kek");
	}

	SECTION("key over a key")
	{
		path_component pc{ "foo"sv };
		pc = "kek"sv;
		CHECK(pc.type() == path_component_type::key);
		CHECK(pc.key() == "kek");
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
		CHECK(tbl["b"][2]["c"] == at_path(tbl, toml::path("b[2]   \t.c"))); // whitespace is allowed after array
																			// indexers

		CHECK(tbl["d"]);
		CHECK(tbl["d"] == at_path(tbl, toml::path("d")));

		CHECK(tbl["d"]["e"]);
		CHECK(tbl["d"]["e"] == at_path(tbl, toml::path("d.e")));
		CHECK(tbl["d"]["e"] != at_path(tbl, toml::path("d. e"))); // equivalent to "d"." e"
		CHECK(!at_path(tbl, toml::path("d. e")));

		CHECK(tbl["d"][""]);
		CHECK(tbl["d"][""] == at_path(tbl, toml::path("d.")));

		CHECK(!at_path(tbl, toml::path("has.missing.component")));
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

		CHECK(!arr.at_path(toml::path("[3].missing.component")));
	}

	SECTION("indexing operator")
	{
		// this section uses the operator[] of table and node_view
		CHECK(tbl[""]);
		CHECK(tbl[""] == tbl[toml::path("")]);

		CHECK(tbl["a"]);
		CHECK(tbl["a"] == tbl[toml::path("a")]);
		CHECK(tbl["a"] != tbl[toml::path(".a")]); // equivalent to ""."a"
		CHECK(!tbl[toml::path(".a")]);

		CHECK(tbl["b"]);
		CHECK(tbl["b"] == tbl[toml::path("b")]);

		CHECK(tbl["b"][0]);
		CHECK(tbl["b"][0] == tbl[toml::path("b[0]")]);
		CHECK(tbl["b"][0] == tbl[toml::path("b[0]     ")]);
		CHECK(tbl["b"][0] == tbl[toml::path("b[ 0\t]")]); // whitespace is allowed inside array indexer

		CHECK(tbl["b"][1]);
		CHECK(tbl["b"][1] != tbl[toml::path("b")][0]);
		CHECK(tbl["b"][1] == tbl[toml::path("b[1]")]);

		CHECK(tbl["b"][1][0]);
		CHECK(tbl["b"][1][0] == tbl[toml::path("b[1]")][0]);
		CHECK(tbl["b"][1][0] == tbl[toml::path("b[1]    \t   [0]")]); // whitespace is allowed after array
																	  // indexers

		CHECK(tbl["b"][2]["c"]);
		CHECK(tbl["b"][2]["c"] == tbl[toml::path("b")][toml::path("[2].c")]);
		CHECK(tbl["b"][2]["c"] == tbl[toml::path("b[2]   \t.c")]); // whitespace is allowed after array
																   // indexers

		CHECK(tbl["d"]);
		CHECK(tbl["d"] == tbl[toml::path("d")]);

		CHECK(tbl["d"]["e"]);
		CHECK(tbl["d"]["e"] == tbl[toml::path("d.e")]);
		CHECK(tbl["d"]["e"] != tbl[toml::path("d. e")]); // equivalent to "d"." e"
		CHECK(!tbl[toml::path("d. e")]);

		CHECK(tbl["d"][""]);
		CHECK(tbl["d"][""] == tbl[toml::path("d.")]);

		CHECK(!tbl[toml::path("has.missing.component")]);
	}

	SECTION("paths that do not resolve")
	{
		CHECK(!at_path(tbl, toml::path("z")));
		CHECK(!at_path(tbl, toml::path("d.z")));
		CHECK(!at_path(tbl, toml::path("a.b")));
		CHECK(!at_path(tbl, toml::path("b.c")));
		CHECK(!at_path(tbl, toml::path("a[0]")));
		CHECK(!at_path(tbl, toml::path("[0]")));
		CHECK(!at_path(tbl, toml::path("b[3]")));
		CHECK(!at_path(tbl, toml::path("b[2]..c")));
		CHECK(!at_path(tbl, toml::path("b[2].")));
	}

	SECTION("string and path overloads agree on strings that parse as paths")
	{
		const auto str = GENERATE("",
								  "a",
								  ".a",
								  "b",
								  "b[0]",
								  "b[ 0\t]",
								  "b[1][0]",
								  "b[1]    \t   [0]",
								  "b[1[0",
								  "b[2].c",
								  "b[2]   \t.c",
								  "b[2.c",
								  "b[2]..c",
								  "b[2].",
								  "b[3]",
								  "b.c",
								  "d",
								  "d.e",
								  "d. e",
								  "d.",
								  "d..",
								  "a.b",
								  "a[0]",
								  "[0]",
								  "z");
		INFO(str);
		CHECK(at_path(tbl, toml::path{ str }).node() == at_path(tbl, str).node());
	}

	SECTION("mutable views")
	{
		auto mtbl = tbl;
		auto view = at_path(mtbl, toml::path("b[2].c"));
		static_assert(std::is_same_v<decltype(view), node_view<node>>);
		static_assert(std::is_same_v<decltype(at_path(tbl, toml::path{})), node_view<const node>>);
		REQUIRE(view);
		view.ref<int64_t>() = 42;
		CHECK(mtbl["b"][2]["c"] == 42);
		CHECK(tbl["b"][2]["c"] == 4);
	}
}

TEST_CASE("path - accessing roots that cannot be traversed")
{
	const auto val = value{ 42 };
	CHECK(!at_path(val, toml::path("a")));
	CHECK(!at_path(val, toml::path("[0]")));
	CHECK(!at_path(val, toml::path("")));

	const table empty_table;
	CHECK(!at_path(empty_table, toml::path("a")));
	CHECK(!at_path(empty_table, toml::path("")));

	const array empty_array;
	CHECK(!at_path(empty_array, toml::path("[0]")));
}

TEST_CASE("path - semantics kept from v3.4.0")
{
	auto tbl		 = table{ { ""sv, 0 }, { "a"sv, 1 }, { "b"sv, array{ 2, 3 } } };
	const auto& ctbl = tbl;
	auto& arr		 = *tbl.get_as<array>("b");

	SECTION("an empty string is one empty key")
	{
		const toml::path p{ "" };
		REQUIRE(p.size() == 1u);
		CHECK(p[0].type() == path_component_type::key);
		CHECK(p[0].key().empty());
		CHECK(p != toml::path{});
		CHECK(at_path(tbl, p).node() == tbl.get(""));
		CHECK(at_path(tbl, ""sv).node() == tbl.get(""));
	}

	SECTION("each dot separates empty keys")
	{
		const toml::path p{ ".." };
		REQUIRE(p.size() == 3u);
		for (const auto& component : p)
			CHECK((component.type() == path_component_type::key && component.key().empty()));
		CHECK(toml::path{ "." }.size() == 2u);
	}

	SECTION("a dot directly before an index is ignored")
	{
		CHECK(toml::path{ "b.[1]" } == toml::path{ "b[1]" });
		CHECK(toml::path{ "[0].[1]" } == toml::path{ "[0][1]" });
		CHECK(at_path(tbl, "b.[1]"sv).node() == arr.get(1));

		const toml::path two_dots{ "b..[1]" };
		REQUIRE(two_dots.size() == 3u);
		CHECK(two_dots[1].key().empty());
		CHECK(!at_path(tbl, two_dots));
	}

	SECTION("printing drops an empty key directly before an index")
	{
		const toml::path p{ "b..[1]" };
		CHECK(p.str() == "b.[1]");
		CHECK(toml::path{ p.str() } == toml::path{ "b[1]" });
		CHECK(toml::path{ p.str() } != p);
		CHECK(toml::path{ ".[1]" }.str() == "[1]");
	}

	SECTION("an index can have leading zeros")
	{
		const toml::path p{ "[007]" };
		REQUIRE(p.size() == 1u);
		CHECK(p[0].index() == 7u);
		CHECK(p.str() == "[7]");
	}

	SECTION("keys are kept byte for byte")
	{
		const auto with_nul = "a\0b"sv;
		const toml::path p{ with_nul };
		REQUIRE(p.size() == 1u);
		CHECK(p[0].key() == with_nul);
		CHECK(toml::path{ "a\nb" }[0].key() == "a\nb");
		CHECK(toml::path{ "\xFF" }[0].key() == "\xFF");
		CHECK(toml::path{ " a . b " }[0].key() == " a ");
	}

	SECTION("the empty path selects the node itself, unless it is a value or an empty container")
	{
		toml::path cleared{ "a" };
		cleared.clear();

		const toml::path empty_paths[] = { toml::path{},
										   cleared,
										   toml::path{ "a" }.parent(),
										   toml::path{ "a.b" }.truncated(2),
										   toml::path{ "a" }.leaf(0),
										   toml::path{ "bad[" },
										   "bad["_tpath };

		const table empty_table;
		const array empty_array;
		for (size_t i = 0; i < std::size(empty_paths); i++)
		{
			CAPTURE(i);
			const auto& p = empty_paths[i];
			REQUIRE(p.empty());

			CHECK(at_path(tbl, p).node() == &tbl);
			CHECK(at_path(ctbl, p).node() == &tbl);
			CHECK(tbl.at_path(p).node() == &tbl);
			CHECK(tbl[p].node() == &tbl);
			CHECK(ctbl[p].node() == &tbl);
			CHECK(node_view<node>{ tbl }[p].node() == &tbl);
			CHECK(node_view<node>{ tbl }.at_path(p).node() == &tbl);
			CHECK(at_path(arr, p).node() == &arr);

			CHECK(!at_path(*tbl.get("a"), p));
			CHECK(!at_path(empty_table, p));
			CHECK(!at_path(empty_array, p));
			CHECK(!node_view<node>{}[p]);
		}
	}

	SECTION("a string that fails to parse selects nothing, but its empty path selects the node itself")
	{
		const auto bad = "bad["sv;
		CHECK(!at_path(tbl, bad));
		CHECK(!at_path(ctbl, bad));
		CHECK(!tbl.at_path(bad));
		CHECK(!node_view<node>{ tbl }.at_path(bad));
		CHECK(at_path(tbl, toml::path{ bad }).node() == &tbl);
	}

	SECTION("equality with a string parses the string")
	{
		CHECK(toml::path{ "b[1]" } == "b.[1]"sv);
		CHECK(toml::path{ "[7]" } == "[ 007 ]"sv);
		CHECK(toml::path{} == "bad["sv);
		CHECK(toml::path{ "bad[" } == toml::path{ "x[" });
		CHECK(toml::path{} != ""sv);
		CHECK(toml::path{ "a" } != " a"sv);
	}

	SECTION("assigning a string that fails to parse empties the path")
	{
		toml::path assigned{ "a.b" };
		assigned.assign("bad["sv);
		CHECK(assigned.empty());

		toml::path operator_assigned{ "a.b" };
		operator_assigned = "bad["sv;
		CHECK(operator_assigned.empty());
	}

	SECTION("appending or prepending an empty string adds an empty key")
	{
		CHECK(toml::path{ "a" } + ""sv == toml::path{ "a." });
		CHECK(""sv + toml::path{ "a" } == toml::path{ ".a" });

		toml::path p{ "a" };
		p.append(""sv);
		p.prepend(""sv);
		CHECK(p == toml::path{ ".a." });
	}
}
