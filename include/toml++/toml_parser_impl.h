//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

#pragma once
#include "toml_parser.h"
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <cmath>
#if TOML_INTEGER_CHARCONV || TOML_FLOATING_POINT_CHARCONV
	#include <charconv>
#endif
#if !TOML_INTEGER_CHARCONV || !TOML_FLOATING_POINT_CHARCONV
	#include <sstream>
#endif
TOML_POP_WARNINGS

namespace toml::impl
{
	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK(...)	(void)0
		#define TOML_ERROR				throw toml::parse_error
	#else
		#define TOML_ERROR_CHECK(...)	if (err) return __VA_ARGS__
		#define TOML_ERROR				err.emplace
	#endif

	#if !TOML_EXCEPTIONS || defined(__INTELLISENSE__)
		#define TOML_NORETURN
	#else
		#define TOML_NORETURN			[[noreturn]]
	#endif

	template <uint64_t> struct parse_integer_traits;
	template <> struct parse_integer_traits<2> final
	{
		static constexpr auto qualifier = "binary"sv;
		static constexpr auto is_digit = is_binary_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 63;
		static constexpr char32_t prefix_codepoint = U'b';
		static constexpr char prefix = 'b';
	};
	template <> struct parse_integer_traits<8> final
	{
		static constexpr auto qualifier = "octal"sv;
		static constexpr auto is_digit = is_octal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 21; // strlen("777777777777777777777")
		static constexpr char32_t prefix_codepoint = U'o';
		static constexpr char prefix = 'o';
	};
	template <> struct parse_integer_traits<10> final
	{
		static constexpr auto qualifier = "decimal"sv;
		static constexpr auto is_digit = is_decimal_digit;
		static constexpr auto is_signed = true;
		static constexpr auto buffer_length = 19; //strlen("9223372036854775807")
	};
	template <> struct parse_integer_traits<16> final
	{
		static constexpr auto qualifier = "hexadecimal"sv;
		static constexpr auto is_digit = is_hexadecimal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 16; //strlen("7FFFFFFFFFFFFFFF")
		static constexpr char32_t prefix_codepoint = U'x';
		static constexpr char prefix = 'x';
	};

	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_impl_ex {
		#else
			inline namespace abi_impl_noex {
		#endif
	#endif
	#ifdef NDEBUG
		#define TOML_NOT_EOF TOML_ASSUME(cp != nullptr)
	#else
		#define TOML_NOT_EOF TOML_ASSERT(cp != nullptr)
	#endif

	class parser final
	{
		private:
			utf8_buffered_reader reader;
			table root;
			source_position prev_pos = { 1, 1 };
			const utf8_codepoint* cp = {};
			std::vector<table*> implicit_tables;
			std::vector<table*> dotted_key_tables;
			std::vector<array*> table_arrays;
			std::string recording_buffer; //for diagnostics 
			bool recording = false;
			#if !TOML_EXCEPTIONS
			mutable optional<toml::parse_error> err;
			#endif

			[[nodiscard]]
			source_position current_position(source_index fallback_offset = 0) const noexcept
			{
				if (cp)
					return cp->position;
				return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
			}

			template <typename T, size_t N>
			static void abort_with_error_concatenate(T&& arg, char(& buf)[N], char*& ptr) noexcept
			{
				(void)buf;

				using arg_t = remove_cvref_t<T>;
				if constexpr (std::is_same_v<arg_t, std::string_view> || std::is_same_v<arg_t, std::string>)
				{
					std::memcpy(ptr, arg.data(), arg.length());
					ptr += arg.length();
				}
				else if constexpr (std::is_same_v<arg_t, utf8_codepoint>)
				{
					toml::string_view cp_view;
					if (arg.value <= U'\x1F') TOML_UNLIKELY
						cp_view = low_character_escape_table[arg.value];
					else if (arg.value == U'\x7F')  TOML_UNLIKELY
						cp_view = TOML_STRING_PREFIX("\\u007F"sv);
					else
						cp_view = arg.template as_view<string_char>();

					std::memcpy(ptr, cp_view.data(), cp_view.length());
					ptr += cp_view.length();
				}
				else if constexpr (std::is_same_v<arg_t, char>)
				{
					*ptr++ = arg;
				}
				else if constexpr (std::is_same_v<arg_t, bool>)
				{
					const auto boolval = arg ? "true"sv : "false"sv;
					std::memcpy(ptr, boolval.data(), boolval.length());
					ptr += boolval.length();
				}
				else if constexpr (std::is_same_v<arg_t, node_type>)
				{
					const auto str = impl::node_type_friendly_names[
						static_cast<std::underlying_type_t<node_type>>(arg)
					];
					std::memcpy(ptr, str.data(), str.length());
					ptr += str.length();
				}
				else if constexpr (std::is_floating_point_v<arg_t>)
				{
					#if TOML_FLOATING_POINT_CHARCONV
					{
						const auto result = std::to_chars(ptr, buf + N, arg);
						ptr = result.ptr;
					}
					#else
					{
						std::ostringstream ss;
						ss.imbue(std::locale::classic());
						ss.precision(std::numeric_limits<arg_t>::digits10 + 1);
						ss << arg;
						const auto str = std::move(ss).str();
						std::memcpy(ptr, str.c_str(), str.length());
						ptr += str.length();
					}
					#endif
				}
				else if constexpr (std::is_integral_v<arg_t>)
				{
					#if TOML_INTEGER_CHARCONV
					{
						const auto result = std::to_chars(ptr, buf + N, arg);
						ptr = result.ptr;
					}
					#else
					{
						std::ostringstream ss;
						ss.imbue(std::locale::classic());
						using cast_type = std::conditional_t<std::is_signed_v<arg_t>, int64_t, uint64_t>;
						ss << static_cast<cast_type>(arg);
						const auto str = std::move(ss).str();
						std::memcpy(ptr, str.c_str(), str.length());
						ptr += str.length();
					}
					#endif
				}
			}

			template <typename... T>
			TOML_NORETURN
			void abort_with_error(T &&... args) const TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();

				if constexpr (sizeof...(T) == 0_sz)
					TOML_ERROR( "An unspecified error occurred", current_position(1), reader.source_path() );
				else
				{
					static constexpr auto buf_size = 512_sz;
					char buf[buf_size];
					auto ptr = buf;
					(abort_with_error_concatenate(std::forward<T>(args), buf, ptr), ...);
					*ptr = '\0';
					#if TOML_EXCEPTIONS
						TOML_ERROR(buf, current_position(1), reader.source_path());
					#else
						TOML_ERROR(std::string(buf, static_cast<size_t>(ptr - buf)), current_position(1), reader.source_path());
					#endif
				}
			}

			TOML_NEVER_INLINE
			void go_back(size_t count = 1_sz) noexcept
			{
				TOML_ERROR_CHECK();
				TOML_ASSERT(count);

				cp = reader.step_back(count);
				prev_pos = cp->position;
			}

			TOML_NEVER_INLINE
			void advance() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();
				TOML_NOT_EOF;

				prev_pos = cp->position;
				cp = reader.read_next();

				#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
				#endif

				if (recording && cp)
					recording_buffer.append(cp->as_view<char>());
			}

			TOML_NEVER_INLINE
			void start_recording(bool include_current = true) noexcept
			{
				TOML_ERROR_CHECK();

				recording = true;
				recording_buffer.clear();
				if (include_current && cp)
					recording_buffer.append(cp->as_view<char>());
			}

			TOML_NEVER_INLINE
			void stop_recording(size_t pop_bytes = 0_sz) noexcept
			{
				TOML_ERROR_CHECK();

				recording = false;
				if (pop_bytes)
				{
					if (pop_bytes >= recording_buffer.length())
						recording_buffer.clear();
					else
						recording_buffer.erase(
							recording_buffer.begin() + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
							recording_buffer.end()
						);
				}
			}

			TOML_NEVER_INLINE
			bool consume_leading_whitespace() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				bool consumed = false;
				while (cp && is_whitespace(*cp))
				{
					consumed = true;
					advance();
					TOML_ERROR_CHECK({});
				}
				return consumed;
			}

			TOML_NEVER_INLINE
			bool consume_line_break() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp || !is_line_break(*cp))
					return false;

				if (*cp == U'\r')
				{
					advance();  // skip \r
					TOML_ERROR_CHECK({});

					if (!cp)
						abort_with_error("Encountered EOF while consuming CRLF"sv);
					else if (*cp != U'\n')
						abort_with_error("Encountered unexpected character while consuming CRLF"sv);
				}
				advance(); // skip \n (or other single-character line ending)
				TOML_ERROR_CHECK({});

				return true;
			}

			TOML_NEVER_INLINE
			bool consume_rest_of_line() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp)
					return false;

				do
				{
					if (is_line_break(*cp))
						return consume_line_break();
					else
						advance();

					TOML_ERROR_CHECK({});
				}
				while (cp);
				return true;
			}

			TOML_NEVER_INLINE
			bool consume_comment() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				if (!cp || *cp != U'#')
					return false;

				advance(); //skip the '#'
				TOML_ERROR_CHECK({});

				while (cp)
				{
					if (consume_line_break())
						return true;

					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						// toml/issues/567 (disallow non-TAB control characters in comments)
						if (is_nontab_control_character(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing comment; control characters "
								"other than TAB (U+0009) are explicitly prohibited from appearing "
								"in comments."sv
							);

						// toml/pull/720 (disallow surrogates in comments)
						else if (is_unicode_surrogate(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing comment; unicode surrogates "
								"(U+D800 to U+DFFF) are explicitly prohibited from appearing in comments."sv
							);
					}

					advance();
					TOML_ERROR_CHECK({});
				}

				return true;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			bool consume_expected_sequence(std::u32string_view seq) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				for (auto c : seq)
				{
					if (!cp || *cp != c)
						return false;

					advance();
					TOML_ERROR_CHECK({});
				}
				return true;
			}

			template <typename T, size_t N>
			[[nodiscard]]
			TOML_NEVER_INLINE
			bool consume_digit_sequence(T(&digits)[N]) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				for (size_t i = 0; i < N; i++)
				{
					if (!cp || !is_decimal_digit(*cp))
						return false;
					digits[i] = static_cast<T>(*cp - U'0');
					advance();
					TOML_ERROR_CHECK({});
				}
				return true;
			}

			template <uint64_t base = 10, typename T, size_t N>
			[[nodiscard]]
			TOML_NEVER_INLINE
			size_t consume_variable_length_digit_sequence(T(&buffer)[N]) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				using traits = parse_integer_traits<base>;

				size_t i = {};
				for (; i < N; i++)
				{
					if (!cp || !traits::is_digit(*cp))
						break;
					buffer[i] = static_cast<T>(*cp - U'0');
					advance();
					TOML_ERROR_CHECK({});
				}
				return i;
			}

			template <bool MultiLine>
			[[nodiscard]]
			TOML_NEVER_INLINE
			string parse_basic_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(*cp == U'"');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv,
							(MultiLine ? "multi-line "sv : ""sv),
							node_type::string
						);
				};

				// skip the '"'
				advance();
				eof_check();
				TOML_ERROR_CHECK({});

				string str;
				bool escaped = false;
				[[maybe_unused]] bool skipping_whitespace = false;
				while (cp)
				{
					if (escaped)
					{
						escaped = false;

						// handle 'line ending slashes' in multi-line mode
						if constexpr (MultiLine)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								skipping_whitespace = true;
								TOML_ERROR_CHECK({});
								continue;
							}
						}

						// skip the escaped character
						const auto escaped_codepoint = cp->value;
						advance();
						eof_check();
						TOML_ERROR_CHECK({});

						TOML_NOT_EOF;
						switch (escaped_codepoint)
						{
							// 'regular' escape codes
							case U'b': str += TOML_STRING_PREFIX('\b'); break;
							case U'f': str += TOML_STRING_PREFIX('\f'); break;
							case U'n': str += TOML_STRING_PREFIX('\n'); break;
							case U'r': str += TOML_STRING_PREFIX('\r'); break;
							case U't': str += TOML_STRING_PREFIX('\t'); break;
							case U'"': str += TOML_STRING_PREFIX('"'); break;
							case U'\\': str += TOML_STRING_PREFIX('\\'); break;

							// unicode scalar sequences
							case U'x':
								if constexpr (!TOML_LANG_UNRELEASED) // toml/pull/709 (\xHH unicode scalar sequences)
								{
									abort_with_error("Escape sequence '\\x' is not supported "
										"in TOML 1.0.0 and earlier."sv
									);
									break;
								}
								[[fallthrough]];
							case U'u': [[fallthrough]];
							case U'U':
							{
								uint32_t place_value = escaped_codepoint == U'U' ? 0x10000000u : (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
								uint32_t sequence_value{};
								while (place_value)
								{
									eof_check();
									TOML_ERROR_CHECK({});

									TOML_NOT_EOF;
									if (!is_hexadecimal_digit(*cp))
									{
										abort_with_error(
											"Encountered unexpected character while parsing "sv,
											(MultiLine ? "multi-line "sv : ""sv),
											node_type::string,
											"; expected hex digit, saw '\\"sv, *cp, "'"sv
										);
										break;
									}
									sequence_value += place_value * hex_to_dec(*cp);
									place_value /= 16u;
									advance();
									TOML_ERROR_CHECK({});
								}

								if (is_unicode_surrogate(sequence_value))
									abort_with_error(
										"Encountered illegal unicode scalar sequence while parsing "sv,
										(MultiLine ? "multi-line "sv : ""sv),
										node_type::string,
										"; unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited."sv
									);
								else if (sequence_value > 0x10FFFFu)
									abort_with_error(
										"Encountered illegal unicode scalar sequence while parsing "sv,
										(MultiLine ? "multi-line "sv : ""sv),
										node_type::string,
										"; scalar values greater than U+10FFFF are invalid."sv
									);
								else if (sequence_value <= 0x7Fu) //ascii
									str += static_cast<string_char>(sequence_value & 0x7Fu);
								else if (sequence_value <= 0x7FFu)
								{
									str += static_cast<string_char>(0xC0u | ((sequence_value >> 6) & 0x1Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}
								else if (sequence_value <= 0xFFFFu)
								{
									str += static_cast<string_char>(0xE0u | ((sequence_value >> 12) & 0x0Fu));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 6) & 0x1Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}
								else
								{
									str += static_cast<string_char>(0xF0u | ((sequence_value >> 18) & 0x07u));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 12) & 0x3Fu));
									str += static_cast<string_char>(0x80u | ((sequence_value >> 6) & 0x3Fu));
									str += static_cast<string_char>(0x80u | (sequence_value & 0x3Fu));
								}
								break;
							}

							// ???
							default:
								abort_with_error(
									"Encountered unexpected character while parsing "sv,
									(MultiLine ? "multi-line "sv : ""sv),
									node_type::string,
									"; unknown escape sequence '\\"sv, *cp, "'"sv
								);
						}
					}
					else TOML_LIKELY
					{
						// handle closing delimiters
						if (*cp == U'"')
						{
							if constexpr (MultiLine)
							{
								advance();
								eof_check();
								TOML_ERROR_CHECK({});
								const auto second = cp->value;

								advance();
								eof_check();
								TOML_ERROR_CHECK({});
								const auto third = cp->value;

								if (second == U'"' && third == U'"')
								{
									advance(); // skip the third closing delimiter
									TOML_ERROR_CHECK({});

									//multi-line basic strings are allowed one additional terminating '"'
									//so that things like this work: """this is a "quote""""
									if (cp && *cp == U'"')
									{
										str += TOML_STRING_PREFIX('"');
										advance(); // skip the final closing delimiter
										TOML_ERROR_CHECK({});
									}

									return str;
								}
								else
								{
									str += TOML_STRING_PREFIX('"');
									go_back(1_sz);
									skipping_whitespace = false;
									continue;
								}
							}
							else
							{
								advance(); // skip the closing delimiter
								TOML_ERROR_CHECK({});
								return str;
							}
						}

						// handle escapes
						else if (*cp == U'\\')
						{
							advance(); // skip the '\'
							TOML_ERROR_CHECK({});
							skipping_whitespace = false;
							escaped = true;
							continue;
						}

						// handle line endings in multi-line mode
						if constexpr (MultiLine)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								TOML_ERROR_CHECK({});
								if (!str.empty() && !skipping_whitespace)
									str += TOML_STRING_PREFIX('\n');
								continue;
							}
						}

						// handle control characters
						if (is_nontab_control_character(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing "sv,
								(MultiLine ? "multi-line "sv : ""sv),
								node_type::string,
								"; unescaped control characters other than TAB (U+0009) are explicitly "
								"prohibited from appearing in strings."sv
							);

						// handle surrogates in strings (1.0.0 and later)
						if constexpr (TOML_LANG_AT_LEAST(1, 0, 0)) 
						{
							if (is_unicode_surrogate(*cp))
								abort_with_error(
									"Encountered unexpected character while parsing "sv,
									(MultiLine ? "multi-line "sv : ""sv),
									node_type::string,
									"; unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly "
									"prohibited from appearing in strings."sv
								);
						}
						TOML_ERROR_CHECK({});

						if constexpr (MultiLine)
						{
							if (!skipping_whitespace || !is_whitespace(*cp))
							{
								skipping_whitespace = false;
								str.append(cp->as_view());
							}
						}
						else
							str.append(cp->as_view());

						advance();
						TOML_ERROR_CHECK({});
					}
				}

				abort_with_error(
					"Encountered EOF while parsing "sv,
					(MultiLine ? "multi-line "sv : ""sv),
					node_type::string
				);
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			template <bool MultiLine>
			[[nodiscard]]
			TOML_NEVER_INLINE
			string parse_literal_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(*cp == U'\'');

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv,
							(MultiLine ? "multi-line "sv : ""sv),
							"literal "sv, node_type::string
						);
				};

				// skip the delimiter
				advance();
				eof_check();
				TOML_ERROR_CHECK({});

				string str;
				while (cp)
				{
					TOML_ERROR_CHECK({});

					// handle closing delimiters
					if (*cp == U'\'')
					{
						if constexpr (MultiLine)
						{
							advance();
							eof_check();
							TOML_ERROR_CHECK({});
							const auto second = cp->value;

							advance();
							eof_check();
							TOML_ERROR_CHECK({});
							const auto third = cp->value;

							if (second == U'\'' && third == U'\'')
							{
								advance(); // skip the third closing delimiter
								TOML_ERROR_CHECK({});
								return str;
							}
							else
							{
								str += TOML_STRING_PREFIX('\'');
								go_back(1_sz);
								continue;
							}
						}
						else
						{
							advance(); // skip the closing delimiter
							TOML_ERROR_CHECK({});
							return str;
						}
					}

					// handle line endings in multi-line mode
					if constexpr (MultiLine)
					{
						if (is_line_break(*cp))
						{
							consume_line_break();
							if (!str.empty())
								str += TOML_STRING_PREFIX('\n');
							continue;
						}
					}

					// handle control characters
					if (is_nontab_control_character(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing "sv,
							(MultiLine ? "multi-line "sv : ""sv),
							"literal "sv, node_type::string,
							"; control characters other than TAB (U+0009) are explicitly prohibited from appearing "
							"in literal strings."sv
						);

					// handle surrogates in strings (1.0.0 and later)
					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						if (is_unicode_surrogate(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing "sv,
								(MultiLine ? "multi-line "sv : ""sv),
								"literal "sv, node_type::string,
								"; unicode surrogates (U+D800 - U+DFFF) are explicitly "
								"prohibited from appearing in literal strings."sv
							);
					}
					TOML_ERROR_CHECK({});

					str.append(cp->as_view());
					advance();
					TOML_ERROR_CHECK({});
				}

				abort_with_error("Encountered EOF while parsing "sv,
					(MultiLine ? "multi-line "sv : ""sv),
					"literal "sv, node_type::string
				);
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			string parse_string() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_string_delimiter(*cp));

				// get the first three characters to determine the string type
				const auto first = cp->value;
				advance();
				TOML_ERROR_CHECK({});
				if (!cp)
				{
					abort_with_error("Encountered EOF while parsing "sv, node_type::string);
					TOML_ERROR_CHECK({});
					TOML_UNREACHABLE;
				}
				const auto second = cp->value;
				advance();
				TOML_ERROR_CHECK({});
				const auto third = cp ? cp->value : U'\0';

				// if we were eof at the third character then first and second need to be
				// the same string character (otherwise it's an unterminated string)
				if (!cp)
				{
					if (second == first)
						return string{};

					abort_with_error("Encountered EOF while parsing "sv, node_type::string);
					TOML_ERROR_CHECK({});
					TOML_UNREACHABLE;
				}
					
				// if the first three characters are all the same string delimiter then
				// it's a multi-line string.
				else if (first == second && first == third)
				{
					return first == U'\''
						? parse_literal_string<true>()
						: parse_basic_string<true>();
				}

				// otherwise it's just a regular string.
				else
				{
					// step back two characters so that the current
					// character is the string delimiter
					go_back(2_sz);

					return first == U'\''
						? parse_literal_string<false>()
						: parse_basic_string<false>();
				}
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			string parse_bare_key_segment() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_bare_key_start_character(*cp));

				string segment;

				while (cp)
				{
					if (!is_bare_key_character(*cp))
						break;

					segment.append(cp->as_view());
					advance();
					TOML_ERROR_CHECK({});
				}

				return segment;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			bool parse_bool() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_match(*cp, U't', U'f', U'T', U'F'));

				start_recording(true);
				auto result = is_match(*cp, U't', U'T');
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				{
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::boolean);
					else
						abort_with_error(
							"Encountered unexpected character while parsing "sv, node_type::boolean,
							"; expected '"sv, result ? "true"sv : "false"sv, "', saw '"sv, recording_buffer, '\''
						);
				}
				stop_recording();
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::boolean,
						"; expected value-terminator, saw '"sv,
						*cp, '\''
					);

				TOML_ERROR_CHECK({});
				return result;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			double parse_inf_or_nan() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::floating_point);
				};

				start_recording(true);
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
				{
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
				}

				const bool inf = is_match(*cp, U'i', U'I');
				if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				{
					eof_check();
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::floating_point,
						"; expected '"sv, inf ? "inf"sv : "nan"sv, "', saw '"sv, recording_buffer, '\''
					);
				}
				stop_recording();
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::floating_point,
						"; expected value-terminator, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});

				return inf
					? sign * std::numeric_limits<double>::infinity()
					: std::numeric_limits<double>::quiet_NaN();
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_SWITCH_WARNINGS
			TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
			TOML_NEVER_INLINE
			double parse_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_match(*cp, U'+', U'-') || is_decimal_digit(*cp));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::floating_point);
				};

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
				{
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
				}

				// consume value chars
				char chars[64];
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent = false;
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
						{
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; underscores may only follow digits"sv
							);
							break;
						}
						prev = cp;
						advance();
						TOML_ERROR_CHECK({});
						continue;
					}

					if (*cp == U'.')
					{
						// 1.0e+.10 (exponent cannot have '.')
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; expected exponent decimal digit or sign, saw '.'"sv
							);

						// 1.0.e+.10
						// 1..0
						// (multiple '.')
						else if (seen_decimal)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; expected decimal digit or exponent, saw , saw '.'"sv
							);

						seen_decimal = true;
					}
					else if (is_match(*cp, U'e', U'E'))
					{
						// 1.0ee+10 (multiple 'e')
						if (seen_exponent)
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::floating_point,
								"; expected decimal digit, saw '"sv, *cp, '\''
							);

						seen_decimal = true; // implied
						seen_exponent = true;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 1.-0 (sign in mantissa)
						if (!seen_exponent)
							abort_with_error(
							"Encountered unexpected character while parsing "sv, node_type::floating_point,
							"; expected decimal digit or '.', saw '"sv, * cp, '\''
						);

						// 1.0e1-0 (misplaced exponent sign)
						else if (!is_match(*prev, U'e', U'E'))
							abort_with_error(
								"Encountered unexpected character while parsing parsing "sv, node_type::floating_point,
								"; expected exponent digit, saw '"sv, *cp, '\''
							);
					}
					else if (!is_decimal_digit(*cp))
						abort_with_error("Encountered unexpected character while parsing "sv,
							node_type::floating_point, "; expected decimal digit, saw '"sv, *cp, '\''
						);
					else if (length == sizeof(chars))
						abort_with_error(
							"Error parsing "sv, node_type::floating_point,
							"; exceeds maximum length of "sv, sizeof(chars), " characters"sv
						);
					TOML_ERROR_CHECK({});

					chars[length++] = static_cast<char>(cp->bytes[0]);
					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				TOML_ERROR_CHECK({});

				// sanity-check ending state
				if (prev)
				{
					if (*prev == U'_')
					{
						eof_check();
						abort_with_error(
							"Error parsing "sv, node_type::floating_point, "; trailing underscores are not allowed"sv
						);
					}
					else if (is_match(*prev, U'e', U'E', U'+', U'-'))
					{
						eof_check();
						abort_with_error(
							"Encountered unexpected character while parsing parsing "sv, node_type::floating_point,
							"; expected exponent digit, saw '"sv, *cp, '\''
						);
					}
				}
				TOML_ERROR_CHECK({});

				// convert to double
				double result;
				#if TOML_FLOATING_POINT_CHARCONV
				{
					auto fc_result = std::from_chars(chars, chars + length, result);
					switch (fc_result.ec)
					{
						case std::errc{}: //ok
							return result * sign;

						case std::errc::invalid_argument:
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
							);
							break;

						case std::errc::result_out_of_range:
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; '"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
							);
							break;

						default: //??
							abort_with_error(
								"Error parsing "sv, node_type::floating_point,
								"; an unspecified error occurred while trying to interpret '"sv,
								std::string_view{ chars, length }, "' as a value"sv
							);
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
						abort_with_error(
							"Error parsing "sv, node_type::floating_point,
							"; '"sv, std::string_view{ chars, length }, "' could not be interpreted as a value"sv
						);
				}
				#endif
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			double parse_hex_float() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_match(*cp, U'0', U'+', U'-'));

				#if TOML_LANG_UNRELEASED // toml/issues/562 (hexfloats)

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing hexadecimal "sv, node_type::floating_point);
				};

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
				{
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
				}

				// '0'
				if (*cp != U'0')
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal "sv, node_type::floating_point,
						"; expected '0', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();

				// 'x' or 'X'
				if (!is_match(*cp, U'x', U'X'))
					abort_with_error(
						"Encountered unexpected character while parsing hexadecimal "sv, node_type::floating_point,
						"; expected 'x' or 'X', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();
				TOML_ERROR_CHECK({});

				// <HEX DIGITS> ([.]<HEX DIGITS>)? [pP] [+-]? <DEC DIGITS>

				// consume value fragments
				struct fragment
				{
					char chars[24];
					size_t length;
					double value;
				};
				fragment fragments[] =
				{
					{}, // mantissa, whole part
					{}, // mantissa, fractional part
					{}  // exponent
				};
				fragment* current_fragment = fragments;
				const utf8_codepoint* prev = {};
				int exponent_sign = 1;
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_hexadecimal_digit(*prev))
						{
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; underscores may only follow digits"sv
							);
							break;
						}

						prev = cp;
						advance();
						TOML_ERROR_CHECK({});
						continue;
					}
					
					if (*cp == U'.')
					{
						// 0x10.0p-.0 (exponent cannot have '.')
						if (current_fragment == fragments + 2)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected exponent digit or sign, , saw '.'"sv
							);

						// 0x10.0.p-0 (multiple '.')
						else if (current_fragment == fragments + 1)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected hexadecimal digit or exponent, , saw '.'"sv
							);
						else
							current_fragment++;
					}
					else if (is_match(*cp, U'p', U'P'))
					{
						// 0x10.0pp-0 (multiple 'p')
						if (current_fragment == fragments + 2)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected exponent digit or sign, saw '"sv, *cp, '\''
							);

						// 0x.p-0 (mantissa is just '.')
						else if (fragments[0].length == 0_sz && fragments[1].length == 0_sz)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected hexadecimal digit, saw '"sv, *cp, '\''
							);
						else
							current_fragment = fragments + 2;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 0x-10.0p-0 (sign in mantissa)
						if (current_fragment != fragments + 2)
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected hexadecimal digit or '.', saw '"sv, *cp, '\''
							);

						// 0x10.0p0- (misplaced exponent sign)
						else if (!is_match(*prev, U'p', U'P'))
							abort_with_error(
								"Encountered unexpected character while parsing hexadecimal "sv,
								node_type::floating_point, "; expected exponent digit, saw '"sv, *cp, '\''
							);
						else
							exponent_sign = *cp == U'-' ? -1 : 1;
					}
					else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
						abort_with_error("Encountered unexpected character while parsing hexadecimal "sv,
							node_type::floating_point, "; expected hexadecimal digit or '.', saw '"sv, *cp, '\''
						);
					else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
						abort_with_error("Encountered unexpected character while parsing hexadecimal "sv,
							node_type::floating_point, "; expected exponent digit or sign, saw '"sv, *cp, '\''
						);
					else if (current_fragment->length == sizeof(fragment::chars))
						abort_with_error(
							"Error parsing hexadecimal "sv, node_type::floating_point,
							"; fragment exceeeds maximum length of "sv, sizeof(fragment::chars), " characters"sv
						);
					else
						current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);
					TOML_ERROR_CHECK({});

					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				TOML_ERROR_CHECK({});

				// sanity-check ending state
				if (current_fragment != fragments + 2 || current_fragment->length == 0_sz)
				{
					eof_check();
					abort_with_error(
						"Error parsing hexadecimal "sv, node_type::floating_point,
						"; missing exponent"sv
					);
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Error parsing hexadecimal "sv, node_type::floating_point,
						"; trailing underscores are not allowed"sv
					);
				}
				TOML_ERROR_CHECK({});

				// calculate values for the three fragments
				static constexpr auto calc_value = [](fragment& f, auto fragment_idx) noexcept
				{
					static constexpr uint32_t base = decltype(fragment_idx)::value == 2 ? 10 : 16;

					// left-trim zeroes
					const char* c = f.chars;
					size_t sig = {};
					while (f.length && *c == '0')
					{
						f.length--;
						c++;
						sig++;
					}
					if (!f.length)
						return;

					// calculate value
					auto place = 1u;
					for (size_t i = 0; i < f.length - 1_sz; i++)
						place *= base;
					uint32_t val{};
					while (place)
					{
						if constexpr (base == 16)
							val += place * hex_to_dec(*c);
						else
							val += place * static_cast<uint32_t>(*c - '0');
						if constexpr (decltype(fragment_idx)::value == 1)
							sig++;
						c++;
						place /= base;
					}
					f.value = static_cast<double>(val);

					// shift the fractional part
					if constexpr (decltype(fragment_idx)::value == 1)
					{
						while (sig--)
							f.value /= base;
					}
				};
				calc_value(fragments[0], std::integral_constant<int, 0>{});
				calc_value(fragments[1], std::integral_constant<int, 1>{});
				calc_value(fragments[2], std::integral_constant<int, 2>{});

				return (fragments[0].value + fragments[1].value)
					* pow(2.0, fragments[2].value * exponent_sign)
					* sign;
			
				#else // !TOML_LANG_UNRELEASED

				TOML_ERROR(
					"Hexadecimal floating-point values are not supported "
					"in TOML 1.0.0 and earlier.",
					cp->position,
					reader.source_path()
				);
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;

				#endif // !TOML_LANG_UNRELEASED
			}

			template <uint64_t base>
			[[nodiscard]]
			TOML_NEVER_INLINE
			int64_t parse_integer() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				using traits = parse_integer_traits<base>;

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv, traits::qualifier, ' ', node_type::integer
						);
				};

				[[maybe_unused]] int64_t sign = 1;
				if constexpr (traits::is_signed)
				{
					if (*cp == U'-')
					{
						sign = -1;
						advance();
					}
					else if(*cp == U'+')
						advance();
					eof_check();

					TOML_ERROR_CHECK({});
				}

				if constexpr (base == 10)
				{
					if (!traits::is_digit(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier, ' ',
							node_type::integer, "; expected expected "sv, traits::qualifier,
							" digit or sign, saw '"sv, *cp, '\''
						);
				}
				else
				{
					// '0'
					if (*cp != U'0')
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier,
							' ', node_type::integer, "; expected '0', saw '"sv, *cp, '\''
						);
					advance();
					eof_check();

					// 'b', 'o', 'x'
					if (*cp != traits::prefix_codepoint)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, traits::qualifier,
							' ', node_type::integer, "; expected '"sv, traits::prefix,
							"', saw '"sv, *cp, '\''
						);
					advance();
					eof_check();
				}

				TOML_ERROR_CHECK({});

				// consume value chars
				char chars[traits::buffer_length];
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !traits::is_digit(*prev))
							abort_with_error(
								"Encountered unexpected character while parsing "sv, traits::qualifier,
								' ', node_type::integer, "; expected "sv, traits::qualifier, " digit, saw '_'"sv
							);
					}
					else
					{
						if (!traits::is_digit(*cp))
							abort_with_error(
								"Encountered unexpected character while parsing "sv, traits::qualifier,
								' ', node_type::integer, "; expected "sv, traits::qualifier,
								" digit, saw '"sv, *cp, '\''
							);
						else if (length == sizeof(chars))
							abort_with_error(
								"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
								"; exceeds maximum length of "sv, sizeof(chars), " characters"sv
							);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}

					prev = cp;
					advance();
					TOML_ERROR_CHECK({});
				}
				if (prev && *prev == U'_')
				{
					eof_check();
					abort_with_error(
						"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
						"; trailing underscores are not allowed"sv
					);
				}

				// check for leading zeroes
				if constexpr (base == 10)
				{
					if (chars[0] == '0')
						abort_with_error(
							"Error parsing "sv, traits::qualifier,
							' ', node_type::integer, "; leading zeroes are not allowed"sv
						);
				}
				
				TOML_ERROR_CHECK({});

				// single digits can be converted trivially
				if (length == 1_sz)
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
				if (result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + (sign < 0 ? 1ull : 0ull))
					abort_with_error(
						"Error parsing "sv, traits::qualifier, ' ', node_type::integer,
						"; '"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
					);
				else
				{
					if constexpr (traits::is_signed)
						return static_cast<int64_t>(result) * sign;
					else
						return static_cast<int64_t>(result);
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			date parse_date(bool part_of_datetime = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_decimal_digit(*cp));

				const auto type = part_of_datetime ? node_type::date_time : node_type::date;
				const auto eof_check = [this, type]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error(
							"Encountered EOF while parsing "sv, type);
				};

				// "YYYY"
				uint32_t year_digits[4];
				if (!consume_digit_sequence(year_digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 4-digit year, saw '"sv, *cp, '\''
						);
				}
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				const auto year = year_digits[3]
					+ year_digits[2] * 10u
					+ year_digits[1] * 100u
					+ year_digits[0] * 1000u;
				const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));

				// '-'
				if (*cp != U'-')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected '-', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// "MM"
				uint32_t month_digits[2];
				if (!consume_digit_sequence(month_digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 2-digit month, saw '"sv, *cp, '\''
						);
				}
				const auto month = month_digits[1] + month_digits[0] * 10u;
				if (month == 0u || month > 12u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected month between 1 and 12 (inclusive), saw "sv, month
					);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// '-'
				if (*cp != U'-')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected '-', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// "DD"
				uint32_t day_digits[2];
				if (!consume_digit_sequence(day_digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 2-digit day, saw '"sv, *cp, '\''
						);
				}
				const auto day = day_digits[1] + day_digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					abort_with_error("Error parsing "sv, type,
						"; expected day between 1 and "sv, max_days_in_month, " (inclusive), saw "sv, day
					);

				if (!part_of_datetime)
				{
					if (cp && !is_value_terminator(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected value-terminator, saw '"sv, *cp, '\''
						);
				}

				TOML_ERROR_CHECK({});

				return
				{
					static_cast<uint16_t>(year),
					static_cast<uint8_t>(month),
					static_cast<uint8_t>(day)
				};
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			time parse_time(bool part_of_datetime = false) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_decimal_digit(*cp));

				const auto type = part_of_datetime ? node_type::date_time : node_type::date;
				const auto eof_check = [this, type]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, type);
				};

				// "HH"
				uint32_t digits[2];
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 2-digit hour, saw '"sv, *cp, '\''
						);
				}
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected hour between 0 to 59 (inclusive), saw "sv, hour
					);
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// ':'
				if (*cp != U':')
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected ':', saw '"sv, *cp, '\''
					);
				advance();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// "MM"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 2-digit minute, saw '"sv,
							*cp, '\''
						);
				}
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected minute between 0 and 59 (inclusive), saw "sv, minute
					);
				auto time = ::toml::time{
					static_cast<uint8_t>(hour),
					static_cast<uint8_t>(minute),
				};
				TOML_ERROR_CHECK({});

				// ':'
				if constexpr (TOML_LANG_UNRELEASED) // toml/issues/671 (allow omission of seconds)
				{
					if (cp
						&& !is_value_terminator(*cp)
						&& *cp != U':'
						&& (!part_of_datetime || !is_match(*cp, U'+', U'-', U'Z')))
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected ':'"sv, (part_of_datetime ? ", offset"sv : ""sv),
							" or value-terminator, saw '"sv, *cp, '\''
						);
					else if (!cp || *cp != U':')
						return time;
				}
				else
				{
					eof_check();

					if (cp && *cp != U':')
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected ':', saw '"sv, *cp, '\''
						);
				}
				advance();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// "SS"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					if (cp)
						abort_with_error(
							"Encountered unexpected character while parsing "sv, type,
							"; expected 2-digit second, saw '"sv,
							*cp, '\''
						);
				}
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					abort_with_error(
						"Error parsing "sv, type,
						"; expected second between 0 and 59 (inclusive), saw "sv, second
					);
				time.second = static_cast<uint8_t>(second);
				TOML_ERROR_CHECK({});

				//early exit here if the fractional is omitted
				if (cp
					&& !is_value_terminator(*cp)
					&& *cp != U'.'
					&& (!part_of_datetime || !is_match(*cp, U'+', U'-', U'Z')))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected fractional"sv,
						(part_of_datetime ? ", offset"sv : ""sv),
						" or value-terminator, saw '"sv, *cp, '\''
					);
				else if (!cp || *cp != U'.')
					return time;
				TOML_ERROR_CHECK({});

				// '.'
				advance();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// ".FFFFFFFFF"
				static constexpr auto max_fractional_digits = 9_sz;
				uint32_t fractional_digits[max_fractional_digits];
				auto digit_count = consume_variable_length_digit_sequence(fractional_digits);
				if (!digit_count)
					abort_with_error(
						"Encountered unexpected character while parsing "sv, type,
						"; expected fractional digits, saw '"sv,
						*cp, '\''
					);
				else if (digit_count == max_fractional_digits && cp && is_decimal_digit(*cp))
					abort_with_error(
						"Error parsing "sv, type,
						"Fractional component exceeds maximum precision of "sv, max_fractional_digits
					);
				TOML_ERROR_CHECK({});

				uint32_t value = 0u;
				uint32_t place = 1u;
				for (auto i = digit_count; i --> 0_sz;)
				{
					value += fractional_digits[i] * place;
					place *= 10u;
				}
				for (auto i = digit_count; i < max_fractional_digits; i++) //implicit zeros
					value *= 10u;
				time.nanosecond = value;

				return time;
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			date_time parse_date_time() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_decimal_digit(*cp));

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing "sv, node_type::date_time);
				};

				// "YYYY-MM-DD"
				auto date = parse_date(true);
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// ' ' or 'T'
				if (!is_match(*cp, U' ', U'T'))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::date_time,
						"; expected space or 'T', saw '"sv, *cp, '\''
					);
				else
				{
					advance();
					eof_check();
				}
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// "HH:MM:SS.fractional"
					
				auto time = parse_time(true);
				TOML_ERROR_CHECK({});

				// offset
				time_offset offset;
				bool has_offset = false;
				if (cp)
				{
					// zero offset ("Z")
					if (*cp == U'Z')
					{
						has_offset = true;
						advance();
					}

					// explicit offset ("+/-HH:MM")
					else if (is_match(*cp, U'+', U'-'))
					{
						// sign
						int sign = *cp == U'-' ? -1 : 1;
						advance();
						eof_check();
						TOML_ERROR_CHECK({});
						TOML_NOT_EOF;

						// "HH"
						int digits[2];
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								" offset; expected 2-digit hour, saw '"sv, *cp, '\''
							);
						}
						const auto hour = digits[1] + digits[0] * 10;
						if (hour > 23)
							abort_with_error(
								"Error parsing "sv, node_type::date_time,
								" offset; expected hour between 0 and 23 (inclusive), saw "sv, hour 
							);
						else
							eof_check();
						TOML_ERROR_CHECK({});
						TOML_NOT_EOF;

						// ':'
						if (*cp != U':')
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								"offset; expected ':', saw '"sv, *cp, '\''
							);
						else
						{
							advance();
							eof_check();
						}
						TOML_ERROR_CHECK({});
						TOML_NOT_EOF;

						// "MM"
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							abort_with_error(
								"Encountered unexpected character while parsing "sv, node_type::date_time,
								" offset; expected 2-digit minute, saw '"sv, *cp, '\''
							);
						}
						const auto minute = digits[1] + digits[0] * 10;
						if (minute > 59)
							abort_with_error(
								"Error parsing "sv, node_type::date_time,
								" offset; expected minute between 0 and 59 (inclusive), saw "sv, hour
							);

						has_offset = true;
						offset.minutes = static_cast<int16_t>((hour * 60 + minute) * sign);
					}
				}
				TOML_ERROR_CHECK({});

				if (cp && !is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing "sv, node_type::date_time,
						"; expected value-terminator, saw '"sv, *cp, '\''
					);

				TOML_ERROR_CHECK({});
				if (has_offset)
					return { date, time, offset };
				else
					return { date, time };
			}

			// TOML_DISABLE_SWITCH_WARNINGS
			// TOML_DISABLE_INIT_WARNINGS
			TOML_POP_WARNINGS 

			[[nodiscard]]
			std::unique_ptr<toml::array> parse_array() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<toml::table> parse_inline_table() TOML_MAY_THROW;

			[[nodiscard]]
			TOML_NEVER_INLINE
			std::unique_ptr<node> parse_value() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(!is_value_terminator(*cp));

				const auto begin_pos = cp->position;
				std::unique_ptr<node> val;

				do
				{
					// detect the value type and parse accordingly,
					// starting with value types that can be detected
					// unambiguously from just one character.

					// strings
					if (is_string_delimiter(*cp))
						val = std::make_unique<value<string>>(parse_string());

					// bools
					else if (is_match(*cp, U't', U'f', U'T', U'F'))
						val = std::make_unique<value<bool>>(parse_bool());

					// arrays
					else if (*cp == U'[')
					{
						val = parse_array();
						if constexpr (!TOML_LANG_AT_LEAST(1, 0, 0)) // toml/issues/665 (heterogeneous arrays)
						{
							if (!val->ref_cast<array>().is_homogeneous())
								TOML_ERROR(
									"Arrays cannot contain values of different types before TOML 1.0.0.",
									begin_pos,
									reader.source_path()
								);
						}
					}

					// inline tables
					else if (*cp == U'{')
						val = parse_inline_table();

					// inf or nan
					else if (is_match(*cp, U'i', U'n', U'I', U'N'))
						val = std::make_unique<value<double>>(parse_inf_or_nan());

					// underscores at the beginning
					else if (*cp == U'_')
						abort_with_error("Values may not begin with underscores."sv);

					TOML_ERROR_CHECK({});
					if (val)
						break;

					// value types from here down require more than one character to unambiguously identify
					// so scan ahead and collect a set of value 'traits'.
					enum value_traits : int
					{
						has_nothing		= 0,
						has_digits		= 1,
						has_b			= 1 << 1, // as second char only (0b)
						has_e			= 1 << 2, // only float exponents
						has_o			= 1 << 3, // as second char only (0o)
						has_p			= 1 << 4, // only hexfloat exponents
						has_t			= 1 << 5,
						has_x			= 1 << 6, // as second or third char only (0x, -0x, +0x)
						has_z			= 1 << 7,
						has_colon		= 1 << 8,
						has_plus		= 1 << 9,
						has_minus		= 1 << 10,
						has_dot			= 1 << 11,
						begins_sign		= 1 << 12,
						begins_digit	= 1 << 13,
						begins_zero		= 1 << 14

						// Q: "why not make these real values in the enum??"
						// A: because the visual studio debugger stops treating them as a set of flags if you add
						// non-pow2 values, making them much harder to debug.
						#define toml_signs_msk (has_plus | has_minus)
						#define toml_bzero_msk (begins_zero | has_digits)
						#define toml_bdigit_msk (begins_digit | has_digits)
					};
					value_traits traits = has_nothing;
					const auto has_trait = [&](auto t) noexcept { return (traits & t) != has_nothing; };
					const auto add_trait = [&](auto t) noexcept { traits = static_cast<value_traits>(traits | t); };
					char32_t chars[utf8_buffered_reader::max_history_length];
					size_t char_count = {}, advance_count = {};
					bool eof_while_scanning = false;
					const auto scan = [&]() TOML_MAY_THROW
					{
						while (advance_count < utf8_buffered_reader::max_history_length)
						{
							if (!cp || is_value_terminator(*cp))
							{
								eof_while_scanning = !cp;
								break;
							}

							if (*cp != U'_')
							{
								chars[char_count++] = *cp;
								switch (*cp)
								{
									case U'B': [[fallthrough]];
									case U'b':
										if (chars[0] == U'0' && char_count == 2_sz)
											add_trait(has_b);
										break;

									case U'E': [[fallthrough]];
									case U'e':
										if (char_count > 1_sz && !has_trait(has_b | has_o | has_p | has_x))
											add_trait(has_e);
										break;

									case U'O': [[fallthrough]];
									case U'o':
										if (chars[0] == U'0' && char_count == 2_sz)
											add_trait(has_o);
										break;

									case U'P': [[fallthrough]];
									case U'p':
										if (has_trait(has_x))
											add_trait(has_p);
										break;

									case U'X': [[fallthrough]];
									case U'x':
										if ((chars[0] == U'0' && char_count == 2_sz)
											|| (is_match(chars[0], U'-', U'+') && chars[1] == U'0' && char_count == 3_sz))
											add_trait(has_x);
										break;

									case U'T': add_trait(has_t); break;
									case U'Z': add_trait(has_z); break;
									case U'+': add_trait(has_plus); break;
									case U'-': add_trait(has_minus); break;
									case U'.': add_trait(has_dot); break;
									case U':': add_trait(has_colon); break;

									default:
										if (is_decimal_digit(*cp))
											add_trait(has_digits);
								}
							}
							advance();
							advance_count++;
							TOML_ERROR_CHECK();
						}
					};
					scan();
					TOML_ERROR_CHECK({});

					// force further scanning if this could have been a date-time with a space instead of a T
					if (char_count == 10_sz
						&& traits == (has_digits | has_minus)
						&& chars[4] == U'-'
						&& chars[7] == U'-'
						&& cp
						&& *cp == U' ')
					{
						const auto pre_advance_count = advance_count;
						const auto pre_scan_traits = traits;
						chars[char_count++] = *cp;
						add_trait(has_t);

						const auto backpedal = [&]() noexcept
						{
							go_back(advance_count - pre_advance_count);
							advance_count = pre_advance_count;
							traits = pre_scan_traits;
							char_count = 10_sz;
						};

						advance();
						advance_count++;
						TOML_ERROR_CHECK({});

						if (!cp || !is_decimal_digit(*cp))
							backpedal();
						else
						{
							chars[char_count++] = *cp;

							advance();
							advance_count++;
							TOML_ERROR_CHECK({});

							scan();
							TOML_ERROR_CHECK({});

							if (char_count == 12_sz)
								backpedal();
						}
					}

					// set the reader back to where we started
					go_back(advance_count);
					if (char_count < utf8_buffered_reader::max_history_length - 1_sz)
						chars[char_count] = U'\0';

					// if after scanning ahead we still only have one value character,
					// the only valid value type is an integer.
					if (char_count == 1_sz)
					{
						if (traits == has_digits)
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[0] - U'0'));
							advance(); //skip the digit
							break;
						}

						//anything else would be ambiguous.
						abort_with_error(
							eof_while_scanning
								? "Encountered EOF while parsing value"sv
								: "Could not determine value type"sv
						);
					}

					// now things that can be identified from two or more characters
					TOML_ERROR_CHECK({});
					TOML_ASSERT(char_count >= 2_sz);

					// slightly deeper trait invariant resolution.
					// some 'fuzzy matching' is done here where there's no ambiguity, since that allows the specific
					// typed parse functions to take over and show better diagnostics if there's an issue
					// (as opposed to the fallback "could not determine type" message)
					if (is_decimal_digit(chars[0]))
					{
						if (chars[0] == U'0')
						{
							// hex integers and floats
							if (has_trait(has_x))
							{
								if (has_trait(has_p))
									val = std::make_unique<value<double>>(parse_hex_float());
								else
									val = std::make_unique<value<int64_t>>(parse_integer<16>());
							}

							// octal integers
							else if (has_trait(has_o))
								val = std::make_unique<value<int64_t>>(parse_integer<8>());

							// binary integers
							else if (has_trait(has_b))
								val = std::make_unique<value<int64_t>>(parse_integer<2>());

							else
								add_trait(begins_zero);
						}
						else
							add_trait(begins_digit);

						if (!val)
						{
							// simple floats (e.g. 1.0, 1e1)
							if (is_match(chars[1], U'.', U'e', U'E'))
								val = std::make_unique<value<double>>(parse_float());
						}
					}
					else if (is_match(chars[0], U'+', U'-'))
					{
						// single-digit signed integers
						if (char_count == 2_sz && has_trait(has_digits))
						{
							val = std::make_unique<value<int64_t>>(
								static_cast<int64_t>(chars[1] - U'0')
								* (chars[1] == U'-' ? -1LL : 1LL)
							);
							advance(); //skip the sign
							advance(); //skip the digit
							break;
						}

						// simple signed floats (e.g. +1.0, -1e1)
						else if (is_decimal_digit(chars[1]) && is_match(chars[2], U'.', U'e', U'E'))
							val = std::make_unique<value<double>>(parse_float());

						// signed hexfloats (e.g. +1.0, -1e1)
						// (hex integers cannot be signed in TOML)
						else if (has_trait(has_x))
							val = std::make_unique<value<double>>(parse_hex_float());

						// signed infinity or nan
						else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
						{
							val = std::make_unique<value<double>>(parse_inf_or_nan());
							break;
						}

						else
							add_trait(begins_sign);
					}

					TOML_ERROR_CHECK({});
					if (val)
						break;

					// match trait masks against what they can match exclusively.
					// all correct value parses will come out of this list, so doing this as a switch is likely to
					// be a better friend to the optimizer on the success path (failure path can be slow but that
					// doesn't matter much).
					switch (unbox_enum(traits))
					{
						//=================== binary integers
						// 0b10
						case toml_bzero_msk | has_b:
							val = std::make_unique<value<int64_t>>(parse_integer<2>());
							break;

						//=================== octal integers
						// 0o10
						case toml_bzero_msk | has_o:
							val = std::make_unique<value<int64_t>>(parse_integer<8>());
							break;

						//=================== decimal integers
						// 00
						// 10
						// +10
						// -10
						case toml_bzero_msk:													[[fallthrough]];
						case toml_bdigit_msk:													[[fallthrough]];
						case begins_sign | has_digits | has_minus:								[[fallthrough]];
						case begins_sign | has_digits | has_plus:
							val = std::make_unique<value<int64_t>>(parse_integer<10>());
							break;

						//=================== hexadecimal integers
						// 0x10
						case toml_bzero_msk | has_x:
							val = std::make_unique<value<int64_t>>(parse_integer<16>());
							break;

						//=================== decimal floats
						// 0e1
						// 0e-1
						// 0e+1
						// 0.0
						// 0.0e1
						// 0.0e-1
						// 0.0e+1
						case toml_bzero_msk | has_e:											[[fallthrough]];
						case toml_bzero_msk | has_e | has_minus:								[[fallthrough]];
						case toml_bzero_msk | has_e | has_plus:									[[fallthrough]];
						case toml_bzero_msk | has_dot:											[[fallthrough]];
						case toml_bzero_msk | has_dot | has_e:									[[fallthrough]];
						case toml_bzero_msk | has_dot | has_e | has_minus:						[[fallthrough]];
						case toml_bzero_msk | has_dot | has_e | has_plus:						[[fallthrough]];
						// 1e1
						// 1e-1
						// 1e+1
						// 1.0
						// 1.0e1
						// 1.0e-1
						// 1.0e+1
						case toml_bdigit_msk | has_e:											[[fallthrough]];
						case toml_bdigit_msk | has_e | has_minus:								[[fallthrough]];
						case toml_bdigit_msk | has_e | has_plus:								[[fallthrough]];
						case toml_bdigit_msk | has_dot:											[[fallthrough]];
						case toml_bdigit_msk | has_dot | has_e:									[[fallthrough]];
						case toml_bdigit_msk | has_dot | has_e | has_minus:						[[fallthrough]];
						case toml_bdigit_msk | has_dot | has_e | has_plus:						[[fallthrough]];
						// +1e1
						// +1.0
						// +1.0e1
						// +1.0e+1
						// +1.0e-1
						// -1.0e+1
						case begins_sign | has_digits | has_e | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | toml_signs_msk:		[[fallthrough]];
						// -1e1
						// -1e+1
						// +1e-1
						// -1.0
						// -1.0e1
						// -1.0e-1
						case begins_sign | has_digits | has_e | has_minus:						[[fallthrough]];
						case begins_sign | has_digits | has_e | toml_signs_msk:					[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_minus:					[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_minus:
							val = std::make_unique<value<double>>(parse_float());
							break;

						//=================== hexadecimal floats
						// 0x10p0
						// 0x10p-0
						// 0x10p+0
						case toml_bzero_msk | has_x | has_p:									[[fallthrough]];
						case toml_bzero_msk | has_x | has_p | has_minus:						[[fallthrough]];
						case toml_bzero_msk | has_x | has_p | has_plus:							[[fallthrough]];
						// -0x10p0
						// -0x10p-0
						// +0x10p0
						// +0x10p+0
						// -0x10p+0
						// +0x10p-0
						case begins_sign | has_digits | has_x | has_p | has_minus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | toml_signs_msk:			[[fallthrough]];
						// 0x10.1p0
						// 0x10.1p-0
						// 0x10.1p+0
						case toml_bzero_msk | has_x | has_dot | has_p:							[[fallthrough]];
						case toml_bzero_msk | has_x | has_dot | has_p | has_minus:				[[fallthrough]];
						case toml_bzero_msk | has_x | has_dot | has_p | has_plus:				[[fallthrough]];
						// -0x10.1p0
						// -0x10.1p-0
						// +0x10.1p0
						// +0x10.1p+0
						// -0x10.1p+0
						// +0x10.1p-0
						case begins_sign | has_digits | has_x | has_dot | has_p | has_minus:	[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | has_plus:		[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | toml_signs_msk:
							val = std::make_unique<value<double>>(parse_hex_float());
							break;

						//=================== times
						// HH:MM
						// HH:MM:SS
						// HH:MM:SS.FFFFFF
						case toml_bzero_msk | has_colon:										[[fallthrough]];
						case toml_bzero_msk | has_colon | has_dot:								[[fallthrough]];
						case toml_bdigit_msk | has_colon:										[[fallthrough]];
						case toml_bdigit_msk | has_colon | has_dot:
							val = std::make_unique<value<time>>(parse_time());
							break;

						//=================== local dates
						// YYYY-MM-DD
						case toml_bzero_msk | has_minus:										[[fallthrough]];
						case toml_bdigit_msk | has_minus:
							val = std::make_unique<value<date>>(parse_date());
							break;

						//=================== date-times
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
						case toml_bzero_msk | has_minus | has_colon | has_t:					[[fallthrough]];
						case toml_bzero_msk | toml_signs_msk | has_colon | has_t:				[[fallthrough]];
						case toml_bdigit_msk | has_minus | has_colon | has_t:					[[fallthrough]];
						case toml_bdigit_msk | toml_signs_msk | has_colon | has_t:				[[fallthrough]];
						// YYYY-MM-DDTHH:MM:SS.FFFFFF
						// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF
						// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
						case toml_bzero_msk | has_minus | has_colon | has_dot | has_t:			[[fallthrough]];
						case toml_bzero_msk | toml_signs_msk | has_colon | has_dot | has_t:		[[fallthrough]];
						case toml_bdigit_msk | has_minus | has_colon | has_dot | has_t:			[[fallthrough]];
						case toml_bdigit_msk | toml_signs_msk | has_colon | has_dot | has_t:	[[fallthrough]];
						// YYYY-MM-DDTHH:MMZ
						// YYYY-MM-DD HH:MMZ
						// YYYY-MM-DDTHH:MM:SSZ
						// YYYY-MM-DD HH:MM:SSZ
						// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
						// YYYY-MM-DD HH:MM:SS.FFFFFFZ
						case toml_bzero_msk | has_minus | has_colon | has_z | has_t:			[[fallthrough]];
						case toml_bzero_msk | has_minus | has_colon | has_dot | has_z | has_t:	[[fallthrough]];
						case toml_bdigit_msk | has_minus | has_colon | has_z | has_t:			[[fallthrough]];
						case toml_bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
							val = std::make_unique<value<date_time>>(parse_date_time());
							break;
					}

					#undef toml_signs_msk
					#undef toml_bzero_msk
					#undef toml_bdigit_msk
				}
				while (false);

				if (!val)
				{
					abort_with_error("Could not determine value type"sv);
					TOML_ERROR_CHECK({});
					TOML_UNREACHABLE;
				}

				val->source_ = { begin_pos, current_position(1), reader.source_path() };
				return val;
			}

			struct key final
			{
				std::vector<string> segments;
			};

			[[nodiscard]]
			TOML_NEVER_INLINE
			key parse_key() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(is_bare_key_start_character(*cp) || is_string_delimiter(*cp));

				key key;

				while (true)
				{
					if (!cp)
						abort_with_error("Encountered EOF while parsing key"sv);

					// bare_key_segment
					#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
					else if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing key; expected bare key starting character "
							"or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					#endif

					else if (is_bare_key_start_character(*cp))
						key.segments.push_back(parse_bare_key_segment());

					// "quoted key segment"
					else if (is_string_delimiter(*cp))
						key.segments.push_back(parse_string());

					// ???
					else
						abort_with_error(
							"Encountered unexpected character while parsing key; expected bare key "
							"starting character or string delimiter, saw '"sv, *cp, '\''
						);
						
					consume_leading_whitespace();
					TOML_ERROR_CHECK({});

					// eof or no more key to come
					if (!cp)
						break;
					if (*cp != U'.')
					{
						if (recording)
							stop_recording(1_sz);
						break;
					}

					// was a dotted key, so go around again to consume the next segment
					TOML_ASSERT(*cp == U'.');
					advance();
					consume_leading_whitespace();
				}
				TOML_ERROR_CHECK({});
				return key;
			}

			struct key_value_pair final
			{
				parser::key key;
				std::unique_ptr<node> value;
			};

			[[nodiscard]]
			TOML_NEVER_INLINE
			key_value_pair parse_key_value_pair() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});

				const auto eof_check = [this]() TOML_MAY_THROW
				{
					TOML_ERROR_CHECK();
					if (!cp)
						abort_with_error("Encountered EOF while parsing key-value pair"sv);
				};

				// get the key
				TOML_NOT_EOF;
				TOML_ASSERT(is_string_delimiter(cp->value) || is_bare_key_start_character(cp->value));
				start_recording();
				auto key = parse_key(); //parse_key() calls stop_recording()

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// consume the '='
				if (*cp != U'=')
					abort_with_error(
						"Encountered unexpected character while parsing key-value pair; "
						"expected '=', saw '"sv, *cp, '\''
					);
				advance();
				TOML_ERROR_CHECK({});

				// skip past any whitespace that followed the '='
				consume_leading_whitespace();
				eof_check();
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;

				// get the value
				if (is_value_terminator(*cp))
					abort_with_error(
						"Encountered unexpected character while parsing key-value pair; "
						"expected value, saw '"sv, *cp, '\''
					);
				TOML_ERROR_CHECK({});
				return { std::move(key), parse_value() };
			}

			[[nodiscard]]
			TOML_NEVER_INLINE
			table* parse_table_header() TOML_MAY_THROW
			{
				TOML_ERROR_CHECK({});
				TOML_NOT_EOF;
				TOML_ASSERT(*cp == U'[');

				const auto header_begin_pos = cp->position;
				source_position header_end_pos;
				key key;
				bool is_arr = false;

				//parse header
				{
					const auto eof_check = [this]() TOML_MAY_THROW
					{
						TOML_ERROR_CHECK();
						if (!cp)
							abort_with_error("Encountered EOF while parsing table header"sv);
					};

					// skip first '['
					advance();
					eof_check();
					TOML_ERROR_CHECK({});
					TOML_NOT_EOF;

					// skip second '[' (if present)
					if (*cp == U'[')
					{
						advance();
						eof_check();
						is_arr = true;
					}

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					eof_check();

					// sanity-check the key start
					#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
					if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing table key; "
							"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					else
					#endif
					if (!is_bare_key_start_character(cp->value) && !is_string_delimiter(cp->value))
					{
						abort_with_error(
							"Encountered unexpected character while parsing table key; "
							"expected bare key starting character or string delimiter, saw '"sv, *cp, '\''
						);
					}
					TOML_ERROR_CHECK({});

					// get the actual key
					start_recording();
					key = parse_key(); //parse_key() calls stop_recording()
					TOML_ERROR_CHECK({});

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					eof_check();
					TOML_ERROR_CHECK({});
					TOML_NOT_EOF;

					// consume the first closing ']'
					if (*cp != U']')
						abort_with_error(
							"Encountered unexpected character while parsing table"sv,
							(is_arr ? " array"sv : ""sv), " header; expected ']', saw '"sv, *cp, '\''
						);
					advance();

					// consume the second closing ']'
					if (is_arr)
					{
						eof_check();
						TOML_ERROR_CHECK({});
						TOML_NOT_EOF;

						if (*cp != U']')
							abort_with_error(
								"Encountered unexpected character while parsing table array header; "
								"expected ']', saw '"sv, *cp, '\''
							);
						advance();
					}
					header_end_pos = current_position(1);

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (cp)
					{
						if (!consume_comment() && !consume_line_break())
							abort_with_error(
								"Encountered unexpected character after table"sv, (is_arr ? " array"sv : ""sv),
								" header; expected a comment or whitespace, saw '"sv, *cp, '\''
							);
					}
				}
				TOML_ERROR_CHECK({});
				TOML_ASSERT(!key.segments.empty());

				// check if each parent is a table/table array, or can be created implicitly as a table.
				auto parent = &root;
				for (size_t i = 0; i < key.segments.size() - 1_sz; i++)
				{
					auto child = parent->get(key.segments[i]);
					if (!child)
					{
						child = parent->values.emplace(
							key.segments[i],
							std::make_unique<table>()
						).first->second.get();
						implicit_tables.push_back(&child->ref_cast<table>());
						child->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						parent = &child->ref_cast<table>();
					}
					else if (child->is_table())
					{
						parent = &child->ref_cast<table>();
					}
					else if (child->is_array() && find(table_arrays, &child->ref_cast<array>()))
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!child->ref_cast<array>().values.empty());
						TOML_ASSERT(child->ref_cast<array>().values.back()->is_table());
						parent = &child->ref_cast<array>().values.back()->ref_cast<table>();
					}
					else
					{
						if (!is_arr && child->type() == node_type::table)
							abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
						else
							abort_with_error(
								"Attempt to redefine existing "sv, child->type(),
								" '"sv, recording_buffer,
								"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
							);
					}
				}
				TOML_ERROR_CHECK({});

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
						auto tab_arr = &parent->values.emplace(key.segments.back(),std::make_unique<array>())
							.first->second->ref_cast<array>();
						table_arrays.push_back(tab_arr);
						tab_arr->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						
						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return &tab_arr->values.back()->ref_cast<table>();
					}

					//otherwise we're just making a table
					else
					{
						auto tab = &parent->values.emplace(key.segments.back(),std::make_unique<table>())
							.first->second->ref_cast<table>();
						tab->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab;
					}
				}

				// if there was already a matching node some sanity checking is necessary;
				// this is ok if we're making an array and the existing element is already an array (new element)
				// or if we're making a table and the existing element is an implicitly-created table (promote it),
				// otherwise this is a redefinition error.
				else
				{
					if (is_arr && matching_node->is_array() && find(table_arrays, &matching_node->ref_cast<array>()))
					{
						auto tab_arr = &matching_node->ref_cast<array>();
						tab_arr->values.push_back(std::make_unique<table>());
						tab_arr->values.back()->source_ = { header_begin_pos, header_end_pos, reader.source_path() };
						return &tab_arr->values.back()->ref_cast<table>();
					}

					else if (!is_arr
						&& matching_node->is_table()
						&& !implicit_tables.empty())
					{
						auto tbl = &matching_node->ref_cast<table>();
						if (auto found = find(implicit_tables, tbl))
						{
							implicit_tables.erase(implicit_tables.cbegin() + (found - implicit_tables.data()));
							tbl->source_.begin = header_begin_pos;
							tbl->source_.end = header_end_pos;
							return tbl;
						}
					}

					//if we get here it's a redefinition error.
					if (!is_arr && matching_node->type() == node_type::table)
						abort_with_error("Attempt to redefine existing table '"sv, recording_buffer, '\'');
					else
						abort_with_error(
							"Attempt to redefine existing "sv, matching_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
						);
				}
				TOML_ERROR_CHECK({});
				TOML_UNREACHABLE;
			}

			TOML_NEVER_INLINE
			void parse_key_value_pair_and_insert(table* tab) TOML_MAY_THROW
			{
				TOML_ERROR_CHECK();

				auto kvp = parse_key_value_pair();
				TOML_ERROR_CHECK();
				TOML_ASSERT(kvp.key.segments.size() >= 1_sz);

				if (kvp.key.segments.size() > 1_sz)
				{
					for (size_t i = 0; i < kvp.key.segments.size() - 1_sz; i++)
					{
						auto child = tab->get(kvp.key.segments[i]);
						if (!child)
						{
							child = tab->values.emplace(
								std::move(kvp.key.segments[i]),
								std::make_unique<table>()
							).first->second.get();
							dotted_key_tables.push_back(&child->ref_cast<table>());
							dotted_key_tables.back()->inline_ = true;
							child->source_ = kvp.value->source_;
						}
						else if (!child->is_table() || !find(dotted_key_tables, &child->ref_cast<table>()))
						{
							abort_with_error("Attempt to redefine "sv, child->type(), " as dotted key-value pair"sv);
						}
						else
							child->source_.end = kvp.value->source_.end;
						TOML_ERROR_CHECK();
						tab = &child->ref_cast<table>();
					}
				}

				if (auto conflicting_node = tab->get(kvp.key.segments.back()))
				{
					if (conflicting_node->type() == kvp.value->type())
						abort_with_error("Attempt to redefine "sv, conflicting_node->type(), " '"sv, recording_buffer, '\'');
					else
						abort_with_error(
							"Attempt to redefine "sv, conflicting_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, kvp.value->type()
						);
				}

				TOML_ERROR_CHECK();
				tab->values.emplace(
					std::move(kvp.key.segments.back()),
					std::move(kvp.value)
				);
			}

			TOML_NEVER_INLINE
			void parse_document() TOML_MAY_THROW
			{
				TOML_NOT_EOF;
					
				table* current_table = &root;

				do
				{
					TOML_ERROR_CHECK();

					// leading whitespace, line endings, comments
					if (consume_leading_whitespace()
						|| consume_line_break()
						|| consume_comment())
						continue;

					// [tables]
					// [[table array]]
					else if (*cp == U'[')
					{
						current_table = parse_table_header();
					}

					// bare_keys
					// dotted.keys
					// "quoted keys"
					#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
					else if (is_unicode_combining_mark(*cp))
						abort_with_error(
							"Encountered unexpected character while parsing key; "
							"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
						);
					#endif
					else if (is_bare_key_start_character(cp->value) || is_string_delimiter(cp->value))
					{
						parse_key_value_pair_and_insert(current_table);

						// handle the rest of the line after the kvp
						// (this is not done in parse_key_value_pair() because that function is also used for inline tables)
						consume_leading_whitespace();
						if (cp)
						{
							if (!consume_comment() && !consume_line_break())
								abort_with_error(
									"Encountered unexpected character after key-value pair; "
									"expected a comment or whitespace, saw '"sv, *cp, '\''
								);
						}
					}

					else // ??
						abort_with_error(
							"Encountered unexpected character while parsing top level of document; "
							"expected keys, tables, whitespace or comments, saw '"sv, *cp, '\''
						);

				}
				while (cp);

				auto eof_pos = current_position(1);
				root.source_.end = eof_pos;
				if (current_table
					&& current_table != &root
					&& current_table->source_.end <= current_table->source_.begin)
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
					if (tbl.inline_) //inline tables (and all their inline descendants) are already correctly terminated
						return;

					auto end = nde.source_.end;
					for (auto& [k, v] : tbl.values)
					{
						(void)k;
						update_region_ends(*v);
						if (end < v->source_.end)
							end = v->source_.end;
					}
				}
				else //arrays
				{
					auto& arr = nde.ref_cast<array>();
					auto end = nde.source_.end;
					for (auto& v : arr.values)
					{
						update_region_ends(*v);
						if (end < v->source_.end)
							end = v->source_.end;
					}
					nde.source_.end = end;
				}
			}

		public:

			parser(utf8_reader_interface&& reader_) TOML_MAY_THROW
				: reader{ reader_ }
			{
				root.source_ = { prev_pos, prev_pos, reader.source_path() };

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

				update_region_ends(root);
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
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

			TOML_POP_WARNINGS
	};

	TOML_FUNC_EXTERNAL_LINKAGE
	std::unique_ptr<toml::array> parser::parse_array() TOML_MAY_THROW
	{
		TOML_ERROR_CHECK({});
		TOML_NOT_EOF;
		TOML_ASSERT(*cp == U'[');

		const auto eof_check = [this]() TOML_MAY_THROW
		{
			TOML_ERROR_CHECK();
			if (!cp)
				abort_with_error("Encountered EOF while parsing array"sv);
		};

		// skip opening '['
		advance();
		eof_check();
		TOML_ERROR_CHECK({});

		auto arr = std::make_unique<array>();
		auto& vals = arr->values;

		enum parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (true)
		{
			TOML_ERROR_CHECK({});

			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
				continue;
			eof_check();

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (prev == val)
				{
					prev = comma;
					advance();
					continue;
				}
				abort_with_error(
					"Encountered unexpected character while parsing array; "
					"expected value or closing ']', saw comma"sv
				);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance();
				break;
			}

			// must be a value
			else
			{
				if (prev == val)
				{
					abort_with_error(
						"Encountered unexpected character while parsing array; "
						"expected comma or closing ']', saw '"sv, *cp, '\''
					);
					continue;
				}
				prev = val;

				vals.push_back(parse_value());
			}
		}

		TOML_ERROR_CHECK({});
		return arr;
	}

	TOML_FUNC_EXTERNAL_LINKAGE
	std::unique_ptr<toml::table> parser::parse_inline_table() TOML_MAY_THROW
	{
		TOML_ERROR_CHECK({});
		TOML_NOT_EOF;
		TOML_ASSERT(*cp == U'{');

		const auto eof_check = [this]() TOML_MAY_THROW
		{
			TOML_ERROR_CHECK();
			if (!cp)
				abort_with_error("Encountered EOF while parsing inline table"sv);
		};

		// skip opening '{'
		advance();
		eof_check();
		TOML_ERROR_CHECK({});

		auto tab = std::make_unique<table>();
		tab->inline_ = true;

		enum parse_elem : int
		{
			none,
			comma,
			kvp
		};
		parse_elem prev = none;

		while (true)
		{
			TOML_ERROR_CHECK({});

			if constexpr (TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
			{
				while (consume_leading_whitespace()
					|| consume_line_break()
					|| consume_comment())
					continue;
			}
			else
			{
				while (consume_leading_whitespace())
					continue;
			}

			eof_check();

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (prev == kvp)
				{
					prev = comma;
					advance();
				}
				else
					abort_with_error(
						"Encountered unexpected character while parsing inline table; "
						"expected key-value pair or closing '}', saw comma"sv
					);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				if constexpr (!TOML_LANG_UNRELEASED) // toml/issues/516 (newlines/trailing commas in inline tables)
				{
					if (prev == comma)
					{
						abort_with_error(
							"Encountered unexpected character while parsing inline table; "
							"expected key-value pair, saw closing '}' (dangling comma)"sv
						);
						continue;
					}
				}

				advance();
				break;
			}

			// key-value pair
			#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
			else if (is_unicode_combining_mark(*cp))
			{
				abort_with_error(
					"Encountered unexpected character while parsing inline table; "
					"expected bare key starting character or string delimiter, saw combining mark '"sv, *cp, '\''
				);
			}
			#endif
			else if (is_string_delimiter(*cp) || is_bare_key_start_character(*cp))
			{
				if (prev == kvp)
					abort_with_error(
						"Encountered unexpected character while parsing inline table; "
						"expected comma or closing '}', saw '"sv, *cp, '\''
					);
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(tab.get());
				}
			}

			/// ???
			else
				abort_with_error(
					"Encountered unexpected character while parsing inline table; "
					"expected key or closing '}', saw '"sv, *cp, '\''
				);
		}

		TOML_ERROR_CHECK({});
		return tab;
	}

	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
	#undef TOML_NORETURN
	#undef TOML_NOT_EOF

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	parse_result do_parse(utf8_reader_interface&& reader) TOML_MAY_THROW
	{
		return impl::parser{ std::move(reader) };
	}

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif
}

namespace toml
{
	#if TOML_ABI_NAMESPACES
		#if TOML_EXCEPTIONS
			inline namespace abi_parse_ex {
		#else
			inline namespace abi_parse_noex {
		#endif
	#endif

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#ifdef __cpp_lib_char8_t

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_FUNC_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#endif // __cpp_lib_char8_t

	#if TOML_ABI_NAMESPACES
		} //end abi namespace for TOML_EXCEPTIONS
	#endif

	inline namespace literals
	{
		#if TOML_ABI_NAMESPACES
			#if TOML_EXCEPTIONS
				inline namespace abi_lit_ex {
			#else
				inline namespace abi_lit_noex {
			#endif
		#endif

		TOML_API
		TOML_FUNC_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::string_view{ str, len });
		}

		#ifdef __cpp_lib_char8_t

		TOML_API
		TOML_FUNC_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::u8string_view{ str, len });
		}

		#endif // __cpp_lib_char8_t

		#if TOML_ABI_NAMESPACES
			} //end abi namespace for TOML_EXCEPTIONS
		#endif
	}
}

