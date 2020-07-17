// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#if __has_include(<tloptional/include/tl/optional.hpp>)
	#include <tloptional/include/tl/optional.hpp>
#else
	#error  TartanLlama/optional is missing! You probably need to fetch submodules ("git submodule update --init extern/tloptional")
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

