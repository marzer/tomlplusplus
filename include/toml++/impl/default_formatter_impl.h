//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once
/// \cond

//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "default_formatter.h"
#include "print_to_stream.h"
#include "utf8.h"
#include "value.h"
#include "table.h"
#include "array.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	std::string default_formatter::make_key_segment(const std::string& str) noexcept
	{
		if (str.empty())
			return "''"s;
		else
		{
			bool requires_quotes = false;
			{
				impl::utf8_decoder decoder;
				for (size_t i = 0; i < str.length() && !requires_quotes; i++)
				{
					decoder(static_cast<uint8_t>(str[i]));
					if (decoder.error())
						requires_quotes = true;
					else if (decoder.has_code_point())
						requires_quotes = !impl::is_bare_key_character(decoder.codepoint);
				}
			}

			if (requires_quotes)
			{
				std::string s;
				s.reserve(str.length() + 2_sz);
				s += '"';
				for (auto c : str)
				{
					if TOML_UNLIKELY(c >= '\x00' && c <= '\x1F')
						s.append(impl::low_character_escape_table[c]);
					else if TOML_UNLIKELY(c == '\x7F')
						s.append("\\u007F"sv);
					else if TOML_UNLIKELY(c == '"')
						s.append("\\\""sv);
					else
						s += c;
				}
				s += '"';
				return s;
			}
			else
				return str;
		}
	}

	TOML_EXTERNAL_LINKAGE
	size_t default_formatter::count_inline_columns(const node& node) noexcept
	{
		switch (node.type())
		{
			case node_type::table:
			{
				auto& n = *reinterpret_cast<const table*>(&node);
				if (n.empty())
					return 2_sz;	  // "{}"
				size_t weight = 3_sz; // "{ }"
				for (auto&& [k, v] : n)
				{
					weight += k.length() + count_inline_columns(v) + 2_sz; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::array:
			{
				auto& n = *reinterpret_cast<const array*>(&node);
				if (n.empty())
					return 2_sz;	  // "[]"
				size_t weight = 3_sz; // "[ ]"
				for (auto& elem : n)
				{
					weight += count_inline_columns(elem) + 2_sz; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::string:
			{
				auto& n = *reinterpret_cast<const value<std::string>*>(&node);
				return n.get().length() + 2_sz; // + ""
			}

			case node_type::integer:
			{
				auto& n = *reinterpret_cast<const value<int64_t>*>(&node);
				auto v	= n.get();
				if (!v)
					return 1_sz;
				size_t weight = {};
				if (v < 0)
				{
					weight += 1;
					v *= -1;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(v))) + 1_sz;
			}

			case node_type::floating_point:
			{
				auto& n = *reinterpret_cast<const value<double>*>(&node);
				auto v	= n.get();
				if (v == 0.0)
					return 3_sz;	  // "0.0"
				size_t weight = 2_sz; // ".0"
				if (v < 0.0)
				{
					weight += 1;
					v *= -1.0;
				}
				return weight + static_cast<size_t>(log10(v)) + 1_sz;
				break;
			}

			case node_type::boolean: return 5_sz;
			case node_type::date: [[fallthrough]];
			case node_type::time: return 10_sz;
			case node_type::date_time: return 30_sz;
			case node_type::none: TOML_UNREACHABLE;
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	TOML_EXTERNAL_LINKAGE
	bool default_formatter::forces_multiline(const node& node, size_t starting_column_bias) noexcept
	{
		return (count_inline_columns(node) + starting_column_bias) >= line_wrap_cols;
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print_pending_table_separator()
	{
		if (pending_table_separator_)
		{
			base::print_newline(true);
			base::print_newline(true);
			pending_table_separator_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print_key_segment(const std::string& str)
	{
		if (str.empty())
			impl::print_to_stream(base::stream(), "''"sv);
		else
		{
			bool requires_quotes = false;
			{
				impl::utf8_decoder decoder;
				for (size_t i = 0; i < str.length() && !requires_quotes; i++)
				{
					decoder(static_cast<uint8_t>(str[i]));
					if (decoder.error())
						requires_quotes = true;
					else if (decoder.has_code_point())
						requires_quotes = !impl::is_bare_key_character(decoder.codepoint);
				}
			}

			if (requires_quotes)
			{
				impl::print_to_stream(base::stream(), '"');
				impl::print_to_stream_with_escapes(base::stream(), str);
				impl::print_to_stream(base::stream(), '"');
			}
			else
				impl::print_to_stream(base::stream(), str);
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print_key_path()
	{
		for (const auto& segment : key_path_)
		{
			if (std::addressof(segment) > key_path_.data())
				impl::print_to_stream(base::stream(), '.');
			impl::print_to_stream(base::stream(), segment);
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print_inline(const table& tbl)
	{
		if (tbl.empty())
			impl::print_to_stream(base::stream(), "{}"sv);
		else
		{
			impl::print_to_stream(base::stream(), "{ "sv);

			bool first = false;
			for (auto&& [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(base::stream(), ", "sv);
				first = true;

				print_key_segment(k);
				impl::print_to_stream(base::stream(), " = "sv);

				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default: base::print_value(v, type);
				}
			}

			impl::print_to_stream(base::stream(), " }"sv);
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print(const array& arr)
	{
		if (arr.empty())
			impl::print_to_stream(base::stream(), "[]"sv);
		else
		{
			const auto original_indent = base::indent();
			const auto multiline =
				forces_multiline(arr,
								 base::indent_columns * static_cast<size_t>(original_indent < 0 ? 0 : original_indent));
			impl::print_to_stream(base::stream(), "["sv);
			if (multiline)
			{
				if (original_indent < 0)
					base::indent(0);
				base::increase_indent();
			}
			else
				impl::print_to_stream(base::stream(), ' ');

			for (size_t i = 0; i < arr.size(); i++)
			{
				if (i > 0_sz)
				{
					impl::print_to_stream(base::stream(), ',');
					if (!multiline)
						impl::print_to_stream(base::stream(), ' ');
				}

				if (multiline)
				{
					base::print_newline(true);
					base::print_indent();
				}

				auto& v			= arr[i];
				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default: base::print_value(v, type);
				}
			}
			if (multiline)
			{
				base::indent(original_indent);
				base::print_newline(true);
				base::print_indent();
			}
			else
				impl::print_to_stream(base::stream(), ' ');
			impl::print_to_stream(base::stream(), "]"sv);
		}
		base::clear_naked_newline();
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print(const table& tbl)
	{
		static constexpr auto is_non_inline_array_of_tables = [](auto&& nde) noexcept
		{
			auto arr = nde.as_array();
			return arr && arr->is_array_of_tables() && !arr->template get_as<table>(0_sz)->is_inline();
		};

		// values, arrays, and inline tables/table arrays
		for (auto&& [k, v] : tbl)
		{
			const auto type = v.type();
			if ((type == node_type::table && !reinterpret_cast<const table*>(&v)->is_inline())
				|| (type == node_type::array && is_non_inline_array_of_tables(v)))
				continue;

			pending_table_separator_ = true;
			base::print_newline();
			base::print_indent();
			print_key_segment(k);
			impl::print_to_stream(base::stream(), " = "sv);
			TOML_ASSUME(type != node_type::none);
			switch (type)
			{
				case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
				case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
				default: base::print_value(v, type);
			}
		}

		// non-inline tables
		for (auto&& [k, v] : tbl)
		{
			const auto type = v.type();
			if (type != node_type::table || reinterpret_cast<const table*>(&v)->is_inline())
				continue;
			auto& child_tbl = *reinterpret_cast<const table*>(&v);

			// we can skip indenting and emitting the headers for tables that only contain other tables
			// (so we don't over-nest)
			size_t child_value_count{}; // includes inline tables and non-table arrays
			size_t child_table_count{};
			size_t child_table_array_count{};
			for (auto&& [child_k, child_v] : child_tbl)
			{
				(void)child_k;
				const auto child_type = child_v.type();
				TOML_ASSUME(child_type != node_type::none);
				switch (child_type)
				{
					case node_type::table:
						if (reinterpret_cast<const table*>(&child_v)->is_inline())
							child_value_count++;
						else
							child_table_count++;
						break;

					case node_type::array:
						if (is_non_inline_array_of_tables(child_v))
							child_table_array_count++;
						else
							child_value_count++;
						break;

					default: child_value_count++;
				}
			}
			bool skip_self = false;
			if (child_value_count == 0_sz && (child_table_count > 0_sz || child_table_array_count > 0_sz))
				skip_self = true;

			key_path_.push_back(make_key_segment(k));

			if (!skip_self)
			{
				print_pending_table_separator();
				base::increase_indent();
				base::print_indent();
				impl::print_to_stream(base::stream(), "["sv);
				print_key_path();
				impl::print_to_stream(base::stream(), "]"sv);
				pending_table_separator_ = true;
			}

			print(child_tbl);

			key_path_.pop_back();
			if (!skip_self)
				base::decrease_indent();
		}

		// table arrays
		for (auto&& [k, v] : tbl)
		{
			if (!is_non_inline_array_of_tables(v))
				continue;
			auto& arr = *reinterpret_cast<const array*>(&v);

			base::increase_indent();
			key_path_.push_back(make_key_segment(k));

			for (size_t i = 0; i < arr.size(); i++)
			{
				print_pending_table_separator();
				base::print_indent();
				impl::print_to_stream(base::stream(), "[["sv);
				print_key_path();
				impl::print_to_stream(base::stream(), "]]"sv);
				pending_table_separator_ = true;
				print(*reinterpret_cast<const table*>(&arr[i]));
			}

			key_path_.pop_back();
			base::decrease_indent();
		}
	}

	TOML_EXTERNAL_LINKAGE
	void default_formatter::print()
	{
		if (base::dump_failed_parse_result())
			return;

		switch (auto source_type = base::source().type())
		{
			case node_type::table:
			{
				auto& tbl = *reinterpret_cast<const table*>(&base::source());
				if (tbl.is_inline())
					print_inline(tbl);
				else
				{
					base::decrease_indent(); // so root kvps and tables have the same indent
					print(tbl);
				}
				break;
			}

			case node_type::array: print(*reinterpret_cast<const array*>(&base::source())); break;

			default: base::print_value(base::source(), source_type);
		}
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
/// \endcond
