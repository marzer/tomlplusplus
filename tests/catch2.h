// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "settings.h"

#if __has_include(<Catch2/single_include/catch2/catch.hpp>)
	#include <Catch2/single_include/catch2/catch.hpp>
#else
	#error Catch2 is missing! You probably need to fetch submodules ("git submodule update --init extern/Catch2")
#endif
