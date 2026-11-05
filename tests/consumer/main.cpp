// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include <toml++/toml.hpp>

int main()
{
	toml::parse_result result = toml::parse("a = 1\nb = { c = 2 }\nd = { e = 3 }\nanswer = 42");
	return result["answer"].value_or(0) == 42 ? 0 : 1;
}
