//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include <toml++/toml.hpp>

#ifdef __clang__
#pragma clang system_header
#elif defined(__GNUC__)
#pragma GCC system_header
#elif defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#define JSON_HAS_FILESYSTEM				 0
#define JSON_HAS_EXPERIMENTAL_FILESYSTEM 0
#if !defined(USE_VENDORED_LIBS) || USE_VENDORED_LIBS
#include "../vendor/json.hpp"
#elif __has_include(<json/single_include/nlohmann/json.hpp>)
#include <json/single_include/nlohmann/json.hpp>
#elif __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#else
#error nlohmann/json is missing!
#endif

#include <string>
#include <string_view>
#include <iostream>
#include <sstream>

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
