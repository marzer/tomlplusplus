//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
//# {{
#include "toml_preprocessor.h"
#if !TOML_PARSER
	#error This header cannot not be included when TOML_PARSER is disabled.
#endif
//# }}
#include "toml_table.h"
#include "toml_parse_error.h"
#include "toml_parse_result.h"
#include "toml_utf8_streams.h"

/// \cond
TOML_IMPL_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);
	
	[[nodiscard]] TOML_API parse_result do_parse(utf8_reader_interface&&) TOML_MAY_THROW;
	
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_IMPL_NAMESPACE_END;

#if TOML_EXCEPTIONS
	#define TOML_THROW_PARSE_ERROR(msg, path)												\
		throw parse_error{																	\
			msg, source_position{}, std::make_shared<const std::string>(std::move(path))	\
		}
#else
	#define TOML_THROW_PARSE_ERROR(msg, path)												\
		return parse_result{ parse_error{													\
			msg, source_position{}, std::make_shared<const std::string>(std::move(path))	\
		}}
#endif
/// \endcond

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	/// \brief	Parses a TOML document from a string view.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse("a = 3"sv);
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	/// \brief	Parses a TOML document from a string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse("a = 3"sv, "foo.toml");
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#if TOML_WINDOWS_COMPAT

	/// \brief	Parses a TOML document from a string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse("a = 3"sv, L"foo.toml");
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \availability This overload is only available when #TOML_WINDOWS_COMPAT is enabled.
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::wstring_view source_path) TOML_MAY_THROW;

	#endif // TOML_WINDOWS_COMPAT

	#ifdef __cpp_lib_char8_t

	/// \brief	Parses a TOML document from a char8_t string view.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse(u8"a = 3"sv);
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	/// \brief	Parses a TOML document from a char8_t string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse(u8"a = 3"sv, "foo.toml");
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#if TOML_WINDOWS_COMPAT

	/// \brief	Parses a TOML document from a char8_t string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse(u8"a = 3"sv, L"foo.toml");
	/// std::cout << tbl["a"] << "\n";
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \availability This overload is only available when #TOML_WINDOWS_COMPAT is enabled.
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::wstring_view source_path) TOML_MAY_THROW;

	#endif // TOML_WINDOWS_COMPAT

	#endif // __cpp_lib_char8_t

	/// \brief	Parses a TOML document from a stream.
	///
	/// \detail \cpp
	/// std::stringstream ss;
	/// ss << "a = 3"sv;
	/// 
	/// auto tbl = toml::parse(ss);
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \tparam	Char			The stream's underlying character type. Must be 1 byte in size.
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	template <typename Char>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<Char>& doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	/// \brief	Parses a TOML document from a stream.
	///
	/// \detail \cpp
	/// std::stringstream ss;
	/// ss << "a = 3"sv;
	/// 
	/// auto tbl = toml::parse(ss, "foo.toml");
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \tparam	Char			The stream's underlying character type. Must be 1 byte in size.
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	template <typename Char>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<Char>& doc, std::string&& source_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#if TOML_WINDOWS_COMPAT

	/// \brief	Parses a TOML document from a stream.
	///
	/// \detail \cpp
	/// std::stringstream ss;
	/// ss << "a = 3"sv;
	/// 
	/// auto tbl = toml::parse(ss);
	/// std::cout << tbl["a"] << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	///
	/// \availability This overload is only available when #TOML_WINDOWS_COMPAT is enabled.
	/// 
	/// \tparam	Char			The stream's underlying character type. Must be 1 byte in size.
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	template <typename Char>
	[[nodiscard]]
	inline parse_result parse(std::basic_istream<Char>& doc, std::wstring_view source_path) TOML_MAY_THROW
	{
		return parse(doc, impl::narrow(source_path));
	}

	#endif // TOML_WINDOWS_COMPAT

	// Q: "why are the parse_file functions templated??"
	// A: I don't want to force users to drag in <fstream> if they're not going to do
	//    any parsing directly from files. Keeping them templated delays their instantiation
	//    until they're actually required, so only those users wanting to use parse_file()
	//    are burdened by the <fstream> overhead.


	/// \brief	Parses a TOML document from a file.
	///
	/// \detail \cpp
	/// #include <fstream>
	/// 
	/// toml::parse_result get_foo_toml()
	/// {
	///		return toml::parse_file("foo.toml");
	/// }
	/// \ecpp
	/// 
	/// \tparam	Char			The path's character type.
	/// \param 	file_path		The TOML document to parse. Must be valid UTF-8.
	///
	/// \returns	\conditional_return{With exceptions}
	///				A toml::table.
	/// 			\conditional_return{Without exceptions}
	///				A toml::parse_result.
	/// 
	/// \attention You must `#include <fstream>` to use this function (toml++ does not transitively include it for you).
	template <typename Char, typename StreamChar = char>
	[[nodiscard]]
	inline parse_result parse_file(std::basic_string_view<Char> file_path) TOML_MAY_THROW
	{
		static_assert(
			!std::is_same_v<Char, wchar_t> || TOML_WINDOWS_COMPAT,
			"Wide-character file paths are only supported on Windows with TOML_WINDOWS_COMPAT enabled."
		);
		#if TOML_WINDOWS_COMPAT
			static_assert(
				sizeof(Char) == 1 || std::is_same_v<Char, wchar_t>,
				"The file path's underlying character type must be wchar_t or be 1 byte in size."
			);
		#else
			static_assert(
				sizeof(Char) == 1,
				"The file path's underlying character type must be 1 byte in size."
			);
		#endif
		static_assert(
			std::is_same_v<StreamChar, char>,
			"StreamChar must be 'char' (it is as an instantiation-delaying hack and is not user-configurable)."
		);

		std::string file_path_str;
		#if TOML_WINDOWS_COMPAT
		if constexpr (std::is_same_v<Char, wchar_t>)
			file_path_str = impl::narrow(file_path);
		else
		#endif
			file_path_str = std::string_view{ reinterpret_cast<const char*>(file_path.data()), file_path.length() };

		// open file with a custom-sized stack buffer
		using ifstream = std::basic_ifstream<StreamChar>;
		ifstream file;
		StreamChar file_buffer[sizeof(void*) * 1024_sz];
		file.rdbuf()->pubsetbuf(file_buffer, sizeof(file_buffer));
		file.open(file_path_str, ifstream::in | ifstream::binary | ifstream::ate);
		if (!file.is_open())
			TOML_THROW_PARSE_ERROR("File could not be opened for reading", file_path_str);

		// get size
		const auto file_size = file.tellg();
		if (file_size == -1)
			TOML_THROW_PARSE_ERROR("Could not determine file size", file_path_str);
		file.seekg(0, ifstream::beg);

		// read the whole file into memory first if the file isn't too large
		constexpr auto large_file_threshold = 1024 * 1024 * static_cast<int>(sizeof(void*)) * 4; // 32 megabytes on 64-bit
		if (file_size <= large_file_threshold)
		{
			std::vector<StreamChar> file_data;
			file_data.resize(static_cast<size_t>(file_size));
			file.read(file_data.data(), static_cast<std::streamsize>(file_size));
			return parse(std::basic_string_view<StreamChar>{ file_data.data(), file_data.size() }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		else
			return parse(file, std::move(file_path_str));
	}

	#if !defined(DOXYGEN) && !TOML_HEADER_ONLY
		extern template TOML_API parse_result parse(std::istream&, std::string_view) TOML_MAY_THROW;
		extern template TOML_API parse_result parse(std::istream&, std::string&&) TOML_MAY_THROW;
		extern template TOML_API parse_result parse_file(std::string_view) TOML_MAY_THROW;
		#ifdef __cpp_lib_char8_t
			extern template TOML_API parse_result parse_file(std::u8string_view) TOML_MAY_THROW;
		#endif
		#if TOML_WINDOWS_COMPAT
			extern template TOML_API parse_result parse_file(std::wstring_view) TOML_MAY_THROW;
		#endif
	#endif

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const std::basic_string<Char>& file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const Char* file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	inline namespace literals
	{
		TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, lit_ex, lit_noex);

		/// \brief	Parses TOML data from a string literal.
		/// 
		/// \detail \cpp
		/// using namespace toml::literals;
		/// 
		/// auto tbl = "a = 3"_toml;
		/// std::cout << tbl["a"] << "\n";
		/// 
		/// \ecpp
		/// 
		/// \out
		/// 3
		/// \eout
		/// 
		/// \param 	str	The string data. Must be valid UTF-8.
		/// \param 	len	The string length.
		///
		/// \returns	\conditional_return{With exceptions}
		///				A toml::table.
		/// 			\conditional_return{Without exceptions}
		///				A toml::parse_result.
		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW;

		#ifdef __cpp_lib_char8_t

		/// \brief	Parses TOML data from a UTF-8 string literal.
		/// 
		/// \detail \cpp
		/// using namespace toml::literals;
		/// 
		/// auto tbl = u8"a = 3"_toml;
		/// std::cout << tbl["a"] << "\n";
		/// 
		/// \ecpp
		/// 
		/// \out
		/// 3
		/// \eout
		/// 
		/// \param 	str	The string data. Must be valid UTF-8.
		/// \param 	len	The string length.
		///
		/// \returns	\conditional_return{With exceptions}
		///				A toml::table.
		/// 			\conditional_return{Without exceptions}
		///				A toml::parse_result.
		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW;

		#endif // __cpp_lib_char8_t

		TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
	}
}
TOML_NAMESPACE_END;

#undef TOML_THROW_PARSE_ERROR
