//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once
/// \cond

//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "table.h"
#include "node_view.h"
#include "header_start.h"

TOML_ANON_NAMESPACE_START
{
#if !TOML_HEADER_ONLY
	using namespace toml;
#endif

	template <typename T, typename U>
	TOML_INTERNAL_LINKAGE
	bool table_is_homogeneous(T & map, node_type ntype, U & first_nonmatch) noexcept
	{
		using namespace toml;

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
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	table::table(const table& other) noexcept //
		: node(other),
		  inline_{ other.inline_ }
	{
		for (auto&& [k, v] : other)
			map_.emplace_hint(map_.end(), k, impl::make_node(v));

#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(table && other) noexcept //
		: node(std::move(other)),
		  map_{ std::move(other.map_) },
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
			map_.clear();
			for (auto&& [k, v] : rhs)
				map_.emplace_hint(map_.end(), k, impl::make_node(v));
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
			map_		  = std::move(rhs.map_);
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
			map_.insert_or_assign(std::move(pairs[i].key), std::move(pairs[i].value));
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype) const noexcept
	{
		if (map_.empty())
			return false;

		if (ntype == node_type::none)
			ntype = map_.cbegin()->second->type();

		for (const auto& [k, v] : map_)
		{
			(void)k;
			if (v->type() != ntype)
				return false;
		}

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		return TOML_ANON_NAMESPACE::table_is_homogeneous(map_, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		return TOML_ANON_NAMESPACE::table_is_homogeneous(map_, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	bool table::equal(const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.map_.size() != rhs.map_.size())
			return false;

		for (auto l = lhs.map_.begin(), r = rhs.map_.begin(), e = lhs.map_.end(); l != e; l++, r++)
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
}
TOML_NAMESPACE_END;

#include "header_end.h"
/// \endcond
