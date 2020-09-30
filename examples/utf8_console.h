//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

inline void init_utf8_console() noexcept
{
	#ifdef _WIN32
	SetConsoleOutputCP(65001); //CP_UTF8
	#endif

	std::cout << std::boolalpha;
}
