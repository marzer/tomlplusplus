#pragma once
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"

namespace toml::impl
{
	struct TOML_INTERFACE formatter_writer_interface
	{
		virtual void operator() (const void*, size_t) const TOML_MAY_THROW = 0;
		virtual void operator() (char) const TOML_MAY_THROW = 0;

		virtual ~formatter_writer_interface() noexcept = default;
	};

	template <typename T>
	class formatter_writer;

	template <typename CHAR>
	class TOML_EMPTY_BASES formatter_writer<std::basic_ostream<CHAR>> : public formatter_writer_interface
	{
		private:
			std::basic_ostream<CHAR>& target;

		public:

			void operator() (const void* data, size_t size) const TOML_MAY_THROW override
			{
				TOML_ASSERT(data && size);
				target.write(reinterpret_cast<const CHAR*>(data), static_cast<std::streamsize>(size));
			}

			void operator() (char character) const TOML_MAY_THROW override
			{
				target.put(static_cast<CHAR>(character));
			}

			formatter_writer(std::basic_ostream<CHAR>& target_) noexcept
				: target{ target_ }
			{}
	};

	template <typename CHAR>
	formatter_writer(std::basic_ostream<CHAR>&) -> formatter_writer<std::basic_ostream<CHAR>>;
}

namespace toml
{
	class default_formatter final
	{
		private:
			const toml::table& source;
			std::string_view indent_string;
			size_t indent_string_columns = {};

			mutable int indent_level;
			mutable bool naked_newline;
			mutable std::vector<toml::string> key_path;
			mutable const impl::formatter_writer_interface* writer = nullptr;

			TOML_ALWAYS_INLINE
			void write(char character) const TOML_MAY_THROW
			{
				(*writer)(character);
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(std::basic_string_view<CHAR> strv) const TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer)(reinterpret_cast<const uint8_t*>(strv.data()), strv.length());
			}

			template <typename CHAR>
			TOML_ALWAYS_INLINE
			void write(const std::basic_string<CHAR>& str) const TOML_MAY_THROW
			{
				static_assert(sizeof(CHAR) == 1_sz);

				(*writer)(reinterpret_cast<const uint8_t*>(str.data()), str.length());
			}

			void write_newline(bool force = false) const TOML_MAY_THROW
			{
				if (!naked_newline || force)
				{
					write('\n');
					naked_newline = true;
				}
			}

			void write_indent() const TOML_MAY_THROW
			{
				for (int i = 0; i < indent_level; i++)
				{
					write(indent_string);
					naked_newline = false;
				}
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_ALL_WARNINGS // some compilers will complain about a tautological unsigned >= 0.
									  // TINAE - char can have signed _or_ unsigned semantics and I can't
									  // be arsed handling this differently

			static toml::string_view escape_string_character(const toml::string_char& c) noexcept
			{
				if (c >= toml::string_char{} && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
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
					return toml::string_view{ &c, 1_sz };
			}

			TOML_POP_WARNINGS

			static toml::string make_key_segment(const toml::string& str) noexcept
			{
				if (str.empty())
					return TOML_STRING_PREFIX("''"s);
				else
				{
					bool requiresQuotes = false;
					{
						impl::utf8_decoder decoder;
						for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
						{
							decoder(static_cast<uint8_t>(str[i]));
							if (decoder.error())
								requiresQuotes = true;
							else if (decoder.has_code_point())
								requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
						}
					}

					if (requiresQuotes)
					{
						toml::string s;
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

			void write_key_segment(const toml::string& str) const TOML_MAY_THROW
			{
				if (str.empty())
					write("''"sv);
				else
				{
					bool requiresQuotes = false;
					{
						impl::utf8_decoder decoder;
						for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
						{
							decoder(static_cast<uint8_t>(str[i]));
							if (decoder.error())
								requiresQuotes = true;
							else if (decoder.has_code_point())
								requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
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

			void write_key_path() const TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						write('.');
					write(segment);
				}
				naked_newline = false;
			}

			void write(const value<toml::string>& val) const TOML_MAY_THROW
			{
				const auto& str = val.get();
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
			void write_integer(T val) const TOML_MAY_THROW
			{
				char buf[20_sz]; //strlen("-9223372036854775808")
				const auto res = std::to_chars(buf, buf + sizeof(buf), val);
				(*writer)(buf, static_cast<size_t>(res.ptr - buf));
				naked_newline = false;
			}

			void write(const value<int64_t>& val) const TOML_MAY_THROW
			{
				write_integer(val.get());
			}

			void write(const value<double>& val) const TOML_MAY_THROW
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

			void write(const value<bool>& val) const TOML_MAY_THROW
			{
				write(val.get() ? "true"sv : "false"sv);
				naked_newline = false;
			}

			template <typename T>
			void write_zero_padded_integer(T intval, size_t digits) const TOML_MAY_THROW
			{
				static_assert(std::is_unsigned_v<T>);

				char buf[17_sz]; //strlen("9223372036854775807")
				const auto res = std::to_chars(buf, buf + sizeof(buf), intval);
				const auto len = static_cast<size_t>(res.ptr - buf);
				for (size_t i = len; i < digits; i++)
					write('0');
				(*writer)(buf, len);
				naked_newline = false;
			}

			void write(const time& tm) const TOML_MAY_THROW
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

			void write(const date& dt) const TOML_MAY_THROW
			{
				write_zero_padded_integer(dt.year, 4_sz);
				write('-');
				write_zero_padded_integer(dt.month, 2_sz);
				write('-');
				write_zero_padded_integer(dt.day, 2_sz);
			}

			void write(const date_time& dt) const TOML_MAY_THROW
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

			void write(const value<time>& val) const TOML_MAY_THROW
			{
				write(val.get());
			}

			void write(const value<date>& val) const TOML_MAY_THROW
			{
				write(val.get());
			}

			void write(const value<date_time>& val) const TOML_MAY_THROW
			{
				write(val.get());
			}

			inline void write_inline(const table& /*tab*/) const TOML_MAY_THROW;

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
						size_t weight = 3_sz; // "[ ]"
						for (auto& v : values)
							weight += inline_columns(*v) + 2_sz; // +  ", "
						return weight;
					}
					case node_type::string:
						return reinterpret_cast<const value<string>*>(&node)->get().length() + 2_sz; // + ""

					case node_type::integer:
					{
						auto v = reinterpret_cast<const value<int64_t>*>(&node)->get();
						if (!v)
							return 1_sz;
						size_t weight = {};
						if (v < 0)
						{
							weight += 1;
							v *= -1;
						}
						return weight + static_cast<size_t>(std::log10(static_cast<double>(v)));
					}

					case node_type::floating_point:
					{
						auto v = reinterpret_cast<const value<double>*>(&node)->get();
						if (v == 0.0)
							return 3_sz;
						size_t weight = 2_sz; // ".0"
						if (v < 0.0)
						{
							weight += 1;
							v *= -1.0;
						}
						return weight + static_cast<size_t>(std::log10(v));
					}

					case node_type::boolean: return 5_sz;
					case node_type::date: [[fallthrough]];
					case node_type::time: return 10_sz;
					case node_type::date_time: return 30_sz;
					TOML_NO_DEFAULT_CASE;
				}
			}

			static bool forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
			{
				return (inline_columns(node) + starting_column_bias) > 100_sz;
			}

			void write(const array& arr) const TOML_MAY_THROW
			{
				if (arr.values.empty())
					write("[]"sv);
				else
				{
					const auto multiline = forces_multiline(
						arr,
						indent_string_columns * static_cast<size_t>(indent_level < 0 ? 0 : indent_level)
					);
					const auto original_indent = indent_level;
					write("["sv);
					if (multiline)
					{
						if (indent_level < 0)
							indent_level++;
						indent_level++;
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
							case node_type::date_time: write(*reinterpret_cast<const value<date_time>*>(v.get())); break;
							TOML_NO_DEFAULT_CASE;
						}

					}
					if (multiline)
					{
						indent_level = original_indent;
						write_newline(true);
						write_indent();
					}
					else
						write(' ');
					write("]"sv);
				}
				naked_newline = false;
			}

			void write(const table& tab) const TOML_MAY_THROW
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
						case node_type::date_time: write(*reinterpret_cast<const value<date_time>*>(v.get())); break;
						TOML_NO_DEFAULT_CASE;
					}
				}

				//non-inline tables
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type != node_type::table || reinterpret_cast<const table*>(v.get())->is_inline())
						continue;
					auto& child_tab = *reinterpret_cast<const table*>(v.get());

					//we can skip indenting and emitting the headers for tables that only contain other tables
					//(so we don't over-nest)
					size_t child_value_count{}; //includes inline tables and non-table arrays
					size_t child_table_count{};
					size_t child_table_array_count{};
					for (auto& [child_k, child_v] : child_tab.values)
					{
						const auto child_type = child_v->type();
						switch (child_type)
						{
							case node_type::table:
								if (reinterpret_cast<const table*>(child_v.get())->is_inline())
									child_value_count++;
								else
									child_table_count++;
								break;

							case node_type::array:
								if (reinterpret_cast<const array*>(child_v.get())->is_array_of_tables())
									child_table_array_count++;
								else
									child_value_count++;
								break;

							default:
								child_value_count++;
						}
					}
					bool skip_self = false;
					if (child_value_count == 0_sz && (child_table_count > 0_sz || child_table_array_count > 0_sz))
						skip_self = true;

					if (!skip_self)
						indent_level++;
					key_path.push_back(make_key_segment(k));

					if (!skip_self)
					{
						write_newline();
						write_newline(true);
						write_indent();
						write("["sv);
						write_key_path();
						write("]"sv);
						write_newline(true);
					}

					write(child_tab);

					key_path.pop_back();
					if (!skip_self)
						indent_level--;
				}

				//table arrays
				for (auto& [k, v] : tab.values)
				{
					const auto type = v->type();
					if (type != node_type::array || !reinterpret_cast<const array*>(v.get())->is_array_of_tables())
						continue;
					auto& node = *reinterpret_cast<const array*>(v.get());

					indent_level++;
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
					indent_level--;
				}
			}

		public:

			explicit default_formatter(const toml::table& source_, std::string_view indent_string_ = "    "sv) noexcept
				: source{ source_ },
				indent_string{ indent_string_ }
			{
				for (auto c : indent_string)
					indent_string_columns += c == '\t' ? 4_sz : 1_sz;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const default_formatter& rhs) TOML_MAY_THROW
			{
				static_assert(
					sizeof(CHAR) == 1,
					"The stream's underlying character type must be 1 byte in size."
				);

				rhs.indent_level = -1;
				rhs.naked_newline = true;
				rhs.key_path.clear();
				auto fw = impl::formatter_writer{ lhs };
				rhs.writer = &fw;
				rhs.write(rhs.source);

				return lhs;
			}
	};

	inline void default_formatter::write_inline(const toml::table& tab) const TOML_MAY_THROW
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
					case node_type::date_time: write(*reinterpret_cast<const value<date_time>*>(v.get())); break;
					TOML_NO_DEFAULT_CASE;
				}

			}

			write(" }"sv);
		}
		naked_newline = false;
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return lhs << default_formatter{ rhs };
	}
}
