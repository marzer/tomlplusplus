//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_common.h"
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif

#if !TOML_ALL_INLINE

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <ostream>
TOML_POP_WARNINGS
#include "toml_node_view.h"
#include "toml_default_formatter.h"
#include "toml_json_formatter.h"

namespace toml
{
	// value<>
	template class TOML_API value<string>;
	template class TOML_API value<int64_t>;
	template class TOML_API value<double>;
	template class TOML_API value<bool>;
	template class TOML_API value<date>;
	template class TOML_API value<time>;
	template class TOML_API value<date_time>;

	// node_view
	template class TOML_API node_view<node>;
	template class TOML_API node_view<const node>;

	// formatters
	namespace impl
	{
		template class TOML_API formatter<char>;
	}
	template class TOML_API default_formatter<char>;
	template class TOML_API json_formatter<char>;

	// various ostream operators
	template TOML_API std::ostream& operator << (std::ostream&, const source_position&);
	template TOML_API std::ostream& operator << (std::ostream&, const source_region&);
	template TOML_API std::ostream& operator << (std::ostream&, const parse_error&);
	template TOML_API std::ostream& operator << (std::ostream&, const date&);
	template TOML_API std::ostream& operator << (std::ostream&, const time&);
	template TOML_API std::ostream& operator << (std::ostream&, const time_offset&);
	template TOML_API std::ostream& operator << (std::ostream&, const date_time&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::string>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<int64_t>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<double>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<bool>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::time>&);
	template TOML_API std::ostream& operator << (std::ostream&, const value<toml::date_time>&);
	template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&);
	template TOML_API std::ostream& operator << (std::ostream&, default_formatter<char>&&);
	template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&);
	template TOML_API std::ostream& operator << (std::ostream&, json_formatter<char>&&);
	template TOML_API std::ostream& operator << (std::ostream&, const table&);
	template TOML_API std::ostream& operator << (std::ostream&, const array&);
	template TOML_API std::ostream& operator << (std::ostream&, const node_view<node>&);
	template TOML_API std::ostream& operator << (std::ostream&, const node_view<const node>&);
	template TOML_API std::ostream& operator << (std::ostream&, node_type);

	namespace impl
	{
		template TOML_API void print_floating_point_to_stream(float, std::ostream&, bool);
		template TOML_API void print_floating_point_to_stream(double, std::ostream&, bool);
	}
}

#endif // !TOML_ALL_INLINE
