// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const int&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const unsigned int&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const bool&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const float&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const double&);
template bool parse_expected_value(std::string_view, uint32_t, std::string_view, const toml::string_view&);

namespace std
{
	template class unique_ptr<const Catch::IExceptionTranslator>;
}
namespace Catch
{
	template struct StringMaker<node_view<node>>;
	template struct StringMaker<node_view<const node>>;
	template ReusableStringStream& ReusableStringStream::operator << (toml::node_view<toml::node> const&);
	template ReusableStringStream& ReusableStringStream::operator << (toml::node_view<const toml::node> const&);
	namespace Detail
	{
		template std::string stringify(const node_view<node>&);
		template std::string stringify(const node_view<const node>&);
	}
}
