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

#include "at_path.hpp"
#include "array.hpp"
#include "table.hpp"
#include "header_start.hpp"

TOML_ANON_NAMESPACE_START
{
	// reads the array indexer starting at path[pos] and moves pos past it
	TOML_INTERNAL_LINKAGE
	bool parse_path_array_indexer(std::string_view path, size_t& pos, size_t& index) noexcept
	{
		static constexpr auto whitespace = " \t"sv;
		const auto end					 = path.length();

		// find first digit in index
		const auto index_start = path.find_first_not_of(whitespace, pos + 1u);
		if (index_start == std::string_view::npos || path[index_start] < '0' || path[index_start] > '9')
			return false;

		// find end of index (first non-digit character)
		const auto index_end = impl::min(path.find_first_not_of("0123456789"sv, index_start), end);

		// move pos to after indexer (char after closing ']' or permissively EOL/subkey '.'/next opening '[')
		pos = impl::min(path.find_first_not_of(whitespace, index_end), end);
		if (pos < end)
		{
			if (path[pos] == ']')
				pos++;
			else if (path[pos] != '.' && path[pos] != '[')
				return false;
		}

		static constexpr auto max_index = static_cast<size_t>(-1);
		index							= {};
		for (const auto digit : path.substr(index_start, index_end - index_start))
		{
			const auto value = static_cast<size_t>(digit - '0');
			if (index > (max_index - value) / 10u)
				return false;
			index = (index * 10u) + value;
		}
		return true;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV parse_path(const std::string_view path,
								  void* const data,
								  const parse_path_callback<std::string_view> on_key,
								  const parse_path_callback<size_t> on_index)
	{
		// a blank string is a valid path; it's just one component representing the "" key
		if (path.empty())
			return on_key(data, ""sv);

		enum class component
		{
			dot,
			array_indexer,
			key
		};

		size_t pos	   = 0;
		const auto end = path.length();
		auto prev	   = component::dot; // invisible root 'dot'

		while (pos < end)
		{
			// start of an array indexer
			if (path[pos] == '[')
			{
				size_t index = {};
				if (!TOML_ANON_NAMESPACE::parse_path_array_indexer(path, pos, index))
					return false;

				prev = component::array_indexer;

				if (!on_index(data, index))
					return false;
			}

			// start of a new table child
			else if (path[pos] == '.')
			{
				// a dot immediately following another dot (or at the beginning of the string) is as if we'd asked
				// for an empty child in between, e.g.
				//
				//     foo..bar
				//
				// is equivalent to
				//
				//     "foo".""."bar"
				//
				if (prev == component::dot && !on_key(data, ""sv))
					return false;

				pos++;
				prev = component::dot;
			}

			// an errant closing ']'
			else if TOML_UNLIKELY(path[pos] == ']')
				return false;

			// some regular subkey
			else
			{
				const auto subkey_start = pos;
				const auto subkey_len	= impl::min(path.find_first_of(".[]"sv, subkey_start + 1u), end) - subkey_start;
				const auto subkey		= path.substr(subkey_start, subkey_len);
				pos += subkey_len;

				// a regular subkey segment immediately after an array indexer is OK if it was all whitespace, e.g.:
				//
				//     "foo[0]  .bar"
				//            ^^ skip this
				//
				// otherwise its an error (since it would have to be preceded by a dot)
				if (prev == component::array_indexer)
				{
					if (subkey.find_first_not_of(" \t"sv) != std::string_view::npos)
						return false;

					prev = component::key;
					continue;
				}

				prev = component::key;

				if (!on_key(data, subkey))
					return false;
			}
		}

		// Last character was a '.', which implies an empty string key at the end of the path
		if (prev == component::dot && !on_key(data, ""sv))
			return false;

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV has_children(const node& n) noexcept
	{
		if (const auto tbl = n.as_table())
			return !tbl->empty();
		if (const auto arr = n.as_array())
			return !arr->empty();
		return false;
	}

	TOML_EXTERNAL_LINKAGE
	node* TOML_CALLCONV child_at(node & n, std::string_view key) noexcept
	{
		const auto tbl = n.as<table>();
		return tbl ? tbl->get(key) : nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	node* TOML_CALLCONV child_at(node & n, size_t index) noexcept
	{
		const auto arr = n.as<array>();
		return arr ? arr->get(index) : nullptr;
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::string_view path) noexcept
	{
		if (!impl::has_children(root))
			return {};

		node* current = &root;

		static constexpr auto on_key = [](void* data, std::string_view key) noexcept -> bool
		{
			auto& curr = *static_cast<node**>(data);
			TOML_ASSERT_ASSUME(curr);

			curr = impl::child_at(*curr, key);
			return curr != nullptr;
		};

		static constexpr auto on_index = [](void* data, size_t index) noexcept -> bool
		{
			auto& curr = *static_cast<node**>(data);
			TOML_ASSERT_ASSUME(curr);

			curr = impl::child_at(*curr, index);
			return curr != nullptr;
		};

		if (!impl::parse_path(path, static_cast<void*>(&current), on_key, on_index))
			current = nullptr;

		return node_view{ current };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::string_view path) noexcept
	{
		return node_view<const node>{ at_path(const_cast<node&>(root), path).node() };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::wstring_view path)
	{
		// the narrow-string version does this check too;
		// it's hoisted up here to avoid doing the wide -> narrow conversion where it would not be necessary
		// (avoids an allocation)
		if (!impl::has_children(root))
			return {};

		return at_path(root, impl::narrow(path));
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::wstring_view path)
	{
		return node_view<const node>{ at_path(const_cast<node&>(root), path).node() };
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
