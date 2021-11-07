//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
//# {{
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}
#if TOML_ENABLE_PARSER

#include "std_optional.h"
#include "parser.h"
#include "source_region.h"
#include "parse_error.h"
#include "utf8.h"
#include "date_time.h"
#include "value.h"
#include "array.h"
#include "table.h"
TOML_DISABLE_WARNINGS;
#include <istream>
#include <fstream>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
#include <sstream>
#endif
#if !TOML_INT_CHARCONV
#include <iomanip>
#endif
TOML_ENABLE_WARNINGS;
#include "simd.h"
#include "header_start.h"

//#---------------------------------------------------------------------------------------------------------------------
//# UTF8 STREAMS
//#---------------------------------------------------------------------------------------------------------------------

#ifdef NDEBUG
#define assert_or_assume(cond) TOML_ASSUME(cond)
#else
#define assert_or_assume(cond) TOML_ASSERT(cond)
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename T>
	TOML_PURE_GETTER
	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	bool is_ascii(const T* str, size_t size) noexcept
	{
		static_assert(sizeof(T) == 1);

		const T* const end = str + size;

#if TOML_HAS_SSE2 && (128 % CHAR_BIT) == 0
		{
			constexpr size_t chars_per_vector = 128 / CHAR_BIT;

			if (const size_t simdable = size - (size % chars_per_vector))
			{
				__m128i mask = _mm_setzero_si128();
				for (const T* const e = str + simdable; str < e; str += chars_per_vector)
				{
					const __m128i current_bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str));
					mask						= _mm_or_si128(mask, current_bytes);
				}
				const __m128i has_error = _mm_cmpgt_epi8(_mm_setzero_si128(), mask);

#if TOML_HAS_SSE4_1
				if (!_mm_testz_si128(has_error, has_error))
					return false;
#else
				if (_mm_movemask_epi8(_mm_cmpeq_epi8(has_error, _mm_setzero_si128())) != 0xFFFF)
					return false;
#endif
			}
		}
#endif

		for (; str < end; str++)
			if (static_cast<unsigned char>(*str) > 127u)
				return false;

		return true;
	}

	template <typename T>
	class utf8_byte_stream;

	TOML_INTERNAL_LINKAGE
	constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	template <typename Char>
	class utf8_byte_stream<std::basic_string_view<Char>>
	{
		static_assert(sizeof(Char) == 1);

	  private:
		std::basic_string_view<Char> source_;
		size_t position_ = {};

	  public:
		TOML_NODISCARD_CTOR
		explicit constexpr utf8_byte_stream(std::basic_string_view<Char> sv) noexcept //
			: source_{ sv }
		{
			// trim trailing nulls
			const size_t initial_len = source_.length();
			size_t actual_len		 = initial_len;
			for (size_t i = actual_len; i-- > 0u;)
			{
				if (source_[i] != Char{}) // not '\0'
				{
					actual_len = i + 1u;
					break;
				}
			}
			if (initial_len != actual_len)
				source_ = source_.substr(0u, actual_len);

			// skip bom
			if (actual_len >= 3u && memcmp(utf8_byte_order_mark.data(), source_.data(), 3u) == 0)
				position_ += 3u;
		}

		TOML_CONST_INLINE_GETTER
		constexpr bool error() const noexcept
		{
			return false;
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool eof() const noexcept
		{
			return position_ >= source_.length();
		}

		TOML_PURE_INLINE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return !eof();
		}

		TOML_PURE_INLINE_GETTER
		constexpr bool peek_eof() const noexcept
		{
			return eof();
		}

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(void* dest, size_t num) noexcept
		{
			assert_or_assume(!eof());

			num = impl::min(position_ + num, source_.length()) - position_;
			std::memcpy(dest, source_.data() + position_, num);
			position_ += num;
			return num;
		}
	};

	template <>
	class utf8_byte_stream<std::istream>
	{
	  private:
		std::istream* source_;

	  public:
		TOML_NODISCARD_CTOR
		explicit utf8_byte_stream(std::istream& stream) noexcept(!TOML_COMPILER_EXCEPTIONS) //
			: source_{ &stream }
		{
			if (!*this) // eof, bad
				return;

			const auto initial_pos = source_->tellg();
			char bom[3];
			source_->read(bom, 3);
			if (source_->bad() || (source_->gcount() == 3 && memcmp(utf8_byte_order_mark.data(), bom, 3u) == 0))
				return;

			source_->clear();
			source_->seekg(initial_pos, std::istream::beg);
		}

		TOML_PURE_INLINE_GETTER
		bool error() const noexcept
		{
			return !!(source_->rdstate() & std::istream::badbit);
		}

		TOML_PURE_INLINE_GETTER
		bool eof() const noexcept
		{
			return !!(source_->rdstate() & std::istream::eofbit);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return !(source_->rdstate() & (std::istream::badbit | std::istream::eofbit));
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			return eof() || source_->peek() == std::istream::traits_type::eof();
		}

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(void* dest, size_t num) noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			TOML_ASSERT(*this);

			source_->read(static_cast<char*>(dest), static_cast<std::streamsize>(num));
			return static_cast<size_t>(source_->gcount());
		}
	};

	struct utf8_codepoint
	{
		char32_t value;
		char bytes[4];
		size_t count;
		source_position position;

		TOML_PURE_INLINE_GETTER
		constexpr operator const char32_t&() const noexcept
		{
			return value;
		}

		TOML_PURE_INLINE_GETTER
		constexpr const char32_t& operator*() const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	struct TOML_ABSTRACT_BASE utf8_reader_interface
	{
		TOML_NODISCARD
		virtual const source_path_ptr& source_path() const noexcept = 0;

		TOML_NODISCARD
		virtual const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS) = 0;

		TOML_NODISCARD
		virtual bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS) = 0;

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		virtual optional<parse_error>&& error() noexcept = 0;

#endif

		virtual ~utf8_reader_interface() noexcept = default;
	};

#if TOML_EXCEPTIONS
#define utf8_reader_error(...)				throw parse_error(__VA_ARGS__)
#define utf8_reader_return_after_error(...) (void)0
#define utf8_reader_error_check(...)		(void)0
#else
#define utf8_reader_error(...)				err_.emplace(__VA_ARGS__)
#define utf8_reader_return_after_error(...) return __VA_ARGS__
#define utf8_reader_error_check(...)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(err_)                                                                                         \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#endif

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final : public utf8_reader_interface
	{
	  private:
		static constexpr size_t block_capacity = 32;
		utf8_byte_stream<T> stream_;
		source_position next_pos_ = { 1, 1 };

		impl::utf8_decoder decoder_;
		struct currently_decoding_t
		{
			char bytes[4];
			size_t count;
		} currently_decoding_;

		struct codepoints_t
		{
			alignas(32) utf8_codepoint buffer[block_capacity];
			size_t current;
			size_t count;
		} codepoints_;

		source_path_ptr source_path_;

#if !TOML_EXCEPTIONS
		optional<parse_error> err_;
#endif

		bool read_next_block() noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			TOML_ASSERT(stream_);

			alignas(32) char raw_bytes[block_capacity];
			size_t raw_bytes_read;

			// read the next raw (encoded) block in from the stream
			if constexpr (noexcept(stream_(raw_bytes, block_capacity)) || !TOML_EXCEPTIONS)
			{
				raw_bytes_read = stream_(raw_bytes, block_capacity);
			}
#if TOML_EXCEPTIONS
			else
			{
				try
				{
					raw_bytes_read = stream_(raw_bytes, block_capacity);
				}
				catch (const std::exception& exc)
				{
					throw parse_error{ exc.what(), next_pos_, source_path_ };
				}
				catch (...)
				{
					throw parse_error{ "An unspecified error occurred", next_pos_, source_path_ };
				}
			}
#endif // TOML_EXCEPTIONS

			// handle a zero-byte read
			if TOML_UNLIKELY(!raw_bytes_read)
			{
				if (stream_.eof())
				{
					// EOF only sets the error state if the decoder wants more input, otherwise
					// a zero-byte read might have just caused the underlying stream to realize it's exhaused and set
					// the EOF flag, and that's totally fine
					if (decoder_.needs_more_input())
						utf8_reader_error("Encountered EOF during incomplete utf-8 code point sequence",
										  next_pos_,
										  source_path_);
				}
				else
				{
					utf8_reader_error("Reading from the underlying stream failed - zero bytes read",
									  next_pos_,
									  source_path_);
				}
				return false;
			}

			assert_or_assume(raw_bytes_read);
			std::memset(&codepoints_, 0, sizeof(codepoints_));

			// helper for calculating decoded codepoint line+cols
			const auto calc_positions = [&]() noexcept
			{
				for (size_t i = 0; i < codepoints_.count; i++)
				{
					auto& cp	= codepoints_.buffer[i];
					cp.position = next_pos_;

					if (impl::is_vertical_whitespace_excl_cr(cp))
					{
						next_pos_.line++;
						next_pos_.column = source_index{ 1 };
					}
					else
						next_pos_.column++;
				}
			};

			// decide whether we need to use the UTF-8 decoder or if we can treat this block as plain ASCII
			const auto ascii_fast_path = !decoder_.needs_more_input() && is_ascii(raw_bytes, raw_bytes_read);

			// ASCII fast-path
			if (ascii_fast_path)
			{
				decoder_.reset();
				currently_decoding_.count = {};

				codepoints_.count = raw_bytes_read;
				for (size_t i = 0; i < codepoints_.count; i++)
				{
					auto& cp	= codepoints_.buffer[i];
					cp.value	= static_cast<char32_t>(raw_bytes[i]);
					cp.bytes[0] = raw_bytes[i];
					cp.count	= 1u;
				}
			}

			// UTF-8 slow-path
			else
			{
				// helper for getting precise error location
				const auto error_pos = [&]() noexcept -> const source_position&
				{ //
					return codepoints_.count ? codepoints_.buffer[codepoints_.count - 1u].position : next_pos_;
				};

				for (size_t i = 0; i < raw_bytes_read; i++)
				{
					decoder_(static_cast<uint8_t>(raw_bytes[i]));
					if TOML_UNLIKELY(decoder_.error())
					{
						calc_positions();
						utf8_reader_error("Encountered invalid utf-8 sequence", error_pos(), source_path_);
						utf8_reader_return_after_error(false);
					}

					currently_decoding_.bytes[currently_decoding_.count++] = raw_bytes[i];

					if (decoder_.has_code_point())
					{
						auto& cp = codepoints_.buffer[codepoints_.count++];

						cp.value = decoder_.codepoint;
						cp.count = currently_decoding_.count;
						std::memcpy(cp.bytes, currently_decoding_.bytes, currently_decoding_.count);
						currently_decoding_.count = {};
					}
					else if TOML_UNLIKELY(currently_decoding_.count == 4u)
					{
						calc_positions();
						utf8_reader_error("Encountered overlong utf-8 sequence", error_pos(), source_path_);
						utf8_reader_return_after_error(false);
					}
				}
				if TOML_UNLIKELY(decoder_.needs_more_input() && stream_.eof())
				{
					calc_positions();
					utf8_reader_error("Encountered EOF during incomplete utf-8 code point sequence",
									  error_pos(),
									  source_path_);
					utf8_reader_return_after_error(false);
				}
			}

			assert_or_assume(codepoints_.count);
			calc_positions();

			// handle general I/O errors
			// (down here so the next_pos_ benefits from calc_positions())
			if TOML_UNLIKELY(stream_.error())
			{
				utf8_reader_error("An I/O error occurred while reading from the underlying stream",
								  next_pos_,
								  source_path_);
				utf8_reader_return_after_error(false);
			}

			return true;
		}

	  public:
		template <typename U, typename String = std::string_view>
		TOML_NODISCARD_CTOR
		explicit utf8_reader(U&& source, String&& source_path = {}) noexcept(
			std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
			: stream_{ static_cast<U&&>(source) }
		{
			currently_decoding_.count = {};

			codepoints_.current = {};
			codepoints_.count	= {};

			if (!source_path.empty())
				source_path_ = std::make_shared<const std::string>(static_cast<String&&>(source_path));
		}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept final
		{
			return source_path_;
		}

		TOML_NODISCARD
		const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS) final
		{
			utf8_reader_error_check({});

			if (codepoints_.current == codepoints_.count)
			{
				if TOML_UNLIKELY(!stream_ || !read_next_block())
					return nullptr;

				assert_or_assume(!codepoints_.current);
			}
			assert_or_assume(codepoints_.count);
			assert_or_assume(codepoints_.count <= block_capacity);
			assert_or_assume(codepoints_.current < codepoints_.count);

			return &codepoints_.buffer[codepoints_.current++];
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS) final
		{
			return stream_.peek_eof();
		}

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		optional<parse_error>&& error() noexcept final
		{
			return std::move(err_);
		}

#endif
	};

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string_view) -> utf8_reader<std::basic_string_view<Char>>;
	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string &&) -> utf8_reader<std::basic_string_view<Char>>;
	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string_view) -> utf8_reader<std::basic_istream<Char>>;
	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string &&) -> utf8_reader<std::basic_istream<Char>>;

#if TOML_EXCEPTIONS
#define utf8_buffered_reader_error_check(...) (void)0
#else
#define utf8_buffered_reader_error_check(...)                                                                          \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(reader_.error())                                                                              \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#endif

	class TOML_EMPTY_BASES utf8_buffered_reader
	{
	  public:
		static constexpr size_t max_history_length = 72;

	  private:
		static constexpr size_t history_buffer_size = max_history_length - 1; //'head' is stored in the reader
		utf8_reader_interface& reader_;
		struct
		{
			utf8_codepoint buffer[history_buffer_size];
			size_t count, first;
		} history_					= {};
		const utf8_codepoint* head_ = {};
		size_t negative_offset_		= {};

	  public:
		TOML_NODISCARD_CTOR
		explicit utf8_buffered_reader(utf8_reader_interface& reader) noexcept //
			: reader_{ reader }
		{}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept
		{
			return reader_.source_path();
		}

		TOML_NODISCARD
		const utf8_codepoint* read_next() noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			utf8_buffered_reader_error_check({});

			if (negative_offset_)
			{
				negative_offset_--;

				// an entry negative offset of 1 just means "replay the current head"
				if (!negative_offset_)
					return head_;

				// otherwise step back into the history buffer
				else
					return history_.buffer
						 + ((history_.first + history_.count - negative_offset_) % history_buffer_size);
			}
			else
			{
				// first character read from stream
				if TOML_UNLIKELY(!history_.count && !head_)
					head_ = reader_.read_next();

				// subsequent characters and not eof
				else if (head_)
				{
					if TOML_UNLIKELY(history_.count < history_buffer_size)
						history_.buffer[history_.count++] = *head_;
					else
						history_.buffer[(history_.first++ + history_buffer_size) % history_buffer_size] = *head_;

					head_ = reader_.read_next();
				}

				return head_;
			}
		}

		TOML_NODISCARD
		const utf8_codepoint* step_back(size_t count) noexcept
		{
			utf8_buffered_reader_error_check({});

			assert_or_assume(history_.count);
			assert_or_assume(negative_offset_ + count <= history_.count);

			negative_offset_ += count;

			return negative_offset_
					 ? history_.buffer + ((history_.first + history_.count - negative_offset_) % history_buffer_size)
					 : head_;
		}

		TOML_NODISCARD
		bool peek_eof() const noexcept(!TOML_COMPILER_EXCEPTIONS)
		{
			return reader_.peek_eof();
		}

#if !TOML_EXCEPTIONS

		TOML_NODISCARD
		optional<parse_error>&& error() noexcept
		{
			return reader_.error();
		}

#endif
	};
}
TOML_ANON_NAMESPACE_END;

//#---------------------------------------------------------------------------------------------------------------------
//# PARSER INTERNAL IMPLEMENTATION
//#---------------------------------------------------------------------------------------------------------------------

#if TOML_EXCEPTIONS
#define TOML_RETURNS_BY_THROWING [[noreturn]]
#else
#define TOML_RETURNS_BY_THROWING
#endif

TOML_ANON_NAMESPACE_START
{
	template <typename... T>
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr bool is_match(char32_t codepoint, T... vals) noexcept
	{
		static_assert((std::is_same_v<char32_t, T> && ...));
		return ((codepoint == vals) || ...);
	}

	template <uint64_t>
	struct parse_integer_traits;
	template <>
	struct parse_integer_traits<2>
	{
		static constexpr auto scope_qualifier  = "binary integer"sv;
		static constexpr auto is_digit		   = impl::is_binary_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto buffer_length	   = 63;
		static constexpr auto prefix_codepoint = U'b';
		static constexpr auto prefix		   = "b"sv;
	};
	template <>
	struct parse_integer_traits<8>
	{
		static constexpr auto scope_qualifier  = "octal integer"sv;
		static constexpr auto is_digit		   = impl::is_octal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto buffer_length	   = 21; // strlen("777777777777777777777")
		static constexpr auto prefix_codepoint = U'o';
		static constexpr auto prefix		   = "o"sv;
	};
	template <>
	struct parse_integer_traits<10>
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto is_digit		  = impl::is_decimal_digit;
		static constexpr auto is_signed		  = true;
		static constexpr auto buffer_length	  = 19; // strlen("9223372036854775807")
	};
	template <>
	struct parse_integer_traits<16>
	{
		static constexpr auto scope_qualifier  = "hexadecimal integer"sv;
		static constexpr auto is_digit		   = impl::is_hexadecimal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto buffer_length	   = 16; // strlen("7FFFFFFFFFFFFFFF")
		static constexpr auto prefix_codepoint = U'x';
		static constexpr auto prefix		   = "x"sv;
	};

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(node_type val) noexcept
	{
		return impl::node_type_friendly_names[impl::unwrap_enum(val)];
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const std::string& str) noexcept
	{
		return std::string_view{ str };
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(bool val) noexcept
	{
		using namespace std::string_view_literals;

		return val ? "true"sv : "false"sv;
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint& cp) noexcept
	{
		if TOML_UNLIKELY(cp.value <= U'\x1F')
			return impl::low_character_escape_table[cp.value];
		else if TOML_UNLIKELY(cp.value == U'\x7F')
			return "\\u007F"sv;
		else
			return std::string_view{ cp.bytes, cp.count };
	}

	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint* cp) noexcept
	{
		if (cp)
			return to_sv(*cp);
		return ""sv;
	}

	template <typename T>
	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	TOML_NEVER_INLINE
	void concatenate(char*& write_pos, char* const buf_end, const T& arg) noexcept
	{
		static_assert(impl::is_one_of<impl::remove_cvref<T>, std::string_view, int64_t, uint64_t, double>,
					  "concatenate inputs are limited to std::string_view, int64_t, uint64_t and double to keep "
					  "instantiations to a minimum as an anti-bloat measure (hint: to_sv will probably help)");

		if (write_pos >= buf_end)
			return;

		using arg_t = impl::remove_cvref<T>;
		if constexpr (std::is_same_v<arg_t, std::string_view>)
		{
			const auto max_chars = static_cast<size_t>(buf_end - write_pos);
			const auto len		 = max_chars < arg.length() ? max_chars : arg.length();
			std::memcpy(write_pos, arg.data(), len);
			write_pos += len;
		}
		else if constexpr (std::is_floating_point_v<arg_t>)
		{
#if TOML_FLOAT_CHARCONV
			const auto result = std::to_chars(write_pos, buf_end, arg);
			write_pos		  = result.ptr;
#else
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			ss.precision(std::numeric_limits<arg_t>::digits10 + 1);
			ss << arg;
			concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
#endif
		}
		else if constexpr (std::is_integral_v<arg_t>)
		{
#if TOML_INT_CHARCONV
			const auto result = std::to_chars(write_pos, buf_end, arg);
			write_pos		  = result.ptr;
#else
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			using cast_type = std::conditional_t<std::is_signed_v<arg_t>, int64_t, uint64_t>;
			ss << static_cast<cast_type>(arg);
			concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
#endif
		}
	}

	struct error_builder
	{
		static constexpr std::size_t buf_size = 512;
		char buf[buf_size];
		char* write_pos			  = buf;
		char* const max_write_pos = buf + (buf_size - std::size_t{ 1 }); // allow for null terminator

		TOML_NODISCARD_CTOR
		error_builder(std::string_view scope) noexcept
		{
			concatenate(write_pos, max_write_pos, "Error while parsing "sv);
			concatenate(write_pos, max_write_pos, scope);
			concatenate(write_pos, max_write_pos, ": "sv);
		}

		template <typename T>
		void append(const T& arg) noexcept
		{
			concatenate(write_pos, max_write_pos, arg);
		}

		TOML_RETURNS_BY_THROWING
		auto finish(const source_position& pos, const source_path_ptr& source_path) const
		{
			*write_pos = '\0';

#if TOML_EXCEPTIONS
			throw parse_error{ buf, pos, source_path };
#else
			return parse_error{ std::string(buf, static_cast<size_t>(write_pos - buf)), pos, source_path };
#endif
		}

		error_builder(const error_builder&) = delete;
		error_builder(error_builder&&)		= delete;
		error_builder& operator=(const error_builder&) = delete;
		error_builder& operator=(error_builder&&) = delete;
	};

	struct parse_scope
	{
		std::string_view& storage_;
		std::string_view parent_;

		TOML_NODISCARD_CTOR
		explicit parse_scope(std::string_view& current_scope, std::string_view new_scope) noexcept
			: storage_{ current_scope },
			  parent_{ current_scope }
		{
			storage_ = new_scope;
		}

		~parse_scope() noexcept
		{
			storage_ = parent_;
		}

		parse_scope(const parse_scope&) = delete;
		parse_scope(parse_scope&&)		= delete;
		parse_scope& operator=(const parse_scope&) = delete;
		parse_scope& operator=(parse_scope&&) = delete;
	};
#define push_parse_scope_2(scope, line)                                                                                \
	parse_scope ps_##line                                                                                              \
	{                                                                                                                  \
		current_scope, scope                                                                                           \
	}
#define push_parse_scope_1(scope, line) push_parse_scope_2(scope, line)
#define push_parse_scope(scope)			push_parse_scope_1(scope, __LINE__)

	// Q: "why not std::unique_ptr??
	// A: It caused a lot of bloat on some implementations so this exists an internal substitute.
	class node_ptr
	{
	  private:
		node* node_ = {};

	  public:
		TOML_NODISCARD_CTOR
		node_ptr() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit node_ptr(node* n) noexcept //
			: node_{ n }
		{}

		~node_ptr() noexcept
		{
			delete node_;
		}

		node_ptr& operator=(node* val) noexcept
		{
			if (val != node_)
			{
				delete node_;
				node_ = val;
			}
			return *this;
		}

		node_ptr(const node_ptr&) = delete;
		node_ptr& operator=(const node_ptr&) = delete;
		node_ptr(node_ptr&&)				 = delete;
		node_ptr& operator=(node_ptr&&) = delete;

		TOML_PURE_INLINE_GETTER
		operator bool() const noexcept
		{
			return node_ != nullptr;
		}

		TOML_PURE_INLINE_GETTER
		node* operator->() noexcept
		{
			return node_;
		}

		TOML_NODISCARD
		node* release() noexcept
		{
			auto n = node_;
			node_  = nullptr;
			return n;
		}
	};

	struct parsed_key
	{
		source_position position;
		std::vector<std::string> segments;
	};

	struct parsed_key_value_pair
	{
		parsed_key key;
		node_ptr value;
	};

	struct parse_depth_counter
	{
		size_t& depth_;

		TOML_NODISCARD_CTOR
		explicit parse_depth_counter(size_t& depth) noexcept : depth_{ depth }
		{
			depth_++;
		}

		~parse_depth_counter() noexcept
		{
			depth_--;
		}

		parse_depth_counter(const parse_depth_counter&) = delete;
		parse_depth_counter(parse_depth_counter&&)		= delete;
		parse_depth_counter& operator=(const parse_depth_counter&) = delete;
		parse_depth_counter& operator=(parse_depth_counter&&) = delete;
	};

	struct parsed_string
	{
		std::string value;
		bool was_multi_line;
	};
}
TOML_ANON_NAMESPACE_END;

#if 1 // parser helper macros

// Q: "what the fuck is this? MACROS????"
// A: The parser needs to work in exceptionless mode (returning error objects directly)
//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
//    These macros encapsulate the differences between the two modes so I can write code code
//    as though I was only targeting one mode and not want yeet myself into the sun.
//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
//    of toml++.

#define is_eof()		 !cp
#define assert_not_eof() assert_or_assume(cp != nullptr)
#define return_if_eof(...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)

#if TOML_EXCEPTIONS
#define is_error()					false
#define return_after_error(...)		TOML_UNREACHABLE
#define assert_not_error()			static_assert(true)
#define return_if_error(...)		static_assert(true)
#define return_if_error_or_eof(...) return_if_eof(__VA_ARGS__)
#else
#define is_error()				!!err
#define return_after_error(...) return __VA_ARGS__
#define assert_not_error()		TOML_ASSERT(!is_error())
#define return_if_error(...)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_error())                                                                                   \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
#define return_if_error_or_eof(...)                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof() || is_error())                                                                       \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
#endif

#if defined(TOML_BREAK_AT_PARSE_ERRORS) && TOML_BREAK_AT_PARSE_ERRORS
#if defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
#define parse_error_break() __builtin_debugtrap()
#elif __has_builtin(__debugbreak)
#define parse_error_break() __debugbreak()
#endif
#endif
#ifndef parse_error_break
#if TOML_MSVC || TOML_ICC
#define parse_error_break() __debugbreak()
#else
#define parse_error_break() TOML_ASSERT(false)
#endif
#endif
#else
#define parse_error_break() static_assert(true)
#endif

#define set_error_and_return(ret, ...)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!is_error())                                                                                               \
			set_error(__VA_ARGS__);                                                                                    \
		return_after_error(ret);                                                                                       \
	}                                                                                                                  \
	while (false)

#define set_error_and_return_default(...) set_error_and_return({}, __VA_ARGS__)

#define set_error_and_return_if_eof(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		if (is_eof())                                                                                                  \
			set_error_and_return(__VA_ARGS__, "encountered end-of-file"sv);                                            \
	}                                                                                                                  \
	while (false)

#define advance_and_return_if_error(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
	}                                                                                                                  \
	while (false)

#define advance_and_return_if_error_or_eof(...)                                                                        \
	do                                                                                                                 \
	{                                                                                                                  \
		assert_not_eof();                                                                                              \
		advance();                                                                                                     \
		return_if_error(__VA_ARGS__);                                                                                  \
		set_error_and_return_if_eof(__VA_ARGS__);                                                                      \
	}                                                                                                                  \
	while (false)

#endif // parser helper macros

TOML_IMPL_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);

	class parser
	{
	  private:
		static constexpr size_t max_nested_values = TOML_MAX_NESTED_VALUES;

		utf8_buffered_reader reader;
		table root;
		source_position prev_pos = { 1, 1 };
		const utf8_codepoint* cp = {};
		std::vector<table*> implicit_tables;
		std::vector<table*> dotted_key_tables;
		std::vector<array*> table_arrays;
		std::string recording_buffer; // for diagnostics
		bool recording = false, recording_whitespace = true;
		std::string_view current_scope;
		size_t nested_values = {};
#if !TOML_EXCEPTIONS
		mutable optional<parse_error> err;
#endif

		TOML_NODISCARD
		source_position current_position(source_index fallback_offset = 0) const noexcept
		{
			if (!is_eof())
				return cp->position;
			return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at(source_position pos, const T&... reason) const
		{
			static_assert(sizeof...(T) > 0);
#if !TOML_EXCEPTIONS
			if (err)
				return;
#endif

			error_builder builder{ current_scope };
			(builder.append(reason), ...);

			parse_error_break();

#if TOML_EXCEPTIONS
			builder.finish(pos, reader.source_path());
#else
			err.emplace(builder.finish(pos, reader.source_path()));
#endif
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		void set_error(const T&... reason) const
		{
			set_error_at(current_position(1), reason...);
		}

		void go_back(size_t count = 1) noexcept
		{
			return_if_error();
			assert_or_assume(count);

			cp		 = reader.step_back(count);
			prev_pos = cp->position;
		}

		void advance()
		{
			return_if_error();
			assert_not_eof();

			prev_pos = cp->position;
			cp		 = reader.read_next();

#if !TOML_EXCEPTIONS
			if (reader.error())
			{
				err = std::move(reader.error());
				return;
			}
#endif

			if (recording && !is_eof())
			{
				if (recording_whitespace || !is_whitespace(*cp))
					recording_buffer.append(cp->bytes, cp->count);
			}
		}

		void start_recording(bool include_current = true) noexcept
		{
			return_if_error();

			recording			 = true;
			recording_whitespace = true;
			recording_buffer.clear();
			if (include_current && !is_eof())
				recording_buffer.append(cp->bytes, cp->count);
		}

		void stop_recording(size_t pop_bytes = 0) noexcept
		{
			return_if_error();

			recording = false;
			if (pop_bytes)
			{
				if (pop_bytes >= recording_buffer.length())
					recording_buffer.clear();
				else if (pop_bytes == 1u)
					recording_buffer.pop_back();
				else
					recording_buffer.erase(recording_buffer.begin()
											   + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
										   recording_buffer.end());
			}
		}

		bool consume_leading_whitespace()
		{
			return_if_error_or_eof({});

			bool consumed = false;
			while (!is_eof() && is_horizontal_whitespace(*cp))
			{
				consumed = true;
				advance_and_return_if_error({});
			}
			return consumed;
		}

		bool consume_line_break()
		{
			return_if_error_or_eof({});

			if (!is_vertical_whitespace(*cp))
				return false;

			if (*cp == U'\r')
			{
				advance_and_return_if_error({}); // skip \r

				if (is_eof())
					return true; // eof after \r is 'fine'
				else if (*cp != U'\n')
					set_error_and_return_default("expected \\n, saw '"sv, to_sv(*cp), "'"sv);
			}
			advance_and_return_if_error({}); // skip \n (or other single-character line ending)
			return true;
		}

		bool consume_rest_of_line()
		{
			return_if_error_or_eof({});

			do
			{
				if (is_vertical_whitespace(*cp))
					return consume_line_break();
				else
					advance();
				return_if_error({});
			}
			while (!is_eof());

			return true;
		}

		bool consume_comment()
		{
			return_if_error_or_eof({});

			if (*cp != U'#')
				return false;

			push_parse_scope("comment"sv);

			advance_and_return_if_error({}); // skip the '#'

			while (!is_eof())
			{
				if (consume_line_break())
					return true;
				return_if_error({});

				if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
				{
					// toml/issues/567 (disallow non-TAB control characters in comments)
					if (is_nontab_control_character(*cp))
						set_error_and_return_default(
							"control characters other than TAB (U+0009) are explicitly prohibited"sv);

					// toml/pull/720 (disallow surrogates in comments)
					else if (is_unicode_surrogate(*cp))
						set_error_and_return_default(
							"unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv);
				}
				advance_and_return_if_error({});
			}

			return true;
		}

		TOML_NODISCARD
		bool consume_expected_sequence(std::u32string_view seq)
		{
			return_if_error({});
			TOML_ASSERT(!seq.empty());

			for (auto c : seq)
			{
				set_error_and_return_if_eof({});
				if (*cp != c)
					return false;
				advance_and_return_if_error({});
			}
			return true;
		}

		template <typename T>
		TOML_NODISCARD
		bool consume_digit_sequence(T* digits, size_t len)
		{
			return_if_error({});
			assert_or_assume(digits);
			assert_or_assume(len);

			for (size_t i = 0; i < len; i++)
			{
				set_error_and_return_if_eof({});
				if (!is_decimal_digit(*cp))
					return false;

				digits[i] = static_cast<T>(*cp - U'0');
				advance_and_return_if_error({});
			}
			return true;
		}

		template <typename T>
		TOML_NODISCARD
		size_t consume_variable_length_digit_sequence(T* buffer, size_t max_len)
		{
			return_if_error({});
			assert_or_assume(buffer);
			assert_or_assume(max_len);

			size_t i = {};
			for (; i < max_len; i++)
			{
				if (is_eof() || !is_decimal_digit(*cp))
					break;

				buffer[i] = static_cast<T>(*cp - U'0');
				advance_and_return_if_error({});
			}
			return i;
		}

		TOML_NODISCARD
		std::string parse_basic_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(*cp == U'"');
			push_parse_scope("string"sv);

			// skip the '"'
			advance_and_return_if_error_or_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			std::string str;
			bool escaped			 = false;
			bool skipping_whitespace = false;
			do
			{
				if (escaped)
				{
					escaped = false;

					// handle 'line ending slashes' in multi-line mode
					if (multi_line && is_whitespace(*cp))
					{
						consume_leading_whitespace();
						if (!consume_line_break())
							set_error_and_return_default(
								"line-ending backslashes must be the last non-whitespace character on the line"sv);
						skipping_whitespace = true;
						return_if_error({});
						continue;
					}

					bool skipped_escaped_codepoint = false;
					assert_not_eof();
					switch (const auto escaped_codepoint = *cp)
					{
						// 'regular' escape codes
						case U'b': str += '\b'; break;
						case U'f': str += '\f'; break;
						case U'n': str += '\n'; break;
						case U'r': str += '\r'; break;
						case U't': str += '\t'; break;
						case U'"': str += '"'; break;
						case U'\\': str += '\\'; break;

						// unicode scalar sequences
						case U'x':
#if TOML_LANG_UNRELEASED // toml/pull/796 (\xHH unicode scalar sequences)
							[[fallthrough]];
#else
							set_error_and_return_default(
								"escape sequence '\\x' is not supported in TOML 1.0.0 and earlier"sv);
#endif
						case U'u': [[fallthrough]];
						case U'U':
						{
							push_parse_scope("unicode scalar escape sequence"sv);
							advance_and_return_if_error_or_eof({});
							skipped_escaped_codepoint = true;

							uint32_t place_value =
								escaped_codepoint == U'U' ? 0x10000000u : (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
							uint32_t sequence_value{};
							while (place_value)
							{
								set_error_and_return_if_eof({});
								if (!is_hexadecimal_digit(*cp))
									set_error_and_return_default("expected hex digit, saw '"sv, to_sv(*cp), "'"sv);
								sequence_value += place_value * hex_to_dec(*cp);
								place_value /= 16u;
								advance_and_return_if_error({});
							}

							if (is_unicode_surrogate(sequence_value))
								set_error_and_return_default(
									"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
							else if (sequence_value > 0x10FFFFu)
								set_error_and_return_default("values greater than U+10FFFF are invalid"sv);
							else if (sequence_value <= 0x7Fu) // ascii
								str += static_cast<char>(sequence_value & 0x7Fu);
							else if (sequence_value <= 0x7FFu)
							{
								str += static_cast<char>(0xC0u | ((sequence_value >> 6) & 0x1Fu));
								str += static_cast<char>(0x80u | (sequence_value & 0x3Fu));
							}
							else if (sequence_value <= 0xFFFFu)
							{
								str += static_cast<char>(0xE0u | ((sequence_value >> 12) & 0x0Fu));
								str += static_cast<char>(0x80u | ((sequence_value >> 6) & 0x1Fu));
								str += static_cast<char>(0x80u | (sequence_value & 0x3Fu));
							}
							else
							{
								str += static_cast<char>(0xF0u | ((sequence_value >> 18) & 0x07u));
								str += static_cast<char>(0x80u | ((sequence_value >> 12) & 0x3Fu));
								str += static_cast<char>(0x80u | ((sequence_value >> 6) & 0x3Fu));
								str += static_cast<char>(0x80u | (sequence_value & 0x3Fu));
							}
							break;
						}

						// ???
						default: set_error_and_return_default("unknown escape sequence '\\"sv, to_sv(*cp), "'"sv);
					}

					// skip the escaped character
					if (!skipped_escaped_codepoint)
						advance_and_return_if_error_or_eof({});
				}
				else
				{
					// handle closing delimiters
					if (*cp == U'"')
					{
						if (multi_line)
						{
							size_t lookaheads			  = {};
							size_t consecutive_delimiters = 1;
							do
							{
								advance_and_return_if_error({});
								lookaheads++;
								if (!is_eof() && *cp == U'"')
									consecutive_delimiters++;
								else
									break;
							}
							while (lookaheads < 4u);

							switch (consecutive_delimiters)
							{
								// """ " (one quote somewhere in a ML string)
								case 1:
									str += '"';
									skipping_whitespace = false;
									continue;

								// """ "" (two quotes somewhere in a ML string)
								case 2:
									str.append("\"\""sv);
									skipping_whitespace = false;
									continue;

								// """ """ (the end of the string)
								case 3: return str;

								// """ """" (one at the end of the string)
								case 4: str += '"'; return str;

								// """ """"" (two quotes at the end of the string)
								case 5:
									str.append("\"\""sv);
									advance_and_return_if_error({}); // skip the last '"'
									return str;

								default: TOML_UNREACHABLE;
							}
						}
						else
						{
							advance_and_return_if_error({}); // skip the closing delimiter
							return str;
						}
					}

					// handle escapes
					else if (*cp == U'\\')
					{
						advance_and_return_if_error_or_eof({}); // skip the '\'
						skipping_whitespace = false;
						escaped				= true;
						continue;
					}

					// handle line endings in multi-line mode
					if (multi_line && is_vertical_whitespace(*cp))
					{
						consume_line_break();
						return_if_error({});
						if (!skipping_whitespace)
							str += '\n';
						continue;
					}

					// handle control characters
					if (is_nontab_control_character(*cp))
						set_error_and_return_default(
							"unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv);

					// handle surrogates in strings (1.0.0 and later)
					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						if (is_unicode_surrogate(*cp))
							set_error_and_return_default(
								"unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv);
					}

					if (multi_line)
					{
						if (!skipping_whitespace || !is_horizontal_whitespace(*cp))
						{
							skipping_whitespace = false;
							str.append(cp->bytes, cp->count);
						}
					}
					else
						str.append(cp->bytes, cp->count);

					advance_and_return_if_error({});
				}
			}
			while (!is_eof());

			set_error_and_return_default("encountered end-of-file"sv);
		}

		TOML_NODISCARD
		std::string parse_literal_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(*cp == U'\'');
			push_parse_scope("literal string"sv);

			// skip the delimiter
			advance_and_return_if_error_or_eof({});

			// multi-line strings ignore a single line ending right at the beginning
			if (multi_line)
			{
				consume_line_break();
				return_if_error({});
				set_error_and_return_if_eof({});
			}

			std::string str;
			do
			{
				return_if_error({});

				// handle closing delimiters
				if (*cp == U'\'')
				{
					if (multi_line)
					{
						size_t lookaheads			  = {};
						size_t consecutive_delimiters = 1;
						do
						{
							advance_and_return_if_error({});
							lookaheads++;
							if (!is_eof() && *cp == U'\'')
								consecutive_delimiters++;
							else
								break;
						}
						while (lookaheads < 4u);

						switch (consecutive_delimiters)
						{
							// ''' ' (one quote somewhere in a ML string)
							case 1: str += '\''; continue;

							// ''' '' (two quotes somewhere in a ML string)
							case 2: str.append("''"sv); continue;

							// ''' ''' (the end of the string)
							case 3: return str;

							// ''' '''' (one at the end of the string)
							case 4: str += '\''; return str;

							// ''' ''''' (two quotes at the end of the string)
							case 5:
								str.append("''"sv);
								advance_and_return_if_error({}); // skip the last '
								return str;

							default: TOML_UNREACHABLE;
						}
					}
					else
					{
						advance_and_return_if_error({}); // skip the closing delimiter
						return str;
					}
				}

				// handle line endings in multi-line mode
				if (multi_line && is_vertical_whitespace(*cp))
				{
					consume_line_break();
					return_if_error({});
					str += '\n';
					continue;
				}

				// handle control characters
				if (is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are explicitly prohibited"sv);

				// handle surrogates in strings (1.0.0 and later)
				if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
				{
					if (is_unicode_surrogate(*cp))
						set_error_and_return_default(
							"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
				}

				str.append(cp->bytes, cp->count);
				advance_and_return_if_error({});
			}
			while (!is_eof());

			set_error_and_return_default("encountered end-of-file"sv);
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		parsed_string parse_string()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_string_delimiter(*cp));
			push_parse_scope("string"sv);

			// get the first three characters to determine the string type
			const auto first = cp->value;
			advance_and_return_if_error_or_eof({});
			const auto second = cp->value;
			advance_and_return_if_error({});
			const auto third = cp ? cp->value : U'\0';

			// if we were eof at the third character then first and second need to be
			// the same string character (otherwise it's an unterminated string)
			if (is_eof())
			{
				if (second == first)
					return {};

				set_error_and_return_default("encountered end-of-file"sv);
			}

			// if the first three characters are all the same string delimiter then
			// it's a multi-line string.
			else if (first == second && first == third)
			{
				return { first == U'\'' ? parse_literal_string(true) : parse_basic_string(true), true };
			}

			// otherwise it's just a regular string.
			else
			{
				// step back two characters so that the current
				// character is the string delimiter
				go_back(2u);

				return { first == U'\'' ? parse_literal_string(false) : parse_basic_string(false), false };
			}
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string parse_bare_key_segment()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_bare_key_character(*cp));

			std::string segment;
			segment.reserve(10u);

			while (!is_eof())
			{
				if (!is_bare_key_character(*cp))
					break;

				segment.append(cp->bytes, cp->count);
				advance_and_return_if_error({});
			}

			return segment;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		bool parse_boolean()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_match(*cp, U't', U'f', U'T', U'F'));
			push_parse_scope("boolean"sv);

			start_recording(true);
			auto result = is_match(*cp, U't', U'T');
			if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				set_error_and_return_default("expected '"sv,
											 to_sv(result),
											 "', saw '"sv,
											 to_sv(recording_buffer),
											 "'"sv);
			stop_recording();

			if (cp && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return result;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_inf_or_nan()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
			push_parse_scope("floating-point"sv);

			start_recording(true);
			const bool negative = *cp == U'-';
			if (negative || *cp == U'+')
				advance_and_return_if_error_or_eof({});

			const bool inf = is_match(*cp, U'i', U'I');
			if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				set_error_and_return_default("expected '"sv,
											 inf ? "inf"sv : "nan"sv,
											 "', saw '"sv,
											 to_sv(recording_buffer),
											 "'"sv);
			stop_recording();

			if (cp && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return inf ? (negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity())
					   : std::numeric_limits<double>::quiet_NaN();
		}

		TOML_NODISCARD
		double parse_float()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_match(*cp, U'+', U'-', U'.') || is_decimal_digit(*cp));
			push_parse_scope("floating-point"sv);

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_eof({});

			// consume value chars
			char chars[64];
			size_t length			   = {};
			const utf8_codepoint* prev = {};
			bool seen_decimal = false, seen_exponent = false;
			char first_integer_part = '\0';
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_decimal_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if (prev && *prev == U'_' && !is_decimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (*cp == U'.')
				{
					// .1
					// -.1
					// +.1 (no integer part)
					if (!first_integer_part)
						set_error_and_return_default("expected decimal digit, saw '.'"sv);

					// 1.0e+.10 (exponent cannot have '.')
					else if (seen_exponent)
						set_error_and_return_default("expected exponent decimal digit or sign, saw '.'"sv);

					// 1.0.e+.10
					// 1..0
					// (multiple '.')
					else if (seen_decimal)
						set_error_and_return_default("expected decimal digit or exponent, saw '.'"sv);

					seen_decimal = true;
				}
				else if (is_match(*cp, U'e', U'E'))
				{
					if (prev && !is_decimal_digit(*prev))
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

					// 1.0ee+10 (multiple 'e')
					else if (seen_exponent)
						set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

					seen_decimal  = true; // implied
					seen_exponent = true;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 1.-0 (sign in mantissa)
					if (!seen_exponent)
						set_error_and_return_default("expected decimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

					// 1.0e1-0 (misplaced exponent sign)
					else if (!is_match(*prev, U'e', U'E'))
						set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);
				}
				else if (is_decimal_digit(*cp))
				{
					if (!seen_decimal)
					{
						if (!first_integer_part)
							first_integer_part = static_cast<char>(cp->bytes[0]);
						else if (first_integer_part == '0')
							set_error_and_return_default("leading zeroes are prohibited"sv);
					}
				}
				else
					set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);

				if (length == sizeof(chars))
					set_error_and_return_default("exceeds maximum length of "sv,
												 static_cast<uint64_t>(sizeof(chars)),
												 " characters"sv);

				chars[length++] = static_cast<char>(cp->bytes[0]);
				prev			= cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (prev)
			{
				if (*prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits"sv);
				}
				else if (is_match(*prev, U'e', U'E', U'+', U'-', U'.'))
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("expected decimal digit, saw '"sv, to_sv(*cp), "'"sv);
				}
			}

			// convert to double
			double result;
#if TOML_FLOAT_CHARCONV
			{
				auto fc_result = std::from_chars(chars, chars + length, result);
				switch (fc_result.ec)
				{
					case std::errc{}: // ok
						return result * sign;

					case std::errc::invalid_argument:
						set_error_and_return_default("'"sv,
													 std::string_view{ chars, length },
													 "' could not be interpreted as a value"sv);
						break;

					case std::errc::result_out_of_range:
						set_error_and_return_default("'"sv,
													 std::string_view{ chars, length },
													 "' is not representable in 64 bits"sv);
						break;

					default: //??
						set_error_and_return_default("an unspecified error occurred while trying to interpret '"sv,
													 std::string_view{ chars, length },
													 "' as a value"sv);
				}
			}
#else
			{
				std::stringstream ss;
				ss.imbue(std::locale::classic());
				ss.write(chars, static_cast<std::streamsize>(length));
				if ((ss >> result))
					return result * sign;
				else
					set_error_and_return_default("'"sv,
												 std::string_view{ chars, length },
												 "' could not be interpreted as a value"sv);
			}
#endif
		}

		TOML_NODISCARD
		double parse_hex_float()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_match(*cp, U'0', U'+', U'-'));
			push_parse_scope("hexadecimal floating-point"sv);

#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_eof({});

			// '0'
			if (*cp != U'0')
				set_error_and_return_default(" expected '0', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// 'x' or 'X'
			if (!is_match(*cp, U'x', U'X'))
				set_error_and_return_default("expected 'x' or 'X', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// <HEX DIGITS> ([.]<HEX DIGITS>)? [pP] [+-]? <DEC DIGITS>

			// consume value fragments
			struct fragment
			{
				char chars[24];
				size_t length;
				double value;
			};
			fragment fragments[] = {
				{}, // mantissa, whole part
				{}, // mantissa, fractional part
				{}	// exponent
			};
			fragment* current_fragment = fragments;
			const utf8_codepoint* prev = {};
			int exponent_sign		   = 1;
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_hexadecimal_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if (prev && *prev == U'_' && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (*cp == U'.')
				{
					// 0x10.0p-.0 (exponent cannot have '.')
					if (current_fragment == fragments + 2)
						set_error_and_return_default("expected exponent digit or sign, saw '.'"sv);

					// 0x10.0.p-0 (multiple '.')
					else if (current_fragment == fragments + 1)
						set_error_and_return_default("expected hexadecimal digit or exponent, saw '.'"sv);

					else
						current_fragment++;
				}
				else if (is_match(*cp, U'p', U'P'))
				{
					// 0x10.0pp-0 (multiple 'p')
					if (current_fragment == fragments + 2)
						set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);

					// 0x.p-0 (mantissa is just '.')
					else if (fragments[0].length == 0u && fragments[1].length == 0u)
						set_error_and_return_default("expected hexadecimal digit, saw '"sv, to_sv(*cp), "'"sv);

					else
						current_fragment = fragments + 2;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 0x-10.0p-0 (sign in mantissa)
					if (current_fragment != fragments + 2)
						set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);

					// 0x10.0p0- (misplaced exponent sign)
					else if (!is_match(*prev, U'p', U'P'))
						set_error_and_return_default("expected exponent digit, saw '"sv, to_sv(*cp), "'"sv);

					else
						exponent_sign = *cp == U'-' ? -1 : 1;
				}
				else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, to_sv(*cp), "'"sv);
				else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
					set_error_and_return_default("expected exponent digit or sign, saw '"sv, to_sv(*cp), "'"sv);
				else if (current_fragment->length == sizeof(fragment::chars))
					set_error_and_return_default("fragment exceeeds maximum length of "sv,
												 static_cast<uint64_t>(sizeof(fragment::chars)),
												 " characters"sv);
				else
					current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);

				prev = cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (current_fragment != fragments + 2 || current_fragment->length == 0u)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("missing exponent"sv);
			}
			else if (prev && *prev == U'_')
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("underscores must be followed by digits"sv);
			}

			// calculate values for the three fragments
			for (int fragment_idx = 0; fragment_idx < 3; fragment_idx++)
			{
				auto& f				= fragments[fragment_idx];
				const uint32_t base = fragment_idx == 2 ? 10u : 16u;

				// left-trim zeroes
				const char* c = f.chars;
				size_t sig	  = {};
				while (f.length && *c == '0')
				{
					f.length--;
					c++;
					sig++;
				}
				if (!f.length)
					continue;

				// calculate value
				auto place = 1u;
				for (size_t i = 0; i < f.length - 1u; i++)
					place *= base;
				uint32_t val{};
				while (place)
				{
					if (base == 16)
						val += place * hex_to_dec(*c);
					else
						val += place * static_cast<uint32_t>(*c - '0');
					if (fragment_idx == 1)
						sig++;
					c++;
					place /= base;
				}
				f.value = static_cast<double>(val);

				// shift the fractional part
				if (fragment_idx == 1)
				{
					while (sig--)
						f.value /= base;
				}
			}

			return (fragments[0].value + fragments[1].value) * pow(2.0, fragments[2].value * exponent_sign) * sign;

#else // !TOML_LANG_UNRELEASED

			set_error_and_return_default("hexadecimal floating-point values are not supported "
										 "in TOML 1.0.0 and earlier"sv);

#endif // !TOML_LANG_UNRELEASED
		}

		template <uint64_t base>
		TOML_NODISCARD
		int64_t parse_integer()
		{
			return_if_error({});
			assert_not_eof();
			using traits = parse_integer_traits<base>;
			push_parse_scope(traits::scope_qualifier);

			[[maybe_unused]] int64_t sign = 1;
			if constexpr (traits::is_signed)
			{
				sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});
			}

			if constexpr (base == 10)
			{
				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected expected digit or sign, saw '"sv, to_sv(*cp), "'"sv);
			}
			else
			{
				// '0'
				if (*cp != U'0')
					set_error_and_return_default("expected '0', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// 'b', 'o', 'x'
				if (*cp != traits::prefix_codepoint)
					set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});
			}

			// consume value chars
			char chars[traits::buffer_length];
			size_t length			   = {};
			const utf8_codepoint* prev = {};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !traits::is_digit(*prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = cp;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				else if (prev && *prev == U'_' && !traits::is_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (!traits::is_digit(*cp))
					set_error_and_return_default("expected digit, saw '"sv, to_sv(*cp), "'"sv);
				else if (length == sizeof(chars))
					set_error_and_return_default("exceeds maximum length of "sv,
												 static_cast<uint64_t>(sizeof(chars)),
												 " characters"sv);
				else
					chars[length++] = static_cast<char>(cp->bytes[0]);

				prev = cp;
				advance_and_return_if_error({});
			}

			// sanity check ending state
			if (prev && *prev == U'_')
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("underscores must be followed by digits"sv);
			}

			// check for leading zeroes
			if constexpr (base == 10)
			{
				if (chars[0] == '0')
					set_error_and_return_default("leading zeroes are prohibited"sv);
			}

			// single digits can be converted trivially
			if (length == 1u)
			{
				if constexpr (base == 16)
					return static_cast<int64_t>(hex_to_dec(chars[0]));
				else if constexpr (base <= 10)
					return static_cast<int64_t>(chars[0] - '0');
			}

			// otherwise do the thing
			uint64_t result = {};
			{
				const char* msd = chars;
				const char* end = msd + length;
				while (msd < end && *msd == '0')
					msd++;
				if (msd == end)
					return 0ll;
				uint64_t power = 1;
				while (--end >= msd)
				{
					if constexpr (base == 16)
						result += power * hex_to_dec(*end);
					else
						result += power * static_cast<uint64_t>(*end - '0');
					power *= base;
				}
			}

			// range check
			if (result > static_cast<uint64_t>((std::numeric_limits<int64_t>::max)()) + (sign < 0 ? 1ull : 0ull))
				set_error_and_return_default("'"sv,
											 std::string_view{ chars, length },
											 "' is not representable in 64 bits"sv);

			if constexpr (traits::is_signed)
				return static_cast<int64_t>(result) * sign;
			else
				return static_cast<int64_t>(result);
		}

		TOML_NODISCARD
		date parse_date(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_decimal_digit(*cp));
			push_parse_scope("date"sv);

			// "YYYY"
			uint32_t digits[4];
			if (!consume_digit_sequence(digits, 4u))
				set_error_and_return_default("expected 4-digit year, saw '"sv, to_sv(cp), "'"sv);
			const auto year			= digits[3] + digits[2] * 10u + digits[1] * 100u + digits[0] * 1000u;
			const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit month, saw '"sv, to_sv(cp), "'"sv);
			const auto month = digits[1] + digits[0] * 10u;
			if (month == 0u || month > 12u)
				set_error_and_return_default("expected month between 1 and 12 (inclusive), saw "sv,
											 static_cast<uint64_t>(month));
			const auto max_days_in_month = month == 2u
											 ? (is_leap_year ? 29u : 28u)
											 : (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u);
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "DD"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit day, saw '"sv, to_sv(cp), "'"sv);
			const auto day = digits[1] + digits[0] * 10u;
			if (day == 0u || day > max_days_in_month)
				set_error_and_return_default("expected day between 1 and "sv,
											 static_cast<uint64_t>(max_days_in_month),
											 " (inclusive), saw "sv,
											 static_cast<uint64_t>(day));

			if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return { year, month, day };
		}

		TOML_NODISCARD
		time parse_time(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_decimal_digit(*cp));
			push_parse_scope("time"sv);

			static constexpr size_t max_digits = 9;
			uint32_t digits[max_digits];

			// "HH"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(cp), "'"sv);
			const auto hour = digits[1] + digits[0] * 10u;
			if (hour > 23u)
				set_error_and_return_default("expected hour between 0 to 59 (inclusive), saw "sv,
											 static_cast<uint64_t>(hour));
			set_error_and_return_if_eof({});

			// ':'
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(cp), "'"sv);
			const auto minute = digits[1] + digits[0] * 10u;
			if (minute > 59u)
				set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv,
											 static_cast<uint64_t>(minute));
			auto time = toml::time{ hour, minute };

			// ':'
			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/671 (allow omission of seconds)
			{
				if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
					return time;
			}
			else
				set_error_and_return_if_eof({});
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "SS"
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit second, saw '"sv, to_sv(cp), "'"sv);
			const auto second = digits[1] + digits[0] * 10u;
			if (second > 59u)
				set_error_and_return_default("expected second between 0 and 59 (inclusive), saw "sv,
											 static_cast<uint64_t>(second));
			time.second = static_cast<decltype(time.second)>(second);

			// '.' (early-exiting is allowed; fractional is optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return time;
			if (*cp != U'.')
				set_error_and_return_default("expected '.', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "FFFFFFFFF"
			size_t digit_count = consume_variable_length_digit_sequence(digits, max_digits);
			if (!digit_count)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("expected fractional digits, saw '"sv, to_sv(*cp), "'"sv);
			}
			else if (!is_eof())
			{
				if (digit_count == max_digits && is_decimal_digit(*cp))
					set_error_and_return_default("fractional component exceeds maximum precision of "sv,
												 static_cast<uint64_t>(max_digits));
				else if (!part_of_datetime && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);
			}

			uint32_t value = 0u;
			uint32_t place = 1u;
			for (auto i = digit_count; i-- > 0u;)
			{
				value += digits[i] * place;
				place *= 10u;
			}
			for (auto i = digit_count; i < max_digits; i++) // implicit zeros
				value *= 10u;
			time.nanosecond = value;
			return time;
		}

		TOML_NODISCARD
		date_time parse_date_time()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_decimal_digit(*cp));
			push_parse_scope("date-time"sv);

			// "YYYY-MM-DD"
			auto date = parse_date(true);
			set_error_and_return_if_eof({});

			// ' ', 'T' or 't'
			if (!is_match(*cp, U' ', U'T', U't'))
				set_error_and_return_default("expected space, 'T' or 't', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// "HH:MM:SS.FFFFFFFFF"
			auto time = parse_time(true);
			return_if_error({});

			// no offset
			if (is_eof() || is_value_terminator(*cp))
				return { date, time };

			// zero offset ('Z' or 'z')
			time_offset offset{};
			if (is_match(*cp, U'Z', U'z'))
				advance_and_return_if_error({});

			// explicit offset ("+/-HH:MM")
			else if (is_match(*cp, U'+', U'-'))
			{
				push_parse_scope("date-time offset"sv);

				// sign
				int sign = *cp == U'-' ? -1 : 1;
				advance_and_return_if_error_or_eof({});

				// "HH"
				int digits[2];
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit hour, saw '"sv, to_sv(cp), "'"sv);
				const auto hour = digits[1] + digits[0] * 10;
				if (hour > 23)
					set_error_and_return_default("expected hour between 0 and 23 (inclusive), saw "sv,
												 static_cast<int64_t>(hour));
				set_error_and_return_if_eof({});

				// ':'
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, to_sv(*cp), "'"sv);
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit minute, saw '"sv, to_sv(cp), "'"sv);
				const auto minute = digits[1] + digits[0] * 10;
				if (minute > 59)
					set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv,
												 static_cast<int64_t>(minute));
				offset.minutes = static_cast<decltype(offset.minutes)>((hour * 60 + minute) * sign);
			}

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected value-terminator, saw '"sv, to_sv(*cp), "'"sv);

			return { date, time, offset };
		}

		TOML_NODISCARD
		array* parse_array();

		TOML_NODISCARD
		table* parse_inline_table();

		TOML_NODISCARD
		node* parse_value_known_prefixes()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(!is_control_character(*cp));
			assert_or_assume(*cp != U'_');

			switch (cp->value)
			{
				// arrays
				case U'[': return parse_array();

				// inline tables
				case U'{':
					return parse_inline_table();

					// floats beginning with '.'
				case U'.':
					return new value{ parse_float() };

					// strings
				case U'"': [[fallthrough]];
				case U'\'':
					return new value{ std::move(parse_string().value) };

					// bools
				case U't': [[fallthrough]];
				case U'f': [[fallthrough]];
				case U'T': [[fallthrough]];
				case U'F':
					return new value{ parse_boolean() };

					// inf/nan
				case U'i': [[fallthrough]];
				case U'I': [[fallthrough]];
				case U'n': [[fallthrough]];
				case U'N': return new value{ parse_inf_or_nan() };
			}
			return nullptr;
		}

		TOML_NODISCARD
		node* parse_value()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(!is_value_terminator(*cp));
			push_parse_scope("value"sv);

			const parse_depth_counter depth_counter{ nested_values };
			if (nested_values > max_nested_values)
				set_error_and_return_default("exceeded maximum nested value depth of "sv,
											 static_cast<uint64_t>(max_nested_values),
											 " (TOML_MAX_NESTED_VALUES)"sv);

			// check if it begins with some control character
			// (note that this will also fail for whitespace but we're assuming we've
			// called consume_leading_whitespace() before calling parse_value())
			if TOML_UNLIKELY(is_control_character(*cp))
				set_error_and_return_default("unexpected control character"sv);

			// underscores at the beginning
			else if (*cp == U'_')
				set_error_and_return_default("values may not begin with underscores"sv);

			const auto begin_pos = cp->position;
			node_ptr val;

			do
			{
				assert_or_assume(!is_control_character(*cp));
				assert_or_assume(*cp != U'_');

				// detect the value type and parse accordingly,
				// starting with value types that can be detected
				// unambiguously from just one character.

				val = parse_value_known_prefixes();
				return_if_error({});
				if (val)
					break;

				// value types from here down require more than one character to unambiguously identify
				// so scan ahead and collect a set of value 'traits'.
				enum TOML_CLOSED_FLAGS_ENUM value_traits : int
				{
					has_nothing	 = 0,
					has_digits	 = 1,
					has_b		 = 1 << 1, // as second char only (0b)
					has_e		 = 1 << 2, // only float exponents
					has_o		 = 1 << 3, // as second char only (0o)
					has_p		 = 1 << 4, // only hexfloat exponents
					has_t		 = 1 << 5,
					has_x		 = 1 << 6, // as second or third char only (0x, -0x, +0x)
					has_z		 = 1 << 7,
					has_colon	 = 1 << 8,
					has_plus	 = 1 << 9,
					has_minus	 = 1 << 10,
					has_dot		 = 1 << 11,
					begins_sign	 = 1 << 12,
					begins_digit = 1 << 13,
					begins_zero	 = 1 << 14,

					signs_msk  = has_plus | has_minus,
					bzero_msk  = begins_zero | has_digits,
					bdigit_msk = begins_digit | has_digits,
				};
				value_traits traits	 = has_nothing;
				const auto has_any	 = [&](auto t) noexcept { return (traits & t) != has_nothing; };
				const auto has_none	 = [&](auto t) noexcept { return (traits & t) == has_nothing; };
				const auto add_trait = [&](auto t) noexcept { traits = static_cast<value_traits>(traits | t); };

				// examine the first character to get the 'begins with' traits
				// (good fail-fast opportunity; all the remaining types begin with numeric digits or signs)
				if (is_decimal_digit(*cp))
					add_trait(*cp == U'0' ? begins_zero : begins_digit);
				else if (is_match(*cp, U'+', U'-'))
					add_trait(begins_sign);
				else
					break;

				// scan the rest of the value to determine the remaining traits
				char32_t chars[utf8_buffered_reader::max_history_length];
				size_t char_count = {}, advance_count = {};
				bool eof_while_scanning = false;
				const auto scan			= [&]()
				{
					if (is_eof())
						return;
					assert_or_assume(!is_value_terminator(*cp));

					do
					{
						if (const auto c = **cp; c != U'_')
						{
							chars[char_count++] = c;

							if (is_decimal_digit(c))
								add_trait(has_digits);
							else if (is_ascii_letter(c))
							{
								assert_or_assume((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'));
								switch (static_cast<char32_t>(c | 32u))
								{
									case U'b':
										if (char_count == 2u && has_any(begins_zero))
											add_trait(has_b);
										break;

									case U'e':
										if (char_count > 1u
											&& has_none(has_b | has_o | has_p | has_t | has_x | has_z | has_colon)
											&& (has_none(has_plus | has_minus) || has_any(begins_sign)))
											add_trait(has_e);
										break;

									case U'o':
										if (char_count == 2u && has_any(begins_zero))
											add_trait(has_o);
										break;

									case U'p':
										if (has_any(has_x))
											add_trait(has_p);
										break;

									case U'x':
										if ((char_count == 2u && has_any(begins_zero))
											|| (char_count == 3u && has_any(begins_sign) && chars[1] == U'0'))
											add_trait(has_x);
										break;

									case U't': add_trait(has_t); break;
									case U'z': add_trait(has_z); break;
								}
							}
							else if (c <= U':')
							{
								assert_or_assume(c < U'0' || c > U'9');
								switch (c)
								{
									case U'+': add_trait(has_plus); break;
									case U'-': add_trait(has_minus); break;
									case U'.': add_trait(has_dot); break;
									case U':': add_trait(has_colon); break;
								}
							}
						}

						advance_and_return_if_error();
						advance_count++;
						eof_while_scanning = is_eof();
					}
					while (advance_count < utf8_buffered_reader::max_history_length && !is_eof()
						   && !is_value_terminator(*cp));
				};
				scan();
				return_if_error({});

				// force further scanning if this could have been a date-time with a space instead of a T
				if (char_count == 10u && traits == (bdigit_msk | has_minus) && chars[4] == U'-' && chars[7] == U'-'
					&& !is_eof() && *cp == U' ')
				{
					const auto pre_advance_count = advance_count;
					const auto pre_scan_traits	 = traits;
					chars[char_count++]			 = *cp;
					add_trait(has_t);

					const auto backpedal = [&]() noexcept
					{
						go_back(advance_count - pre_advance_count);
						advance_count = pre_advance_count;
						traits		  = pre_scan_traits;
						char_count	  = 10u;
					};

					advance_and_return_if_error({});
					advance_count++;

					if (is_eof() || !is_decimal_digit(*cp))
						backpedal();
					else
					{
						chars[char_count++] = *cp;

						advance_and_return_if_error({});
						advance_count++;

						scan();
						return_if_error({});

						if (char_count == 12u)
							backpedal();
					}
				}

				// set the reader back to where we started
				go_back(advance_count);
				if (char_count < utf8_buffered_reader::max_history_length - 1u)
					chars[char_count] = U'\0';

				// if after scanning ahead we still only have one value character,
				// the only valid value type is an integer.
				if (char_count == 1u)
				{
					if (has_any(begins_zero | begins_digit))
					{
						val = new value{ static_cast<int64_t>(chars[0] - U'0') };
						advance(); // skip the digit
						break;
					}

					// anything else would be ambiguous.
					else
						set_error_and_return_default(eof_while_scanning ? "encountered end-of-file"sv
																		: "could not determine value type"sv);
				}

				// now things that can be identified from two or more characters
				return_if_error({});
				assert_or_assume(char_count >= 2u);

				// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
				// typed parse functions to take over and show better diagnostics if there's an issue
				// (as opposed to the fallback "could not determine type" message)
				if (has_any(has_p))
					val = new value{ parse_hex_float() };
				else if (has_any(has_x | has_o | has_b))
				{
					int64_t i;
					value_flags flags;
					if (has_any(has_x))
					{
						i	  = parse_integer<16>();
						flags = value_flags::format_as_hexadecimal;
					}
					else if (has_any(has_o))
					{
						i	  = parse_integer<8>();
						flags = value_flags::format_as_octal;
					}
					else // has_b
					{
						i	  = parse_integer<2>();
						flags = value_flags::format_as_binary;
					}
					return_if_error({});

					val = new value{ i };
					val->ref_cast<int64_t>().flags(flags);
				}
				else if (has_any(has_e) || (has_any(begins_zero | begins_digit) && chars[1] == U'.'))
					val = new value{ parse_float() };
				else if (has_any(begins_sign))
				{
					// single-digit signed integers
					if (char_count == 2u && has_any(has_digits))
					{
						val = new value{ static_cast<int64_t>(chars[1] - U'0') * (chars[0] == U'-' ? -1LL : 1LL) };
						advance(); // skip the sign
						advance(); // skip the digit
						break;
					}

					// simple signed floats (e.g. +1.0)
					if (is_decimal_digit(chars[1]) && chars[2] == U'.')
						val = new value{ parse_float() };

					// signed infinity or nan
					else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
						val = new value{ parse_inf_or_nan() };
				}

				return_if_error({});
				if (val)
					break;

				// match trait masks against what they can match exclusively.
				// all correct value parses will come out of this list, so doing this as a switch is likely to
				// be a better friend to the optimizer on the success path (failure path can be slow but that
				// doesn't matter much).
				switch (unwrap_enum(traits))
				{
					// binary integers
					// 0b10
					case bzero_msk | has_b:
						val = new value{ parse_integer<2>() };
						val->ref_cast<int64_t>().flags(value_flags::format_as_binary);
						break;

					// octal integers
					// 0o10
					case bzero_msk | has_o:
						val = new value{ parse_integer<8>() };
						val->ref_cast<int64_t>().flags(value_flags::format_as_octal);
						break;

					// decimal integers
					// 00
					// 10
					// +10
					// -10
					case bzero_msk: [[fallthrough]];
					case bdigit_msk: [[fallthrough]];
					case begins_sign | has_digits | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_plus: val = new value{ parse_integer<10>() }; break;

					// hexadecimal integers
					// 0x10
					case bzero_msk | has_x:
						val = new value{ parse_integer<16>() };
						val->ref_cast<int64_t>().flags(value_flags::format_as_hexadecimal);
						break;

					// decimal floats
					// 0e1
					// 0e-1
					// 0e+1
					// 0.0
					// 0.0e1
					// 0.0e-1
					// 0.0e+1
					case bzero_msk | has_e: [[fallthrough]];
					case bzero_msk | has_e | has_minus: [[fallthrough]];
					case bzero_msk | has_e | has_plus: [[fallthrough]];
					case bzero_msk | has_dot: [[fallthrough]];
					case bzero_msk | has_dot | has_e: [[fallthrough]];
					case bzero_msk | has_dot | has_e | has_minus: [[fallthrough]];
					case bzero_msk | has_dot | has_e | has_plus: [[fallthrough]];
					// 1e1
					// 1e-1
					// 1e+1
					// 1.0
					// 1.0e1
					// 1.0e-1
					// 1.0e+1
					case bdigit_msk | has_e: [[fallthrough]];
					case bdigit_msk | has_e | has_minus: [[fallthrough]];
					case bdigit_msk | has_e | has_plus: [[fallthrough]];
					case bdigit_msk | has_dot: [[fallthrough]];
					case bdigit_msk | has_dot | has_e: [[fallthrough]];
					case bdigit_msk | has_dot | has_e | has_minus: [[fallthrough]];
					case bdigit_msk | has_dot | has_e | has_plus: [[fallthrough]];
					// +1e1
					// +1.0
					// +1.0e1
					// +1.0e+1
					// +1.0e-1
					// -1.0e+1
					case begins_sign | has_digits | has_e | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | signs_msk: [[fallthrough]];
					// -1e1
					// -1e+1
					// +1e-1
					// -1.0
					// -1.0e1
					// -1.0e-1
					case begins_sign | has_digits | has_e | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_e | signs_msk: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_dot | has_e | has_minus:
						val = new value{ parse_float() };
						break;

					// hexadecimal floats
					// 0x10p0
					// 0x10p-0
					// 0x10p+0
					case bzero_msk | has_x | has_p: [[fallthrough]];
					case bzero_msk | has_x | has_p | has_minus: [[fallthrough]];
					case bzero_msk | has_x | has_p | has_plus: [[fallthrough]];
					// -0x10p0
					// -0x10p-0
					// +0x10p0
					// +0x10p+0
					// -0x10p+0
					// +0x10p-0
					case begins_sign | has_digits | has_x | has_p | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_p | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_p | signs_msk: [[fallthrough]];
					// 0x10.1p0
					// 0x10.1p-0
					// 0x10.1p+0
					case bzero_msk | has_x | has_dot | has_p: [[fallthrough]];
					case bzero_msk | has_x | has_dot | has_p | has_minus: [[fallthrough]];
					case bzero_msk | has_x | has_dot | has_p | has_plus: [[fallthrough]];
					// -0x10.1p0
					// -0x10.1p-0
					// +0x10.1p0
					// +0x10.1p+0
					// -0x10.1p+0
					// +0x10.1p-0
					case begins_sign | has_digits | has_x | has_dot | has_p | has_minus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_dot | has_p | has_plus: [[fallthrough]];
					case begins_sign | has_digits | has_x | has_dot | has_p | signs_msk:
						val = new value{ parse_hex_float() };
						break;

					// times
					// HH:MM
					// HH:MM:SS
					// HH:MM:SS.FFFFFF
					case bzero_msk | has_colon: [[fallthrough]];
					case bzero_msk | has_colon | has_dot: [[fallthrough]];
					case bdigit_msk | has_colon: [[fallthrough]];
					case bdigit_msk | has_colon | has_dot: val = new value{ parse_time() }; break;

					// local dates
					// YYYY-MM-DD
					case bzero_msk | has_minus: [[fallthrough]];
					case bdigit_msk | has_minus: val = new value{ parse_date() }; break;

					// date-times
					// YYYY-MM-DDTHH:MM
					// YYYY-MM-DDTHH:MM-HH:MM
					// YYYY-MM-DDTHH:MM+HH:MM
					// YYYY-MM-DD HH:MM
					// YYYY-MM-DD HH:MM-HH:MM
					// YYYY-MM-DD HH:MM+HH:MM
					// YYYY-MM-DDTHH:MM:SS
					// YYYY-MM-DDTHH:MM:SS-HH:MM
					// YYYY-MM-DDTHH:MM:SS+HH:MM
					// YYYY-MM-DD HH:MM:SS
					// YYYY-MM-DD HH:MM:SS-HH:MM
					// YYYY-MM-DD HH:MM:SS+HH:MM
					case bzero_msk | has_minus | has_colon | has_t: [[fallthrough]];
					case bzero_msk | signs_msk | has_colon | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_t: [[fallthrough]];
					case bdigit_msk | signs_msk | has_colon | has_t: [[fallthrough]];
					// YYYY-MM-DDTHH:MM:SS.FFFFFF
					// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
					// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
					// YYYY-MM-DD HH:MM:SS.FFFFFF
					// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
					// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
					case bzero_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
					case bzero_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_dot | has_t: [[fallthrough]];
					case bdigit_msk | signs_msk | has_colon | has_dot | has_t: [[fallthrough]];
					// YYYY-MM-DDTHH:MMZ
					// YYYY-MM-DD HH:MMZ
					// YYYY-MM-DDTHH:MM:SSZ
					// YYYY-MM-DD HH:MM:SSZ
					// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
					// YYYY-MM-DD HH:MM:SS.FFFFFFZ
					case bzero_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
					case bzero_msk | has_minus | has_colon | has_dot | has_z | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_z | has_t: [[fallthrough]];
					case bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
						val = new value{ parse_date_time() };
						break;
				}
			}
			while (false);

			if (!val)
			{
				set_error_at(begin_pos, "could not determine value type"sv);
				return_after_error({});
			}

#if !TOML_LANG_AT_LEAST(1, 0, 0) // toml/issues/665 (heterogeneous arrays)
			{
				if (auto arr = val->as_array(); arr && !arr->is_homogeneous())
				{
					delete arr;
					set_error_at(begin_pos, "arrays cannot contain values of different types before TOML 1.0.0"sv);
					return_after_error({});
				}
			}
#endif

			val->source_ = { begin_pos, current_position(1), reader.source_path() };
			return val.release();
		}

		TOML_NODISCARD
		parsed_key parse_key()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_bare_key_character(*cp) || is_string_delimiter(*cp));
			push_parse_scope("key"sv);

			parsed_key key;
			key.position		 = current_position();
			recording_whitespace = false;
			std::string pending_key_segment;

			while (!is_error())
			{
#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
				if (is_combining_mark(*cp))
					set_error_and_return_default("bare keys may not begin with unicode combining marks"sv);
#endif

				// bare_key_segment
				if (is_bare_key_character(*cp))
					pending_key_segment = parse_bare_key_segment();

				// "quoted key segment"
				else if (is_string_delimiter(*cp))
				{
					const auto begin_pos = cp->position;

					recording_whitespace = true;
					parsed_string str	 = parse_string();
					recording_whitespace = false;
					return_if_error({});

					if (str.was_multi_line)
					{
						set_error_at(begin_pos,
									 "multi-line strings are prohibited in "sv,
									 key.segments.empty() ? ""sv : "dotted "sv,
									 "keys"sv);
						return_after_error({});
					}
					else
						pending_key_segment = std::move(str.value);
				}

				// ???
				else
					set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
												 to_sv(*cp),
												 "'"sv);

				// whitespace following the key segment
				consume_leading_whitespace();

				// eof or no more key to come
				if (is_eof() || *cp != U'.')
				{
					key.segments.push_back(std::move(pending_key_segment));
					break;
				}

				// was a dotted key - reserve capacity for a few segments
				if (!key.segments.capacity())
					key.segments.reserve(3u);
				key.segments.push_back(std::move(pending_key_segment));

				// go around again to consume the next segment
				advance_and_return_if_error_or_eof({});
				consume_leading_whitespace();
				set_error_and_return_if_eof({});
			}
			return_if_error({});
			return key;
		}

		TOML_NODISCARD
		parsed_key_value_pair parse_key_value_pair()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(is_string_delimiter(*cp) || is_bare_key_character(*cp));
			push_parse_scope("key-value pair"sv);

			// get the key
			start_recording();
			auto key = parse_key();
			stop_recording(1u);

			// skip past any whitespace that followed the key
			consume_leading_whitespace();
			set_error_and_return_if_eof({});

			// '='
			if (*cp != U'=')
				set_error_and_return_default("expected '=', saw '"sv, to_sv(*cp), "'"sv);
			advance_and_return_if_error_or_eof({});

			// skip past any whitespace that followed the '='
			consume_leading_whitespace();
			return_if_error({});
			set_error_and_return_if_eof({});

			// get the value
			if (is_value_terminator(*cp))
				set_error_and_return_default("expected value, saw '"sv, to_sv(*cp), "'"sv);
			return { std::move(key), node_ptr{ parse_value() } };
		}

		TOML_NODISCARD
		table* parse_table_header()
		{
			return_if_error({});
			assert_not_eof();
			assert_or_assume(*cp == U'[');
			push_parse_scope("table header"sv);

			const source_position header_begin_pos = cp->position;
			source_position header_end_pos;
			parsed_key key;
			bool is_arr = false;

			// parse header
			{
				// skip first '['
				advance_and_return_if_error_or_eof({});

				// skip past any whitespace that followed the '['
				const bool had_leading_whitespace = consume_leading_whitespace();
				set_error_and_return_if_eof({});

				// skip second '[' (if present)
				if (*cp == U'[')
				{
					if (had_leading_whitespace)
						set_error_and_return_default(
							"[[array-of-table]] brackets must be contiguous (i.e. [ [ this ] ] is prohibited)"sv);

					is_arr = true;
					advance_and_return_if_error_or_eof({});

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					set_error_and_return_if_eof({});
				}

				// check for a premature closing ']'
				if (*cp == U']')
					set_error_and_return_default("tables with blank bare keys are explicitly prohibited"sv);

				// get the actual key
				start_recording();
				key = parse_key();
				stop_recording(1u);
				return_if_error({});

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});

				// consume the closing ']'
				if (*cp != U']')
					set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
				if (is_arr)
				{
					advance_and_return_if_error_or_eof({});
					if (*cp != U']')
						set_error_and_return_default("expected ']', saw '"sv, to_sv(*cp), "'"sv);
				}
				advance_and_return_if_error({});
				header_end_pos = current_position(1);

				// handle the rest of the line after the header
				consume_leading_whitespace();
				if (!is_eof() && !consume_comment() && !consume_line_break())
					set_error_and_return_default("expected a comment or whitespace, saw '"sv, to_sv(cp), "'"sv);
			}
			TOML_ASSERT(!key.segments.empty());

			// check if each parent is a table/table array, or can be created implicitly as a table.
			auto parent = &root;
			for (size_t i = 0; i < key.segments.size() - 1u; i++)
			{
				auto child = parent->get(key.segments[i]);
				if (!child)
				{
					child = parent->map_.emplace(key.segments[i], new table{}).first->second.get();
					implicit_tables.push_back(&child->ref_cast<table>());
					child->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					parent		   = &child->ref_cast<table>();
				}
				else if (child->is_table())
				{
					parent = &child->ref_cast<table>();
				}
				else if (child->is_array()
						 && impl::find(table_arrays.begin(), table_arrays.end(), &child->ref_cast<array>()))
				{
					// table arrays are a special case;
					// the spec dictates we select the most recently declared element in the array.
					TOML_ASSERT(!child->ref_cast<array>().elems_.empty());
					TOML_ASSERT(child->ref_cast<array>().elems_.back()->is_table());
					parent = &child->ref_cast<array>().back().ref_cast<table>();
				}
				else
				{
					if (!is_arr && child->type() == node_type::table)
						set_error_and_return_default("cannot redefine existing table '"sv,
													 to_sv(recording_buffer),
													 "'"sv);
					else
						set_error_and_return_default("cannot redefine existing "sv,
													 to_sv(child->type()),
													 " '"sv,
													 to_sv(recording_buffer),
													 "' as "sv,
													 is_arr ? "array-of-tables"sv : "table"sv);
				}
			}

			// check the last parent table for a node matching the last key.
			// if there was no matching node, then sweet;
			// we can freely instantiate a new table/table array.
			auto matching_node = parent->get(key.segments.back());
			if (!matching_node)
			{
				// if it's an array we need to make the array and it's first table element,
				// set the starting regions, and return the table element
				if (is_arr)
				{
					array& tbl_arr = parent->emplace<array>(key.segments.back()).first->second.ref_cast<array>();
					table_arrays.push_back(&tbl_arr);
					tbl_arr.source_ = { header_begin_pos, header_end_pos, reader.source_path() };

					table& tbl	= tbl_arr.emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				// otherwise we're just making a table
				else
				{
					table& tbl	= parent->emplace<table>(key.segments.back()).first->second.ref_cast<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}
			}

			// if there was already a matching node some sanity checking is necessary;
			// this is ok if we're making an array and the existing element is already an array (new element)
			// or if we're making a table and the existing element is an implicitly-created table (promote it),
			// otherwise this is a redefinition error.
			else
			{
				if (is_arr && matching_node->is_array()
					&& impl::find(table_arrays.begin(), table_arrays.end(), &matching_node->ref_cast<array>()))
				{
					table& tbl	= matching_node->ref_cast<array>().emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				else if (!is_arr && matching_node->is_table() && !implicit_tables.empty())
				{
					table& tbl = matching_node->ref_cast<table>();
					if (auto found = impl::find(implicit_tables.begin(), implicit_tables.end(), &tbl);
						found && (tbl.empty() || tbl.is_homogeneous<table>()))
					{
						implicit_tables.erase(implicit_tables.cbegin() + (found - implicit_tables.data()));
						tbl.source_.begin = header_begin_pos;
						tbl.source_.end	  = header_end_pos;
						return &tbl;
					}
				}

				// if we get here it's a redefinition error.
				if (!is_arr && matching_node->type() == node_type::table)
				{
					set_error_at(header_begin_pos,
								 "cannot redefine existing table '"sv,
								 to_sv(recording_buffer),
								 "'"sv);
					return_after_error({});
				}
				else
				{
					set_error_at(header_begin_pos,
								 "cannot redefine existing "sv,
								 to_sv(matching_node->type()),
								 " '"sv,
								 to_sv(recording_buffer),
								 "' as "sv,
								 is_arr ? "array-of-tables"sv : "table"sv);
					return_after_error({});
				}
			}
		}

		void parse_key_value_pair_and_insert(table* tab)
		{
			return_if_error();
			assert_not_eof();
			push_parse_scope("key-value pair"sv);

			auto kvp = parse_key_value_pair();
			return_if_error();

			TOML_ASSERT(kvp.key.segments.size() >= 1u);

			// if it's a dotted kvp we need to spawn the sub-tables if necessary,
			// and set the target table to the second-to-last one in the chain
			if (kvp.key.segments.size() > 1u)
			{
				for (size_t i = 0; i < kvp.key.segments.size() - 1u; i++)
				{
					auto child = tab->get(kvp.key.segments[i]);
					if (!child)
					{
						child = tab->map_.emplace(std::move(kvp.key.segments[i]), new table{}).first->second.get();
						dotted_key_tables.push_back(&child->ref_cast<table>());
						child->source_ = kvp.value->source_;
					}
					else if (!child->is_table()
							 || !(impl::find(dotted_key_tables.begin(),
											 dotted_key_tables.end(),
											 &child->ref_cast<table>())
								  || impl::find(implicit_tables.begin(),
												implicit_tables.end(),
												&child->ref_cast<table>())))
						set_error_at(kvp.key.position,
									 "cannot redefine existing "sv,
									 to_sv(child->type()),
									 " as dotted key-value pair"sv);
					else
						child->source_.end = kvp.value->source_.end;

					return_if_error();
					tab = &child->ref_cast<table>();
				}
			}

			if (auto conflicting_node = tab->get(kvp.key.segments.back()))
			{
				if (conflicting_node->type() == kvp.value->type())
					set_error("cannot redefine existing "sv,
							  to_sv(conflicting_node->type()),
							  " '"sv,
							  to_sv(recording_buffer),
							  "'"sv);
				else
					set_error("cannot redefine existing "sv,
							  to_sv(conflicting_node->type()),
							  " '"sv,
							  to_sv(recording_buffer),
							  "' as "sv,
							  to_sv(kvp.value->type()));
			}

			return_if_error();
			tab->map_.emplace(std::move(kvp.key.segments.back()), std::unique_ptr<node>{ kvp.value.release() });
		}

		void parse_document()
		{
			assert_not_error();
			assert_not_eof();
			push_parse_scope("root table"sv);

			table* current_table = &root;

			do
			{
				return_if_error();

				// leading whitespace, line endings, comments
				if (consume_leading_whitespace() || consume_line_break() || consume_comment())
					continue;
				return_if_error();

				// [tables]
				// [[table array]]
				if (*cp == U'[')
					current_table = parse_table_header();

				// bare_keys
				// dotted.keys
				// "quoted keys"
				else if (is_bare_key_character(*cp) || is_string_delimiter(*cp))
				{
					push_parse_scope("key-value pair"sv);

					parse_key_value_pair_and_insert(current_table);

					// handle the rest of the line after the kvp
					// (this is not done in parse_key_value_pair() because that is also used for inline tables)
					consume_leading_whitespace();
					return_if_error();
					if (!is_eof() && !consume_comment() && !consume_line_break())
						set_error("expected a comment or whitespace, saw '"sv, to_sv(cp), "'"sv);
				}

				else // ??
					set_error("expected keys, tables, whitespace or comments, saw '"sv, to_sv(cp), "'"sv);
			}
			while (!is_eof());

			auto eof_pos	 = current_position(1);
			root.source_.end = eof_pos;
			if (current_table && current_table != &root && current_table->source_.end <= current_table->source_.begin)
				current_table->source_.end = eof_pos;
		}

		static void update_region_ends(node& nde) noexcept
		{
			const auto type = nde.type();
			if (type > node_type::array)
				return;

			if (type == node_type::table)
			{
				auto& tbl = nde.ref_cast<table>();
				if (tbl.inline_) // inline tables (and all their inline descendants) are already correctly terminated
					return;

				auto end = nde.source_.end;
				for (auto& [k, v] : tbl.map_)
				{
					(void)k;
					update_region_ends(*v);
					if (end < v->source_.end)
						end = v->source_.end;
				}
			}
			else // arrays
			{
				auto& arr = nde.ref_cast<array>();
				auto end  = nde.source_.end;
				for (auto& v : arr)
				{
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
				nde.source_.end = end;
			}
		}

	  public:
		parser(utf8_reader_interface&& reader_) //
			: reader{ reader_ }
		{
			root.source_ = { prev_pos, prev_pos, reader.source_path() };

			if (!reader.peek_eof())
			{
				cp = reader.read_next();

#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
#endif

				if (cp)
					parse_document();
			}

			update_region_ends(root);
		}

		TOML_NODISCARD
		operator parse_result() && noexcept
		{
#if TOML_EXCEPTIONS

			return { std::move(root) };

#else

			if (err)
				return parse_result{ *std::move(err) };
			else
				return parse_result{ std::move(root) };

#endif
		}
	};

	TOML_EXTERNAL_LINKAGE
	array* parser::parse_array()
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_eof({});

		node_ptr arr_ptr{ new array{} };
		array& arr = arr_ptr->ref_cast<array>();
		enum TOML_CLOSED_ENUM parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (!is_error())
		{
			while (consume_leading_whitespace() || consume_line_break() || consume_comment())
				continue;
			set_error_and_return_if_eof({});

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == val)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
					continue;
				}
				set_error_and_return_default("expected value or closing ']', saw comma"sv);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance_and_return_if_error({});
				break;
			}

			// must be a value
			else
			{
				if (prev == val)
				{
					set_error_and_return_default("expected comma or closing ']', saw '"sv, to_sv(*cp), "'"sv);
					continue;
				}
				prev = val;
				if (!arr.capacity())
					arr.reserve(4u);
				arr.elems_.emplace_back(parse_value());
			}
		}

		return_if_error({});
		return reinterpret_cast<array*>(arr_ptr.release());
	}

	TOML_EXTERNAL_LINKAGE
	table* parser::parse_inline_table()
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_eof({});

		node_ptr tbl_ptr{ new table{} };
		table& tbl	= tbl_ptr->ref_cast<table>();
		tbl.inline_ = true;
		enum TOML_CLOSED_ENUM parse_elem : int
		{
			none,
			comma,
			kvp
		};
		parse_elem prev = none;

		while (!is_error())
		{
			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
			{
				while (consume_leading_whitespace() || consume_line_break() || consume_comment())
					continue;
			}
			else
			{
				while (consume_leading_whitespace())
					continue;
			}
			return_if_error({});
			set_error_and_return_if_eof({});

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == kvp)
				{
					prev = comma;
					advance_and_return_if_error_or_eof({});
				}
				else
					set_error_and_return_default("expected key-value pair or closing '}', saw comma"sv);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				if constexpr (!TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
				{
					if (prev == comma)
					{
						set_error_and_return_default("expected key-value pair, saw closing '}' (dangling comma)"sv);
						continue;
					}
				}
				advance_and_return_if_error({});
				break;
			}

			// key-value pair
			else if (is_string_delimiter(*cp) || is_bare_key_character(*cp))
			{
				if (prev == kvp)
					set_error_and_return_default("expected comma or closing '}', saw '"sv, to_sv(*cp), "'"sv);
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(&tbl);
				}
			}

			/// ???
			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, to_sv(*cp), "'"sv);
		}

		return_if_error({});
		return reinterpret_cast<table*>(tbl_ptr.release());
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_IMPL_NAMESPACE_END;

#undef TOML_RETURNS_BY_THROWING
#undef advance_and_return_if_error
#undef advance_and_return_if_error_or_eof
#undef assert_or_assume
#undef assert_not_eof
#undef assert_not_error
#undef bdigit_msk
#undef bzero_msk
#undef is_eof
#undef is_error
#undef parse_error_break
#undef push_parse_scope
#undef push_parse_scope_1
#undef push_parse_scope_2
#undef return_after_error
#undef return_if_eof
#undef return_if_error
#undef return_if_error_or_eof
#undef set_error_and_return
#undef set_error_and_return_default
#undef set_error_and_return_if_eof
#undef signs_msk
#undef utf8_buffered_reader_error_check
#undef utf8_reader_error
#undef utf8_reader_error_check
#undef utf8_reader_return_after_error

//#---------------------------------------------------------------------------------------------------------------------
//# PARSER PUBLIC IMPLEMENTATION
//#---------------------------------------------------------------------------------------------------------------------

TOML_ANON_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse(utf8_reader_interface && reader)
	{
		return impl::parser{ std::move(reader) };
	}

	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse_file(std::string_view file_path)
	{
#if TOML_EXCEPTIONS
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	throw parse_error                                                                                                  \
	{                                                                                                                  \
		msg, source_position{}, std::make_shared<const std::string>(std::move(path))                                   \
	}
#else
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	return parse_result                                                                                                \
	{                                                                                                                  \
		parse_error                                                                                                    \
		{                                                                                                              \
			msg, source_position{}, std::make_shared<const std::string>(std::move(path))                               \
		}                                                                                                              \
	}
#endif

		std::string file_path_str(file_path);

		// open file with a custom-sized stack buffer
		std::ifstream file;
		alignas(32) char file_buffer[sizeof(void*) * 1024u];
		file.rdbuf()->pubsetbuf(file_buffer, sizeof(file_buffer));
		file.open(file_path_str, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
		if (!file.is_open())
			TOML_PARSE_FILE_ERROR("File could not be opened for reading", file_path_str);

		// get size
		const auto file_size = file.tellg();
		if (file_size == -1)
			TOML_PARSE_FILE_ERROR("Could not determine file size", file_path_str);
		file.seekg(0, std::ifstream::beg);

		// read the whole file into memory first if the file isn't too large
		constexpr auto large_file_threshold = 1024 * 1024 * 2; // 2 MB
		if (file_size <= large_file_threshold)
		{
			std::vector<char> file_data;
			file_data.resize(static_cast<size_t>(file_size));
			file.read(file_data.data(), static_cast<std::streamsize>(file_size));
			return parse(std::string_view{ file_data.data(), file_data.size() }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		else
			return parse(file, std::move(file_path_str));

#undef TOML_PARSE_FILE_ERROR
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::string_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(file_path);
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::u8string_view file_path)
	{
		std::string file_path_str;
		file_path_str.resize(file_path.length());
		memcpy(file_path_str.data(), file_path.data(), file_path.length());
		return TOML_ANON_NAMESPACE::do_parse_file(file_path_str);
	}

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::istream & doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::wstring_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(impl::narrow(file_path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::utf8_reader{ doc, impl::narrow(source_path) });
	}

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif // TOML_ENABLE_PARSER
