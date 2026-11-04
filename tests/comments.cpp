// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

#if TOML_ENABLE_COMMENTS

TOML_DISABLE_WARNINGS;
#include <sstream>
TOML_ENABLE_WARNINGS;

TEST_CASE("comments - parsing")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								# about key
								# second line
								key = "value" # trailing key
								no_comments = 1
								#nospace
								arr = [ 1, # inside array, ignored
										2 ]

								# about table
								[tbl] # trailing table
								# about nested
								nested = true

								# about array of tables
								[[aot]] # trailing aot
							)"sv,
						   [](table&& tbl)
						   {
							   auto& key = *tbl.get("key");
							   REQUIRE(key.leading_comments().size() == 2u);
							   CHECK(key.leading_comments()[0] == "about key"sv);
							   CHECK(key.leading_comments()[1] == "second line"sv);
							   CHECK(key.trailing_comment() == "trailing key"sv);

							   auto& none = *tbl.get("no_comments");
							   CHECK(none.leading_comments().empty());
							   CHECK(none.trailing_comment().empty());

							   auto& arr = *tbl.get("arr");
							   REQUIRE(arr.leading_comments().size() == 1u);
							   CHECK(arr.leading_comments()[0] == "nospace"sv);
							   CHECK(arr.trailing_comment().empty());

							   auto& sub = *tbl.get("tbl");
							   REQUIRE(sub.leading_comments().size() == 1u);
							   CHECK(sub.leading_comments()[0] == "about table"sv);
							   CHECK(sub.trailing_comment() == "trailing table"sv);
							   CHECK(sub.as_table()->get("nested")->leading_comments()[0] == "about nested"sv);

							   auto& aot = (*tbl.get_as<array>("aot"))[0];
							   REQUIRE(aot.leading_comments().size() == 1u);
							   CHECK(aot.leading_comments()[0] == "about array of tables"sv);
							   CHECK(aot.trailing_comment() == "trailing aot"sv);
						   });
}

TEST_CASE("comments - copying")
{
	table tbl;
	tbl.emplace<int64_t>("a", 1);
	tbl.get("a")->leading_comments().push_back("lead");
	tbl.get("a")->trailing_comment() = "trail";

	const table copy{ tbl };
	CHECK(copy.get("a")->leading_comments() == std::vector<std::string>{ "lead" });
	CHECK(copy.get("a")->trailing_comment() == "trail"sv);

	table moved{ std::move(tbl) };
	CHECK(moved.get("a")->trailing_comment() == "trail"sv);
}

TEST_CASE("comments - printing")
{
	static constexpr auto toml_text = R"(# about a
a = 1 # trailing a

# about tbl
[tbl] # trailing tbl
# about b
b = 2

# about aot
[[aot]] # trailing aot
c = 3)"sv;

	// round trip
	auto result = toml::parse(toml_text);
	std::stringstream ss;
	ss << result;
	CHECK(ss.str() == toml_text);

	// comments set programmatically
	table tbl;
	tbl.emplace<int64_t>("x", 1);
	tbl.get("x")->leading_comments().push_back("first\nsecond");
	std::stringstream ss2;
	ss2 << tbl;
	CHECK(ss2.str() == "# first second\nx = 1"sv);
}

#endif // TOML_ENABLE_COMMENTS
