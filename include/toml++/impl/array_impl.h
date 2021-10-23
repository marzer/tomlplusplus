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

#include "array.h"

/// \cond

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
	array::array(const array& other) noexcept //
		: node(other)
	{
		elements.reserve(other.elements.size());
		for (const auto& elem : other)
			elements.emplace_back(impl::make_node(elem));

#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array::array(array && other) noexcept //
		: node(std::move(other)),
		  elements{ std::move(other.elements) }
	{
#if TOML_LIFETIME_HOOKS
		TOML_ARRAY_CREATED;
#endif
	}

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(const array& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(rhs);
			elements.clear();
			elements.reserve(rhs.elements.size());
			for (const auto& elem : rhs)
				elements.emplace_back(impl::make_node(elem));
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	array& array::operator=(array&& rhs) noexcept
	{
		if (&rhs != this)
		{
			node::operator=(std::move(rhs));
			elements	  = std::move(rhs.elements);
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	void array::preinsertion_resize(size_t idx, size_t count) noexcept
	{
		TOML_ASSERT(idx <= elements.size());
		TOML_ASSERT(count >= 1_sz);
		const auto old_size			= elements.size();
		const auto new_size			= old_size + count;
		const auto inserting_at_end = idx == old_size;
		elements.resize(new_size);
		if (!inserting_at_end)
		{
			for (size_t left = old_size, right = new_size - 1_sz; left-- > idx; right--)
				elements[right] = std::move(elements[left]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(pure)
	const node& array::operator[](size_t index) const noexcept
	{
		return *elements[index];
	}

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(pure)
	node& array::operator[](size_t index) noexcept
	{
		return *elements[index];
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype) const noexcept
	{
		if (elements.empty())
			return false;

		if (ntype == node_type::none)
			ntype = elements[0]->type();

		for (const auto& val : elements)
			if (val->type() != ntype)
				return false;

		return true;
	}

	namespace impl
	{
		template <typename T, typename U>
		TOML_INTERNAL_LINKAGE
		bool array_is_homogeneous(T& elements, node_type ntype, U& first_nonmatch) noexcept
		{
			if (elements.empty())
			{
				first_nonmatch = {};
				return false;
			}
			if (ntype == node_type::none)
				ntype = elements[0]->type();
			for (const auto& val : elements)
			{
				if (val->type() != ntype)
				{
					first_nonmatch = val.get();
					return false;
				}
			}
			return true;
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, toml::node * &first_nonmatch) noexcept
	{
		return impl::array_is_homogeneous(elements, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	bool array::is_homogeneous(node_type ntype, const toml::node*& first_nonmatch) const noexcept
	{
		return impl::array_is_homogeneous(elements, ntype, first_nonmatch);
	}

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(pure)
	node* array::get(size_t index) noexcept
	{
		return index < elements.size() ? elements[index].get() : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(pure)
	const node* array::get(size_t index) const noexcept
	{
		return index < elements.size() ? elements[index].get() : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	bool operator==(const array& lhs, const array& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.elements.size() != rhs.elements.size())
			return false;
		for (size_t i = 0, e = lhs.elements.size(); i < e; i++)
		{
			const auto lhs_type = lhs.elements[i]->type();
			const node& rhs_	= *rhs.elements[i];
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = lhs.elements[i]->visit(
				[&](const auto& lhs_) noexcept
				{ return lhs_ == *reinterpret_cast<std::remove_reference_t<decltype(lhs_)>*>(&rhs_); });
			if (!equal)
				return false;
		}
		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool operator!=(const array& lhs, const array& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	TOML_EXTERNAL_LINKAGE
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (size_t i = 0, e = elements.size(); i < e; i++)
		{
			auto arr = elements[i]->as_array();
			leaves += arr ? arr->total_leaf_count() : 1_sz;
		}
		return leaves;
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten_child(array && child, size_t & dest_index) noexcept
	{
		for (size_t i = 0, e = child.size(); i < e; i++)
		{
			auto type = child.elements[i]->type();
			if (type == node_type::array)
			{
				array& arr = *reinterpret_cast<array*>(child.elements[i].get());
				if (!arr.empty())
					flatten_child(std::move(arr), dest_index);
			}
			else
				elements[dest_index++] = std::move(child.elements[i]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	array& array::flatten()&
	{
		if (elements.empty())
			return *this;

		bool requires_flattening	 = false;
		size_t size_after_flattening = elements.size();
		for (size_t i = elements.size(); i-- > 0_sz;)
		{
			auto arr = elements[i]->as_array();
			if (!arr)
				continue;
			size_after_flattening--; // discount the array itself
			const auto leaf_count = arr->total_leaf_count();
			if (leaf_count > 0_sz)
			{
				requires_flattening = true;
				size_after_flattening += leaf_count;
			}
			else
				elements.erase(elements.cbegin() + static_cast<ptrdiff_t>(i));
		}

		if (!requires_flattening)
			return *this;

		elements.reserve(size_after_flattening);

		size_t i = 0;
		while (i < elements.size())
		{
			auto arr = elements[i]->as_array();
			if (!arr)
			{
				i++;
				continue;
			}

			std::unique_ptr<node> arr_storage = std::move(elements[i]);
			const auto leaf_count			  = arr->total_leaf_count();
			if (leaf_count > 1_sz)
				preinsertion_resize(i + 1_sz, leaf_count - 1_sz);
			flatten_child(std::move(*arr), i); // increments i
		}

		return *this;
	}
}
TOML_NAMESPACE_END;

/// \endcond
