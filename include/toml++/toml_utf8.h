#pragma once
#include "toml_common.h"
#if !TOML_STRICT
#include "toml_utf8_is_unicode_letter.h"
#endif

namespace toml::impl
{
	[[nodiscard]]
	constexpr bool is_whitespace(char32_t codepoint) noexcept
	{
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
	}

	template <bool including_cr = true>
	[[nodiscard]]
	constexpr bool is_line_break(char32_t codepoint) noexcept
	{
		//see https://en.wikipedia.org/wiki/Whitespace_character#Unicode
		//(characters that say "is a line-break")

		constexpr auto low_range_end = including_cr ? U'\r' : U'\f';

		return (codepoint >= U'\n' && codepoint <= low_range_end)
			|| codepoint == U'\u0085' // next line
			|| codepoint == U'\u2028' // line separator
			|| codepoint == U'\u2029' // paragraph separator
		;
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

	[[nodiscard]] TOML_ALWAYS_INLINE
	constexpr bool is_hex_digit(char32_t codepoint) noexcept
	{
		return (codepoint >= U'a' && codepoint <= U'f')
			|| (codepoint >= U'A' && codepoint <= U'F')
			|| is_decimal_digit(codepoint);
	}

	[[nodiscard]] TOML_ALWAYS_INLINE_WHEN_STRICT
	constexpr bool is_bare_key_start_character(char32_t codepoint) noexcept
	{
		return is_ascii_letter(codepoint)
			|| is_decimal_digit(codepoint)
			|| codepoint == U'-'
			|| codepoint == U'_'
			#if !TOML_STRICT
			|| is_unicode_letter(codepoint)
			|| is_unicode_number(codepoint)
			#endif
		;
	}

	[[nodiscard]] TOML_ALWAYS_INLINE_WHEN_STRICT
	constexpr bool is_bare_key_character(char32_t codepoint) noexcept
	{
		return is_bare_key_start_character(codepoint)
			#if !TOML_STRICT
			|| is_unicode_combining_mark(codepoint)
			#endif
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

		[[nodiscard]]
		constexpr bool error() const noexcept
		{
			return state == uint_least32_t{ 12u };
		}

		[[nodiscard]]
		constexpr bool has_code_point() const noexcept
		{
			return state == uint_least32_t{};
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
		uint8_t bytes[4];
		toml::document_position position;

		template <typename CHAR = toml::string_char>
		[[nodiscard]] TOML_ALWAYS_INLINE
		std::basic_string_view<CHAR> as_view() const noexcept
		{
			static_assert(
				sizeof(CHAR) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes), 4_sz }
				: std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR* const>(bytes) };
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
		virtual const std::shared_ptr<const toml::string>& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;

		virtual ~utf8_reader_interface() noexcept = default;
	};

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_byte_stream<T> stream;
			utf8_decoder decoder;
			utf8_codepoint prev{}, current{};
			uint8_t current_byte_count{};
			std::shared_ptr<const string> source_path_;

		public:

			template <typename U, typename STR = string_view>
			explicit utf8_reader(U && source, STR&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				current.position = { 1u, 1u };

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
					throw parse_error{ "The underlying stream entered an error state", prev.position, source_path_ };
				if (decoder.error())
					throw parse_error{ "Encountered invalid utf-8 sequence", prev.position, source_path_ };

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
							throw parse_error{ "An unspecified error occurred", prev.position, source_path_ };
						}
					}
					if (stream.error())
						throw parse_error{ "The underlying stream entered an error state", prev.position, source_path_ };

					decoder(nextByte);
					if (decoder.error())
						throw parse_error{ "Encountered invalid utf-8 sequence", prev.position, source_path_ };

					current.bytes[current_byte_count++] = nextByte;
					if (decoder.has_code_point())
					{
						prev = current;
						prev.value = decoder.codepoint;
						current_byte_count = {};

						if (is_line_break<false>(prev.value))
						{
							current.position.line++;
							current.position.column = 1u;
						}
						else
							current.position.column++;

						return &prev;
					}

					if (stream.eof())
						throw parse_error{ "Encountered EOF during incomplete utf-8 code point sequence", prev.position, source_path_ };
				}
			}
	};

	template <typename CHAR>
	utf8_reader(std::basic_string_view<CHAR>, toml::string_view) -> utf8_reader<std::basic_string_view<CHAR>>;

	template <typename CHAR>
	utf8_reader(std::basic_istream<CHAR>&, toml::string_view) -> utf8_reader<std::basic_istream<CHAR>>;

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		public:
			static constexpr auto max_history_length = 64_sz;

		private:
			static constexpr auto history_buffer_size = max_history_length - 1_sz; //the 'head' is stored in the underlying reader
			utf8_reader_interface& reader;
			struct
			{
				
				utf8_codepoint buffer[history_buffer_size];
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
			const std::shared_ptr<const toml::string>& source_path() const noexcept override
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
				TOML_ASSERT(history.count);
				TOML_ASSERT(negative_offset + count <= history.count);
				negative_offset += count;

				return negative_offset
					? history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size)
					: head;
			}
	};
}
