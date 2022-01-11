//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "node_view.h"
#include "header_start.h"

#if TOML_EXTERN_TEMPLATES && TOML_IMPLEMENTATION
#include "node_view_extern.inl"
#endif // TOML_EXTERN_TEMPLATES

#include "header_end.h"
