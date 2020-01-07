#pragma once
#include "toml_formatter.h"

namespace toml
{
	template <typename CHAR = char>
	class json_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;

			inline void print(const toml::table& tbl) TOML_MAY_THROW;

			void print(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					impl::print_to_stream("[]"sv, base::stream());
				else
				{
					impl::print_to_stream('[', base::stream());
					base::increase_indent();
					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
							impl::print_to_stream(',', base::stream());
						base::print_newline(true);
						base::print_indent();

						auto& v = arr[i];
						const auto type = v.type();
						switch (type)
						{
							case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
							case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
							default:
								base::print(v, type);
						}

					}
					base::decrease_indent();
					base::print_newline(true);
					base::print_indent();
					impl::print_to_stream(']', base::stream());
				}
				base::clear_naked_newline();
			}

		public:

			TOML_NODISCARD_CTOR
				json_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: base{
					source_,
					impl::formatter_options{
						indent_string,
						true //quote_dates_and_times
					}
				}
			{}

			template <typename T>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter& rhs)
				TOML_MAY_THROW
			{
				rhs.attach(lhs);
				rhs.print(rhs.source());
				rhs.detach();
				return lhs;
			}

			template <typename T>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<T>& lhs, json_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	template <typename CHAR>
	inline void json_formatter<CHAR>::print(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream('{', base::stream());
			base::increase_indent();
			bool first = false;
			for (auto [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;
				base::print_newline(true);
				base::print_indent();

				base::print_quoted_string(k);
				impl::print_to_stream(" : "sv, base::stream());

				const auto type = v.type();
				switch (type)
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print(v, type);
				}

			}
			base::decrease_indent();
			base::print_newline(true);
			base::print_indent();
			impl::print_to_stream('}', base::stream());
		}
		base::clear_naked_newline();
	}
}

