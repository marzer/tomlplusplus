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

#include "at_path.h"
#include "array.h"
#include "table.h"
TOML_DISABLE_WARNINGS;
#if TOML_INT_CHARCONV
#include <charconv>
#else
#include <sstream>
#endif
TOML_ENABLE_WARNINGS;
#include "header_start.h"

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	node* TOML_CALLCONV get_at_path(node & root, std::string_view path) noexcept
	{
		if (root.is_value()) // values don't have child nodes
			return nullptr;

		size_t pos					= 0;
		const auto end				= path.length();
		node* current				= &root;
		bool prev_was_array_indexer = false;
		bool prev_was_dot			= root.is_table(); // implicit '.' at the start for tables

		while (pos < end && current)
		{
			// start of an array indexer
			if (path[pos] == '[')
			{
				const auto current_array = current->as<array>();
				if (!current_array)
					return nullptr;

				// find first digit in index
				size_t index_start = pos + 1u;
				while (true)
				{
					if TOML_UNLIKELY(index_start >= path.length())
						return nullptr;

					const auto c = path[index_start];
					if TOML_LIKELY(c >= '0' && c <= '9')
						break;
					else if (c == ' ' || c == '\t')
						index_start++;
					else
						return nullptr;
				}
				TOML_ASSERT(path[index_start] >= '0');
				TOML_ASSERT(path[index_start] <= '9');

				// find end of index (first non-digit character)
				size_t index_end = index_start + 1u;
				while (true)
				{
					// if an array indexer is missing the trailing ']' at the end of the string, permissively accept it
					if TOML_UNLIKELY(index_end >= path.length())
						break;

					const auto c = path[index_end];
					if (c >= '0' && c <= '9')
						index_end++;
					else if (c == ']' || c == ' ' || c == '\t' || c == '.' || c == '[')
						break;
					else
						return nullptr;
				}
				TOML_ASSERT(path[index_end - 1u] >= '0');
				TOML_ASSERT(path[index_end - 1u] <= '9');

				// move pos to after indexer (char after closing ']' or permissively EOL/subkey '.'/next opening '[')
				pos = index_end;
				while (true)
				{
					if TOML_UNLIKELY(pos >= path.length())
						break;

					const auto c = path[pos];
					if (c == ']')
					{
						pos++;
						break;
					}
					else if TOML_UNLIKELY(c == '.' || c == '[')
						break;
					else if (c == '\t' || c == '.')
						pos++;
					else
						return nullptr;
				}

				// get array index substring
				auto index_str = path.substr(index_start, index_end - index_start);

				// parse the actual array index
				size_t index;
				if (index_str.length() == 1u)
					index = static_cast<size_t>(index_str[0] - '0');
				else
				{
#if TOML_INT_CHARCONV

					auto fc_result = std::from_chars(index_str.data(), index_str.data() + index_str.length(), index);
					if (fc_result.ec != std::errc{})
						return nullptr;

#else

					std::stringstream ss;
					ss.imbue(std::locale::classic());
					ss.write(index_str.data(), static_cast<std::streamsize>(index_str.length()));
					if (!(ss >> index))
						return nullptr;

#endif
				}

				current				   = current_array->get(index);
				prev_was_dot		   = false;
				prev_was_array_indexer = true;
			}

			// start of a new table child
			else if (path[pos] == '.')
			{
				const auto current_table = current->as<table>();
				if (!current_table)
					return nullptr;

				// a dot immediately following another dot (or at the beginning of the string) is as if we'd asked
				// for an empty child in between, e.g.
				//
				//     foo..bar
				//
				// is equivalent to
				//
				//     "foo".""."bar"
				//
				if (prev_was_dot)
					current = current_table->get(""sv);

				pos++;
				prev_was_dot		   = true;
				prev_was_array_indexer = false;
			}

			// an errant closing ']'
			else if TOML_UNLIKELY(path[pos] == ']')
				return nullptr;

			// some regular subkey
			else
			{
				// get subkey text
				const auto subkey_start = pos;
				const auto subkey_len =
					impl::min(path.find_first_of(".[]"sv, subkey_start + 1u), path.length()) - subkey_start;
				const auto subkey = path.substr(subkey_start, subkey_len);

				// a regular subkey segment immediately after an array indexer is OK if it was all whitespace, e.g.:
				//
				//     "foo[0]  .bar"
				//            ^^ skip this
				//
				// otherwise its an error (since it would have to be preceeded by a dot)
				if (prev_was_array_indexer)
				{
					auto non_ws = subkey.find_first_not_of(" \t");
					if (non_ws == std::string_view::npos)
					{
						pos += subkey_len;
						prev_was_dot		   = false;
						prev_was_array_indexer = false;
						continue;
					}
					else
						return nullptr;
				}

				const auto current_table = current->as<table>();
				if (!current_table)
					return nullptr;

				current = current_table->get(subkey);
				pos += subkey_len;
				prev_was_dot		   = false;
				prev_was_array_indexer = false;
			}
		}

		// a dot at the end is as if we'd asked for an empty child at the end, e.g.
		//
		//     foo.bar.
		//
		// is equivalent to
		//
		//     "foo"."bar".""
		//
		if (current && prev_was_dot)
		{
			const auto current_table = current->as<table>();
			if (!current_table)
				return nullptr;

			current = current_table->get(""sv);
		}

		return current;
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_INTERNAL_LINKAGE
	node* TOML_CALLCONV get_at_path(node & root, std::wstring_view path) noexcept
	{
		if (root.is_value())
			return {};
		if (auto tbl = root.as_table(); tbl && tbl->empty())
			return {};
		if (auto arr = root.as_array(); arr && arr->empty())
			return {};

		return get_at_path(root, impl::narrow(path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	TOML_INTERNAL_LINKAGE
	node* TOML_CALLCONV get_at_path(node & root, const toml::path& path)
	{
		if (root.is_value()) // values don't have child nodes
			return nullptr;

		node* current = &root;

		for (const auto& component : path)
		{
			auto type = component.type;
			if (type == path_component_type::array_index && std::holds_alternative<size_t>(component.value))
			{
				const auto current_array = current->as<array>();
				if (!current_array)
					return nullptr; // not an array, using array index doesn't work

				current = current_array->get(std::get<size_t>(component.value));
			}
			else if (type == path_component_type::key && std::holds_alternative<std::string>(component.value))
			{
				const auto current_table = current->as<table>();
				if (!current_table)
					return nullptr;

				current = current_table->get(std::get<std::string>(component.value));
			}
			else
			{
				// Error: invalid component
				return nullptr;
			}
		}

		return current;
	}
}

TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::string_view path) noexcept
	{
		return node_view<node>{ TOML_ANON_NAMESPACE::get_at_path(root, path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::string_view path) noexcept
	{
		return node_view<const node>{ TOML_ANON_NAMESPACE::get_at_path(const_cast<node&>(root), path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, const toml::path& path) noexcept
	{
		return node_view<node>{ TOML_ANON_NAMESPACE::get_at_path(root, path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, const toml::path& path) noexcept
	{
		return node_view<const node>{ TOML_ANON_NAMESPACE::get_at_path(const_cast<node&>(root), path) };
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> TOML_CALLCONV at_path(node & root, std::wstring_view path)
	{
		return node_view<node>{ TOML_ANON_NAMESPACE::get_at_path(root, path) };
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::wstring_view path)
	{
		return node_view<const node>{ TOML_ANON_NAMESPACE::get_at_path(const_cast<node&>(root), path) };
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT
}
TOML_NAMESPACE_END;

#include "header_end.h"
