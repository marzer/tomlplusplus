//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_formatter.h"

namespace toml
{
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&);
	template <typename T, typename U>
	std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&&);

	/// \brief	A wrapper for printing TOML objects out to a stream as formatted JSON.
	///
	/// \detail \cpp
	/// auto some_toml = toml::parse(R"(
	///		[fruit]
	///		apple.color = "red"
	///		apple.taste.sweet = true
	///
	///		[fruit.apple.texture]
	///		smooth = true
	/// )"sv);
	///	std::cout << toml::json_formatter{ some_toml } << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// {
	///     "fruit" : {
	///         "apple" : {
	///             "color" : "red",
	///             "taste" : {
	///                 "sweet" : true
	///             },
	///             "texture" : {
	///                 "smooth" : true
	///             }
	///         }
	///     }
	/// }
	/// \eout
	/// 
	/// \tparam	CHAR	The underlying character type of the output stream. Must be 1 byte in size.
	template <typename CHAR = char>
	class json_formatter final : impl::formatter<CHAR>
	{
		private:
			using base = impl::formatter<CHAR>;

			inline void print(const toml::table& tbl);

			void print(const array& arr)
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

			void print()
			{
				switch (auto source_type = base::source().type())
				{
					case node_type::table: print(*reinterpret_cast<const table*>(&base::source())); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&base::source())); break;
					default: base::print(base::source(), source_type);
				}
			}

		public:

			/// \brief	Constructs a JSON formatter and binds it to a TOML object.
			///
			/// \param 	source	The source TOML object.
			/// \param 	flags 	Format option flags.
			TOML_NODISCARD_CTOR
				explicit json_formatter(const toml::node& source, format_flags flags = {}) noexcept
				: base{ source, flags | format_flags::quote_dates_and_times }
			{}

			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&);
			template <typename T, typename U>
			friend std::basic_ostream<T>& operator << (std::basic_ostream<T>&, json_formatter<U>&&);
	};

	json_formatter(const table&) -> json_formatter<char>;
	json_formatter(const array&) -> json_formatter<char>;
	template <typename T> json_formatter(const value<T>&) -> json_formatter<char>;

	template <typename CHAR>
	inline void json_formatter<CHAR>::print(const toml::table& tbl)
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

	/// \brief	Prints the bound TOML object out to the stream as JSON.
	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>& rhs)
	{
		rhs.attach(lhs);
		rhs.print();
		rhs.detach();
		return lhs;
	}

	/// \brief	Prints the bound TOML object out to the stream as JSON (rvalue overload).
	template <typename T, typename U>
	inline std::basic_ostream<T>& operator << (std::basic_ostream<T>& lhs, json_formatter<U>&& rhs)
	{
		return lhs << rhs; //as lvalue
	}
}

