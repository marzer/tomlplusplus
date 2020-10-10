// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
/*

	This example is one of diagnostics; it forces a set of specific parsing
	failures and prints their error messages to stdout so you can see what the
	default error messages look like.

*/
#include <iostream>
#include "utf8_console.h"

#define TOML_EXCEPTIONS 0
#define TOML_UNRELEASED_FEATURES 0
#include <toml++/toml.h>

using namespace std::string_view_literals;
using toml::operator""_sz;

namespace
{
	inline constexpr std::string_view invalid_parses[] =
	{
		"########## comments"sv,
		"# bar\rkek"sv,
		"# bar\bkek"sv,
		"# \xf1\x63"sv,

		"########## inline tables"sv,
		"val = {,}"sv,
		"val = {a='b',}"sv,				// allowed when TOML_UNRELEASED_FEATURES == 1
		"val = {a='b',,}"sv,
		"val = {a='b',"sv,
		"val = {a='b',\n c='d'}"sv,		// allowed when TOML_UNRELEASED_FEATURES == 1
		"val = {?='b'}"sv,

		"########## tables"sv,
		"[]"sv,
		"[foo"sv,
		"[foo] ?"sv,
		"[foo] [bar]"sv,
		"[foo]\n[foo]"sv,
		"? = 'foo' ?"sv,
		"[ [foo] ]"sv

		"########## arrays"sv,
		"val = [,]"sv,
		"val = ['a',,]"sv,
		"val = ['a',"sv,

		"########## key-value pairs"sv,
		"val = 'foo' ?"sv,
		"val = "sv,
		"val "sv,
		"val ?"sv,
		"val = ]"sv,
		"[foo]\nbar = 'kek'\nbar = 'kek2'"sv,
		"[foo]\nbar = 'kek'\nbar = 7"sv,
		"[foo.bar]\n[foo]\nbar = 'kek'"sv,
		"[foo]\nbar = 'kek'\nbar.kek = 7"sv,
		"[foo]\nbar.? = 'kek'"sv,
		R"('''val''' = 1)"sv,
		R"(a."""val""" = 1)"sv,
		"1= 0x6cA#+\xf1"sv,

		"########## values"sv,
		"val = _"sv,
		"val = G"sv,

		"########## strings"sv,
		"val = \" \r \""sv,
		R"(val = ")"sv,
		R"(val = "\g")"sv,
		R"(val = "\x20")"sv,			// allowed when TOML_UNRELEASED_FEATURES == 1
		R"(val = "\uFFF")"sv,
		R"(val = "\uFFFG")"sv,
		R"(val = "\UFFFFFFF")"sv,
		R"(val = "\UFFFFFGF")"sv,
		R"(val = "\uD801")"sv,
		R"(val = "\U00110000")"sv,
		R"(val = """ """""")"sv,
		R"(val = ''' '''''')"sv,
		"val = '\n'"sv,

		"########## integers"sv,
		R"(val = -0b0)"sv,
		R"(val = -0o0)"sv,
		R"(val = -0x0)"sv,
		R"(val = +0b0)"sv,
		R"(val = +0o0)"sv,
		R"(val = +0x0)"sv,
		R"(val = 1-)"sv,
		R"(val = -1+)"sv,
		R"(val = -+1)"sv,
		R"(val = 1_0_)"sv,
		R"(val = 1_0_ )"sv,
		R"(val = 999999999999999999999999999999999999 )"sv,
		R"(val = 9223372036854775808 )"sv,
		R"(val = 01 )"sv,

		"########## floats"sv,
		R"(val = 9999999999999999999999999999999999999999999999999999999999999995.0)"sv,
	};
}


int main(int /*argc*/, char** /*argv*/)
{
	std::ios_base::sync_with_stdio(false);
	init_utf8_console();

	for (auto str : invalid_parses)
	{
		if (str.empty())
			continue;

		if (str.substr(0_sz, 10_sz) == "##########"sv)
		{
			const auto substr = str.substr(11_sz);
			size_t cols = 80_sz;
			for (size_t i = (cols - substr.length()) / 2_sz - 1_sz; i-- > 0_sz; )
			{
				std::cout.put('#');
				cols--;
			}
			std::cout.put(' ');
			std::cout << substr;
			std::cout.put(' ');
			cols -= substr.length() + 2_sz;
			while (cols--)
				std::cout.put('#');

			std::cout << "\n\n"sv;
		}
		else
		{
			auto result = toml::parse(str);
			if (!result)
			{
				std::cout << result.error();
				std::cout << "\n\n"sv;
			}
		}
	}
	return 0;
}
