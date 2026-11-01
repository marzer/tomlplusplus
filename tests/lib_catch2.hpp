// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "settings.hpp"

#ifdef __clang__
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#elif defined(_MSC_VER)
#pragma warning(push, 0)
#pragma warning(disable : 4619)
#pragma warning(disable : 4365)
#pragma warning(disable : 4868)
#pragma warning(disable : 5105)
#pragma warning(disable : 5262)
#pragma warning(disable : 5264)
#endif

#if USE_VENDORED_LIBS
#include "../vendor/catch_amalgamated.hpp"
#elif __has_include(<catch2/catch_all.hpp>)
#include <catch2/catch_all.hpp>
#else
#error Catch2 v3 is missing!
#endif

#if defined(CATCH_AMALGAMATED_CUSTOM_MAIN) && USE_VENDORED_LIBS
#include "../vendor/catch_amalgamated.cpp"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
