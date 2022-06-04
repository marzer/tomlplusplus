//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "std_vector.h"
#include "std_string.h"
#include "std_variant.h"

#include "header_start.h"


TOML_NAMESPACE_START
{

	/// \brief Indicates type of path component, either a key, an index in an array, or invalid
	enum class TOML_CLOSED_ENUM path_component_type : uint8_t
	{
		invalid		= 0x0,
		key			= 0x1,
		array_index = 0x2
	};

	/// \brief Holds the value of a path component, either the name of the key in a string_view, or the index of an array as a size_t
	using path_component_value = std::variant<std::string, size_t>;

	/// \brief Represents a single component of a complete 'TOML-path': either a key or an array index
	struct path_component
	{
		path_component_value value;
		path_component_type type;
	};

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
	  private:
		/// \cond

		bool parse_error_ = false;

		std::vector<path_component> components_;

		std::vector<path_component> parse_(std::string_view, bool& parse_success);

		/// \endcond

	  public:
		/// \brief	Default constructor.
		TOML_NODISCARD_CTOR
		path() noexcept = default;

		/// \brief Construct from string
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::string_view);

		/// \brief	Default destructor.
		~path() noexcept = default;

		/// \brief	Copy constructor.
		TOML_NODISCARD_CTOR
		path(const path& other) = default;

		/// \brief	Move constructor.
		TOML_NODISCARD_CTOR
		path(path&& other) noexcept = default;

		/// \brief	Copy-assignment operator.
		path& operator=(const path&) = default;

		/// \brief	Move-assignment operator.
		path& operator=(path&&) = default;

		/// \brief	Append operator.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator/=(path&&) noexcept;

		/// \brief	Append operator.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator/=(std::string_view);

		/// \brief	Append operator.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(path&&) noexcept;

		/// \brief	Append operator.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::string_view);

		TOML_NODISCARD
		/// \brief  Append a path to the current path
		inline path operator+(const toml::path& rhs) const
		{
			toml::path result = { *this };
			result.append(rhs);

			return result;
		}

		TOML_NODISCARD
		/// \brief  Append a path to the current path
		inline path operator/(const toml::path& rhs) const
		{
			return *this + rhs;
		}

		/// \brief Evaluate whether path parsing succeeded
		TOML_NODISCARD
		explicit inline operator bool() const noexcept
		{
			return !parse_error_;
		};

		/// \brief Implicitly cast to a std::string
		TOML_NODISCARD
		inline operator std::string() const { return string(); }

		/// \brief Evaluate whether two paths are the same
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator==(const path& compare) const noexcept;

		/// \brief Evaluate whether two paths are the same
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator==(std::string_view compare) const noexcept;

		/// \brief Evaluate whether two paths are the same
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator==(const char* compare) const noexcept;

		/// \brief Evaluate whether two paths are different
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator!=(const path& compare) const noexcept;

		/// \brief Evaluate whether two paths are different
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator!=(std::string_view compare) const noexcept;

		/// \brief Evaluate whether two paths are different
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator!=(const char* compare) const noexcept;

		/// \brief Fetch a path component by index for modification
		TOML_NODISCARD
		inline path_component& operator[](size_t index) noexcept
		{
			return components_[index];
		};

		/// \brief Fetch a path component by index
		TOML_NODISCARD
		inline const path_component& operator[](size_t index) const noexcept
		{
			return components_[index];
		};

		/// \brief Number of components in the path
		TOML_NODISCARD
		inline size_t size() const noexcept
		{
			return components_.size();
		};

		/// \brief Whether (true) or not (false) the path is empty
		TOML_NODISCARD
		inline bool empty() const { return size() <= 0; }

		/// \brief	Erases the contents of the path
		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		TOML_NODISCARD
		/// \brief  Iterator at the start of the vector of path components (see #path_component)
		inline auto begin() const noexcept { return components_.begin(); }

		TOML_NODISCARD
		/// \brief  Iterator at the end of the vector of path components (see #path_component)
		inline auto end() const noexcept { return components_.end(); }

		/// \brief Removes the number of terminal path components specified by n
		TOML_EXPORTED_MEMBER_FUNCTION
		path& truncate(size_t n);

		/// \brief Returns a toml::path object which has had n terminal path components removed
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path truncated(size_t n) const;

		/// \brief	Returns a toml::path object representing the path of the parent node
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path parent_path() const;

		/// \brief	Returns a toml::path object representing terminal n-parts of a TOML path
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path leaf(size_t n = 1) const;

		/// \brief	Returns a toml::path object that is a specified subpath of the current path, representing the
		/// range of path components from [start, end).
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(std::vector<path_component>::const_iterator start,
					 std::vector<path_component>::const_iterator end) const;

		/// \brief	Returns a toml::path object that is a specified subpath of the current path, representing the
		/// range of path components with indexes from [start, start + length].
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(size_t start, size_t length) const;
		
		/// \brief	Appends elements to the end of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& append(const toml::path&);

		/// \brief	Appends elements to the end of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& append(toml::path&&);

		/// \brief	Appends elements to the end of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& append(std::string_view);

		/// \brief	Prepends elements to the beginning of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(const toml::path&);

		/// \brief	Prepends elements to the beginning of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(toml::path&&);

		/// \brief	Prepends elements to the beginning of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::string_view);

		/// \brief	Replaces the contents of the path object by a new path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& assign(std::string_view);

		/// \brief	Replaces the contents of the path object by a new path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& assign(const path&);

		/// \brief Returns a string representing the path
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::string string() const;

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief Construct from wstring
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::wstring_view);

		/// \brief	Append operator
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator/=(std::wstring_view);

		/// \brief	Append operator.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::wstring_view);

		/// \brief Implicitly cast to a std::wstring
		TOML_NODISCARD
		inline operator std::wstring() const
		{
			return wstring();
		}

		/// \brief Evaluate whether two paths are the same
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator==(std::wstring_view compare) const noexcept;

		/// \brief Evaluate whether two paths are the same
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		bool operator!=(std::wstring_view compare) const noexcept;

		/// \brief	Appends elements to the end of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& append(std::wstring_view);

		/// \brief	Prepends elements to the beginning of the TOML path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::wstring_view);

		/// \brief	Replaces the contents of the path object by a new path
		TOML_EXPORTED_MEMBER_FUNCTION
		path& assign(std::wstring_view);

		/// \brief Returns a wstring representing the path
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::wstring wstring() const;

#endif // TOML_ENABLE_WINDOWS_COMPAT

		// === Hidden friend operators ========================================
		/// \brief	Writes the #toml::path out to the output stream in a human-readbale format
		friend std::ostream& operator<<(std::ostream& os, const toml::path& rhs);

		/// \brief	Reads and parses a #toml::path from the input stream
		friend std::istream& operator>>(std::istream& is, toml::path& rhs);

		/// \brief	Appends a string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator+(const toml::path& lhs, std::string_view rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a c-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator+(const toml::path& lhs, const char* rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a #toml::path object to a string-formatted toml path
		TOML_NODISCARD
		friend path operator+(std::string_view lhs, const toml::path& rhs)
		{
			toml::path result = { rhs };
			result.prepend(lhs);

			return result;
		}

		/// \brief	Appends a #toml::path object to a c-string-formatted toml path
		TOML_NODISCARD
		friend path operator+(const char* lhs, const toml::path& rhs)
		{
			toml::path result = { rhs };
			result.prepend(lhs);

			return result;
		}

		/// \brief	Appends a string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator/(const toml::path& lhs, std::string_view rhs)
		{
			return lhs + rhs;
		}

		/// \brief	Appends a c-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator/(const toml::path& lhs, const char* rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a #toml::path object to a string-formatted toml path
		TOML_NODISCARD
		friend path operator/(std::string_view lhs, const toml::path& rhs)
		{
			return lhs + rhs;
		}

		/// \brief	Appends a #toml::path object to a c-string-formatted toml path
		TOML_NODISCARD
		friend path operator/(const char* lhs, const toml::path& rhs)
		{
			return lhs + rhs;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Appends a wide-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator+(const toml::path& lhs, std::wstring_view rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a wchar_t-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator+(const toml::path& lhs, const wchar_t* rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a #toml::path object to a wide-string-formatted toml path
		TOML_NODISCARD
		friend path operator+(std::wstring_view lhs, const toml::path& rhs)
		{
			toml::path result = { rhs };
			result.prepend(lhs);

			return result;
		}

		/// \brief	Appends a #toml::path object to a wchar_t-string-formatted toml path
		TOML_NODISCARD
		friend path operator+(const wchar_t* lhs, const toml::path& rhs)
		{
			toml::path result = { rhs };
			result.prepend(lhs);

			return result;
		}

		/// \brief	Appends a wide-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator/(const toml::path& lhs, std::wstring_view rhs)
		{
			return lhs + rhs;
		}

		/// \brief	Appends a wchar_t-string-formatted toml path to a #toml::path object
		TOML_NODISCARD
		friend path operator/(const toml::path& lhs, const wchar_t* rhs)
		{
			return lhs + toml::path(rhs);
		}

		/// \brief	Appends a #toml::path object to a wide-string-formatted toml path
		TOML_NODISCARD
		friend path operator/(std::wstring_view lhs, const toml::path& rhs)
		{
			return lhs + rhs;
		}

		/// \brief	Appends a #toml::path object to a wchar_t-string-formatted toml path
		TOML_NODISCARD
		friend path operator/(const wchar_t* lhs, const toml::path& rhs)
		{
			return lhs + rhs;
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	};

	inline namespace literals
	{
		/// \brief	Parses a TOML path from a string literal.
		///
		/// \detail \cpp
		/// using namespace toml::literals;
		///
		/// auto path = "main.settings.devices[2]"_tpath;
		/// std::cout << path.parent_path() << "\n";
		/// \ecpp
		///
		/// \out
		/// main.settings.devices
		/// \eout
		///
		/// \param 	str	The string data.
		/// \param 	len	The string length.
		///
		/// \returns	A #toml::path generated from the string literal.
		TOML_NODISCARD
		inline toml::path operator"" _tpath(const char* str, size_t len)
		{
			return toml::path(std::string_view{ str, len });
		}
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
