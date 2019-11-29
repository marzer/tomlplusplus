#pragma once
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table_array.h"

namespace TOML17_NAMESPACE
{
	class parse_error final
		: std::runtime_error
	{
		private:
			document_region where_;

		public:
			parse_error(const std::string& what, document_region&& where) noexcept
				: std::runtime_error{ what },
				where_{ std::move(where) }
			{}

			parse_error(const std::string& what, document_position&& where) noexcept
				: std::runtime_error{ what },
				where_{ where, where }
			{}

			[[nodiscard]]
			const document_region& where() const noexcept
			{
				return where_;
			}
	};

	namespace impl
	{
		using namespace std::literals::string_literals;
		using namespace std::literals::string_view_literals;

		[[nodiscard]] TOML17_ALWAYS_INLINE
		constexpr uint32_t operator"" _cp(char c) noexcept
		{
			return static_cast<uint32_t>(c);
		}

		//based on the decoder found here: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
		struct utf8_decoder final
		{
			uint32_t state{};
			uint32_t codepoint{};

			static constexpr auto state_table = std::array<uint8_t, 364>
			{{
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
			}};

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
				assert(!error());

				const auto type = state_table[byte];

				codepoint = has_code_point()
					? (0xFFu >> type) & byte
					: (byte & 0x3Fu) | (codepoint << 6);

				state = state_table[state + 256u + type];
			};
		};

		template <typename T>
		struct utf8_byte_stream;

		template <typename CHAR>
		struct utf8_byte_stream<std::basic_string_view<CHAR>> final
		{
			static_assert(sizeof(CHAR) == 1_sz);

			std::basic_string_view<CHAR> source;
			size_t position = {};

			utf8_byte_stream(std::basic_string_view<CHAR> sv) noexcept
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
			bool eof() const noexcept
			{
				return position >= source.length();
			}

			[[nodiscard]]
			constexpr bool error() const noexcept
			{
				return false;
			}

			[[nodiscard]]
			uint8_t operator() () noexcept
			{
				return static_cast<uint8_t>(source[position++]);
			}
		};

		template <typename CHAR>
		struct utf8_byte_stream<std::basic_istream<CHAR>> final
		{
			static_assert(sizeof(CHAR) == 1_sz);

			std::basic_istream<CHAR>& source;

			utf8_byte_stream(std::basic_istream<CHAR>& stream)
				: source{ stream }
			{
				if (stream)
				{
					std::array<uint8_t, 3> bom;
					const auto intialPos = stream.tellg();

					size_t i{};
					for (; i < 3 && stream; i++)
						bom[i] = static_cast<uint8_t>(source.get());

					if (i < 3
						|| bom[0] != 0xEF_u8
						|| bom[1] != 0xBB_u8
						|| bom[2] != 0xBF_u8)
						stream.seekg(initialPos);
				}
			}

			[[nodiscard]]
			bool eof() const noexcept
			{
				return source.eof();
			}

			[[nodiscard]]
			bool error() const noexcept
			{
				return !source;
			}

			[[nodiscard]]
			uint8_t operator() ()
			{
				return static_cast<uint8_t>(source.get());
			}
		};

		struct utf8_codepoint final
		{
			uint32_t value;
			document_position position;
		};

		
		struct TOML17_INTERFACE utf8_reader_interface
		{
			[[nodiscard]]
			virtual const utf8_codepoint* next() = 0;
		};

		template <typename T>
		class TOML17_EMPTY_BASES utf8_reader final
			: public utf8_reader_interface
		{
			private:
				utf8_byte_stream<T> stream;
				utf8_decoder decoder;
				document_position next_position{};
				utf8_codepoint last_codepoint;

			public:

				template <typename U>
				utf8_reader(U && source)
					noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
					: stream{ std::forward<U>(source) }
				{}

				[[nodiscard]]
				const utf8_codepoint* next() override
				{
					if (stream.eof())
						return nullptr;
					if (stream.error())
						throw parse_error{ "The underlying stream entered an error state."s, last_codepoint.position };
					if (decoder.error())
						throw parse_error{ "Encountered invalid utf-8 sequence."s, last_codepoint.position };

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
								throw parse_error{ exc.what(), last_codepoint.position };
							}
							catch (...)
							{
								throw parse_error{ "An unspecified error occurred."s, last_codepoint.position };
							}
						}
						if (stream.error())
							throw parse_error{ "The underlying stream entered an error state."s, last_codepoint.position };

						decoder(nextByte);
						if (decoder.error())
							throw parse_error{ "Encountered invalid utf-8 sequence."s, last_codepoint.position };

						if (decoder.has_code_point())
						{
							last_codepoint.value = decoder.codepoint;
							last_codepoint.position = next_position;

							next_position.position++;
							if (last_codepoint.value == '\n'_cp)
							{
								next_position.line++;
								next_position.column = {};
							}
							else
								next_position.column++;
							return &last_codepoint;
						}

						if (stream.eof())
							throw parse_error{ "Incomplete utf-8 code point at EOF."s, last_codepoint.position };
					}
				}
		};

		template <typename CHAR>
		utf8_reader(std::basic_string_view<CHAR>) -> utf8_reader<std::basic_string_view<CHAR>>;


		inline void parse_table_body(utf8_reader_interface& doc, std::shared_ptr<table>& table)
		{

		}
	}

	inline std::shared_ptr<table> parse(std::string_view doc)
	{
		impl::utf8_reader reader{ doc };
		auto rootTable = std::make_shared<table>();
		impl::parse_table_body(reader, rootTable);
		return rootTable;
	}

	#ifdef __cpp_lib_char8_t

	inline std::shared_ptr<table> parse(std::u8string_view doc)
	{
		impl::utf8_reader reader{ doc };
		auto rootTable = std::make_shared<table>();
		impl::parse_table_body(reader, rootTable);
		return rootTable;
	}

	#endif
}
