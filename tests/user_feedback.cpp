// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

// this file is about testing user misc. repros submitted via github issues, et cetera.

TEST_CASE("user feedback")
{
	SECTION("github/issues/49") // https://github.com/marzer/tomlplusplus/issues/49#issuecomment-664428571
	{
		toml::table t1;
		t1.insert_or_assign("bar1", toml::array{ 1, 2, 3 });
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } }
		}});

		t1.insert_or_assign("foo1", *t1.get("bar1"));
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3 } }
		}});

		//t1["foo1"] = t1["bar1"]; // does nothing, should this fail to compile?
		// - yes -

		//*t1["foo1"].node() = *t1["bar1"].node(); // compile failure; copying node_view would be a bad thing
		// - correct; copying toml::node directly like that is impossible b.c. it's an abstract base class-

		toml::array* array1 = t1["foo1"].node()->as_array();
		array1->push_back(4);
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } }
		}});

		t1.insert_or_assign("foo3", t1["foo1"]);
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo3"sv, toml::array{ 1, 2, 3, 4 } }
		}});

		t1.insert_or_assign("foo2", *t1["foo1"].node());
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo2"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo3"sv, toml::array{ 1, 2, 3, 4 } }
		}});

		toml::array* array2 = t1["foo2"].node()->as_array();
		array2->push_back("wrench");
		CHECK(t1 == toml::table{{
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo2"sv, toml::array{ 1, 2, 3, 4, "wrench" } },
			{ "foo3"sv, toml::array{ 1, 2, 3, 4 } }
		}});

		toml::table t2 = t1;
		CHECK(t2 == t1);
		CHECK(&t2 != &t1);

		//t2.emplace("bar", toml::array{6, 7}); // fails to compile? not sure what I did wrong
		// - it should be this: -
		t2.emplace<toml::array>("bar", 6, 7);
		CHECK(t2 == toml::table{{
			{ "bar"sv, toml::array{ 6, 7 } },
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo2"sv, toml::array{ 1, 2, 3, 4, "wrench" } },
			{ "foo3"sv, toml::array{ 1, 2, 3, 4 } }
		}});

		t2.insert_or_assign("bar2", toml::array{ 6, 7 });
		CHECK(t2 == toml::table{{
			{ "bar"sv, toml::array{ 6, 7 } },
			{ "bar1"sv, toml::array{ 1, 2, 3 } },
			{ "bar2"sv, toml::array{ 6, 7 } },
			{ "foo1"sv, toml::array{ 1, 2, 3, 4 } },
			{ "foo2"sv, toml::array{ 1, 2, 3, 4, "wrench" } },
			{ "foo3"sv, toml::array{ 1, 2, 3, 4 } }
		}});
	}

	SECTION("github/issues/65") // https://github.com/marzer/tomlplusplus/issues/65
	{
		// these test a number of things
		// - a comment at EOF
		// - a malformed UTF-8 sequence in a comment
		// - a malformed UTF-8 sequence during a KVP
		// - overlong numeric literals
		// all should fail to parse, but correctly issue an error (not crash!)
		
		parsing_should_fail(FILE_LINE_ARGS, "#\xf1\x63");
		parsing_should_fail(FILE_LINE_ARGS, "1= 0x6cA#+\xf1");
		parsing_should_fail(FILE_LINE_ARGS, "p=06:06:06#\x0b\xff");
		parsing_should_fail(FILE_LINE_ARGS, "''''d' 't' '+o\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
			"\x0c\x0c\x0c\x0c\x0c\r\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
			"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
			"\x0c\x0c\x0c\x0c\x0c\x0c\x0cop1\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
			"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
			"\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c' 'ml'\n\n%\x87"
		);
		parsing_should_fail(FILE_LINE_ARGS,
			R"(t =[ 9, 2, 1,"r", 9999999999999999999999999999999999999999999999999999999999999995.0 ])"
		);
	}

	SECTION("github/issues/67") // https://github.com/marzer/tomlplusplus/issues/67
	{
		const auto data = R"(array=["v1", "v2", "v3"])"sv;

		parsing_should_succeed(FILE_LINE_ARGS, data, [](auto&& table)
		{
			auto arr = table["array"].as_array();
			for (auto it = arr->cbegin(); it != arr->cend();)
				if (it->value_or(std::string_view{}) == "v2"sv)
					it = arr->erase(it);
				else
					++it;
			CHECK(arr->size() == 2);
		});
	}

	SECTION("github/issues/68") // https://github.com/marzer/tomlplusplus/issues/68
	{
		const auto data = R"(array=["v1", "v2", "v3"])"sv;
		parsing_should_succeed(FILE_LINE_ARGS, data, [](auto&& table)
		{
			std::stringstream ss;
			ss << table;
			CHECK(ss.str() == "array = [ 'v1', 'v2', 'v3' ]"sv);
		});
	}

	SECTION("github/issues/69") // https://github.com/marzer/tomlplusplus/issues/69
	{
		using namespace toml::literals; // should compile without namespace ambiguity
		auto table = "[table]\nkey=\"value\""_toml;
	}

	SECTION("github/pull/80") // https://github.com/marzer/tomlplusplus/pull/80
	{
		const auto data = R"(
			a = { "key" = 1 } # inline table
			b = []            # array value
			[[c]]             # array-of-tables with a single, empty table element
		)"sv;

		parsing_should_succeed(FILE_LINE_ARGS, data, [](auto&& table)
		{
			std::stringstream ss;
			ss << table;
			CHECK(ss.str() == R"(a = { key = 1 }
b = []

[[c]])"sv);
		});
	}

	SECTION("github/issues/100") // https://github.com/marzer/tomlplusplus/issues/100
	{
		// this tests for two separate things that should fail gracefully, not crash:
		// 1. pathologically-nested inputs
		// 2. a particular sequence of malformed UTF-8

		parsing_should_fail(FILE_LINE_ARGS, "fl =[ [[[[[[[[[[[[[[[\x36\x80\x86\x00\x00\x00\x2D\x36\x9F\x20\x00"sv);

		std::string s(2048_sz, '[');
		constexpr auto start = "fl =[ "sv;
		memcpy(s.data(), start.data(), start.length());
		parsing_should_fail(FILE_LINE_ARGS, std::string_view{ s });
	}

	SECTION("github/issues/112") // https://github.com/marzer/tomlplusplus/issues/112
	{
		parsing_should_fail(FILE_LINE_ARGS, R"(
			[a.b.c.d]
			  u = 6
			[a]
			  b.t = 8
			[a.b] # should cause redefinition error here
			  u = 0
		)", 6);

		parsing_should_fail(FILE_LINE_ARGS, R"(
			[a]
			  b.t = 8
			[a.b] # should cause redefinition error here
			  u = 0
		)", 4);
	}

}

