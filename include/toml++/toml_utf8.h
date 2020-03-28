//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de> (utf8_decoder)
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_common.h"
#include "toml_utf8_generated.h"

namespace toml::impl
{
	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_whitespace(char32_t codepoint) noexcept
	{
		return codepoint == U'\t' || codepoint == U' ';
	}

	[[nodiscard]]
	constexpr bool is_unicode_whitespace(char32_t codepoint) noexcept
	{
		// see: https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that don't say "is a line-break")

		return codepoint == U'\u00A0' // no-break space
			|| codepoint == U'\u1680' // ogham space mark
			|| (codepoint >= U'\u2000' && codepoint <= U'\u200A') // em quad -> hair space
			|| codepoint == U'\u202F' // narrow no-break space
			|| codepoint == U'\u205F' // medium mathematical space
			|| codepoint == U'\u3000' // ideographic space
		;
	}

	[[nodiscard]]
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
		return is_ascii_whitespace(codepoint) || is_unicode_whitespace(codepoint);
	}

	template <bool CR = true>
	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_line_break(char32_t codepoint) noexcept
	{
		constexpr auto low_range_end = CR ? U'\r' : U'\f';
		return (codepoint >= U'\n' && codepoint <= low_range_end);
	}

	[[nodiscard]]
	constexpr bool is_unicode_line_break(char32_t codepoint) noexcept
	{
		// see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		// (characters that say "is a line-break")

		return codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;
	}

	template <bool CR = true>
	[[nodiscard]]
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		return is_ascii_line_break<CR>(codepoint) || is_unicode_line_break(codepoint);
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_string_delimiter(char32_t codepoint) noexcept
	{
		return codepoint == U'"' || codepoint == U'\'';
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_letter(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'z')
			|| (codepoint >= U'A' && codepoint <= U'Z');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_binary_digit(char32_t codepoint) noexcept
	{
		return codepoint == U'0' || codepoint == U'1';
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_octal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'7');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_decimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'9');
	}

	[[nodiscard]]
	constexpr bool is_hexadecimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'f')
			|| (codepoint >= U'A' && codepoint <= U'F')
			|| is_decimal_digit(codepoint)
		;
	}

	[[nodiscard]]
	constexpr bool is_bare_key_start_character(char32_t codepoint) noexcept
	{
		return is_ascii_letter(codepoint)
			|| is_decimal_digit(codepoint)
			|| codepoint == U'-'
			|| codepoint == U'_'
			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/644 & toml/issues/687
			|| codepoint == U'+'
			|| is_unicode_letter(codepoint)
			|| is_unicode_number(codepoint)
			#endif
		;
	}

	[[nodiscard]]
	constexpr bool is_bare_key_character(char32_t codepoint) noexcept
	{
		return is_bare_key_start_character(codepoint)
			#if TOML_LANG_HIGHER_THAN(0, 5, 0) // toml/issues/687
			|| is_unicode_combining_mark(codepoint)
			#endif
		;
	}

	[[nodiscard]]
	constexpr bool is_value_terminator(char32_t codepoint) noexcept
	{
		return is_ascii_line_break(codepoint)
			|| is_ascii_whitespace(codepoint)
			|| codepoint == U']'
			|| codepoint == U'}'
			|| codepoint == U','
			|| codepoint == U'#'
			|| is_unicode_line_break(codepoint)
			|| is_unicode_whitespace(codepoint)
		;
	}

	struct utf8_decoder final
	{
		//# This decoder is based on the 'Flexible and Economical UTF-8 Decoder'
		//# Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
		//# See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

		uint_least32_t state{};
		char32_t codepoint{};

		static constexpr uint8_t state_table[]
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,		11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

			0,12,24,36,60,96,84,12,12,12,48,72,		12,12,12,12,12,12,12,12,12,12,12,12,
			12, 0,12,12,12,12,12, 0,12, 0,12,12,	12,24,12,12,12,12,12,24,12,24,12,12,
			12,12,12,12,12,12,12,24,12,12,12,12,	12,24,12,12,12,12,12,12,12,24,12,12,
			12,12,12,12,12,12,12,36,12,36,12,12,	12,36,12,12,12,12,12,36,12,36,12,12,
			12,36,12,12,12,12,12,12,12,12,12,12
		};

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool has_code_point() const noexcept
		{
			return state == uint_least32_t{};
		}

		[[nodiscard]] TOML_ALWAYS_INLINE
		constexpr bool needs_more_input() const noexcept
		{
			return state > uint_least32_t{} && state != uint_least32_t{ 12u };
		}

		constexpr void operator () (uint8_t byte) noexcept
		{
			TOML_ASSERT(!error());

			const auto type = state_table[byte];

			codepoint = static_cast<char32_t>(
				has_code_point()
					? (uint_least32_t{ 255u } >> type) & byte
					: (byte & uint_least32_t{ 63u }) | (static_cast<uint_least32_t>(codepoint) << 6)
				);

			state = state_table[state + uint_least32_t{ 256u } + type];
		}
	};

	template <typename T>
	class utf8_byte_stream;

	template <typename CHAR>
	class utf8_byte_stream<std::basic_string_view<CHAR>> final
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_string_view<CHAR> source;
			size_t position = {};

		public:
			explicit constexpr utf8_byte_stream(std::basic_string_view<CHAR> sv) noexcept
				: source{ sv }
			{
				if (source.length() >= 3_sz
					&& static_cast<uint8_t>(source[0]) == static_cast<uint8_t>(0xEFu)
					&& static_cast<uint8_t>(source[1]) == static_cast<uint8_t>(0xBBu)
					&& static_cast<uint8_t>(source[2]) == static_cast<uint8_t>(0xBFu))
				{
					position += 3_sz;
				}
			}

			[[nodiscard]]
			constexpr bool eof() const noexcept
			{
				return position >= source.length();
			}

			[[nodiscard]]
			constexpr bool error() const noexcept
			{
				return false;
			}

			[[nodiscard]]
			constexpr optional<uint8_t> operator() () noexcept
			{
				if (position >= source.length())
					return {};
				return static_cast<uint8_t>(source[position++]);
			}
	};

	template <typename CHAR>
	class utf8_byte_stream<std::basic_istream<CHAR>> final
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_istream<CHAR>* source;

		public:
			explicit utf8_byte_stream(std::basic_istream<CHAR>& stream)
				: source{ &stream }
			{
				if (*source)
				{
					static constexpr uint8_t bom[] {
						0xEF,
						0xBB,
						0xBF
					};

					using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
					const auto initial_pos = source->tellg();
					size_t bom_pos{};
					auto bom_char = source->get();
					while (*source && bom_char != stream_traits::eof() && bom_char == bom[bom_pos])
					{
						bom_pos++;
						bom_char = source->get();
					}
					if (!(*source) || bom_pos < 3_sz)
						source->seekg(initial_pos);
				}
			}

			[[nodiscard]]
			bool eof() const noexcept
			{
				return source->eof();
			}

			[[nodiscard]]
			bool error() const noexcept
			{
				return !(*source);
			}

			[[nodiscard]]
			optional<uint8_t> operator() ()
			{
				auto val = source->get();
				if (val == std::basic_istream<CHAR>::traits_type::eof())
					return {};
				return static_cast<uint8_t>(val);
			}
	};

	struct utf8_codepoint final
	{
		char32_t value;
		string_char bytes[4];
		source_position position;

		template <typename CHAR = string_char>
		[[nodiscard]] TOML_ALWAYS_INLINE
		std::basic_string_view<CHAR> as_view() const noexcept
		{
			static_assert(
				sizeof(CHAR) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(bytes), 4_sz }
				: std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(bytes) };
		}

		[[nodiscard]]
		constexpr operator char32_t& () noexcept
		{
			return value;
		}

		[[nodiscard]]
		constexpr operator const char32_t& () const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK	(void)0
		#define TOML_ERROR			throw parse_error
		#if TOML_ABI_NAMESPACES
			inline namespace abi_impl_ex {
		#endif
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR			err.emplace
		#if TOML_ABI_NAMESPACES
			inline namespace abi_impl_noex {
		#endif
	#endif

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const source_path_ptr& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;

		#if !TOML_EXCEPTIONS

		[[nodiscard]]
		virtual optional<parse_error>&& error() noexcept = 0;

		#endif

		virtual ~utf8_reader_interface() noexcept = default;
	};

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_byte_stream<T> stream;
			utf8_decoder decoder;
			utf8_codepoint codepoints[2];
			size_t cp_idx = 1;
			uint8_t current_byte_count{};
			source_path_ptr source_path_;
			#if !TOML_EXCEPTIONS
			optional<parse_error> err;
			#endif

		public:

			template <typename U, typename STR = std::string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				std::memset(codepoints, 0, sizeof(codepoints));
				codepoints[0].position = { 1, 1 };
				codepoints[1].position = { 1, 1 };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<STR>(source_path));
			}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
			{
				return source_path_;
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				TOML_ERROR_CHECK;

				auto& prev = codepoints[(cp_idx - 1_sz) % 2_sz];

				if (stream.eof())
					return nullptr;
				else if (stream.error())
					TOML_ERROR("An error occurred while reading from the underlying stream", prev.position, source_path_ );
				else if (decoder.error())
					TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

				TOML_ERROR_CHECK;

				while (true)
				{
					optional<uint8_t> nextByte;
					if constexpr (noexcept(stream()) || !TOML_EXCEPTIONS)
					{
						nextByte = stream();
					}
					#if TOML_EXCEPTIONS
					else
					{
						try
						{
							nextByte = stream();
						}
						catch (const std::exception& exc)
						{
							throw parse_error{ exc.what(), prev.position, source_path_ };
						}
						catch (...)
						{
							throw parse_error{ "An unspecified error occurred", prev.position, source_path_ };
						}
					}
					#endif

					if (!nextByte)
					{
						if (stream.eof())
						{
							if (decoder.needs_more_input())
								TOML_ERROR("Encountered EOF during incomplete utf-8 code point sequence",
									prev.position, source_path_);
							return nullptr;
						}
						else
							TOML_ERROR("An error occurred while reading from the underlying stream",
								prev.position, source_path_);
					}

					TOML_ERROR_CHECK;

					decoder(*nextByte);
					if (decoder.error())
						TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

					TOML_ERROR_CHECK;

					auto& current = codepoints[cp_idx % 2_sz];
					current.bytes[current_byte_count++] = static_cast<string_char>(*nextByte);
					if (decoder.has_code_point())
					{
						//store codepoint
						current.value = decoder.codepoint;

						//reset prev (will be the next 'current')
						std::memset(prev.bytes, 0, sizeof(prev.bytes));
						current_byte_count = {};
						if (is_line_break<false>(current.value))
							prev.position = { static_cast<source_index>(current.position.line + 1), 1 };
						else
							prev.position = { current.position.line, static_cast<source_index>(current.position.column + 1) };
						cp_idx++;
						return &current;
					}
				}

				TOML_UNREACHABLE;
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
			{
				return std::move(err);
			}

			#endif
	};

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, std::string_view) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, std::string_view) -> utf8_reader<std::basic_istream<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, std::string&&) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, std::string&&) -> utf8_reader<std::basic_istream<CHAR>>;

	#if !TOML_EXCEPTIONS
		#undef TOML_ERROR_CHECK
		#define TOML_ERROR_CHECK	if (reader.error()) return nullptr
	#endif

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		public:
			static constexpr size_t max_history_length = 64;

		private:
			static constexpr size_t history_buffer_size = max_history_length - 1; //'head' is stored in the reader
			utf8_reader_interface& reader;
			struct
			{
				
				utf8_codepoint buffer[history_buffer_size];
				size_t count, first;
			}
			history = {};
			const utf8_codepoint* head = {};
			size_t negative_offset = {};

		public:

			explicit utf8_buffered_reader(utf8_reader_interface& reader_) noexcept
				: reader{ reader_ }
			{}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
			{
				return reader.source_path();
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				TOML_ERROR_CHECK;

				if (negative_offset)
				{
					negative_offset--;

					// an entry negative offset of 1 just means "replay the current head"
					if (!negative_offset) 
						return head;

					// otherwise step back into the history buffer
					else
						return history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size);
				}
				else
				{
					// first character read from stream
					if (!history.count && !head) TOML_UNLIKELY
						head = reader.read_next();

					// subsequent characters and not eof
					else if (head)
					{
						if (history.count < history_buffer_size) TOML_UNLIKELY
							history.buffer[history.count++] = *head;
						else
							history.buffer[(history.first++ + history_buffer_size) % history_buffer_size] = *head;

						head = reader.read_next();
					}

					return head;
				}
			}

			[[nodiscard]]
			const utf8_codepoint* step_back(size_t count) noexcept
			{
				TOML_ERROR_CHECK;
				TOML_ASSERT(history.count);
				TOML_ASSERT(negative_offset + count <= history.count);

				negative_offset += count;

				return negative_offset
					? history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size)
					: head;
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
			{
				return reader.error();
			}

			#endif
	};

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif
}

