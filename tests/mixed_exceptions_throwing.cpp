// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include <toml++/toml.hpp>
#include <string>

std::string parse_throwing(std::string_view source, bool from_file);

std::string parse_throwing(std::string_view source, bool from_file)
{
	try
	{
		(void)(from_file ? toml::parse_file(source) : toml::parse(source));
		return "parsed";
	}
	catch (const toml::parse_error& err)
	{
		return std::to_string(err.source().begin.line) + ":" + std::to_string(err.source().begin.column) + ": "
			 + std::string{ err.description() };
	}
}
