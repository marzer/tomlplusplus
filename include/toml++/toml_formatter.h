#pragma once
#include "toml_print_to_stream.h"

namespace toml::impl
{
	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS	// some compilers will complain about a tautological unsigned >= 0.
								// TINAE - char can have signed _or_ unsigned semantics and I can't
								// be arsed handling this differently

	[[nodiscard]]
	inline toml::string_view escape_string_character(const toml::string_char& c) noexcept
	{
		if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
			return low_character_escape_table[c];
		else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
			return TOML_STRING_PREFIX("\\u007F"sv);
		else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
			return TOML_STRING_PREFIX("\\\""sv);
		else
			return toml::string_view{ &c, 1_sz };
	}

	TOML_POP_WARNINGS

	struct formatter_options final
	{
		toml::string_view indent_string;
		bool quote_dates_and_times;
	};

	template <typename CHAR = char>
	class formatter
	{
		private:
			const toml::table& source_;
			std::basic_ostream<CHAR>* stream_ = nullptr;
			formatter_options options_;
			int indent_;
			bool naked_newline_;
			size_t indent_columns_;

		protected:
			
			[[nodiscard]] const toml::table& source() const noexcept { return source_; }
			[[nodiscard]] const formatter_options& options() const noexcept { return options_; }
			[[nodiscard]] std::basic_ostream<CHAR>& stream() const noexcept { return *stream_; }

			[[nodiscard]] int indent() const noexcept { return indent_; }
			[[nodiscard]] size_t indent_columns() const noexcept { return indent_columns_; }
			void indent(int level) noexcept { indent_ = level; }
			void increase_indent() noexcept { indent_++; }
			void decrease_indent() noexcept { indent_--; }

			void clear_naked_newline() noexcept { naked_newline_ = false; }

			void attach(std::basic_ostream<CHAR>& stream) noexcept
			{
				indent_ = 0;
				naked_newline_ = true;
				stream_ = &stream;
			}

			void detach() noexcept
			{
				stream_ = nullptr;
			}

			void print_newline(bool force = false) TOML_MAY_THROW
			{
				if (!naked_newline_ || force)
				{
					print_to_stream('\n', *stream_);
					naked_newline_ = true;
				}
			}

			void print_indent() TOML_MAY_THROW
			{
				for (int i = 0; i < indent_; i++)
				{
					print_to_stream(options_.indent_string, *stream_);
					naked_newline_ = false;
				}
			}

			void print_quoted_string(toml::string_view str) TOML_MAY_THROW
			{
				if (str.empty())
					print_to_stream("\"\""sv, *stream_);
				else
				{
					print_to_stream('"', *stream_);
					for (auto c : str)
						print_to_stream(escape_string_character(c), *stream_);
					print_to_stream('"', *stream_);
				}
				naked_newline_ = false;
			}

			template <typename T>
			void print(const value<T>& val) TOML_MAY_THROW
			{
				if constexpr (std::is_same_v<T, string>)
				{
					print_quoted_string(val.get());
				}
				else
				{
					static constexpr auto is_date_time =
						std::is_same_v<T, date>
						|| std::is_same_v<T, time>
						|| std::is_same_v<T, date_time>;

					if constexpr (is_date_time)
					{
						if (options_.quote_dates_and_times)
							print_to_stream('"', *stream_);
					}

					*stream_ << val;

					if constexpr (is_date_time)
					{
						if (options_.quote_dates_and_times)
							print_to_stream('"', *stream_);
					}

					naked_newline_ = false;
				}
			}

			void print(const node& val_node, node_type type) TOML_MAY_THROW
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

			formatter(const toml::table& source, formatter_options&& options) noexcept
				: source_{ source },
				options_{ std::move(options) }
				
			{
				if (options_.indent_string.empty())
				{
					options_.indent_string = TOML_STRING_PREFIX("    "sv);
					indent_columns_ = 4_sz;
				}
				else
				{
					indent_columns_ = {};
					for (auto c : options_.indent_string)
						indent_columns_ += c == '\t' ? 4_sz : 1_sz;
				}
			}
	};
}
