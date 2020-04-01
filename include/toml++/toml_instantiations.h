//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_value.h"
#include "toml_node_view.h"
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif

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

	// table and array iterators
	#if !TOML_ALL_INLINE && !TOML_HAS_API_ANNOTATION
	namespace impl
	{
		template struct table_proxy_pair<true>;
		template struct table_proxy_pair<false>;
		template class table_iterator<true>;
		template class table_iterator<false>;
		template class array_iterator<true>;
		template class array_iterator<false>;
	}
	#endif
}

// unique_ptrs to various things
#if !TOML_ALL_INLINE && !TOML_HAS_API_ANNOTATION
namespace std
{
	template class unique_ptr<toml::node>;
	template class unique_ptr<toml::table>;
	template class unique_ptr<toml::array>;
	template class unique_ptr<toml::value<toml::string>>;
	template class unique_ptr<toml::value<int64_t>>;
	template class unique_ptr<toml::value<double>>;
	template class unique_ptr<toml::value<bool>>;
	template class unique_ptr<toml::value<toml::date>>;
	template class unique_ptr<toml::value<toml::time>>;
	template class unique_ptr<toml::value<toml::date_time>>;
}
#endif

