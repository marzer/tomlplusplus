#pragma once
#include "toml_formatter.h"

namespace toml
{
	class json_formatter final : impl::formatter
	{
		private:
			using base = impl::formatter;

			inline void write(const toml::table& tbl) TOML_MAY_THROW;

			void write(const array& arr) TOML_MAY_THROW
			{
				if (arr.empty())
					base::write("[]"sv);
				else
				{
					base::write('[');
					indent_level++;
					for (size_t i = 0; i < arr.size(); i++)
					{
						if (i > 0_sz)
							base::write(',');
						write_newline(true);
						write_indent();

						auto v = arr.get(i);
						const auto type = v->type();
						switch (type)
						{
							case node_type::table: write(*reinterpret_cast<const table*>(v)); break;
							case node_type::array: write(*reinterpret_cast<const array*>(v)); break;
							default:
								write_value(v, type);
						}

					}
					indent_level--;
					write_newline(true);
					write_indent();
					base::write(']');
				}
				naked_newline = false;
			}

			void write() TOML_MAY_THROW
			{
				write(source);
			}

		public:

			TOML_NODISCARD_CTOR
				json_formatter(const toml::table& source_, toml::string_view indent_string = {}) noexcept
				: formatter{
					source_,
					impl::formatter_options{
						indent_string,
						true //quote_dates_and_times
					}
				}
			{}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, json_formatter& rhs)
				TOML_MAY_THROW
			{
				auto fw = impl::formatter_writer{ lhs };
				rhs.attach(fw);
				rhs.write();
				rhs.detach();
				return lhs;
			}

			template <typename CHAR>
			friend std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, json_formatter&& rhs)
				TOML_MAY_THROW
			{
				return lhs << rhs; //as lvalue
			}
	};

	inline void json_formatter::write(const toml::table& tbl) TOML_MAY_THROW
	{
		if (tbl.empty())
			base::write("{}"sv);
		else
		{
			base::write('{');
			indent_level++;
			bool first = false;
			for (auto& [k, v] : tbl.values)
			{
				if (first)
					base::write(", "sv);
				first = true;
				write_newline(true);
				write_indent();

				base::write_quoted_string(k);
				base::write(" : "sv);

				const auto type = v->type();
				switch (type)
				{
					case node_type::table: write(*reinterpret_cast<const table*>(v.get())); break;
					case node_type::array: write(*reinterpret_cast<const array*>(v.get())); break;
					default:
						write_value(v.get(), type);
				}

			}
			indent_level--;
			write_newline(true);
			write_indent();
			base::write('}');
		}
		naked_newline = false;
	}
}

