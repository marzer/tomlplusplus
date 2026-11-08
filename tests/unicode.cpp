// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include <cstdio>

namespace
{
	struct code_point_range
	{
		uint32_t first;
		uint32_t last;
	};

	// between them, White_Space in Unicode 17.0's PropList.txt
	constexpr code_point_range horizontal_whitespace[] = {
		{ 0x0009, 0x0009 }, { 0x0020, 0x0020 }, { 0x00A0, 0x00A0 }, { 0x1680, 0x1680 },
		{ 0x2000, 0x200A }, { 0x202F, 0x202F }, { 0x205F, 0x205F }, { 0x3000, 0x3000 },
	};
	constexpr code_point_range vertical_whitespace[] = { { 0x000A, 0x000D }, { 0x0085, 0x0085 }, { 0x2028, 0x2029 } };

	// Default_Ignorable_Code_Point in Unicode 17.0's DerivedCoreProperties.txt
	constexpr code_point_range default_ignorable[] = {
		{ 0x00AD, 0x00AD },	  { 0x034F, 0x034F },	{ 0x061C, 0x061C },	  { 0x115F, 0x1160 },	{ 0x17B4, 0x17B5 },
		{ 0x180B, 0x180D },	  { 0x180E, 0x180E },	{ 0x180F, 0x180F },	  { 0x200B, 0x200F },	{ 0x202A, 0x202E },
		{ 0x2060, 0x2064 },	  { 0x2065, 0x2065 },	{ 0x2066, 0x206F },	  { 0x3164, 0x3164 },	{ 0xFE00, 0xFE0F },
		{ 0xFEFF, 0xFEFF },	  { 0xFFA0, 0xFFA0 },	{ 0xFFF0, 0xFFF8 },	  { 0x1BCA0, 0x1BCA3 }, { 0x1D173, 0x1D17A },
		{ 0xE0000, 0xE0000 }, { 0xE0001, 0xE0001 }, { 0xE0002, 0xE001F }, { 0xE0020, 0xE007F }, { 0xE0080, 0xE00FF },
		{ 0xE0100, 0xE01EF }, { 0xE01F0, 0xE0FFF },
	};

	constexpr code_point_range bare_key_characters[] = {
		{ 0x0030, 0x0039 },
		{ 0x0041, 0x005A },
		{ 0x0061, 0x007A },
		{ 0x002D, 0x002D },
		{ 0x005F, 0x005F },
#if TOML_LANG_UNRELEASED
		{ 0x002B, 0x002B }, // toml/issues/644

		// toml/pull/891
		{ 0x00B2, 0x00B2 },
		{ 0x00B3, 0x00B3 },
		{ 0x00B9, 0x00B9 },
		{ 0x00BC, 0x00BE },
		{ 0x00C0, 0x00D6 },
		{ 0x00D8, 0x00F6 },
		{ 0x00F8, 0x037D },
		{ 0x037F, 0x1FFF },
		{ 0x200C, 0x200D },
		{ 0x203F, 0x2040 },
		{ 0x2070, 0x218F },
		{ 0x2460, 0x24FF },
		{ 0x2C00, 0x2FEF },
		{ 0x3001, 0xD7FF },
		{ 0xF900, 0xFDCF },
		{ 0xFDF0, 0xFFFD },
		{ 0x10000, 0xEFFFF },
#endif
	};

	template <size_t N>
	std::vector<unsigned char> code_points_in(const code_point_range (&ranges)[N])
	{
		std::vector<unsigned char> in(0x110000u);
		for (const auto& range : ranges)
			for (auto cp = range.first; cp <= range.last; cp++)
				in[cp] = 1u;
		return in;
	}

	template <typename Func, typename Expected>
	void check_every_code_point(std::string_view name, Func&& func, Expected&& expected)
	{
		uint32_t mismatches = {};
		uint32_t first		= {};
		for (uint32_t cp = 0; cp < 0x110000u; cp++)
			if (func(static_cast<char32_t>(cp)) != expected(cp) && !mismatches++)
				first = cp;

		char first_str[16];
		std::snprintf(first_str, sizeof(first_str), "U+%04X", static_cast<unsigned>(first));
		INFO(name << " is wrong for " << mismatches << " code points, the first being " << first_str);
		CHECK(mismatches == 0u);
	}
}

TEST_CASE("unicode - classifying every code point")
{
	const auto h = code_points_in(horizontal_whitespace);
	const auto v = code_points_in(vertical_whitespace);
	const auto k = code_points_in(bare_key_characters);

	check_every_code_point("is_ascii_horizontal_whitespace",
						   impl::is_ascii_horizontal_whitespace,
						   [&](uint32_t cp) { return cp < 0x80u && h[cp]; });
	check_every_code_point("is_non_ascii_horizontal_whitespace",
						   impl::is_non_ascii_horizontal_whitespace,
						   [&](uint32_t cp) { return cp >= 0x80u && h[cp]; });
	check_every_code_point("is_horizontal_whitespace",
						   impl::is_horizontal_whitespace,
						   [&](uint32_t cp) { return h[cp] != 0u; });

	check_every_code_point("is_ascii_vertical_whitespace",
						   impl::is_ascii_vertical_whitespace,
						   [&](uint32_t cp) { return cp < 0x80u && v[cp]; });
	check_every_code_point("is_non_ascii_vertical_whitespace",
						   impl::is_non_ascii_vertical_whitespace,
						   [&](uint32_t cp) { return cp >= 0x80u && v[cp]; });
	check_every_code_point("is_vertical_whitespace",
						   impl::is_vertical_whitespace,
						   [&](uint32_t cp) { return v[cp] != 0u; });

	check_every_code_point("is_whitespace", impl::is_whitespace, [&](uint32_t cp) { return h[cp] || v[cp]; });

	const auto d = code_points_in(default_ignorable);
	check_every_code_point("is_default_ignorable",
						   impl::is_default_ignorable,
						   [&](uint32_t cp) { return d[cp] != 0u; });

	check_every_code_point("is_ascii_bare_key_character",
						   impl::is_ascii_bare_key_character,
						   [&](uint32_t cp) { return cp < 0x80u && k[cp]; });
#if TOML_LANG_UNRELEASED
	check_every_code_point("is_non_ascii_bare_key_character",
						   impl::is_non_ascii_bare_key_character,
						   [&](uint32_t cp) { return cp >= 0x80u && k[cp]; });
#endif
	check_every_code_point("is_bare_key_character",
						   impl::is_bare_key_character,
						   [&](uint32_t cp) { return k[cp] != 0u; });
}
