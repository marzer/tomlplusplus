//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
/// \cond

#include "node_view.h"

TOML_NAMESPACE_START
{
	TOML_EXTERN
	template class TOML_API node_view<node>;
	TOML_EXTERN
	template class TOML_API node_view<const node>;

#define TOML_EXTERN_FUNC(name, T)                                                                                      \
	TOML_EXTERN                                                                                                        \
	template TOML_API                                                                                                  \
	optional<T> node_view<node>::name<T>() const TOML_EXTERN_NOEXCEPT(impl::value_retrieval_is_nothrow<T>);            \
	TOML_EXTERN                                                                                                        \
	template TOML_API                                                                                                  \
	optional<T> node_view<const node>::name<T>() const TOML_EXTERN_NOEXCEPT(impl::value_retrieval_is_nothrow<T>)

	TOML_EXTERN_FUNC(value_exact, std::string_view);
	TOML_EXTERN_FUNC(value_exact, std::string);
	TOML_EXTERN_FUNC(value_exact, const char*);
	TOML_EXTERN_FUNC(value_exact, int64_t);
	TOML_EXTERN_FUNC(value_exact, double);
	TOML_EXTERN_FUNC(value_exact, date);
	TOML_EXTERN_FUNC(value_exact, time);
	TOML_EXTERN_FUNC(value_exact, date_time);
	TOML_EXTERN_FUNC(value_exact, bool);
	TOML_EXTERN_FUNC(value, std::string_view);
	TOML_EXTERN_FUNC(value, std::string);
	TOML_EXTERN_FUNC(value, const char*);
	TOML_EXTERN_FUNC(value, signed char);
	TOML_EXTERN_FUNC(value, signed short);
	TOML_EXTERN_FUNC(value, signed int);
	TOML_EXTERN_FUNC(value, signed long);
	TOML_EXTERN_FUNC(value, signed long long);
	TOML_EXTERN_FUNC(value, unsigned char);
	TOML_EXTERN_FUNC(value, unsigned short);
	TOML_EXTERN_FUNC(value, unsigned int);
	TOML_EXTERN_FUNC(value, unsigned long);
	TOML_EXTERN_FUNC(value, unsigned long long);
	TOML_EXTERN_FUNC(value, double);
	TOML_EXTERN_FUNC(value, float);
	TOML_EXTERN_FUNC(value, date);
	TOML_EXTERN_FUNC(value, time);
	TOML_EXTERN_FUNC(value, date_time);
	TOML_EXTERN_FUNC(value, bool);

#if TOML_HAS_CHAR8
	TOML_EXTERN_FUNC(value_exact, std::u8string_view);
	TOML_EXTERN_FUNC(value_exact, std::u8string);
	TOML_EXTERN_FUNC(value_exact, const char8_t*);
	TOML_EXTERN_FUNC(value, std::u8string_view);
	TOML_EXTERN_FUNC(value, std::u8string);
	TOML_EXTERN_FUNC(value, const char8_t*);
#endif

#if TOML_ENABLE_WINDOWS_COMPAT
	TOML_EXTERN_FUNC(value_exact, std::wstring);
	TOML_EXTERN_FUNC(value, std::wstring);
#endif

#undef TOML_EXTERN_FUNC
}
TOML_NAMESPACE_END;

/// \endcond
