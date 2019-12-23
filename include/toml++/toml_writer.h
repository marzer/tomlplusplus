#pragma once
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table_array.h"

namespace toml::impl
{
	template <>
	class TOML_INTERFACE writer<void>
	{
		protected:
			virtual void write(const void*, size_t) = 0;
			virtual void write(char) = 0;

		private:
			int indent;
			bool naked_newline;
			std::vector<string> key_path;

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(std::basic_string_view<CHAR> strv)
			{
				static_assert(sizeof(CHAR) == 1_sz);

				write(reinterpret_cast<const uint8_t*>(strv.data()), strv.length());
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const std::basic_string<CHAR>& str)
			{
				static_assert(sizeof(CHAR) == 1_sz);

				write(reinterpret_cast<const uint8_t*>(str.data()), str.length());
			}

			void write_newline(bool force = false)
			{
				if (!naked_newline || force)
				{
					write('\n');
					naked_newline = true;
				}
			}

			void write_indent()
			{
				for (int i = 0; i < indent; i++)
				{
					write('\t');
					naked_newline = false;
				}
			}

			void write_string_character(string_char c)
			{
				if (c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
				{
					switch (c)
					{
						case '\x00': write(TOML_STRING_PREFIX("\\u0000"sv)); break;
						case '\x01': write(TOML_STRING_PREFIX("\\u0001"sv)); break;
						case '\x02': write(TOML_STRING_PREFIX("\\u0002"sv)); break;
						case '\x03': write(TOML_STRING_PREFIX("\\u0003"sv)); break;
						case '\x04': write(TOML_STRING_PREFIX("\\u0004"sv)); break;
						case '\x05': write(TOML_STRING_PREFIX("\\u0005"sv)); break;
						case '\x06': write(TOML_STRING_PREFIX("\\u0006"sv)); break;
						case '\x07': write(TOML_STRING_PREFIX("\\u0007"sv)); break;
						case '\x08': write(TOML_STRING_PREFIX("\\b"sv));	 break;
						case '\x09': write(TOML_STRING_PREFIX("\\t"sv));	 break;
						case '\x0A': write(TOML_STRING_PREFIX("\\n"sv));	 break;
						case '\x0B': write(TOML_STRING_PREFIX("\\u000B"sv)); break;
						case '\x0C': write(TOML_STRING_PREFIX("\\f"sv));	 break;
						case '\x0D': write(TOML_STRING_PREFIX("\\r"sv));	 break;
						case '\x0E': write(TOML_STRING_PREFIX("\\u000E"sv)); break;
						case '\x0F': write(TOML_STRING_PREFIX("\\u000F"sv)); break;
						case '\x10': write(TOML_STRING_PREFIX("\\u0010"sv)); break;
						case '\x11': write(TOML_STRING_PREFIX("\\u0011"sv)); break;
						case '\x12': write(TOML_STRING_PREFIX("\\u0012"sv)); break;
						case '\x13': write(TOML_STRING_PREFIX("\\u0013"sv)); break;
						case '\x14': write(TOML_STRING_PREFIX("\\u0014"sv)); break;
						case '\x15': write(TOML_STRING_PREFIX("\\u0015"sv)); break;
						case '\x16': write(TOML_STRING_PREFIX("\\u0016"sv)); break;
						case '\x17': write(TOML_STRING_PREFIX("\\u0017"sv)); break;
						case '\x18': write(TOML_STRING_PREFIX("\\u0018"sv)); break;
						case '\x19': write(TOML_STRING_PREFIX("\\u0019"sv)); break;
						case '\x1A': write(TOML_STRING_PREFIX("\\u001A"sv)); break;
						case '\x1B': write(TOML_STRING_PREFIX("\\u001B"sv)); break;
						case '\x1C': write(TOML_STRING_PREFIX("\\u001C"sv)); break;
						case '\x1D': write(TOML_STRING_PREFIX("\\u001D"sv)); break;
						case '\x1E': write(TOML_STRING_PREFIX("\\u001E"sv)); break;
						case '\x1F': write(TOML_STRING_PREFIX("\\u001F"sv)); break;
						TOML_NO_DEFAULT_CASE;
					}
				}
				else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
					write(TOML_STRING_PREFIX("\\u007F"sv));
				else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
					write(TOML_STRING_PREFIX("\\\""sv));
				else
					write(c);
			}

			void write_key_segment(const string& str)
			{
				if (str.empty())
					write("\"\""sv);
				else
				{
					bool requiresQuotes = false;
					{
						utf8_decoder decoder;
						for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
						{
							decoder(static_cast<uint8_t>(str[i]));
							if (decoder.error())
								requiresQuotes = true;
							else if (decoder.has_code_point())
								requiresQuotes = !is_bare_key_character(decoder.codepoint);
						}
					}

					if (requiresQuotes)
					{
						write('"');
						for (auto c : str)
							write_string_character(c);
						write('"');
					}
					else
						write(str);
				}
				naked_newline = false;
			}

			void write(const value<string>& val)
			{
				const auto& str = val.val_;
				if (str.empty())
					write("\"\""sv);
				else
				{
					write('"');
					for (auto c : str)
						write_string_character(c);
					write('"');
				}
				naked_newline = false;
			}

			template <typename T>
			void write_integer(T val)
			{
				char buf[20_sz]; //strlen("-9223372036854775808")
				const auto res = std::to_chars(buf, buf + sizeof(buf), val);
				write(buf, static_cast<size_t>(res.ptr - buf));
				naked_newline = false;
			}

			void write(const value<int64_t>& val)
			{
				write_integer(val.val_);
			}

			void write(const value<double>& val)
			{
				char buf[32_sz];
				const auto res = std::to_chars(buf, buf + sizeof(buf), val.val_);
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
				naked_newline = false;
			}

			void write(const value<bool>& val)
			{
				write(val.val_ ? "true"sv : "false"sv);
				naked_newline = false;
			}

			template <typename T>
			void write_zero_padded_integer(T intval, size_t digits)
			{
				static_assert(std::is_unsigned_v<T>);

				char buf[17_sz]; //strlen("9223372036854775807")
				const auto res = std::to_chars(buf, buf + sizeof(buf), intval);
				const auto len = static_cast<size_t>(res.ptr - buf);
				for (size_t i = len; i < digits; i++)
					write('0');
				write(buf, len);
				naked_newline = false;
			}

			void write(const time& tm)
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

			void write(const date& dt)
			{
				write_zero_padded_integer(dt.year, 4_sz);
				write('-');
				write_zero_padded_integer(dt.month, 2_sz);
				write('-');
				write_zero_padded_integer(dt.day, 2_sz);
			}

			void write(const datetime& dt)
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

			void write(const value<time>& val)
			{
				write(val.val_);
			}

			void write(const value<date>& val)
			{
				write(val.val_);
			}

			void write(const value<datetime>& val)
			{
				write(val.val_);
			}

			void write(const array& /*arr*/)
			{
				naked_newline = false;
			}

			void write_inline(const table& /*tab*/)
			{
				write("{"sv);

				write("}"sv);
				naked_newline = false;
			}

			void write(const table& tab)
			{
				//values, arrays, and inline tables
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type == node_type::table_array
						|| (type == node_type::table && !reinterpret_cast<const table*>(v.get())->is_inline()))
						continue;

					write_newline();
					write_indent();
					write_key_segment(k);
					write(" = "sv);
					switch (type)
					{
						case node_type::table: write_inline(*reinterpret_cast<const table*>(v.get())); break;
						case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
						case node_type::string: write(*reinterpret_cast<const value<string>*>(v.get())); break;
						case node_type::integer: write(*reinterpret_cast<const value<int64_t>*>(v.get())); break;
						case node_type::floating_point: write(*reinterpret_cast<const value<double>*>(v.get())); break;
						case node_type::boolean: write(*reinterpret_cast<const value<bool>*>(v.get())); break;
						case node_type::date: write(*reinterpret_cast<const value<date>*>(v.get())); break;
						case node_type::time: write(*reinterpret_cast<const value<time>*>(v.get())); break;
						case node_type::datetime: write(*reinterpret_cast<const value<datetime>*>(v.get())); break;
						TOML_NO_DEFAULT_CASE;
					}
				}

				//tables
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type != node_type::table)
						continue;
					auto& node = *reinterpret_cast<const table*>(v.get());

					indent++;

					write_newline();
					write_newline(true);
					write_indent();
					write("["sv);

					for (const auto& segment : key_path)
					{
						write_key_segment(segment);
						write('.');
					}
					write_key_segment(k);
					key_path.push_back(k);

					write("]"sv);
					write_newline(true);
					write(node);

					key_path.pop_back();

					indent--;
				}

				//table arrays
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type != node_type::table_array)
						continue;
					auto& node = *reinterpret_cast<const table_array*>(v.get());

					indent++;
					key_path.push_back(k);

					for (auto& t : node.tables)
					{
						write_newline();
						write_newline(true);
						write_indent();
						write("[["sv);
						for (const auto& segment : key_path)
						{
							if (&segment > key_path.data())
								write('.');
							write_key_segment(segment);
						}
						write("]]"sv);
						write_newline(true);
						write(*t);
					}

					key_path.pop_back();
					indent--;
				}
			}

		public:
			virtual ~writer() noexcept = default;

			void operator() (const table& tab)
			{
				indent = -1;
				naked_newline = true;
				key_path.clear();
				write(tab);
			}
	};

	template <typename CHAR>
	class writer<std::basic_string<CHAR>> final : public writer<void>
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_string<CHAR>& target;

			void write(const void* data, size_t size) override
			{
				TOML_ASSERT(data && size);
				target.append(std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(data), size });
			}

			void write(char byte) override
			{
				target += static_cast<CHAR>(byte);
			}

		public:
			explicit writer(std::basic_ostream<CHAR>& target_) noexcept
				: target{ target_ }
			{
			}
	};

	template <typename CHAR>
	writer(std::basic_string<CHAR>&) -> writer<std::basic_string<CHAR>>;

	template <typename CHAR>
	class writer<std::basic_ostream<CHAR>> final : public writer<void>
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_ostream<CHAR>& target;

			void write(const void* data, size_t size) override
			{
				TOML_ASSERT(data && size);
				target.write(reinterpret_cast<const CHAR*>(data), static_cast<std::streamsize>(size));
			}

			void write(char byte) override
			{
				target.put(static_cast<CHAR>(byte));
			}

		public:
			explicit writer(std::basic_ostream<CHAR>& target_) noexcept
				: target{ target_ }
			{
			}
	};

	template <typename CHAR>
	writer(std::basic_ostream<CHAR>&) -> writer<std::basic_ostream<CHAR>>;
}

namespace toml
{
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		impl::writer{ lhs }(rhs);
		return lhs;
	}
}
