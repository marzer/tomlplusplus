//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "header_start.h"
#include "std_vector.h"
#include "std_string.h"
/// \cond
TOML_IMPL_NAMESPACE_START
{

}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{

	/// \brief	A TOML path.
	///
	/// \detail This type parses and represents a path to a TOML node. It validates
	///         the syntax of the path but does not ensure that the path refers to
	///         a valid node in any particular TOML document. If parsing fails,
	///         the object will evaluate as 'falsy', and will be empty.
	///
	/// \cpp
	/// toml::path the_path("animals.cats[1]");
	///
	/// // can use with tbl.at_path or operator[]
	/// std::cout << "second cat: " << tbl[the_path] << "\n";
	/// std::cout << "cats: " << tbl.at_path(the_path.parent_path()) << "\n";
	/// \ecpp
	///
	/// \out
	/// second cat: lion
	/// cats: ['tiger', 'lion', 'puma']
	/// \eout
	class TOML_EXPORTED_CLASS path
	{
	  public:
		/// \brief indicates type of path component, either a key, an index in an array, or invalid
		enum path_component_type
		{
			INVALID		= 0x0,
			KEY			= 0x1,
			ARRAY_INDEX = 0x2
		};

	  private:
		/// \cond

		std::string full_path_;

		bool parse_error_ = false;

		struct path_component
		{
			size_t start;
			size_t length;
			path_component_type type;
		};

		std::vector<path_component> components_;

		void parse_(std::string_view path);

		/// \endcond

	  public:
		/// \brief	Default constructor.
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path() noexcept;

		/// \brief Construct from string
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path(std::string_view path);

		/// \brief Evaluate whether path parsing succeeded
		TOML_EXPORTED_MEMBER_FUNCTION
		inline operator bool() const noexcept
		{
			return parse_error_;
		};

		/// \brief Fetch a path component by index
		TOML_EXPORTED_MEMBER_FUNCTION
		inline std::tuple<std::string_view, path_component_type> operator[](size_t index) const noexcept
		{
			// TODO: Rethink this - string_view in tuple could be held by someone else and become invalid.
			if (index < components_.size())
			{
				auto sv = std::string_view(full_path_).substr(components_[index].start, components_[index].length);

				return std::make_tuple(sv, components_[index].type);
			}

			return std::make_tuple("", path_component_type::INVALID);
		};

		/// \brief number of components in the path
		TOML_EXPORTED_MEMBER_FUNCTION
		inline size_t length() const noexcept
		{
			return components_.size();
		};

		/// \brief	Erases the contents of the path
		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		inline auto begin() const noexcept { return components_.begin(); }

		TOML_EXPORTED_MEMBER_FUNCTION
		inline auto end() const noexcept { return components_.end(); }

		/// \brief	Returns a toml::path object representing the path of the parent node
		TOML_EXPORTED_MEMBER_FUNCTION
		path parent_path() const;

		/// \brief Returns a string representing the path currently pointed at
		TOML_EXPORTED_MEMBER_FUNCTION
		std::string string() const;

	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
