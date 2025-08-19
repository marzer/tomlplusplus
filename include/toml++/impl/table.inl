//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

//# {{
#include "preprocessor.hpp"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "table.hpp"
#include "node_view.hpp"
#include "header_start.hpp"

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
	table::~table() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const impl::table_init_pair* b, const impl::table_init_pair* e)
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif

		TOML_ASSERT_ASSUME(b);
		TOML_ASSERT_ASSUME(e);
		TOML_ASSERT_ASSUME(b <= e);

		if TOML_UNLIKELY(b == e)
			return;

		for (; b != e; b++)
		{
			if (!b->value) // empty node_views
				continue;

#if TOML_ENABLE_ORDERED_TABLES
			entries_.push_back({ std::move(b->key), std::move(b->value) });
			map_.insert_or_assign(std::move(b->key), std::prev(entries_.end()));
#else
			map_.insert_or_assign(std::move(b->key), std::move(b->value));
#endif
		}
	}

	TOML_EXTERNAL_LINKAGE
	table::table(const table& other) //
		: node(other),
		  inline_{ other.inline_ }
	{
#if TOML_ENABLE_ORDERED_TABLES
		for (auto&& [k, v] : other.entries_)
		{
			entries_.push_back({ k, impl::make_node(*v) });
			map_.emplace(k, std::prev(entries_.end()));
		}
#else
		for (auto&& [k, v] : other.map_)
		{
			map_.emplace_hint(map_.end(), k, impl::make_node(*v));
		}
#endif

#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::table(table && other) noexcept //
		: node(std::move(other)),
		  map_{ std::move(other.map_) },
#if TOML_ENABLE_ORDERED_TABLES
		  entries_{ std::move(other.entries_) },
#endif
		  inline_{ other.inline_ }
	{
#if TOML_LIFETIME_HOOKS
		TOML_TABLE_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table& table::operator=(const table& rhs)
	{
		if (&rhs != this)
		{
			node::operator=(rhs);
			map_.clear();
#if TOML_ENABLE_ORDERED_TABLES
			entries_.clear();
			for (auto&& [k, v] : rhs.entries_)
			{
				entries_.push_back({ k, impl::make_node(*v) });
				map_.emplace(k, std::prev(entries_.end()));
			}
#else
			for (auto&& [k, v] : rhs.map_)
			{
				map_.emplace_hint(map_.end(), k, impl::make_node(*v));
			}
#endif
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
			map_	= std::move(rhs.map_);
			inline_ = rhs.inline_;
		}
		return *this;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype) const noexcept
	{
		if (map_.empty())
			return false;

#if TOML_ENABLE_ORDERED_TABLES
		if (ntype == node_type::none)
			ntype = entries_.cbegin()->second->type();

		for (auto&& [k, v] : entries_)
#else
		if (ntype == node_type::none)
			ntype = map_.cbegin()->second->type();

		for (auto&& [k, v] : map_)
#endif
		{
			TOML_UNUSED(k);
			if (v->type() != ntype)
				return false;
		}

		return true;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (map_.empty())
		{
			first_nonmatch = {};
			return false;
		}

		if (ntype == node_type::none)
#if TOML_ENABLE_ORDERED_TABLES
			ntype = entries_.cbegin()->second->type();
#else
			ntype = map_.cbegin()->second->type();
#endif

#if TOML_ENABLE_ORDERED_TABLES
		for (const auto& [k, v] : entries_)
#else
		for (const auto& [k, v] : map_)
#endif
		{
			TOML_UNUSED(k);
			if (v->type() != ntype)
			{
				first_nonmatch = v.get();
				return false;
			}
		}
		return true;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool table::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* fnm		  = nullptr;
		const auto result = const_cast<table&>(*this).is_homogeneous(ntype, fnm);
		first_nonmatch	  = fnm;
		return result;
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	node* table::get(std::string_view key) noexcept
	{
		if (auto it = map_.find(toml::key{ key }); it != map_.end())
#if TOML_ENABLE_ORDERED_TABLES
			return it->second->second.get();
#else
			return it->second.get();
#endif
		return nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	node& table::at(std::string_view key)
	{
		auto n = get(key);

#if TOML_COMPILER_HAS_EXCEPTIONS

		if (!n)
		{
			auto err = "key '"s;
			err.append(key);
			err.append("' not found in table"sv);
			throw std::out_of_range{ err };
		}

#else

		TOML_ASSERT_ASSUME(n && "key not found in table!");

#endif

		return *n;
	}

#if !TOML_ENABLE_ORDERED_TABLES
	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::get_lower_bound(std::string_view key) noexcept
	{
		return map_.lower_bound(key);
	}
#endif // !TOML_ENABLE_ORDERED_TABLES

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::iterator table::find(std::string_view key) noexcept
	{
#if TOML_ENABLE_ORDERED_TABLES
		auto ipos = map_.find(toml::key{ key });
		if (ipos == map_.end())
		{
			return iterator{ entries_.end() };
		}
		return iterator{ ipos->second };
#else
		return iterator{ map_.find(key) };
#endif
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	table::const_iterator table::find(std::string_view key) const noexcept
	{
#if TOML_ENABLE_ORDERED_TABLES
		return const_iterator{ map_.find(toml::key{ key })->second };
#else
		return const_iterator{ map_.find(key) };
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator pos) noexcept
	{
#if TOML_ENABLE_ORDERED_TABLES
		map_.erase(pos->first);
		return entries_.erase(pos);
#else
		return map_.erase(pos);
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::erase(const_map_iterator begin, const_map_iterator end) noexcept
	{
#if TOML_ENABLE_ORDERED_TABLES
		for (auto ipos = begin; ipos != end; ipos++) {
			map_.erase(begin->first);
		}
		return entries_.erase(begin, end);
#else
		return map_.erase(begin, end);
#endif
	}

	TOML_EXTERNAL_LINKAGE
	size_t table::erase(std::string_view key) noexcept
	{
#if TOML_ENABLE_ORDERED_TABLES
		size_t result = map_.erase(toml::key{ key });
		auto ipos = map_.find(toml::key{ key });
		if (ipos != map_.end())
		{
			map_.erase(ipos);
			entries_.erase(ipos->second);
		}
		return result;
#else
		if (auto it = map_.find(key); it != map_.end())
		{
			map_.erase(it);
			return size_t{ 1 };
		}
		return size_t{};
#endif
	}

	TOML_EXTERNAL_LINKAGE
	table& table::prune(bool recursive)& noexcept
	{
		if (map_.empty())
			return *this;

		for (auto it = map_.begin(); it != map_.end();)
		{
#if TOML_ENABLE_ORDERED_TABLES
			if (auto arr = it->second->second->as_array())
#else
			if (auto arr = it->second->as_array())
#endif
			{
				if (recursive)
					arr->prune(true);

				if (arr->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
#if TOML_ENABLE_ORDERED_TABLES
			else if (auto tbl = it->second->second->as_table())
#else
			else if (auto tbl = it->second->as_table())
#endif
			{
				if (recursive)
					tbl->prune(true);

				if (tbl->empty())
				{
					it = map_.erase(it);
					continue;
				}
			}
			it++;
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void table::clear() noexcept
	{
		map_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	table::map_iterator table::insert_with_hint(const_iterator hint, key && k, impl::node_ptr && v)
	{
#if TOML_ENABLE_ORDERED_TABLES
		auto ipos = map_.find(k);
		if (ipos == map_.end())
		{
			entries_.emplace_back(std::pair{ k, std::move(v) });
			auto entry_ipos = std::prev(entries_.end());
			map_.emplace(std::move(k), entry_ipos);
			return entry_ipos;
		}
		else
		{
			return ipos->second;
		}
#else
		auto prev_size = map_.size();
		auto ipos = map_.emplace_hint(const_map_iterator{ hint }, std::move(k), std::move(v));
		if (map_.size() > prev_size)
			last_inserted_ = iterator{ ipos };
		return ipos;
#endif
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV table::equal(const table& lhs, const table& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.map_.size() != rhs.map_.size())
			return false;

#if TOML_ENABLE_ORDERED_TABLES
		for (auto l = lhs.entries_.begin(), r = rhs.entries_.begin(), e = lhs.entries_.end(); l != e; l++, r++)
#else
		for (auto l = lhs.map_.begin(), r = rhs.map_.begin(), e = lhs.map_.end(); l != e; l++, r++)
#endif
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

#include "header_end.hpp"
