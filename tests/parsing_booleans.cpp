// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

TEST_CASE("parsing - booleans")
{
	parsing_should_succeed(FILE_LINE_ARGS,
						   R"(
								bool1 = true
								bool2 = false
							)"sv,
						   [](table&& tbl)
						   {
							   CHECK(tbl["bool1"] == true);
							   CHECK(tbl["bool2"] == false);
						   });

	parse_expected_value(FILE_LINE_ARGS, " true", true);
	parse_expected_value(FILE_LINE_ARGS, "false", false);
}

TEST_CASE("parsing - booleans (always lowercase)")
{
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = True"sv,
						"1:8: Error while parsing boolean: expected 'true', saw 'T'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = TRUE"sv,
						"1:8: Error while parsing boolean: expected 'true', saw 'T'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = tRUE"sv,
						"1:9: Error while parsing boolean: expected 'true', saw 'tR'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = False"sv,
						"1:8: Error while parsing boolean: expected 'false', saw 'F'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = FALSE"sv,
						"1:8: Error while parsing boolean: expected 'false', saw 'F'"sv);
	parsing_should_fail(FILE_LINE_ARGS,
						"bool = fALSE"sv,
						"1:9: Error while parsing boolean: expected 'false', saw 'fA'"sv);
}
