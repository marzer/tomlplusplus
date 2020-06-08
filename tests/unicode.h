#pragma once
#include "tests.h"

using func_type = bool(char32_t)noexcept;
inline constexpr func_type* funcs[] =
{
	// these must be mutually-exclusive

	impl::is_ascii_letter,
	impl::is_ascii_whitespace,
	impl::is_ascii_line_break<true>,
	impl::is_decimal_digit,
	impl::is_string_delimiter,
	impl::is_unicode_whitespace,
	impl::is_unicode_line_break,
	impl::is_unicode_surrogate,
	#if TOML_LANG_UNRELEASED
	impl::is_unicode_letter,
	impl::is_unicode_number,
	impl::is_unicode_combining_mark,
	#endif
};

template <typename T>
inline bool in_only(func_type* fptr, T cp) noexcept
{
	if (!fptr(static_cast<char32_t>(cp)))
		return false;
	for (auto fn : funcs)
	{
		if (fn == fptr)
			continue;
		if (fn(static_cast<char32_t>(cp)))
			return false;
	}
	return true;
}

inline constexpr uint32_t unimax = 0x10FFFFu;

struct codepoint_range
{
	char32_t first;
	char32_t last;

	template <typename T, typename U>
	TOML_NODISCARD_CTOR
	constexpr codepoint_range(T first_, U last_) noexcept
		: first{ static_cast<char32_t>(first_) },
		last{ static_cast<char32_t>(last_) }
	{
		if (last < first)
			std::swap(first, last);
	}

	template <typename T>
	TOML_NODISCARD_CTOR
	constexpr codepoint_range(T first_) noexcept
		: first{ static_cast<char32_t>(first_) },
		last{ first }
	{}
};

inline bool in(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (!fptr(cp))
			return false;
	return true;
}

inline bool in_only(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (!in_only(fptr, cp))
			return false;
	return true;
}

inline bool not_in(func_type* fptr, codepoint_range range) noexcept
{
	for (auto cp = range.first; cp <= range.last; cp++)
		if (fptr(cp))
			return false;
	return true;
}
