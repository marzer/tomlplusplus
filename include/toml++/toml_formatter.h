#pragma once
#include "toml_value.h"

namespace toml::impl
{
	struct TOML_INTERFACE formatter_writer_interface
	{
		virtual void operator() (const char*, size_t) TOML_MAY_THROW = 0;
		virtual void operator() (char) TOML_MAY_THROW = 0;

		virtual ~formatter_writer_interface() noexcept = default;
	};

	template <typename T>
	class formatter_writer;

	template <typename CHAR>
	class TOML_EMPTY_BASES formatter_writer<std::basic_ostream<CHAR>> : public formatter_writer_interface
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		private:
			std::basic_ostream<CHAR>& target_;

		public:

			void operator() (const char* characters, size_t num) TOML_MAY_THROW override
			{
				TOML_ASSERT(data && size);
				target_.write(reinterpret_cast<const CHAR*>(characters), static_cast<std::streamsize>(num));
			}

			void operator() (char character) TOML_MAY_THROW override
			{
				target_.put(static_cast<CHAR>(character));
			}

			TOML_NODISCARD_CTOR
			formatter_writer(std::basic_ostream<CHAR>& target) noexcept
				: target_{ target }
			{}
	};

	template <typename CHAR>
	formatter_writer(std::basic_ostream<CHAR>&) -> formatter_writer<std::basic_ostream<CHAR>>;

	struct formatter_options final
	{
		toml::string_view indent_string;
		bool quote_dates_and_times;
	};

	class formatter
	{
		private:
			formatter_writer_interface* writer_ = nullptr;
			formatter_options opts;

		protected:

			[[nodiscard]] TOML_ALWAYS_INLINE
			const formatter_options& options() const noexcept
			{
				return opts;
			}

			const toml::table& source;
			size_t indent_string_columns = {};
			int indent_level;
			bool naked_newline;

			void attach(formatter_writer_interface& writer) noexcept
			{
				indent_level = 0;
				naked_newline = true;
				writer_ = &writer;
			}

			void detach() noexcept
			{
				writer_ = nullptr;
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_ALL_WARNINGS // some compilers will complain about a tautological unsigned >= 0.
									  // TINAE - char can have signed _or_ unsigned semantics and I can't
									  // be arsed handling this differently

			static toml::string_view escape_string_character(const toml::string_char& c) noexcept
			{
				static constexpr toml::string_view low_character_escape_table[]
				{
					TOML_STRING_PREFIX("\\u0000"sv),
					TOML_STRING_PREFIX("\\u0001"sv),
					TOML_STRING_PREFIX("\\u0002"sv),
					TOML_STRING_PREFIX("\\u0003"sv),
					TOML_STRING_PREFIX("\\u0004"sv),
					TOML_STRING_PREFIX("\\u0005"sv),
					TOML_STRING_PREFIX("\\u0006"sv),
					TOML_STRING_PREFIX("\\u0007"sv),
					TOML_STRING_PREFIX("\\b"sv),
					TOML_STRING_PREFIX("\\t"sv),
					TOML_STRING_PREFIX("\\n"sv),
					TOML_STRING_PREFIX("\\u000B"sv),
					TOML_STRING_PREFIX("\\f"sv),
					TOML_STRING_PREFIX("\\r"sv),
					TOML_STRING_PREFIX("\\u000E"sv),
					TOML_STRING_PREFIX("\\u000F"sv),
					TOML_STRING_PREFIX("\\u0010"sv),
					TOML_STRING_PREFIX("\\u0011"sv),
					TOML_STRING_PREFIX("\\u0012"sv),
					TOML_STRING_PREFIX("\\u0013"sv),
					TOML_STRING_PREFIX("\\u0014"sv),
					TOML_STRING_PREFIX("\\u0015"sv),
					TOML_STRING_PREFIX("\\u0016"sv),
					TOML_STRING_PREFIX("\\u0017"sv),
					TOML_STRING_PREFIX("\\u0018"sv),
					TOML_STRING_PREFIX("\\u0019"sv),
					TOML_STRING_PREFIX("\\u001A"sv),
					TOML_STRING_PREFIX("\\u001B"sv),
					TOML_STRING_PREFIX("\\u001C"sv),
					TOML_STRING_PREFIX("\\u001D"sv),
					TOML_STRING_PREFIX("\\u001E"sv),
					TOML_STRING_PREFIX("\\u001F"sv),
				};

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

			TOML_ALWAYS_INLINE
			void write(char character) TOML_MAY_THROW
			{
				(*writer_)(character);
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const CHAR* characters, size_t num) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(characters), num);
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(std::basic_string_view<CHAR> strv) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(strv.data()), strv.length());
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const std::basic_string<CHAR>& str) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer_)(reinterpret_cast<const char*>(str.data()), str.length());
			}

			void write_newline(bool force = false) TOML_MAY_THROW
			{
				if (!naked_newline || force)
				{
					write('\n');
					naked_newline = true;
				}
			}

			void write_indent() TOML_MAY_THROW
			{
				for (int i = 0; i < indent_level; i++)
				{
					write(opts.indent_string);
					naked_newline = false;
				}
			}

			template <typename T>
			void write_integer(T val) TOML_MAY_THROW
			{
				char buf[20_sz]; //strlen("-9223372036854775808")
				const auto res = std::to_chars(buf, buf + sizeof(buf), val);
				write(buf, static_cast<size_t>(res.ptr - buf));
				naked_newline = false;
			}

			void write(const value<int64_t>& val) TOML_MAY_THROW
			{
				write_integer(val.get());
			}

			void write(const value<double>& val) TOML_MAY_THROW
			{
				#if TOML_USE_STREAMS_FOR_FLOATS
				{
					std::ostringstream oss;
					oss << val.get();
					write(oss.str());
					if (val.get() == 0.0)
						write(".0"sv);
				}
				#else
				{
					char buf[32_sz];
					const auto res = std::to_chars(buf, buf + sizeof(buf), val.get());
					const auto sv = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
					write(sv);
					bool needs_decimal_point = true;
					for (auto ch : sv)
					{
						if (ch == 'e' || ch == 'E' || ch == '.')
						{
							needs_decimal_point = false;
							break;
						}
					}
					if (needs_decimal_point)
						write(".0"sv);
				}
				#endif
				naked_newline = false;
			}

			void write(const value<bool>& val) TOML_MAY_THROW
			{
				write(val.get() ? "true"sv : "false"sv);
				naked_newline = false;
			}

			void write_quoted_string(toml::string_view str) TOML_MAY_THROW
			{
				if (str.empty())
					write("\"\""sv);
				else
				{
					write('"');
					for (auto c : str)
						write(escape_string_character(c));
					write('"');
				}
				naked_newline = false;
			}

			void write(const value<toml::string>& val) TOML_MAY_THROW
			{
				write_quoted_string(string_view{ val.get() });
			}

			template <typename T>
			void write_zero_padded_integer(T intval, size_t digits) TOML_MAY_THROW
			{
				static_assert(std::is_unsigned_v<T>);

				char buf[19_sz]; //strlen("9223372036854775807")
				const auto res = std::to_chars(buf, buf + sizeof(buf), intval);
				const auto len = static_cast<size_t>(res.ptr - buf);
				for (size_t i = len; i < digits; i++)
					write('0');
				write(buf, len);
				naked_newline = false;
			}

			void write(const time& tm) TOML_MAY_THROW
			{
				write_zero_padded_integer(tm.hour, 2_sz);
				write(':');
				write_zero_padded_integer(tm.minute, 2_sz);
				write(':');
				write_zero_padded_integer(tm.second, 2_sz);

				if (tm.nanosecond && tm.nanosecond <= 999999999u)
				{
					write('.');
					auto ns = tm.nanosecond;
					size_t digits = 9_sz;
					while (ns % 10u == 0u)
					{
						ns /= 10u;
						digits--;
					}
					write_integer(ns);
				}
			}

			void write(const date& dt) TOML_MAY_THROW
			{
				write_zero_padded_integer(dt.year, 4_sz);
				write('-');
				write_zero_padded_integer(dt.month, 2_sz);
				write('-');
				write_zero_padded_integer(dt.day, 2_sz);
			}

			void write(const date_time& dt) TOML_MAY_THROW
			{
				write(dt.date);
				write('T');
				write(dt.time);
				if (dt.time_offset)
				{
					const auto& to = *dt.time_offset;
					if (!to.hours && !to.minutes)
						write('Z');
					else
					{
						write(to.hours < 0 || to.minutes < 0 ? '-' : '+');
						write_zero_padded_integer(static_cast<uint8_t>(to.hours < 0 ? -to.hours : to.hours), 2_sz);
						write(':');
						write_zero_padded_integer(static_cast<uint8_t>(to.minutes < 0 ? -to.minutes : to.minutes), 2_sz);
					}
				}
			}

			void write(const value<time>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write(const value<date>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write(const value<date_time>& val) TOML_MAY_THROW
			{
				if (opts.quote_dates_and_times)
					write('"');
				write(val.get());
				if (opts.quote_dates_and_times)
					write('"');
			}

			void write_value(const node* val_node, node_type type) TOML_MAY_THROW
			{
				switch (type)
				{
					
					case node_type::string: write(*val_node->reinterpret_as<string>()); break;
					case node_type::integer: write(*val_node->reinterpret_as<int64_t>()); break;
					case node_type::floating_point: write(*val_node->reinterpret_as<double>()); break;
					case node_type::boolean: write(*val_node->reinterpret_as<bool>()); break;
					case node_type::date: write(*val_node->reinterpret_as<date>()); break;
					case node_type::time: write(*val_node->reinterpret_as<time>()); break;
					case node_type::date_time: write(*val_node->reinterpret_as<date_time>()); break;
					TOML_NO_DEFAULT_CASE;
				}
			}

			formatter(const toml::table& source_, formatter_options&& options_) noexcept
				: opts{ std::move(options_) },
				source{ source_ }
			{
				if (opts.indent_string.empty())
				{
					opts.indent_string = TOML_STRING_PREFIX("    "sv);
					indent_string_columns = 4;
				}
				else
				{
					for (auto c : opts.indent_string)
						indent_string_columns += c == '\t' ? 4_sz : 1_sz;
				}
			}
	};
}
