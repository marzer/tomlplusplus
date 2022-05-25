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
#include "std_algorithm.h"

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
	path::path(std::string_view path_str)
	{
		components_ = parse_(path_str, parse_error_);
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(const path& rhs)
	{
		if (&rhs != this && rhs.components_.size() > 0)
		{
			components_.resize(rhs.size());
			std::copy(rhs.components_.begin(), rhs.components_.end(), components_.begin());
		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator=(path&& rhs) noexcept
	{
		if (&rhs != this && rhs.components_.size() > 0)
		{
			components_ = std::move(rhs.components_);

		}
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator/=(path&& rhs) noexcept
	{
		return append(std::move(rhs));
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator/=(std::string_view source)
	{
		return append(source);
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(path&& rhs) noexcept
	{
		return append(std::move(rhs));
	}

	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::string_view source)
	{
		return append(source);
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator==(const path& compare) const noexcept
	{
		if (components_.size() != compare.components_.size())
			return false;

		for (size_t i = 0; i < components_.size(); ++i)
		{
			if (components_[i].type != compare.components_[i].type
				|| components_[i].value != compare.components_[i].value)
				return false;
		}

		return true;
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator==(std::string_view compare) const noexcept
	{
		return string() == compare;
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator==(const char* compare) const noexcept
	{
		return string() == std::string_view(compare);
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator!=(const path& compare) const noexcept
	{
		return !(*this == compare);
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator!=(std::string_view compare) const noexcept
	{
		return !(*this == compare);
	}

	TOML_EXTERNAL_LINKAGE
	bool path::operator!=(const char* compare) const noexcept
	{
		return !(*this == std::string_view(compare));
	}

	TOML_EXTERNAL_LINKAGE
	std::vector<path_component> path::parse_(std::string_view path_str, bool& parse_error)
	{
		std::vector<path_component> parsed_components;
		parse_error = false; // success by default, set to true if fails to parse.

		if (path_str == "") // empty path
			return parsed_components;

		auto str_start				= path_str.data();
		size_t pos					= 0;
		const auto end				= path_str.length();
		bool prev_was_array_indexer = false;
		bool prev_was_dot			= true; // invisible root 'dot'

		while (pos < end)
		{
			// start of an array indexer
			if (path_str[pos] == '[')
			{
				// get array index substring
				const auto index_start = pos + 1u;									 // first position after '['
				const auto index_end   = path_str.find(']', index_start);			 // position of ']'
				if (index_end == std::string_view::npos || index_end == index_start) // nothing in brackets, error
				{
					parsed_components.clear(); // empty object in case of error
					parse_error = true;
					return parsed_components;
				}
				auto index_str = std::string_view(&path_str[index_start], index_end - index_start);

				// trim whitespace from either side of the index
				const auto first_non_ws = index_str.find_first_not_of(" \t"sv);
				const auto last_non_ws	= index_str.find_last_not_of(" \t"sv);
				if (first_non_ws == std::string_view::npos)
				{
					parsed_components.clear(); // empty object in case of error
					parse_error = true;
					return parsed_components;
				}
				TOML_ASSERT_ASSUME(last_non_ws != std::string_view::npos);
				index_str = index_str.substr(first_non_ws, (last_non_ws - first_non_ws) + 1u);

				// parse the actual array index to an integer type
				size_t index;
				if (index_str.length() == 1u && index_str[0] >= '0' && index_str[0] <= '9')
					index = static_cast<size_t>(index_str[0] - '0');
				else
				{
#if TOML_INT_CHARCONV

					auto fc_result = std::from_chars(index_str.data(), index_str.data() + index_str.length(), index);
					if (fc_result.ec != std::errc{} || fc_result.ptr != index_str.data() + index_str.length())
					{
						parsed_components.clear(); // empty object in case of error
						parse_error_ = true;
						return parsed_components;
					}

#else

					std::stringstream ss;
					ss.imbue(std::locale::classic());
					ss.write(index_str.data(), static_cast<std::streamsize>(index_str.length()));
					if (!(ss >> index))
					{
						clear(); // empty object in case of error
						parse_error_ = true;
						return;
					}

#endif
				}

				pos					   = index_end + 1u;
				prev_was_dot		   = false;
				prev_was_array_indexer = true;

				parsed_components.emplace_back(path_component{ { index }, path_component_type::ARRAY_INDEX });
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
					parsed_components.emplace_back(path_component{ { ""s }, path_component_type::KEY });

				pos++;
				prev_was_dot		   = true;
				prev_was_array_indexer = false;
			}

			// some regular subkey
			else
			{
				const auto subkey_start = pos;
				const auto subkey_len =
					impl::min(path_str.find_first_of(".["sv, subkey_start + 1u), path_str.length()) - subkey_start;
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
						parsed_components.clear(); // empty object in case of error
						parse_error = true;
						return parsed_components;
					}
				}

				pos += subkey_len;
				prev_was_dot		   = false;
				prev_was_array_indexer = false;

				parsed_components.emplace_back(path_component{
					std::string(std::string_view{ str_start + subkey_start, subkey_len }),
					path_component_type::KEY
				});
			}
		}

		if (prev_was_dot) // Last character was a '.', which implies an empty string key at the end of the path
		{
			parsed_components.emplace_back(path_component{ ""s, path_component_type::KEY });
		}

		return parsed_components;
	}

	TOML_EXTERNAL_LINKAGE
	void path::clear() noexcept
	{
		this->components_.clear();
	}

	TOML_EXTERNAL_LINKAGE
	path& path::truncate(int n)
	{
		n = std::clamp(n, 0, static_cast<int>(components_.size()));

		auto it_end	  = components_.end();
		components_.erase(it_end - n, it_end);

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path path::truncated(int n) const
	{
		path truncated_path {};

		n = std::clamp(n, 0, static_cast<int>(components_.size()));

		// Copy all components except one
		// Need at least two path components to have a parent, since if there is
		// only one path component, the parent is the root/null path ""
		truncated_path.components_.insert(truncated_path.components_.begin(),
										  components_.begin(),
										  components_.end() - n);

		return truncated_path;
	}


	TOML_EXTERNAL_LINKAGE
	path path::parent_path() const
	{
		return truncated(1);
	}

	TOML_EXTERNAL_LINKAGE
	path path::leaf(int n) const
	{
		toml::path leaf_path {};

		n = std::clamp(n, 0, static_cast<int>(components_.size())); // Limit to number of elements in path and > 0

		if (n > 0)
		{
			leaf_path.components_.insert(leaf_path.components_.begin(),
										 components_.end() - n,
										 components_.end());
		}

		return leaf_path;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::append(const toml::path& source)
	{
		parse_error_ = false; // This will end up being a valid path when appended (even if previously failed and now
							  // empty)

		// Copy path parts to this object
		for (const auto& component : source.components_)
		{
			components_.push_back(component);
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::append(toml::path&& source)
	{
		parse_error_ = false; // This will end up being a valid path when appended (even if previously failed and now
							  // empty)

		// Copy path parts to this object
		for (auto& component : source.components_)
		{
			components_.emplace_back(std::move(component));
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::append(std::string_view source)
	{
		auto components_to_append = parse_(source, parse_error_);
		for (auto& component : components_to_append)
		{
			components_.emplace_back(std::move(component));
		}

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(const toml::path& source)
	{
		parse_error_ = false; // This will end up being a valid path when appended (even if previously failed and now
							  // empty)

		components_.insert(components_.begin(), source.components_.begin(), source.components_.end());

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(toml::path&& source)
	{
		parse_error_ = false; // This will end up being a valid path when appended (even if previously failed and now
							  // empty)

		components_.insert(components_.begin(), std::make_move_iterator(source.components_.begin()), std::make_move_iterator(source.components_.end()));

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::prepend(std::string_view source)
	{
		auto components_to_prepend = parse_(source, parse_error_);

		components_.insert(components_.begin(), components_to_prepend.begin(), components_to_prepend.end());

		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::assign(std::string_view source)
	{
		components_ = parse_(source, parse_error_);
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	path& path::assign(const path& source)
	{
		if (source)
		{
			components_ = source.components_;
		}
		else // propagate error of source
		{
			clear();
			parse_error_ = true;
		}

		return *this;
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
				ss << (atRoot ? "" : ".") << std::get<std::string>(component.value);
			}
			else if (component.type == path_component_type::ARRAY_INDEX) // array
			{
				ss << "[" << std::get<size_t>(component.value) << "]";
			}
			atRoot = false;
		}
		
		return ss.str();
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	/// \brief Construct from wstring
	TOML_EXTERNAL_LINKAGE
	path::path(std::wstring_view path_str) : path(impl::narrow(path_str))
	{ }

	/// \brief	Append operator
	TOML_EXTERNAL_LINKAGE
	path& path::operator/=(std::wstring_view rhs)
	{
		return append(rhs);
	}

	/// \brief	Append operator.
	TOML_EXTERNAL_LINKAGE
	path& path::operator+=(std::wstring_view rhs)
	{
		return append(rhs);
	}

	/// \brief Evaluate whether two paths are the same
	TOML_EXTERNAL_LINKAGE
	bool path::operator==(std::wstring_view compare) const noexcept
	{
		return *this == impl::narrow(compare);
	}

	/// \brief Evaluate whether two paths are the same
	TOML_EXTERNAL_LINKAGE
	bool path::operator!=(std::wstring_view compare) const noexcept
	{
		return *this != impl::narrow(compare);
	}

	/// \brief	Appends elements to the end of the TOML path
	TOML_EXTERNAL_LINKAGE
	path& path::append(std::wstring_view source)
	{
		return append(impl::narrow(source));
	}

	/// \brief	Prepends elements to the beginning of the TOML path
	TOML_EXTERNAL_LINKAGE
		path& path::prepend(std::wstring_view source)
	{
		return prepend(impl::narrow(source));
	}

	/// \brief	Replaces the contents of the path object by a new path
	TOML_EXTERNAL_LINKAGE
	path& path::assign(std::wstring_view source)
	{
		return assign(impl::narrow(source));
	}

	/// \brief Returns a wstring representing the path
	TOML_EXTERNAL_LINKAGE
	std::wstring path::wstring() const
	{
		return impl::widen(string());
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

}
TOML_NAMESPACE_END;

#include "header_end.h"
