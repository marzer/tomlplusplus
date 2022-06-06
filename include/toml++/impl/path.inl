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
#include <ostream>
#if TOML_INT_CHARCONV
#include <charconv>
#endif
TOML_ENABLE_WARNINGS;
#include "print_to_stream.h"
#include "header_start.h"

//#=====================================================================================================================
//# toml::path_component
//#=====================================================================================================================

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV path_component::equal(const path_component& lhs, const path_component& rhs) noexcept
	{
		return lhs.type == rhs.type && lhs.value == rhs.value;
	}
}
TOML_NAMESPACE_END;

//#=====================================================================================================================
//# toml::path
//#=====================================================================================================================

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV path::parse_into(std::string_view path_str, std::vector<path_component> & components)
	{
		// a blank string is a valid path; it's just one component representing the "" key
		if (path_str.empty())
		{
			components.emplace_back(path_component{ ""s, path_component_type::key });
			return true;
		}

		auto str_start				= path_str.data();
		size_t pos					= 0;
		const auto end				= path_str.length();
		bool prev_was_array_indexer = false;
		bool prev_was_dot			= true; // invisible root 'dot'

		const auto parse_failed = [&, original_len = components.size()]() noexcept -> bool
		{
			if (components.size() > original_len)
				components.resize(original_len);
			return false;
		};

		while (pos < end)
		{
			// start of an array indexer
			if (path_str[pos] == '[')
			{
				// find first digit in index
				size_t index_start = pos + 1u;
				while (true)
				{
					if TOML_UNLIKELY(index_start >= path_str.length())
						return parse_failed();

					const auto c = path_str[index_start];
					if TOML_LIKELY(c >= '0' && c <= '9')
						break;
					else if (c == ' ' || c == '\t')
						index_start++;
					else
						return parse_failed();
				}
				TOML_ASSERT(path_str[index_start] >= '0');
				TOML_ASSERT(path_str[index_start] <= '9');

				// find end of index (first non-digit character)
				size_t index_end = index_start + 1u;
				while (true)
				{
					// if an array indexer is missing the trailing ']' at the end of the string, permissively accept it
					if TOML_UNLIKELY(index_end >= path_str.length())
						break;

					const auto c = path_str[index_end];
					if (c >= '0' && c <= '9')
						index_end++;
					else if (c == ']' || c == ' ' || c == '\t' || c == '.' || c == '[')
						break;
					else
						return parse_failed();
				}
				TOML_ASSERT(path_str[index_end - 1u] >= '0');
				TOML_ASSERT(path_str[index_end - 1u] <= '9');

				// move pos to after indexer (char after closing ']' or permissively EOL/subkey '.'/next opening '[')
				pos = index_end;
				while (true)
				{
					if TOML_UNLIKELY(pos >= path_str.length())
						break;

					const auto c = path_str[pos];
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
						return parse_failed();
				}

				// get array index substring
				auto index_str = path_str.substr(index_start, index_end - index_start);

				// parse the actual array index to an integer type
				size_t index;
				if (index_str.length() == 1u)
					index = static_cast<size_t>(index_str[0] - '0');
				else
				{
#if TOML_INT_CHARCONV

					auto fc_result = std::from_chars(index_str.data(), index_str.data() + index_str.length(), index);
					if (fc_result.ec != std::errc{})
					{
						return parse_failed();
					}

#else

					std::stringstream ss;
					ss.imbue(std::locale::classic());
					ss.write(index_str.data(), static_cast<std::streamsize>(index_str.length()));
					if (!(ss >> index))
					{
						return parse_failed();
					}

#endif
				}

				prev_was_dot		   = false;
				prev_was_array_indexer = true;

				components.emplace_back(path_component{ { index }, path_component_type::array_index });
			}

			// start of a new table child
			else if (path_str[pos] == '.')
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
					components.emplace_back(path_component{ ""s, path_component_type::key });

				pos++;
				prev_was_dot		   = true;
				prev_was_array_indexer = false;
			}

			// an errant closing ']'
			else if TOML_UNLIKELY(path_str[pos] == ']')
				return parse_failed();

			// some regular subkey
			else
			{
				const auto subkey_start = pos;
				const auto subkey_len =
					impl::min(path_str.find_first_of(".[]"sv, subkey_start + 1u), path_str.length()) - subkey_start;
				const auto subkey = path_str.substr(subkey_start, subkey_len);

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
						return parse_failed();
					}
				}

				pos += subkey_len;
				prev_was_dot		   = false;
				prev_was_array_indexer = false;

				components.emplace_back(
					path_component{ std::string(str_start + subkey_start, subkey_len), path_component_type::key });
			}
		}

		if (prev_was_dot) // Last character was a '.', which implies an empty string key at the end of the path
		{
			components.emplace_back(path_component{ ""s, path_component_type::key });
		}

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	void path::print_to(std::ostream & os) const
	{
		bool root = true;
		for (const auto& component : components_)
		{
			if (component.type == path_component_type::key) // key
			{
				if (!root)
					impl::print_to_stream(os, '.');
				impl::print_to_stream(os, std::get<std::string>(component.value));
			}
			else if (component.type == path_component_type::array_index) // array
			{
				impl::print_to_stream(os, '[');
				impl::print_to_stream(os, std::get<size_t>(component.value));
				impl::print_to_stream(os, ']');
			}
			root = false;
		}
	}

	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV path::equal(const path& lhs, const path& rhs) noexcept
	{
		return lhs.components_ == rhs.components_;
	}

	//#=== constructors =================================================

	TOML_EXTERNAL_LINKAGE
	path::path(std::string_view str) //
	{
		parse_into(str, components_);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path::path(std::wstring_view str) //
		: path(impl::narrow(str))
	{}

#endif

	//#=== assignment =================================================

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(std::string_view rhs)
	{
		components_.clear();
		parse_into(rhs, components_);
		return *this;
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(std::wstring_view rhs)
	{
		return assign(impl::narrow(rhs));
	}

#endif

	//#=== appending =================================================

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(const path& rhs)
	{
		components_.insert(components_.cend(), rhs.begin(), rhs.end());
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(path&& rhs)
	{
		components_.insert(components_.end(),
						   std::make_move_iterator(rhs.components_.begin()),
						   std::make_move_iterator(rhs.components_.end()));
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::string_view str)
	{
		parse_into(str, components_);
		return *this;
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::wstring_view str)
	{
		return *this += impl::narrow(str);
	}

#endif

	//#=== prepending =================================================

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(const path& source)
	{
		components_.insert(components_.begin(), source.components_.begin(), source.components_.end());
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(path && source)
	{
		components_.insert(components_.begin(),
						   std::make_move_iterator(source.components_.begin()),
						   std::make_move_iterator(source.components_.end()));
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(std::string_view source)
	{
		return prepend(path{ source });
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(std::wstring_view source)
	{
		return prepend(impl::narrow(source));
	}

#endif

	//#=== string conversion =================================================

	TOML_EXTERNAL_LINKAGE
	std::string path::str() const
	{
		if (empty())
			return "";

		std::ostringstream ss;
		print_to(ss);
		return std::move(ss).str();
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	std::wstring path::wide_str() const
	{
		return impl::widen(str());
	}

#endif

	//#=== equality and comparison =================================================

	TOML_EXTERNAL_LINKAGE
	void path::clear() noexcept
	{
		components_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	path& path::truncate(size_t n)
	{
		n = n > components_.size() ? components_.size() : n;

		auto it_end = components_.end();
		components_.erase(it_end - static_cast<int>(n), it_end);

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path path::truncated(size_t n) const
	{
		path truncated_path{};

		n = n > components_.size() ? components_.size() : n;

		// Copy all components except one
		// Need at least two path components to have a parent, since if there is
		// only one path component, the parent is the root/null path ""
		truncated_path.components_.insert(truncated_path.components_.begin(),
										  components_.begin(),
										  components_.end() - static_cast<int>(n));

		return truncated_path;
	}

	TOML_EXTERNAL_LINKAGE
	path path::parent() const
	{
		return truncated(1);
	}

	TOML_EXTERNAL_LINKAGE
	path path::leaf(size_t n) const
	{
		path leaf_path{};

		n = n > components_.size() ? components_.size() : n;

		if (n > 0)
		{
			leaf_path.components_.insert(leaf_path.components_.begin(),
										 components_.end() - static_cast<int>(n),
										 components_.end());
		}

		return leaf_path;
	}

	TOML_EXTERNAL_LINKAGE
	path path::subpath(std::vector<path_component>::const_iterator start,
					   std::vector<path_component>::const_iterator end) const
	{
		if (start >= end)
			return {};

		path subpath;
		subpath.components_.insert(subpath.components_.begin(), start, end);
		return subpath;
	}

	TOML_EXTERNAL_LINKAGE
	path path::subpath(size_t start, size_t length) const
	{
		return subpath(begin() + static_cast<int>(start), begin() + static_cast<int>(start + length));
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
