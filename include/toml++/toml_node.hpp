//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_node.h"
//# {{
#if !TOML_DOXYGEN
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}

namespace toml
{
	TOML_EXTERNAL_LINKAGE
	node::node(node && other) noexcept
		: source_{ std::move(other.source_) }
	{
		other.source_.begin = {};
		other.source_.end = {};
	}

	TOML_EXTERNAL_LINKAGE
	node & node::operator= (node && rhs) noexcept
	{
		source_ = std::move(rhs.source_);
		rhs.source_.begin = {};
		rhs.source_.end = {};
		return *this;
	}

	TOML_EXTERNAL_LINKAGE bool node::is_string() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_integer() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_floating_point() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_number() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_boolean() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_date() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_time() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_date_time() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool node::is_array_of_tables() const noexcept { return false; }

	TOML_EXTERNAL_LINKAGE table* node::as_table() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE array* node::as_array() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<string>* node::as_string() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<int64_t>* node::as_integer() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<double>* node::as_floating_point() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<bool>* node::as_boolean() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<date>* node::as_date() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<time>* node::as_time() noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE value<date_time>* node::as_date_time() noexcept { return nullptr; }

	TOML_EXTERNAL_LINKAGE const table* node::as_table() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const array* node::as_array() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<string>* node::as_string() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<int64_t>* node::as_integer() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<double>* node::as_floating_point() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<bool>* node::as_boolean() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<date>* node::as_date() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<time>* node::as_time() const noexcept { return nullptr; }
	TOML_EXTERNAL_LINKAGE const value<date_time>* node::as_date_time() const noexcept { return nullptr; }

	TOML_EXTERNAL_LINKAGE const source_region& node::source() const noexcept { return source_; }
}

//# {{
#endif // !TOML_DOXYGEN
//# }} 
