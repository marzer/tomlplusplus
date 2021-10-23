//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}

#include "table.h"
#include "node_view.h"

/// \cond

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	table::table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const table& other) noexcept //
		: node(other),
		  inline_{ other.inline_ }
	{
		for (auto&& [k, v] : other)
			map.emplace_hint(map.end(), k, impl::make_node(v));

#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(table && other) noexcept //
		: node(std::move(other)),
		  map{ std::move(other.map) },
		  inline_{ other.inline_ }
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(const table& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(rhs);
			map.clear();
			for (auto&& [k, v] : rhs)
				map.emplace_hint(map.end(), k, impl::make_node(v));
			inline_ = rhs.inline_;
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(table&& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(std::move(rhs));
			map			  = std::move(rhs.map);
			inline_		  = rhs.inline_;
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	table::table(impl::table_init_pair * pairs, size_t count) noexcept
	{
		for (size_t i = 0; i < count; i++)
		{
			if (!pairs[i].value) // empty node_views
				continue;
			map.insert_or_assign(std::move(pairs[i].key), std::move(pairs[i].value));
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype) const noexcept
	{
		if (map.empty())
			return false;

		if (ntype == node_type::none)
			ntype = map.cbegin()->second->type();

		for (const auto& [k, v] : map)
		{
			(void)k;
			if (v->type() != ntype)
				return false;
		}

		return true;
	}

	namespace impl
	{
		template <typename T, typename U>
		TOML_INTERNAL_LINKAGE
		bool table_is_homogeneous(T& map, node_type ntype, U& first_nonmatch) noexcept
		{
			if (map.empty())
			{
				first_nonmatch = {};
				return false;
			}
			if (ntype == node_type::none)
				ntype = map.cbegin()->second->type();
			for (const auto& [k, v] : map)
			{
				(void)k;
				if (v->type() != ntype)
				{
					first_nonmatch = v.get();
					return false;
				}
			}
			return true;
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, toml::node * &first_nonmatch) noexcept
	{
		return impl::table_is_homogeneous(map, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, const toml::node*& first_nonmatch) const noexcept
	{
		return impl::table_is_homogeneous(map, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> table::operator[](std::string_view key) noexcept
	{
		return node_view<node>{ this->get(key) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> table::operator[](std::string_view key) const noexcept
	{
		return node_view<const node>{ this->get(key) };
	}

#if TOML_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> table::operator[](std::wstring_view key) noexcept
	{
		return node_view<node>{ this->get(key) };
	}
	TOML_EXTERNAL_LINKAGE
	node_view<const node> table::operator[](std::wstring_view key) const noexcept
	{
		return node_view<const node>{ this->get(key) };
	}

#endif // TOML_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	bool operator==(const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.map.size() != rhs.map.size())
			return false;

		for (auto l = lhs.map.begin(), r = rhs.map.begin(), e = lhs.map.end(); l != e; l++, r++)
		{
			if (l->first != r->first)
				return false;

			const auto lhs_type = l->second->type();
			const node& rhs_	= *r->second;
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = l->second->visit(
				[&](const auto& lhs_) noexcept
				{ return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_); });
			if (!equal)
				return false;
		}
		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool operator!=(const table& lhs, const table& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}
TOML_NAMESPACE_END;

/// \endcond
