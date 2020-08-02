// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

// this file is about testing user misc. repros submitted via github issues, et cetera.

TEST_CASE("feedback - github/issues/49")
{
	// see: https://github.com/marzer/tomlplusplus/issues/49#issuecomment-664428571
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
}

TEST_CASE("feedback - github/pull/50")
{
	// see: https://github.com/marzer/tomlplusplus/pull/50
	{
		auto tbl = toml::table{ {{"value", 10}} };
		const toml::node* val = tbl.get("value");
		REQUIRE(val);
		REQUIRE(val->is_number());
		REQUIRE(val->is_integer());
		REQUIRE(val->ref<int64_t>() == 10);
		REQUIRE(val->value<int64_t>() == 10);
		REQUIRE(val->value_or(0) == 10);
		REQUIRE(val->value<double>() == 10.0);
		REQUIRE(val->value_or(0.0) == 10.0);
	}
}
