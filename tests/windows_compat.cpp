// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

#if TOML_WINDOWS_COMPAT

TEST_CASE("windows compat")
{
	static constexpr auto toml_text = R"(
		[library]
		name = "toml++"
		authors = ["Mark Gillard <mark.gillard@outlook.com.au>"]
		
		[dependencies]
		cpp = 17
	)"sv;

	auto res = toml::parse(toml_text, L"kek.toml");
	#if !TOML_EXCEPTIONS
		REQUIRE(res.succeeded());
	#endif
	toml::table& tbl = res;

	// source paths
	REQUIRE(tbl.source().path != nullptr);
	CHECK(*tbl.source().path == "kek.toml"sv);
	CHECK(tbl.source().wide_path().has_value());
	CHECK(tbl.source().wide_path().value() == L"kek.toml"sv);

	// direct lookups from tables
	REQUIRE(tbl.get(S("library")) != nullptr);
	CHECK(tbl.get(S("library")) == tbl.get(S("library"sv)));
	CHECK(tbl.get(S("library")) == tbl.get(S("library"s)));
	CHECK(tbl.get(L"library") != nullptr);
	CHECK(tbl.get(L"library") == tbl.get(L"library"sv));
	CHECK(tbl.get(L"library") == tbl.get(L"library"s));
	CHECK(tbl.get(L"library") == tbl.get(S("library")));

	// node-view lookups
	CHECK(tbl[L"library"].node() != nullptr);
	CHECK(tbl[L"library"].node() == tbl.get(L"library"));

	// value queries
	REQUIRE(tbl[L"library"][L"name"].as_string() != nullptr);
	CHECK(tbl[L"library"][L"name"].value<std::wstring>() == L"toml++"s);
	CHECK(tbl[L"library"][L"name"].value_or(L""sv) == L"toml++"s);
	CHECK(tbl[L"library"][L"name"].value_or(L""s) == L"toml++"s);
	CHECK(tbl[L"library"][L"name"].value_or(L"") == L"toml++"s);

	// node-view comparisons
	CHECK(tbl[L"library"][L"name"] == S("toml++"sv));
	CHECK(tbl[L"library"][L"name"] == S("toml++"s));
	CHECK(tbl[L"library"][L"name"] == S("toml++"));
	CHECK(tbl[L"library"][L"name"] == L"toml++"sv);
	CHECK(tbl[L"library"][L"name"] == L"toml++"s);
	CHECK(tbl[L"library"][L"name"] == L"toml++");

	// table manipulation
	tbl.insert(L"foo", L"bar");
	REQUIRE(tbl.contains(S("foo")));
	REQUIRE(tbl.contains(L"foo"));
	CHECK(tbl[S("foo")] == S("bar"));
	tbl.insert_or_assign(L"foo", L"kek");
	CHECK(tbl[S("foo")] == S("kek"));
	tbl.erase(L"foo");
	REQUIRE(!tbl.contains(S("foo")));
	REQUIRE(!tbl.contains(L"foo"));
}

#endif // TOML_WINDOWS_COMPAT
