//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once
/// \cond

//# {{
#include "preprocessor.h"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#if TOML_EXTERN_TEMPLATES
#include "value.h"
#include "date_time.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	template class value<std::string>;
	template class value<int64_t>;
	template class value<double>;
	template class value<bool>;
	template class value<date>;
	template class value<time>;
	template class value<date_time>;

#define TOML_EXTERN(name, T) template optional<T> node::name<T>() const noexcept

	TOML_EXTERN(value_exact, std::string_view);
	TOML_EXTERN(value_exact, std::string);
	TOML_EXTERN(value_exact, const char*);
	TOML_EXTERN(value_exact, int64_t);
	TOML_EXTERN(value_exact, double);
	TOML_EXTERN(value_exact, date);
	TOML_EXTERN(value_exact, time);
	TOML_EXTERN(value_exact, date_time);
	TOML_EXTERN(value_exact, bool);
	TOML_EXTERN(value, std::string_view);
	TOML_EXTERN(value, std::string);
	TOML_EXTERN(value, const char*);
	TOML_EXTERN(value, signed char);
	TOML_EXTERN(value, signed short);
	TOML_EXTERN(value, signed int);
	TOML_EXTERN(value, signed long);
	TOML_EXTERN(value, signed long long);
	TOML_EXTERN(value, unsigned char);
	TOML_EXTERN(value, unsigned short);
	TOML_EXTERN(value, unsigned int);
	TOML_EXTERN(value, unsigned long);
	TOML_EXTERN(value, unsigned long long);
	TOML_EXTERN(value, double);
	TOML_EXTERN(value, float);
	TOML_EXTERN(value, date);
	TOML_EXTERN(value, time);
	TOML_EXTERN(value, date_time);
	TOML_EXTERN(value, bool);

#if TOML_HAS_CHAR8
	TOML_EXTERN(value_exact, std::u8string_view);
	TOML_EXTERN(value_exact, std::u8string);
	TOML_EXTERN(value_exact, const char8_t*);
	TOML_EXTERN(value, std::u8string_view);
	TOML_EXTERN(value, std::u8string);
	TOML_EXTERN(value, const char8_t*);
#endif

#if TOML_WINDOWS_COMPAT
	TOML_EXTERN(value_exact, std::wstring);
	TOML_EXTERN(value, std::wstring);
#endif

#undef TOML_EXTERN
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif // TOML_EXTERN_TEMPLATES
/// \endcond
