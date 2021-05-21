// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

// This example demonstrates how to parse TOML from a file and re-serialize it (print it out) to stdout.

#include "examples.h"

#define TOML_UNRELEASED_FEATURES 1
#include <toml++/toml.h>

using namespace std::string_view_literals;

int main(int argc, char** argv)
{
	examples::init();

	const auto path = argc > 1 ? std::string_view{ argv[1] } : "example.toml"sv;
	try
	{
		const auto table = toml::parse_file(path);
		std::cout << table << "\n";
	}
	catch (const toml::parse_error& err)
	{
		std::cerr << err << "\n";
		return 1;
	}
	return 0;
}
