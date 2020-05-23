//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_array.h"
//# {{
#if !TOML_DOXYGEN
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}
 
namespace toml
{
	TOML_EXTERNAL_LINKAGE
	void array::preinsertion_resize(size_t idx, size_t count) noexcept
	{
		const auto new_size = values.size() + count;
		const auto inserting_at_end = idx == values.size();
		values.resize(new_size);
		if (!inserting_at_end)
		{
			for (size_t r = new_size, e = idx + count, l = e; r-- > e; l--)
				values[r] = std::move(values[l]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	array::array() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	array::array(array&& other) noexcept
		: node{ std::move(other) },
		values{ std::move(other.values) }
	{}

	TOML_EXTERNAL_LINKAGE
	array& array::operator= (array&& rhs) noexcept
	{
		node::operator=(std::move(rhs));
		values = std::move(rhs.values);
		return *this;
	}

	TOML_EXTERNAL_LINKAGE node_type array::type() const noexcept { return node_type::array; }
	TOML_EXTERNAL_LINKAGE bool array::is_table() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE bool array::is_array() const noexcept { return true; }
	TOML_EXTERNAL_LINKAGE bool array::is_value() const noexcept { return false; }
	TOML_EXTERNAL_LINKAGE array* array::as_array() noexcept { return this; }
	TOML_EXTERNAL_LINKAGE const array* array::as_array() const noexcept { return this; }

	TOML_EXTERNAL_LINKAGE node& array::operator[] (size_t index) noexcept { return *values[index]; }
	TOML_EXTERNAL_LINKAGE const node& array::operator[] (size_t index) const noexcept { return *values[index]; }

	TOML_EXTERNAL_LINKAGE node& array::front() noexcept { return *values.front(); }
	TOML_EXTERNAL_LINKAGE const node& array::front() const noexcept { return *values.front(); }
	TOML_EXTERNAL_LINKAGE node& array::back() noexcept { return *values.back(); }
	TOML_EXTERNAL_LINKAGE const node& array::back() const noexcept { return *values.back(); }

	TOML_EXTERNAL_LINKAGE array::iterator array::begin() noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::begin() const noexcept { return { values.begin() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::cbegin() const noexcept { return { values.cbegin() }; }

	TOML_EXTERNAL_LINKAGE array::iterator array::end() noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::end() const noexcept { return { values.end() }; }
	TOML_EXTERNAL_LINKAGE array::const_iterator array::cend() const noexcept { return { values.cend() }; }

	TOML_EXTERNAL_LINKAGE bool array::empty() const noexcept { return values.empty(); }
	TOML_EXTERNAL_LINKAGE size_t array::size() const noexcept { return values.size(); }
	TOML_EXTERNAL_LINKAGE void array::reserve(size_t new_capacity) { values.reserve(new_capacity); }
	TOML_EXTERNAL_LINKAGE void array::clear() noexcept { values.clear(); }

	TOML_EXTERNAL_LINKAGE size_t array::max_size() const noexcept { return values.max_size(); }
	TOML_EXTERNAL_LINKAGE size_t array::capacity() const noexcept { return values.capacity(); }
	TOML_EXTERNAL_LINKAGE void array::shrink_to_fit() { values.shrink_to_fit(); }

	TOML_EXTERNAL_LINKAGE
	void array::truncate(size_t new_size)
	{
		if (new_size < values.size())
			values.resize(new_size);
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator pos) noexcept
	{
		return { values.erase(pos.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	array::iterator array::erase(const_iterator first, const_iterator last) noexcept
	{
		return { values.erase(first.raw_, last.raw_) };
	}

	TOML_EXTERNAL_LINKAGE
	void array::pop_back() noexcept
	{
		values.pop_back();
	}

	TOML_EXTERNAL_LINKAGE
	node* array::get(size_t index) noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	const node* array::get(size_t index) const noexcept
	{
		return index < values.size() ? values[index].get() : nullptr;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	bool operator == (const array& lhs, const array& rhs) noexcept
	{
		if (&lhs == &rhs)
			return true;
		if (lhs.values.size() != rhs.values.size())
			return false;
		for (size_t i = 0, e = lhs.values.size(); i < e; i++)
		{
			const auto lhs_type = lhs.values[i]->type();
			const node& rhs_ = *rhs.values[i];
			const auto rhs_type = rhs_.type();
			if (lhs_type != rhs_type)
				return false;

			const bool equal = lhs.values[i]->visit([&](const auto& lhs_) noexcept
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
	bool operator != (const array& lhs, const array& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	TOML_EXTERNAL_LINKAGE
	size_t array::total_leaf_count() const noexcept
	{
		size_t leaves{};
		for (size_t i = 0, e = values.size(); i < e; i++)
		{
			auto arr = values[i]->as_array();
			leaves += arr ? arr->total_leaf_count() : 1_sz;
		}
		return leaves;
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten_child(array&& child, size_t& dest_index) noexcept
	{
		for (size_t i = 0, e = child.size(); i < e; i++)
		{
			auto type = child.values[i]->type();
			if (type == node_type::array)
			{
				array& arr = *reinterpret_cast<array*>(child.values[i].get());
				if (!arr.empty())
					flatten_child(std::move(arr), dest_index);
			}
			else
				values[dest_index++] = std::move(child.values[i]);
		}
	}

	TOML_EXTERNAL_LINKAGE
	void array::flatten()
	{
		if (values.empty())
			return;

		bool requires_flattening = false;
		size_t size_after_flattening = values.size();
		for (size_t i = values.size(); i --> 0_sz;)
		{
			auto arr = values[i]->as_array();
			if (!arr)
				continue;
			size_after_flattening--; //discount the array itself
			const auto leaf_count = arr->total_leaf_count();
			if (leaf_count > 0_sz)
			{
				requires_flattening = true;
				size_after_flattening += leaf_count;
			}
			else
				values.erase(values.cbegin() + static_cast<ptrdiff_t>(i));
		}

		if (!requires_flattening)
			return;

		values.reserve(size_after_flattening);

		size_t i = 0;
		while (i < values.size())
		{
			auto arr = values[i]->as_array();
			if (!arr)
			{
				i++;
				continue;
			}

			std::unique_ptr<node> arr_storage = std::move(values[i]);
			const auto leaf_count = arr->total_leaf_count();
			if (leaf_count > 1_sz)
				preinsertion_resize(i + 1_sz, leaf_count - 1_sz);
			flatten_child(std::move(*arr), i); //increments i
		}
	}
}

//# {{
#endif // !TOML_DOXYGEN
//# }}
