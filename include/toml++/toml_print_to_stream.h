//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_date_time.h"

namespace toml::impl
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm supporting C++20's char8_t as well; wrapping streams allows switching string modes transparently.
	//    - I'm using <charconv> to format numerics. Faster and locale-independent.
	//    - I can avoid forcing users to drag in <sstream> and <iomanip>.

	// Q: "there's a lot of reinterpret_casting here, is any of it UB?"
	// A: - If the source string data is char and the output string is char8_t, then technically yes,
	//      but not in the other direction. I test in both modes on Clang, GCC and MSVC and have yet to
	//      see it actually causing an issue, but in the event it does present a problem it's not going to
	//      be a show-stopper since all it means is I need to do duplicate some code.
	//    - Strings in C++. Honestly.

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(std::basic_string_view<CHAR1> str, std::basic_ostream<CHAR2>& stream)
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(const std::basic_string<CHAR1>& str, std::basic_ostream<CHAR2>& stream)
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(reinterpret_cast<const CHAR2*>(str.data()), static_cast<std::streamsize>(str.length()));
	}

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char character, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GNU_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char* str, size_t len, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#if defined(__cpp_lib_char8_t)

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char8_t character, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GNU_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char8_t* str, size_t len, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#endif

	template <typename T> inline constexpr size_t charconv_buffer_length = 0;
	template <> inline constexpr size_t charconv_buffer_length<double> = 60;
	template <> inline constexpr size_t charconv_buffer_length<float> = 40;
	template <> inline constexpr size_t charconv_buffer_length<uint64_t> = 20; // strlen("18446744073709551615")
	template <> inline constexpr size_t charconv_buffer_length<int64_t> = 20;  // strlen("-9223372036854775808")
	template <> inline constexpr size_t charconv_buffer_length<int32_t> = 11;  // strlen("-2147483648")
	template <> inline constexpr size_t charconv_buffer_length<int16_t> = 6;   // strlen("-32768")
	template <> inline constexpr size_t charconv_buffer_length<int8_t> = 4;    // strlen("-128")
	template <> inline constexpr size_t charconv_buffer_length<uint32_t> = 10; // strlen("4294967295")
	template <> inline constexpr size_t charconv_buffer_length<uint16_t> = 5;  // strlen("65535")
	template <> inline constexpr size_t charconv_buffer_length<uint8_t> = 3;   // strlen("255")

	template <typename T, typename CHAR>
	inline void print_integer_to_stream(T val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	#define TOML_P2S_OVERLOAD(type)											\
		template <typename CHAR>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream)	\
		{																	\
			static_assert(sizeof(CHAR) == 1);								\
			print_integer_to_stream(val, stream);							\
		}

	TOML_P2S_OVERLOAD(int8_t)
	TOML_P2S_OVERLOAD(int16_t)
	TOML_P2S_OVERLOAD(int32_t)
	TOML_P2S_OVERLOAD(int64_t)
	TOML_P2S_OVERLOAD(uint8_t)
	TOML_P2S_OVERLOAD(uint16_t)
	TOML_P2S_OVERLOAD(uint32_t)
	TOML_P2S_OVERLOAD(uint64_t)

	#undef TOML_P2S_OVERLOAD

	template <typename T, typename CHAR>
	inline void print_floating_point_to_stream(T val, std::basic_ostream<CHAR>& stream, bool hexfloat = false)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		static constexpr auto needs_decimal_point = [](auto&& s) noexcept
		{
			for (auto c : s)
				if (c == '.' || c == 'E' || c == 'e')
					return false;
			return true;
		};

		#if TOML_USE_STREAMS_FOR_FLOATS
		{
			std::ostringstream ss;
			ss.precision(std::numeric_limits<T>::digits10 + 1);
			if (hexfloat)
				ss << std::hexfloat;
			ss << val;
			const auto str = std::move(ss).str();
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#else
		{
			char buf[charconv_buffer_length<T>];
			const auto res = hexfloat
				? std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::hex)
				: std::to_chars(buf, buf + sizeof(buf), val);
			const auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#endif
	}

	#define TOML_P2S_OVERLOAD(type)											\
		template <typename CHAR>											\
		TOML_ALWAYS_INLINE													\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream)	\
		{																	\
			static_assert(sizeof(CHAR) == 1);								\
			print_floating_point_to_stream(val, stream);					\
		}

	TOML_P2S_OVERLOAD(float)
	TOML_P2S_OVERLOAD(double)

	#undef TOML_P2S_OVERLOAD

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(bool val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val ? "true"sv : "false"sv, stream);
	}

	template <typename T, typename CHAR>
	inline void print_to_stream(T val, std::basic_ostream<CHAR>& stream, size_t zero_pad_to_digits)
	{
		static_assert(sizeof(CHAR) == 1);
		char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		const auto len = static_cast<size_t>(res.ptr - buf);
		for (size_t i = len; i < zero_pad_to_digits; i++)
			print_to_stream('0', stream);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date& val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.year, stream, 4_sz);
		print_to_stream('-', stream);
		print_to_stream(val.month, stream, 2_sz);
		print_to_stream('-', stream);
		print_to_stream(val.day, stream, 2_sz);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::time& val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.hour, stream, 2_sz);
		print_to_stream(':', stream);
		print_to_stream(val.minute, stream, 2_sz);
		print_to_stream(':', stream);
		print_to_stream(val.second, stream, 2_sz);
		if (val.nanosecond && val.nanosecond <= 999999999u)
		{
			print_to_stream('.', stream);
			auto ns = val.nanosecond;
			size_t digits = 9_sz;
			while (ns % 10u == 0u)
			{
				ns /= 10u;
				digits--;
			}
			print_to_stream(ns, stream, digits);
		}
	}

	template <typename CHAR>
	inline void print_to_stream(toml::time_offset val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		if (!val.minutes)
			print_to_stream('Z', stream);
		else
		{
			auto mins = static_cast<int>(val.minutes);
			if (mins < 0)
			{
				print_to_stream('-', stream);
				mins = -mins;
			}
			else
				print_to_stream('+', stream);
			const auto hours = mins / 60;
			if (hours)
			{
				print_to_stream(static_cast<unsigned int>(hours), stream, 2_sz);
				mins -= hours * 60;
			}
			else
				print_to_stream("00"sv, stream);
			print_to_stream(':', stream);
			print_to_stream(static_cast<unsigned int>(mins), stream, 2_sz);
		}
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date_time& val, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.date, stream);
		print_to_stream('T', stream);
		print_to_stream(val.time, stream);
		if (val.time_offset)
			print_to_stream(*val.time_offset, stream);
	}

	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	template <typename T, typename CHAR>
	void print_to_stream_with_escapes(T && str, std::basic_ostream<CHAR>& stream)
	{
		static_assert(sizeof(CHAR) == 1);
		for (auto c : str)
		{
			if (c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F')) TOML_UNLIKELY
				print_to_stream(low_character_escape_table[c], stream);
			else if (c == TOML_STRING_PREFIX('\x7F')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\u007F"sv), stream);
			else if (c == TOML_STRING_PREFIX('"')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\\""sv), stream);
			else if (c == TOML_STRING_PREFIX('\\')) TOML_UNLIKELY
				print_to_stream(TOML_STRING_PREFIX("\\\\"sv), stream);
			else
				print_to_stream(c, stream);
		}
	}

	TOML_POP_WARNINGS
}


namespace toml
{
	/// \brief	Prints a source_position to a stream.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse("bar = 42"sv);
	/// 
	/// std::cout << "The value for 'bar' was found on "sv
	///		<< tbl.get("bar")->source()
	///		<< std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// The value for 'bar' was found on line 1, column 7
	/// \eout
	/// 
	/// \tparam CHAR The output stream's underlying character type. Must be 1 byte in size.
	/// \param 	lhs	The stream.
	/// \param 	rhs	The source_position.
	///
	/// \returns	The input stream.
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_position& rhs)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);
		impl::print_to_stream("line "sv, lhs);
		impl::print_to_stream(rhs.line, lhs);
		impl::print_to_stream(", column "sv, lhs);
		impl::print_to_stream(rhs.column, lhs);
		return lhs;
	}

	/// \brief	Prints a source_region to a stream.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse("bar = 42", "config.toml");
	/// 
	/// std::cout << "The value for 'bar' was found on "sv
	///		<< tbl.get("bar")->source()
	///		<< std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// The value for 'bar' was found on line 1, column 7 of 'config.toml'
	/// \eout
	/// 
	/// \tparam CHAR The output stream's underlying character type. Must be 1 byte in size.
	/// \param 	lhs	The stream.
	/// \param 	rhs	The source_position.
	///
	/// \returns	The input stream.
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const source_region& rhs)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);
		lhs << rhs.begin;
		if (rhs.path)
		{
			impl::print_to_stream(" of '"sv, lhs);
			impl::print_to_stream(*rhs.path, lhs);
			impl::print_to_stream('\'', lhs);
		}
		return lhs;
	}

	/// \brief	Prints a parse_error to a stream.
	///
	/// \detail \cpp
	/// try
	/// {
	/// 	auto tbl = toml::parse("enabled = trUe"sv);
	/// }
	/// catch (const toml::parse_error & err)
	/// {
	/// 	std::cerr << "Parsing failed:\n"sv << err << std::endl;
	/// }
	/// \ecpp
	/// 
	/// \out
	/// Parsing failed:
	/// Encountered unexpected character while parsing boolean; expected 'true', saw 'trU'
	///		(error occurred at line 1, column 13)
	/// \eout
	/// 
	/// \tparam CHAR The output stream's underlying character type. Must be 1 byte in size.
	/// \param 	lhs	The stream.
	/// \param 	rhs	The parse_error.
	///
	/// \returns	The input stream.
	template <typename CHAR>
	std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const parse_error& rhs)
	{
		impl::print_to_stream(rhs.description(), lhs);
		impl::print_to_stream("\n\t(error occurred at "sv, lhs);
		lhs << rhs.source();
		impl::print_to_stream(")"sv, lhs);
		return lhs;
	}
}

