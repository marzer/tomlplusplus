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

TEST_CASE("parsing - comments (every ascii byte after a long comment)")
{
	std::string accented;
	for (size_t i = 0; i < 40u; i++)
		accented += "\xC3\xA9";

	for (const auto& filler : { std::string(40u, 'x'), accented })
	{
		for (unsigned byte = 0; byte < 0x80u; byte++)
		{
			INFO("byte " << byte);
			const auto c   = static_cast<char>(byte);
			const auto doc = "# " + filler + c + "\na = 1";
			if (c == '\v' || c == '\f')
				parsing_should_fail(
					FILE_LINE_ARGS,
					doc,
					R"(1:43: Error while parsing comment: vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);
			else if (c != '\t' && c != '\n' && c != '\r' && (byte < 0x20u || byte == 0x7Fu))
				parsing_should_fail(
					FILE_LINE_ARGS,
					doc,
					"1:43: Error while parsing comment: control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);
			else
				parsing_should_succeed(FILE_LINE_ARGS, doc, [](table&& tbl) { CHECK(tbl["a"] == 1); });
		}
	}
}

TEST_CASE("parsing - comments (bytes within a 16-byte block)")
{
	const auto check = [](const std::string& filler, size_t columns, char c, std::string_view after)
	{
		INFO("byte " << static_cast<unsigned>(c) << " after " << filler.length() << " bytes");
		const auto doc = "# " + filler + c + std::string{ after };
		const auto at  = "1:" + std::to_string(3u + columns) + ": Error while parsing comment: ";
		if (c == '\v' || c == '\f')
			parsing_should_fail(FILE_LINE_ARGS,
								doc,
								at + R"(vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)");
		else if (c != '\t' && c != '\n' && c != '\r' && (static_cast<unsigned char>(c) < 0x20u || c == '\x7F'))
			parsing_should_fail(
				FILE_LINE_ARGS,
				doc,
				at + "control characters other than TAB (U+0009) are explicitly prohibited in comments");
		else
			parsing_should_succeed(FILE_LINE_ARGS, doc, [](table&& tbl) { CHECK(tbl["a"] == 1); });
	};

	static constexpr auto next_lines = "\n#yyyyyyyyyyyyyyyy\na = 1"sv;
	for (unsigned byte = 0; byte < 0x80u; byte++)
	{
		check(std::string(5u, 'x'), 5u, static_cast<char>(byte), next_lines);
		check("\xC3\xA9" + std::string(3u, 'x'), 4u, static_cast<char>(byte), next_lines);
	}

	static constexpr auto accented = "\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9"sv;
	for (size_t length = 0; length < 18u; length++)
	{
		for (const char c : { '\x01', '\x7F' })
		{
			check(std::string(length, 'x'), length, c, accented);
			if (length >= 2u)
				check("\xC3\xA9" + std::string(length - 2u, 'x'), length - 1u, c, accented);
		}
	}
}
