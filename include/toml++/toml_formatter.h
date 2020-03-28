//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_print_to_stream.h"

namespace toml
{
	/// \brief	Format flags for modifying how TOML data is printed to streams.
	enum class format_flags : uint8_t
	{
		none,
		quote_dates_and_times = 1
	};
	[[nodiscard]] constexpr format_flags operator & (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>(impl::unbox_enum(lhs) & impl::unbox_enum(rhs));
	}
	[[nodiscard]] constexpr format_flags operator | (format_flags lhs, format_flags rhs) noexcept
	{
		return static_cast<format_flags>( impl::unbox_enum(lhs) | impl::unbox_enum(rhs) );
	}
}


namespace toml::impl
{
	template <typename CHAR = char>
	class formatter
	{
		private:
			const toml::node* source_;
			std::basic_ostream<CHAR>* stream_ = nullptr;
			format_flags flags_;
			int8_t indent_;
			bool naked_newline_;

		protected:
			
			[[nodiscard]] const toml::node& source() const noexcept { return *source_; }
			[[nodiscard]] format_flags flags() const noexcept { return flags_; }
			[[nodiscard]] std::basic_ostream<CHAR>& stream() const noexcept { return *stream_; }

			static constexpr size_t indent_columns = 4;
			static constexpr toml::string_view indent_string = TOML_STRING_PREFIX("    "sv);
			[[nodiscard]] int8_t indent() const noexcept { return indent_; }
			void indent(int8_t level) noexcept { indent_ = level; }
			void increase_indent() noexcept { indent_++; }
			void decrease_indent() noexcept { indent_--; }

			void clear_naked_newline() noexcept { naked_newline_ = false; }

			void attach(std::basic_ostream<CHAR>& stream) noexcept
			{
				indent_ = {};
				naked_newline_ = true;
				stream_ = &stream;
			}

			void detach() noexcept
			{
				stream_ = nullptr;
			}

			void print_newline(bool force = false)
			{
				if (!naked_newline_ || force)
				{
					print_to_stream('\n', *stream_);
					naked_newline_ = true;
				}
			}

			void print_indent()
			{
				for (int8_t i = 0; i < indent_; i++)
				{
					print_to_stream(indent_string, *stream_);
					naked_newline_ = false;
				}
			}

			void print_quoted_string(toml::string_view str)
			{
				if (str.empty())
					print_to_stream("\"\""sv, *stream_);
				else
				{
					print_to_stream('"', *stream_);
					print_to_stream_with_escapes(str, *stream_);
					print_to_stream('"', *stream_);
				}
				naked_newline_ = false;
			}

			template <typename T>
			void print(const value<T>& val)
			{
				if constexpr (std::is_same_v<T, string>)
				{
					print_quoted_string(val.get());
				}
				else
				{
					static constexpr auto is_dt =
						std::is_same_v<T, date>
						|| std::is_same_v<T, time>
						|| std::is_same_v<T, date_time>;

					if constexpr (is_dt)
					{
						if ((flags_ & format_flags::quote_dates_and_times) != format_flags::none)
							print_to_stream('"', *stream_);
					}

					*stream_ << val;

					if constexpr (is_dt)
					{
						if ((flags_ & format_flags::quote_dates_and_times) != format_flags::none)
							print_to_stream('"', *stream_);
					}

					naked_newline_ = false;
				}
			}

			void print(const node& val_node, node_type type)
			{
				switch (type)
				{
					case node_type::string:			print(*reinterpret_cast<const value<string>*>(&val_node)); break;
					case node_type::integer:		print(*reinterpret_cast<const value<int64_t>*>(&val_node)); break;
					case node_type::floating_point:	print(*reinterpret_cast<const value<double>*>(&val_node)); break;
					case node_type::boolean:		print(*reinterpret_cast<const value<bool>*>(&val_node)); break;
					case node_type::date:			print(*reinterpret_cast<const value<date>*>(&val_node)); break;
					case node_type::time:			print(*reinterpret_cast<const value<time>*>(&val_node)); break;
					case node_type::date_time:		print(*reinterpret_cast<const value<date_time>*>(&val_node)); break;
					TOML_NO_DEFAULT_CASE;
				}
			}

			formatter(const toml::node& source, format_flags flags) noexcept
				: source_{ &source },
				flags_{ flags }
			{}
	};
}

