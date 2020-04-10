//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_table.h"
#include "toml_node_view.h"
//# {{
#if !TOML_DOXYGEN
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}

namespace toml
{
	TOML_EXTERNAL_LINKAGE
	table::table(impl::table_init_pair* pairs, size_t count) noexcept
	{
		for (size_t i = 0; i < count; i++)
		{
			values.insert_or_assign(
				std::move(pairs[i].key),
				std::move(pairs[i].value)
			);
		}
	}

	TOML_EXTERNAL_LINKAGE
	table::table() noexcept {}

	TOML_EXTERNAL_LINKAGE
	table::table(table&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) },
		inline_{ other.inline_ }
	{}

	TOML_EXTERNAL_LINKAGE
	table& table::operator = (table&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		inline_ = rhs.inline_;
		return *this;
	}

	TOML_EXTERNAL_LINKAGE node_type table::type() const noexcept { return node_type::table; }
	TOML_EXTERNAL_LINKAGE bool table::is_table() const noexcept { return true; }
	TOML_EXTERNAL_LINKAGE bool table::is_array() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool table::is_value() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE table* table::as_table() noexcept { return this; }
	TOML_EXTERNAL_LINKAGE const table* table::as_table() const noexcept { return this; }
	TOML_EXTERNAL_LINKAGE bool table::is_inline() const noexcept { return inline_; }
	TOML_EXTERNAL_LINKAGE void table::is_inline(bool val) noexcept { inline_ = val; }

	TOML_EXTERNAL_LINKAGE
	node_view<node> table::operator[] (string_view key) noexcept
	{
		return { this->get(key) };
	}
	TOML_EXTERNAL_LINKAGE
	node_view<const node> table::operator[] (string_view key) const noexcept
	{
		return { this->get(key) };
	}

	TOML_EXTERNAL_LINKAGE table::iterator table::begin() noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::begin() const noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_EXTERNAL_LINKAGE table::iterator table::end() noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::end() const noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE table::const_iterator table::cend() const noexcept { return { values.cend() }; }

	TOML_EXTERNAL_LINKAGE bool table::empty() const noexcept { return values.empty(); }
	TOML_EXTERNAL_LINKAGE size_t table::size() const noexcept { return values.size(); }
	TOML_EXTERNAL_LINKAGE void table::clear() noexcept { values.clear(); }

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	bool table::erase(string_view key) noexcept
	{
		if (auto it = values.find(key); it != values.end())
		{
			values.erase(it);
			return true;
		}
		return false;
	}

	TOML_EXTERNAL_LINKAGE
	node* table::get(string_view key) noexcept
	{
		return do_get(values, key);
	}

	TOML_EXTERNAL_LINKAGE
	const node* table::get(string_view key) const noexcept
	{
		return do_get(values, key);
	}

	TOML_EXTERNAL_LINKAGE
	table::iterator table::find(string_view key) noexcept
	{
		return { values.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	table::const_iterator table::find(string_view key) const noexcept
	{
		return { values.find(key) };
	}

	TOML_EXTERNAL_LINKAGE
	bool table::contains(string_view key) const noexcept
	{
		return do_contains(values, key);
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	bool operator == (const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.values.size() != rhs.values.size())
			return false;

		for (auto l = lhs.values.begin(), r = rhs.values.begin(), e = lhs.values.end(); l != e; l++, r++)
		{
			if (l->first != r->first)
				return false;

			const auto lhs_type = l->second->type();
			const node& rhs_ = *r->second;
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = l->second->visit([&](const auto& lhs_) noexcept
			{
				return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_);
			});
			if (!equal)
				return false;
		}
		return true;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	bool operator != (const table& lhs, const table& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}

//# {{
#endif // !TOML_DOXYGEN
//# }} 
