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
#include <sstream>
#endif
TOML_ENABLE_WARNINGS;
#include "header_start.hpp"

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	inline constexpr size_t charconv_buffer_length = 0;

	template <>
	inline constexpr size_t charconv_buffer_length<int8_t> = 4; // strlen("-128")

	template <>
	inline constexpr size_t charconv_buffer_length<int16_t> = 6; // strlen("-32768")

	template <>
	inline constexpr size_t charconv_buffer_length<int32_t> = 11; // strlen("-2147483648")

	template <>
	inline constexpr size_t charconv_buffer_length<int64_t> = 20; // strlen("-9223372036854775808")

	template <>
	inline constexpr size_t charconv_buffer_length<uint8_t> = 3; // strlen("255")

	template <>
	inline constexpr size_t charconv_buffer_length<uint16_t> = 5; // strlen("65535")

	template <>
	inline constexpr size_t charconv_buffer_length<uint32_t> = 10; // strlen("4294967295")

	template <>
	inline constexpr size_t charconv_buffer_length<uint64_t> = 20; // strlen("18446744073709551615")

	template <>
	inline constexpr size_t charconv_buffer_length<float> = 64;

	template <>
	inline constexpr size_t charconv_buffer_length<double> = 64;

	template <typename T>
	TOML_INTERNAL_LINKAGE
	void print_integer_to_stream(std::ostream & stream, T val, value_flags format = {}, size_t min_digits = 0)
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
	void print_floating_point_to_stream(std::ostream & stream, T val, [[maybe_unused]] bool relaxed_precision)
	{
		switch (impl::fpclassify(val))
		{
			case impl::fp_class::neg_inf: impl::print_to_stream(stream, "-inf"sv); break;

			case impl::fp_class::pos_inf: impl::print_to_stream(stream, "inf"sv); break;

			case impl::fp_class::nan: impl::print_to_stream(stream, "nan"sv); break;

			case impl::fp_class::ok:
			{
				static constexpr auto needs_decimal_point = [](auto&& s) noexcept
				{
					for (auto c : s)
						if (c == '.' || c == 'E' || c == 'e')
							return false;
					return true;
				};

#if TOML_FLOAT_CHARCONV

				char buf[charconv_buffer_length<T>];
				auto res = std::to_chars(buf, buf + sizeof(buf), val);
				auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };

				char buf2[charconv_buffer_length<T>];
				if (relaxed_precision)
				{
					res				= std::to_chars(buf2, buf2 + sizeof(buf2), val, std::chars_format::general, 6);
					const auto str2 = std::string_view{ buf2, static_cast<size_t>(res.ptr - buf2) };
					if (str2.length() < str.length())
						str = str2;
				}

				impl::print_to_stream(stream, str);
				if (needs_decimal_point(str))
					toml::impl::print_to_stream(stream, ".0"sv);

#else

				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				if (!relaxed_precision)
					ss.precision(std::numeric_limits<T>::max_digits10);
				ss << val;
				const auto str = std::move(ss).str();
				impl::print_to_stream(stream, str);
				if (needs_decimal_point(str))
					impl::print_to_stream(stream, ".0"sv);

#endif
			}
			break;

			default: TOML_UNREACHABLE;
		}
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
