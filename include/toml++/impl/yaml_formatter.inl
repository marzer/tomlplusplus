//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.hpp"
//# {{
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}
#if TOML_ENABLE_FORMATTERS

#include "yaml_formatter.hpp"
#include "formatter.inl"
#include "print_to_stream.inl"
#include "table.hpp"
#include "array.hpp"
#include "unicode.hpp"
#include "header_start.hpp"

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	bool yaml_block_scalar_preserves(std::string_view str) noexcept
	{
		if (str.empty() || str.back() == '\n')
			return false;

		impl::utf8_decoder decoder;
		bool contains_newline = {};
		bool seen_content	  = {};
		for (auto c : str)
		{
			decoder(c);
			if (decoder.error())
				return false;
			if (!decoder.has_codepoint())
				continue;

			const auto cp = decoder.codepoint;
			if (cp == U'\n')
			{
				contains_newline = true;
				continue;
			}
			// libyaml rejects a block scalar whose content starts with a tab
			if (((cp == U' ' || cp == U'\t') && !seen_content) || impl::is_nontab_control_character(cp)
				|| impl::is_non_ascii_unprintable(cp, impl::string_dialect::yaml))
				return false;
			seen_content = true;
		}
		return contains_newline && !decoder.needs_more_input();
	}

	TOML_INTERNAL_LINKAGE
	bool yaml_plain_scalar_is_string(std::string_view str) noexcept
	{
		static constexpr std::string_view keywords[] = { "y"sv,	  "n"sv,	"yes"sv,   "no"sv,	"on"sv,
														 "off"sv, "true"sv, "false"sv, "null"sv };
		static constexpr size_t max_keyword_length	 = []() constexpr noexcept
		{
			size_t longest = {};
			for (auto keyword : keywords)
				longest = keyword.length() > longest ? keyword.length() : longest;
			return longest;
		}();

		if (str.empty() || str.front() == '-' || str.front() == '+' || (str.front() >= '0' && str.front() <= '9'))
			return false;
		if (str.length() > max_keyword_length)
			return true;

		char lower[max_keyword_length];
		for (size_t i = 0; i < str.length(); i++)
			lower[i] = str[i] >= 'A' && str[i] <= 'Z' ? static_cast<char>(str[i] - 'A' + 'a') : str[i];
		const auto word = std::string_view{ lower, str.length() };

		for (auto keyword : keywords)
			if (word == keyword)
				return false;
		return true;
	}

	// YAML limits a key written as 'key: value' to 1024 characters. the count here is an upper bound
	// on what the key prints as, so it can only err towards the explicit form
	TOML_INTERNAL_LINKAGE
	bool yaml_key_needs_explicit_form(std::string_view key, bool unicode_allowed) noexcept
	{
		static constexpr size_t max_implicit_key_length = 1024;
		static constexpr size_t escape_length			= 6;  // \uXXXX
		static constexpr size_t long_escape_length		= 10; // \UXXXXXXXX
		static constexpr size_t quotes					= 2;

		// fits even with every byte escaped
		if (key.length() <= (max_implicit_key_length - quotes) / escape_length)
			return false;

		size_t length = quotes;
		impl::utf8_decoder decoder;
		for (const auto c : key)
		{
			decoder(c);
			if (decoder.error())
				return true;
			if (!decoder.has_codepoint())
				continue;

			const auto cp = decoder.codepoint;
			if (cp < U'\x80')
				length += cp >= U' ' && cp < U'\x7F' && cp != U'"' && cp != U'\\' ? size_t{ 1 } : escape_length;
			else if (unicode_allowed && !impl::is_non_ascii_unprintable(cp, impl::string_dialect::yaml))
				length++;
			else
				length += cp > U'\xFFFF' ? long_escape_length : escape_length;

			if (length > max_implicit_key_length)
				return true;
		}
		return decoder.needs_more_input();
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_scalar(std::string_view str)
	{
		print_string(str,
					 false,
					 TOML_ANON_NAMESPACE::yaml_plain_scalar_is_string(str),
					 true,
					 impl::string_dialect::yaml);
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_string(const value<std::string>& str)
	{
		if (!TOML_ANON_NAMESPACE::yaml_block_scalar_preserves(*str))
		{
			print_yaml_scalar(*str);
			return;
		}

		print_unformatted("|-"sv);

		const indent_scope indented{ *this };

		std::string_view remaining{ *str };
		while (true)
		{
			const auto line_length = remaining.find('\n');
			print_newline();
			print_indent();
			print_unformatted(remaining.substr(0u, line_length));
			if (line_length == std::string_view::npos)
				break;
			remaining.remove_prefix(line_length + 1u);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_float(const value<double>& val)
	{
		if (impl::fpclassify(*val) != impl::fp_class::ok)
		{
			base::print(val);
			return;
		}

		// yaml 1.1 reads 1e+16 as a string
		char buf[32];
		const auto end = TOML_ANON_NAMESPACE::write_float(buf, *val, relaxed_float_precision(), true);
		print_unformatted(std::string_view{ buf, static_cast<size_t>(end - buf) });
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_node(const toml::node& n, bool parent_is_array)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print(static_cast<const table&>(n), parent_is_array); break;
			case node_type::array: print(static_cast<const array&>(n), parent_is_array); break;
			case node_type::string: print_yaml_string(static_cast<const value<std::string>&>(n)); break;
			case node_type::floating_point: print_yaml_float(static_cast<const value<double>&>(n)); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::table& tbl, bool parent_is_array)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		const indent_scope indented{ *this };

		bool continues_line = parent_is_array;
		for (auto&& [k, v] : tbl)
		{
			if (!continues_line)
			{
				print_newline();
				print_indent();
			}
			continues_line = false;

			const bool explicit_key =
				TOML_ANON_NAMESPACE::yaml_key_needs_explicit_form(k.str(), unicode_strings_allowed());
			if (explicit_key)
				print_unformatted("? "sv);

			print_yaml_scalar(k.str());

			if (explicit_key)
			{
				print_newline();
				print_indent();
			}
			print_unformatted(": "sv);

			print_node(v);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::array& arr, bool parent_is_array)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		const indent_scope indented{ *this };

		bool continues_line = parent_is_array;
		for (auto&& v : arr)
		{
			if (!continues_line)
			{
				print_newline();
				print_indent();
			}
			continues_line = false;

			print_unformatted("- "sv);

			print_node(v, true);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		const auto type = source().type();
		if (type != node_type::table && type != node_type::array)
		{
			print_node(source());
			return;
		}

		impl::formatter_buffer buffer{ *this };
		if (type == node_type::table)
			decrease_indent(); // so root kvps and tables have the same indent
		print_node(source());
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
