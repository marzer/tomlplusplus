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

#include "json_formatter.hpp"
#include "formatter.inl"
#include "table.hpp"
#include "array.hpp"
#include "header_start.hpp"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void json_formatter::print_json_string(std::string_view str)
	{
		print_string(str, false, false, true, impl::string_dialect::json);
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print_node(const toml::node& n)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print(static_cast<const table&>(n)); break;
			case node_type::array: print(static_cast<const array&>(n)); break;
			case node_type::string: print_json_string(static_cast<const value<std::string>&>(n).get()); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted('{');
		{
			const indent_scope indented{ *this, indent_sub_tables() };

			bool first = true;
			for (auto&& [k, v] : tbl)
			{
				if (!first)
					print_unformatted(',');
				first = false;
				print_newline(true);
				print_indent();

				print_json_string(k.str());
				print_unformatted(terse_kvps() ? ":"sv : " : "sv);

				print_node(v);
			}
		}
		print_newline(true);
		print_indent();
		print_unformatted('}');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print(const toml::array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		print_unformatted('[');
		{
			const indent_scope indented{ *this, indent_array_elements() };

			bool first = true;
			for (auto& v : arr)
			{
				if (!first)
					print_unformatted(',');
				first = false;
				print_newline(true);
				print_indent();

				print_node(v);
			}
		}
		print_newline(true);
		print_indent();
		print_unformatted(']');
	}

	TOML_EXTERNAL_LINKAGE
	void json_formatter::print()
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
		print_node(source());
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
