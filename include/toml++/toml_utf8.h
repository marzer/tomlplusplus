#pragma once
#include "toml_common.h"
#if !TOML_STRICT
#include "toml_utf8_is_unicode_letter.h"
#endif

namespace TOML_NAMESPACE::impl
{
	[[nodiscard]] TOML_ALWAYS_INLINE_WHEN_STRICT
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
		#if TOML_STRICT

		return codepoint == U'\t'
			|| codepoint <= U' ';

		#else

		//see: https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		//(characters that don't say "is a line-break")

		return codepoint == U'\t'
			|| codepoint == U' '
			|| codepoint == U'\u00A0' // no-break space
			|| codepoint == U'\u1680' // ogham space mark
			|| (codepoint >= U'\u2000' && codepoint <= U'\u200A') // em quad -> hair space
			|| codepoint == U'\u202F' // narrow no-break space
			|| codepoint == U'\u205F' // medium mathematical space
			|| codepoint == U'\u3000' // ideographic space
		;

		#endif
	}

	template <bool including_cr = true>
	[[nodiscard]] TOML_ALWAYS_INLINE_WHEN_STRICT
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		#if TOML_STRICT
		
		if constexpr (including_cr)
		{
			return codepoint == U'\n'
				|| codepoint == U'\r';
		}
		else
			return codepoint == U'\n';

		#else

		//see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		//(characters that say "is a line-break")

		constexpr auto low_range_end = including_cr ? U'\r' : U'\f';

		return (codepoint >= U'\n' && codepoint <= low_range_end)
			|| codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;

		#endif
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_string_delimiter(char32_t codepoint) noexcept
	{
		return codepoint == U'"'
			|| codepoint == U'\'';
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_ascii_letter(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'z')
			|| (codepoint >= U'A' && codepoint <= U'Z');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE_WHEN_STRICT
	constexpr bool is_letter(char32_t codepoint) noexcept
	{
		#if TOML_STRICT
		return is_ascii_letter(codepoint);
		#else
		return is_ascii_letter(codepoint) || is_unicode_letter(codepoint);
		#endif
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_decimal_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'0' && codepoint <= U'9');
	}

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_hex_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'f')
			|| (codepoint >= U'A' && codepoint <= U'F')
			|| is_decimal_digit(codepoint);
	}

	[[nodiscard]]
	constexpr uint32_t hex_digit_to_int(char32_t codepoint) noexcept
	{
		if (is_decimal_digit(codepoint))
			return static_cast<uint32_t>(codepoint - U'0');
		return 10u + static_cast<uint32_t>(codepoint - ((codepoint >= U'a' && codepoint <= U'f') ? U'a' : U'A'));
	}

	[[nodiscard]]
	constexpr bool is_bare_key_character(char32_t codepoint) noexcept
	{
		return is_decimal_digit(codepoint)
			|| codepoint == U'-'
			|| codepoint == U'_'
			|| is_letter(codepoint)
		;
	}

	[[nodiscard]]
	constexpr bool is_value_terminator(char32_t codepoint) noexcept
	{
		return is_line_break(codepoint)
			|| is_whitespace(codepoint)
			|| codepoint == U']'
			|| codepoint == U'}'
			|| codepoint == U','
			|| codepoint == U'#'
		;
	}

	// based on the decoder found here: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
	struct utf8_decoder final
	{
		uint32_t state{};
		uint32_t codepoint{};

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

		[[nodiscard]]
		constexpr bool error() const noexcept
		{
			return state == 12u;
		}

		[[nodiscard]]
		constexpr bool has_code_point() const noexcept
		{
			return state == 0u;
		}

		constexpr void operator () (uint8_t byte) noexcept
		{
			TOML_ASSERT(!error());

			const auto type = state_table[byte];

			codepoint = has_code_point()
				? (0xFFu >> type) & byte
				: (byte & 0x3Fu) | (codepoint << 6);

			state = state_table[state + 256u + type];
		};
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
					&& static_cast<uint8_t>(source[0]) == 0xEF_u8
					&& static_cast<uint8_t>(source[1]) == 0xBB_u8
					&& static_cast<uint8_t>(source[2]) == 0xBF_u8)
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
			constexpr uint8_t operator() () noexcept
			{
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
			explicit utf8_byte_stream(std::basic_istream<CHAR>& stream) noexcept
				: source{ &stream }
			{
				if (*source)
				{
					static constexpr uint8_t bom[] {
						0xEF_u8,
						0xBB_u8,
						0xBF_u8
					};

					using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
					const auto initial_pos = source->tellg();
					size_t bom_pos{};
					auto bom_char = source->get();
					while (*source && bom_char != stream_traits::eof && bom_char == bom[bom_pos])
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
			uint8_t operator() ()
			{
				return static_cast<uint8_t>(source->get());
			}
	};

	struct utf8_codepoint final
	{
		char32_t value;
		TOML_NAMESPACE::document_position position;
		uint8_t byte_count;
		uint8_t bytes[4];

		template <typename CHAR = TOML_NAMESPACE::string_char>
		[[nodiscard]] TOML_ALWAYS_INLINE
		std::basic_string_view<CHAR> as_view() const noexcept
		{
			static_assert(
				sizeof(CHAR) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes), byte_count };
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

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const std::shared_ptr<const TOML_NAMESPACE::string>& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;
	};

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_byte_stream<T> stream;
			utf8_decoder decoder;
			utf8_codepoint prev{}, current{};
			std::shared_ptr<const string> source_path_;

		public:

			template <typename U, typename STR = string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				current.position.line = 1_sz;
				current.position.column = 1_sz;

				if (!source_path.empty())
					source_path_ = std::make_shared<const string>(std::forward<STR>(source_path));
			}

			[[nodiscard]]
			const std::shared_ptr<const string>& source_path() const noexcept override
			{
				return source_path_;
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				if (stream.eof())
					return nullptr;
				if (stream.error())
					throw parse_error{ "The underlying stream entered an error state"s, prev.position, source_path_ };
				if (decoder.error())
					throw parse_error{ "Encountered invalid utf-8 sequence"s, prev.position, source_path_ };

				while (true)
				{
					uint8_t nextByte;
					if constexpr (noexcept(stream()))
					{
						nextByte = stream();
					}
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
							throw parse_error{ "An unspecified error occurred"s, prev.position, source_path_ };
						}
					}
					if (stream.error())
						throw parse_error{ "The underlying stream entered an error state"s, prev.position, source_path_ };

					decoder(nextByte);
					if (decoder.error())
						throw parse_error{ "Encountered invalid utf-8 sequence"s, prev.position, source_path_ };

					current.bytes[current.byte_count++] = nextByte;
					if (decoder.has_code_point())
					{
						prev = current;
						prev.value = static_cast<char32_t>(decoder.codepoint);
						current.byte_count = {};
						current.position.index++;

						if (is_line_break<false>(prev.value))
						{
							current.position.line++;
							current.position.column = 1_sz;
						}
						else
							current.position.column++;

						#undef TOML_NEW_LINE_TEST

						return &prev;
					}

					if (stream.eof())
						throw parse_error{ "Encountered EOF during incomplete utf-8 code point sequence"s, prev.position, source_path_ };
				}
			}
	};

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, TOML_NAMESPACE::string_view) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, TOML_NAMESPACE::string_view) -> utf8_reader<std::basic_istream<CHAR>>;

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_reader_interface& reader;
			struct
			{
				static constexpr size_t capacity = 31; //+1 for the 'head' stored in the underlying reader
				utf8_codepoint buffer[capacity];
				size_t count = {}, first = {};
			}
			history;
			const utf8_codepoint* head = {};
			size_t negative_offset = {};

		public:

			explicit utf8_buffered_reader(utf8_reader_interface& reader_)
				: reader{ reader_ }
			{}

			[[nodiscard]]
			const std::shared_ptr<const TOML_NAMESPACE::string>& source_path() const noexcept override
			{
				return reader.source_path();
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				if (negative_offset)
				{
					negative_offset--;

					// an entry negative offset of 1 just means "replay the current head"
					if (!negative_offset) 
						return head;

					// otherwise step back into the history buffer
					else
						return history.buffer + ((history.first + history.count - negative_offset) % history.capacity);
				}
				else
				{
					// first character read from stream
					if (!history.count && !head) TOML_UNLIKELY
						head = reader.read_next();

					// subsequent characters and not eof
					else if (head)
					{
						if (history.count < history.capacity) TOML_UNLIKELY
							history.buffer[history.count++] = *head;
						else
							history.buffer[(history.first++ + history.capacity) % history.capacity] = *head;

						head = reader.read_next();
					}

					return head;
				}
			}

			[[nodiscard]]
			const utf8_codepoint* step_back(size_t count) noexcept
			{
				TOML_ASSERT(history.count);
				TOML_ASSERT(negative_offset + count <= history.count);
				negative_offset += count;

				return negative_offset
					? history.buffer + ((history.first + history.count - negative_offset) % history.capacity)
					: head;
			}
	};
}
