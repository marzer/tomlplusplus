// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

TEST_CASE("parsing - comments")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								# This is a full-line comment
								key = "value"  # This is a comment at the end of a line
								another = "# This is not a comment"
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl.size() == 2);
							   CHECK(tbl["key"] == "value"sv);
							   CHECK(tbl["another"] == "# This is not a comment"sv);
						   });

	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(# this = "looks like a KVP but is commented out)"sv,
						   [](table&& tbl) { CHECK(tbl.size() == 0); });
}

TEST_CASE("parsing - comments (control characters)")
{
	// toml/issues/567 (disallow non-TAB control characters in comments)
	parsing_should_fail(
		FILE_LINE_ARGS,
		"# \u0000 some trailing garbage"sv,
		"1:3: Error while parsing comment: control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);
	for (unsigned cp = 0; cp <= 0x7Fu; cp++)
	{
		if ((cp >= 0x20u && cp < 0x7Fu) || cp == '\t' || cp == '\n' || cp == '\v' || cp == '\f' || cp == '\r')
			continue;

		CAPTURE(cp);
		const char input[] = { '#', ' ', static_cast<char>(cp) };
		parsing_should_fail(
			FILE_LINE_ARGS,
			std::string_view{ input, sizeof(input) },
			"1:3: Error while parsing comment: control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);
	}
	parsing_should_fail(
		FILE_LINE_ARGS,
		"# \v"sv,
		R"(1:3: Error while parsing comment: vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);
	parsing_should_fail(
		FILE_LINE_ARGS,
		"# \f"sv,
		R"(1:3: Error while parsing comment: vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);
}
