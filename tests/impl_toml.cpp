// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "settings.h"
#if !TOML_ALL_INLINE
	#define TOML_IMPLEMENTATION
#endif

#if USE_TARTANLLAMA_OPTIONAL
	#if __has_include(<tloptional/include/tl/optional.hpp>)
		#include <tloptional/include/tl/optional.hpp>
	#else
		#error TartanLlama/optional is missing! You probably need to fetch submodules ("git submodule update --init extern/tloptional")
	#endif
#endif

#if USE_SINGLE_HEADER
	#include "../toml.hpp"
#else
	#include "../include/toml++/toml.h"
#endif

#if TOML_COMPILER_EXCEPTIONS != TOML_EXCEPTIONS
	#error TOML_EXCEPTIONS does not match TOML_COMPILER_EXCEPTIONS (default behaviour should be to match)
#endif

#if (defined(_WIN32) && !TOML_WINDOWS_COMPAT) || (!defined(_WIN32) && TOML_WINDOWS_COMPAT)
	#error TOML_WINDOWS_COMPAT does not match _WIN32 (default behaviour should be to match)
#endif

namespace toml
{
	using std::declval;
	using std::is_same_v;

	static_assert(is_same_v<decltype(declval<node&>().ref<double>()), double&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<double>()), double&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<double>()), const double&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<value<double>>()), double&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<value<double>>()), double&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<value<double>>()), const double&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<table>()), table&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<table>()), table&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<table>()), const table&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<array>()), array&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<array>()), array&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<array>()), const array&>);

	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<double>()), double&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<double>()), const double&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<value<double>>()), double&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<value<double>>()), const double&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<table>()), table&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<table>()), const table&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<array>()), array&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<array>()), const array&>);
}
