#pragma once
#include "toml_common.h"

namespace TOML_NAMESPACE
{
	namespace impl
	{
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
				constexpr utf8_byte_stream(std::basic_string_view<CHAR> sv) noexcept
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
				utf8_byte_stream(std::basic_istream<CHAR>& stream) noexcept
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
			document_position position;
			uint8_t byte_count;
			uint8_t bytes[4];

			template <typename CHAR = string_char>
			[[nodiscard]] TOML_ALWAYS_INLINE
			std::basic_string_view<CHAR> as_view() const noexcept
			{
				static_assert(
					sizeof(CHAR) == 1,
					"The string view's underlying character type must be 1 byte in size."
				);

				return byte_count
					? std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes), byte_count }
					: std::basic_string_view<CHAR>{};
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
			virtual const std::shared_ptr<const string>& source_path() const noexcept = 0;

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

				template <typename U>
				utf8_reader(U && source, string_view source_path = {})
					noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
					: stream{ std::forward<U>(source) }
				{
					current.position.line = 1_sz;
					current.position.column = 1_sz;

					if (!source_path.empty())
						source_path_ = std::make_shared<const string>(source_path);
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
							if (prev.value == U'\n')
							{
								current.position.line++;
								current.position.column = 1_sz;
							}
							else
								current.position.column++;

							return &prev;
						}

						if (stream.eof())
							throw parse_error{ "Encountered EOF during incomplete utf-8 code point sequence"s, prev.position, source_path_ };
					}
				}
		};

		template <typename CHAR>
		utf8_reader(std::basic_string_view<CHAR>, string_view) -> utf8_reader<std::basic_string_view<CHAR>>;

		template <typename CHAR>
		utf8_reader(std::basic_istream<CHAR>&, string_view) -> utf8_reader<std::basic_istream<CHAR>>;

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

				utf8_buffered_reader(utf8_reader_interface& reader_)
					: reader{ reader_ }
				{}

				[[nodiscard]]
				const std::shared_ptr<const string>& source_path() const noexcept override
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

		[[nodiscard]]
		constexpr bool is_whitespace(char32_t codepoint) noexcept
		{
			switch (codepoint)
			{
				case U'\t':		[[fallthrough]];
				case U'\v':		[[fallthrough]]; 
				case U'\f':		[[fallthrough]]; 
				case U' ':		[[fallthrough]];
				case U'\u0085':	[[fallthrough]]; // next line
				case U'\u00A0':	[[fallthrough]]; // no-break space
				case U'\u1680':	[[fallthrough]]; // ogham space mark
				case U'\u2000':	[[fallthrough]]; // en quad
				case U'\u2001':	[[fallthrough]]; // em quad
				case U'\u2002':	[[fallthrough]]; // en space
				case U'\u2003':	[[fallthrough]]; // em space
				case U'\u2004':	[[fallthrough]]; // three-per-em space
				case U'\u2005':	[[fallthrough]]; // four-per-em space
				case U'\u2006':	[[fallthrough]]; // six-per-em space
				case U'\u2007':	[[fallthrough]]; // figure space
				case U'\u2008':	[[fallthrough]]; // punctuation space
				case U'\u2009':	[[fallthrough]]; // thin space
				case U'\u200A':	[[fallthrough]]; // hair space
				case U'\u2028':	[[fallthrough]]; // line separator
				case U'\u2029':	[[fallthrough]]; // paragraph separator
				case U'\u202F':	[[fallthrough]]; // narrow no-break space
				case U'\u205F':	[[fallthrough]]; // medium mathematical space
				case U'\u3000':	                 // ideographic space
					return true;
			}
			return false;
		}

		[[nodiscard]]
		constexpr bool is_string_delimiter(char32_t codepoint) noexcept
		{
			return codepoint == U'"'
				|| codepoint == U'\'';
		}

		[[nodiscard]]
		constexpr bool is_line_ending(char32_t codepoint) noexcept
		{
			return codepoint == U'\r'
				|| codepoint == U'\n';
		}

		[[nodiscard]]
		constexpr bool is_digit(char32_t codepoint) noexcept
		{
			return (codepoint >= U'0' && codepoint <= U'9');
		}

		[[nodiscard]]
		constexpr bool is_hex_digit(char32_t codepoint) noexcept
		{
			return (codepoint >= U'a' && codepoint <= U'f')
				|| (codepoint >= U'A' && codepoint <= U'F')
				|| is_digit(codepoint);
		}

		[[nodiscard]]
		constexpr uint32_t hex_digit_to_int(char32_t codepoint) noexcept
		{
			if (is_digit(codepoint))
				return static_cast<uint32_t>(codepoint - U'0');
			return 10u + static_cast<uint32_t>(codepoint - ((codepoint >= U'a' && codepoint <= U'f') ? U'a' : U'A'));
		}

		[[nodiscard]]
		constexpr bool is_bare_key_character(char32_t codepoint) noexcept
		{
			return (codepoint >= U'a' && codepoint <= U'z')
				|| (codepoint >= U'A' && codepoint <= U'Z')
				|| (codepoint >= U'0' && codepoint <= U'9')
				|| codepoint == U'-'
				|| codepoint == U'_'

				// these ranges are not legal bare key characters in the TOML spec but Europe exists so *shrug*
				|| (codepoint >= U'\u00E0' && codepoint <= U'\u00F6')	// LATIN SMALL LETTER A WITH GRAVE -> LATIN SMALL LETTER O WITH DIAERESIS
				|| (codepoint >= U'\u00F8' && codepoint <= U'\u00FE')	// LATIN SMALL LETTER O WITH STROKE -> LATIN SMALL LETTER THORN
				|| (codepoint >= U'\u00C0' && codepoint <= U'\u00D6')	// LATIN CAPITAL LETTER A WITH GRAVE -> LATIN CAPITAL LETTER O WITH DIAERESIS
				|| (codepoint >= U'\u00D8' && codepoint <= U'\u00DE')	// LATIN CAPITAL LETTER O WITH STROKE -> LATIN CAPITAL LETTER THORN
			;
		}

		[[nodiscard]]
		constexpr bool is_value_terminator(char32_t codepoint) noexcept
		{
			return is_line_ending(codepoint)
				|| is_whitespace(codepoint)
				|| codepoint == U']'
				|| codepoint == U'}'
				|| codepoint == U','
				|| codepoint == U'#'
			;
		}
	}
}
