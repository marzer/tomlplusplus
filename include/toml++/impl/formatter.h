//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "print_to_stream.h"
#include "header_start.h"
/// \cond

TOML_IMPL_NAMESPACE_START
{
	struct formatter_constants
	{
		std::string_view pos_inf;
		std::string_view neg_inf;
		std::string_view nan;
	};

	struct formatter_config
	{
		format_flags flags;
		std::string_view indent;
	};

	class formatter
	{
	  private:
		const node* source_;
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		const parse_result* result_;
#endif
		const formatter_constants* constants_;
		formatter_config config_;
		size_t indent_columns_;
		std::ostream* stream_; //
		int indent_;		   // these are set in attach()
		bool naked_newline_;   //

	  protected:
		TOML_PURE_INLINE_GETTER
		const node& source() const noexcept
		{
			return *source_;
		}

		TOML_PURE_INLINE_GETTER
		std::ostream& stream() const noexcept
		{
			return *stream_;
		}

		TOML_PURE_INLINE_GETTER
		int indent() const noexcept
		{
			return indent_;
		}

		void indent(int level) noexcept
		{
			indent_ = level;
		}

		void increase_indent() noexcept
		{
			indent_++;
		}

		void decrease_indent() noexcept
		{
			indent_--;
		}

		TOML_PURE_INLINE_GETTER
		size_t indent_columns() const noexcept
		{
			return indent_columns_;
		}

		TOML_PURE_INLINE_GETTER
		bool indent_array_elements() const noexcept
		{
			return !!(config_.flags & format_flags::indent_array_elements);
		}

		TOML_PURE_INLINE_GETTER
		bool indent_sub_tables() const noexcept
		{
			return !!(config_.flags & format_flags::indent_sub_tables);
		}

		TOML_PURE_INLINE_GETTER
		bool quote_dates_and_times() const noexcept
		{
			return !!(config_.flags & format_flags::quote_dates_and_times);
		}

		TOML_PURE_INLINE_GETTER
		bool quote_infinities_and_nans() const noexcept
		{
			return !!(config_.flags & format_flags::quote_infinities_and_nans);
		}

		TOML_PURE_INLINE_GETTER
		bool literal_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_literal_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool multi_line_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_multi_line_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool value_format_flags_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_value_format_flags);
		}

		TOML_PURE_INLINE_GETTER
		bool naked_newline() const noexcept
		{
			return naked_newline_;
		}

		void clear_naked_newline() noexcept
		{
			naked_newline_ = false;
		}

		TOML_API
		void attach(std::ostream& stream) noexcept;

		TOML_API
		void detach() noexcept;

		TOML_API
		void print_newline(bool force = false);

		TOML_API
		void print_indent();

		TOML_API
		void print_string(std::string_view str, bool allow_multi_line = true, bool allow_bare = false);

		TOML_API
		void print(const value<std::string>&);

		TOML_API
		void print(const value<int64_t>&);

		TOML_API
		void print(const value<double>&);

		TOML_API
		void print(const value<bool>&);

		TOML_API
		void print(const value<date>&);

		TOML_API
		void print(const value<time>&);

		TOML_API
		void print(const value<date_time>&);

		TOML_API
		void print_value(const node&, node_type);

		TOML_NODISCARD
		TOML_API
		bool dump_failed_parse_result();

		TOML_NODISCARD_CTOR
		TOML_API
		formatter(const node*, const parse_result*, const formatter_constants&, const formatter_config&) noexcept;
	};
}
TOML_IMPL_NAMESPACE_END;

/// \endcond
#include "header_end.h"
