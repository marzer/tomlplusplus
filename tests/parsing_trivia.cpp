// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include "toml++/impl/forward_declarations.hpp"
#include "toml++/impl/toml_formatter.hpp"
#include <iostream>

TEST_CASE("parsing - trivia")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		R"(
integers = [ 1, 2, 3 ]
integers2 = [
	1, 2, 3,
]
integers3 = [
	1,
	2, # this is ok
]
# Colors?
colors = [ "red", "yellow", "green" ]
nested_array_of_int = [ [ 1, 2 ], [3, 4, 5] , ]
nested_mixed_array = [ [ 1, 2 ], ["a", "b", "c"] ]
string_array = [ "all", 'strings', """are the same""", '''type''' ]
inline_table = {number =1 , "string"= 'hi', array=[  1,2, 3] }

[asdf]
key = "value"

)"sv,
		[](table&& tbl)
		{
			tbl.for_each(
				[](const toml::key& key, auto&& val)
				{
					CHECK(key.leading_trivia().has_value());
					auto vector = key.leading_trivia();

					std::cout << "Key: " << key.str() << "\n";
					for (auto & trivia_piece : *vector)
					{
						if (trivia_piece.type == trivia_type::whitespace)
							std::cout << "whitespace: '";
						else
							std::cout << "comment: '";
						std::cout << trivia_piece.text;
						std::cout << "'\n";
					}
					std::cout << "\n";

					if (val.as_array() != nullptr)
					{
						std::cout << "Value[0]:\n";
						for (auto & trivia_piece : *val.as_array()->leading_trivia())
						{
							if (trivia_piece.type == trivia_type::whitespace)
								std::cout << "whitespace: '";
							else
								std::cout << "comment: '";
							std::cout << trivia_piece.text;
							std::cout << "'\n";
						}
					}
					std::cout << "\n";
				});

			std::cout << "Formatted:\n";
			std::cout << toml_formatter(tbl, toml_formatter::default_flags | format_flags::preserve_source_trivia);
		},
		{},
		true);
}
