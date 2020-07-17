//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#ifdef _WIN32

	#include <Windows.h>

	inline void init_utf8_console() noexcept
	{
		SetConsoleOutputCP(65001); //CP_UTF8
	}

#else

	inline void init_utf8_console() noexcept
	{
		// no-op
	}

#endif
