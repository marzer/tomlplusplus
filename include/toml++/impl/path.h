//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "std_vector.h"
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

	  private:
		/// \cond

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path_component&, const path_component&) noexcept;

		/// \endcond
	  public:
		/// \brief	Returns true if two path components represent the same key or array index.
		TOML_PURE_INLINE_GETTER
		friend bool TOML_CALLCONV operator==(const path_component& lhs, const path_component& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		/// \brief	Returns true if two path components do not represent the same key or array index.
		TOML_PURE_INLINE_GETTER
		friend bool TOML_CALLCONV operator!=(const path_component& lhs, const path_component& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}
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

		std::vector<path_component> components_;

		static bool TOML_CALLCONV parse_into(std::string_view, std::vector<path_component>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_to(std::ostream&) const;

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path&, const path&) noexcept;

		/// \endcond

	  public:
		/// \brief	Default constructor.
		TOML_NODISCARD_CTOR
		path() noexcept = default;

		/// \brief Construct a path by parsing from a string.
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief Construct a path by parsing from a string.
		///
		/// \availability This constructor is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::wstring_view);

#endif

		/// \brief	Default destructor.
		~path() noexcept = default;

		/// \brief	Copy constructor.
		TOML_NODISCARD_CTOR
		path(const path&) = default;

		/// \brief	Move constructor.
		TOML_NODISCARD_CTOR
		path(path&&) noexcept = default;

		/// \brief Returns the number of components in the path.
		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return components_.size();
		}

		/// \brief Returns true if the path has one or more components.
		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return !components_.empty();
		}

		/// \brief Whether (true) or not (false) the path is empty
		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return components_.empty();
		}

		/// \brief Fetch a path component by index for modification
		TOML_PURE_INLINE_GETTER
		path_component& operator[](size_t index) noexcept
		{
			return components_[index];
		}

		/// \brief Fetch a path component by index
		TOML_PURE_INLINE_GETTER
		const path_component& operator[](size_t index) const noexcept
		{
			return components_[index];
		}

		/// \name Assignment
		/// @{

		/// \brief	Copy-assignment operator.
		path& operator=(const path&) = default;

		/// \brief	Move-assignment operator.
		path& operator=(path&&) noexcept = default;

		/// \brief	Replaces the contents of the path by parsing from a string.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Replaces the contents of the path by parsing from a string.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::wstring_view);

#endif

		/// \brief	Replaces the contents of the path with that of another.
		TOML_ALWAYS_INLINE
		path& assign(const path& p)
		{
			return *this = p;
		}

		/// \brief	Replaces the contents of the path with that of another.
		TOML_ALWAYS_INLINE
		path& assign(path&& p) noexcept
		{
			return *this = std::move(p);
		}

		/// \brief	Replaces the contents of the path object by a new path
		TOML_ALWAYS_INLINE
		path& assign(std::string_view str)
		{
			return *this = str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Replaces the contents of the path object by a new path
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_ALWAYS_INLINE
		path& assign(std::wstring_view str)
		{
			return *this = str;
		}

#endif

		/// @}

		/// \name Appending
		/// @{

		/// \brief	Appends another path onto the end of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(const path&);

		/// \brief	Appends another path onto the end of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(path&&);

		/// \brief	Parses a path and appends it onto the end of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Parses a path and appends it onto the end of this one.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::wstring_view);

#endif

		/// \brief	Appends another path onto the end of this one.
		TOML_ALWAYS_INLINE
		path& append(const path& p)
		{
			return *this += p;
		}

		/// \brief	Appends another path onto the end of this one.
		TOML_ALWAYS_INLINE
		path& append(path&& p)
		{
			return *this += std::move(p);
		}

		/// \brief	Parses a path and appends it onto the end of this one.
		TOML_ALWAYS_INLINE
		path& append(std::string_view str)
		{
			return *this += str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Parses a path and appends it onto the end of this one.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_ALWAYS_INLINE
		path& append(std::wstring_view str)
		{
			return *this += str;
		}

#endif

		/// @}

		/// \name Prepending
		/// @{

		/// \brief	Prepends another path onto the beginning of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(const path&);

		/// \brief	Prepends another path onto the beginning of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(path&&);

		/// \brief	Parses a path and prepends it onto the beginning of this one.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	Parses a path and prepends it onto the beginning of this one.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::wstring_view);

#endif

		/// @}

		/// \name Concatenation
		/// @{

		/// \brief  Concatenates two paths.
		TOML_NODISCARD
		friend path TOML_CALLCONV operator+(const path& lhs, const path& rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		/// \brief  Concatenates two paths.
		TOML_NODISCARD
		friend path TOML_CALLCONV operator+(const path& lhs, std::string_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		/// \brief  Concatenates two paths.
		TOML_NODISCARD
		friend path TOML_CALLCONV operator+(std::string_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief  Concatenates two paths.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		friend path TOML_CALLCONV operator+(const path& lhs, std::wstring_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		/// \brief  Concatenates two paths.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		friend path TOML_CALLCONV operator+(std::wstring_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#endif

		/// @}

		/// \name String conversion
		/// @{

		/// \brief	Prints the string representation of a #toml::path out to a stream.
		TOML_ALWAYS_INLINE
		friend std::ostream& TOML_CALLCONV operator<<(std::ostream& os, const path& rhs)
		{
			rhs.print_to(os);
			return os;
		}

		/// \brief Returns a string representation of this path.
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::string str() const;

		/// \brief Returns a string representation of this path.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::string() const
		{
			return str();
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief Returns a string representation of this path.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::wstring wide_str() const;

		/// \brief Returns a string representation of this path.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::wstring() const
		{
			return wide_str();
		}

#endif

		/// @}

		/// \name Equality
		/// @{

		/// \brief Returns whether two paths are the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator==(const path& lhs, const path& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		/// \brief Returns whether two paths are not the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator!=(const path& lhs, const path& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		/// \brief Returns whether two paths are the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator==(const path& lhs, std::string_view rhs)
		{
			return lhs == path{ rhs };
		}

		/// \brief Returns whether two paths are the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator==(std::string_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		/// \brief Returns whether two paths are not the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator!=(const path& lhs, std::string_view rhs)
		{
			return lhs != path{ rhs };
		}

		/// \brief Returns whether two paths are not the same.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator!=(std::string_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief Returns whether two paths are the same.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator==(const path& lhs, std::wstring_view rhs)
		{
			return lhs == path{ rhs };
		}

		/// \brief Returns whether two paths are the same.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator==(std::wstring_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		/// \brief Returns whether two paths are not the same.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator!=(const path& lhs, std::wstring_view rhs)
		{
			return lhs != path{ rhs };
		}

		/// \brief Returns whether two paths are not the same.
		///
		/// \availability This overload is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool TOML_CALLCONV operator!=(std::wstring_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#endif // TOML_ENABLE_WINDOWS_COMPAT

		/// @}

		/// \brief	Erases the contents of the path
		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		/// \brief  Iterator at the start of the vector of path components (see #path_component)
		TOML_NODISCARD
		auto begin() const noexcept
		{
			return components_.begin();
		}

		/// \brief  Iterator at the end of the vector of path components (see #path_component)
		TOML_NODISCARD
		auto end() const noexcept
		{
			return components_.end();
		}

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
		path parent() const;

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
		TOML_ALWAYS_INLINE
		path TOML_CALLCONV operator"" _tpath(const char* str, size_t len)
		{
			return path(std::string_view{ str, len });
		}
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
