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

#include "toml_formatter.hpp"
#include "formatter.inl"
#include "print_to_stream.inl"
#include "value.hpp"
#include "table.hpp"
#include "array.hpp"
#include "unicode.hpp"
#include "header_start.hpp"

TOML_ANON_NAMESPACE_START
{
	// counts a float that has at most eight decimals, since printing one to count it costs as much as printing it.
	// returns zero for any other float
	TOML_INTERNAL_LINKAGE
	size_t toml_formatter_count_short_float_columns(double val) noexcept
	{
		static constexpr double scale		 = 1e8;
		static constexpr size_t max_decimals = 8u;

		const bool negative	   = impl::signbit(val);
		const double magnitude = negative ? -val : val;

		// below 1e7 only one text with this many decimals reads back as the value; below 1e-4 the exponent form prints
		if (!(magnitude < 1e7) || (magnitude < fixed_form_min && magnitude > 0.0))
			return {};

		const double scaled = magnitude * scale;
		auto digits			= static_cast<uint64_t>(scaled);
		if (static_cast<double>(digits) != scaled || static_cast<double>(digits) / scale != magnitude)
			return {};

		size_t decimals = max_decimals;
		for (; decimals > 1u && digits % 10u == 0u; digits /= 10u)
			decimals--;
		for (size_t i = 0; i < decimals; i++)
			digits /= 10u;

		size_t weight = (negative ? 2u : 1u) + decimals; // "-" and "."
		do
		{
			weight++;
			digits /= 10u;
		}
		while (digits);
		return weight;
	}

	TOML_INTERNAL_LINKAGE
	size_t toml_formatter_count_inline_columns(const node& n, size_t line_wrap_cols, format_flags flags) noexcept
	{
		switch (n.type())
		{
			case node_type::table:
			{
				auto& tbl = static_cast<const table&>(n);
				if (tbl.empty())
					return 2u;		// "{}"
				size_t weight = 3u; // "{ }"
				for (auto&& [k, v] : tbl)
				{
					// +  ", "
					weight += k.length() + toml_formatter_count_inline_columns(v, line_wrap_cols, flags) + 2u;
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::array:
			{
				auto& arr = static_cast<const array&>(n);
				if (arr.empty())
					return 2u;		// "[]"
				size_t weight = 3u; // "[ ]"
				for (auto& elem : arr)
				{
					weight += toml_formatter_count_inline_columns(elem, line_wrap_cols, flags) + 2u; // +  ", "
					if (weight >= line_wrap_cols)
						break;
				}
				return weight;
			}

			case node_type::string:
			{
				auto& str = static_cast<const value<std::string>&>(n).get();
				return str.length() + 2u; // + ""
			}

			case node_type::integer:
			{
				auto& integer  = static_cast<const value<int64_t>&>(n);
				const auto val = integer.get();

				unsigned bits_per_digit = {};
				switch (TOML_ANON_NAMESPACE::printed_integer_format(val, integer.flags(), flags))
				{
					case value_flags::format_as_binary: bits_per_digit = 1u; break;
					case value_flags::format_as_octal: bits_per_digit = 3u; break;
					case value_flags::format_as_hexadecimal: bits_per_digit = 4u; break;
					default: break;
				}
				if (bits_per_digit)
				{
					size_t weight = 3u; // "0x" and the first digit
					for (auto rest = static_cast<uint64_t>(val) >> bits_per_digit; rest; rest >>= bits_per_digit)
						weight++;
					return weight;
				}

				size_t weight  = val < 0 ? 2u : 1u; // "-" and the first digit
				auto magnitude = static_cast<uint64_t>(val);
				if (val < 0)
					magnitude = 0u - magnitude;
				for (; magnitude >= 10u; magnitude /= 10u)
					weight++;
				return weight;
			}

			case node_type::floating_point:
			{
				const auto val = static_cast<const value<double>&>(n).get();
				if (const auto cls = impl::fpclassify(val); cls != impl::fp_class::ok)
					return cls == impl::fp_class::neg_inf ? 4u : 3u; // "-inf", or "inf" and "nan"

				const bool relaxed_floats = !!(flags & format_flags::relaxed_float_precision);
				if (!relaxed_floats)
				{
					if (const auto columns = toml_formatter_count_short_float_columns(val))
						return columns;
				}

				char buf[32];
				return static_cast<size_t>(TOML_ANON_NAMESPACE::write_float(buf, val, relaxed_floats, false) - buf);
			}

			case node_type::boolean: return static_cast<const value<bool>&>(n).get() ? 4u : 5u; // "true", "false"

			case node_type::date:
			{
				char buf[64];
				const auto end = TOML_ANON_NAMESPACE::write_date(buf, static_cast<const value<date>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::time:
			{
				char buf[64];
				const auto end = TOML_ANON_NAMESPACE::write_time(buf, static_cast<const value<time>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::date_time:
			{
				char buf[64];
				const auto end =
					TOML_ANON_NAMESPACE::write_date_time(buf, static_cast<const value<date_time>&>(n).get());
				return static_cast<size_t>(end - buf);
			}

			case node_type::none:
			default: TOML_UNREACHABLE;
		}

		TOML_UNREACHABLE;
	}

	class toml_formatter_key_path_scope
	{
	  private:
		std::vector<const key*>* path_;
		size_t previous_size_;

	  public:
		TOML_NODISCARD_CTOR
		explicit toml_formatter_key_path_scope(std::vector<const key*>& path, const key& k) //
			: path_{ &path },
			  previous_size_{ path.size() }
		{
			path.push_back(&k);
		}

		// restores instead of popping, as the parser's scopes do (#288)
		~toml_formatter_key_path_scope() noexcept
		{
			path_->resize(previous_size_);
		}

		TOML_DELETE_DEFAULTS(toml_formatter_key_path_scope);
	};

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_is_sub_table(const node& n) noexcept
	{
		const auto tbl = n.as_table();
		return tbl && !tbl->is_inline();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_is_table_array(const node& n) noexcept
	{
		const auto arr = n.as_array();
		if (!arr || !arr->is_array_of_tables())
			return false;

		return !static_cast<const table&>((*arr)[0]).is_inline();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_holds_only_tables(const table& tbl) noexcept
	{
		for (auto&& [_, v] : tbl)
		{
			if (!toml_formatter_is_sub_table(v) && !toml_formatter_is_table_array(v))
				return false;
		}
		return !tbl.empty();
	}

	TOML_INTERNAL_LINKAGE
	bool toml_formatter_forces_multiline(const node& n,
										 size_t line_wrap_cols,
										 size_t starting_column_bias,
										 format_flags flags) noexcept
	{
		return (toml_formatter_count_inline_columns(n, line_wrap_cols, flags) + starting_column_bias) >= line_wrap_cols;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_pending_table_separator()
	{
		if (pending_table_separator_)
		{
			print_newline(true);
			print_newline(true);
			pending_table_separator_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const key& k)
	{
		print_string(k.str(), false, true, false);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_equals()
	{
		if (terse_kvps())
			print_unformatted('=');
		else
			print_unformatted(" = "sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_inline(const node& n)
	{
		const auto type = n.type();
		TOML_ASSUME(type != node_type::none);
		switch (type)
		{
			case node_type::table: print_inline(static_cast<const table&>(n)); break;
			case node_type::array: print(static_cast<const array&>(n)); break;
			default: print_value(n, type);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_inline(const table& tbl)
	{
		if (tbl.empty())
		{
			print_unformatted("{}"sv);
			return;
		}

		print_unformatted("{ "sv);

		bool first = true;
		for (auto&& [k, v] : tbl)
		{
			if (!first)
				print_unformatted(", "sv);
			first = false;

			print(k);
			print_equals();
			print_inline(v);
		}

		print_unformatted(" }"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const array& arr)
	{
		if (arr.empty())
		{
			print_unformatted("[]"sv);
			return;
		}

		static constexpr size_t line_wrap_columns = 120u;

		const auto original_indent = indent();
		const auto multiline	   = force_multi_line_arrays()
								  || TOML_ANON_NAMESPACE::toml_formatter_forces_multiline(
										 arr,
										 line_wrap_columns,
										 indent_columns() * static_cast<size_t>(original_indent < 0 ? 0 : original_indent),
										 effective_flags());

		print_unformatted('[');

		if (multiline)
		{
			if (original_indent < 0)
				indent(0);
			if (indent_array_elements())
				increase_indent();
		}
		else
			print_unformatted(' ');

		bool first = true;
		for (auto& v : arr)
		{
			if (!first)
			{
				print_unformatted(',');
				if (!multiline)
					print_unformatted(' ');
			}
			first = false;

			if (multiline)
			{
				print_newline(true);
				print_indent();
			}

			print_inline(v);
		}
		if (multiline)
		{
			indent(original_indent);
			print_newline(true);
			print_indent();
		}
		else
			print_unformatted(' ');

		print_unformatted(']');
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print(const table& tbl)
	{
		// sub-tables and arrays of tables print after the values; the counts skip those passes for a table with none
		size_t sub_tables	= {};
		size_t table_arrays = {};

		// values, arrays, and inline tables/table arrays
		for (auto&& [k, v] : tbl)
		{
			if (TOML_ANON_NAMESPACE::toml_formatter_is_sub_table(v))
			{
				sub_tables++;
				continue;
			}
			if (TOML_ANON_NAMESPACE::toml_formatter_is_table_array(v))
			{
				table_arrays++;
				continue;
			}

			pending_table_separator_ = true;
			print_newline();
			print_indent();
			print(k);
			print_equals();
			print_inline(v);
		}

		// non-inline tables
		if (sub_tables)
		{
			for (auto&& [k, v] : tbl)
			{
				if (TOML_ANON_NAMESPACE::toml_formatter_is_sub_table(v))
					print_sub_table(k, static_cast<const table&>(v));
			}
		}

		// table arrays
		if (table_arrays)
		{
			for (auto&& [k, v] : tbl)
			{
				if (TOML_ANON_NAMESPACE::toml_formatter_is_table_array(v))
					print_table_array(k, static_cast<const array&>(v));
			}
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_key_path()
	{
		bool first = true;
		for (const auto k : key_path_)
		{
			if (!first)
				print_unformatted('.');
			first = false;
			print(*k);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_sub_table(const key& k, const table& tbl)
	{
		// we can skip indenting and emitting the headers for tables that only contain other tables
		// (so we don't over-nest)
		const bool skip_self = TOML_ANON_NAMESPACE::toml_formatter_holds_only_tables(tbl);

		const TOML_ANON_NAMESPACE::toml_formatter_key_path_scope key_path{ key_path_, k };
		const indent_scope indented{ *this, !skip_self && indent_sub_tables() };

		if (!skip_self)
		{
			print_pending_table_separator();
			print_indent();
			print_unformatted('[');
			print_key_path();
			print_unformatted(']');
			pending_table_separator_ = true;
		}

		print(tbl);
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print_table_array(const key& k, const array& arr)
	{
		const indent_scope indented{ *this, indent_sub_tables() };
		const TOML_ANON_NAMESPACE::toml_formatter_key_path_scope key_path{ key_path_, k };

		for (auto& elem : arr)
		{
			print_pending_table_separator();
			print_indent();
			print_unformatted("[["sv);
			print_key_path();
			print_unformatted("]]"sv);
			pending_table_separator_ = true;
			print(static_cast<const table&>(elem));
		}
	}

	TOML_EXTERNAL_LINKAGE
	void toml_formatter::print()
	{
		if (dump_failed_parse_result())
			return;

		const auto type = source().type();
		if (type != node_type::table && type != node_type::array)
		{
			print_value(source(), type);
			return;
		}

		impl::formatter_buffer buffer{ *this };
		if (type == node_type::array)
			print(static_cast<const array&>(source()));
		else if (auto& tbl = static_cast<const table&>(source()); tbl.is_inline())
			print_inline(tbl);
		else
		{
			decrease_indent(); // so root kvps and tables have the same indent
			print(tbl);
		}
		buffer.flush();
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
