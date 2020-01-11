#pragma once
#include "toml_formatter.h"
#include "toml_table.h"
#include "toml_array.h"
#include "toml_utf8.h"

namespace toml::impl
{
	[[nodiscard]]
	inline toml::string default_formatter_make_key_segment(const toml::string& str) noexcept
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

	[[nodiscard]]
	inline size_t default_formatter_inline_columns(const node& node) noexcept
	{
		return node.visit([](const auto& n) noexcept
			-> size_t
			{
				using node_t = impl::remove_cvref_t<decltype(n)>;
				if constexpr (std::is_same_v<node_t, table>)
				{
					if (n.empty())
						return 2_sz; // "{}"
					size_t weight = 3_sz; // "{ }"
					for (auto [k, v] : n)
						weight += k.length() + default_formatter_inline_columns(v) + 2_sz; // +  ", "
					return weight;
				}
				else if constexpr (std::is_same_v<node_t, array>)
				{
					if (n.empty())
						return 2_sz; // "[]"
					size_t weight = 3_sz; // "[ ]"
					for (auto& elem : n)
						weight += default_formatter_inline_columns(elem) + 2_sz; // +  ", "
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

	[[nodiscard]]
	inline bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias = 0) noexcept
	{
		return (default_formatter_inline_columns(node) + starting_column_bias) > 120_sz;
	}
}

namespace toml
{
	template <typename CHAR = char>
	class default_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;
			std::vector<toml::string> key_path;

			void print_key_segment(const toml::string& str) TOML_MAY_THROW
			{
				if (str.empty())
					impl::print_to_stream("''"sv, base::stream());
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
						base::print_quoted_string(str);
					else
						impl::print_to_stream(str, base::stream());
				}
				base::clear_naked_newline();
			}

			void print_key_path() TOML_MAY_THROW
			{
				for (const auto& segment : key_path)
				{
					if (std::addressof(segment) > key_path.data())
						impl::print_to_stream('.', base::stream());
					impl::print_to_stream(segment, base::stream());
				}
				base::clear_naked_newline();
			}

			inline void print_inline(const table& /*tbl*/) TOML_MAY_THROW;

			void print(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					impl::print_to_stream("[]"sv, base::stream());
				else
				{
					const auto original_indent = base::indent();
					const auto multiline = impl::default_formatter_forces_multiline(
						arr,
						base::indent_columns() * static_cast<size_t>(original_indent < 0 ? 0 : original_indent)
					);
					impl::print_to_stream("["sv, base::stream());
					if (multiline)
					{
						if (original_indent < 0)
							base::indent(0);
						base::increase_indent();
					}
					else
						impl::print_to_stream(' ', base::stream());

					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
						{
							impl::print_to_stream(',', base::stream());
							if (!multiline)
								impl::print_to_stream(' ', base::stream());
						}

						if (multiline)
						{
							base::print_newline(true);
							base::print_indent();
						}

						auto& v = arr[i];
						const auto type = v.type();
						switch (type)
						{
							case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print(v, type);
						}

					}
					if (multiline)
					{
						base::indent(original_indent);
						base::print_newline(true);
						base::print_indent();
					}
					else
						impl::print_to_stream(' ', base::stream());
					impl::print_to_stream("]"sv, base::stream());
				}
				base::clear_naked_newline();
			}

			void print(const table& tbl) TOML_MAY_THROW
			{
				//values, arrays, and inline tables
				for (auto [k, v] : tbl)
				{
					const auto type = v.type();
					if ((type == node_type::table && !reinterpret_cast<const table*>(&v)->is_inline())
						|| (type == node_type::array && reinterpret_cast<const array*>(&v)->is_array_of_tables()))
						continue;

					base::print_newline();
					base::print_indent();
					print_key_segment(k);
					impl::print_to_stream(" = "sv, base::stream());
					switch (type)
					{
						case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
						case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
						default:
							base::print(v, type);
					}
				}

				//non-inline tables
				for (auto [k, v] : tbl)
				{
					const auto type = v.type();
					if (type != node_type::table || reinterpret_cast<const table*>(&v)->is_inline())
						continue;
					auto& child_tbl = *reinterpret_cast<const table*>(&v);

					//we can skip indenting and emitting the headers for tables that only contain other tables
					//(so we don't over-nest)
					size_t child_value_count{}; //includes inline tables and non-table arrays
					size_t child_table_count{};
					size_t child_table_array_count{};
					for (auto [child_k, child_v] : child_tbl)
					{
						const auto child_type = child_v.type();
						switch (child_type)
						{
							case node_type::table:
								if (reinterpret_cast<const table*>(&child_v)->is_inline())
									child_value_count++;
								else
									child_table_count++;
								break;

							case node_type::array:
								if (reinterpret_cast<const array*>(&child_v)->is_array_of_tables())
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
						base::increase_indent();
					key_path.push_back(impl::default_formatter_make_key_segment(k));

					if (!skip_self)
					{
						base::print_newline();
						base::print_newline(true);
						base::print_indent();
						impl::print_to_stream("["sv, base::stream());
						print_key_path();
						impl::print_to_stream("]"sv, base::stream());
						base::print_newline(true);
					}

					print(child_tbl);

					key_path.pop_back();
					if (!skip_self)
						base::decrease_indent();
				}

				//table arrays
				for (auto [k, v] : tbl)
				{
					const auto type = v.type();
					if (type != node_type::array || !reinterpret_cast<const array*>(&v)->is_array_of_tables())
						continue;
					auto& arr = *reinterpret_cast<const array*>(&v);

					base::increase_indent();
					key_path.push_back(impl::default_formatter_make_key_segment(k));

					for (size_t i = 0; i < arr.size(); i++)
					{
						base::print_newline();
						base::print_newline(true);
						base::print_indent();
						impl::print_to_stream("[["sv, base::stream());
						print_key_path();
						impl::print_to_stream("]]"sv, base::stream());
						base::print_newline(true);
						print(*reinterpret_cast<const table*>(&arr[i]));
					}

					key_path.pop_back();
					base::decrease_indent();
				}
			}

		public:

			TOML_NODISCARD_CTOR
			default_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: base{
					source_,
					impl::formatter_options{
						indent_string,
						false //quote_dates_and_times
					}
				}
			{}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, default_formatter& rhs)
				TOML_MAY_THROW
			{
				rhs.attach(lhs);
				rhs.base::decrease_indent(); //starts at -1 so root kvps and first-level child tables have the same indent
				rhs.key_path.clear();
				rhs.print(rhs.source());
				rhs.detach();
				return lhs;
			}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, default_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	template <typename CHAR>
	inline void default_formatter<CHAR>::print_inline(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream("{ "sv, base::stream());

			bool first = false;
			for (auto [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;

				print_key_segment(k);
				impl::print_to_stream(" = "sv, base::stream());

				const auto type = v.type();
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print(v, type);
				}
			}

			impl::print_to_stream(" }"sv, base::stream());
		}
		base::clear_naked_newline();
	}

	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs) TOML_MAY_THROW
	{
		return lhs << default_formatter<CHAR>{ rhs };
	}
}
