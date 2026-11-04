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
#include "print_to_stream.hpp"
#include "table.hpp"
#include "array.hpp"
#include "unicode.hpp"
#include "header_start.hpp"

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	bool yaml_block_scalar_preserves(std::string_view str) noexcept
	{
		if (str.back() == '\n')
			return false;

		impl::utf8_decoder decoder;
		bool contains_newline = false;
		bool seen_content	  = false;
		for (auto c : str)
		{
			decoder(static_cast<uint8_t>(c));
			if (decoder.error())
				return false;
			if (!decoder.has_code_point())
				continue;

			const auto cp = decoder.codepoint;
			if (cp == U'\n')
			{
				contains_newline = true;
				continue;
			}
			if ((cp == U' ' && !seen_content) || impl::is_nontab_control_character(cp)
				|| impl::is_non_ascii_vertical_whitespace(cp) || impl::is_yaml_unprintable(cp))
				return false;
			seen_content = true;
		}
		return contains_newline && !decoder.needs_more_input();
	}

	TOML_INTERNAL_LINKAGE
	bool yaml_plain_scalar_is_string(std::string_view str) noexcept
	{
		if (str.empty() || str.front() == '-' || str.front() == '+' || (str.front() >= '0' && str.front() <= '9'))
			return false;
		if (str.length() > 5u)
			return true;

		char lower[5];
		for (size_t i = 0; i < str.length(); i++)
			lower[i] = str[i] >= 'A' && str[i] <= 'Z' ? static_cast<char>(str[i] - 'A' + 'a') : str[i];
		const auto word = std::string_view{ lower, str.length() };
		for (auto keyword : { "y"sv, "n"sv, "yes"sv, "no"sv, "on"sv, "off"sv, "true"sv, "false"sv, "null"sv })
			if (word == keyword)
				return false;
		return true;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print_yaml_string(const value<std::string>& str)
	{
		if (str->empty())
		{
			base::print(str);
			return;
		}

		if (TOML_ANON_NAMESPACE::yaml_block_scalar_preserves(*str))
		{
			print_unformatted("|-"sv);

			increase_indent();

			auto line_end  = str->c_str() - 1u;
			const auto end = str->c_str() + str->length();
			while (line_end != end)
			{
				auto line_start = line_end + 1u;
				line_end		= line_start;
				for (; line_end != end && *line_end != '\n'; line_end++)
					;

				if TOML_LIKELY(line_start != line_end || line_end != end)
				{
					print_newline();
					print_indent();
					print_unformatted(std::string_view{ line_start, static_cast<size_t>(line_end - line_start) });
				}
			}

			decrease_indent();
		}
		else
			print_string(*str,
						 false,
						 TOML_ANON_NAMESPACE::yaml_plain_scalar_is_string(*str),
						 true,
						 impl::string_dialect::yaml);
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::table& tbl, bool parent_is_array)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		increase_indent();

		for (auto&& [k, v] : tbl)
		{
			if (!parent_is_array)
			{
				print_newline();
				print_indent();
			}
			parent_is_array = false;

			print_string(k.str(),
						 false,
						 TOML_ANON_NAMESPACE::yaml_plain_scalar_is_string(k.str()),
						 true,
						 impl::string_dialect::yaml);
			print_unformatted(": "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*static_cast<const table*>(&v)); break;
				case node_type::array: print(*static_cast<const array*>(&v)); break;
				case node_type::string: print_yaml_string(*static_cast<const value<std::string>*>(&v)); break;
				default: print_value(v, type);
			}
		}

		decrease_indent();
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print(const toml::array& arr, bool parent_is_array)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		increase_indent();

		for (auto&& v : arr)
		{
			if (!parent_is_array)
			{
				print_newline();
				print_indent();
			}
			parent_is_array = false;

			print_unformatted("- "sv);

			const auto type = v.type();
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print(*static_cast<const table*>(&v), true); break;
				case node_type::array: print(*static_cast<const array*>(&v), true); break;
				case node_type::string: print_yaml_string(*static_cast<const value<std::string>*>(&v)); break;
				default: print_value(v, type);
			}
		}

		decrease_indent();
	}

	TOML_EXTERNAL_LINKAGE
	void yaml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		switch (auto source_type = source().type())
		{
			case node_type::table:
				decrease_indent(); // so root kvps and tables have the same indent
				print(*static_cast<const table*>(&source()));
				break;

			case node_type::array: print(*static_cast<const array*>(&source())); break;

			case node_type::string: print_yaml_string(*static_cast<const value<std::string>*>(&source())); break;

			default: print_value(source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
