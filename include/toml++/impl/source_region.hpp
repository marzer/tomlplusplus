//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_optional.hpp"
#include "std_string.hpp"
#include "forward_declarations.hpp"
#include "print_to_stream.hpp"
#include "header_start.hpp"

TOML_NAMESPACE_START
{
	/// \brief	The integer type used to tally line numbers and columns.
	using source_index = uint32_t;

	/// \brief	A pointer to a shared string resource containing a source path.
	using source_path_ptr = std::shared_ptr<const std::string>;

	/// \brief	A source document line-and-column pair.
	///
	/// \detail \cpp
	/// auto table = toml::parse_file("config.toml"sv);
	/// std::cout << "The node 'description' was defined at "sv
	///		<< table.get("description")->source().begin()
	///		<< "\n";
	/// \ecpp
	///
	/// \out
	///	The value 'description' was defined at line 7, column 15
	/// \eout
	///
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 non-ASCII whitespace and newline characters, but it doesn't give much thought
	///			 to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct TOML_TRIVIAL_ABI source_position
	{
		/// \brief The line number.
		/// \remarks Valid line numbers start at 1.
		source_index line;

		/// \brief The column number.
		/// \remarks Valid column numbers start at 1.
		source_index column;

		/// \brief	Returns true if both line and column numbers are non-zero.
		TOML_PURE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return line > source_index{} //
				&& column > source_index{};
		}

		/// \brief	Equality operator.
		TOML_PURE_GETTER
		friend constexpr bool operator==(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line //
				&& lhs.column == rhs.column;
		}

		/// \brief	Inequality operator.
		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:
		/// \cond

		TOML_PURE_GETTER
		static constexpr uint64_t pack(const source_position& pos) noexcept
		{
			return static_cast<uint64_t>(pos.line) << 32 | static_cast<uint64_t>(pos.column);
		}

		/// \endcond

	  public:
		/// \brief	Less-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		/// \brief	Less-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator<=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		/// \brief	Greater-than operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		/// \brief	Greater-than-or-equal-to operator.
		TOML_PURE_GETTER
		friend constexpr bool operator>=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

		/// \brief	Prints a source_position to a stream.
		///
		/// \detail \cpp
		/// auto tbl = toml::parse("bar = 42"sv);
		///
		/// std::cout << "The value for 'bar' was found on "sv
		///		<< tbl.get("bar")->source().begin()
		///		<< "\n";
		/// \ecpp
		///
		/// \out
		/// The value for 'bar' was found on line 1, column 7
		/// \eout
		///
		/// \param 	lhs	The stream.
		/// \param 	rhs	The source_position.
		///
		/// \returns	The input stream.
		friend std::ostream& operator<<(std::ostream& lhs, const source_position& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	/// \brief	A source document region.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse_file("config.toml"sv);
	/// if (auto server = tbl.get("server"))
	/// {
	///		std::cout << "begin: "sv << server->source().begin << "\n";
	///		std::cout << "end: "sv << server->source().end << "\n";
	///		std::cout << "path: "sv << *server->source().path << "\n";
	///	}
	/// \ecpp
	///
	/// \out
	///	begin: line 3, column 1
	///	end: line 3, column 22
	///	path: config.toml
	/// \eout
	///
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 non-ASCII whitespace and newline characters, but it doesn't give much thought
	///			 to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct source_region
	{
		/// \brief The beginning of the region (inclusive).
		source_position begin;

		/// \brief The end of the region (exclusive).
		source_position end;

		/// \brief	The path to the corresponding source document.
		///
		/// \remarks This will be `nullptr` if no path was provided to toml::parse().
		source_path_ptr path;

#if TOML_ENABLE_WINDOWS_COMPAT

		/// \brief	The path to the corresponding source document as a wide-string.
		///
		/// \availability This function is only available when #TOML_ENABLE_WINDOWS_COMPAT is enabled.
		///
		/// \remarks This will return an empty optional if no path was provided to toml::parse().
		TOML_NODISCARD
		optional<std::wstring> wide_path() const
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

#endif

		/// \brief	Prints a source_region to a stream.
		///
		/// \detail \cpp
		/// auto tbl = toml::parse("bar = 42", "config.toml");
		///
		/// std::cout << "The value for 'bar' was found on "sv
		///		<< tbl.get("bar")->source()
		///		<< "\n";
		/// \ecpp
		///
		/// \out
		/// The value for 'bar' was found on line 1, column 7 of 'config.toml'
		/// \eout
		///
		/// \param 	lhs	The stream.
		/// \param 	rhs	The source_position.
		///
		/// \returns	The input stream.
		friend std::ostream& operator<<(std::ostream& lhs, const source_region& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	/// \brief	Returns the line at the specified line number, from the specified document.
	///
	/// \detail \cpp
	/// auto doc = "alpha = 1\nbeta = 2"sv;
	/// auto second_line = toml::get_line(doc, 2);
	///
	/// std::cout << "The second line says \"" << second_line.value() << "\"\n";
	/// \ecpp
	///
	/// \out
	/// The second line says "beta = 2"
	/// \eout
	///
	/// \param 	doc	The document.
	/// \param 	line_num	The line number (1-based).
	///
	/// \returns	The specified line, excluding any possible trailing carriage return or line feed character.
	/// \remarks	Returns an empty `optional` when the specified line number is out of range, i.e., when
	///				the line number is zero or greater than the total number of lines of the specified document.
	TOML_NODISCARD
	constexpr optional<std::string_view> get_line(std::string_view doc, source_index line_num) noexcept
	{
		if (line_num == 0)
		{
			// Invalid line number. Should be greater than zero.
			return {};
		}

		// The position of the first character of the specified line.
		const auto begin_of_line = [doc, line_num]() -> std::size_t
		{
			if (line_num == 1)
			{
				return 0;
			}

			const auto num_chars_of_doc = doc.size();
			std::size_t current_line_num{ 1 };

			for (std::size_t i{}; i < num_chars_of_doc; ++i)
			{
				if (doc[i] == '\n')
				{
					++current_line_num;

					if (current_line_num == line_num)
					{
						return i + 1;
					}
				}
			}
			return std::string_view::npos;
		}();

		if (begin_of_line >= doc.size())
		{
			return {};
		}

		if (const auto end_of_line = doc.find('\n', begin_of_line); end_of_line != std::string_view::npos)
		{
			const auto num_chars_of_line = end_of_line - begin_of_line;

			// Trim an optional trailing carriage return.
			return doc.substr(begin_of_line,
							  ((num_chars_of_line > 0) && (doc[end_of_line - 1] == '\r')) ? num_chars_of_line - 1
																						  : num_chars_of_line);
		}

		// Return the last line. Apparently this doc has no trailing line break character at the end.
		return doc.substr(begin_of_line);
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
