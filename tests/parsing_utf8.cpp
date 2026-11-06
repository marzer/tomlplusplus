// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	constexpr auto doc_prefix = "# \xC3\xA9\na = \""sv;
	constexpr auto invalid	  = "2:6: Encountered invalid utf-8 sequence"sv;
	constexpr auto cut_short  = "2:6: Encountered EOF during incomplete utf-8 code point sequence"sv;

	std::string bytes(std::initializer_list<unsigned> values)
	{
		std::string out;
		for (const auto v : values)
			out += static_cast<char>(v);
		return out;
	}

	std::string hex(std::string_view seq)
	{
		static constexpr auto digits = "0123456789ABCDEF"sv;
		std::string out;
		for (const auto c : seq)
		{
			const auto byte = static_cast<unsigned char>(c);
			out += "\\x";
			out += digits[byte >> 4];
			out += digits[byte & 0xFu];
		}
		return out;
	}

	void valid_in_string(std::string_view seq)
	{
		INFO("sequence " << hex(seq));
		parsing_should_succeed(FILE_LINE_ARGS,
							   std::string{ doc_prefix }.append(seq).append("\""),
							   [=](table&& tbl) { CHECK(tbl["a"] == seq); });
	}

	void invalid_in_string(std::string_view seq)
	{
		INFO("sequence " << hex(seq));
		parsing_should_fail(FILE_LINE_ARGS, std::string{ doc_prefix }.append(seq).append("\""), invalid);
	}

	void at_eof(std::string_view seq, std::string_view expected)
	{
		INFO("sequence " << hex(seq));
		parsing_should_fail(FILE_LINE_ARGS, std::string{ doc_prefix }.append(seq), expected);
	}
}

TEST_CASE("parsing - utf-8 (bytes that can't start a sequence)")
{
	for (const auto lead : { 0x80u, 0x8Fu, 0x90u, 0x9Fu, 0xA0u, 0xBFu, 0xC0u, 0xC1u, 0xF5u, 0xFFu })
	{
		invalid_in_string(bytes({ lead }));
		invalid_in_string(bytes({ lead, 0x80u, 0x80u, 0x80u }));
		at_eof(bytes({ lead }), invalid);
	}
}

TEST_CASE("parsing - utf-8 (two-byte sequences)")
{
	for (const auto lead : { 0xC2u, 0xDFu })
	{
		for (const auto second : { 0x00u, 0x7Fu, 0x80u, 0x8Fu, 0x90u, 0x9Fu, 0xA0u, 0xBFu, 0xC0u, 0xC1u, 0xC2u,
								   0xDFu, 0xE0u, 0xE1u, 0xEDu, 0xEFu, 0xF0u, 0xF1u, 0xF4u, 0xF5u, 0xFFu })
		{
			if (second >= 0x80u && second <= 0xBFu)
				valid_in_string(bytes({ lead, second }));
			else
				invalid_in_string(bytes({ lead, second }));
		}
		at_eof(bytes({ lead }), cut_short);
	}
}

TEST_CASE("parsing - utf-8 (three- and four-byte sequences)")
{
	struct lead_byte
	{
		unsigned value;
		unsigned second_min;
		unsigned second_max;
		size_t length;
	};

	for (const auto& lead : { lead_byte{ 0xE0u, 0xA0u, 0xBFu, 3u },
							  lead_byte{ 0xE1u, 0x80u, 0xBFu, 3u },
							  lead_byte{ 0xECu, 0x80u, 0xBFu, 3u },
							  lead_byte{ 0xEDu, 0x80u, 0x9Fu, 3u },
							  lead_byte{ 0xEEu, 0x80u, 0xBFu, 3u },
							  lead_byte{ 0xEFu, 0x80u, 0xBFu, 3u },
							  lead_byte{ 0xF0u, 0x90u, 0xBFu, 4u },
							  lead_byte{ 0xF1u, 0x80u, 0xBFu, 4u },
							  lead_byte{ 0xF3u, 0x80u, 0xBFu, 4u },
							  lead_byte{ 0xF4u, 0x80u, 0x8Fu, 4u } })
	{
		std::string seq(lead.length, '\x80');
		seq[0] = static_cast<char>(lead.value);
		seq[1] = static_cast<char>(lead.second_min);
		valid_in_string(seq);

		for (size_t i = 1; i < lead.length; i++)
		{
			const auto min = i == 1u ? lead.second_min : 0x80u;
			const auto max = i == 1u ? lead.second_max : 0xBFu;
			for (const auto byte : { min - 1u, min, max, max + 1u })
			{
				auto changed = seq;
				changed[i]	 = static_cast<char>(byte);
				if (byte >= min && byte <= max)
					valid_in_string(changed);
				else
					invalid_in_string(changed);
			}
		}

		for (size_t length = 1; length < lead.length; length++)
			at_eof(std::string_view{ seq }.substr(0, length), cut_short);
	}
}

TEST_CASE("parsing - utf-8 (the first error in the document wins)")
{
	static constexpr auto no_type = "1:5: Error while parsing value: could not determine value type"sv;

	SECTION("an error before an invalid sequence")
	{
		parsing_should_fail(FILE_LINE_ARGS, "a = ; \xFF"sv, no_type);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1__2\xFF"sv,
							"1:7: Error while parsing decimal integer: underscores may only follow digits"sv);
		parsing_should_fail(
			FILE_LINE_ARGS,
			"a = 'x\x01\xFF'"sv,
			"1:7: Error while parsing literal string: control characters other than TAB (U+0009) are explicitly prohibited"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 2024-01-01 1\xFF"sv,
							"1:16: Error while parsing key-value pair: expected a comment or whitespace, saw '1'"sv);
	}

	SECTION("an error found only after landing on an invalid sequence")
	{
		parsing_should_fail(FILE_LINE_ARGS, "a = 07\xFF"sv, "1:7: Encountered invalid utf-8 sequence"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 9223372036854775808\xFF"sv,
							"1:24: Encountered invalid utf-8 sequence"sv);
		parsing_should_fail(FILE_LINE_ARGS,
							"t = 1979-05-27T07:32:60\xFF"sv,
							"1:24: Encountered invalid utf-8 sequence"sv);
		parsing_should_fail(FILE_LINE_ARGS, "[a]\n[a]\xFF"sv, "2:4: Encountered invalid utf-8 sequence"sv);
	}

	SECTION("an error before a sequence cut short")
	{
		parsing_should_fail(FILE_LINE_ARGS, "a = ; \xE2\x82"sv, no_type);
		parsing_should_fail(FILE_LINE_ARGS,
							"a = 1__2\xE2"sv,
							"1:7: Error while parsing decimal integer: underscores may only follow digits"sv);
	}

	SECTION("an invalid sequence before an error")
	{
		parsing_should_fail(FILE_LINE_ARGS, "a = '\xFF' ;"sv, "1:6: Encountered invalid utf-8 sequence"sv);
		parsing_should_fail(FILE_LINE_ARGS, "a = ''\xFF ;"sv, "1:7: Encountered invalid utf-8 sequence"sv);
		parsing_should_fail(FILE_LINE_ARGS, "a = 2024-01-01 \xFF ;"sv, "1:16: Encountered invalid utf-8 sequence"sv);
	}
}
