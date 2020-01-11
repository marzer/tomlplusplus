#pragma once
#include "toml_date_time.h"

namespace toml::impl
{
	// Q: "why does print_to_stream() exist? why not just use ostream::write(), ostream::put() etc?"
	// A: - I'm supporting C++20's char8_t as well; wrapping streams allows switching string modes transparently.
	//    - <charconv> is locale-independent.
	//    - I can avoid forcing users to drag in <sstream> and <iomanip>.

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(std::basic_string_view<CHAR1> str, std::basic_ostream<CHAR2>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(str.data(), str.length());
	}

	template <typename CHAR1, typename CHAR2>
	TOML_ALWAYS_INLINE
	void print_to_stream(const std::basic_string<CHAR1>& str, std::basic_ostream<CHAR2>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR1) == 1);
		static_assert(sizeof(CHAR2) == 1);
		stream.write(str.data(), str.length());
	}

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char character, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GCC_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char* str, size_t len, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#if defined(__cpp_lib_char8_t)

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(char8_t character, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		stream.put(static_cast<CHAR>(character));
	}

	template <typename CHAR>
	TOML_GCC_ATTR(nonnull) TOML_ALWAYS_INLINE
	void print_to_stream(const char8_t* str, size_t len, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW 
	{
		static_assert(sizeof(CHAR) == 1);
		stream.write(reinterpret_cast<const CHAR*>(str), static_cast<std::streamsize>(len));
	}

	#endif

	template <typename T> inline constexpr size_t charconv_buffer_length = 0;
	template <> inline constexpr size_t charconv_buffer_length<double> = 60;
	template <> inline constexpr size_t charconv_buffer_length<float> = 40;
	template <> inline constexpr size_t charconv_buffer_length<uint64_t> = 20; //strlen("18446744073709551615")
	template <> inline constexpr size_t charconv_buffer_length<int64_t> = 20; //strlen("-9223372036854775808")
	template <> inline constexpr size_t charconv_buffer_length<int32_t> = 11; //strlen("-2147483648")
	template <> inline constexpr size_t charconv_buffer_length<int16_t> = 6; //strlen("-32768")
	template <> inline constexpr size_t charconv_buffer_length<int8_t> = 4; //strlen("-128")
	template <> inline constexpr size_t charconv_buffer_length<uint32_t> = 10; //strlen("4294967295")
	template <> inline constexpr size_t charconv_buffer_length<uint16_t> = 5; //strlen("65535")
	template <> inline constexpr size_t charconv_buffer_length<uint8_t> = 3; //strlen("255")

	template <typename T, typename CHAR>
	inline void print_integer_to_stream(T val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	#define TOML_P2S_OVERLOAD(type)																\
		template <typename CHAR>																\
		TOML_ALWAYS_INLINE																		\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW			\
		{																						\
			static_assert(sizeof(CHAR) == 1);													\
			print_integer_to_stream(val, stream);												\
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
	inline void print_floating_point_to_stream(T val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
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
			std::ostringstream oss;
			oss << val;
			const auto str = oss.str();
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#else
		{
			TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
			const auto res = std::to_chars(buf, buf + sizeof(buf), val);
			const auto str = std::string_view{ buf, static_cast<size_t>(res.ptr - buf) };
			print_to_stream(str, stream);
			if (needs_decimal_point(str))
				print_to_stream(".0"sv, stream);
		}
		#endif
	}

	#define TOML_P2S_OVERLOAD(type)																\
		template <typename CHAR>																\
		TOML_ALWAYS_INLINE																		\
		void print_to_stream(type val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW			\
		{																						\
			static_assert(sizeof(CHAR) == 1);													\
			print_floating_point_to_stream(val, stream);										\
		}

	TOML_P2S_OVERLOAD(float)
	TOML_P2S_OVERLOAD(double)

	#undef TOML_P2S_OVERLOAD

	template <typename CHAR>
	TOML_ALWAYS_INLINE
	void print_to_stream(bool& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val ? "true"sv : "false"sv, stream);
	}

	template <typename T, typename CHAR>
	inline void print_to_stream(T val, std::basic_ostream<CHAR>& stream, size_t zero_pad_to_digits) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		TOML_GCC_ATTR(uninitialized) char buf[charconv_buffer_length<T>];
		const auto res = std::to_chars(buf, buf + sizeof(buf), val);
		const auto len = static_cast<size_t>(res.ptr - buf);
		for (size_t i = len; i < zero_pad_to_digits; i++)
			print_to_stream('0', stream);
		print_to_stream(buf, static_cast<size_t>(res.ptr - buf), stream);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::date& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);
		print_to_stream(val.year, stream, 4_sz);
		print_to_stream('-', stream);
		print_to_stream(val.month, stream, 2_sz);
		print_to_stream('-', stream);
		print_to_stream(val.day, stream, 2_sz);
	}

	template <typename CHAR>
	inline void print_to_stream(const toml::time& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
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
	inline void print_to_stream(toml::time_offset val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
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
	inline void print_to_stream(const toml::date_time& val, std::basic_ostream<CHAR>& stream) TOML_MAY_THROW
	{
		static_assert(sizeof(CHAR) == 1);

		print_to_stream(val.date, stream);
		print_to_stream('T', stream);
		print_to_stream(val.time, stream);
		if (val.time_offset)
			print_to_stream(*val.time_offset, stream);
	}
}
