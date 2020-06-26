// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

TEST_CASE("values - printing")
{
	static constexpr auto print_value = [](auto&& raw)
	{
		auto val = toml::value{ std::forward<decltype(raw)>(raw) };
		std::stringstream ss;
		ss.imbue(std::locale::classic());
		ss << val;
		return ss.str();
	};

	CHECK(print_value(1) == "1");
	CHECK(print_value(1.0f) == "1.0");
	CHECK(print_value(1.0) == "1.0");

	CHECK(print_value(1.5f) == "1.5");
	CHECK(print_value(1.5) == "1.5");

	CHECK(print_value(10) == "10");
	CHECK(print_value(10.0f) == "10.0");
	CHECK(print_value(10.0) == "10.0");

	CHECK(print_value(100) == "100");
	CHECK(print_value(100.0f) == "100.0");
	CHECK(print_value(100.0) == "100.0");

	CHECK(print_value(1000) == "1000");
	CHECK(print_value(1000.0f) == "1000.0");
	CHECK(print_value(1000.0) == "1000.0");

	CHECK(print_value(10000) == "10000");
	CHECK(print_value(10000.0f) == "10000.0");
	CHECK(print_value(10000.0) == "10000.0");

	CHECK(print_value(std::numeric_limits<double>::infinity()) == "inf");
	CHECK(print_value(-std::numeric_limits<double>::infinity()) == "-inf");
	CHECK(print_value(std::numeric_limits<double>::quiet_NaN()) == "nan");

	// only integers for large values;
	// large floats might get output as scientific notation and that's fine
	CHECK(print_value(10000000000) == "10000000000");
	CHECK(print_value(100000000000000) == "100000000000000");

}

