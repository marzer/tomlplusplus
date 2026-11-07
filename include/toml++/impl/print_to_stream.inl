//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

//# {{
#include "preprocessor.hpp"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "print_to_stream.hpp"
#include "source_region.hpp"
#include "date_time.hpp"
#include "toml_formatter.hpp"
#include "value.hpp"
#include "array.hpp"
#include "table.hpp"
TOML_DISABLE_WARNINGS;
#include <ostream>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_FLOAT_CHARCONV
#include <cstdio>
#include <cstdlib>
#endif
TOML_ENABLE_WARNINGS;
#include "header_start.hpp"
TOML_DISABLE_ARITHMETIC_WARNINGS;

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_wide_integer_to_stream(std::ostream & stream, T val, value_flags format, size_t min_digits)
	{
		static constexpr auto value_flags_mask =
			value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;
		format &= value_flags_mask;

		int base = 10;
		if (format != value_flags::none && val > T{})
		{
			switch (format)
			{
				case value_flags::format_as_binary: base = 2; break;
				case value_flags::format_as_octal: base = 8; break;
				case value_flags::format_as_hexadecimal: base = 16; break;
				default: break;
			}
		}

		static constexpr size_t max_padding = 16;
		char buf[max_padding + (sizeof(T) * CHAR_BIT)];

#if TOML_INT_CHARCONV

		char* const first	   = buf + max_padding;
		const char* const last = std::to_chars(first, buf + sizeof(buf), val, base).ptr;
		if (base == 16)
		{
			for (auto c = first; c != last; c++)
				if (*c >= 'a')
					*c -= 32;
		}

#else

		using unsigned_type = std::make_unsigned_t<T>;
		const auto radix	= static_cast<unsigned_type>(base);
		auto magnitude		= static_cast<unsigned_type>(val);
		if (val < T{})
			magnitude = static_cast<unsigned_type>(unsigned_type{} - magnitude);

		char* const last = buf + sizeof(buf);
		char* first		 = last;
		do
		{
			*--first  = "0123456789ABCDEF"[magnitude % radix];
			magnitude = static_cast<unsigned_type>(magnitude / radix);
		}
		while (magnitude);
		if (val < T{})
			*--first = '-';

#endif

		const auto len = static_cast<size_t>(last - first);
		size_t padding = min_digits > len ? min_digits - len : 0u;
		for (; padding > max_padding; padding--)
			stream.put('0');
		std::memset(first - padding, '0', padding);
		impl::print_to_stream(stream, first - padding, len + padding);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_integer_to_stream(std::ostream & stream, T val, value_flags format, size_t min_digits)
	{
		using wide_type = std::conditional_t<std::is_signed_v<T>, long long, unsigned long long>;
		print_wide_integer_to_stream(stream, static_cast<wide_type>(val), format, min_digits);
	}

	TOML_INTERNAL_LINKAGE
	char* write_decimal(char* out, uint_least32_t val, size_t min_digits) noexcept
	{
		char digits[10];
		size_t len = {};
		do
		{
			digits[len++] = static_cast<char>('0' + (val % 10u));
			val /= 10u;
		}
		while (val);
		for (; min_digits > len; min_digits--)
			*out++ = '0';
		while (len)
			*out++ = digits[--len];
		return out;
	}

	TOML_INTERNAL_LINKAGE
	char* write_date(char* out, const toml::date& val) noexcept
	{
		out	   = write_decimal(out, val.year, 4u);
		*out++ = '-';
		out	   = write_decimal(out, val.month, 2u);
		*out++ = '-';
		return write_decimal(out, val.day, 2u);
	}

	TOML_INTERNAL_LINKAGE
	char* write_time(char* out, const toml::time& val) noexcept
	{
		out	   = write_decimal(out, val.hour, 2u);
		*out++ = ':';
		out	   = write_decimal(out, val.minute, 2u);
		*out++ = ':';
		out	   = write_decimal(out, val.second, 2u);
		if (!val.nanosecond || val.nanosecond > 999999999u)
			return out;

		*out++		  = '.';
		auto ns		  = val.nanosecond;
		size_t digits = 9u;
		while (ns % 10u == 0u)
		{
			ns /= 10u;
			digits--;
		}
		return write_decimal(out, ns, digits);
	}

	TOML_INTERNAL_LINKAGE
	char* write_time_offset(char* out, const toml::time_offset& val) noexcept
	{
		if (!val.minutes)
		{
			*out++ = 'Z';
			return out;
		}

		auto mins = static_cast<int>(val.minutes);
		*out++	  = mins < 0 ? '-' : '+';
		if (mins < 0)
			mins = -mins;
		out	   = write_decimal(out, static_cast<uint_least32_t>(mins / 60), 2u);
		*out++ = ':';
		return write_decimal(out, static_cast<uint_least32_t>(mins % 60), 2u);
	}

	struct float_digits
	{
		char digits[17];
		size_t count;
		int exponent;
	};

	// printf takes its decimal point from setlocale(), so non-digits are skipped
	TOML_INTERNAL_LINKAGE
	float_digits read_scientific(const char* text) noexcept
	{
		float_digits result{};
		bool in_exponent	   = {};
		bool negative_exponent = {};
		for (; *text; text++)
		{
			const char c = *text;
			if (c == 'e')
				in_exponent = true;
			else if (c == '-')
				negative_exponent = true;
			else if (c < '0' || c > '9')
				continue;
			else if (in_exponent)
				result.exponent = (result.exponent * 10) + (c - '0');
			else if (result.count < sizeof(result.digits))
				result.digits[result.count++] = c;
		}
		if (negative_exponent)
			result.exponent = -result.exponent;
		return result;
	}

	// with denormals-are-zero set, every subnormal compares equal to zero
	template <typename T>
	TOML_INTERNAL_LINKAGE
	bool same_bits(T a, T b) noexcept
	{
		using bits_type = std::conditional_t<sizeof(T) == sizeof(uint32_t), uint32_t, uint64_t>;
		static_assert(sizeof(T) == sizeof(bits_type));

		bits_type a_bits = {};
		bits_type b_bits = {};
		std::memcpy(&a_bits, &a, sizeof(T));
		std::memcpy(&b_bits, &b, sizeof(T));
		return a_bits == b_bits;
	}

#if TOML_FLOAT_CHARCONV

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits rounded_digits(T val, int precision) noexcept
	{
		char buf[33];
		*std::to_chars(buf, buf + 32, val, std::chars_format::scientific, precision).ptr = '\0';
		return read_scientific(buf);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits shortest_digits(T val) noexcept
	{
		char buf[33];
		*std::to_chars(buf, buf + 32, val, std::chars_format::scientific).ptr = '\0';
		return read_scientific(buf);
	}

#else

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits rounded_digits(T val, int precision) noexcept
	{
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.*e", precision, static_cast<double>(val)); // NOLINT(*-pro-type-vararg)
		return read_scientific(buf);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	T read_back(const float_digits& val) noexcept
	{
		// strtod takes its decimal point from setlocale(), so the digits go in without one
		char text[32];
		std::memcpy(text, val.digits, val.count);
		char* out	  = text + val.count;
		*out++		  = 'e';
		auto exponent = val.exponent - static_cast<int>(val.count - 1u);
		if (exponent < 0)
		{
			*out++	 = '-';
			exponent = -exponent;
		}
		*write_decimal(out, static_cast<uint_least32_t>(exponent), 1u) = '\0';

		if constexpr (std::is_same_v<T, float>)
			return std::strtof(text, nullptr);
		else
			return std::strtod(text, nullptr);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	float_digits shortest_digits(T val) noexcept
	{
		using limits = std::numeric_limits<T>;

		for (int precision = val < (limits::min)() ? 0 : limits::digits10 - 1;; precision++)
		{
			auto result = rounded_digits(val, precision);
			if (precision + 1 >= limits::max_digits10)
				return result;

			const auto back = read_back<T>(result);
			if (same_bits(back, val))
				return result;

			// the gap below a power of two is half the one above it, so past digits10 the nearest candidate can
			// fall outside while the next one up reads back
			if (precision >= limits::digits10 && back < val)
			{
				size_t i = result.count;
				while (i && result.digits[i - 1u] == '9')
					result.digits[--i] = '0';
				if (i)
					result.digits[i - 1u]++;
				else
				{
					result.digits[0] = '1';
					result.exponent++;
				}
				if (same_bits(read_back<T>(result), val))
					return result;
			}
		}
	}

#endif

	// python's repr() layout, or %g's when relaxed
	template <typename T, size_t N>
	TOML_INTERNAL_LINKAGE
	char* write_float(char (&buf)[N], T val, bool relaxed_precision, bool point_before_exponent) noexcept
	{
		static_assert(N >= 32u);

		char* out = buf;
		if (impl::signbit(val))
		{
			*out++ = '-';
			val	   = -val;
		}
		if (same_bits(val, T{}))
		{
			*out++ = '0';
			*out++ = '.';
			*out++ = '0';
			return out;
		}

#if TOML_FLOAT_CHARCONV
		// fixed form keeps integer digits the shortest text drops once the gap between values passes 2,
		// which happens below 1e16 for floats
		if (std::is_same_v<T, double> && !relaxed_precision)
		{
			// the shortest text reads back, so comparing the value with the bounds matches comparing the text
			const bool fixed  = val >= 1e-4 && val < 1e16;
			char* const first = out;
			out = std::to_chars(first, buf + N, val, fixed ? std::chars_format::fixed : std::chars_format::scientific)
					  .ptr;
			if (fixed)
			{
				if (!std::memchr(first, '.', static_cast<size_t>(out - first)))
				{
					*out++ = '.';
					*out++ = '0';
				}
			}
			else if (point_before_exponent && first[1] == 'e')
			{
				std::memmove(first + 3, first + 1, static_cast<size_t>(out - first - 1));
				first[1] = '.';
				first[2] = '0';
				out += 2;
			}
			return out;
		}
#endif

#if !TOML_FLOAT_CHARCONV
		[[maybe_unused]] const impl::default_fenv fenv;
#endif
		auto digits = shortest_digits(val);
		if (relaxed_precision && digits.count > 6u)
			digits = rounded_digits(val, 5);
		while (digits.count > 1u && digits.digits[digits.count - 1u] == '0')
			digits.count--;

		if (digits.exponent < -4 || digits.exponent >= (relaxed_precision ? 6 : 16))
		{
			*out++ = digits.digits[0];
			if (digits.count > 1u)
			{
				*out++ = '.';
				std::memcpy(out, digits.digits + 1, digits.count - 1u);
				out += digits.count - 1u;
			}
			else if (point_before_exponent)
			{
				*out++ = '.';
				*out++ = '0';
			}
			*out++ = 'e';
			*out++ = digits.exponent < 0 ? '-' : '+';
			return write_decimal(out,
								 static_cast<uint_least32_t>(digits.exponent < 0 ? -digits.exponent : digits.exponent),
								 2u);
		}

		if (digits.exponent < 0)
		{
			*out++ = '0';
			*out++ = '.';
			for (int i = digits.exponent + 1; i < 0; i++)
				*out++ = '0';
			std::memcpy(out, digits.digits, digits.count);
			return out + digits.count;
		}

		const auto whole = static_cast<size_t>(digits.exponent) + 1u;
		for (size_t i = 0; i < whole; i++)
			*out++ = i < digits.count ? digits.digits[i] : '0';
		*out++ = '.';
		if (whole >= digits.count)
		{
			*out++ = '0';
			return out;
		}
		std::memcpy(out, digits.digits + whole, digits.count - whole);
		return out + (digits.count - whole);
	}

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_floating_point_to_stream(std::ostream & stream, T val, bool relaxed_precision)
	{
		switch (impl::fpclassify(val))
		{
			case impl::fp_class::neg_inf: impl::print_to_stream(stream, "-inf"sv); break;

			case impl::fp_class::pos_inf: impl::print_to_stream(stream, "inf"sv); break;

			case impl::fp_class::nan: impl::print_to_stream(stream, "nan"sv); break;

			case impl::fp_class::ok:
			{
				char buf[32];
				const auto end = write_float(buf, val, relaxed_precision, false);
				impl::print_to_stream(stream, buf, static_cast<size_t>(end - buf));
			}
			break;

			default: TOML_UNREACHABLE;
		}
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(nonnull)
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const char* val, size_t len)
	{
		stream.write(val, static_cast<std::streamsize>(len));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, std::string_view val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const std::string& val)
	{
		stream.write(val.data(), static_cast<std::streamsize>(val.length()));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, char val)
	{
		stream.put(val);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed char val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed short val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed int val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, signed long val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream,
									   signed long long val,
									   value_flags format,
									   size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned char val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned short val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned int val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, unsigned long val, value_flags format, size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream,
									   unsigned long long val,
									   value_flags format,
									   size_t min_digits)
	{
		TOML_ANON_NAMESPACE::print_integer_to_stream(stream, val, format, min_digits);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, float val, value_flags, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, double val, value_flags, bool relaxed_precision)
	{
		TOML_ANON_NAMESPACE::print_floating_point_to_stream(stream, val, relaxed_precision);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, bool val)
	{
		print_to_stream(stream, val ? "true"sv : "false"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::date& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_date(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::time& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_time(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::time_offset& val)
	{
		char buf[64];
		print_to_stream(stream, buf, static_cast<size_t>(TOML_ANON_NAMESPACE::write_time_offset(buf, val) - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const toml::date_time& val)
	{
		char buf[64];
		auto end = TOML_ANON_NAMESPACE::write_date(buf, val.date);
		*end++	 = 'T';
		end		 = TOML_ANON_NAMESPACE::write_time(end, val.time);
		if (val.offset)
			end = TOML_ANON_NAMESPACE::write_time_offset(end, *val.offset);
		print_to_stream(stream, buf, static_cast<size_t>(end - buf));
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const source_position& val)
	{
		print_to_stream(stream, "line "sv);
		print_to_stream(stream, val.line);
		print_to_stream(stream, ", column "sv);
		print_to_stream(stream, val.column);
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const source_region& val)
	{
		print_to_stream(stream, val.begin);
		if (val.begin != val.end)
		{
			print_to_stream(stream, " to "sv);
			print_to_stream(stream, val.end);
		}
		if (val.path)
		{
			print_to_stream(stream, " of '"sv);
			print_to_stream(stream, *val.path);
			stream.put('\'');
		}
	}

#if TOML_ENABLE_FORMATTERS

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const array& arr)
	{
		stream << toml_formatter{ arr };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const table& tbl)
	{
		stream << toml_formatter{ tbl };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<std::string>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<int64_t>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<double>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<bool>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<date>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<time>& val)
	{
		stream << toml_formatter{ val };
	}

	TOML_EXTERNAL_LINKAGE
	void TOML_CALLCONV print_to_stream(std::ostream & stream, const value<date_time>& val)
	{
		stream << toml_formatter{ val };
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.hpp"
