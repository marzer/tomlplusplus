#pragma once
#include "toml_formatter.h"
#include "toml_table.h"
#include "toml_array.h"
#include "toml_utf8.h"

namespace toml
{
	class default_formatter final : impl::formatter
	{
		private:
			using base = impl::formatter;
			std::vector<toml::string> key_path;

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

			void write_key_segment(const toml::string& str) TOML_MAY_THROW
			{
				if (str.empty())
					base::write("''"sv);
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
						write_quoted_string(str);
					else
						base::write(str);
				}
			}

			void write_key_path() TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						base::write('.');
					base::write(segment);
				}
				naked_newline = false;
			}

			inline void write_inline(const table& /*tbl*/) TOML_MAY_THROW;

			static size_t inline_columns(const node& node) noexcept
			{
				return node.visit([](const auto& n) noexcept
					-> size_t
				{
					using node_t = impl::remove_cvref_t<decltype(n)>;
					if constexpr (std::is_same_v<node_t, table>)
					{
						const auto& values = n.values;
						if (values.empty())
							return 2_sz; // "{}"
						size_t weight = 3_sz; // "{ }"
						for (const auto& [k, v] : values)
							weight += k.length() + inline_columns(*v) + 2_sz; // +  ", "
						return weight;
					}
					else if constexpr (std::is_same_v<node_t, array>)
					{
						if (n.empty())
							return 2_sz; // "[]"
						size_t weight = 3_sz; // "[ ]"
						for (size_t i = 0; i < n.size(); i++)
							weight += inline_columns(*n.get(i)) + 2_sz; // +  ", "
						return weight;
					}
					else if constexpr (std::is_same_v<node_t, value<string>>)
					{
						return n.get().length() + 2_sz; // + ""
					}
					else if constexpr (std::is_same_v<node_t, value<int64_t>>)
					{
						auto v = n.get();
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
					else if constexpr (std::is_same_v<node_t, value<double>>)
					{
						auto v = n.get();
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
					else if constexpr (std::is_same_v<node_t, value<bool>>)
					{
						return 5_sz;
					}
					else if constexpr (std::is_same_v<node_t, value<date>> || std::is_same_v<node_t, value<time>>)
					{
						return 10_sz;
					}
					else if constexpr (std::is_same_v<node_t, value<date_time>>)
					{
						return 30_sz;
					}
					TOML_UNREACHABLE;
				});
			}

			static bool forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
			{
				return (inline_columns(node) + starting_column_bias) > 120_sz;
			}

			void write(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					base::write("[]"sv);
				else
				{
					const auto multiline = forces_multiline(
						arr,
						base::indent_string_columns * static_cast<size_t>(indent_level < 0 ? 0 : indent_level)
					);
					const auto original_indent = indent_level;
					base::write("["sv);
					if (multiline)
					{
						if (indent_level < 0)
							indent_level++;
						indent_level++;
					}
					else
						base::write(' ');

					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
						{
							base::write(',');
							if (!multiline)
								base::write(' ');
						}

						if (multiline)
						{
							write_newline(true);
							write_indent();
						}

						auto v = arr.get(i);
						const auto type = v->type();
						switch (type)
						{
							case node_type::table: write_inline(*reinterpret_cast<const table*>(v)); break;
							case node_type::array: write(*reinterpret_cast<const array*>(v)); break;
							default:
								write_value(v, type);
						}

					}
					if (multiline)
					{
						indent_level = original_indent;
						write_newline(true);
						write_indent();
					}
					else
						base::write(' ');
					base::write("]"sv);
				}
				naked_newline = false;
			}

			void write(const table& tbl) TOML_MAY_THROW
			{
				//values, arrays, and inline tables
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(v.get())->is_inline())
						|| (type == node_type::array && reinterpret_cast<const array*>(v.get())->is_array_of_tables()))
						continue;

					write_newline();
					write_indent();
					write_key_segment(k);
					base::write(" = "sv);
					switch (type)
					{
						case node_type::table: write_inline(*reinterpret_cast<const table*>(v.get())); break;
						case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
						default:
							write_value(v.get(), type);
					}
				}

				//non-inline tables
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if (type != node_type::table || reinterpret_cast<const table*>(v.get())->is_inline())
						continue;
					auto& child_tbl = *reinterpret_cast<const table*>(v.get());

					//we can skip indenting and emitting the headers for tables that only contain other tables
					//(so we don't over-nest)
					size_t child_value_count{}; //includes inline tables and non-table arrays
					size_t child_table_count{};
					size_t child_table_array_count{};
					for (auto& [child_k, child_v] : child_tbl.values)
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
						base::write("["sv);
						write_key_path();
						base::write("]"sv);
						write_newline(true);
					}

					write(child_tbl);

					key_path.pop_back();
					if (!skip_self)
						indent_level--;
				}

				//table arrays
				for (const auto& [k, v] : tbl.values)
				{
					const auto type = v->type();
					if (type != node_type::array || !reinterpret_cast<const array*>(v.get())->is_array_of_tables())
						continue;
					auto& arr = *reinterpret_cast<const array*>(v.get());

					indent_level++;
					key_path.push_back(make_key_segment(k));

					for (size_t i = 0; i < arr.size(); i++)
					{
						write_newline();
						write_newline(true);
						write_indent();
						base::write("[["sv);
						write_key_path();
						base::write("]]"sv);
						write_newline(true);
						write(*reinterpret_cast<const table*>(arr.get(i)));
					}

					key_path.pop_back();
					indent_level--;
				}
			}

			void write() TOML_MAY_THROW
			{
				write(source);
			}

		public:

			TOML_NODISCARD_CTOR
			default_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: formatter{
					source_,
					impl::formatter_options{
						indent_string,
						false //quote_dates_and_times
					}
				}
			{}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, default_formatter& rhs)
				TOML_MAY_THROW
			{
				auto fw = impl::formatter_writer{ lhs };
				rhs.attach(fw);
				rhs.indent_level--; //starts at -1 so root kvps and first-level child tables have the same indent
				rhs.key_path.clear();
				rhs.write();
				rhs.detach();
				return lhs;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, default_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	inline void default_formatter::write_inline(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.values.empty())
			base::write("{}"sv);
		else
		{
			base::write("{ "sv);

			bool first = false;
			for (auto& [k, v] : tbl.values)
			{
				if (first)
					base::write(", "sv);
				first = true;

				write_key_segment(k);
				base::write(" = "sv);

				const auto type = v->type();
				switch (type)
				{
					case node_type::table: write_inline(*reinterpret_cast<const table*>(v.get())); break;
					case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
					default:
						write_value(v.get(), type);
				}
			}

			base::write(" }"sv);
		}
		naked_newline = false;
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		return lhs << default_formatter{ rhs };
	}
}
