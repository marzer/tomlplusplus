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

#include "array.hpp"
#include "header_start.hpp"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	array::array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::~array() noexcept
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_DESTROYED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const impl::array_init_elem* first, const impl::array_init_elem* last)
	{
		TOML_ASSERT_ASSUME(first);
		TOML_ASSERT_ASSUME(last);
		TOML_ASSERT_ASSUME(first <= last);

		size_t cap{};
		for (auto it = first; it != last; it++)
		{
			if (it->value)
				cap++;
		}

		elems_.reserve(cap);
		for (; first != last; first++)
		{
			if (first->value)
				elems_.push_back(std::move(first->value));
		}

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(const array& other) //
		: node(other)
	{
		elems_.reserve(other.elems_.size());
		for (const auto& elem : other)
			elems_.emplace_back(impl::make_node(elem));

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	array::array(array && other) noexcept //
		: node(std::move(other)),
		  elems_(std::move(other.elems_))
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(const array& rhs)
	{
		if (&rhs == this)
			return *this;

		vector_type copy;
		copy.reserve(rhs.elems_.size());
		for (const auto& elem : rhs)
			copy.emplace_back(impl::make_node(elem));
		node::operator=(rhs);
		elems_.swap(copy);
		return *this;
	}

	// NOLINTBEGIN(bugprone-use-after-move)
	TOML_EXTERNAL_LINKAGE
	array& array::operator=(array&& rhs) noexcept
	{
		if (&rhs == this)
			return *this;

		node::operator=(std::move(rhs));
		vector_type stolen = std::move(rhs.elems_);
		elems_.swap(stolen);
		return *this;
	}
	// NOLINTEND(bugprone-use-after-move)

	TOML_EXTERNAL_LINKAGE
	void array::preinsertion_resize(size_t idx, size_t count)
	{
		TOML_ASSERT(idx <= elems_.size());
		TOML_ASSERT_ASSUME(count >= 1u);
		const auto old_size = elems_.size();
		const auto new_size = old_size + count;
		elems_.resize(new_size);
		for (size_t left = old_size, right = new_size - 1u; left-- > idx; right--)
			elems_[right] = std::move(elems_[left]);
	}

	TOML_EXTERNAL_LINKAGE
	void array::insert_at_back(impl::node_ptr && elem)
	{
		TOML_ASSERT(elem);
		elems_.push_back(std::move(elem));
	}

	TOML_EXTERNAL_LINKAGE
	array::vector_iterator array::insert_at(const_vector_iterator pos, impl::node_ptr && elem)
	{
		return elems_.insert(pos, std::move(elem));
	}

	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype) const noexcept
	{
		const node* first_nonmatch = {};
		return is_homogeneous(ntype, first_nonmatch);
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, node * &first_nonmatch) noexcept
	{
		if (elems_.empty())
		{
			first_nonmatch = {};
			return false;
		}
		if (ntype == node_type::none)
			ntype = elems_[0]->type();
		for (const auto& val : elems_)
		{
			if (val->type() != ntype)
			{
				first_nonmatch = val.get();
				return false;
			}
		}
		return true;
	}

	TOML_NODISCARD
	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept
	{
		node* nonmatch	  = {};
		const auto result = const_cast<array&>(*this).is_homogeneous(ntype, nonmatch);
		first_nonmatch	  = nonmatch;
		return result;
	}

	TOML_EXTERNAL_LINKAGE
	node& array::at(size_t index)
	{
#if TOML_COMPILER_HAS_EXCEPTIONS

		return *elems_.at(index);

#else

		const auto n = get(index);
		TOML_ASSERT_ASSUME(n && "element index not found in array");
		return *n;

#endif
	}

	TOML_EXTERNAL_LINKAGE
	void array::reserve(size_t new_capacity)
	{
		elems_.reserve(new_capacity);
	}

	TOML_EXTERNAL_LINKAGE
	void array::shrink_to_fit()
	{
		elems_.shrink_to_fit();
	}

	TOML_EXTERNAL_LINKAGE
	void array::truncate(size_t new_size)
	{
		if (new_size < elems_.size())
			elems_.resize(new_size);
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ pos }) };
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return iterator{ elems_.erase(const_vector_iterator{ first }, const_vector_iterator{ last }) };
	}

	TOML_EXTERNAL_LINKAGE
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (const auto& elem : elems_)
		{
			const auto arr = elem->as_array();
			leaves += arr ? arr->total_leaf_count() : size_t{ 1 };
		}
		return leaves;
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten_child(array && child, size_t& dest_index) noexcept
	{
		for (auto& elem : child.elems_)
		{
			if (const auto arr = elem->as_array())
			{
				if (!arr->empty())
					flatten_child(std::move(*arr), dest_index);
			}
			else
				elems_[dest_index++] = std::move(elem);
		}
	}

	TOML_EXTERNAL_LINKAGE
	array& array::flatten() &
	{
		bool has_child_arrays = {};
		for (const auto& elem : elems_)
		{
			if (elem->is_array())
			{
				has_child_arrays = true;
				break;
			}
		}
		if (!has_child_arrays)
			return *this;

		array flattened;
		flattened.elems_.resize(total_leaf_count());

		size_t dest_index = {};
		flattened.flatten_child(std::move(*this), dest_index);
		elems_ = std::move(flattened.elems_);
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	array& array::prune(bool recursive) & noexcept
	{
		size_t kept = {};
		for (size_t i = 0, e = elems_.size(); i < e; i++)
		{
			bool is_empty = {};
			if (const auto arr = elems_[i]->as_array())
			{
				if (recursive)
					arr->prune(true);
				is_empty = arr->empty();
			}
			else if (const auto tbl = elems_[i]->as_table())
			{
				if (recursive)
					tbl->prune(true);
				is_empty = tbl->empty();
			}
			if (is_empty)
				continue;

			if (kept != i)
				elems_[kept] = std::move(elems_[i]);
			kept++;
		}
		elems_.erase(elems_.cbegin() + static_cast<ptrdiff_t>(kept), elems_.cend());

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void array::pop_back() noexcept
	{
		elems_.pop_back();
	}

	TOML_EXTERNAL_LINKAGE
	void array::clear() noexcept
	{
		elems_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV array::equal(const array& lhs, const array& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.elems_.size() != rhs.elems_.size())
			return false;
		for (size_t i = 0, e = lhs.elems_.size(); i < e; i++)
		{
			const node& lhs_elem = *lhs.elems_[i];
			const node& rhs_elem = *rhs.elems_[i];
			if (lhs_elem.type() != rhs_elem.type() || !lhs_elem.visit(impl::equal_to_same_type{ &rhs_elem }))
				return false;
		}
		return true;
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
