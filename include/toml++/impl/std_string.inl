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

#if TOML_WINDOWS
#include "std_string.hpp"
#ifndef _WINDOWS_
#if TOML_INCLUDE_WINDOWS_H
#include <windows.h>
#else

extern "C" __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int CodePage,
																   unsigned long dwFlags,
																   const wchar_t* lpWideCharStr,
																   int cchWideChar,
																   char* lpMultiByteStr,
																   int cbMultiByte,
																   const char* lpDefaultChar,
																   int* lpUsedDefaultChar);

extern "C" __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int CodePage,
																   unsigned long dwFlags,
																   const char* lpMultiByteStr,
																   int cbMultiByte,
																   wchar_t* lpWideCharStr,
																   int cchWideChar);

#endif // TOML_INCLUDE_WINDOWS_H
#endif // _WINDOWS_
#include "header_start.hpp"

TOML_IMPL_NAMESPACE_START
{
	// CP_UTF8
	inline constexpr unsigned int utf8_code_page = 65001u;

	TOML_EXTERNAL_LINKAGE
	std::string narrow(std::wstring_view str)
	{
		// the win32 conversions take an int length; a longer string converts to nothing
		const auto str_len = static_cast<int>(str.length());
		if (str.empty() || static_cast<size_t>(str_len) != str.length())
			return {};

		std::string s;
		const auto len = ::WideCharToMultiByte(utf8_code_page, 0, str.data(), str_len, nullptr, 0, nullptr, nullptr);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::WideCharToMultiByte(utf8_code_page, 0, str.data(), str_len, s.data(), len, nullptr, nullptr);
		}
		return s;
	}

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::string_view str)
	{
		const auto str_len = static_cast<int>(str.length());
		if (str.empty() || static_cast<size_t>(str_len) != str.length())
			return {};

		std::wstring s;
		const auto len = ::MultiByteToWideChar(utf8_code_page, 0, str.data(), str_len, nullptr, 0);
		if (len)
		{
			s.resize(static_cast<size_t>(len));
			::MultiByteToWideChar(utf8_code_page, 0, str.data(), str_len, s.data(), len);
		}
		return s;
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	std::wstring widen(std::u8string_view str)
	{
		if (str.empty())
			return {};

		return widen(std::string_view{ reinterpret_cast<const char*>(str.data()), str.length() });
	}

#endif // TOML_HAS_CHAR8
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_WINDOWS
