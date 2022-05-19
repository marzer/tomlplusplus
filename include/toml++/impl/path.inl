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

#include "path.h"

TOML_DISABLE_WARNINGS;
#include <sstream>
#if TOML_INT_CHARCONV
#include <charconv>
#endif
TOML_ENABLE_WARNINGS;
#include "header_start.h"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	path::path() noexcept
	{ }

	TOML_EXTERNAL_LINKAGE
	path::path(std::string_view pathStr) : full_path_(pathStr)
	{
		parse_(full_path_);
	}

	TOML_EXTERNAL_LINKAGE
	void path::parse_(std::string_view pathStr)
	{
		if (pathStr == "") // empty path
			return;

		size_t pos					= 0;
		const auto end				= pathStr.length();
		bool prev_was_array_indexer = false;
		bool prev_was_dot			= true; // invisible root 'dot'

		parse_error_ = false; // success by default, set to true if fails to parse.

		while (pos < end)
		{
			// start of an array indexer
			if (pathStr[pos] == '[')
			{
				// get array index substring
				const auto index_start = pos + 1u;									 // first position after '['
				const auto index_end   = pathStr.find(']', index_start);			 // position of ']'
				if (index_end == std::string_view::npos || index_end == index_start) // nothing in brackets, error
				{
					clear(); // empty object in case of error
					parse_error_ = true;
					return;
				}
				auto index_str = std::string_view(&pathStr[index_start], index_end - index_start);

				// trim whitespace from either side of the index
				const auto first_non_ws = index_str.find_first_not_of(" \t"sv);
				const auto last_non_ws	= index_str.find_last_not_of(" \t"sv);
				if (first_non_ws == std::string_view::npos)
				{
					clear(); // empty object in case of error
					parse_error_ = true;
					return;
				}
				TOML_ASSERT_ASSUME(last_non_ws != std::string_view::npos);

				pos					   = index_end + 1u;
				prev_was_dot		   = false;
				prev_was_array_indexer = true;

				components_.emplace_back(path_component{
					index_start + first_non_ws, index_end - index_start - last_non_ws,
					path_component_type::ARRAY_INDEX });
			}

			// start of a new table child
			else if (pathStr[pos] == '.')
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
				if (prev_was_dot)
					components_.emplace_back(
						path_component{ pos, 0, path_component_type::KEY });

				pos++;
				prev_was_dot		   = true;
				prev_was_array_indexer = false;
			}

			// some regular subkey
			else
			{
				const auto subkey_start = pos;
				const auto subkey_len =
					impl::min(pathStr.find_first_of(".["sv, subkey_start + 1u), pathStr.length()) - subkey_start;
				const auto subkey = pathStr.substr(subkey_start, subkey_len);

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
					{
						clear(); // empty object in case of error
						parse_error_ = true;
						return;
					}
				}

				pos += subkey_len;
				prev_was_dot		   = false;
				prev_was_array_indexer = false;

				components_.emplace_back(
					path_component{ subkey_start, subkey_len, path_component_type::KEY });
			}
		}
	}

	TOML_EXTERNAL_LINKAGE
	void path::clear() noexcept
	{
		this->full_path_._Orphan_all();
		this->full_path_.clear();

		this->components_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	path path::parent_path() const
	{
		path parent {};

		if (components_.size() > 0)
		{
			auto end = --components_.end();
			for (auto it = components_.begin(); it != end; ++it)
			{
				parent.components_.push_back(*it);
			}
			parent.full_path_ = full_path_.substr(0, end->start - 1);
		}

		return parent;
	}

	TOML_EXTERNAL_LINKAGE
	std::string path::string() const
	{
		std::stringstream ss;
		bool atRoot = true;
		 
		for (const auto& component : components_)
		{
			if (component.type == path_component_type::KEY) // key
			{
				ss << (atRoot ? "" : ".") << std::string_view(full_path_).substr(component.start, component.length);
			}
			else if (component.type == path_component_type::ARRAY_INDEX) // array
			{
				ss << "[" << std::string_view(full_path_).substr(component.start, component.length) << "]";
			}
			atRoot = false;
		}
		
		return ss.str();
	}

}
TOML_NAMESPACE_END;

#include "header_end.h"
