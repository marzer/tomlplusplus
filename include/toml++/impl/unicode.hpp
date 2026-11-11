//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.hpp"
#include "header_start.hpp"
/// \cond

TOML_IMPL_NAMESPACE_START
{
	TOML_CONST_GETTER
	constexpr bool is_string_delimiter(char32_t c) noexcept
	{
		return c == U'"' || c == U'\'';
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_letter(char32_t c) noexcept
	{
		return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
	}

	TOML_CONST_GETTER
	constexpr bool is_binary_digit(char32_t c) noexcept
	{
		return c == U'0' || c == U'1';
	}

	TOML_CONST_GETTER
	constexpr bool is_octal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'7';
	}

	TOML_CONST_GETTER
	constexpr bool is_decimal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'9';
	}

	TOML_CONST_GETTER
	constexpr bool is_hexadecimal_digit(char32_t c) noexcept
	{
		return c >= U'0' && c <= U'f' && (1ull << (static_cast<uint_least64_t>(c) - 0x30u)) & 0x7E0000007E03FFull;
	}

	template <typename T>
	TOML_CONST_GETTER
	constexpr uint_least32_t hex_to_dec(const T c) noexcept
	{
		if constexpr (std::is_same_v<T, uint_least32_t>)
			return c >= 0x41u					 // >= 'A'
					 ? 10u + (c | 0x20u) - 0x61u // - 'a'
					 : c - 0x30u				 // - '0'
				;
		else
			return hex_to_dec(static_cast<uint_least32_t>(c));
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		return c == U'\t' || c == U' ';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_horizontal_whitespace(char32_t c) noexcept
	{
		if (c < U'\u00A0')
			return false;

		return c == U'\u00A0' || c == U'\u1680' || (c >= U'\u2000' && c <= U'\u200A') || c == U'\u202F'
			|| c == U'\u205F' || c == U'\u3000';
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return c >= U'\n' && c <= U'\r';
	}

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_vertical_whitespace(char32_t c) noexcept
	{
		return c == U'\u0085' || c == U'\u2028' || c == U'\u2029';
	}

	TOML_CONST_GETTER
	constexpr bool is_horizontal_whitespace(char32_t c) noexcept
	{
		return is_ascii_horizontal_whitespace(c) || is_non_ascii_horizontal_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_vertical_whitespace(char32_t c) noexcept
	{
		return is_ascii_vertical_whitespace(c) || is_non_ascii_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_whitespace(char32_t c) noexcept
	{
		return is_horizontal_whitespace(c) || is_vertical_whitespace(c);
	}

	TOML_CONST_GETTER
	constexpr bool is_default_ignorable(char32_t c) noexcept
	{
		if (c < U'\u00AD')
			return false;

		return c == U'\u00AD' || c == U'\u034F' || c == U'\u061C' || (c >= U'\u115F' && c <= U'\u1160')
			|| (c >= U'\u17B4' && c <= U'\u17B5') || (c >= U'\u180B' && c <= U'\u180F')
			|| (c >= U'\u200B' && c <= U'\u200F') || (c >= U'\u202A' && c <= U'\u202E')
			|| (c >= U'\u2060' && c <= U'\u206F') || c == U'\u3164' || (c >= U'\uFE00' && c <= U'\uFE0F')
			|| c == U'\uFEFF' || c == U'\uFFA0' || (c >= U'\uFFF0' && c <= U'\uFFF8')
			|| (c >= U'\U0001BCA0' && c <= U'\U0001BCA3') || (c >= U'\U0001D173' && c <= U'\U0001D17A')
			|| (c >= U'\U000E0000' && c <= U'\U000E0FFF');
	}

	TOML_CONST_GETTER
	constexpr bool is_ascii_bare_key_character(char32_t c) noexcept
	{
		return is_ascii_letter(c) || is_decimal_digit(c) || c == U'-' || c == U'_'
#if TOML_LANG_UNRELEASED // toml/issues/644 ('+' in bare keys)
			|| c == U'+'
#endif
			;
	}

#if TOML_LANG_UNRELEASED // toml/pull/891 (unicode bare keys)

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_bare_key_character(char32_t c) noexcept
	{
		if (c < U'\u00B2')
			return false;

		return c == U'\u00B2' || c == U'\u00B3' || c == U'\u00B9' || (c >= U'\u00BC' && c <= U'\u00BE')
			|| (c >= U'\u00C0' && c <= U'\u00D6') || (c >= U'\u00D8' && c <= U'\u00F6')
			|| (c >= U'\u00F8' && c <= U'\u037D') || (c >= U'\u037F' && c <= U'\u1FFF')
			|| (c >= U'\u200C' && c <= U'\u200D') || (c >= U'\u203F' && c <= U'\u2040')
			|| (c >= U'\u2070' && c <= U'\u218F') || (c >= U'\u2460' && c <= U'\u24FF')
			|| (c >= U'\u2C00' && c <= U'\u2FEF') || (c >= U'\u3001' && c <= U'\uD7FF')
			|| (c >= U'\uF900' && c <= U'\uFDCF') || (c >= U'\uFDF0' && c <= U'\uFFFD')
			|| (c >= U'\U00010000' && c <= U'\U000EFFFF');
	}

#endif

	TOML_CONST_GETTER
	constexpr bool is_bare_key_character(char32_t c) noexcept
	{
		return is_ascii_bare_key_character(c)
#if TOML_LANG_UNRELEASED // toml/pull/891 (unicode bare keys)
			|| is_non_ascii_bare_key_character(c)
#endif
			;
	}

	TOML_CONST_GETTER
	constexpr bool is_value_terminator(char32_t c) noexcept
	{
		return is_whitespace(c) || c == U']' || c == U'}' || c == U',' || c == U'#';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char c) noexcept
	{
		return static_cast<unsigned char>(c) <= 0x1Fu || c == '\x7F';
	}

	TOML_CONST_GETTER
	constexpr bool is_control_character(char32_t c) noexcept
	{
		return c <= U'\u001F' || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_nontab_control_character(char32_t c) noexcept
	{
		return c <= U'\u0008' || (c >= U'\u000A' && c <= U'\u001F') || c == U'\u007F';
	}

	TOML_CONST_GETTER
	constexpr bool is_unicode_surrogate(char32_t c) noexcept
	{
		return c >= 0xD800u && c <= 0xDFFFu;
	}

	struct utf8_decoder
	{
		// utf8_decoder based on this: https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
		// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

		uint_least32_t state{};
		char32_t codepoint{};

		static constexpr uint8_t state_table[]{
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
			0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,
			1,	1,	1,	1,	1,	1,	1,	1,	1,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	7,	7,
			7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,	7,
			7,	7,	7,	8,	8,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
			2,	2,	2,	2,	2,	2,	2,	2,	10, 3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	4,	3,	3,	11, 6,	6,
			6,	5,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,	8,

			0,	12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 0,	12,
			12, 12, 12, 12, 0,	12, 0,	12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12,
			12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36, 12,
			36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12
		};

		TOML_PURE_INLINE_GETTER
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool has_codepoint() const noexcept
		{
			return state == uint_least32_t{};
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool needs_more_input() const noexcept
		{
			return !has_codepoint() && !error();
		}

		constexpr void operator()(uint8_t byte) noexcept
		{
			TOML_ASSERT_ASSUME(!error());

			const auto type = state_table[byte];

			codepoint = static_cast<char32_t>(has_codepoint() ? (uint_least32_t{ 255u } >> type) & byte
															  : (byte & uint_least32_t{ 63u })
																	| (static_cast<uint_least32_t>(codepoint) << 6));

			state = state_table[state + uint_least32_t{ 256u } + type];
		}

		TOML_ALWAYS_INLINE
		constexpr void operator()(char c) noexcept
		{
			operator()(static_cast<uint8_t>(c));
		}

		TOML_ALWAYS_INLINE
		constexpr void reset() noexcept
		{
			state = {};
		}
	};

	TOML_PURE_GETTER
	TOML_ATTR(nonnull)
	bool is_ascii(const char* str, size_t len) noexcept;
}
TOML_IMPL_NAMESPACE_END;

/// \endcond
#include "header_end.hpp"
