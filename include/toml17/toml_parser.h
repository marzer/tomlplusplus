#pragma once
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table_array.h"

namespace TOML_NAMESPACE
{
	class parse_error final
		: public std::runtime_error
	{
		private:
			document_region region_;

		public:
			parse_error(const std::string& what, document_region&& region) noexcept
				: std::runtime_error{ what },
				region_{ std::move(region) }
			{}

			parse_error(const std::string& what, const document_position& position, const std::shared_ptr<const path>& source_path) noexcept
				: std::runtime_error{ what },
				region_{ position, position, source_path }
			{}

			parse_error(const std::string& what, const document_position& position) noexcept
				: std::runtime_error{ what },
				region_{ position, position }
			{}

			[[nodiscard]]
			const document_region& where() const noexcept
			{
				return region_;
			}
	};

	namespace impl
	{
		using namespace std::literals::string_literals;
		using namespace std::literals::string_view_literals;

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
						static constexpr auto bom = std::array{
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
			std::array<uint8_t, 4> bytes;
		};
		static_assert(std::is_trivial_v<utf8_codepoint>);
		static_assert(std::is_standard_layout_v<utf8_codepoint>);

		struct TOML_INTERFACE utf8_reader_interface
		{
			[[nodiscard]]
			virtual const std::shared_ptr<const path>& source_path() noexcept = 0;

			[[nodiscard]]
			virtual const utf8_codepoint* next() = 0;
		};

		template <typename T>
		class TOML_EMPTY_BASES utf8_reader final
			: public utf8_reader_interface
		{
			private:
				utf8_byte_stream<T> stream;
				utf8_decoder decoder;
				utf8_codepoint prev{}, current{};
				std::shared_ptr<const path> source_path_;

			public:

				template <typename U>
				utf8_reader(U && source, const path& source_path = {})
					noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
					: stream{ std::forward<U>(source) }
				{
					current.position.line = 1_sz;
					current.position.column = 1_sz;

					if (!source_path.empty())
						source_path_ = std::make_shared<const path>(source_path);
				}

				[[nodiscard]]
				const std::shared_ptr<const path>& source_path() noexcept override
				{
					return source_path_;
				}

				[[nodiscard]]
				const utf8_codepoint* next() override
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
		utf8_reader(std::basic_string_view<CHAR>, const path&) -> utf8_reader<std::basic_string_view<CHAR>>;

		template <typename CHAR>
		utf8_reader(std::basic_istream<CHAR>&, const path&) -> utf8_reader<std::basic_istream<CHAR>>;

		[[nodiscard]]
		constexpr bool is_whitespace(char32_t codepoint) noexcept
		{
			switch (codepoint)
			{
				case U'\t':		[[fallthrough]];
				case U'\v':		[[fallthrough]]; 
				case U'\f':		[[fallthrough]]; 
				case U' ':		[[fallthrough]];
				case U'\u0085':	[[fallthrough]]; //next line
				case U'\u00A0':	[[fallthrough]]; //no-break space
				case U'\u1680':	[[fallthrough]]; //ogham space mark
				case U'\u2000':	[[fallthrough]]; //en quad
				case U'\u2001':	[[fallthrough]]; //em quad
				case U'\u2002':	[[fallthrough]]; //en space
				case U'\u2003':	[[fallthrough]]; //em space
				case U'\u2004':	[[fallthrough]]; //three-per-em space
				case U'\u2005':	[[fallthrough]]; //four-per-em space
				case U'\u2006':	[[fallthrough]]; //six-per-em space
				case U'\u2007':	[[fallthrough]]; //figure space
				case U'\u2008':	[[fallthrough]]; //punctuation space
				case U'\u2009':	[[fallthrough]]; //thin space
				case U'\u200A':	[[fallthrough]]; //hair space
				case U'\u2028':	[[fallthrough]]; //line separator
				case U'\u2029':	[[fallthrough]]; //paragraph separator
				case U'\u202F':	[[fallthrough]]; //narrow no-break space
				case U'\u205F':	[[fallthrough]]; //medium mathematical space
				case U'\u3000':	                 //ideographic space
					return true;
			}
			return false;
		}

		[[nodiscard]]
		constexpr bool is_bare_key_character(char32_t codepoint) noexcept
		{
			return (codepoint >= U'a' && codepoint <= U'z')
				|| (codepoint >= U'A' && codepoint <= U'Z')
				|| (codepoint >= U'0' && codepoint <= U'9')
				|| codepoint == U'-'
				|| codepoint == U'_'
				/*
				|| (codepoint >= U'\u00E0' && codepoint <= U'\u00F6')	//LATIN SMALL LETTER A WITH GRAVE -> LATIN SMALL LETTER O WITH DIAERESIS
				|| (codepoint >= U'\u00F8' && codepoint <= U'\u00FE')	//LATIN SMALL LETTER O WITH STROKE -> LATIN SMALL LETTER THORN
				|| (codepoint >= U'\u00C0' && codepoint <= U'\u00D6')	//LATIN CAPITAL LETTER A WITH GRAVE -> LATIN CAPITAL LETTER O WITH DIAERESIS
				|| (codepoint >= U'\u00D8' && codepoint <= U'\u00DE')	//LATIN CAPITAL LETTER O WITH STROKE -> LATIN CAPITAL LETTER THORN
				*/
			;
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

		class parser final
		{
			private:
				utf8_reader_interface& reader_;
				std::shared_ptr<table> root;
				document_position prev_pos = { 1_sz, 1_sz };
				const utf8_codepoint* cp = {};
				std::set<table*> implicit_tables;
				table* current_table;

				void advance()
				{
					TOML_ASSERT(cp);

					prev_pos = cp->position;
					cp = reader_.next();
				}

				bool consume_leading_whitespace()
				{
					if (!cp || !is_whitespace(cp->value))
						return false;

					do
					{
						advance();
					}
					while (cp && is_whitespace(cp->value));
					return true;
				}

				bool consume_line_ending()
				{
					if (!cp || !is_line_ending(cp->value))
						return false;

					if (cp->value == U'\r')
					{
						advance();  //skip \r
						if (!cp)
							throw parse_error{ "Encountered EOF while consuming CRLF"s, prev_pos, reader_.source_path() };
						if (cp->value != U'\n')
							throw parse_error{ "Encountered unexpected character while consuming CRLF"s, cp->position, reader_.source_path() };
					}
					advance(); //skip \n
					return true;
				}

				bool consume_rest_of_line()
				{
					if (!cp)
						return false;

					do
					{
						if (is_line_ending(cp->value))
							return consume_line_ending();
						else
							advance();
					}
					while (cp);
					return true;
				}

				bool consume_comment()
				{
					if (!cp || cp->value != U'#')
						return false;
					return consume_rest_of_line();
				}

				string parse_string()
				{
					TOML_ASSERT(cp && is_string_delimiter(cp->value));

					TOML_NOT_IMPLEMENTED_YET;
				}

				string parse_bare_key_segment()
				{
					TOML_ASSERT(cp && is_string_delimiter(cp->value));

					TOML_NOT_IMPLEMENTED_YET;
				}

				std::vector<string> parse_key()
				{
					TOML_ASSERT(cp && (is_string_delimiter(cp->value) || is_bare_key_character(cp->value)));

					TOML_NOT_IMPLEMENTED_YET;

					return {};
				}

				struct table_header
				{
					std::vector<string> key;
					bool is_array;
				};

				table_header parse_table_header()
				{
					TOML_ASSERT(cp && cp->value == U'[');

					const auto start_pos = prev_pos;
					do
					{
						//skip first [
						advance();
						if (!cp)
							break;

						//skip second [ (if present)
						table_header header{};
						if (cp->value == U'[')
						{
							advance();
							if (!cp)
								break;
							header.is_array = true;
						}

						//skip past any whitespace that followed the [
						if (consume_leading_whitespace() && !cp)
							break;

						//get the actual key
						header.key = parse_key();

						//skip past any whitespace that followed the key
						if (consume_leading_whitespace() && !cp)
							break;

						//consume the first closing ]
						if (cp->value != U']')
							throw parse_error{ "Encountered unexpected character while parsing table header"s, prev_pos, reader_.source_path() };
						advance();

						//consume the second closing ]
						if (header.is_array)
						{
							if (cp->value != U']')
								throw parse_error{ "Encountered unexpected character while parsing table header"s, prev_pos, reader_.source_path() };
							advance();
						}

						//handle the rest of the line after the table 
						consume_leading_whitespace();
						if ((!consume_comment() && !consume_line_ending()) && cp)
							throw parse_error{ "Encountered unexpected character while parsing table header"s, prev_pos, reader_.source_path() };

						return header;
					}
					while (false);

					throw parse_error{ "Encountered EOF while parsing table header"s, start_pos, reader_.source_path() };
				}

				void parse_key_value_pair()
				{
					TOML_ASSERT(cp && (is_string_delimiter(cp->value) || is_bare_key_character(cp->value)));

					TOML_NOT_IMPLEMENTED_YET;
				}

				bool is_implicit_table(node* node) const noexcept
				{
					return node
						&& node->is_table()
						&& implicit_tables.find(node->reinterpret_as_table()) != implicit_tables.end();
				};

				void parse_table_or_table_array()
				{
					TOML_ASSERT(cp && cp->value == U'[');

					const auto table_start_position = prev_pos;
					auto header = parse_table_header();
					TOML_ASSERT(!header.key.empty());

					if (header.is_array)
					{
						TOML_NOT_IMPLEMENTED_YET;
					}
					else
					{
						auto super = root.get();
						for (size_t i = 0; i < header.key.size() - 1_sz; i++)
						{
							//each super table must either not exist already, or be a table
							auto next_super = super->get(header.key[i]);
							if (!next_super)
							{
								next_super = super->values.emplace(
									string{ header.key[i] },
									std::make_shared<table>()
								).first->second.get();
								implicit_tables.insert(next_super->reinterpret_as_table());
								next_super->region_ = { table_start_position, table_start_position, reader_.source_path() };
							}
							else if (!next_super->is_table())
							{
								//throw parse_error{ "FFFFFFFFFFFFF"s, prev_pos, reader_.source_path() };
								TOML_NOT_IMPLEMENTED_YET;
							}
							super = next_super->reinterpret_as_table();
						}

						//the last table in the key must not exist already, or be a table that was created implicitly
						{
							auto matching_node = super->get(header.key.back());
							if (matching_node)
							{
								if (!matching_node->is_table())
								{
									//redefinition error
									TOML_NOT_IMPLEMENTED_YET;
								}
								else if (!is_implicit_table(matching_node))
								{
									//invalid ordering error
									TOML_NOT_IMPLEMENTED_YET;
								}

								//table was implicit; promote it to explicit and update the region info so it matches the explicit definition
								implicit_tables.erase(matching_node->reinterpret_as_table());
								matching_node->region_.begin = matching_node->region_.end = table_start_position;
							}
							else
							{
								matching_node = super->values.emplace(
									string{ header.key.back() },
									std::make_shared<table>()
								).first->second.get();
								matching_node->region_ = { table_start_position, table_start_position, reader_.source_path() };
							}
						}
					}
				}

				void parse_document()
				{
					TOML_ASSERT(cp);
					
					do
					{
						// leading whitespace, line endings, comments
						if (consume_leading_whitespace()
							|| consume_line_ending()
							|| consume_comment())
							continue;

						// [tables]
						// [[array of tables]]
						else if (cp->value == U'[')
						{
							parse_table_or_table_array();
						}

						// bare_keys
						// dotted.keys
						// "quoted keys"
						else if (is_string_delimiter(cp->value)
							|| is_bare_key_character(cp->value))
						{
							parse_key_value_pair();
						}

						else //??
							throw parse_error{ "Encountered unexpected character while parsing document"s, prev_pos, reader_.source_path() };

					}
					while (cp);
				}

			public:

				parser(utf8_reader_interface&& reader)
					: reader_{ reader },
					root{ std::make_shared<table>() }
				{
					root->region_ = { prev_pos, prev_pos, reader_.source_path() };
					current_table = root.get();

					cp = reader_.next();
					if (cp)
						parse_document();
				}

				[[nodiscard]]
				operator std::shared_ptr<table>() const noexcept
				{
					return root;
				}
		};
	}

	template <typename CHAR>
	inline std::shared_ptr<table> parse(std::basic_string_view<CHAR> doc, const path& source_path = {})
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The string view's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}

	template <typename CHAR>
	inline std::shared_ptr<table> parse(std::basic_istream<CHAR>& doc, const path& source_path = {})
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}
}
