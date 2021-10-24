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

#include "formatter.h"
#include "print_to_stream.h"
#include "utf8.h"
#include "value.h"
#include "table.h"
#include "array.h"
#include "parse_result.h"
#include "header_start.h"

TOML_IMPL_NAMESPACE_START
{
#if TOML_PARSER && !TOML_EXCEPTIONS

	TOML_EXTERNAL_LINKAGE
	formatter::formatter(const parse_result& result, format_flags flags) noexcept //
		: source_{ result ? &result.table() : nullptr },
		  flags_{ flags },
		  result_{ &result }
	{}

#endif

	TOML_EXTERNAL_LINKAGE
	void formatter::attach(std::ostream & stream) noexcept
	{
		indent_		   = {};
		naked_newline_ = true;
		stream_		   = &stream;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::detach() noexcept
	{
		stream_ = nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_newline(bool force)
	{
		if (!naked_newline_ || force)
		{
			print_to_stream(*stream_, '\n');
			naked_newline_ = true;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_indent()
	{
		for (int i = 0; i < indent_; i++)
		{
			print_to_stream(*stream_, indent_string);
			naked_newline_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_quoted_string(std::string_view str, bool allow_multi_line)
	{
		auto literals = literal_strings_allowed();
		if (str.empty())
		{
			print_to_stream(*stream_, literals ? "''"sv : "\"\""sv);
			naked_newline_ = false;
			return;
		}

		auto multi_line = allow_multi_line && multi_line_strings_allowed();
		if (multi_line || literals)
		{
			utf8_decoder decoder;
			bool has_line_breaks   = false;
			bool has_control_chars = false;
			bool has_single_quotes = false;
			for (size_t i = 0; i < str.length() && !(has_line_breaks && has_control_chars && has_single_quotes); i++)
			{
				decoder(static_cast<uint8_t>(str[i]));
				if (decoder.error())
				{
					has_line_breaks	  = false;
					has_control_chars = true; // force ""
					has_single_quotes = true;
					break;
				}
				else if (decoder.has_code_point())
				{
					if (is_line_break(decoder.codepoint))
						has_line_breaks = true;
					else if (is_nontab_control_character(decoder.codepoint))
						has_control_chars = true;
					else if (decoder.codepoint == U'\'')
						has_single_quotes = true;
				}
			}
			multi_line = multi_line && has_line_breaks;
			literals   = literals && !has_control_chars && !(!multi_line && has_single_quotes);
		}

		if (literals)
			print_to_stream_bookended(*stream_, str, multi_line ? "'''"sv : "'"sv);
		else
		{
			const auto quot = multi_line ? R"(""")"sv : R"(")"sv;
			print_to_stream(*stream_, quot);
			print_to_stream_with_escapes(*stream_, str);
			print_to_stream(*stream_, quot);
		}
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<std::string>& val)
	{
		print_quoted_string(val.get());
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<int64_t>& val)
	{
		if (value_format_flags_allowed() && *val >= 0)
		{
			static constexpr auto value_format_flags =
				value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;
			const auto fmt = val.flags() & value_format_flags;
			if (fmt != value_flags::none)
			{
				switch (fmt)
				{
					case value_flags::format_as_binary: print_to_stream(*stream_, "0b"sv); break;
					case value_flags::format_as_octal: print_to_stream(*stream_, "0o"sv); break;
					case value_flags::format_as_hexadecimal: print_to_stream(*stream_, "0x"sv); break;
					default: TOML_UNREACHABLE;
				}
			}
			print_to_stream(*stream_, *val, fmt);
		}
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<double>& val)
	{
		print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<bool>& val)
	{
		print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date>& val)
	{
		if (quote_dates_and_times())
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<time>& val)
	{
		if (quote_dates_and_times())
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date_time>& val)
	{
		if (quote_dates_and_times())
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_value(const node& val_node, node_type type)
	{
		TOML_ASSUME(type > node_type::array);
		switch (type)
		{
			case node_type::string: print(*reinterpret_cast<const value<std::string>*>(&val_node)); break;
			case node_type::integer: print(*reinterpret_cast<const value<int64_t>*>(&val_node)); break;
			case node_type::floating_point: print(*reinterpret_cast<const value<double>*>(&val_node)); break;
			case node_type::boolean: print(*reinterpret_cast<const value<bool>*>(&val_node)); break;
			case node_type::date: print(*reinterpret_cast<const value<date>*>(&val_node)); break;
			case node_type::time: print(*reinterpret_cast<const value<time>*>(&val_node)); break;
			case node_type::date_time: print(*reinterpret_cast<const value<date_time>*>(&val_node)); break;
			default: TOML_UNREACHABLE;
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool formatter::dump_failed_parse_result() noexcept(!TOML_PARSER || TOML_EXCEPTIONS)
	{
#if TOML_PARSER && !TOML_EXCEPTIONS
		if (result_ && !(*result_))
		{
			stream() << result_->error();
			return true;
		}
#endif
		return false;
	}
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.h"
/// \endcond
