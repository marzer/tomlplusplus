// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"
using namespace toml::impl;

using func_type = bool(char32_t)noexcept;
inline constexpr func_type* funcs[] =
{
	// these must be mutually-exclusive

	impl::is_ascii_letter,
	impl::is_ascii_whitespace,
	impl::is_ascii_line_break<true>,
	impl::is_decimal_digit,
	impl::is_string_delimiter,
	impl::is_non_ascii_whitespace,
	impl::is_non_ascii_line_break,
	impl::is_unicode_surrogate,
	#if TOML_LANG_UNRELEASED
	impl::is_non_ascii_letter,
	impl::is_non_ascii_number,
	impl::is_combining_mark,
	#endif
};

template <typename T>
inline bool in_only(func_type* fptr, T cp) noexcept
{
	if (!fptr(static_cast<char32_t>(cp)))
		return false;
	for (auto fn : funcs)
	{
		if (fn == fptr)
			continue;
		if (fn(static_cast<char32_t>(cp)))
			return false;
	}
	return true;
}

inline constexpr uint32_t unimax = 0x10FFFFu;

struct codepoint_range
{
	char32_t first;
	char32_t last;

	template <typename T, typename U>
	TOML_NODISCARD_CTOR
	constexpr codepoint_range(T first_, U last_) noexcept
		: first{ static_cast<char32_t>(first_) },
		last{ static_cast<char32_t>(last_) }
	{
		if (last < first)
			std::swap(first, last);
	}

	template <typename T>
	TOML_NODISCARD_CTOR
	constexpr codepoint_range(T first_) noexcept
		: first{ static_cast<char32_t>(first_) },
		last{ first }
	{}
};

inline bool in(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (!fptr(cp))
			return false;
	return true;
}

inline bool in_only(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (!in_only(fptr, cp))
			return false;
	return true;
}

inline bool not_in(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (fptr(cp))
			return false;
	return true;
}

TEST_CASE("unicode - is_ascii_letter")
{
	static constexpr auto fn = is_ascii_letter;
	REQUIRE(not_in(fn,	{	U'\0',		U'@'		}));
	REQUIRE(in_only(fn,	{	U'A',		U'Z'		}));
	REQUIRE(not_in(fn,	{	U'[',		U'`'		}));
	REQUIRE(in_only(fn,	{	U'a',		U'z'		}));
	REQUIRE(not_in(fn,	{	U'{',		unimax		}));
}

TEST_CASE("unicode - is_ascii_whitespace")
{
	static constexpr auto fn = is_ascii_whitespace;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0008'	}));
	REQUIRE(in_only(fn,		U'\t'					));
	REQUIRE(not_in(fn,	{	U'\n',		U'\u001F'	}));
	REQUIRE(in_only(fn,		U' '					));
	REQUIRE(not_in(fn,	{	U'!',		unimax		}));
}

TEST_CASE("unicode - is_ascii_line_break")
{
	static constexpr auto fn = is_ascii_line_break<true>;
	REQUIRE(not_in(fn,	{	U'\0',		U'\t'		}));
	REQUIRE(in_only(fn,	{	U'\n',		U'\r'		}));
	REQUIRE(not_in(fn,	{	U'\u000E',	unimax		}));
}

TEST_CASE("unicode - is_binary_digit")
{
	static constexpr auto fn = is_binary_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'1'		}));
	REQUIRE(not_in(fn,	{	U'2',		unimax		}));
}

TEST_CASE("unicode - is_octal_digit")
{
	static constexpr auto fn = is_octal_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'7'		}));
	REQUIRE(not_in(fn,	{	U'8',		unimax		}));
}

TEST_CASE("unicode - is_decimal_digit")
{
	static constexpr auto fn = is_decimal_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'9'		}));
	REQUIRE(not_in(fn,	{	U':',		unimax		}));
}

TEST_CASE("unicode - hex_to_dec")
{
	REQUIRE(hex_to_dec(U'0') == 0x0u);
	REQUIRE(hex_to_dec(U'1') == 0x1u);
	REQUIRE(hex_to_dec(U'2') == 0x2u);
	REQUIRE(hex_to_dec(U'3') == 0x3u);
	REQUIRE(hex_to_dec(U'4') == 0x4u);
	REQUIRE(hex_to_dec(U'5') == 0x5u);
	REQUIRE(hex_to_dec(U'6') == 0x6u);
	REQUIRE(hex_to_dec(U'7') == 0x7u);
	REQUIRE(hex_to_dec(U'8') == 0x8u);
	REQUIRE(hex_to_dec(U'9') == 0x9u);
	REQUIRE(hex_to_dec(U'A') == 0xAu);
	REQUIRE(hex_to_dec(U'B') == 0xBu);
	REQUIRE(hex_to_dec(U'C') == 0xCu);
	REQUIRE(hex_to_dec(U'D') == 0xDu);
	REQUIRE(hex_to_dec(U'E') == 0xEu);
	REQUIRE(hex_to_dec(U'F') == 0xFu);
	REQUIRE(hex_to_dec(U'a') == 0xau);
	REQUIRE(hex_to_dec(U'b') == 0xbu);
	REQUIRE(hex_to_dec(U'c') == 0xcu);
	REQUIRE(hex_to_dec(U'd') == 0xdu);
	REQUIRE(hex_to_dec(U'e') == 0xeu);
	REQUIRE(hex_to_dec(U'f') == 0xfu);
}

TEST_CASE("unicode - is_string_delimiter")
{
	static constexpr auto fn = is_string_delimiter;
	REQUIRE(not_in(fn,	{	U'\0',		U'!'		}));
	REQUIRE(in_only(fn,		U'"'					));
	REQUIRE(not_in(fn,	{	U'#',		U'&'		}));
	REQUIRE(in_only(fn,		U'\''					));
	REQUIRE(not_in(fn,	{	U'(',		unimax		}));
}

TEST_CASE("unicode - is_non_ascii_whitespace")
{
	static constexpr auto fn = is_non_ascii_whitespace;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u009F'	}));
	REQUIRE(in_only(fn,		U'\u00A0'				));
	REQUIRE(not_in(fn,	{	U'\u00A1',	U'\u167F'	}));
	REQUIRE(in_only(fn,		U'\u1680'				));
	REQUIRE(not_in(fn,	{	U'\u1681',	U'\u1FFF'	}));
	REQUIRE(in_only(fn,	{	U'\u2000',	U'\u200A'	}));
	REQUIRE(not_in(fn,	{	U'\u200B',	U'\u202E'	}));
	REQUIRE(in_only(fn,		U'\u202F'				));
	REQUIRE(not_in(fn,	{	U'\u2030',	U'\u205E'	}));
	REQUIRE(in_only(fn,		U'\u205F'				));
	REQUIRE(not_in(fn,	{	U'\u2060',	U'\u2FFF'	}));
	REQUIRE(in_only(fn,		U'\u3000'				));
	REQUIRE(not_in(fn,	{	U'\u3001',	unimax		}));
}

TEST_CASE("unicode - is_non_ascii_line_break")
{
	static constexpr auto fn = is_non_ascii_line_break;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0084'	}));
	REQUIRE(in_only(fn,		U'\u0085'				));
	REQUIRE(not_in(fn,	{	U'\u0086',	U'\u2027'	}));
	REQUIRE(in_only(fn,	{	U'\u2028',	U'\u2029'	}));
	REQUIRE(not_in(fn,	{	U'\u202A',	unimax		}));
}

TEST_CASE("unicode - is_unicode_surrogate")
{
	static constexpr auto fn = is_unicode_surrogate;
	REQUIRE(not_in(fn,	{	U'\0',		0xD7FFu		}));
	REQUIRE(in_only(fn,	{	0xD800u,	0xDFFF		}));
	REQUIRE(not_in(fn,	{	0xE000,		unimax		}));
}
