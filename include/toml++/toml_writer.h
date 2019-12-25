#pragma once
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"

namespace toml::impl
{
	template <>
	class TOML_INTERFACE writer<void>
	{
		protected:
			virtual void write(const void*, size_t) TOML_MAY_THROW = 0;
			virtual void write(char) TOML_MAY_THROW = 0;

		private:
			int indent;
			bool naked_newline;
			std::vector<string> key_path;
			static constexpr std::string_view indent_string = TOML_INDENT;
			static constexpr auto indent_string_columns = ([](auto str) noexcept
			{   
				size_t cols = {};
				for (auto c : str)
					cols += c == '\t' ? 4_sz : 1_sz;
				return cols;
			})(indent_string);

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(std::basic_string_view<CHAR> strv) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				write(reinterpret_cast<const uint8_t*>(strv.data()), strv.length());
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const std::basic_string<CHAR>& str) TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				write(reinterpret_cast<const uint8_t*>(str.data()), str.length());
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
				for (int i = 0; i < indent; i++)
				{
					write(indent_string);
					naked_newline = false;
				}
			}

			static string_view escape_string_character(const string_char& c) noexcept
			{
				if (c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
				{
					switch (c)
					{
						case TOML_STRING_PREFIX('\x00'): return TOML_STRING_PREFIX("\\u0000"sv);
						case TOML_STRING_PREFIX('\x01'): return TOML_STRING_PREFIX("\\u0001"sv);
						case TOML_STRING_PREFIX('\x02'): return TOML_STRING_PREFIX("\\u0002"sv);
						case TOML_STRING_PREFIX('\x03'): return TOML_STRING_PREFIX("\\u0003"sv);
						case TOML_STRING_PREFIX('\x04'): return TOML_STRING_PREFIX("\\u0004"sv);
						case TOML_STRING_PREFIX('\x05'): return TOML_STRING_PREFIX("\\u0005"sv);
						case TOML_STRING_PREFIX('\x06'): return TOML_STRING_PREFIX("\\u0006"sv);
						case TOML_STRING_PREFIX('\x07'): return TOML_STRING_PREFIX("\\u0007"sv);
						case TOML_STRING_PREFIX('\x08'): return TOML_STRING_PREFIX("\\b"sv);
						case TOML_STRING_PREFIX('\x09'): return TOML_STRING_PREFIX("\\t"sv);
						case TOML_STRING_PREFIX('\x0A'): return TOML_STRING_PREFIX("\\n"sv);
						case TOML_STRING_PREFIX('\x0B'): return TOML_STRING_PREFIX("\\u000B"sv);
						case TOML_STRING_PREFIX('\x0C'): return TOML_STRING_PREFIX("\\f"sv);
						case TOML_STRING_PREFIX('\x0D'): return TOML_STRING_PREFIX("\\r"sv);
						case TOML_STRING_PREFIX('\x0E'): return TOML_STRING_PREFIX("\\u000E"sv);
						case TOML_STRING_PREFIX('\x0F'): return TOML_STRING_PREFIX("\\u000F"sv);
						case TOML_STRING_PREFIX('\x10'): return TOML_STRING_PREFIX("\\u0010"sv);
						case TOML_STRING_PREFIX('\x11'): return TOML_STRING_PREFIX("\\u0011"sv);
						case TOML_STRING_PREFIX('\x12'): return TOML_STRING_PREFIX("\\u0012"sv);
						case TOML_STRING_PREFIX('\x13'): return TOML_STRING_PREFIX("\\u0013"sv);
						case TOML_STRING_PREFIX('\x14'): return TOML_STRING_PREFIX("\\u0014"sv);
						case TOML_STRING_PREFIX('\x15'): return TOML_STRING_PREFIX("\\u0015"sv);
						case TOML_STRING_PREFIX('\x16'): return TOML_STRING_PREFIX("\\u0016"sv);
						case TOML_STRING_PREFIX('\x17'): return TOML_STRING_PREFIX("\\u0017"sv);
						case TOML_STRING_PREFIX('\x18'): return TOML_STRING_PREFIX("\\u0018"sv);
						case TOML_STRING_PREFIX('\x19'): return TOML_STRING_PREFIX("\\u0019"sv);
						case TOML_STRING_PREFIX('\x1A'): return TOML_STRING_PREFIX("\\u001A"sv);
						case TOML_STRING_PREFIX('\x1B'): return TOML_STRING_PREFIX("\\u001B"sv);
						case TOML_STRING_PREFIX('\x1C'): return TOML_STRING_PREFIX("\\u001C"sv);
						case TOML_STRING_PREFIX('\x1D'): return TOML_STRING_PREFIX("\\u001D"sv);
						case TOML_STRING_PREFIX('\x1E'): return TOML_STRING_PREFIX("\\u001E"sv);
						case TOML_STRING_PREFIX('\x1F'): return TOML_STRING_PREFIX("\\u001F"sv);
						TOML_NO_DEFAULT_CASE;
					}
				}
				else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
					return TOML_STRING_PREFIX("\\u007F"sv);
				else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
					return TOML_STRING_PREFIX("\\\""sv);
				else
					return string_view{ &c, 1_sz };
			}

			static string make_key_segment(const string& str) noexcept
			{
				if (str.empty())
					return TOML_STRING_PREFIX("\"\""s);
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
						string s;
						s.reserve(str.length() + 2_sz);
						s += TOML_STRING_PREFIX('"');
						for (auto c : str)
							s.append(escape_string_character(c));
						s += TOML_STRING_PREFIX('"');
						return s;
					}
					else
						return str;
				}
			}

			void write_key_segment(const string& str) TOML_MAY_THROW
			{
				if (str.empty())
					write("''"sv);
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
							write(escape_string_character(c));
						write('"');
					}
					else
						write(str);
				}
			}

			void write_key_path() TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						write('.');
					write_key_segment(segment);
				}
				naked_newline = false;
			}

			void write(const value<string>& val) TOML_MAY_THROW
			{
				const auto& str = val.val_;
				if (str.empty())
					write("''"sv);
				else
				{
					write('"');
					for (auto c : str)
						write(escape_string_character(c));
					write('"');
				}
				naked_newline = false;
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
				write_integer(val.val_);
			}

			void write(const value<double>& val) TOML_MAY_THROW
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

			void write(const value<bool>& val) TOML_MAY_THROW
			{
				write(val.val_ ? "true"sv : "false"sv);
				naked_newline = false;
			}

			template <typename T>
			void write_zero_padded_integer(T intval, size_t digits) TOML_MAY_THROW
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

			void write(const datetime& dt) TOML_MAY_THROW
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
				write(val.val_);
			}

			void write(const value<date>& val) TOML_MAY_THROW
			{
				write(val.val_);
			}

			void write(const value<datetime>& val) TOML_MAY_THROW
			{
				write(val.val_);
			}

			void write_inline(const table& /*tab*/) TOML_MAY_THROW;

			static size_t inline_columns(const node& node) noexcept
			{
				switch (node.type())
				{
					case node_type::table:  //assumed to be inline
					{
						auto& values = reinterpret_cast<const table*>(&node)->values;
						if (values.empty())
							return 2_sz; // "{}"
						size_t weight = 3_sz; // "{ }"
						for (auto& [k, v] : values)
							weight += k.length() + inline_columns(*v) + 2_sz; // +  ", "
						return weight;
					}
					case node_type::array:
					{
						auto& values = reinterpret_cast<const array*>(&node)->values;
						if (values.empty())
							return 2_sz; // "[]"
						size_t weight = 3_sz; // "[]"
						for (auto& v : values)
							weight += inline_columns(*v) + 2_sz; // +  ", "
						return weight;
					}
					case node_type::string:
						return reinterpret_cast<const value<string>*>(&node)->val_.length() + 2_sz; // + ""

					case node_type::integer:
					{
						auto v = reinterpret_cast<const value<int64_t>*>(&node)->val_;
						if (!v)
							return 1_sz;
						size_t weight = {};
						if (v < 0)
						{
							weight += 1;
							v *= -1;
						}
						return static_cast<size_t>(log10(static_cast<double>(v)));
					}

					case node_type::floating_point:
					{
						auto v = reinterpret_cast<const value<double>*>(&node)->val_;
						if (v == 0.0)
							return 3_sz;
						size_t weight = 2_sz; // ".0"
						if (v < 0.0)
						{
							weight += 1;
							v *= -1.0;
						}
						return static_cast<size_t>(log10(v));
					}

					case node_type::boolean: return 5_sz;
					case node_type::date: [[fallthrough]];
					case node_type::time: return 10_sz;
					case node_type::datetime: return 30_sz;
					TOML_NO_DEFAULT_CASE;
				}
			}

			static bool forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
			{
				return (inline_columns(node) + starting_column_bias) > 80_sz;
			}

			void write(const array& arr) TOML_MAY_THROW
			{
				if (arr.values.empty())
					write("[]"sv);
				else
				{
					const auto multiline = forces_multiline(arr, indent_string_columns * static_cast<size_t>(indent < 0 ? 0 : indent));
					const auto original_indent = indent;
					write("["sv);
					if (multiline)
					{
						if (indent < 0)
							indent++;
						indent++;
					}
					else
						write(' ');

					for (auto& v : arr.values)
					{
						if (std::addressof(v) > arr.values.data())
						{
							write(',');
							if (!multiline)
								write(' ');
						}

						if (multiline)
						{
							write_newline(true);
							write_indent();
						}

						const auto type = v->type();
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
					if (multiline)
					{
						indent = original_indent;
						write_newline(true);
						write_indent();
					}
					else
						write(' ');
					write("]"sv);
				}
				naked_newline = false;
			}

			void write(const table& tab) TOML_MAY_THROW
			{
				//values, arrays, and inline tables
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(v.get())->is_inline())
						|| (type == node_type::array && reinterpret_cast<const array*>(v.get())->is_array_of_tables()))
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

				//non-inline tables
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type != node_type::table || reinterpret_cast<const table*>(v.get())->is_inline())
						continue;
					auto& node = *reinterpret_cast<const table*>(v.get());

					indent++;
					key_path.push_back(make_key_segment(k));

					write_newline();
					write_newline(true);
					write_indent();
					write("["sv);
					write_key_path();
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
					if (type != node_type::array || !reinterpret_cast<const array*>(v.get())->is_array_of_tables())
						continue;
					auto& node = *reinterpret_cast<const array*>(v.get());

					indent++;
					key_path.push_back(make_key_segment(k));

					for (auto& t : node.values)
					{
						write_newline();
						write_newline(true);
						write_indent();
						write("[["sv);
						write_key_path();
						write("]]"sv);
						write_newline(true);
						write(*reinterpret_cast<const table*>(t.get()));
					}

					key_path.pop_back();
					indent--;
				}
			}

		public:
			virtual ~writer() noexcept = default;

			void operator() (const table& tab) TOML_MAY_THROW
			{
				indent = -1;
				naked_newline = true;
				key_path.clear();
				write(tab);
			}
	};

	void writer<void>::write_inline(const toml::table& tab) TOML_MAY_THROW
	{
		if (tab.values.empty())
			write("{}"sv);
		else
		{
			write("{ "sv);

			bool first = false;
			for (auto& [k, v] : tab.values)
			{
				if (first)
				{
					write(", "sv);
				}
				first = true;

				write_key_segment(k);
				write(" = "sv);

				const auto type = v->type();
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

			write(" }"sv);
		}
		naked_newline = false;
	}

	template <typename CHAR>
	class writer<std::basic_string<CHAR>> final : public writer<void>
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_string<CHAR>& target;

			void write(const void* data, size_t size) TOML_MAY_THROW override
			{
				TOML_ASSERT(data && size);
				target.append(std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(data), size });
			}

			void write(char byte) TOML_MAY_THROW override
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

			void write(const void* data, size_t size) TOML_MAY_THROW override
			{
				TOML_ASSERT(data && size);
				target.write(reinterpret_cast<const CHAR*>(data), static_cast<std::streamsize>(size));
			}

			void write(char byte) TOML_MAY_THROW override
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
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		impl::writer{ lhs }(rhs);
		return lhs;
	}
}
