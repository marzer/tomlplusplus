// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

import tomlplusplus;

int main()
{
	toml::parse_result result = toml::parse("answer = 42");
	return result["answer"].value_or(0) == 42 ? 0 : 1;
}
