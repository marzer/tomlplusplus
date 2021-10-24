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
	class formatter
	{
	  private:
		const toml::node* source_;
		std::ostream* stream_ = {};
		format_flags flags_;
		int indent_;
		bool naked_newline_;
#if TOML_PARSER && !TOML_EXCEPTIONS
		const parse_result* result_ = {};
#endif

	  protected:
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		const toml::node& source() const noexcept
		{
			return *source_;
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		std::ostream& stream() const noexcept
		{
			return *stream_;
		}

		static constexpr size_t indent_columns			= 4;
		static constexpr std::string_view indent_string = "    "sv;

		TOML_NODISCARD
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

		TOML_NODISCARD
		bool quote_dates_and_times() const noexcept
		{
			return !!(flags_ & format_flags::quote_dates_and_times);
		}

		TOML_NODISCARD
		bool literal_strings_allowed() const noexcept
		{
			return !!(flags_ & format_flags::allow_literal_strings);
		}

		TOML_NODISCARD
		bool multi_line_strings_allowed() const noexcept
		{
			return !!(flags_ & format_flags::allow_multi_line_strings);
		}

		TOML_NODISCARD
		bool value_format_flags_allowed() const noexcept
		{
			return !!(flags_ & format_flags::allow_value_format_flags);
		}

		TOML_NODISCARD
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
		void print_quoted_string(std::string_view str, bool allow_multi_line = true);

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
		bool dump_failed_parse_result() noexcept(!TOML_PARSER || TOML_EXCEPTIONS);

		TOML_NODISCARD_CTOR
		formatter(const toml::node& source, format_flags flags) noexcept //
			: source_{ &source },
			  flags_{ flags }
		{}

#if TOML_PARSER && !TOML_EXCEPTIONS

		TOML_NODISCARD_CTOR
		TOML_API
		formatter(const parse_result& result, format_flags flags) noexcept;

#endif
	};
}
TOML_IMPL_NAMESPACE_END;

/// \endcond
#include "header_end.h"
