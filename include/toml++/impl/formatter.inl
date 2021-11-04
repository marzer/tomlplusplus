//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
//# {{
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}
#if TOML_ENABLE_FORMATTERS

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
	TOML_EXTERNAL_LINKAGE
	formatter::formatter(const node* source_node,
						 const parse_result* source_pr,
						 const formatter_constants& constants,
						 const formatter_config& config) noexcept //
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		: source_{ source_pr && *source_pr ? &source_pr->table() : source_node },
		  result_{ source_pr },
#else
		: source_{ source_pr ? source_pr : source_node },
#endif
		  constants_{ &constants },
		  config_{ config }
	{
		TOML_ASSERT(source_ != nullptr);

		config_.flags = (config_.flags | constants_->mandatory_flags) & ~constants_->ignored_flags;

		indent_columns_ = {};
		for (auto c : config_.indent)
			indent_columns_ += c == '\t' ? 4u : 1u;

		int_format_mask_ = config_.flags
						 & (format_flags::allow_binary_integers | format_flags::allow_octal_integers
							| format_flags::allow_hexadecimal_integers);
	}

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
			print_to_stream(*stream_, config_.indent);
			naked_newline_ = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(char c)
	{
		print_to_stream(*stream_, c);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(std::string_view str)
	{
		print_to_stream(*stream_, str);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str, bool allow_multi_line, bool allow_bare)
	{
		auto literal = literal_strings_allowed();
		if (str.empty())
		{
			print_to_stream(*stream_, literal ? "''"sv : "\"\""sv);
			naked_newline_ = false;
			return;
		}

		bool multi_line = allow_multi_line && !!(config_.flags & format_flags::allow_multi_line_strings);
		const bool treat_raw_tab_as_control_char = !(config_.flags & format_flags::allow_real_tabs_in_strings);
		if (multi_line || literal || treat_raw_tab_as_control_char || allow_bare)
		{
			utf8_decoder decoder;
			bool has_line_breaks   = false;
			bool has_control_chars = false;
			bool has_single_quotes = false;
			for (size_t i = 0; i < str.length(); i++)
			{
				decoder(static_cast<uint8_t>(str[i]));
				if (decoder.error())
				{
					has_line_breaks	  = false;
					has_control_chars = true; // force ""
					has_single_quotes = true;
					allow_bare		  = false;
					break;
				}
				else if (decoder.has_code_point())
				{
					if (is_vertical_whitespace(decoder.codepoint))
						has_line_breaks = true;
					else if (is_nontab_control_character(decoder.codepoint)
							 || (treat_raw_tab_as_control_char && decoder.codepoint == U'\t'))
						has_control_chars = true;
					else if (decoder.codepoint == U'\'')
						has_single_quotes = true;
					if (allow_bare)
						allow_bare = is_bare_key_character(decoder.codepoint);
				}

				if (has_line_breaks && has_control_chars && has_single_quotes && !allow_bare)
					break;
			}
			multi_line = multi_line && has_line_breaks;
			literal	   = literal && !has_control_chars && !(!multi_line && has_single_quotes);
		}

		if (allow_bare)
			print_to_stream(*stream_, str);
		else if (literal)
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
		print_string(val.get());
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<int64_t>& val)
	{
		naked_newline_ = false;

		if (*val >= 0 && !!int_format_mask_)
		{
			static constexpr auto value_flags_mask =
				value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;

			const auto fmt = val.flags() & value_flags_mask;
			switch (fmt)
			{
				case value_flags::format_as_binary:
					if (!!(int_format_mask_ & format_flags::allow_binary_integers))
					{
						print_to_stream(*stream_, "0b"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_octal:
					if (!!(int_format_mask_ & format_flags::allow_octal_integers))
					{
						print_to_stream(*stream_, "0o"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_hexadecimal:
					if (!!(int_format_mask_ & format_flags::allow_hexadecimal_integers))
					{
						print_to_stream(*stream_, "0x"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				default: break;
			}
		}

		// fallback to decimal
		print_to_stream(*stream_, *val);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<double>& val)
	{
		const std::string_view* inf_nan = nullptr;
		switch (fpclassify(*val))
		{
			case fp_class::neg_inf: inf_nan = &constants_->float_neg_inf; break;
			case fp_class::pos_inf: inf_nan = &constants_->float_pos_inf; break;
			case fp_class::nan: inf_nan = &constants_->float_nan; break;
			case fp_class::ok: print_to_stream(*stream_, *val); break;
			default: TOML_UNREACHABLE;
		}

		if (inf_nan)
		{
			if (!!(config_.flags & format_flags::quote_infinities_and_nans))
				print_to_stream_bookended(*stream_, *inf_nan, '"');
			else
				print_to_stream(*stream_, *inf_nan);
		}

		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<bool>& val)
	{
		print_unformatted(*val ? constants_->bool_true : constants_->bool_false);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date_time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
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

#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

	TOML_EXTERNAL_LINKAGE
	bool formatter::dump_failed_parse_result()
	{
		if (result_ && !(*result_))
		{
			stream() << result_->error();
			return true;
		}
		return false;
	}

#else

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(const)
	bool formatter::dump_failed_parse_result()
	{
		return false;
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.h"
#endif // TOML_ENABLE_FORMATTERS
