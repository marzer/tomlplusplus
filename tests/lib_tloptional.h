// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#if __has_include(<tloptional/include/tl/optional.hpp>)
#include <tloptional/include/tl/optional.hpp>
#else
#error TartanLlama/optional is missing! You probably need to fetch submodules ("git submodule update --init --depth 1 external/tloptional")
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
