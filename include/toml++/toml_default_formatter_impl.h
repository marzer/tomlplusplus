//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_default_formatter.h"
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif

namespace toml::impl
{
	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	toml::string default_formatter_make_key_segment(const toml::string& str) noexcept
	{
		if (str.empty())
			return TOML_STRING_PREFIX("''"s);
		else
		{
			bool requiresQuotes = false;
			{
				impl::utf8_decoder decoder;
				for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
				{
					decoder(static_cast<uint8_t>(str[i]));
					if (decoder.error())
						requiresQuotes = true;
					else if (decoder.has_code_point())
						requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
				}
			}

			if (requiresQuotes)
			{
				toml::string s;
				s.reserve(str.length() + 2_sz);
				s += TOML_STRING_PREFIX('"');
				for (auto c : str)
				{
					if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
						s.append(low_character_escape_table[c]);
					else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
						s.append(TOML_STRING_PREFIX("\\u007F"sv));
					else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
						s.append(TOML_STRING_PREFIX("\\\""sv));
					else
						s += c;
				}
				s += TOML_STRING_PREFIX('"');
				return s;
			}
			else
				return str;
		}
	}

	TOML_POP_WARNINGS

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	size_t default_formatter_inline_columns(const node& node) noexcept
	{
		return node.visit([](const auto& n) noexcept
			-> size_t
			{
				if constexpr (is_table<decltype(n)>)
				{
					if (n.empty())
						return 2_sz; // "{}"
					size_t weight = 3_sz; // "{ }"
					for (auto [k, v] : n)
						weight += k.length() + default_formatter_inline_columns(v) + 2_sz; // +  ", "
					return weight;
				}
				else if constexpr (is_array<decltype(n)>)
				{
					if (n.empty())
						return 2_sz; // "[]"
					size_t weight = 3_sz; // "[ ]"
					for (auto& elem : n)
						weight += default_formatter_inline_columns(elem) + 2_sz; // +  ", "
					return weight;
				}
				else if constexpr (is_string<decltype(n)>)
				{
					return n.get().length() + 2_sz; // + ""
				}
				else if constexpr (is_number<decltype(n)>)
				{
					static constexpr auto digit_count = [](auto num) noexcept
						-> size_t
					{
						using number_t = decltype(num);
						size_t digits = 1_sz;
						while (num >= number_t{ 10 })
						{
							num /= number_t{ 10 };
							digits++;
						}
						return digits;
					};

					if constexpr (is_integer<decltype(n)>)
					{
						auto v = n.get();
						if (!v)
							return 1_sz;
						size_t weight = {};
						if (v < 0)
						{
							weight += 1;
							v *= -1;
						}
						return weight + digit_count(v);
					}
					else if constexpr (is_floating_point<decltype(n)>)
					{
						auto v = n.get();
						if (v == 0.0)
							return 3_sz;
						size_t weight = 2_sz; // ".0"
						if (v < 0.0)
						{
							weight += 1;
							v *= -1.0;
						}
						return weight + digit_count(v);
					}
				}
				else if constexpr (is_boolean<decltype(n)>)
				{
					return 5_sz;
				}
				else if constexpr (is_date<decltype(n)> || is_time<decltype(n)>)
				{
					return 10_sz;
				}
				else if constexpr (is_date_time<decltype(n)>)
				{
					return 30_sz;
				}
				TOML_UNREACHABLE;
			});
	}

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias) noexcept
	{
		return (default_formatter_inline_columns(node) + starting_column_bias) > 120_sz;
	}
}
