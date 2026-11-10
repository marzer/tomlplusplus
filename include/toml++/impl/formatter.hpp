//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.hpp"
#if TOML_ENABLE_FORMATTERS

#include "forward_declarations.hpp"
#include "parse_error.hpp"
#include "print_to_stream.hpp"
#include "unicode.hpp"
#include "std_format.hpp"
#include "header_start.hpp"
#if TOML_STD_FORMATTER
TOML_DISABLE_WARNINGS;
#include <sstream>
TOML_ENABLE_WARNINGS;
#endif
/// \cond

TOML_IMPL_NAMESPACE_START
{
	struct formatter_constants
	{
		format_flags mandatory_flags;
		format_flags ignored_flags;

		std::string_view float_pos_inf;
		std::string_view float_neg_inf;
		std::string_view float_nan;

		std::string_view bool_true;
		std::string_view bool_false;
	};

	struct formatter_config
	{
		format_flags flags;
		std::string_view indent;
	};

	enum class TOML_CLOSED_ENUM string_dialect : uint8_t
	{
		toml,
		json,
		yaml
	};

	TOML_CONST_GETTER
	constexpr bool is_non_ascii_unprintable(char32_t c, string_dialect dialect) noexcept
	{
		return (c >= U'\x80' && c <= U'\x9F') || is_non_ascii_vertical_whitespace(c)
			|| (dialect == string_dialect::yaml && (c == U'\xFEFF' || c == U'\xFFFE' || c == U'\xFFFF'));
	}

	class formatter_buffer;

	class TOML_EXPORTED_CLASS formatter
	{
	  private:
		friend class formatter_buffer;
		const node* source_;
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		const parse_result* result_;
#endif
		const formatter_constants* constants_;
		formatter_config config_;
		size_t indent_columns_;
		format_flags int_format_mask_;
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
		bool real_tabs_in_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_real_tabs_in_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool unicode_strings_allowed() const noexcept
		{
			return !!(config_.flags & format_flags::allow_unicode_strings);
		}

		TOML_PURE_INLINE_GETTER
		bool terse_kvps() const noexcept
		{
			return !!(config_.flags & format_flags::terse_key_value_pairs);
		}

		TOML_PURE_INLINE_GETTER
		bool force_multiline_arrays() const noexcept
		{
			return !!(config_.flags & format_flags::force_multiline_arrays);
		}

		TOML_PURE_INLINE_GETTER
		bool relaxed_float_precision() const noexcept
		{
			return !!(config_.flags & format_flags::relaxed_float_precision);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void attach(std::ostream& stream) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void detach() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_newline(bool force = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_indent();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(char);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_unformatted(std::string_view);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_string(std::string_view str,
						  bool allow_multi_line			= true,
						  bool allow_bare				= false,
						  bool allow_literal_whitespace = true);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_string(std::string_view str,
						  bool allow_multi_line,
						  bool allow_bare,
						  bool allow_literal_whitespace,
						  string_dialect dialect);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<int64_t>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<double>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<bool>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const value<date_time>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_value(const node&, node_type);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool dump_failed_parse_result();

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		formatter(const node*, const parse_result*, const formatter_constants&, const formatter_config&) noexcept;
	};

	enum class toml_format_style : uint8_t
	{
		toml,
		json,
		yaml
	};

	template <typename T>
	inline constexpr bool toml_std_formatter_supported =
		toml::is_node<T> || toml::is_node_view<T> || std::is_same_v<impl::remove_cvref<T>, toml::key>
		|| std::is_same_v<impl::remove_cvref<T>, toml::path> || std::is_same_v<impl::remove_cvref<T>, toml::date>
		|| std::is_same_v<impl::remove_cvref<T>, toml::time> || std::is_same_v<impl::remove_cvref<T>, toml::time_offset>
		|| std::is_same_v<impl::remove_cvref<T>, toml::date_time>
#if TOML_ENABLE_PARSER
		|| std::is_same_v<impl::remove_cvref<T>, toml::parse_error>
#endif
		|| std::is_same_v<impl::remove_cvref<T>, toml::parse_result>;

	TOML_EXPORTED_FREE_FUNCTION
	std::string format_node_to_string(const node&, toml_format_style);

#if !TOML_EXCEPTIONS
	TOML_EXPORTED_FREE_FUNCTION
	std::string format_parse_result_to_string(const parse_result&, toml_format_style);
#endif

#if TOML_STD_FORMATTER
	template <typename T>
	std::string format_to_string(const T& value, toml_format_style style)
	{
		if constexpr (toml::is_node<T>)
			return format_node_to_string(value, style);
		else if constexpr (toml::is_node_view<T>)
			return value ? format_node_to_string(*value.node(), style) : ""s;
#if !TOML_EXCEPTIONS
		else if constexpr (std::is_same_v<impl::remove_cvref<T>, toml::parse_result>)
			return format_parse_result_to_string(value, style);
#endif
		else
		{
			std::ostringstream stream;
			stream << value;
			return std::move(stream).str();
		}
	}
#endif
}
TOML_IMPL_NAMESPACE_END;

#if TOML_STD_FORMATTER
namespace std
{
	template <typename T>
	requires(toml::impl::toml_std_formatter_supported<T>)
	struct formatter<T, char>
	{
		toml::impl::toml_format_style style = toml::impl::toml_format_style::toml;

		constexpr auto parse(format_parse_context& ctx)
		{
			auto it		   = ctx.begin();
			const auto end = ctx.end();
			if (it == end || *it == '}')
				return it;

			const auto starts_with = [&it, end](std::string_view text) constexpr
			{
				for (const auto c : text)
				{
					if (it == end || *it != c)
						return false;
					++it;
				}
				return true;
			};

			if (starts_with("toml"sv))
				style = toml::impl::toml_format_style::toml;
			else if (starts_with("json"sv))
				style = toml::impl::toml_format_style::json;
			else if (starts_with("yaml"sv))
				style = toml::impl::toml_format_style::yaml;
			else
#if TOML_EXCEPTIONS
				throw format_error{ "toml++ format must be toml, json or yaml" };
#else
				TOML_ASSERT(false);
#endif

			if (it != end && *it != '}')
#if TOML_EXCEPTIONS
				throw format_error{ "toml++ format does not accept additional specifiers" };
#else
				TOML_ASSERT(false);
#endif
			return it;
		}

		auto format(const T& value, format_context& ctx) const
		{
			const auto output = toml::impl::format_to_string(value, style);
			auto out		  = ctx.out();
			for (const auto c : output)
				*out++ = c;
			return out;
		}
	};
}
#endif

/// \endcond
#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
