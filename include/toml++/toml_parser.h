#pragma once
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table_array.h"

namespace TOML_NAMESPACE::impl
{
	class parser final
	{
		private:
			utf8_buffered_reader reader;
			std::unique_ptr<table> root;
			document_position prev_pos = { 1_sz, 1_sz };
			const utf8_codepoint* cp = {};
			std::vector<table*> implicit_tables;

			template <typename... T>
			[[noreturn]]
			void throw_parse_error(T &&... args) const
			{
				const auto& pos = cp ? cp->position : prev_pos;
				if constexpr (sizeof...(T) == 0_sz)
					throw parse_error{ "An unspecified error occurred"s, pos, reader.source_path() };
				else
				{
					std::string str;
					static constexpr auto concatenator = [&](std::string& s, auto&& arg) noexcept //a.k.a. "no stringstreams, thanks"
					{
						using arg_t = remove_cvref_t<decltype(arg)>;
						if constexpr (std::is_same_v<arg_t, std::string_view> || std::is_same_v<arg_t, std::string>)
						{
							s.append(std::forward<decltype(arg)>(arg));
						}
						else if constexpr (std::is_same_v<arg_t, char>)
						{
							s += arg;
						}
						else if constexpr (std::is_same_v<arg_t, bool>)
						{
							s.append(arg ? "true"sv : "false"sv);
						}
						else if constexpr (std::is_arithmetic_v<arg_t>)
						{
							char buf[64];
							std::to_chars_result result;
							if constexpr (std::is_floating_point_v<arg_t>)
								result = std::to_chars(buf, buf + sizeof(buf), arg, std::chars_format::general);
							else if constexpr (std::is_integral_v<arg_t>)
								result = std::to_chars(buf, buf + sizeof(buf), arg); {
							}
							s.append(std::string_view{ buf, static_cast<size_t>(result.ptr - buf) });
						}
					};
					(concatenator(str, std::forward<T>(args)), ...);
					throw parse_error{ str, pos, reader.source_path() };
				}
			}

			void go_back(size_t count = 1_sz)
			{
				TOML_ASSERT(count);

				cp = reader.step_back(count);
				prev_pos = cp->position;
			}

			void advance()
			{
				TOML_ASSERT(cp);

				prev_pos = cp->position;
				cp = reader.read_next();
			}

			bool consume_leading_whitespace()
			{
				bool consumed = false;
				while (cp && is_whitespace(*cp))
				{
					consumed = true;
					advance();
				}
				return consumed;
			}

			bool consume_line_break()
			{
				if (!cp || !is_line_break(*cp))
					return false;

				if (*cp == U'\r')
				{
					advance();  // skip \r
					if (!cp)
						throw_parse_error("Encountered EOF while consuming CRLF"sv);
					if (*cp != U'\n')
						throw_parse_error("Encountered unexpected character while consuming CRLF"sv);
				}
				advance(); // skip \n (or other single-character line ending)
				return true;
			}

			bool consume_rest_of_line()
			{
				if (!cp)
					return false;

				do
				{
					if (is_line_break(*cp))
						return consume_line_break();
					else
						advance();
				}
				while (cp);
				return true;
			}

			bool consume_comment()
			{
				if (!cp || *cp != U'#')
					return false;
				return consume_rest_of_line();
			}

			[[nodiscard]]
			bool consume_expected_sequence(std::u32string_view seq)
			{
				for (auto c : seq)
				{
					if (!cp || *cp != c)
						return false;
					advance();
				}
				return true;
			}

			template <typename T, size_t N>
			[[nodiscard]]
			bool consume_digit_sequence(T(&digits)[N])
			{
				for (size_t i = 0; i < N; i++)
				{
					if (!cp || !is_decimal_digit(*cp))
						return false;
					digits[i] = static_cast<T>(*cp - U'0');
					advance();
				}
				return true;
			}

			template <typename T, size_t N>
			[[nodiscard]]
			size_t consume_variable_length_digit_sequence(T(&digits)[N])
			{
				size_t i = {};
				for (; i < N; i++)
				{
					if (!cp || !is_decimal_digit(*cp))
						break;
					digits[i] = static_cast<T>(*cp - U'0');
					advance();
				}
				return i;
			}

			template <bool MULTI_LINE>
			[[nodiscard]]
			string parse_basic_string()
			{
				TOML_ASSERT(cp && *cp == U'"');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string"sv);
				};


				// skip the '"'
				advance();
				eof_check();

				string str;
				bool escaped = false, skipping_whitespace = false;
				while (cp)
				{
					if (escaped)
					{
						escaped = false;

						// handle 'line ending slashes' in multi-line mode
						if constexpr (MULTI_LINE)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								skipping_whitespace = true;
								continue;
							}
						}

						// skip the escaped character
						const auto escaped_codepoint = cp->value;
						advance();

						switch (escaped_codepoint)
						{
							// 'regular' escape codes
							case U'b': str += TOML_STRING_PREFIX('\b'); break;
							case U'f': str += TOML_STRING_PREFIX('\f'); break;
							case U'n': str += TOML_STRING_PREFIX('\n'); break;
							case U'r': str += TOML_STRING_PREFIX('\r'); break;
							#if !TOML_STRICT
							case U's': str += TOML_STRING_PREFIX(' '); break;
							#endif
							case U't': str += TOML_STRING_PREFIX('\t'); break;
							case U'"': str += TOML_STRING_PREFIX('"'); break;
							case U'\\': str += TOML_STRING_PREFIX('\\'); break;

							// unicode scalar sequences
							case U'u': [[fallthrough]];
							case U'U':
							{
								uint32_t place_value = escaped_codepoint == U'U' ? 0x10000000u : 0x1000u;
								uint32_t sequence_value{};
								while (place_value)
								{
									eof_check();
									if (!is_hex_digit(*cp))
										throw_parse_error("Encountered unexpected character while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string; expected hex digit, saw '\\"sv, cp->as_view<char>(), "'"sv);
									sequence_value += place_value * hex_digit_to_int(*cp);
									place_value /= 16u;
									advance();
								}

								if ((sequence_value > 0xD7FFu && sequence_value < 0xE000u) || sequence_value > 0x10FFFFu)
									throw_parse_error("Unknown Unicode scalar sequence"sv);

								if (sequence_value <= 0x7Fu) //ascii
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
								throw_parse_error("Encountered unexpected character while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string; unknown escape sequence '\\"sv, cp->as_view<char>(), "'"sv);
						}

							
					}
					else TOML_LIKELY
					{
						// handle closing delimiters
						if (*cp == U'"')
						{
							if constexpr (MULTI_LINE)
							{
								advance();
								eof_check();
								const auto second = cp->value;

								advance();
								eof_check();
								const auto third = cp->value;

								if (second == U'"' && third == U'"')
								{
									advance(); // skip the third closing delimiter
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
								return str;
							}
						}

						// handle escapes
						if (*cp == U'\\')
						{
							advance(); // skip the '\'
							skipping_whitespace = false;
							escaped = true;
							continue;
						}

						// handle line endings in multi-line mode
						if constexpr (MULTI_LINE)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								if (!str.empty() && !skipping_whitespace)
									str += TOML_STRING_PREFIX('\n');
								continue;
							}
						}

						// handle illegal characters
						if ((*cp >= U'\u0000' && *cp <= U'\u0008')
							|| (*cp >= U'\u000A' && *cp <= U'\u001F')
							|| *cp == U'\u007F')
							throw_parse_error("Encountered unexpected character while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string; control characters must be escaped with back-slashes."sv);

						if constexpr (MULTI_LINE)
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
					}
				}

				throw_parse_error("Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " string"sv);
			}

			template <bool MULTI_LINE>
			[[nodiscard]]
			string parse_literal_string()
			{
				TOML_ASSERT(cp && *cp == U'\'');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " literal string"sv);
				};

				// skip the delimiter
				advance();
				eof_check();

				string str;
				while (cp)
				{
					// handle closing delimiters
					if (*cp == U'\'')
					{
						if constexpr (MULTI_LINE)
						{
							advance();
							eof_check();
							const auto second = cp->value;

							advance();
							eof_check();
							const auto third = cp->value;

							if (second == U'\'' && third == U'\'')
							{
								advance(); // skip the third closing delimiter
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
							return str;
						}
					}

					// handle line endings in multi-line mode
					if constexpr (MULTI_LINE)
					{
						if (is_line_break(*cp))
						{
							consume_line_break();
							if (!str.empty())
								str += TOML_STRING_PREFIX('\n');
							continue;
						}
					}

					// handle illegal characters
					if ((*cp >= U'\u0000' && *cp <= U'\u0008')
						|| (*cp >= U'\u000A' && *cp <= U'\u001F')
						|| *cp == U'\u007F')
						throw_parse_error("Encountered unexpected character while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " literal string; control characters may not appear in literal strings"sv);

					str.append(cp->as_view());
					advance();
				}

				throw_parse_error("Encountered EOF while parsing"sv, (MULTI_LINE ? " multi-line"sv : ""sv), " literal string"sv);
			}

			[[nodiscard]]
			string parse_string()
			{
				TOML_ASSERT(cp && is_string_delimiter(*cp));

				// get the first three characters to determine the string type
				const auto first = cp->value;
				advance();
				if (!cp)
					throw_parse_error("Encountered EOF while parsing string"sv);
				const auto second = cp->value;
				advance();
				const auto third = cp ? cp->value : U'\0';

				// if we were eof at the third character then first and second need to be
				// the same string character (otherwise it's an unterminated string)
				if (!cp)
				{
					if (second == first)
						return string{};
					throw_parse_error("Encountered EOF while parsing string"sv);
				}
					
				// if the first three characters are all the same string delimiter then
				// it's a multi-line string.
				if (first == second && first == third)
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
			string parse_bare_key_segment()
			{
				TOML_ASSERT(cp && is_bare_key_start_character(*cp));

				string segment;

				while (cp)
				{
					if (!is_bare_key_character(*cp))
						break;

					segment.append(cp->as_view());
					advance();
				}

				return segment;
			}

			[[nodiscard]]
			bool parse_bool()
			{
				TOML_ASSERT(cp && (*cp == U't' || *cp == U'f'));

				auto result = *cp == U't';
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing boolean"sv);
					else
						throw_parse_error("Encountered unexpected character while parsing boolean; expected 'true' or 'false', saw '"sv, cp->as_view<char>(), '\'');
				}
				if (cp && !is_value_terminator(*cp))
					throw_parse_error("Encountered unexpected character while parsing boolean; expected value-terminator, saw '"sv, cp->as_view<char>(), '\'');

				return result;
			}

			[[nodiscard]]
			double parse_inf_or_nan()
			{
				TOML_ASSERT(cp && (*cp == U'i' || *cp == U'n' || *cp == U'+' || *cp == U'-'));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing floating-point"sv);
				};

				const int sign = *cp == U'-' ? -1 : 1;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
				}

				const bool inf = *cp == U'i';
				if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing floating-point; expected '"sv, inf ? "inf"sv : "nan"sv, "', saw '"sv, cp->as_view<char>(), '\'');
				}
				if (cp && !is_value_terminator(*cp))
					throw_parse_error("Encountered unexpected character while parsing floating-point; expected value-terminator, saw '"sv, cp->as_view<char>(), '\'');

				return inf
					? sign * std::numeric_limits<double>::infinity()
					: std::numeric_limits<double>::quiet_NaN();
			}

			#pragma warning(push)
			#pragma warning(disable: 4063) //case '0' is not a valid value for switch of enum 'std::errc'

			[[nodiscard]]
			double parse_float()
			{
				TOML_ASSERT(cp && (*cp == U'+' || *cp == U'-' || is_decimal_digit(*cp)));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing floating-point"sv);
				};

				// sign
				const int sign = *cp == U'-' ? -1 : 1;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
				}

				// consume value chars
				char chars[128];
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					bool seen_decimal = false;
					bool seen_exponent = false;
					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							throw_parse_error("Encountered unexpected character while parsing floating-point; underscores may only follow digits"sv);
					}
					else
					{
						if ((!prev || *prev == U'_') && !is_decimal_digit(*cp))
							throw_parse_error("Encountered unexpected character while parsing floating-point; expected decimal digit, saw '"sv, cp->as_view<char>(), '\'');
						if (*cp == U'.')
						{
							if (seen_decimal)
								throw_parse_error("Encountered unexpected character while parsing floating-point; decimal points may only appear once"sv);
							if (seen_exponent)
								throw_parse_error("Encountered unexpected character while parsing floating-point; decimal points may not appear after exponents"sv);
							seen_decimal = true;
						}
						else if (*cp == U'e' || *cp == U'E')
						{
							if (seen_exponent)
								throw_parse_error("Encountered unexpected character while parsing floating-point; exponents may only appear once"sv);
							seen_exponent = true;
						}

						if (length + cp->byte_count > sizeof(chars))
							throw_parse_error("Floating-point value out-of-range; exceeds maximum length of "sv, sizeof(chars), " characters"sv);
						memcpy(chars + length, cp->bytes, cp->byte_count);
						length += cp->byte_count;

					}
					prev = cp;
					advance();
				}

				// convert to double
				double result;
				auto parse_result = std::from_chars(chars, chars + length, result);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result * sign;

					case std::errc::invalid_argument:
						throw_parse_error("Error parsing floating-point; the character sequence could not be interpreted as a floating-point value"sv);

					case std::errc::result_out_of_range:
						throw_parse_error(
							"Error parsing floating-point; the character sequence contained a value not representable by a 64-bit floating-point"sv);

					default: //??
						throw_parse_error(
							"Error parsing floating-point; an unspecified error occurred while trying to interpret the character sequence as a floating-point value"sv);
				}
				TOML_UNREACHABLE;
			}

			#if !TOML_STRICT

			[[nodiscard]]
			double parse_hex_float()
			{
				TOML_ASSERT(cp && *cp == U'0');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing hexadecimal floating-point"sv);
				};

				TOML_NOT_IMPLEMENTED_YET;

				return {};
			}

			#endif //!TOML_STRICT

			[[nodiscard]]
			int64_t parse_binary_integer()
			{
				TOML_ASSERT(cp && *cp == U'0');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing binary integer"sv);
				};

				// '0'
				if (*cp != U'0')
					throw_parse_error("Encountered unexpected character while parsing binary integer; expected '0', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// 'b' or 'B'
				if (*cp != U'b' && *cp != U'B')
					throw_parse_error("Encountered unexpected character while parsing binary integer; expected 'b' or 'B', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// consume value chars
				char chars[127]; //127 == strlen("0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0")
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_binary_digit(*prev))
							throw_parse_error("Encountered unexpected character while parsing binary integer; underscores may only follow digits"sv);
					}
					else
					{
						if (!is_binary_digit(*cp))
							throw_parse_error("Encountered unexpected character while parsing binary integer; expected binary digit, saw '"sv, cp->as_view<char>(), '\'');
						TOML_ASSERT(cp->byte_count == 1_sz);
						if (length == sizeof(chars))
							throw_parse_error("Binary integer value out-of-range; exceeds maximum length of "sv, sizeof(chars), " characters"sv);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}
					prev = cp;
					advance();
				}

				// convert to int
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 2);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						throw_parse_error("Error parsing binary integer; the character sequence could not be interpreted as a binary integer value"sv);

					case std::errc::result_out_of_range:
						throw_parse_error("Error parsing binary integer; the character sequence contained a value not representable by a signed 64-bit integer"sv);

					default: //??
						throw_parse_error("Error parsing binary integer; an unspecified error occurred while trying to interpret the character sequence as a binary integer value"sv);
				}
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_octal_integer()
			{
				TOML_ASSERT(cp && *cp == U'0');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing octal integer"sv);
				};

				// '0'
				if (*cp != U'0')
					throw_parse_error("Encountered unexpected character while parsing octal integer; expected '0', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// 'o'
				if (*cp != U'o')
					throw_parse_error("Encountered unexpected character while parsing octal integer; expected 'o', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// consume value chars
				char chars[41]; //41 == strlen("7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7_7")
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_octal_digit(*prev))
							throw_parse_error("Encountered unexpected character while parsing octal integer; underscores may only follow digits"sv);
					}
					else
					{
						if (!is_octal_digit(*cp))
							throw_parse_error("Encountered unexpected character while parsing octal integer; expected octal digit, saw '"sv, cp->as_view<char>(), '\'');
						TOML_ASSERT(cp->byte_count == 1_sz);
						if (length == sizeof(chars))
							throw_parse_error("Octal integer value out-of-range; exceeds maximum length of "sv, sizeof(chars), " characters"sv);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}
					prev = cp;
					advance();
				}

				// convert to int
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 8);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						throw_parse_error("Error parsing octal integer; the character sequence could not be interpreted as a octal integer value"sv);

					case std::errc::result_out_of_range:
						throw_parse_error("Error parsing octal integer; the character sequence contained a value not representable by a signed 64-bit integer"sv);

					default: //??
						throw_parse_error("Error parsing octal integer; an unspecified error occurred while trying to interpret the character sequence as a octal integer value"sv);
				}
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_decimal_integer()
			{
				TOML_ASSERT(cp && (*cp == U'+' || *cp == U'-' || is_decimal_digit(*cp)));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing integer"sv);
				};

				// sign
				const int64_t sign = *cp == U'-' ? -1ll : 1ll;
				if (*cp == U'+' || *cp == U'-')
				{
					advance();
					eof_check();
				}

				// consume value chars
				char chars[37]; //37 == strlen("9_2_2_3_3_7_2_0_3_6_8_5_4_7_7_5_8_0_7")
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							throw_parse_error("Encountered unexpected character while parsing integer; underscores may only follow digits"sv);
					}
					else
					{
						if (!is_decimal_digit(*cp))
							throw_parse_error("Encountered unexpected character while parsing integer; expected decimal digit, saw '"sv, cp->as_view<char>(), '\'');
						TOML_ASSERT(cp->byte_count == 1_sz);
						if (length == sizeof(chars))
							throw_parse_error("Integer value out-of-range; exceeds maximum length of "sv, sizeof(chars), " characters"sv);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}
					prev = cp;
					advance();
				}
				TOML_ASSERT(length);

				// check for leading zeroes etc
				if (chars[0] == '0')
					throw_parse_error("Encountered unexpected character while parsing integer; leading zeroes are not allowed"sv);

				// convert to int
				uint64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				else if ((sign < 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1ull)
					|| (sign > 0 && result > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())))
					parse_result.ec = std::errc::result_out_of_range;
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result * sign;

					case std::errc::invalid_argument:
						throw_parse_error("Error parsing integer; the character sequence could not be interpreted as an integer value"sv);

					case std::errc::result_out_of_range:
						throw_parse_error("Error parsing integer; the character sequence contained a value not representable by a signed 64-bit integer"sv);

					default: //??
						throw_parse_error("Error parsing integer; an unspecified error occurred while trying to interpret the character sequence as an integer value"sv);
				}
				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			int64_t parse_hex_integer()
			{
				TOML_ASSERT(cp && *cp == U'0');
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing hexadecimal integer"sv);
				};

				// '0'
				if (*cp != U'0')
					throw_parse_error("Encountered unexpected character while parsing hexadecimal integer; expected '0', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// 'x' pr 'X'
				if (*cp != U'x' && *cp != U'X')
					throw_parse_error("Encountered unexpected character while parsing hexadecimal integer; expected 'x' or 'X', saw '"sv, cp->as_view<char>(), '\'');
				advance();
				eof_check();

				// consume value chars
				char chars[31]; //31 == strlen("0_0_1_1_2_2_3_3_A_A_B_B_C_C_D_D")
				size_t length = {};
				const utf8_codepoint* prev = {};
				while (true)
				{
					if (!cp || is_value_terminator(*cp))
						break;

					if (*cp == U'_')
					{
						if (!prev || !is_hex_digit(*prev))
							throw_parse_error("Encountered unexpected character while parsing hexadecimal integer; underscores may only follow digits");
					}
					else
					{
						if (!is_hex_digit(*cp))
							throw_parse_error("Encountered unexpected character while parsing hexadecimal integer; expected hexadecimal digit, saw '"sv, cp->as_view<char>(), '\'');
						TOML_ASSERT(cp->byte_count == 1_sz);
						if (length == sizeof(chars))
							throw_parse_error("Hexadecimal integer value out-of-range; exceeds maximum length of "sv, sizeof(chars), " characters"sv);
						chars[length++] = static_cast<char>(cp->bytes[0]);
					}
					prev = cp;
					advance();
				}

				// convert to int
				int64_t result;
				auto parse_result = std::from_chars(chars, chars + length, result, 16);
				if (parse_result.ec == std::errc{} && parse_result.ptr < (chars + length))
				{
					eof_check();
					parse_result.ec = std::errc::invalid_argument;
				}
				switch (parse_result.ec)
				{
					case std::errc{}: //ok
						return result;

					case std::errc::invalid_argument:
						throw_parse_error(
							"Error parsing hexadecimal integer; the character sequence '"sv,
							std::string_view{ chars, length },
							"' could not be interpreted as a hexadecimal integer value"
						);

					case std::errc::result_out_of_range:
						throw_parse_error(
							"Error parsing hexadecimal integer; the character sequence '"sv,
							std::string_view{ chars, length },
							"' contained a value not representable by a signed 64-bit integer"
						);

					default: //??
						throw_parse_error(
							"Error parsing hexadecimal integer; an unspecified error occurred while trying to interpret the character sequence '"sv,
							std::string_view{ chars, length },
							"' as a hexadecimal integer value"
						);
				}
				TOML_UNREACHABLE;
			}

			#pragma warning(pop) //disabled C4063: case '0' is not a valid value for switch of enum 'std::errc'

			[[nodiscard]]
			date parse_date(bool time_may_follow = false)
			{
				TOML_ASSERT(cp && is_decimal_digit(*cp));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing date"sv);
				};

				// "YYYY"
				uint32_t year_digits[4];
				if (!consume_digit_sequence(year_digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing date; expected 4-digit year, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto year = year_digits[3]
					+ year_digits[2] * 10u
					+ year_digits[1] * 100u
					+ year_digits[0] * 1000u;
				const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));

				// '-'
				eof_check();
				if (*cp != U'-')
					throw_parse_error("Encountered unexpected character while parsing date; expected '-', saw '"sv, cp->as_view<char>(), '\'');
				advance();

				// "MM"
				uint32_t month_digits[2];
				if (!consume_digit_sequence(month_digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing date; expected 2-digit month, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto month = month_digits[1] + month_digits[0] * 10u;
				if (month == 0u || month > 12u)
					throw_parse_error("Month value out-of-range; expected 1-12 (inclusive), saw "sv, month);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;

				// '-'
				eof_check();
				if (*cp != U'-')
					throw_parse_error("Encountered unexpected character while parsing date; expected '-', saw '"sv, cp->as_view<char>(), '\'');
				advance();

				// "DD"
				uint32_t day_digits[2];
				if (!consume_digit_sequence(day_digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing date; expected 2-digit day, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto day = day_digits[1] + day_digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					throw_parse_error("Day value out-of-range; expected 1-"sv, max_days_in_month, " (inclusive), saw "sv, day);

				if (!time_may_follow)
				{
					if (cp && !is_value_terminator(*cp))
						throw_parse_error("Encountered unexpected character while parsing date; expected value-terminator, saw '"sv, cp->as_view<char>(), '\'');
				}

				return
				{
					static_cast<uint16_t>(year),
					static_cast<uint8_t>(month),
					static_cast<uint8_t>(day)
				};
			}

			[[nodiscard]]
			time parse_time(bool offset_may_follow = false)
			{
				TOML_ASSERT(cp && is_decimal_digit(*cp));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing time"sv);
				};

				// "HH"
				uint32_t digits[2];
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing time; expected 2-digit hour, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					throw_parse_error("Hour value out-of-range; expected 0-23 (inclusive), saw "sv, hour);

				// ':'
				eof_check();
				if (*cp != U':')
					throw_parse_error("Encountered unexpected character while parsing time; expected ':', saw '"sv, cp->as_view<char>(), '\'');
				advance();

				// "MM"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing time; expected 2-digit minute, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					throw_parse_error("Minute value out-of-range; expected 0-59 (inclusive), saw "sv, minute);

				auto time = ::toml::time{
					static_cast<uint8_t>(hour),
					static_cast<uint8_t>(minute),
				};

				if constexpr (TOML_STRICT)
				{
					// ':'
					eof_check();
					if (*cp != U':')
						throw_parse_error("Encountered unexpected character while parsing time; expected ':', saw '"sv, cp->as_view<char>(), '\'');
					advance();
				}
				else
				{
					//early exit here if seconds are omitted
					//(extension as per https://github.com/toml-lang/toml/issues/671)
					if (cp && !is_value_terminator(*cp) && *cp != U':' && (!offset_may_follow || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
					throw_parse_error("Encountered unexpected character while parsing time; expected ':'"sv, (offset_may_follow ? ", offset"sv : ""sv), " or value-terminator, saw '"sv, cp->as_view<char>(), '\'');
					if (!cp || *cp != U':')
					return time;

					// ':'
					advance();
				}

				// "SS"
				if (!consume_digit_sequence(digits))
				{
					eof_check();
					throw_parse_error("Encountered unexpected character while parsing time; expected 2-digit second, saw '"sv, cp->as_view<char>(), '\'');
				}
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					throw_parse_error("Second value out-of-range; expected 0-59 (inclusive), saw "sv, second);
				time.second = static_cast<uint8_t>(second);

				//early exit here if the fractional is omitted
				if (cp && !is_value_terminator(*cp) && *cp != U'.' && (!offset_may_follow || (*cp != U'+' && *cp != U'-' && *cp != U'Z' && *cp != U'z')))
					throw_parse_error("Encountered unexpected character while parsing time; expected fractional"sv, (offset_may_follow ? ", offset"sv : ""sv), " or value-terminator, saw '"sv, cp->as_view<char>(), '\'');
				if (!cp || *cp != U'.')
					return time;

				// '.'
				advance();
				eof_check();

				// ".FFFFFFFFFFFFF"
				uint32_t fractional_digits[24]; //surely that will be enough
				auto digit_count = consume_variable_length_digit_sequence(fractional_digits);
				if (!digit_count)
					throw_parse_error("Encountered unexpected character while parsing time; expected fractional digits, saw '"sv, cp->as_view<char>(), '\'');
				if (digit_count == 24_sz && cp && is_decimal_digit(*cp))
					throw_parse_error("Fractional value out-of-range; exceeds maximum precision of 24"sv, second);

				uint64_t value = 0;
				uint64_t place = 1;
				for (; digit_count --> 0_sz;)
				{
					value += fractional_digits[digit_count] * place;
					place *= 10u;
				}
				time.microsecond = static_cast<uint32_t>(static_cast<double>(1000000ull * value) / place);

				return time;
			}

			[[nodiscard]]
			datetime parse_datetime()
			{
				TOML_ASSERT(cp && is_decimal_digit(*cp));
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing datetime"sv);
				};

				// "YYYY-MM-DD"
				auto date = parse_date(true);

				// ' ' or 'T'
				eof_check();
				if (*cp != U' ' && *cp != U'T' && *cp != U't')
					throw_parse_error("Encountered unexpected character while parsing datetime; expected space or 'T', saw '"sv, cp->as_view<char>(), '\'');
				advance();

				// "HH:MM:SS.fractional"
					
				auto time = parse_time(true);

				// offset
				std::optional<time_offset> offset;
				if (cp)
				{
					// zero offset ("Z")
					if (*cp == U'Z' || *cp == U'z')
					{
						offset.emplace(time_offset{});
						advance();
					}

					// explicit offset ("+/-HH:MM")
					else if (*cp == U'+' || *cp == U'-')
					{
						// sign
						int sign = *cp == U'-' ? -1 : 1;
						advance();
						eof_check();

						// "HH"
						uint32_t digits[2];
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							throw_parse_error("Encountered unexpected character while parsing datetime offset; expected 2-digit hour, saw '"sv, cp->as_view<char>(), '\'');
						}
						const auto hour = digits[1] + digits[0] * 10u;
						if (hour > 23u)
							throw_parse_error("Hour value out-of-range; expected 0-23 (inclusive), saw "sv, hour);

						// ':'
						eof_check();
						if (*cp != U':')
							throw_parse_error("Encountered unexpected character while parsing datetime offset; expected ':', saw '"sv, cp->as_view<char>(), '\'');
						advance();

						// "MM"
						if (!consume_digit_sequence(digits))
						{
							eof_check();
							throw_parse_error("Encountered unexpected character while parsing datetime offset; expected 2-digit minute, saw '"sv, cp->as_view<char>(), '\'');
						}
						const auto minute = digits[1] + digits[0] * 10u;
						if (minute > 59u)
							throw_parse_error("Minute value out-of-range; expected 0-59 (inclusive), saw "sv, minute);

						offset.emplace(time_offset{
							static_cast<int8_t>(static_cast<int>(hour) * sign),
							static_cast<int8_t>(static_cast<int>(minute)* sign)
						});
					}
				}

					
				if (cp && !is_value_terminator(*cp))
					throw_parse_error("Encountered unexpected character while parsing datetime; expected value-terminator, saw '"sv, cp->as_view<char>(), '\'');

				return {
					date,
					time,
					offset
				};
			}

			[[nodiscard]]
			std::unique_ptr<array> parse_array();

			[[nodiscard]]
			std::unique_ptr<table> parse_inline_table();

			[[nodiscard]]
			std::unique_ptr<node> parse_value()
			{
				TOML_ASSERT(cp && !is_value_terminator(*cp));

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
					else if (*cp == U't' || *cp == U'f')
						val = std::make_unique<value<bool>>(parse_bool());

					// arrays
					else if (*cp == U'[')
					{
						val = parse_array();
						if constexpr (TOML_STRICT)
						{
							if (!reinterpret_cast<array*>(val.get())->is_homogeneous())
								throw parse_error{ "Arrays cannot contain values of different types"s, val->region() };
						}
					}

					// inline tables
					else if (*cp == U'{')
						val = parse_inline_table();

					// inf or nan
					else if (*cp == U'i' || *cp == U'n')
						val = std::make_unique<value<double>>(parse_inf_or_nan());

					if (val)
						break;

					// value types from here down require more than one character
					// to unambiguously identify, so scan ahead a bit.
					constexpr size_t max_scan_chars = 32_sz;
					char32_t chars[max_scan_chars]{ *cp };
					size_t char_count = 1_sz;
					bool eof_while_scanning = false;
					for (size_t i = max_scan_chars-1_sz; i --> 0_sz;)
					{
						advance();
						if (!cp || is_value_terminator(*cp))
						{
							eof_while_scanning = !cp;
							break;
						}
						chars[char_count++] = *cp;
					}
					go_back(char_count);

					// if after scanning ahead we still only have one value character,
					// the only valid value type is an integer.
					if (char_count == 1_sz)
					{
						if (is_decimal_digit(chars[0]))
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[0] - U'0'));
							advance(); //skip the digit
							break;
						}

						//anything else would be ambiguous.
						throw_parse_error(eof_while_scanning ? "Encountered EOF while parsing value"sv : "Could not determine value type"sv);
					}

					// now things that can be identified from two characters
					TOML_ASSERT(char_count >= 2_sz);

					// numbers that begin with a sign
					const auto begins_with_sign = chars[0] == U'+' || chars[0] == U'-';
					const auto begins_with_digit = is_decimal_digit(chars[0]);
					if (begins_with_sign)
					{
						if (char_count == 2_sz && is_decimal_digit(chars[1]))
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[1] - U'0')* (chars[1] == U'-' ? -1LL : 1LL));
							advance(); //skip the sign
							advance(); //skip the digit
						}

						else if (chars[1] == U'i' || chars[1] == U'n')
							val = std::make_unique<value<double>>(parse_inf_or_nan());
						else if (is_decimal_digit(chars[1]) && (chars[2] == U'.' || chars[2] == U'e' || chars[2] == U'E'))
							val = std::make_unique<value<double>>(parse_float());
					}

					// numbers that begin with 0
					else if (chars[0] == U'0')
					{
						switch (chars[1])
						{
							case U'E': [[fallthrough]];
							case U'e': [[fallthrough]];
							case U'.': val = std::make_unique<value<double>>(parse_float()); break;
							case U'b': val = std::make_unique<value<int64_t>>(parse_binary_integer()); break;
							case U'o': val = std::make_unique<value<int64_t>>(parse_octal_integer()); break;
							case U'X': [[fallthrough]];
							case U'x':
							{
								#if !TOML_STRICT
								for (size_t i = char_count; i-- > 2_sz;)
								{
									if (chars[i] == U'p' || chars[i] == U'P')
									{
										val = std::make_unique<value<double>>(parse_hex_float());
										break;
									}
								}
								if (val)
									break;
								#endif //!TOML_STRICT

								val = std::make_unique<value<int64_t>>(parse_hex_integer());
								break;
							}
						}
					}

					//floats
					else if (begins_with_digit && (chars[1] == U'.' || chars[1] == U'e' || chars[1] == U'E'))
						val = std::make_unique<value<double>>(parse_float());

					if (val || !(begins_with_sign || begins_with_digit))
						break;

					// if we get here then all the easy cases are taken care of, so we need to do 
					// a 'deeper dive' to figure out what the value type could possibly be.
					// 
					// note that the dive is actually not that deep, which is intentional; we want
					// types to be 'loosely' determined here even if we could rule them out as being invalidly-formed
					// so that the selected parse method can emit better diagnostics when parsing fails,
					// e.g. "Failed to parse date-time; blah" instead of just "Could not determine value type".
						
					enum possible_value_types : int
					{
						impossibly_anything = 0,
						possibly_int = 1,
						possibly_float = 2,
						possibly_datetime = 4,
					};
					auto possible_types =
						possibly_int
						| possibly_float
						| (begins_with_digit && char_count >= 8_sz ? possibly_datetime : impossibly_anything) // strlen("HH:MM:SS")
					;
					std::optional<size_t> first_colon, first_minus;
					for (size_t i = 0; i < char_count; i++)
					{
						const auto digit = is_decimal_digit(chars[i]);
						const auto colon = chars[i] == U':';
						const auto minus = chars[i] == U'-';
						const auto sign = chars[i] == U'+' || minus;

						if (colon && !first_colon)
							first_colon = i;
						if (minus && !first_minus)
							first_minus = i;

						if ((possible_types & possibly_int))
						{
							if (colon || !(digit || (i == 0_sz && sign) || chars[i] == U'_'))
								possible_types &= ~possibly_int;
						}
						if ((possible_types & possibly_float))
						{
							if (colon || !(digit || sign
								|| chars[i] == U'_'
								|| chars[i] == U'.'
								|| chars[i] == U'e'
								|| chars[i] == U'E'))
								possible_types &= ~possibly_float;
						}

						if ((possible_types & possibly_datetime))
						{
							if (!(digit || (i >= 4_sz && sign)
								|| chars[i] == U'T'
								|| chars[i] == U'Z'
								|| chars[i] == U't'
								|| chars[i] == U'z'
								|| chars[i] == U':'
								|| chars[i] == U'.'))
								possible_types &= ~possibly_datetime;
						}

						if (!possible_types)
							break;
					}

					// resolve ambiguites
					if ((possible_types & (possibly_int | possibly_datetime)) == (possibly_int | possibly_datetime))
					{
						possible_types &= first_colon || first_minus.value_or(0_sz) > 0_sz
							? ~possibly_int
							: ~possibly_datetime;
					}
					if ((possible_types & (possibly_int | possibly_float)) == (possibly_int | possibly_float))
						possible_types &= ~possibly_float;

					// dispatch to appropriate parsing method
					switch (possible_types)
					{
						case possibly_int:
							val = std::make_unique<value<int64_t>>(parse_decimal_integer());
							break;

						case possibly_float:
							val = std::make_unique<value<double>>(parse_float());
							break;

						case possibly_datetime:
						{
							if (first_colon && !first_minus)
								val = std::make_unique<value<time>>(parse_time());
							else if (!first_colon && first_minus)
								val = std::make_unique<value<date>>(parse_date());
							else
								val = std::make_unique<value<datetime>>(parse_datetime());
							break;
						}

						default: break; // ??? still ambiguous
					}
				}
				while (false);

				if (!val)
					throw_parse_error("Could not determine value type"sv);

				val->rgn = { begin_pos, cp ? cp->position : prev_pos, reader.source_path() };
				return val;
			}

			struct key final
			{
				std::vector<string> segments;
			};

			[[nodiscard]]
			key parse_key()
			{
				TOML_ASSERT(cp && (is_bare_key_start_character(*cp) || is_string_delimiter(*cp)));

				key key;

				while (true)
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing key"sv);

					// bare_key_segment
					#if !TOML_STRICT
					if (is_unicode_combining_mark(*cp))
						throw_parse_error("Encountered unexpected character while parsing key; expected bare key starting character or string delimiter, saw combining mark '"sv, cp->as_view<char>(), '\'');
					#endif
					if (is_bare_key_start_character(*cp))
						key.segments.push_back(parse_bare_key_segment());

					// "quoted key segment"
					else if (is_string_delimiter(*cp))
						key.segments.push_back(parse_string());

					// ???
					else
						throw_parse_error("Encountered unexpected character while parsing key; expected bare key starting character or string delimiter, saw '"sv, cp->as_view<char>(), '\'');
						
					consume_leading_whitespace();

					// eof or no more key to come
					if (!cp || *cp != U'.')
						break;

					// was a dotted key, so go around again to consume the next segment
					TOML_ASSERT(*cp == U'.');
					advance();
					consume_leading_whitespace();
				}

				return key;
			}

			struct key_value_pair final
			{
				key key;
				std::unique_ptr<node> value;
			};

			[[nodiscard]]
			key_value_pair parse_key_value_pair()
			{
				const auto eof_check = [this]()
				{
					if (!cp)
						throw_parse_error("Encountered EOF while parsing key-value pair"sv);
				};

				// get the key
				TOML_ASSERT(cp && (is_string_delimiter(cp->value) || is_bare_key_start_character(cp->value)));
				auto key = parse_key();

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				eof_check();

				// consume the '='
				if (*cp != U'=')
					throw_parse_error("Encountered unexpected character while parsing key-value pair; expected '=', saw '"sv, cp->as_view<char>(), '\'');
				advance();

				// skip past any whitespace that followed the '='
				consume_leading_whitespace();
				eof_check();

				// get the value
				return { std::move(key), parse_value() };
			}

			[[nodiscard]]
			table* parse_table_header()
			{
				TOML_ASSERT(cp && *cp == U'[');

				const auto header_begin_pos = cp->position;
				document_position header_end_pos;
				key key;
				bool is_array = false;

				//parse header
				{
					const auto eof_check = [this]()
					{
						if (!cp)
							throw_parse_error("Encountered EOF while parsing table header"sv);
					};

					// skip first '['
					advance();
					eof_check();

					// skip second '[' (if present)
					if (*cp == U'[')
					{
						advance();
						eof_check();
						is_array = true;
					}

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					eof_check();

					// get the actual key
					key = parse_key();

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					eof_check();

					// consume the first closing ']'
					if (*cp != U']')
						throw_parse_error("Encountered unexpected character while parsing table"sv, (is_array ? " array"sv : ""sv), " header; expected ']', saw '"sv, cp->as_view<char>(), '\'');
					advance();

					// consume the second closing ']'
					if (is_array)
					{
						eof_check();

						if (*cp != U']')
							throw_parse_error("Encountered unexpected character while parsing table array header; expected ']', saw '"sv, cp->as_view<char>(), '\'');
						advance();
					}
					header_end_pos = cp ? cp->position : prev_pos;

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (cp)
					{
						if (!consume_comment() && !consume_line_break())
							throw_parse_error("Encountered unexpected character after table"sv, (is_array ? " array"sv : ""sv), " header; expected a comment or whitespace, saw '"sv, cp->as_view<char>(), '\'');
					}
				}
				TOML_ASSERT(!key.segments.empty());

				// check if each parent is a table/table array, or can be created implicitly as a table.
				auto parent = root.get();
				for (size_t i = 0; i < key.segments.size() - 1_sz; i++)
				{
					auto child = parent->get(key.segments[i]);
					if (!child)
					{
						child = parent->values.emplace(
							key.segments[i],
							std::make_unique<table>()
						).first->second.get();
						implicit_tables.push_back(reinterpret_cast<table*>(child));
						child->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
					}
					else if (child->is_table())
					{
						parent = reinterpret_cast<table*>(child);
					}
					else if (child->is_table_array())
					{
						// table arrays are a special case;
						// the spec dictates we select the most recently declared element in the array.
						TOML_ASSERT(!reinterpret_cast<table_array*>(child)->tables.empty());
						parent = reinterpret_cast<table_array*>(child)->tables.back().get();
					}
					else
					{
						TOML_NOT_IMPLEMENTED_YET;
						throw_parse_error("FFFFFFFFFFFFF"sv);
					}
				}

				// check the last parent table for a node matching the last key.
				// if there was no matching node, then sweet;
				// we can freely instantiate a new table/table array.
				auto matching_node = parent->get(key.segments.back());
				if (!matching_node)
				{
					// if it's an array we need to make the array and it's first table element, set the starting regions, and return the table element
					if (is_array)
					{
						auto tab_arr = reinterpret_cast<table_array*>(
							parent->values.emplace(
								key.segments.back(),
								std::make_unique<table_array>()
							).first->second.get()
						);
						tab_arr->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						
						tab_arr->tables.push_back(std::make_unique<table>());
						tab_arr->tables.back()->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab_arr->tables.back().get();
					}

					//otherwise we're just making a table
					else
					{
						auto tab = reinterpret_cast<table*>(
							parent->values.emplace(
								key.segments.back(),
								std::make_unique<table>()
							).first->second.get()
						);
						tab->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab;
					}
				}

				// if there was already a matching node some sanity checking is necessary;
				// this is ok if we're making an array and the existing element is already an array (new element)
				// or if we're making a table and the existing element is an implicitly-created table (promote it),
				// otherwise this is a redefinition error.
				else
				{
					if (is_array && matching_node->is_table_array())
					{
						auto tab_arr = reinterpret_cast<table_array*>(matching_node);
						tab_arr->tables.push_back(std::make_unique<table>());
						tab_arr->tables.back()->rgn = { header_begin_pos, header_end_pos, reader.source_path() };
						return tab_arr->tables.back().get();
					}

					else if (!is_array
						&& matching_node->is_table()
						&& !implicit_tables.empty())
					{
						auto tab = reinterpret_cast<table*>(matching_node);
						const auto iter = std::find(implicit_tables.cbegin(), implicit_tables.cend(), tab);
						if (iter != implicit_tables.cend())
						{
							implicit_tables.erase(iter);
							tab->rgn.begin = header_begin_pos;
							tab->rgn.end = header_end_pos;
							return tab;
						}
					}

					//if we get here it's a redefinition error.
					TOML_NOT_IMPLEMENTED_YET;
					throw_parse_error( "FFFFFFFFFFFFF"sv );
				}
			}

			void parse_document()
			{
				TOML_ASSERT(cp);
					
				table* current_table = root.get();

				do
				{
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
					#if !TOML_STRICT
					else if (is_unicode_combining_mark(*cp))
						throw_parse_error("Encountered unexpected character while parsing key; expected bare key starting character or string delimiter, saw combining mark '"sv, cp->as_view<char>(), '\'');
					#endif
					else if (is_bare_key_start_character(cp->value) || is_string_delimiter(cp->value))
					{
						auto kvp = parse_key_value_pair();

						//todo : check for collisions
						//todo : handle dotted keys
							
						// handle the rest of the line after the kvp
						// (this is not done in parse_key_value_pair() because that function is also used for inline tables)
						consume_leading_whitespace();
						if (cp)
						{
							if (!consume_comment() && !consume_line_break())
								throw_parse_error("Encountered unexpected character after key-value pair; expected a comment or whitespace, saw '"sv, cp->as_view<char>(), '\'');
						}
					}

					else // ??
						throw_parse_error("Encountered unexpected character while parsing top level of document; expected keys, tables, whitespace or comments, saw '"sv, cp->as_view<char>(), '\'');

				}
				while (cp);
			}

		public:

			parser(utf8_reader_interface&& reader_)
				: reader{ reader_ },
				root{ std::make_unique<table>() }
			{
				root->rgn = { prev_pos, prev_pos, reader.source_path() };

				cp = reader.read_next();
				if (cp)
					parse_document();
			}

			[[nodiscard]]
			operator std::unique_ptr<table>() && noexcept
			{
				return std::move(root);
			}
	};

	std::unique_ptr<array> parser::parse_array()
	{
		TOML_ASSERT(cp && *cp == U'[');
		const auto eof_check = [this]()
		{
			if (!cp)
				throw_parse_error("Encountered EOF while parsing array"sv);
		};

		// skip opening '['
		advance();
		eof_check();

		auto arr = std::make_unique<array>();
		auto& vals = arr->values;

		while (true)
		{
			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
				continue;
			eof_check();

			// commas - only legal after a value
			if (*cp == U',')
			{
				if (!vals.empty())
				{
					advance();
					continue;
				}
				throw_parse_error("Encountered unexpected character while parsing array; expected value or closing ']', saw ','"sv);
			}

			// closing ']'
			else if (*cp == U']')
			{
				advance();
				break;
			}

			// must be a value
			else
				vals.push_back(parse_value());
		}

		return arr;
	}

	std::unique_ptr<table> parser::parse_inline_table()
	{
		TOML_ASSERT(cp && *cp == U'{');
		const auto eof_check = [this]()
		{
			if (!cp)
				throw_parse_error("Encountered EOF while parsing inline table"sv);
		};

		// skip opening '{'
		advance();
		eof_check();

		auto tab = std::make_unique<table>();
		tab->inline_ = true;
		auto& vals = tab->values;

		while (true)
		{
			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
				continue;
			eof_check();

			// commas - only legal after a key-value pair
			if (*cp == U',')
			{
				if (!vals.empty())
				{
					advance();
					continue;
				}
				throw_parse_error("Encountered unexpected character while parsing inline table; expected key-value pair or closing '}', saw ','"sv);
			}

			// closing '}'
			else if (*cp == U'}')
			{
				advance();
				break;
			}

			// must be a key_value-pair
			else
			{
				auto kvp = parse_key_value_pair();

				//todo : check for collisions
				//todo : handle dotted keys

				TOML_NOT_IMPLEMENTED_YET;
			}
		}

		return tab;
	}
}

namespace TOML_NAMESPACE
{
	template <typename CHAR>
	[[nodiscard]]
	inline std::unique_ptr<table> parse(std::basic_string_view<CHAR> doc, string_view source_path = {})
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The string view's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}

	template <typename CHAR>
	[[nodiscard]]
	inline std::unique_ptr<table> parse(std::basic_istream<CHAR>& doc, string_view source_path = {})
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::parser{ impl::utf8_reader{ doc, source_path } };
	}
}
