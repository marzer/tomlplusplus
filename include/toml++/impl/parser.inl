//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.hpp"
//# {{
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}
#if TOML_ENABLE_PARSER

#include "parser.hpp"
#include "std_optional.hpp"
#include "source_region.hpp"
#include "parse_error.hpp"
#include "date_time.hpp"
#include "value.hpp"
#include "array.hpp"
#include "table.hpp"
#include "unicode.hpp"
#include "simd.hpp"
TOML_DISABLE_WARNINGS;
#include <istream>
#include <fstream>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
#include <charconv>
#endif
#if !TOML_INT_CHARCONV
#include <sstream>
#include <iomanip>
#endif
#include <cstdlib>
#include <exception>
#if defined(__GLIBCXX__)
#include <cxxabi.h>
#endif
TOML_ENABLE_WARNINGS;
#include "header_start.hpp"
TOML_DISABLE_MSVC_WARNING(4702);
TOML_DISABLE_CLANG_WARNING("-Wnan-infinity-disabled");

//#---------------------------------------------------------------------------------------------------------------------
//# UTF8 STREAMS
//#---------------------------------------------------------------------------------------------------------------------

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	class utf8_byte_stream
	{
	  private:
		std::istream* source_ = {};
		// bytes read for the bom check, kept because seeking back to reread them fails on pipes
		char prefix_[3]		 = {};
		size_t prefix_begin_ = {};
		size_t prefix_end_	 = {};
		bool started_		 = {};
#if TOML_COMPILER_HAS_EXCEPTIONS
		// rethrown where the error for a failed read would be reported
		mutable std::exception_ptr failure_;
#endif

		TOML_PURE_INLINE_GETTER
		bool has_prefix() const noexcept
		{
			return prefix_begin_ < prefix_end_;
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		TOML_PURE_INLINE_GETTER
		bool reached_end() const noexcept
		{
			return (source_->rdstate() & (std::istream::badbit | std::istream::eofbit)) == std::istream::eofbit;
		}

#endif

		TOML_NODISCARD
		size_t read(char* dest, size_t num) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			try
			{
				source_->read(dest, static_cast<std::streamsize>(num));
			}
#if defined(__GLIBCXX__)
			// glibc cancels threads by unwinding with this, and aborts the process if it's swallowed
			catch (abi::__forced_unwind&)
			{
				throw;
			}
#endif
			catch (...)
			{
				if (!failure_ && !reached_end())
					failure_ = std::current_exception();
			}
#else
			source_->read(dest, static_cast<std::streamsize>(num));
#endif
			return static_cast<size_t>(source_->gcount());
		}

		TOML_NODISCARD
		std::istream::int_type peek() const noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			try
			{
				return source_->peek();
			}
#if defined(__GLIBCXX__)
			catch (abi::__forced_unwind&)
			{
				throw;
			}
#endif
			catch (...)
			{
				if (!failure_ && !reached_end())
					failure_ = std::current_exception();
				return std::istream::traits_type::eof();
			}
#else
			return source_->peek();
#endif
		}

		// in_avail() can call the streambuf's showmanyc(), which may throw
		TOML_NODISCARD
		bool available(std::streamsize& held) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			try
			{
				held = source_->rdbuf()->in_avail();
				return true;
			}
#if defined(__GLIBCXX__)
			catch (abi::__forced_unwind&)
			{
				throw;
			}
#endif
			catch (...)
			{
				if (!failure_)
					failure_ = std::current_exception();
				return false;
			}
#else
			held = source_->rdbuf()->in_avail();
			return true;
#endif
		}

		// istream::read keeps nothing of a read the streambuf throws in, so this takes only what the stream holds,
		// or the rest of the block from one that holds nothing even after a peek
		TOML_NODISCARD
		size_t take(char* dest, size_t num, size_t block) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
#if TOML_COMPILER_HAS_EXCEPTIONS
			if (failure_)
				return {};
#endif
			if (source_->good())
			{
				std::streamsize held = {};
				if (!available(held))
					return {};
				if (held <= 0)
				{
					// msvc's peek returns 0 when the streambuf throws
					if (peek() == std::istream::traits_type::eof() || !source_->good() || !available(held))
						return {};
				}
				const auto limit = held > 0 ? held : static_cast<std::streamsize>(block);
				num				 = static_cast<size_t>(impl::min(static_cast<std::streamsize>(num), limit));
			}
			return read(dest, num);
		}

	  public:
		TOML_NODISCARD_CTOR
		utf8_byte_stream() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit utf8_byte_stream(std::istream& stream) noexcept //
			: source_{ &stream }
		{}

		TOML_PURE_INLINE_GETTER
		bool error() const noexcept
		{
			return !!(source_->rdstate() & std::istream::badbit);
		}

		TOML_PURE_INLINE_GETTER
		bool eof() const noexcept
		{
			return started_ && !has_prefix() && !!(source_->rdstate() & std::istream::eofbit);
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		void rethrow_failure() const
		{
			if (failure_)
				std::rethrow_exception(failure_);
		}

#endif

		TOML_NODISCARD
		TOML_ATTR(nonnull)
		size_t operator()(char* dest, size_t num, uint64_t offset) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!started_)
			{
				started_   = true;
				size_t got = {};
				do
				{
					got = take(prefix_ + prefix_end_, 3u - prefix_end_, 3u - prefix_end_);
					prefix_end_ += got;
				}
				while (got && prefix_end_ < 3u && source_->good());
				if (prefix_end_ == 3u && std::memcmp(utf8_byte_order_mark.data(), prefix_, 3u) == 0)
					prefix_end_ = 0u;
			}

			if (has_prefix())
			{
				const auto count = impl::min(num, prefix_end_ - prefix_begin_);
				std::memcpy(dest, prefix_ + prefix_begin_, count);
				prefix_begin_ += count;
				return count;
			}

			if (source_->rdstate() & (std::istream::badbit | std::istream::eofbit))
				return {};
			return take(dest, num, 32u - static_cast<size_t>(offset % 32u));
		}
	};

	struct utf8_codepoint
	{
		char32_t value;
		char bytes[4];
		size_t count;

		TOML_PURE_INLINE_GETTER
		constexpr operator const char32_t&() const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivially_default_constructible_v<utf8_codepoint>
				  && std::is_trivially_copyable_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	struct lookahead_pos
	{
		uint64_t offset;
		size_t count;
		char32_t value;
	};

	TOML_INTERNAL_LINKAGE
	constexpr char32_t eof_mark = static_cast<char32_t>(0x110000u);

	TOML_INTERNAL_LINKAGE
	constexpr char32_t wall_mark = static_cast<char32_t>(0x110001u);

	enum class TOML_CLOSED_ENUM wall_kind : unsigned char
	{
		invalid,
		cut_sequence,
		zero_bytes,
		io_error
	};

	enum class TOML_CLOSED_FLAGS_ENUM run_class : unsigned char
	{
		comment		   = 1,
		basic_string   = 2,
		literal_string = 4,
		bare_key	   = 8
	};

	struct run_table
	{
		unsigned char classes[256];
	};

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr run_table make_run_table() noexcept
	{
		run_table table{};
		for (unsigned i = 0; i < 0x80u; i++)
		{
			const auto c	 = static_cast<char32_t>(i);
			unsigned classes = {};
			if (!impl::is_ascii_vertical_whitespace(c) && !impl::is_nontab_control_character(c))
			{
				classes |= static_cast<unsigned>(run_class::comment);
				if (c != U'"' && c != U'\\')
					classes |= static_cast<unsigned>(run_class::basic_string);
				if (c != U'\'')
					classes |= static_cast<unsigned>(run_class::literal_string);
			}
			if (impl::is_ascii_bare_key_character(c))
				classes |= static_cast<unsigned>(run_class::bare_key);
			table.classes[i] = static_cast<unsigned char>(classes);
		}
		for (unsigned i = 0x80u; i < 0x100u; i++)
			table.classes[i] = static_cast<unsigned char>(static_cast<unsigned>(run_class::comment)
														  | static_cast<unsigned>(run_class::basic_string)
														  | static_cast<unsigned>(run_class::literal_string));
		return table;
	}

	TOML_INTERNAL_LINKAGE
	constexpr run_table run_bytes = make_run_table();

	TOML_INTERNAL_LINKAGE
	constexpr uint64_t word_high_bits = 0x8080808080808080ull;

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t word_of(uint64_t byte) noexcept
	{
		return 0x0101010101010101ull * byte;
	}

	// the first byte is the lowest, whatever the byte order
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	uint64_t load_word(const char* bytes) noexcept
	{
		uint64_t word = {};
		std::memcpy(&word, bytes, 8u);
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		word = __builtin_bswap64(word);
#endif
		return word;
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	unsigned trailing_zeros(uint64_t bits) noexcept
	{
#if TOML_GCC_LIKE
		return static_cast<unsigned>(__builtin_ctzll(bits));
#else
		static constexpr unsigned char positions[64] = {
			0,	1,	48, 2,	57, 49, 28, 3,	61, 58, 50, 42, 38, 29, 17, 4,	62, 55, 59, 36, 53, 51,
			43, 22, 45, 39, 33, 30, 24, 18, 12, 5,	63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21,
			44, 32, 23, 11, 46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,	13, 8,	7,	6
		};
		return positions[((bits & (~bits + 1u)) * 0x03F79D71B4CB0A89ull) >> 58];
#endif
	}

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr size_t count_high_bits(uint64_t bits) noexcept
	{
		return static_cast<size_t>(((bits >> 7) * word_of(1u)) >> 56);
	}

	// for words of ascii bytes: the bytes equal to c, as their high bits
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t bytes_equal(uint64_t ascii, unsigned c) noexcept
	{
		return ~((ascii ^ word_of(c)) + word_of(0x7Fu)) & word_high_bits;
	}

	// the bytes of a word that end a comment or string run, as their high bits
	template <run_class Class>
	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr uint64_t run_stops(uint64_t word) noexcept
	{
		const auto ascii = word & ~word_high_bits;
		auto delimiters	 = bytes_equal(ascii, 0x7Fu);
		if constexpr (Class == run_class::basic_string)
			delimiters |= bytes_equal(ascii, '"') | bytes_equal(ascii, '\\');
		if constexpr (Class == run_class::literal_string)
			delimiters |= bytes_equal(ascii, '\'');
		const auto controls = ~((ascii + word_of(0x60u)) | word | bytes_equal(ascii, '\t')) & word_high_bits;
		return controls | (delimiters & ~word);
	}

#if TOML_HAS_SSE2

	TOML_CONST_GETTER
	TOML_INTERNAL_LINKAGE
	constexpr unsigned count_bits(unsigned bits) noexcept
	{
		bits = bits - ((bits >> 1) & 0x55555555u);
		bits = (bits & 0x33333333u) + ((bits >> 2) & 0x33333333u);
		return (((bits + (bits >> 4)) & 0x0F0F0F0Fu) * 0x01010101u) >> 24;
	}

	// the bytes of a block that end a comment or string run, as a movemask
	template <run_class Class>
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	unsigned run_stops(__m128i bytes) noexcept
	{
		const auto equal = [&](char c) noexcept { return _mm_cmpeq_epi8(bytes, _mm_set1_epi8(c)); };

		// bytes from 0x80 compare as negative
		const auto controls =
			_mm_andnot_si128(equal('\t'),
							 _mm_and_si128(_mm_cmpgt_epi8(bytes, _mm_set1_epi8(static_cast<char>(-1))),
										   _mm_cmplt_epi8(bytes, _mm_set1_epi8(0x20))));
		auto stops = _mm_or_si128(controls, equal(0x7F));
		if constexpr (Class == run_class::basic_string)
			stops = _mm_or_si128(stops, _mm_or_si128(equal('"'), equal('\\')));
		if constexpr (Class == run_class::literal_string)
			stops = _mm_or_si128(stops, equal('\''));
		return static_cast<unsigned>(_mm_movemask_epi8(stops));
	}

#endif

	// how many ascii bytes the first n start with
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	size_t ascii_length(const char* bytes, size_t n) noexcept
	{
		size_t i = {};
#if TOML_HAS_SSE2
		for (; n - i >= 16u; i += 16u)
			if (const auto high = static_cast<unsigned>(
					_mm_movemask_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i*>(bytes + i)))))
				return i + trailing_zeros(high);
#endif
		while (n - i >= 8u && !(load_word(bytes + i) & word_high_bits))
			i += 8u;
		while (i < n && static_cast<unsigned char>(bytes[i]) < 0x80u)
			i++;
		return i;
	}

	struct run_span
	{
		const char* data;
		size_t length;
		size_t continuation;
	};

	class source_reader
	{
	  private:
		static constexpr uint64_t no_wall	   = static_cast<uint64_t>(-1);
		static constexpr size_t string_chunk   = 4096;
		static constexpr size_t initial_window = 256;

		// pos_, valid_ and filled_ index data_; base_, line_start_ and wall_ are offsets into the document, 64-bit
		// because a stream can outgrow size_t
		utf8_codepoint current_ = {};
		const char* data_		= {};
		size_t pos_				= {};
		size_t valid_			= {};
		size_t filled_			= {};
		uint64_t base_			= {};
		uint64_t line_start_	= {};
		size_t extra_			= {};
		uint64_t wall_			= no_wall;
		source_index line_		= 1;
		source_index lf_column_ = {};
		wall_kind wall_kind_	= {};
		bool ended_				= {};
		utf8_byte_stream stream_;
		std::unique_ptr<char[]> window_;
		size_t capacity_ = {};
		source_path_ptr source_path_;

		template <typename String>
		TOML_NODISCARD
		static source_path_ptr make_source_path(String&& source_path)
		{
			if (source_path.empty())
				return {};
			return std::make_shared<const std::string>(static_cast<String&&>(source_path));
		}

		void set_wall(wall_kind kind) noexcept
		{
			wall_	   = base_ + valid_;
			wall_kind_ = kind;
		}

		void validate(size_t limit) noexcept
		{
			auto i = valid_;
			while (i < limit)
			{
				i += ascii_length(data_ + i, limit - i);
				if (i == limit)
					break;

				impl::utf8_decoder decoder;
				auto end = i;
				do
				{
					if (end == filled_)
					{
						valid_ = i;
						return;
					}
					decoder(data_[end++]);
					if (decoder.error())
					{
						valid_ = i;
						set_wall(wall_kind::invalid);
						return;
					}
				}
				while (!decoder.has_code_point());
				i = end;
			}
			valid_ = i;
		}

		void make_room()
		{
			const auto live = filled_ - pos_;
			auto capacity	= capacity_;
			while (live > capacity / 2u)
				capacity *= 2u;
			if (!pos_ && capacity == capacity_)
				return;

#ifdef NDEBUG
			const bool move = capacity != capacity_;
#else
			// moving the window on every compaction lets asan catch a pointer kept into the old one
			const bool move = true;
#endif
			if (move)
			{
				std::unique_ptr<char[]> window{ new char[capacity] };
				std::memcpy(window.get(), window_.get() + pos_, live);
				window_	  = std::move(window);
				capacity_ = capacity;
			}
			else
				std::memmove(window_.get(), window_.get() + pos_, live);

			data_ = window_.get();
			base_ += pos_;
			valid_ -= pos_;
			filled_ -= pos_;
			pos_ = {};
		}

		TOML_NEVER_INLINE
		void extend() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!window_)
			{
				const auto limit = filled_ - valid_ > string_chunk ? valid_ + string_chunk : filled_;
				validate(limit);
				if (wall_ == no_wall && valid_ < limit)
					set_wall(wall_kind::cut_sequence);
				return;
			}

			make_room();
			TOML_ASSERT_ASSUME(filled_ < capacity_);
			const auto got = stream_(window_.get() + filled_, capacity_ - filled_, base_ + filled_);
			filled_ += got;
			validate(filled_);
			if (wall_ != no_wall)
				return;
			if (got)
			{
				if (stream_.error())
					set_wall(wall_kind::io_error);
			}
			else if (!stream_.eof())
				set_wall(base_ + filled_ ? wall_kind::io_error : wall_kind::zero_bytes);
			else if (valid_ < filled_)
				set_wall(wall_kind::cut_sequence);
			else
				ended_ = true;
		}

		TOML_NODISCARD
		bool ensure(uint64_t at) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			while (at >= base_ + valid_)
			{
				if (at >= wall_ || (ended_ && valid_ == filled_))
					return false;
				extend();
			}
			return true;
		}

		TOML_NODISCARD
		size_t decode(size_t i, char32_t& value) const noexcept
		{
			const auto lead = static_cast<unsigned char>(data_[i]);
			if (lead < 0x80u)
			{
				value = lead;
				return 1u;
			}
			const size_t count = lead < 0xE0u ? 2u : (lead < 0xF0u ? 3u : 4u);
			uint_least32_t cp  = lead & (0x7Fu >> count);
			for (size_t k = 1; k < count; k++)
				cp = (cp << 6) | (static_cast<unsigned char>(data_[i + k]) & 0x3Fu);
			value = static_cast<char32_t>(cp);
			return count;
		}

		TOML_NODISCARD
		const utf8_codepoint* land() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (pos_ < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[pos_]);
				if TOML_LIKELY(byte < 0x80u)
				{
					current_.value	  = byte;
					current_.bytes[0] = data_[pos_];
					current_.count	  = 1u;
					return &current_;
				}
			}
			return land_slow();
		}

		TOML_NEVER_INLINE
		const utf8_codepoint* land_slow() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!ensure(base_ + pos_))
				return nullptr;

			current_.count = decode(pos_, current_.value);
			std::memcpy(current_.bytes, data_ + pos_, current_.count);
			return &current_;
		}

		TOML_NEVER_INLINE
		void step_slow(lookahead_pos& la) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (!ensure(la.offset))
			{
				la.value = la.offset == wall_ ? wall_mark : eof_mark;
				la.count = {};
				return;
			}
			la.count = decode(static_cast<size_t>(la.offset - base_), la.value);
		}

	  public:
		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::string_view doc, String&& source_path) //
			: data_{ doc.data() },
			  filled_{ doc.length() },
			  ended_{ true },
			  source_path_{ make_source_path(static_cast<String&&>(source_path)) }
		{
			if (filled_ >= 3u && std::memcmp(utf8_byte_order_mark.data(), data_, 3u) == 0)
			{
				data_ += 3u;
				filled_ -= 3u;
			}
		}

#if TOML_HAS_CHAR8

		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::u8string_view doc, String&& source_path) //
			: source_reader{ std::string_view{ reinterpret_cast<const char*>(doc.data()), doc.length() },
							 static_cast<String&&>(source_path) }
		{}

#endif

		template <typename String>
		TOML_NODISCARD_CTOR
		source_reader(std::istream& doc, String&& source_path) //
			: stream_{ doc },
			  window_{ new char[initial_window] },
			  capacity_{ initial_window },
			  source_path_{ make_source_path(static_cast<String&&>(source_path)) }
		{
			data_ = window_.get();
		}

		TOML_PURE_INLINE_GETTER
		const source_path_ptr& source_path() const noexcept
		{
			return source_path_;
		}

		// the wall is the first byte that can't be read: invalid utf-8, a cut sequence or a failed read
		TOML_PURE_INLINE_GETTER
		bool at_wall() const noexcept
		{
			return base_ + pos_ == wall_;
		}

		TOML_PURE_GETTER
		const char* wall_reason() const noexcept
		{
			TOML_ASSERT(at_wall());

			if (wall_kind_ == wall_kind::invalid)
				return "Encountered invalid utf-8 sequence";
			if (wall_kind_ == wall_kind::cut_sequence)
				return "Encountered EOF during incomplete utf-8 code point sequence";
			if (wall_kind_ == wall_kind::zero_bytes)
				return "Reading from the underlying stream failed - zero bytes read";
			return "An I/O error occurred while reading from the underlying stream";
		}

#if TOML_COMPILER_HAS_EXCEPTIONS

		void rethrow_stream_failure() const
		{
			if (wall_kind_ == wall_kind::zero_bytes || wall_kind_ == wall_kind::io_error)
				stream_.rethrow_failure();
		}

#endif

		TOML_PURE_INLINE_GETTER
		source_position position() const noexcept
		{
			return { line_, static_cast<source_index>(base_ + pos_ - line_start_ - extra_ + 1u) };
		}

		TOML_PURE_GETTER
		source_position eof_anchor() const noexcept
		{
			const auto at = base_ + pos_;
			if (!at)
				return { 1, 1 };
			if (at == line_start_)
				return { line_ - 1u, lf_column_ };
			return { line_, static_cast<source_index>(at - line_start_ - extra_) };
		}

		TOML_PURE_INLINE_GETTER
		lookahead_pos lookahead() const noexcept
		{
			return { base_ + pos_, current_.count, current_.value };
		}

		TOML_NODISCARD
		const utf8_codepoint* first() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			return land_slow();
		}

		TOML_NODISCARD
		const utf8_codepoint* next() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			if (current_.value == U'\n')
			{
				lf_column_ = position().column;
				line_++;
				line_start_ = base_ + pos_ + 1u;
				extra_		= {};
			}
			else
				extra_ += current_.count - 1u;
			pos_ += current_.count;
			return land();
		}

		template <run_class Class>
		TOML_NODISCARD
		run_span run() const noexcept
		{
			static constexpr auto mask		= static_cast<unsigned>(Class);
			static constexpr bool non_ascii = (run_bytes.classes[0x80u] & mask) != 0u;
			TOML_ASSERT(pos_ < valid_);

			auto end			= pos_ + current_.count;
			size_t continuation = current_.count - 1u;
			if constexpr (non_ascii)
			{
#if TOML_HAS_SSE2
				while (valid_ - end >= 16u)
				{
					const auto bytes   = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data_ + end));
					const auto stops   = run_stops<Class>(bytes);
					auto continuations = static_cast<unsigned>(
						_mm_movemask_epi8(_mm_cmplt_epi8(bytes, _mm_set1_epi8(static_cast<char>(-64)))));
					if (stops)
					{
						continuations &= (stops & (~stops + 1u)) - 1u;
						return { data_ + pos_,
								 end + trailing_zeros(stops) - pos_,
								 continuation + count_bits(continuations) };
					}
					if (continuations)
						continuation += count_bits(continuations);
					end += 16u;
				}
#endif
				while (valid_ - end >= 8u)
				{
					const auto word	   = load_word(data_ + end);
					const auto stops   = run_stops<Class>(word);
					auto continuations = word & ~(word << 1) & word_high_bits;
					if (stops)
					{
						continuations &= (stops & (~stops + 1u)) - 1u;
						return { data_ + pos_,
								 end + (trailing_zeros(stops) / 8u) - pos_,
								 continuation + count_high_bits(continuations) };
					}
					continuation += count_high_bits(continuations);
					end += 8u;
				}
			}
			while (end < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[end]);
				if (!(run_bytes.classes[byte] & mask))
					break;
				continuation += non_ascii && (byte & 0xC0u) == 0x80u;
				end++;
			}
			return { data_ + pos_, end - pos_, continuation };
		}

		TOML_NODISCARD
		const utf8_codepoint* skip(const run_span& span) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			TOML_ASSERT(span.data == data_ + pos_);
			TOML_ASSERT(span.length >= current_.count);
#ifndef NDEBUG
			size_t continuation = {};
			for (size_t i = pos_, end = pos_ + span.length; i < end;)
			{
				char32_t value	 = {};
				const auto count = decode(i, value);
				TOML_ASSERT(value != U'\n');
				continuation += count - 1u;
				i += count;
				TOML_ASSERT(i <= end);
			}
			TOML_ASSERT(continuation == span.continuation);
#endif
			extra_ += span.continuation;
			pos_ += span.length;
			return land();
		}

		void step(lookahead_pos& la) noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
		{
			la.offset += la.count;
			const auto at = static_cast<size_t>(la.offset - base_);
			if (at < valid_)
			{
				const auto byte = static_cast<unsigned char>(data_[at]);
				if (byte < 0x80u)
				{
					la.value = byte;
					la.count = 1u;
					return;
				}
			}
			step_slow(la);
		}
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
		static constexpr auto digit_name	   = "binary digit"sv;
		static constexpr auto is_digit		   = impl::is_binary_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 63;
		static constexpr auto prefix_codepoint = U'b';
		static constexpr auto prefix		   = "b"sv;
		static constexpr auto full_prefix	   = "0b"sv;
	};
	template <>
	struct parse_integer_traits<8>
	{
		static constexpr auto scope_qualifier  = "octal integer"sv;
		static constexpr auto digit_name	   = "octal digit"sv;
		static constexpr auto is_digit		   = impl::is_octal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 21; // strlen("777777777777777777777")
		static constexpr auto prefix_codepoint = U'o';
		static constexpr auto prefix		   = "o"sv;
		static constexpr auto full_prefix	   = "0o"sv;
	};
	template <>
	struct parse_integer_traits<10>
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto digit_name	  = "digit"sv;
		static constexpr auto is_digit		  = impl::is_decimal_digit;
		static constexpr auto is_signed		  = true;
		static constexpr auto max_digits	  = 19; // strlen("9223372036854775807")
		static constexpr auto full_prefix	  = ""sv;
	};
	template <>
	struct parse_integer_traits<16>
	{
		static constexpr auto scope_qualifier  = "hexadecimal integer"sv;
		static constexpr auto digit_name	   = "hexadecimal digit"sv;
		static constexpr auto is_digit		   = impl::is_hexadecimal_digit;
		static constexpr auto is_signed		   = false;
		static constexpr auto max_digits	   = 16; // strlen("7FFFFFFFFFFFFFFF")
		static constexpr auto prefix_codepoint = U'x';
		static constexpr auto prefix		   = "x"sv;
		static constexpr auto full_prefix	   = "0x"sv;
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

	// code points would otherwise convert to bool
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint&) = delete;
	TOML_INTERNAL_LINKAGE
	std::string_view to_sv(const utf8_codepoint*) = delete;

	template <typename T>
	TOML_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, char* const buf_end, const T& arg) noexcept
	{
		if TOML_UNLIKELY(write_pos >= buf_end)
			return;

		using arg_type = impl::remove_cvref<T>;

		// string views
		if constexpr (std::is_same_v<arg_type, std::string_view>)
		{
			const auto max_chars = static_cast<size_t>(buf_end - write_pos);
			const auto len		 = max_chars < arg.length() ? max_chars : arg.length();
			std::memcpy(write_pos, arg.data(), len);
			write_pos += len;
		}

		// 64-bit integers
		else if constexpr (impl::is_one_of<arg_type, int64_t, uint64_t>)
		{
#if TOML_INT_CHARCONV
			const auto result = std::to_chars(write_pos, buf_end, arg);
			write_pos		  = result.ptr;
#else
			std::ostringstream ss;
			ss.imbue(std::locale::classic());
			using cast_type = std::conditional_t<std::is_signed_v<arg_type>, int64_t, uint64_t>;
			ss << static_cast<cast_type>(arg);
			concatenate(write_pos, buf_end, to_sv(std::move(ss).str()));
#endif
		}

		// code points
		else if constexpr (std::is_same_v<arg_type, utf8_codepoint>)
		{
			if (arg.value <= U'\x1F')
				concatenate(write_pos, buf_end, impl::control_char_escapes[arg.value]);
			else if (arg.value == U'\x7F')
				concatenate(write_pos, buf_end, "\\u007F"sv);
			else if (arg.value <= U'\x7F'
					 || (arg.value > U'\x9F' && !impl::is_whitespace(arg.value)
						 && !impl::is_default_ignorable(arg.value)))
				concatenate(write_pos, buf_end, std::string_view{ arg.bytes, arg.count });
			else
			{
				auto val			= static_cast<uint_least32_t>(arg.value);
				const auto digits	= val > 0xFFFFu ? 8u : 4u;
				constexpr auto mask = uint_least32_t{ 0xFu };
				char buf[10]		= { '\\', digits > 4 ? 'U' : 'u' };
				for (auto i = 2u + digits; i-- > 2u;)
				{
					const auto hexdig = val & mask;
					buf[i]			  = static_cast<char>(hexdig >= 0xAu ? ('A' + (hexdig - 0xAu)) : ('0' + hexdig));
					val >>= 4;
				}
				concatenate(write_pos, buf_end, std::string_view{ buf, digits + 2u });
			}
		}

		else
		{
			static_assert(impl::always_false<T>,
						  "concatenate() inputs are limited to std::string_views, 64-bit integers and code points");
		}
	}

	struct error_arg
	{
		enum class kinds : unsigned char
		{
			string,
			signed_integer,
			unsigned_integer,
			codepoint
		};

		const void* ptr = {};
		uint64_t val	= {};
		kinds kind		= {};
	};

	template <typename T>
	TOML_PURE_GETTER
	TOML_INTERNAL_LINKAGE
	error_arg make_error_arg(const T& arg) noexcept
	{
		using arg_type = impl::remove_cvref<T>;
		if constexpr (std::is_same_v<arg_type, std::string_view>)
			return { arg.data(), static_cast<uint64_t>(arg.length()), error_arg::kinds::string };
		else if constexpr (std::is_same_v<arg_type, utf8_codepoint>)
			return { &arg, {}, error_arg::kinds::codepoint };
		else if constexpr (std::is_same_v<arg_type, const utf8_codepoint*>)
			return { arg, {}, error_arg::kinds::codepoint };
		else if constexpr (std::is_integral_v<arg_type> && std::is_signed_v<arg_type>)
			return { {}, static_cast<uint64_t>(static_cast<int64_t>(arg)), error_arg::kinds::signed_integer };
		else if constexpr (std::is_integral_v<arg_type>)
			return { {}, static_cast<uint64_t>(arg), error_arg::kinds::unsigned_integer };
		else
			static_assert(impl::always_false<T>,
						  "error inputs are limited to std::string_views, integers and code points");
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

		void append(const error_arg& arg) noexcept
		{
			switch (arg.kind)
			{
				case error_arg::kinds::string:
					concatenate(write_pos,
								max_write_pos,
								std::string_view{ static_cast<const char*>(arg.ptr), static_cast<size_t>(arg.val) });
					break;
				case error_arg::kinds::signed_integer:
					concatenate(write_pos, max_write_pos, static_cast<int64_t>(arg.val));
					break;
				case error_arg::kinds::unsigned_integer: concatenate(write_pos, max_write_pos, arg.val); break;
				case error_arg::kinds::codepoint:
					if (arg.ptr)
						concatenate(write_pos, max_write_pos, *static_cast<const utf8_codepoint*>(arg.ptr));
					break;
			}
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

		TOML_DELETE_DEFAULTS(error_builder);
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

		TOML_DELETE_DEFAULTS(parse_scope);
	};
#define push_parse_scope_2(scope, line) const parse_scope ps_##line(current_scope, scope)
#define push_parse_scope_1(scope, line) push_parse_scope_2(scope, line)
#define push_parse_scope(scope)			push_parse_scope_1(scope, __LINE__)

	struct parse_key_buffer
	{
		std::string buffer;
		std::vector<std::pair<size_t, size_t>> segments;
		std::vector<source_position> starts;
		std::vector<source_position> ends;

		void clear() noexcept
		{
			buffer.clear();
			segments.clear();
			starts.clear();
			ends.clear();
		}

		void push_back(std::string_view segment, source_position b, source_position e)
		{
			segments.emplace_back(buffer.length(), segment.length());
			buffer.append(segment);
			starts.push_back(b);
			ends.push_back(e);
		}

		TOML_PURE_INLINE_GETTER
		std::string_view operator[](size_t i) const noexcept
		{
			return std::string_view{ buffer.c_str() + segments[i].first, segments[i].second };
		}

		TOML_PURE_INLINE_GETTER
		std::string_view back() const noexcept
		{
			return (*this)[segments.size() - 1u];
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return segments.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return segments.size();
		}
	};

	struct depth_counter_scope
	{
		size_t& depth_;
		size_t previous_;

		TOML_NODISCARD_CTOR
		explicit depth_counter_scope(size_t& depth) noexcept //
			: depth_{ depth },
			  previous_{ depth }
		{
			depth_++;
		}

		// restores instead of decrementing, so msvc destroying it twice when built as a module is harmless (#288)
		~depth_counter_scope() noexcept
		{
			depth_ = previous_;
		}

		TOML_DELETE_DEFAULTS(depth_counter_scope);
	};

	struct parsed_string
	{
		std::string_view value;
		bool was_multi_line;
	};

	class node_set
	{
		static constexpr size_t max_pending	   = 16;
		static constexpr unsigned initial_bits = 6;

		std::vector<const node*> pending_;
		std::vector<const node*> slots_;
		size_t count_  = {};
		unsigned bits_ = {};

		TOML_PURE_INLINE_GETTER
		size_t home(const node* n) const noexcept
		{
			const auto address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(n));
			return static_cast<size_t>((address * 0x9E3779B97F4A7C15ull) >> (64u - bits_));
		}

		TOML_PURE_GETTER
		size_t slot_of(const node* n) const noexcept
		{
			const auto mask = slots_.size() - 1u;
			auto i			= home(n);
			while (slots_[i] && slots_[i] != n)
				i = (i + 1u) & mask;
			return i;
		}

		void flush()
		{
			auto bits = slots_.empty() ? initial_bits : bits_;
			while ((count_ + pending_.size()) * 2u > (size_t{ 1 } << bits))
				bits++;
			if (bits != bits_)
			{
				std::vector<const node*> old(size_t{ 1 } << bits);
				old.swap(slots_);
				bits_ = bits;
				for (auto n : old)
					if (n)
						slots_[slot_of(n)] = n;
			}

			for (auto n : pending_)
			{
				auto& slot = slots_[slot_of(n)];
				if (!slot)
					count_++;
				slot = n;
			}
			pending_.clear();
		}

	  public:
		void insert(const node* n)
		{
			TOML_ASSERT(n);
			pending_.push_back(n);
		}

		TOML_NODISCARD
		bool contains(const node* n)
		{
			TOML_ASSERT(n);
			if (pending_.size() > max_pending)
				flush();
			for (auto p : pending_)
				if (p == n)
					return true;
			return !slots_.empty() && slots_[slot_of(n)] == n;
		}

		void erase(const node* n) noexcept
		{
			for (auto& p : pending_)
			{
				if (p == n)
				{
					p = pending_.back();
					pending_.pop_back();
					return;
				}
			}
			if (slots_.empty())
				return;
			auto hole = slot_of(n);
			if (slots_[hole] != n)
				return;

			// entries after the hole move back into it unless that would put them before their home slot
			const auto mask = slots_.size() - 1u;
			for (auto i = (hole + 1u) & mask; slots_[i]; i = (i + 1u) & mask)
			{
				if (((i - home(slots_[i])) & mask) >= ((i - hole) & mask))
				{
					slots_[hole] = slots_[i];
					hole		 = i;
				}
			}
			slots_[hole] = nullptr;
			count_--;
		}
	};

	template <size_t N>
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	bool decimal_to_double(const char (&chars)[N], size_t length, double& result) noexcept
	{
		// strtod takes its decimal point from setlocale(), so the digits go in without one
		char text[N + 16];
		size_t text_length = {};
		int64_t exponent   = {};
		bool in_fraction   = {};
		size_t i		   = {};
		for (; i < length && chars[i] != 'e' && chars[i] != 'E'; i++)
		{
			if (chars[i] == '.')
				in_fraction = true;
			else
			{
				text[text_length++] = chars[i];
				if (in_fraction)
					exponent--;
			}
		}

		if (i < length)
		{
			i++;
			const bool negative = chars[i] == '-';
			if (chars[i] == '+' || chars[i] == '-')
				i++;
			int64_t written = {};
			for (; i < length; i++)
				if (written < 1'000'000'000)
					written = (written * 10) + (chars[i] - '0');
			exponent += negative ? -written : written;
		}

		text[text_length++] = 'e';
		if (exponent < 0)
		{
			text[text_length++] = '-';
			exponent			= -exponent;
		}
		char reversed[20];
		size_t digits = {};
		do
		{
			reversed[digits++] = static_cast<char>('0' + (exponent % 10));
			exponent /= 10;
		}
		while (exponent);
		while (digits)
			text[text_length++] = reversed[--digits];
		text[text_length] = '\0';

		char* end = {};
		result	  = std::strtod(text, &end);
		return end == text + text_length && impl::fpclassify(result) != impl::fp_class::pos_inf;
	}
}
TOML_ANON_NAMESPACE_END;

// Q: "what the fuck is this? MACROS????"
// A: The parser needs to work in exceptionless mode (returning error objects directly)
//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
//    These macros encapsulate the differences between the two modes so I can write code code
//    as though I was only targeting one mode and not want yeet myself into the sun.
//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
//    of toml++.

#define is_eof()		 !cp
#define assert_not_eof() TOML_ASSERT_ASSUME(cp != nullptr)
// clang-format off
#define return_if_eof(...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
			return __VA_ARGS__;                                                                                        \
	}                                                                                                                  \
	while (false)
// clang-format on

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
// clang-format off
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
// clang-format on
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

// clang-format off
#define set_error_and_return_if_eof(...)                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		if TOML_UNLIKELY(is_eof())                                                                                     \
		{                                                                                                              \
			if (!is_error())                                                                                           \
				set_eof_error();                                                                                       \
			return_after_error(__VA_ARGS__);                                                                           \
		}                                                                                                              \
	}                                                                                                                  \
	while (false)
// clang-format on

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

TOML_IMPL_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, impl_ex, impl_noex);

	class parser
	{
	  private:
		static constexpr size_t max_nested_values	  = TOML_MAX_NESTED_VALUES;
		static constexpr size_t max_dotted_keys_depth = TOML_MAX_DOTTED_KEYS_DEPTH;
		static constexpr size_t max_value_length	  = 128;

		// NOLINTBEGIN(readability-identifier-naming)
		source_reader reader;
		table root;
		const utf8_codepoint* cp = {};
		node_set implicit_tables;
		node_set dotted_key_tables;
		node_set table_arrays;
		parse_key_buffer key_buffer;
		std::string string_buffer;
		std::string recording_buffer; // for diagnostics
		bool recording = false, recording_whitespace = true;
		std::string_view current_scope;
		size_t nested_values = {};
#if !TOML_EXCEPTIONS
		mutable optional<parse_error> err;
#endif
		// NOLINTEND(readability-identifier-naming)

		TOML_NODISCARD
		source_position current_position(source_index fallback_offset = 0) const noexcept
		{
			if (!is_eof())
				return reader.position();
			auto pos = reader.eof_anchor();
			pos.column += fallback_offset;
			return pos;
		}

		// a stream's exception is rethrown instead, as a parse_error when exceptions are enabled
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_wall_error() const
		{
			const auto pos = reader.position();
#if TOML_EXCEPTIONS
			try
			{
				reader.rethrow_stream_failure();
			}
			catch (const std::exception& exc)
			{
				throw parse_error{ exc.what(), pos, reader.source_path() };
			}
			catch (...)
			{
				throw parse_error{ "An unspecified error occurred", pos, reader.source_path() };
			}
			throw parse_error{ reader.wall_reason(), pos, reader.source_path() };
#else
#if TOML_COMPILER_HAS_EXCEPTIONS
			reader.rethrow_stream_failure();
#endif
			err.emplace(reader.wall_reason(), pos, reader.source_path());
#endif
		}

		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at_impl(source_position pos, const error_arg* args, size_t count) const
		{
			return_if_error();

			error_builder builder{ current_scope };
			for (size_t i = 0; i < count; i++)
				builder.append(args[i]);

			parse_error_break();

#if TOML_EXCEPTIONS
			builder.finish(pos, reader.source_path());
#else
			err.emplace(builder.finish(pos, reader.source_path()));
#endif
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_error_at(source_position pos, const T&... reason) const
		{
			static_assert(sizeof...(T) > 0);
			const error_arg args[] = { make_error_arg(reason)... };
			set_error_at_impl(pos, args, sizeof...(T));
		}

		template <typename... T>
		TOML_RETURNS_BY_THROWING
		void set_error(const T&... reason) const
		{
			set_error_at(current_position(1), reason...);
		}

		TOML_RETURNS_BY_THROWING
		TOML_NEVER_INLINE
		void set_eof_error() const
		{
			set_error("encountered end-of-file"sv);
		}

		void land(const utf8_codepoint* next)
		{
			cp = next;
			if TOML_UNLIKELY(!cp)
			{
				if (reader.at_wall())
					set_wall_error();
				return;
			}
			if (recording && (recording_whitespace || !is_whitespace(*cp)))
				recording_buffer.append(cp->bytes, cp->count);
		}

		void advance()
		{
			return_if_error();
			assert_not_eof();

			land(reader.next());
		}

		template <run_class Class>
		void advance_by(const run_span& span)
		{
			return_if_error();
			assert_not_eof();

			if (recording && span.length > cp->count)
			{
				// a run mixing whitespace with other code points can only be recorded whole
				TOML_ASSERT(Class == run_class::bare_key || recording_whitespace);
				if (Class == run_class::bare_key || recording_whitespace)
					recording_buffer.append(span.data + cp->count, span.length - cp->count);
			}

			land(reader.skip(span));
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

		void stop_recording_key() noexcept
		{
			stop_recording(1u);
			while (!recording_buffer.empty() && (recording_buffer.back() == ' ' || recording_buffer.back() == '\t'))
				recording_buffer.pop_back();
		}

		bool consume_leading_whitespace()
		{
			return_if_error_or_eof({});

			bool consumed = false;
			while (!is_eof() && is_horizontal_whitespace(*cp))
			{
				if TOML_UNLIKELY(!is_ascii_horizontal_whitespace(*cp))
					set_error_and_return_default("expected space or tab, saw '"sv, *cp, "'"sv);

				consumed = true;
				advance_and_return_if_error({});
			}
			return consumed;
		}

		bool consume_line_break()
		{
			return_if_error_or_eof({});

			if TOML_UNLIKELY(is_match(*cp, U'\v', U'\f'))
				set_error_and_return_default(
					R"(vertical tabs '\v' and form-feeds '\f' are not legal line breaks in TOML)"sv);

			if (*cp == U'\r')
			{
				advance_and_return_if_error({}); // skip \r

				if TOML_UNLIKELY(is_eof())
					set_error_and_return_default("expected '\\n' after '\\r', saw EOF"sv);

				if TOML_UNLIKELY(*cp != U'\n')
					set_error_and_return_default("expected '\\n' after '\\r', saw '"sv, *cp, "'"sv);
			}
			else if (*cp != U'\n')
				return false;

			advance_and_return_if_error({}); // skip \n
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

#if TOML_LANG_AT_LEAST(1, 0, 0)

				// toml/issues/567 (disallow non-TAB control characters in comments)
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are explicitly prohibited in comments"sv);

				// toml/pull/720 (disallow surrogates in comments)
				else if TOML_UNLIKELY(is_unicode_surrogate(*cp))
					set_error_and_return_default(
						"unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited in comments"sv);
#endif

				advance_by<run_class::comment>(reader.run<run_class::comment>());
				return_if_error({});
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
			TOML_ASSERT_ASSUME(digits);
			TOML_ASSERT_ASSUME(len);

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
			TOML_ASSERT_ASSUME(buffer);
			TOML_ASSERT_ASSUME(max_len);

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
		TOML_NEVER_INLINE
		std::string_view parse_basic_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'"');
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

			auto& str = string_buffer;
			str.clear();
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

						if TOML_UNLIKELY(!consume_line_break())
							set_error_and_return_default(
								"line-ending backslashes must be the last non-whitespace character on the line"sv);

						skipping_whitespace = true;
						return_if_error({});
						continue;
					}

					bool skip_escaped_codepoint = true;
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
						case U'e': str += '\x1B'; break;

						// unicode scalar sequences
						case U'x': [[fallthrough]];
						case U'u': [[fallthrough]];
						case U'U':
						{
							push_parse_scope("unicode scalar sequence"sv);
							auto escape_pos = current_position();
							escape_pos.column--;
							advance_and_return_if_error_or_eof({});
							skip_escaped_codepoint = false;

							uint32_t place_value =
								escaped_codepoint == U'U' ? 0x10000000u : (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
							uint32_t sequence_value{};
							while (place_value)
							{
								set_error_and_return_if_eof({});

								if TOML_UNLIKELY(!is_hexadecimal_digit(*cp))
									set_error_and_return_default("expected hex digit, saw '"sv, *cp, "'"sv);

								sequence_value += place_value * hex_to_dec(*cp);
								place_value /= 16u;
								advance_and_return_if_error({});
							}

							if TOML_UNLIKELY(is_unicode_surrogate(sequence_value))
							{
								set_error_at(escape_pos,
											 "unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
								return_after_error({});
							}
							if TOML_UNLIKELY(sequence_value > 0x10FFFFu)
							{
								set_error_at(escape_pos, "values greater than U+10FFFF are invalid"sv);
								return_after_error({});
							}

							if (sequence_value < 0x80)
							{
								str += static_cast<char>(sequence_value);
							}
							else if (sequence_value < 0x800u)
							{
								str += static_cast<char>((sequence_value >> 6) | 0xC0u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							else if (sequence_value < 0x10000u)
							{
								str += static_cast<char>((sequence_value >> 12) | 0xE0u);
								str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							else if (sequence_value < 0x110000u)
							{
								str += static_cast<char>((sequence_value >> 18) | 0xF0u);
								str += static_cast<char>(((sequence_value >> 12) & 0x3Fu) | 0x80u);
								str += static_cast<char>(((sequence_value >> 6) & 0x3Fu) | 0x80u);
								str += static_cast<char>((sequence_value & 0x3Fu) | 0x80u);
							}
							break;
						}

							// ???
							TOML_UNLIKELY_CASE
						default: set_error_and_return_default("unknown escape sequence '\\"sv, *cp, "'"sv);
					}

					if (skip_escaped_codepoint)
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
					if (multi_line && is_ascii_vertical_whitespace(*cp))
					{
						consume_line_break();
						return_if_error({});
						if (!skipping_whitespace)
							str += '\n';
						continue;
					}

					// handle control characters
					if TOML_UNLIKELY(is_nontab_control_character(*cp))
						set_error_and_return_default(
							"unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv);

#if TOML_LANG_AT_LEAST(1, 0, 0)

					// handle surrogates in strings
					if TOML_UNLIKELY(is_unicode_surrogate(*cp))
						set_error_and_return_default(
							"unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv);
#endif

					if (skipping_whitespace && is_ascii_horizontal_whitespace(*cp))
					{
						advance_and_return_if_error({});
						continue;
					}

					skipping_whitespace = false;
					const auto span		= reader.run<run_class::basic_string>();
					str.append(span.data, span.length);
					advance_by<run_class::basic_string>(span);
					return_if_error({});
				}
			}
			while (!is_eof());

			set_error_and_return_default("encountered end-of-file"sv);
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_literal_string(bool multi_line)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'\'');
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

			auto& str = string_buffer;
			str.clear();
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
				if (multi_line && is_ascii_vertical_whitespace(*cp))
				{
					consume_line_break();
					return_if_error({});
					str += '\n';
					continue;
				}

				// handle control characters
				if TOML_UNLIKELY(is_nontab_control_character(*cp))
					set_error_and_return_default(
						"control characters other than TAB (U+0009) are explicitly prohibited"sv);

#if TOML_LANG_AT_LEAST(1, 0, 0)

				// handle surrogates in strings
				if TOML_UNLIKELY(is_unicode_surrogate(*cp))
					set_error_and_return_default("unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv);
#endif

				const auto span = reader.run<run_class::literal_string>();
				str.append(span.data, span.length);
				advance_by<run_class::literal_string>(span);
				return_if_error({});
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
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp));
			push_parse_scope("string"sv);

			// the single-line parsers start on the opening delimiter, so peek instead of consuming
			const auto first = cp->value;
			auto la			 = reader.lookahead();
			reader.step(la);
			const auto second = la.value;
			if (second < eof_mark)
			{
				reader.step(la);
				if (la.value != eof_mark && (second != first || la.value != first))
					return { first == U'\'' ? parse_literal_string(false) : parse_basic_string(false), false };
			}

			advance_and_return_if_error_or_eof({});
			advance_and_return_if_error({});

			// if we were eof at the third character then first and second need to be
			// the same string character (otherwise it's an unterminated string)
			if (is_eof())
			{
				if (second == first)
					return {};

				set_error_and_return_default("encountered end-of-file"sv);
			}

			// otherwise the first three characters are all the same string delimiter, so it's a multi-line string
			return { first == U'\'' ? parse_literal_string(true) : parse_basic_string(true), true };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		std::string_view parse_bare_key_segment()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp));

			string_buffer.clear();

			while (!is_eof())
			{
				if (!is_bare_key_character(*cp))
					break;

				const auto span = reader.run<run_class::bare_key>();
				string_buffer.append(span.data, span.length);
				advance_by<run_class::bare_key>(span);
				return_if_error({});
			}

			return string_buffer;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		bool parse_boolean()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U't', U'f', U'T', U'F'));
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
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return result;
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_inf_or_nan()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
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
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return inf ? (negative ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity())
					   : std::numeric_limits<double>::quiet_NaN();
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'+', U'-', U'.') || is_decimal_digit(*cp));
			push_parse_scope("floating-point"sv);
			const auto begin_pos = current_position();

			// sign
			const int sign					 = *cp == U'-' ? -1 : 1;
			const std::string_view sign_text = *cp == U'-' ? "-"sv : (*cp == U'+' ? "+"sv : ""sv);
			if (is_match(*cp, U'+', U'-'))
				advance_and_return_if_error_or_eof({});

			// consume value chars
			char chars[max_value_length];
			size_t length			= {};
			char32_t prev			= {};
			bool seen_decimal		= false;
			bool seen_exponent		= false;
			char first_integer_part = '\0';
			source_position first_integer_pos{};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_decimal_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				if TOML_UNLIKELY(prev == U'_' && !is_decimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(length == sizeof(chars))
					set_error_and_return_default("exceeds length limit of "sv,
												 sizeof(chars),
												 " digits"sv,
												 (seen_exponent ? ""sv : " (consider using exponent notation)"sv));
				else if (*cp == U'.')
				{
					// .1
					// -.1
					// +.1 (no integer part)
					if (!first_integer_part)
						set_error_and_return_default("expected decimal digit, saw '.'"sv);

					// 1.0e+.10 (exponent cannot have '.')
					else if (seen_exponent)
						set_error_and_return_default(is_match(prev, U'e', U'E')
														 ? "expected exponent decimal digit or sign, saw '.'"sv
														 : "expected exponent decimal digit, saw '.'"sv);

					// 1.0.e+.10
					// 1..0
					// (multiple '.')
					else if (seen_decimal)
						set_error_and_return_default("expected decimal digit or exponent, saw '.'"sv);

					seen_decimal = true;
				}
				else if (is_match(*cp, U'e', U'E'))
				{
					if (!is_decimal_digit(prev) // +e5, 1.e5
						|| seen_exponent)		// 1.0ee+10 (multiple 'e')
						set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);

					seen_decimal  = true; // implied
					seen_exponent = true;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 1.-0 (sign in mantissa)
					if (!seen_exponent)
						set_error_and_return_default("expected decimal digit or '.', saw '"sv, *cp, "'"sv);

					// 1.0e1-0 (misplaced exponent sign)
					else if (!is_match(prev, U'e', U'E'))
						set_error_and_return_default("expected exponent digit, saw '"sv, *cp, "'"sv);
				}
				else if (is_decimal_digit(*cp))
				{
					if (!seen_decimal)
					{
						if (!first_integer_part)
						{
							first_integer_part = static_cast<char>(cp->bytes[0]);
							first_integer_pos  = current_position();
						}
						else if (first_integer_part == '0')
						{
							set_error_at(first_integer_pos, "leading zeroes are prohibited"sv);
							return_after_error({});
						}
					}
				}
				else
					set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);

				chars[length++] = static_cast<char>(cp->bytes[0]);
				prev			= *cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (prev)
			{
				if (prev == U'_')
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (is_match(prev, U'e', U'E', U'+', U'-', U'.'))
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("expected decimal digit, saw '"sv, *cp, "'"sv);
				}
			}

			// convert to double
			double result = {};
#if TOML_FLOAT_CHARCONV
			if TOML_LIKELY(std::from_chars(chars, chars + length, result).ec == std::errc{})
				return result * sign;
#endif
			if (decimal_to_double(chars, length, result))
				return result * sign;

			set_error_at(begin_pos,
						 "'"sv,
						 sign_text,
						 std::string_view{ chars, length },
						 "' is not representable in 64 bits"sv);
			return_after_error({});
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		double parse_hex_float()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_match(*cp, U'0', U'+', U'-'));
			push_parse_scope("hexadecimal floating-point"sv);

#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

			const auto begin_pos = current_position();
			char text[80];
			size_t text_length = {};

			// sign
			const int sign = *cp == U'-' ? -1 : 1;
			if (is_match(*cp, U'+', U'-'))
			{
				text[text_length++] = static_cast<char>(cp->bytes[0]);
				advance_and_return_if_error_or_eof({});
			}

			// '0'
			if (*cp != U'0')
				set_error_and_return_default(" expected '0', saw '"sv, *cp, "'"sv);
			text[text_length++] = '0';
			advance_and_return_if_error_or_eof({});

			// 'x' or 'X'
			if (!is_match(*cp, U'x', U'X'))
				set_error_and_return_default("expected 'x' or 'X', saw '"sv, *cp, "'"sv);
			text[text_length++] = static_cast<char>(cp->bytes[0]);
			advance_and_return_if_error_or_eof({});

			// <HEX DIGITS> ([.]<HEX DIGITS>)? [pP] [+-]? <DEC DIGITS>

			// consume value fragments
			struct fragment
			{
				char chars[24];
				size_t length;
			};
			fragment fragments[] = {
				{}, // mantissa, whole part
				{}, // mantissa, fractional part
				{}	// exponent
			};
			fragment* current_fragment	 = fragments;
			char32_t prev				 = {};
			int exponent_sign			 = 1;
			const auto expected_exponent = [&]() noexcept
			{
				return is_match(prev, U'p', U'P') ? "expected exponent digit or sign, saw '"sv
												  : "expected exponent digit, saw '"sv;
			};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !is_hexadecimal_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				else if (prev == U'_' && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if (*cp == U'.')
				{
					// 0x10.0p-.0 (exponent cannot have '.')
					if (current_fragment == fragments + 2)
						set_error_and_return_default(expected_exponent(), "."sv, "'"sv);

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
						set_error_and_return_default(expected_exponent(), *cp, "'"sv);

					// 0x.p-0 (mantissa is just '.')
					else if (fragments[0].length == 0u && fragments[1].length == 0u)
						set_error_and_return_default("expected hexadecimal digit, saw '"sv, *cp, "'"sv);

					else
						current_fragment = fragments + 2;
				}
				else if (is_match(*cp, U'+', U'-'))
				{
					// 0x-10.0p-0 (sign in mantissa)
					if (current_fragment != fragments + 2)
						set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, *cp, "'"sv);

					// 0x10.0p0- (misplaced exponent sign)
					else if (!is_match(prev, U'p', U'P'))
						set_error_and_return_default("expected exponent digit, saw '"sv, *cp, "'"sv);

					else
						exponent_sign = *cp == U'-' ? -1 : 1;
				}
				else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
					set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, *cp, "'"sv);
				else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
					set_error_and_return_default(expected_exponent(), *cp, "'"sv);
				else if (current_fragment->length == sizeof(fragment::chars))
					set_error_and_return_default("fragment exceeds maximum length of "sv,
												 sizeof(fragment::chars),
												 " characters"sv);
				else
					current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);

				text[text_length++] = static_cast<char>(cp->bytes[0]);
				prev				= *cp;
				advance_and_return_if_error({});
			}

			// sanity-check ending state
			if (current_fragment != fragments + 2 || current_fragment->length == 0u)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("missing exponent"sv);
			}
			else if (prev == U'_')
				set_error_and_return_default("underscores must be followed by digits"sv);

			uint64_t mantissa	= {};
			int binary_exponent = {};
			bool inexact		= {};
			for (size_t fragment_idx = 0; fragment_idx < 2u; fragment_idx++)
			{
				const auto& f = fragments[fragment_idx];
				for (size_t i = 0; i < f.length; i++)
				{
					if (mantissa >> 60)
					{
						inexact = inexact || f.chars[i] != '0';
						if (fragment_idx == 0u)
							binary_exponent += 4;
						continue;
					}
					mantissa = (mantissa << 4) | hex_to_dec(f.chars[i]);
					if (fragment_idx == 1u)
						binary_exponent -= 4;
				}
			}
			if (inexact)
				mantissa |= 1u;

			int exponent = {};
			for (size_t i = 0; i < fragments[2].length && exponent < 100000; i++)
				exponent = exponent * 10 + (fragments[2].chars[i] - '0');
			binary_exponent += exponent * exponent_sign;

			// round once, to 53 bits or the subnormal grid, so the ldexp below is exact;
			// converting to double first rounds subnormals twice
			if (mantissa)
			{
				constexpr int digits		= std::numeric_limits<double>::digits;
				constexpr int lowest_binary = std::numeric_limits<double>::min_exponent - digits;

				int msb = 63;
				while (!(mantissa >> msb))
					msb--;
				const int drop = msb + 1 - digits > lowest_binary - binary_exponent ? msb + 1 - digits
																					: lowest_binary - binary_exponent;
				if (drop > 64)
					mantissa = 0u;
				else if (drop > 0)
				{
					const uint64_t rem	= drop == 64 ? mantissa : mantissa & ((uint64_t{ 1 } << drop) - 1u);
					const uint64_t half = uint64_t{ 1 } << (drop - 1);
					mantissa			= drop == 64 ? 0u : mantissa >> drop;
					if (rem > half || (rem == half && (mantissa & 1u)))
						mantissa++;
				}
				if (drop > 0)
					binary_exponent += drop;
			}

			const double result = impl::ldexp(static_cast<double>(mantissa), binary_exponent);
			if (result > (std::numeric_limits<double>::max)())
			{
				set_error_at(begin_pos,
							 "'"sv,
							 std::string_view{ text, text_length },
							 "' is not representable in 64 bits"sv);
				return_after_error({});
			}
			return result * sign;

#else // !TOML_LANG_UNRELEASED

			set_error_and_return_default("hexadecimal floating-point values are not supported "
										 "in TOML 1.1.0 and earlier"sv);

#endif // !TOML_LANG_UNRELEASED
		}

		template <uint64_t Base>
		TOML_NODISCARD
		TOML_NEVER_INLINE
		int64_t parse_integer()
		{
			return_if_error({});
			assert_not_eof();
			using traits = parse_integer_traits<Base>;
			push_parse_scope(traits::scope_qualifier);

			const auto begin_pos = current_position();

			const std::string_view sign_text =
				traits::is_signed && is_match(*cp, U'+', U'-') ? (*cp == U'-' ? "-"sv : "+"sv) : ""sv;
			[[maybe_unused]] int64_t sign = 1;
			if constexpr (traits::is_signed)
			{
				sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});
			}

			[[maybe_unused]] const auto digits_pos = current_position();

			if constexpr (Base == 10)
			{
				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
			}
			else
			{
				// '0'
				if (*cp != U'0')
					set_error_and_return_default("expected '0', saw '"sv, *cp, "'"sv);
				advance_and_return_if_error_or_eof({});

				// 'b', 'o', 'x'
				if (*cp != traits::prefix_codepoint)
					set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, *cp, "'"sv);
				advance_and_return_if_error_or_eof({});

				if (!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
			}

			// consume digits
			char digits[max_value_length];
			size_t length = {};
			char32_t prev = {};
			while (!is_eof() && !is_value_terminator(*cp))
			{
				if (*cp == U'_')
				{
					if (!prev || !traits::is_digit(prev))
						set_error_and_return_default("underscores may only follow digits"sv);

					prev = *cp;
					advance_and_return_if_error({});
					continue;
				}
				if TOML_UNLIKELY(prev == U'_' && !traits::is_digit(*cp))
					set_error_and_return_default("underscores must be followed by digits"sv);
				else if TOML_UNLIKELY(!traits::is_digit(*cp))
					set_error_and_return_default("expected "sv, traits::digit_name, ", saw '"sv, *cp, "'"sv);
				else if TOML_UNLIKELY(length == sizeof(digits))
					set_error_and_return_default("exceeds length limit of "sv, sizeof(digits), " digits"sv);
				else
					digits[length++] = static_cast<char>(cp->bytes[0]);

				prev = *cp;
				advance_and_return_if_error({});
			}

			// sanity check ending state
			if (prev == U'_')
				set_error_and_return_default("underscores must be followed by digits"sv);

			// single digits can be converted trivially
			if (length == 1u)
			{
				int64_t result = {};

				if constexpr (Base == 16)
					result = static_cast<int64_t>(hex_to_dec(digits[0]));
				else
					result = static_cast<int64_t>(digits[0] - '0');

				if constexpr (traits::is_signed)
					result *= sign;

				return result;
			}

			// bin, oct and hex allow leading zeroes so trim them first
			const char* end = digits + length;
			const char* msd = digits;
			if constexpr (Base != 10)
			{
				while (msd < end && *msd == '0')
					msd++;
				if (msd == end)
					return 0ll;
			}

			// decimal integers do not allow leading zeroes
			else
			{
				if TOML_UNLIKELY(digits[0] == '0')
				{
					set_error_at(digits_pos, "leading zeroes are prohibited"sv);
					return_after_error({});
				}
			}

			// range check
			if TOML_UNLIKELY(static_cast<size_t>(end - msd) > traits::max_digits)
			{
				set_error_at(begin_pos,
							 "'"sv,
							 sign_text,
							 traits::full_prefix,
							 std::string_view{ digits, length },
							 "' is not representable as a signed 64-bit integer"sv);
				return_after_error({});
			}

			// do the thing
			{
				uint64_t result = {};
				{
					uint64_t power = 1;
					while (--end >= msd)
					{
						if constexpr (Base == 16)
							result += power * hex_to_dec(*end);
						else
							result += power * static_cast<uint64_t>(*end - '0');

						power *= Base;
					}
				}

				// range check
				static constexpr auto i64_max = static_cast<uint64_t>((std::numeric_limits<int64_t>::max)());
				if TOML_UNLIKELY(result > i64_max + (sign < 0 ? 1u : 0u))
				{
					set_error_at(begin_pos,
								 "'"sv,
								 sign_text,
								 traits::full_prefix,
								 std::string_view{ digits, length },
								 "' is not representable as a signed 64-bit integer"sv);
					return_after_error({});
				}

				if constexpr (traits::is_signed)
				{
					// avoid signed multiply UB when parsing INT64_MIN
					if TOML_UNLIKELY(sign < 0 && result == i64_max + 1u)
						return (std::numeric_limits<int64_t>::min)();

					return static_cast<int64_t>(result) * sign;
				}
				else
					return static_cast<int64_t>(result);
			}
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		date parse_date(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date"sv);

			// "YYYY"
			uint32_t digits[4];
			if (!consume_digit_sequence(digits, 4u))
				set_error_and_return_default("expected 4-digit year, saw '"sv, cp, "'"sv);
			const auto year			= digits[3] + (digits[2] * 10u) + (digits[1] * 100u) + (digits[0] * 1000u);
			const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			const auto month_pos = current_position();
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit month, saw '"sv, cp, "'"sv);
			const auto month = digits[1] + (digits[0] * 10u);
			if (month == 0u || month > 12u)
			{
				set_error_at(month_pos, "expected month between 1 and 12 (inclusive), saw "sv, month);
				return_after_error({});
			}
			const auto max_days_in_month = month == 2u
											 ? (is_leap_year ? 29u : 28u)
											 : (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u);
			set_error_and_return_if_eof({});

			// '-'
			if (*cp != U'-')
				set_error_and_return_default("expected '-', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "DD"
			const auto day_pos = current_position();
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit day, saw '"sv, cp, "'"sv);
			const auto day = digits[1] + (digits[0] * 10u);
			if (day == 0u || day > max_days_in_month)
			{
				set_error_at(day_pos, "expected day between 1 and "sv, max_days_in_month, " (inclusive), saw "sv, day);
				return_after_error({});
			}

			if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			return { year, month, day };
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		time parse_time(bool part_of_datetime = false)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("time"sv);

			static constexpr size_t max_digits = 9;
			uint32_t digits[max_digits];

			// "HH"
			const auto hour_pos = current_position();
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit hour, saw '"sv, cp, "'"sv);
			const auto hour = digits[1] + (digits[0] * 10u);
			if (hour > 23u)
			{
				set_error_at(hour_pos, "expected hour between 0 and 23 (inclusive), saw "sv, hour);
				return_after_error({});
			}
			set_error_and_return_if_eof({});

			// ':'
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "MM"
			const auto minute_pos = current_position();
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit minute, saw '"sv, cp, "'"sv);
			const auto minute = digits[1] + (digits[0] * 10u);
			if (minute > 59u)
			{
				set_error_at(minute_pos, "expected minute between 0 and 59 (inclusive), saw "sv, minute);
				return_after_error({});
			}
			auto time = toml::time{ hour, minute };

			// ':' (early-exiting is allowed; seconds are optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return time;
			if (*cp != U':')
				set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "SS"
			const auto second_pos = current_position();
			if (!consume_digit_sequence(digits, 2u))
				set_error_and_return_default("expected 2-digit second, saw '"sv, cp, "'"sv);
			const auto second = digits[1] + (digits[0] * 10u);
			if (second > 60u)
			{
				set_error_at(second_pos, "expected second between 0 and 60 (inclusive), saw "sv, second);
				return_after_error({});
			}
			if (second == 60u && minute != 59u && !part_of_datetime)
			{
				set_error_at(second_pos, "second 60 (a leap second) is only valid at minute 59, saw minute "sv, minute);
				return_after_error({});
			}
			time.second = static_cast<decltype(time.second)>(second);

			// '.' (early-exiting is allowed; fractional is optional)
			if (is_eof() || is_value_terminator(*cp) || (part_of_datetime && is_match(*cp, U'+', U'-', U'Z', U'z')))
				return time;
			if (*cp != U'.')
				set_error_and_return_default("expected '.', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "FFFFFFFFF"
			size_t digit_count = consume_variable_length_digit_sequence(digits, max_digits);
			if (!digit_count)
			{
				set_error_and_return_if_eof({});
				set_error_and_return_default("expected fractional digits, saw '"sv, *cp, "'"sv);
			}
			while (!is_eof() && is_decimal_digit(*cp))
				advance_and_return_if_error({});
			if (!is_eof() && !part_of_datetime && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);
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
		TOML_NEVER_INLINE
		date_time parse_date_time()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_decimal_digit(*cp));
			push_parse_scope("date-time"sv);

			// "YYYY-MM-DD"
			auto date = parse_date(true);
			set_error_and_return_if_eof({});

			// ' ', 'T' or 't'
			if (!is_match(*cp, U' ', U'T', U't'))
				set_error_and_return_default("expected space, 'T' or 't', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// "HH:MM:SS.FFFFFFFFF"
			if TOML_UNLIKELY(!is_decimal_digit(*cp))
				set_error_and_return_default("expected 2-digit hour, saw '"sv, *cp, "'"sv);
			const auto time_pos = current_position();
			auto time			= parse_time(true);
			return_if_error({});
			const source_position second_pos{ time_pos.line, static_cast<source_index>(time_pos.column + 6u) };

			// no offset
			if (is_eof() || is_value_terminator(*cp))
			{
				if (time.second == 60u && time.minute != 59u)
				{
					set_error_at(second_pos,
								 "second 60 (a leap second) is only valid at minute 59, saw minute "sv,
								 static_cast<unsigned>(time.minute));
					return_after_error({});
				}
				return { date, time };
			}

			// zero offset ('Z' or 'z')
			time_offset offset{};
			if (is_match(*cp, U'Z', U'z'))
				advance_and_return_if_error({});

			// explicit offset ("+/-HH:MM")
			else if (is_match(*cp, U'+', U'-'))
			{
				push_parse_scope("date-time offset"sv);

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				advance_and_return_if_error_or_eof({});

				// "HH"
				int digits[2];
				const auto hour_pos = current_position();
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit hour, saw '"sv, cp, "'"sv);
				const auto hour = digits[1] + (digits[0] * 10);
				if (hour > 23)
				{
					set_error_at(hour_pos, "expected hour between 0 and 23 (inclusive), saw "sv, hour);
					return_after_error({});
				}
				set_error_and_return_if_eof({});

				// ':'
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, *cp, "'"sv);
				advance_and_return_if_error_or_eof({});

				// "MM"
				const auto minute_pos = current_position();
				if (!consume_digit_sequence(digits, 2u))
					set_error_and_return_default("expected 2-digit minute, saw '"sv, cp, "'"sv);
				const auto minute = digits[1] + (digits[0] * 10);
				if (minute > 59)
				{
					set_error_at(minute_pos, "expected minute between 0 and 59 (inclusive), saw "sv, minute);
					return_after_error({});
				}
				offset.minutes = static_cast<decltype(offset.minutes)>(((hour * 60) + minute) * sign);
			}

			if (!is_eof() && !is_value_terminator(*cp))
				set_error_and_return_default("expected end of value, saw '"sv, *cp, "'"sv);

			if (time.second == 60u)
			{
				const int utc_minutes = (time.hour * 60) + time.minute - offset.minutes;
				const int utc_minute  = ((utc_minutes % 60) + 60) % 60;
				if (utc_minute != 59)
				{
					set_error_at(second_pos,
								 "second 60 (a leap second) is only valid at minute 59 UTC, saw minute "sv,
								 utc_minute,
								 " UTC"sv);
					return_after_error({});
				}
			}

			return { date, time, offset };
		}

		TOML_NODISCARD
		node_ptr parse_array();

		TOML_NODISCARD
		node_ptr parse_inline_table();

		TOML_NODISCARD
		node_ptr parse_value_known_prefixes()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_control_character(*cp));
			TOML_ASSERT_ASSUME(*cp != U'_');

			switch (cp->value)
			{
				// arrays
				case U'[': return parse_array();

				// inline tables
				case U'{': return parse_inline_table();

				// floats beginning with '.'
				case U'.': return node_ptr{ new value{ parse_float() } };

				// strings
				case U'"': [[fallthrough]];
				case U'\'': return node_ptr{ new value{ parse_string().value } };

				default:
				{
					const auto cp_upper = static_cast<uint_least32_t>(cp->value) & ~0x20u;

					// bools
					if (cp_upper == 70u || cp_upper == 84u) // F or T
						return node_ptr{ new value{ parse_boolean() } };

					// inf/nan
					if (cp_upper == 73u || cp_upper == 78u) // I or N
						return node_ptr{ new value{ parse_inf_or_nan() } };

					return nullptr;
				}
			}
			TOML_UNREACHABLE;
		}

		TOML_NODISCARD
		node_ptr parse_value()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(!is_value_terminator(*cp));
			push_parse_scope("value"sv);

			const depth_counter_scope depth_counter{ nested_values };
			if TOML_UNLIKELY(nested_values > max_nested_values)
				set_error_and_return_default("exceeded maximum nested value depth of "sv,
											 max_nested_values,
											 " (TOML_MAX_NESTED_VALUES)"sv);

			// check if it begins with some control character
			// (note that this will also fail for whitespace but we're assuming we've
			// called consume_leading_whitespace() before calling parse_value())
			if TOML_UNLIKELY(is_control_character(*cp))
				set_error_and_return_default("unexpected control character"sv);

			// underscores at the beginning
			else if (*cp == U'_')
				set_error_and_return_default("values may not begin with underscores"sv);

			const auto begin_pos = current_position();
			node_ptr val;

			do
			{
				TOML_ASSERT_ASSUME(!is_control_character(*cp));
				TOML_ASSERT_ASSUME(*cp != U'_');

				// detect the value type and parse accordingly,
				// starting with value types that can be detected
				// unambiguously from just one character.

				val = parse_value_known_prefixes();
				return_if_error({});
				if (val)
					break;

				// value types from here down require more than one character to unambiguously identify
				// so scan ahead and collect a set of value 'traits'.
				// NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
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
					bdigit_msk = has_digits | begins_digit,
					bzero_msk  = bdigit_msk | begins_zero,
				};
				value_traits traits	 = has_nothing;
				const auto has_any	 = [&](auto t) noexcept { return (traits & t) != has_nothing; };
				const auto has_none	 = [&](auto t) noexcept { return (traits & t) == has_nothing; };
				const auto add_trait = [&](auto t) noexcept { traits = static_cast<value_traits>(traits | t); };

				// examine the first character to get the 'begins with' traits
				// (good fail-fast opportunity; all the remaining types begin with numeric digits or signs)
				if (is_decimal_digit(*cp))
				{
					add_trait(begins_digit);
					if (*cp == U'0')
						add_trait(begins_zero);
				}
				else if (is_match(*cp, U'+', U'-'))
					add_trait(begins_sign);
				else
					break;

				// scan the rest of the value to determine the remaining traits
				char32_t chars[max_value_length];
				size_t char_count		= {};
				size_t advance_count	= {};
				bool eof_while_scanning = false;
				auto la					= reader.lookahead();
				const auto ends_value	= [&]() noexcept
				{ return la.value >= eof_mark || is_value_terminator(la.value); };
				const auto scan = [&]() noexcept(!TOML_COMPILER_HAS_EXCEPTIONS)
				{
					if (ends_value())
						return;

					do
					{
						if (const auto c = la.value; c != U'_')
						{
							chars[char_count++] = c;

							if (is_decimal_digit(c))
								add_trait(has_digits);
							else if (is_ascii_letter(c))
							{
								TOML_ASSERT_ASSUME((c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z'));
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
									default: break;
								}
							}
							else if (c <= U':')
							{
								TOML_ASSERT_ASSUME(c < U'0' || c > U'9');
								switch (c)
								{
									case U'+': add_trait(has_plus); break;
									case U'-': add_trait(has_minus); break;
									case U'.': add_trait(has_dot); break;
									case U':': add_trait(has_colon); break;
									default: break;
								}
							}
						}

						reader.step(la);
						advance_count++;
						eof_while_scanning = la.value == eof_mark;
					}
					while (advance_count < (max_value_length - 1u) && !ends_value());
				};
				scan();

				// force further scanning if this could have been a date-time with a space instead of a T
				if (char_count == 10u									 //
					&& (traits | begins_zero) == (bzero_msk | has_minus) //
					&& chars[4] == U'-'									 //
					&& chars[7] == U'-'									 //
					&& la.value == U' ')
				{
					const auto pre_advance_count = advance_count;
					const auto pre_scan_traits	 = traits;
					chars[char_count++]			 = la.value;
					add_trait(has_t);

					const auto backpedal = [&]() noexcept
					{
						advance_count = pre_advance_count;
						traits		  = pre_scan_traits;
						char_count	  = 10u;
					};

					reader.step(la);
					advance_count++;

					if (!is_decimal_digit(la.value))
						backpedal();
					else
					{
						chars[char_count++] = la.value;

						reader.step(la);
						advance_count++;

						scan();

						if (char_count == 12u)
							backpedal();
					}
				}

				// if after scanning ahead we still only have one value character,
				// the only valid value type is an integer.
				if (char_count == 1u)
				{
					if (has_any(begins_digit))
					{
						if (advance_count == 1u)
						{
							val.reset(new value{ static_cast<int64_t>(chars[0] - U'0') });
							advance(); // skip the digit
						}
						else
							val.reset(new value{ parse_integer<10>() });
						break;
					}

					if (eof_while_scanning)
						set_error_and_return_default("encountered end-of-file"sv);
					val.reset(new value{ parse_integer<10>() });
					break;
				}

				// now things that can be identified from two or more characters
				return_if_error({});
				if (char_count < 2u)
					set_error_and_return_default("could not determine value type"sv);

				if (has_any(begins_sign) && has_none(has_p) && chars[1] == U'0' && char_count > 2u)
				{
					const auto prefix = static_cast<char32_t>(chars[2] | 32u);
					if (is_match(prefix, U'x', U'o', U'b'))
						set_error_and_return_default("signs are not allowed on "sv,
													 prefix == U'x'	  ? "hexadecimal"sv
													 : prefix == U'o' ? "octal"sv
																	  : "binary"sv,
													 " integers"sv);
				}

				// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
				// typed parse functions to take over and show better diagnostics if there's an issue
				// (as opposed to the fallback "could not determine type" message)
				if (has_any(has_p))
					val.reset(new value{ parse_hex_float() });
				else if (has_any(has_x | has_o | has_b))
				{
					int64_t i		  = {};
					value_flags flags = {};
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

					val.reset(new value{ i });
					val->ref_cast<int64_t>().flags(flags);
				}
				else if (has_any(has_e) || (has_any(begins_digit) && chars[1] == U'.'))
					val.reset(new value{ parse_float() });
				else if (has_any(begins_sign))
				{
					// single-digit signed integers
					if (char_count == 2u && has_any(has_digits))
					{
						if (advance_count == 2u)
						{
							val.reset(
								new value{ static_cast<int64_t>(chars[1] - U'0') * (chars[0] == U'-' ? -1LL : 1LL) });
							advance(); // skip the sign
							advance(); // skip the digit
							break;
						}
					}

					// simple signed floats (e.g. +1.0)
					else if (is_decimal_digit(chars[1]) && chars[2] == U'.')
						val.reset(new value{ parse_float() });

					// signed infinity or nan
					else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
						val.reset(new value{ parse_inf_or_nan() });
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
						val.reset(new value{ parse_integer<2>() });
						val->ref_cast<int64_t>().flags(value_flags::format_as_binary);
						break;

					// octal integers
					// 0o10
					case bzero_msk | has_o:
						val.reset(new value{ parse_integer<8>() });
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
					case begins_sign | has_digits | has_plus:
					{
						// past the scan limit the value could still be a float, so its type is unknown
						static constexpr size_t max_numeric_value_length = max_value_length - 2u;
						if TOML_UNLIKELY(!eof_while_scanning && advance_count > max_numeric_value_length)
							set_error_and_return_default("numeric value too long to identify type - cannot exceed "sv,
														 max_numeric_value_length,
														 " characters"sv);

						val.reset(new value{ parse_integer<10>() });
						break;
					}

					// hexadecimal integers
					// 0x10
					case bzero_msk | has_x:
						val.reset(new value{ parse_integer<16>() });
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
						val.reset(new value{ parse_float() });
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
						val.reset(new value{ parse_hex_float() });
						break;

					// times
					// HH:MM
					// HH:MM:SS
					// HH:MM:SS.FFFFFF
					case bzero_msk | has_colon: [[fallthrough]];
					case bzero_msk | has_colon | has_dot: [[fallthrough]];
					case bdigit_msk | has_colon: [[fallthrough]];
					case bdigit_msk | has_colon | has_dot: val.reset(new value{ parse_time() }); break;

					// local dates
					// YYYY-MM-DD
					case bzero_msk | has_minus: [[fallthrough]];
					case bdigit_msk | has_minus: val.reset(new value{ parse_date() }); break;

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
						val.reset(new value{ parse_date_time() });
						break;

					// everything valid matched above, so this just picks the parser with the most useful error
					default:
						if (has_any(begins_digit) && char_count > 4u && chars[4] == U'-')
						{
							if (has_any(has_t | has_colon))
								val.reset(new value{ parse_date_time() });
							else
								val.reset(new value{ parse_date() });
						}
						else if (has_any(begins_digit) && has_any(has_colon))
							val.reset(new value{ parse_time() });
						else if (has_any(has_dot | has_e))
							val.reset(new value{ parse_float() });
						else
							val.reset(new value{ parse_integer<10>() });
						break;
				}
			}
			while (false);

			if (!val)
			{
				set_error_at(begin_pos, "could not determine value type"sv);
				return_after_error({});
			}

			val->source_ = { begin_pos, current_position(1), reader.source_path() };
			return val;
		}

		TOML_NEVER_INLINE
		bool parse_key()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_bare_key_character(*cp) || is_string_delimiter(*cp));
			push_parse_scope("key"sv);

			key_buffer.clear();
			recording_whitespace = false;

			while (!is_error())
			{
				std::string_view key_segment;
				const auto key_begin = current_position();

				// bare_key_segment
				if (is_bare_key_character(*cp))
					key_segment = parse_bare_key_segment();

				// "quoted key segment"
				else if (is_string_delimiter(*cp))
				{
					const auto begin_pos = current_position();

					recording_whitespace	= true;
					const parsed_string str = parse_string();
					recording_whitespace	= false;
					return_if_error({});

					if (str.was_multi_line)
					{
						set_error_at(begin_pos,
									 "multi-line strings are prohibited in "sv,
									 key_buffer.empty() ? ""sv : "dotted "sv,
									 "keys"sv);
						return_after_error({});
					}
					else
						key_segment = str.value;
				}

				// ???
				else
					set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
												 *cp,
												 "'"sv);

				const auto key_end = current_position();

				// whitespace following the key segment
				consume_leading_whitespace();

				// store segment
				key_buffer.push_back(key_segment, key_begin, key_end);

				if TOML_UNLIKELY(key_buffer.size() > max_dotted_keys_depth)
					set_error_and_return_default("exceeded maximum dotted keys depth of "sv,
												 max_dotted_keys_depth,
												 " (TOML_MAX_DOTTED_KEYS_DEPTH)"sv);

				// eof or no more key to come
				if (is_eof() || *cp != U'.')
					break;

				// was a dotted key - go around again
				advance_and_return_if_error_or_eof({});
				consume_leading_whitespace();
				set_error_and_return_if_eof({});
			}
			return_if_error({});

			return true;
		}

		TOML_NODISCARD
		key make_key(size_t segment_index) const
		{
			TOML_ASSERT(key_buffer.size() > segment_index);

			return key{
				key_buffer[segment_index],
				source_region{ key_buffer.starts[segment_index], key_buffer.ends[segment_index], root.source().path }
			};
		}

		TOML_NODISCARD
		TOML_NEVER_INLINE
		table* parse_table_header()
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(*cp == U'[');
			push_parse_scope("table header"sv);

			const source_position header_begin_pos = current_position();
			source_position header_end_pos;
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

				if (!is_bare_key_character(*cp) && !is_string_delimiter(*cp))
					set_error_and_return_default("expected bare key starting character or string delimiter, saw '"sv,
												 *cp,
												 "'"sv);

				// get the actual key
				start_recording();
				parse_key();
				stop_recording_key();
				return_if_error({});

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});

				// consume the closing ']'
				if (*cp != U']')
					set_error_and_return_default("expected ']', saw '"sv, *cp, "'"sv);
				if (is_arr)
				{
					advance_and_return_if_error_or_eof({});
					if (*cp != U']')
						set_error_and_return_default("expected ']', saw '"sv, *cp, "'"sv);
				}
				advance_and_return_if_error({});
				header_end_pos = current_position(1);

				// handle the rest of the line after the header
				consume_leading_whitespace();
				if (!is_eof() && !consume_comment() && !consume_line_break())
					set_error_and_return_default("expected a comment or whitespace, saw '"sv, cp, "'"sv);
			}
			TOML_ASSERT(!key_buffer.empty());

			// check if each parent is a table/table array, or can be created implicitly as a table.
			table* parent = &root;
			for (size_t i = 0, e = key_buffer.size() - 1u; i < e; i++)
			{
				const std::string_view segment = key_buffer[i];
				auto pit					   = parent->lower_bound(segment);

				// parent already existed
				if (pit != parent->end() && pit->first == segment)
				{
					node& p = pit->second;

					if (auto tbl = p.as_table())
					{
						// adding to closed inline tables is illegal
						if (tbl->is_inline())
						{
							set_error_at(header_begin_pos,
										 "cannot insert '"sv,
										 to_sv(recording_buffer),
										 "' into existing inline table"sv);
							return_after_error({});
						}

						parent = tbl;
					}
					else if (auto arr = p.as_array(); arr && table_arrays.contains(arr))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!arr->empty());
						TOML_ASSERT(arr->back().is_table());
						parent = &arr->back().ref_cast<table>();
					}
					else
					{
						set_error_at(key_buffer.starts[i],
									 "cannot redefine existing "sv,
									 to_sv(p.type()),
									 " as table"sv);
						return_after_error({});
					}
				}

				// need to create a new implicit table
				else
				{
					pit		  = parent->emplace_hint<table>(pit, make_key(i));
					table& p  = pit->second.ref_cast<table>();
					p.source_ = { header_begin_pos, header_end_pos, reader.source_path() };

					implicit_tables.insert(&p);
					parent = &p;
				}
			}

			const auto last_segment = key_buffer.back();
			auto it					= parent->lower_bound(last_segment);

			// if there was already a matching node some sanity checking is necessary;
			// this is ok if we're making an array and the existing element is already an array (new element)
			// or if we're making a table and the existing element is an implicitly-created table (promote it),
			// otherwise this is a redefinition error.
			if (it != parent->end() && it->first == last_segment)
			{
				node& matching_node = it->second;
				if (auto arr = matching_node.as_array(); is_arr && arr && table_arrays.contains(arr))
				{
					table& tbl	= arr->emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				if (auto tbl = matching_node.as_table(); !is_arr && tbl && implicit_tables.contains(tbl))
				{
					bool ok = true;
					if (!tbl->empty())
					{
						for (auto& [_, child] : *tbl)
						{
							if (!child.is_table() && !child.is_array_of_tables())
							{
								ok = false;
								break;
							}
						}
					}

					if (ok)
					{
						implicit_tables.erase(tbl);
						tbl->source_.begin = header_begin_pos;
						tbl->source_.end   = header_end_pos;
						return tbl;
					}
				}

				// if we get here it's a redefinition error.
				if (!is_arr && matching_node.type() == node_type::table)
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
								 to_sv(matching_node.type()),
								 " '"sv,
								 to_sv(recording_buffer),
								 "' as "sv,
								 is_arr ? "array-of-tables"sv : "table"sv);
					return_after_error({});
				}
			}

			// there was no matching node, sweet - we can freely instantiate a new table/table array.
			else
			{
				auto last_key = make_key(key_buffer.size() - 1u);

				// if it's an array we need to make the array and it's first table element,
				// set the starting regions, and return the table element
				if (is_arr)
				{
					it			   = parent->emplace_hint<array>(it, std::move(last_key));
					array& tbl_arr = it->second.ref_cast<array>();
					table_arrays.insert(&tbl_arr);
					tbl_arr.source_ = { header_begin_pos, header_end_pos, reader.source_path() };

					table& tbl	= tbl_arr.emplace_back<table>();
					tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
					return &tbl;
				}

				// otherwise we're just making a table

				it			= parent->emplace_hint<table>(it, std::move(last_key));
				table& tbl	= it->second.ref_cast<table>();
				tbl.source_ = { header_begin_pos, header_end_pos, reader.source_path() };
				return &tbl;
			}
		}

		TOML_NEVER_INLINE
		bool parse_key_value_pair_and_insert(table* tbl)
		{
			return_if_error({});
			assert_not_eof();
			TOML_ASSERT_ASSUME(is_string_delimiter(*cp) || is_bare_key_character(*cp));
			push_parse_scope("key-value pair"sv);

			// read the key into the key buffer
			start_recording();
			parse_key();
			stop_recording_key();
			return_if_error({});
			TOML_ASSERT(key_buffer.size() >= 1u);

			// skip past any whitespace that followed the key
			consume_leading_whitespace();
			set_error_and_return_if_eof({});

			// '='
			if (*cp != U'=')
				set_error_and_return_default("expected '=', saw '"sv, *cp, "'"sv);
			advance_and_return_if_error_or_eof({});

			// skip past any whitespace that followed the '='
			consume_leading_whitespace();
			return_if_error({});
			set_error_and_return_if_eof({});

			// check that the next character could actually be a value
			if (is_value_terminator(*cp))
				set_error_and_return_default("expected value, saw '"sv, *cp, "'"sv);

			// if it's a dotted kvp we need to spawn the parent sub-tables if necessary,
			// and set the target table to the second-to-last one in the chain
			if (key_buffer.size() > 1u)
			{
				for (size_t i = 0; i < key_buffer.size() - 1u; i++)
				{
					const std::string_view segment = key_buffer[i];
					auto pit					   = tbl->lower_bound(segment);

					// parent already existed
					if (pit != tbl->end() && pit->first == segment)
					{
						table* p = pit->second.as_table();

						// redefinition
						if TOML_UNLIKELY(!p || !(dotted_key_tables.contains(p) || implicit_tables.contains(p)))
						{
							set_error_at(key_buffer.starts[i],
										 "cannot redefine existing "sv,
										 to_sv(pit->second.type()),
										 " as dotted key-value pair"sv);
							return_after_error({});
						}

						tbl = p;
					}

					// need to create a new implicit table
					else
					{
						pit		  = tbl->emplace_hint<table>(pit, make_key(i));
						table& p  = pit->second.ref_cast<table>();
						p.source_ = pit->first.source();

						dotted_key_tables.insert(&p);
						tbl = &p;
					}
				}
			}

			// ensure this isn't a redefinition
			const std::string_view last_segment = key_buffer.back();
			auto it								= tbl->lower_bound(last_segment);
			if (it != tbl->end() && it->first == last_segment)
			{
				set_error_at(key_buffer.starts[0],
							 "cannot redefine existing "sv,
							 to_sv(it->second.type()),
							 " '"sv,
							 to_sv(recording_buffer),
							 "'"sv);
				return_after_error({});
			}

			// create the key first since the key buffer will likely get overwritten during value parsing (inline
			// tables)
			auto last_key = make_key(key_buffer.size() - 1u);

			// now we can actually parse the value
			node_ptr val = parse_value();
			return_if_error({});

			tbl->emplace_hint<node_ptr>(it, std::move(last_key), std::move(val));
			return true;
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
						set_error("expected a comment or whitespace, saw '"sv, cp, "'"sv);
				}

				else // ??
					set_error("expected keys, tables, whitespace or comments, saw '"sv, cp, "'"sv);
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
				if (tbl.is_inline()) // inline tables (and all their inline descendants) are already correctly
									 // terminated
					return;

				auto end = nde.source_.end;
				for (auto&& [k, v] : tbl)
				{
					TOML_UNUSED(k);
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
			}
			else // arrays
			{
				auto& arr = nde.ref_cast<array>();
				auto end  = nde.source_.end;
				for (auto&& v : arr)
				{
					update_region_ends(v);
					if (end < v.source_.end)
						end = v.source_.end;
				}
				nde.source_.end = end;
			}
		}

	  public:
		parser(source_reader&& input) //
			: reader{ std::move(input) }
		{
			root.source_ = { source_position{ 1, 1 }, source_position{ 1, 1 }, reader.source_path() };

			land(reader.first());
			if (cp)
				parse_document();

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
	node_ptr parser::parse_array()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_eof({});

		node_ptr arr_ptr{ new array{} };
		array& arr = arr_ptr->ref_cast<array>();
		enum class TOML_CLOSED_ENUM parse_type : int
		{
			none,
			comma,
			val
		};
		parse_type prev = parse_type::none;

		while (!is_error())
		{
			while (consume_leading_whitespace() || consume_line_break() || consume_comment())
				continue;
			set_error_and_return_if_eof({});

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == parse_type::val)
				{
					prev = parse_type::comma;
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
				if (prev == parse_type::val)
					set_error_and_return_default("expected comma or closing ']', saw '"sv, *cp, "'"sv);
				prev = parse_type::val;

				// e.g. '}' or a non-ASCII line break; parse_value() requires a value to start here
				if TOML_UNLIKELY(is_value_terminator(*cp))
					set_error_and_return_default("expected value or closing ']', saw '"sv, *cp, "'"sv);

				auto val = parse_value();
				return_if_error({});

				if (!arr.capacity())
					arr.reserve(4u);
				arr.emplace_back<node_ptr>(std::move(val));
			}
		}

		return_if_error({});
		return arr_ptr;
	}

	TOML_EXTERNAL_LINKAGE
	node_ptr parser::parse_inline_table()
	{
		return_if_error({});
		assert_not_eof();
		TOML_ASSERT_ASSUME(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_eof({});

		node_ptr tbl_ptr{ new table{} };
		table& tbl = tbl_ptr->ref_cast<table>();
		tbl.is_inline(true);

		enum class TOML_CLOSED_ENUM parse_type : int
		{
			none,
			comma,
			kvp
		};
		parse_type prev = parse_type::none;
		while (!is_error())
		{
			while (consume_leading_whitespace() || consume_line_break() || consume_comment())
				continue;
			return_if_error({});
			set_error_and_return_if_eof({});

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == parse_type::kvp)
				{
					prev = parse_type::comma;
					advance_and_return_if_error_or_eof({});
				}
				else
					set_error_and_return_default("expected key-value pair or closing '}', saw comma"sv);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				advance_and_return_if_error({});
				break;
			}

			// key-value pair
			else if (is_string_delimiter(*cp) || is_bare_key_character(*cp))
			{
				if (prev == parse_type::kvp)
					set_error_and_return_default("expected comma or closing '}', saw '"sv, *cp, "'"sv);
				else
				{
					prev = parse_type::kvp;
					parse_key_value_pair_and_insert(&tbl);
				}
			}

			/// ???
			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, *cp, "'"sv);
		}

		return_if_error({});
		return tbl_ptr;
	}

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_IMPL_NAMESPACE_END;

#undef TOML_RETURNS_BY_THROWING
#undef advance_and_return_if_error
#undef advance_and_return_if_error_or_eof
#undef assert_not_eof
#undef assert_not_error
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

//#---------------------------------------------------------------------------------------------------------------------
//# PARSER PUBLIC IMPLEMENTATION
//#---------------------------------------------------------------------------------------------------------------------

TOML_ANON_NAMESPACE_START
{
	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse(source_reader && reader)
	{
		[[maybe_unused]] const impl::default_fenv fenv;
		return impl::parser{ std::move(reader) };
	}

	TOML_NODISCARD
	TOML_INTERNAL_LINKAGE
	parse_result do_parse_file(std::string_view file_path)
	{
#if TOML_EXCEPTIONS
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	throw parse_error(msg, source_position{}, std::make_shared<const std::string>(std::move(path)))
#else
#define TOML_PARSE_FILE_ERROR(msg, path)                                                                               \
	return parse_result(parse_error(msg, source_position{}, std::make_shared<const std::string>(std::move(path))))
#endif

#if defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
#define TOML_OVERALIGNED
#else
#define TOML_OVERALIGNED alignas(32)
#endif

		std::string file_path_str(file_path);

		// open file with a custom-sized stack buffer
		std::ifstream file;
		TOML_OVERALIGNED char file_buffer[sizeof(void*) * 1024u];
		file.rdbuf()->pubsetbuf(file_buffer, sizeof(file_buffer));
#if TOML_WINDOWS && !(defined(__MINGW32__) || defined(__MINGW64__))
		file.open(impl::widen(file_path_str).c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
#else
		file.open(file_path_str, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
#endif
		if (!file.is_open())
			TOML_PARSE_FILE_ERROR("File could not be opened for reading", file_path_str);

		// get size
		const auto file_size = file.tellg();
		if (file_size == -1)
			TOML_PARSE_FILE_ERROR("Could not determine file size", file_path_str);
		file.seekg(0, std::ifstream::beg);

		// read the whole file into memory first if the file isn't too large
		static constexpr auto large_file_threshold = 1024 * 1024 * 2; // 2 MB
		if (file_size <= large_file_threshold)
		{
			const auto size = static_cast<size_t>(file_size);
			const std::unique_ptr<char[]> file_data{ new char[size] };
			file.read(file_data.get(), static_cast<std::streamsize>(file_size));
			if (file.gcount() != static_cast<std::streamsize>(file_size))
				TOML_PARSE_FILE_ERROR("File could not be read", file_path_str);
			return parse(std::string_view{ file_data.get(), size }, std::move(file_path_str));
		}

		// otherwise parse it using the streams
		if (file.peek() == std::ifstream::traits_type::eof())
			TOML_PARSE_FILE_ERROR("File could not be read", file_path_str);
		return parse(file, std::move(file_path_str));

#undef TOML_PARSE_FILE_ERROR
#undef TOML_OVERALIGNED
	}
}
TOML_ANON_NAMESPACE_END;

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::string_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(file_path);
	}

#if TOML_HAS_CHAR8

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, source_path });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string && source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, std::move(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::u8string_view file_path)
	{
		std::string file_path_str;
		file_path_str.resize(file_path.length());
		std::memcpy(file_path_str.data(), file_path.data(), file_path.length());
		return TOML_ANON_NAMESPACE::do_parse_file(file_path_str);
	}

#endif // TOML_HAS_CHAR8

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::istream & doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse_file(std::wstring_view file_path)
	{
		return TOML_ANON_NAMESPACE::do_parse_file(impl::narrow(file_path));
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::wstring_view source_path)
	{
		return TOML_ANON_NAMESPACE::do_parse(TOML_ANON_NAMESPACE::source_reader{ doc, impl::narrow(source_path) });
	}

#endif // TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_PARSER
