//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_parser.h"
//# {{
#if !TOML_DOXYGEN
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}
TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#include <cmath>
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
	#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
	#include <sstream>
#endif
#if !TOML_ALL_INLINE
	using namespace std::string_view_literals;
#endif
TOML_POP_WARNINGS

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace TOML_INTERNAL_NAMESPACE
{
	template <uint64_t> struct parse_integer_traits;
	template <> struct parse_integer_traits<2> final
	{
		static constexpr auto scope_qualifier = "binary integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_binary_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 63;
		static constexpr char32_t prefix_codepoint = U'b';
		static constexpr char prefix = 'b';
	};
	template <> struct parse_integer_traits<8> final
	{
		static constexpr auto scope_qualifier = "octal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_octal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 21; // strlen("777777777777777777777")
		static constexpr char32_t prefix_codepoint = U'o';
		static constexpr char prefix = 'o';
	};
	template <> struct parse_integer_traits<10> final
	{
		static constexpr auto scope_qualifier = "decimal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_decimal_digit;
		static constexpr auto is_signed = true;
		static constexpr auto buffer_length = 19; //strlen("9223372036854775807")
	};
	template <> struct parse_integer_traits<16> final
	{
		static constexpr auto scope_qualifier = "hexadecimal integer"sv;
		static constexpr auto is_digit = ::toml::impl::is_hexadecimal_digit;
		static constexpr auto is_signed = false;
		static constexpr auto buffer_length = 16; //strlen("7FFFFFFFFFFFFFFF")
		static constexpr char32_t prefix_codepoint = U'x';
		static constexpr char prefix = 'x';
	};

	template <typename T>
	TOML_GNU_ATTR(nonnull)
	TOML_INTERNAL_LINKAGE
	void concatenate(char*& write_pos, char* buf_end, const T& arg) noexcept
	{
		using namespace ::toml;
		using namespace ::toml::impl;

		static_assert(!is_one_of<std::decay_t<T>, const char*, char*>);

		if (write_pos >= buf_end)
			return;
		const auto max_chars = static_cast<size_t>(buf_end - write_pos);

		using arg_t = remove_cvref_t<T>;
		if constexpr (std::is_same_v<arg_t, std::string_view> || std::is_same_v<arg_t, std::string>)
		{
			const auto len = max_chars < arg.length() ? max_chars : arg.length();
			std::memcpy(write_pos, arg.data(), len);
			write_pos += len;
		}
		else if constexpr (std::is_same_v<arg_t, char>)
		{
			*write_pos++ = arg;
		}
		else if constexpr (std::is_same_v<arg_t, utf8_codepoint>)
		{
			string_view cp_view;
			if TOML_UNLIKELY(arg.value <= U'\x1F')
				cp_view = low_character_escape_table[arg.value];
			else if TOML_UNLIKELY(arg.value == U'\x7F')
				cp_view = TOML_STRING_PREFIX("\\u007F"sv);
			else
				cp_view = arg.template as_view<string_char>();

			if (cp_view.length() <= max_chars)
				concatenate(write_pos, buf_end, cp_view);
		}
		else if constexpr (std::is_same_v<arg_t, bool>)
		{
			concatenate(write_pos, buf_end, arg ? "true"sv : "false"sv);
		}
		else if constexpr (std::is_same_v<arg_t, node_type>)
		{
			concatenate(
				write_pos,
				buf_end,
				impl::node_type_friendly_names[static_cast<std::underlying_type_t<node_type>>(arg)]
			);
		}
		else if constexpr (std::is_floating_point_v<arg_t>)
		{
			#if TOML_FLOAT_CHARCONV
			{
				const auto result = std::to_chars(write_pos, buf_end, arg);
				write_pos = result.ptr;
			}
			#else
			{
				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				ss.precision(std::numeric_limits<arg_t>::digits10 + 1);
				ss << arg;
				concatenate(write_pos, buf_end, std::move(ss).str());
			}
			#endif
		}
		else if constexpr (std::is_integral_v<arg_t>)
		{
			#if TOML_INT_CHARCONV
			{
				const auto result = std::to_chars(write_pos, buf_end, arg);
				write_pos = result.ptr;
			}
			#else
			{
				std::ostringstream ss;
				ss.imbue(std::locale::classic());
				using cast_type = std::conditional_t<std::is_signed_v<arg_t>, int64_t, uint64_t>;
				ss << static_cast<cast_type>(arg);
				concatenate(write_pos, buf_end, std::move(ss).str());
			}
			#endif
		}
	}

	struct parse_scope final
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
	};
	#define push_parse_scope_2(scope, line)		parse_scope ps_##line{ current_scope, scope }
	#define push_parse_scope_1(scope, line)		push_parse_scope_2(scope, line)
	#define push_parse_scope(scope)				push_parse_scope_1(scope, __LINE__)

	struct parsed_key final
	{
		std::vector<toml::string> segments;
	};

	struct parsed_key_value_pair final
	{
		parsed_key key;
		std::unique_ptr<toml::node> value;
	};
}

namespace toml::impl
{
	#if TOML_EXCEPTIONS && !defined(__INTELLISENSE__)
		#define TOML_RETURNS_BY_THROWING		[[noreturn]]
	#else
		#define TOML_RETURNS_BY_THROWING
	#endif

	#ifdef NDEBUG
		#define assert_or_assume(cond)			TOML_ASSUME(cond)
	#else
		#define assert_or_assume(cond)			TOML_ASSERT(cond)
	#endif

	// Q: "what the fuck is this? MACROS????"
	// A: The parser needs to work in exceptionless mode (returning error objects directly)
	//    and exception mode (reporting parse failures by throwing). Two totally different control flows.
	//    These macros encapsulate the differences between the two modes so I can write code code
	//    as though I was only targeting one mode and not want yeet myself into the sun.
	//    They're all #undef'd at the bottom of the parser's implementation so they should be harmless outside
	//    of toml++.

	#define is_eof()							!cp
	#define assert_not_eof()					assert_or_assume(cp)
	#define return_if_eof(...)					do { if (is_eof()) return __VA_ARGS__; } while(false)
	#if TOML_EXCEPTIONS
		#define is_error()						false
		#define return_after_error(...)			TOML_UNREACHABLE
		#define assert_not_error()				(void)0
		#define return_if_error(...)			(void)0
		#define return_if_error_or_eof(...)		return_if_eof(__VA_ARGS__)
	#else
		#define is_error()						!!err
		#define return_after_error(...)			return __VA_ARGS__
		#define assert_not_error()				TOML_ASSERT(!is_error())
		#define return_if_error(...)			do { if (is_error()) return __VA_ARGS__; } while(false)
		#define return_if_error_or_eof(...)		do { if (is_eof() || is_error()) return __VA_ARGS__; } while(false)
	#endif
	#define set_error_and_return(ret, ...)		\
		do { if (!is_error()) set_error(__VA_ARGS__); return_after_error(ret); } while(false)
	#define set_error_and_return_default(...)	set_error_and_return({}, __VA_ARGS__)
	#define set_error_and_return_if_eof(...)	\
		do { if (is_eof()) set_error_and_return(__VA_ARGS__, "encountered end-of-file"sv); } while(false)
	#define advance_and_return_if_error(...)	\
		do { assert_not_eof(); advance(); return_if_error(__VA_ARGS__); } while (false)
	#define advance_and_return_if_error_or_eof(...)		\
		do {											\
			assert_not_eof();							\
			advance();									\
			return_if_error(__VA_ARGS__);				\
			set_error_and_return_if_eof(__VA_ARGS__);	\
		} while (false)

	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		TOML_ABI_NAMESPACE_START(impl_noex)
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
			bool recording = false, recording_whitespace = true;
			std::string_view current_scope;
			#if !TOML_EXCEPTIONS
			mutable optional<toml::parse_error> err;
			#endif

			[[nodiscard]]
			source_position current_position(source_index fallback_offset = 0) const noexcept
			{
				if (!is_eof())
					return cp->position;
				return { prev_pos.line, static_cast<source_index>(prev_pos.column + fallback_offset) };
			}

			template <typename... T>
			TOML_RETURNS_BY_THROWING
			void set_error_at(source_position pos, const T&... reason) const TOML_MAY_THROW
			{
				static_assert(sizeof...(T) > 0_sz);
				#if !TOML_EXCEPTIONS
				if (err)
					return;
				#endif

				static constexpr auto buf_size = 512_sz;
				char buf[buf_size];
				auto write_pos = buf;
				const auto max_write_pos = buf + (buf_size - 1_sz); //allow for null terminator
				concatenate(write_pos, max_write_pos, "Error while parsing "sv);
				concatenate(write_pos, max_write_pos, current_scope);
				concatenate(write_pos, max_write_pos, ": "sv);
				(concatenate(write_pos, max_write_pos, reason), ...);
				*write_pos = '\0';
				#if TOML_EXCEPTIONS
					throw parse_error{ buf, pos, reader.source_path() };
				#else
					err.emplace(
						std::string(buf, static_cast<size_t>(write_pos - buf)),
						pos,
						reader.source_path()
					);
				#endif
			}

			template <typename... T>
			TOML_RETURNS_BY_THROWING
			TOML_ALWAYS_INLINE
			void set_error(const T&... reason) const TOML_MAY_THROW
			{
				set_error_at(current_position(1), reason...);
			}

			void go_back(size_t count = 1_sz) noexcept
			{
				return_if_error();
				assert_or_assume(count);

				cp = reader.step_back(count);
				prev_pos = cp->position;
			}

			void advance() TOML_MAY_THROW
			{
				return_if_error();
				assert_not_eof();

				prev_pos = cp->position;
				cp = reader.read_next();

				#if !TOML_EXCEPTIONS
				if (reader.error())
				{
					err = std::move(reader.error());
					return;
				}
				#endif

				if (recording && !is_eof())
				{
					if (recording_whitespace || !(is_whitespace(*cp) || is_line_break(*cp)))
						recording_buffer.append(cp->as_view<char>());
				}
			}

			void start_recording(bool include_current = true) noexcept
			{
				return_if_error();

				recording = true;
				recording_whitespace = true;
				recording_buffer.clear();
				if (include_current && !is_eof())
					recording_buffer.append(cp->as_view<char>());
			}

			void stop_recording(size_t pop_bytes = 0_sz) noexcept
			{
				return_if_error();

				recording = false;
				if (pop_bytes)
				{
					if (pop_bytes >= recording_buffer.length())
						recording_buffer.clear();
					else if (pop_bytes == 1_sz)
						recording_buffer.pop_back();
					else
						recording_buffer.erase(
							recording_buffer.begin() + static_cast<ptrdiff_t>(recording_buffer.length() - pop_bytes),
							recording_buffer.end()
						);
				}
			}

			bool consume_leading_whitespace() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				bool consumed = false;
				while (!is_eof() && is_whitespace(*cp))
				{
					consumed = true;
					advance_and_return_if_error({});
				}
				return consumed;
			}

			bool consume_line_break() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				if (!is_line_break(*cp))
					return false;

				if (*cp == U'\r')
				{
					advance_and_return_if_error({}); // skip \r

					if (is_eof())
						return true; //eof after \r is 'fine'
					else if (*cp != U'\n')
						set_error_and_return_default("expected \\n, saw '"sv, *cp, "'"sv);
				}
				advance_and_return_if_error({}); // skip \n (or other single-character line ending)
				return true;
			}

			bool consume_rest_of_line() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				do
				{
					if (is_line_break(*cp))
						return consume_line_break();
					else
						advance();
					return_if_error({});
				}
				while (!is_eof());

				return true;
			}

			bool consume_comment() TOML_MAY_THROW
			{
				return_if_error_or_eof({});

				if (*cp != U'#')
					return false;

				push_parse_scope("comment"sv);

				advance_and_return_if_error({}); //skip the '#'

				while (!is_eof())
				{
					if (consume_line_break())
						return true;

					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						// toml/issues/567 (disallow non-TAB control characters in comments)
						if (is_nontab_control_character(*cp))
							set_error_and_return_default(
								"control characters other than TAB (U+0009) are explicitly prohibited"sv
							);

						// toml/pull/720 (disallow surrogates in comments)
						else if (is_unicode_surrogate(*cp))
							set_error_and_return_default(
								"unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv
							);
					}
					advance_and_return_if_error({});
				}

				return true;
			}

			[[nodiscard]]
			bool consume_expected_sequence(std::u32string_view seq) TOML_MAY_THROW
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
			[[nodiscard]]
			bool consume_digit_sequence(T* digits, size_t len) TOML_MAY_THROW
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
			[[nodiscard]]
			size_t consume_variable_length_digit_sequence(T* buffer, size_t max_len) TOML_MAY_THROW
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

			template <bool MultiLine>
			[[nodiscard]]
			string parse_basic_string() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'"');
				push_parse_scope("string"sv);

				// skip the '"'
				advance_and_return_if_error_or_eof({});

				// multiline strings ignore a single line ending right at the beginning
				if constexpr (MultiLine)
				{
					consume_line_break();
					return_if_error({});
					set_error_and_return_if_eof({});
				}

				string str;
				bool escaped = false;
				[[maybe_unused]] bool skipping_whitespace = false;
				do
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
								return_if_error({});
								continue;
							}
						}

						bool skipped_escaped_codepoint = false;
						assert_not_eof();
						switch (const auto escaped_codepoint = *cp)
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
									set_error_and_return_default(
										"escape sequence '\\x' is not supported in TOML 1.0.0 and earlier"sv
									);
								}
								[[fallthrough]];
							case U'u': [[fallthrough]];
							case U'U':
							{
								push_parse_scope("unicode scalar escape sequence"sv);
								advance_and_return_if_error_or_eof({});
								skipped_escaped_codepoint = true;

								uint32_t place_value = escaped_codepoint == U'U'
									? 0x10000000u
									: (escaped_codepoint == U'u' ? 0x1000u : 0x10u);
								uint32_t sequence_value{};
								while (place_value)
								{
									set_error_and_return_if_eof({});
									if (!is_hexadecimal_digit(*cp))
										set_error_and_return_default("expected hex digit, saw '"sv, *cp, "'"sv);
									sequence_value += place_value * hex_to_dec(*cp);
									place_value /= 16u;
									advance_and_return_if_error({});
								}

								if (is_unicode_surrogate(sequence_value))
									set_error_and_return_default(
										"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv
									);
								else if (sequence_value > 0x10FFFFu)
									set_error_and_return_default("values greater than U+10FFFF are invalid"sv);
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
								set_error_and_return_default("unknown escape sequence '\\"sv, *cp, "'"sv);
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
							if constexpr (MultiLine)
							{
								size_t lookaheads = {};
								size_t consecutive_delimiters = 1_sz;
								do
								{
									advance_and_return_if_error({});
									lookaheads++;
									if (!is_eof() && *cp == U'"')
										consecutive_delimiters++;
									else
										break;
								}
								while (lookaheads < 4_sz);

								switch (consecutive_delimiters)
								{
									// """ " (one quote somewhere in a ML string)
									case 1_sz:
										str += TOML_STRING_PREFIX('"');
										skipping_whitespace = false;
										continue;

									// """ "" (two quotes somewhere in a ML string)
									case 2_sz:
										str.append(TOML_STRING_PREFIX("\"\""sv));
										skipping_whitespace = false;
										continue;

									// """ """ (the end of the string)
									case 3_sz:
										return str;

									// """ """" (one at the end of the string)
									case 4_sz:
										str += TOML_STRING_PREFIX('"');
										return str;

									// """ """"" (two quotes at the end of the string)
									case 5_sz:
										str.append(TOML_STRING_PREFIX("\"\""sv));
										advance_and_return_if_error({}); // skip the last '"'
										return str;

									TOML_NO_DEFAULT_CASE;
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
							escaped = true;
							continue;
						}

						// handle line endings in multi-line mode
						if constexpr (MultiLine)
						{
							if (is_line_break(*cp))
							{
								consume_line_break();
								return_if_error({});
								if (!skipping_whitespace)
									str += TOML_STRING_PREFIX('\n');
								continue;
							}
						}

						// handle control characters
						if (is_nontab_control_character(*cp))
							set_error_and_return_default(
								"unescaped control characters other than TAB (U+0009) are explicitly prohibited"sv
							);

						// handle surrogates in strings (1.0.0 and later)
						if constexpr (TOML_LANG_AT_LEAST(1, 0, 0)) 
						{
							if (is_unicode_surrogate(*cp))
								set_error_and_return_default(
									"unescaped unicode surrogates (U+D800 to U+DFFF) are explicitly prohibited"sv
								);
						}

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

						advance_and_return_if_error({});
					}
				}
				while (!is_eof());

				set_error_and_return_default("encountered end-of-file"sv);
			}

			template <bool MultiLine>
			[[nodiscard]]
			string parse_literal_string() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'\'');
				push_parse_scope("literal string"sv);

				// skip the delimiter
				advance_and_return_if_error_or_eof({});

				// multiline strings ignore a single line ending right at the beginning
				if constexpr (MultiLine)
				{
					consume_line_break();
					return_if_error({});
					set_error_and_return_if_eof({});
				}

				string str;
				do
				{
					assert_not_error();

					// handle closing delimiters
					if (*cp == U'\'')
					{
						if constexpr (MultiLine)
						{
							size_t lookaheads = {};
							size_t consecutive_delimiters = 1_sz;
							do
							{
								advance_and_return_if_error({});
								lookaheads++;
								if (!is_eof() && *cp == U'\'')
									consecutive_delimiters++;
								else
									break;
							}
							while (lookaheads < 4_sz);

							switch (consecutive_delimiters)
							{
								// ''' ' (one quote somewhere in a ML string)
								case 1_sz:
									str += TOML_STRING_PREFIX('\'');
									continue;

								// ''' '' (two quotes somewhere in a ML string)
								case 2_sz:
									str.append(TOML_STRING_PREFIX("''"sv));
									continue;

								// ''' ''' (the end of the string)
								case 3_sz:
									return str;

								// ''' '''' (one at the end of the string)
								case 4_sz:
									str += TOML_STRING_PREFIX('\'');
									return str;

								// ''' ''''' (two quotes at the end of the string)
								case 5_sz:
									str.append(TOML_STRING_PREFIX("''"sv));
									advance_and_return_if_error({}); // skip the last '
									return str;

								TOML_NO_DEFAULT_CASE;
							}
						}
						else
						{
							advance_and_return_if_error({}); // skip the closing delimiter
							return str;
						}
					}

					// handle line endings in multi-line mode
					if constexpr (MultiLine)
					{
						if (is_line_break(*cp))
						{
							consume_line_break();
							str += TOML_STRING_PREFIX('\n');
							continue;
						}
					}

					// handle control characters
					if (is_nontab_control_character(*cp))
						set_error_and_return_default(
							"control characters other than TAB (U+0009) are explicitly prohibited"sv
						);

					// handle surrogates in strings (1.0.0 and later)
					if constexpr (TOML_LANG_AT_LEAST(1, 0, 0))
					{
						if (is_unicode_surrogate(*cp))
							set_error_and_return_default(
								"unicode surrogates (U+D800 - U+DFFF) are explicitly prohibited"sv
							);
					}

					str.append(cp->as_view());
					advance_and_return_if_error({});
				}
				while (!is_eof());

				set_error_and_return_default("encountered end-of-file"sv);
			}

			struct parsed_string final
			{
				string value;
				bool was_multi_line;
			};

			[[nodiscard]]
			parsed_string parse_string() TOML_MAY_THROW
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
					return
					{
						first == U'\''
							? parse_literal_string<true>()
							: parse_basic_string<true>(),
						true
					};
				}

				// otherwise it's just a regular string.
				else
				{
					// step back two characters so that the current
					// character is the string delimiter
					go_back(2_sz);

					return
					{
						first == U'\''
							? parse_literal_string<false>()
							: parse_basic_string<false>(),
						false
					};
				}
			}

			[[nodiscard]]
			string parse_bare_key_segment() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_bare_key_character(*cp));

				string segment;

				while (!is_eof())
				{
					if (!is_bare_key_character(*cp))
						break;

					segment.append(cp->as_view());
					advance_and_return_if_error({});
				}

				return segment;
			}

			[[nodiscard]]
			bool parse_boolean() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U't', U'f', U'T', U'F'));
				push_parse_scope("boolean"sv);

				start_recording(true);
				auto result = is_match(*cp, U't', U'T');
				if (!consume_expected_sequence(result ? U"true"sv : U"false"sv))
					set_error_and_return_default(
						"expected '"sv, result ? "true"sv : "false"sv, "', saw '"sv, recording_buffer, '\''
					);
				stop_recording();

				if (cp && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, *cp, '\'');

				return result;
			}

			[[nodiscard]]
			double parse_inf_or_nan() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_match(*cp, U'i', U'n', U'I', U'N', U'+', U'-'));
				push_parse_scope("floating-point"sv);

				start_recording(true);
				const int sign = *cp == U'-' ? -1 : 1;
				if (is_match(*cp, U'+', U'-'))
					advance_and_return_if_error_or_eof({});

				const bool inf = is_match(*cp, U'i', U'I');
				if (!consume_expected_sequence(inf ? U"inf"sv : U"nan"sv))
					set_error_and_return_default(
						"expected '"sv, inf ? "inf"sv : "nan"sv, "', saw '"sv, recording_buffer, '\''
					);
				stop_recording();

				if (cp && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, *cp, '\'');

				return inf
					? sign * std::numeric_limits<double>::infinity()
					: std::numeric_limits<double>::quiet_NaN();
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_SWITCH_WARNINGS
			TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
			double parse_float() TOML_MAY_THROW
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
				size_t length = {};
				const utf8_codepoint* prev = {};
				bool seen_decimal = false, seen_exponent = false;
				while (!is_eof() && !is_value_terminator(*cp))
				{
					if (*cp == U'_')
					{
						if (!prev || !is_decimal_digit(*prev))
							set_error_and_return_default("underscores may only follow digits."sv);

						prev = cp;
						advance_and_return_if_error_or_eof({});
						continue;
					}
					else if (prev && *prev == U'_' && !is_decimal_digit(*cp))
						set_error_and_return_default("underscores must be followed by digits."sv);
					else if (*cp == U'.')
					{
						// 1.0e+.10 (exponent cannot have '.')
						if (seen_exponent)
							set_error_and_return_default("expected exponent decimal digit or sign, saw '.'"sv);

						// 1.0.e+.10
						// 1..0
						// (multiple '.')
						else if (seen_decimal)
							set_error_and_return_default("expected decimal digit or exponent, saw , saw '.'"sv);

						seen_decimal = true;
					}
					else if (is_match(*cp, U'e', U'E'))
					{
						// 1.0ee+10 (multiple 'e')
						if (seen_exponent)
							set_error_and_return_default("expected decimal digit, saw '"sv, *cp, '\'');

						seen_decimal = true; // implied
						seen_exponent = true;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 1.-0 (sign in mantissa)
						if (!seen_exponent)
							set_error_and_return_default("expected decimal digit or '.', saw '"sv, * cp, '\'');

						// 1.0e1-0 (misplaced exponent sign)
						else if (!is_match(*prev, U'e', U'E'))
							set_error_and_return_default("expected exponent digit, saw '"sv, *cp, '\'');
					}
					else if (!is_decimal_digit(*cp))
						set_error_and_return_default("expected decimal digit, saw '"sv, *cp, '\'');
					else if (length == sizeof(chars))
						set_error_and_return_default("exceeds maximum length of "sv, sizeof(chars), " characters."sv);


					chars[length++] = static_cast<char>(cp->bytes[0]);
					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity-check ending state
				if (prev)
				{
					if (*prev == U'_')
					{
						set_error_and_return_if_eof({});
						set_error_and_return_default("underscores must be followed by digits."sv);
					}
					else if (is_match(*prev, U'e', U'E', U'+', U'-'))
					{
						set_error_and_return_if_eof({});
						set_error_and_return_default("expected exponent digit, saw '"sv, *cp, '\'');
					}
				}

				// convert to double
				double result;
				#if TOML_FLOAT_CHARCONV
				{
					auto fc_result = std::from_chars(chars, chars + length, result);
					switch (fc_result.ec)
					{
						case std::errc{}: //ok
							return result * sign;

						case std::errc::invalid_argument:
							set_error_and_return_default(
								"'"sv, std::string_view{ chars, length }, "' could not be interpreted as a value."sv
							);
							break;

						case std::errc::result_out_of_range:
							set_error_and_return_default(
								"'"sv, std::string_view{ chars, length }, "' is not representable in 64 bits."sv
							);
							break;

						default: //??
							set_error_and_return_default(
								"an unspecified error occurred while trying to interpret '"sv,
								std::string_view{ chars, length }, "' as a value."sv
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
						set_error_and_return_default(
							"'"sv, std::string_view{ chars, length }, "' could not be interpreted as a value."sv
						);
				}
				#endif
			}

			[[nodiscard]]
			double parse_hex_float() TOML_MAY_THROW
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
					set_error_and_return_default(" expected '0', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// 'x' or 'X'
				if (!is_match(*cp, U'x', U'X'))
					set_error_and_return_default("expected 'x' or 'X', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

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
						set_error_and_return_default("underscores must be followed by digits."sv);
					else if (*cp == U'.')
					{
						// 0x10.0p-.0 (exponent cannot have '.')
						if (current_fragment == fragments + 2)
							set_error_and_return_default("expected exponent digit or sign, saw '.'"sv);

						// 0x10.0.p-0 (multiple '.')
						else if (current_fragment == fragments + 1)
							set_error_and_return_default("expected hexadecimal digit or exponent, , saw '.'"sv);

						else
							current_fragment++;
					}
					else if (is_match(*cp, U'p', U'P'))
					{
						// 0x10.0pp-0 (multiple 'p')
						if (current_fragment == fragments + 2)
							set_error_and_return_default("expected exponent digit or sign, saw '"sv, *cp, '\'');

						// 0x.p-0 (mantissa is just '.')
						else if (fragments[0].length == 0_sz && fragments[1].length == 0_sz)
							set_error_and_return_default("expected hexadecimal digit, saw '"sv, *cp, '\'');

						else
							current_fragment = fragments + 2;
					}
					else if (is_match(*cp, U'+', U'-'))
					{
						// 0x-10.0p-0 (sign in mantissa)
						if (current_fragment != fragments + 2)
							set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, *cp, '\'');

						// 0x10.0p0- (misplaced exponent sign)
						else if (!is_match(*prev, U'p', U'P'))
							set_error_and_return_default("expected exponent digit, saw '"sv, *cp, '\'');

						else
							exponent_sign = *cp == U'-' ? -1 : 1;
					}
					else if (current_fragment < fragments + 2 && !is_hexadecimal_digit(*cp))
						set_error_and_return_default("expected hexadecimal digit or '.', saw '"sv, *cp, '\'');
					else if (current_fragment == fragments + 2 && !is_decimal_digit(*cp))
						set_error_and_return_default("expected exponent digit or sign, saw '"sv, *cp, '\'');
					else if (current_fragment->length == sizeof(fragment::chars))
						set_error_and_return_default(
							"fragment exceeeds maximum length of "sv, sizeof(fragment::chars), " characters"sv
						);
					else
						current_fragment->chars[current_fragment->length++] = static_cast<char>(cp->bytes[0]);

					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity-check ending state
				if (current_fragment != fragments + 2 || current_fragment->length == 0_sz)
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("missing exponent"sv);
				}
				else if (prev && *prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits."sv);
				}

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

				set_error_and_return_default(
					"hexadecimal floating-point values are not supported "
					"in TOML 1.0.0 and earlier."sv
				);

				#endif // !TOML_LANG_UNRELEASED
			}

			template <uint64_t base>
			[[nodiscard]]
			int64_t parse_integer() TOML_MAY_THROW
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
						set_error_and_return_default("expected expected digit or sign, saw '"sv, *cp, '\'');
				}
				else
				{
					// '0'
					if (*cp != U'0')
						set_error_and_return_default("expected '0', saw '"sv, *cp, '\'');
					advance_and_return_if_error_or_eof({});

					// 'b', 'o', 'x'
					if (*cp != traits::prefix_codepoint)
						set_error_and_return_default("expected '"sv, traits::prefix, "', saw '"sv, *cp, '\'');
					advance_and_return_if_error_or_eof({});
				}

				// consume value chars
				char chars[traits::buffer_length];
				size_t length = {};
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
						set_error_and_return_default("underscores must be followed by digits."sv);
					else if (!traits::is_digit(*cp))
						set_error_and_return_default("expected digit, saw '"sv, *cp, '\'');
					else if (length == sizeof(chars))
						set_error_and_return_default("exceeds maximum length of "sv, sizeof(chars), " characters"sv);
					else
						chars[length++] = static_cast<char>(cp->bytes[0]);

					prev = cp;
					advance_and_return_if_error({});
				}

				// sanity check ending state
				if (prev && *prev == U'_')
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("underscores must be followed by digits."sv);
				}

				// check for leading zeroes
				if constexpr (base == 10)
				{
					if (chars[0] == '0')
						set_error_and_return_default("leading zeroes are prohibited"sv);
				}

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
					set_error_and_return_default(
						"'"sv, std::string_view{ chars, length }, "' is not representable in 64 bits"sv
					);

				if constexpr (traits::is_signed)
					return static_cast<int64_t>(result) * sign;
				else
					return static_cast<int64_t>(result);
			}

			[[nodiscard]]
			date parse_date(bool part_of_datetime = false) TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("date"sv);

				// "YYYY"
				uint32_t digits[4];
				if (!consume_digit_sequence(digits, 4_sz))
					set_error_and_return_default("expected 4-digit year, saw '"sv, *cp, '\'');
				const auto year = digits[3]
					+ digits[2] * 10u
					+ digits[1] * 100u
					+ digits[0] * 1000u;
				const auto is_leap_year = (year % 4u == 0u) && ((year % 100u != 0u) || (year % 400u == 0u));
				set_error_and_return_if_eof({});

				// '-'
				if (*cp != U'-')
					set_error_and_return_default("expected '-', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit month, saw '"sv, *cp, '\'');
				const auto month = digits[1] + digits[0] * 10u;
				if (month == 0u || month > 12u)
					set_error_and_return_default("expected month between 1 and 12 (inclusive), saw "sv, month);
				const auto max_days_in_month =
					month == 2u
					? (is_leap_year ? 29u : 28u)
					: (month == 4u || month == 6u || month == 9u || month == 11u ? 30u : 31u)
				;
				set_error_and_return_if_eof({});

				// '-'
				if (*cp != U'-')
					set_error_and_return_default("expected '-', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "DD"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit day, saw '"sv, *cp, '\'');
				const auto day = digits[1] + digits[0] * 10u;
				if (day == 0u || day > max_days_in_month)
					set_error_and_return_default(
						"expected day between 1 and "sv, max_days_in_month, " (inclusive), saw "sv, day
					);

				if (!part_of_datetime && !is_eof() && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, *cp, '\'');

				return
				{
					static_cast<uint16_t>(year),
					static_cast<uint8_t>(month),
					static_cast<uint8_t>(day)
				};
			}

			[[nodiscard]]
			time parse_time(bool part_of_datetime = false) TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("time"sv);

				static constexpr auto max_digits = 9_sz;
				uint32_t digits[max_digits];

				// "HH"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit hour, saw '"sv, *cp, '\'');
				const auto hour = digits[1] + digits[0] * 10u;
				if (hour > 23u)
					set_error_and_return_default("expected hour between 0 to 59 (inclusive), saw "sv, hour);
				set_error_and_return_if_eof({});

				// ':'
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "MM"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit minute, saw '"sv, *cp, '\'');
				const auto minute = digits[1] + digits[0] * 10u;
				if (minute > 59u)
					set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv, minute);
				auto time = ::toml::time{
					static_cast<uint8_t>(hour),
					static_cast<uint8_t>(minute),
				};

				// ':'
				if constexpr (TOML_LANG_UNRELEASED) // toml/issues/671 (allow omission of seconds)
				{
					if (is_eof()
						|| is_value_terminator(*cp)
						|| (part_of_datetime && is_match(*cp, U'+', U'-', U'Z')))
						return time;
				}
				else
					set_error_and_return_if_eof({});
				if (*cp != U':')
					set_error_and_return_default("expected ':', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "SS"
				if (!consume_digit_sequence(digits, 2_sz))
					set_error_and_return_default("expected 2-digit second, saw '"sv, *cp, '\'');
				const auto second = digits[1] + digits[0] * 10u;
				if (second > 59u)
					set_error_and_return_default("expected second between 0 and 59 (inclusive), saw "sv, second);
				time.second = static_cast<uint8_t>(second);


				// '.' (early-exiting is allowed; fractional is optional)
				if (is_eof()
					|| is_value_terminator(*cp)
					|| (part_of_datetime && is_match(*cp, U'+', U'-', U'Z')))
					return time;
				if (*cp != U'.')
					set_error_and_return_default("expected '.', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "FFFFFFFFF"
				auto digit_count = consume_variable_length_digit_sequence(digits, max_digits);
				if (!digit_count)
				{
					set_error_and_return_if_eof({});
					set_error_and_return_default("expected fractional digits, saw '"sv, *cp, '\'');
				}
				else if (!is_eof())
				{
					if (digit_count == max_digits && is_decimal_digit(*cp))
						set_error_and_return_default("fractional component exceeds maximum precision of "sv, max_digits);
					else if (!part_of_datetime && !is_value_terminator(*cp))
						set_error_and_return_default("expected value-terminator, saw '"sv, *cp, '\'');
				}

				uint32_t value = 0u;
				uint32_t place = 1u;
				for (auto i = digit_count; i --> 0_sz;)
				{
					value += digits[i] * place;
					place *= 10u;
				}
				for (auto i = digit_count; i < max_digits; i++) //implicit zeros
					value *= 10u;
				time.nanosecond = value;
				return time;
			}

			[[nodiscard]]
			date_time parse_date_time() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_decimal_digit(*cp));
				push_parse_scope("date-time"sv);

				// "YYYY-MM-DD"
				auto date = parse_date(true);
				set_error_and_return_if_eof({});

				// ' ' or 'T'
				if (!is_match(*cp, U' ', U'T'))
					set_error_and_return_default("expected space or 'T', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// "HH:MM:SS.FFFFFFFFF"
				auto time = parse_time(true);
				return_if_error({});

				// no offset
				if (is_eof() || is_value_terminator(*cp))
					return { date, time };

				// zero offset ("Z")
				time_offset offset;
				if (*cp == U'Z')
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
					if (!consume_digit_sequence(digits, 2_sz))
						set_error_and_return_default("expected 2-digit hour, saw '"sv, *cp, '\'');
					const auto hour = digits[1] + digits[0] * 10;
					if (hour > 23)
						set_error_and_return_default("expected hour between 0 and 23 (inclusive), saw "sv, hour);
					set_error_and_return_if_eof({});

					// ':'
					if (*cp != U':')
						set_error_and_return_default("expected ':', saw '"sv, *cp, '\'');
					advance_and_return_if_error_or_eof({});

					// "MM"
					if (!consume_digit_sequence(digits, 2_sz))
						set_error_and_return_default("expected 2-digit minute, saw '"sv, *cp, '\'');
					const auto minute = digits[1] + digits[0] * 10;
					if (minute > 59)
						set_error_and_return_default("expected minute between 0 and 59 (inclusive), saw "sv, hour);
					offset.minutes = static_cast<int16_t>((hour * 60 + minute) * sign);
				}

				if (!is_eof() && !is_value_terminator(*cp))
					set_error_and_return_default("expected value-terminator, saw '"sv, *cp, '\'');

				return { date, time, offset };
			}

			TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_INIT_WARNINGS

			[[nodiscard]]
			std::unique_ptr<toml::array> parse_array() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<toml::table> parse_inline_table() TOML_MAY_THROW;

			[[nodiscard]]
			std::unique_ptr<node> parse_value() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(!is_value_terminator(*cp));
				push_parse_scope("value"sv);

				// check if it begins with some control character
				// (note that this will also fail for whitespace but we're assuming we've
				// called consume_leading_whitespace() before calling parse_value())
				if TOML_UNLIKELY(is_control_character(*cp))
					set_error_and_return_default("unexpected control character"sv);

				// underscores at the beginning
				else if (*cp == U'_')
					set_error_and_return_default("values may not begin with underscores"sv);

				const auto begin_pos = cp->position;
				std::unique_ptr<node> val;

				do
				{
					assert_or_assume(!is_control_character(*cp));
					assert_or_assume(*cp != U'_');

					// detect the value type and parse accordingly,
					// starting with value types that can be detected
					// unambiguously from just one character.

					// arrays
					if (*cp == U'[')
					{
						val = parse_array();
						if constexpr (!TOML_LANG_AT_LEAST(1, 0, 0)) // toml/issues/665 (heterogeneous arrays)
						{
							if (!val->ref_cast<array>().is_homogeneous())
								set_error_at(
									begin_pos,
									"arrays cannot contain values of different types before TOML 1.0.0"
								);
						}
					}

					// inline tables
					else if (*cp == U'{')
						val = parse_inline_table();

					// floats beginning with '.'
					else if (*cp == U'.')
						val = std::make_unique<value<double>>(parse_float());

					// strings
					else if (is_string_delimiter(*cp))
						val = std::make_unique<value<string>>(std::move(parse_string().value));

					// bools
					else if (is_match(*cp, U't', U'f', U'T', U'F'))
						val = std::make_unique<value<bool>>(parse_boolean());

					// inf or nan
					else if (is_match(*cp, U'i', U'n', U'I', U'N'))
						val = std::make_unique<value<double>>(parse_inf_or_nan());

					return_if_error({});
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
						#define signs_msk (has_plus | has_minus)
						#define bzero_msk (begins_zero | has_digits)
						#define bdigit_msk (begins_digit | has_digits)
					};
					value_traits traits = has_nothing;
					const auto has_any = [&](auto t) noexcept { return (traits & t) != has_nothing; };
					const auto has_none = [&](auto t) noexcept { return (traits & t) == has_nothing; };
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
					const auto scan = [&]() TOML_MAY_THROW
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
											if (char_count == 2_sz && has_any(begins_zero))
												add_trait(has_b);
											break;

										case U'e':
											if (char_count > 1_sz
												&& has_none(has_b | has_o | has_p | has_t | has_x | has_z | has_colon)
												&& (has_none(has_plus | has_minus) || has_any(begins_sign)))
												add_trait(has_e);
											break;

										case U'o':
											if (char_count == 2_sz && has_any(begins_zero))
												add_trait(has_o);
											break;

										case U'p':
											if (has_any(has_x))
												add_trait(has_p);
											break;

										case U'x':
											if ((char_count == 2_sz && has_any(begins_zero))
												|| (char_count == 3_sz && has_any(begins_sign) && chars[1] == U'0'))
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
						while (advance_count < utf8_buffered_reader::max_history_length
							&& !is_eof()
							&& !is_value_terminator(*cp)
						);
					};
					scan();
					return_if_error({});

					// force further scanning if this could have been a date-time with a space instead of a T
					if (char_count == 10_sz
						&& traits == (bdigit_msk | has_minus)
						&& chars[4] == U'-'
						&& chars[7] == U'-'
						&& !is_eof()
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
						if (has_any(begins_zero | begins_digit))
						{
							val = std::make_unique<value<int64_t>>(static_cast<int64_t>(chars[0] - U'0'));
							advance(); //skip the digit
							break;
						}

						//anything else would be ambiguous.
						else
							set_error_and_return_default(
								eof_while_scanning
									? "encountered end-of-file"sv
									: "could not determine value type"sv
							);
					}

					// now things that can be identified from two or more characters
					return_if_error({});
					assert_or_assume(char_count >= 2_sz);

					// do some 'fuzzy matching' where there's no ambiguity, since that allows the specific
					// typed parse functions to take over and show better diagnostics if there's an issue
					// (as opposed to the fallback "could not determine type" message)
					if (has_any(has_p))
						val = std::make_unique<value<double>>(parse_hex_float());
					else if (has_any(has_x))
						val = std::make_unique<value<int64_t>>(parse_integer<16>());
					else if (has_any(has_o))
						val = std::make_unique<value<int64_t>>(parse_integer<8>());
					else if (has_any(has_b))
						val = std::make_unique<value<int64_t>>(parse_integer<2>());
					else if (has_any(has_e) || (has_any(begins_zero | begins_digit) && chars[1] == U'.'))
						val = std::make_unique<value<double>>(parse_float());
					else if (has_any(begins_sign))
					{
						// single-digit signed integers
						if (char_count == 2_sz && has_any(has_digits))
						{
							val = std::make_unique<value<int64_t>>(
								static_cast<int64_t>(chars[1] - U'0')
								* (chars[1] == U'-' ? -1LL : 1LL)
							);
							advance(); //skip the sign
							advance(); //skip the digit
							break;
						}

						// simple signed floats (e.g. +1.0)
						if (is_decimal_digit(chars[1]) && chars[2] == U'.')
							val = std::make_unique<value<double>>(parse_float());

						// signed infinity or nan
						else if (is_match(chars[1], U'i', U'n', U'I', U'N'))
							val = std::make_unique<value<double>>(parse_inf_or_nan());
					}

					return_if_error({});
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
						case bzero_msk | has_b:
							val = std::make_unique<value<int64_t>>(parse_integer<2>());
							break;

						//=================== octal integers
						// 0o10
						case bzero_msk | has_o:
							val = std::make_unique<value<int64_t>>(parse_integer<8>());
							break;

						//=================== decimal integers
						// 00
						// 10
						// +10
						// -10
						case bzero_msk:															[[fallthrough]];
						case bdigit_msk:														[[fallthrough]];
						case begins_sign | has_digits | has_minus:								[[fallthrough]];
						case begins_sign | has_digits | has_plus:
							val = std::make_unique<value<int64_t>>(parse_integer<10>());
							break;

						//=================== hexadecimal integers
						// 0x10
						case bzero_msk | has_x:
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
						case bzero_msk | has_e:													[[fallthrough]];
						case bzero_msk | has_e | has_minus:										[[fallthrough]];
						case bzero_msk | has_e | has_plus:										[[fallthrough]];
						case bzero_msk | has_dot:												[[fallthrough]];
						case bzero_msk | has_dot | has_e:										[[fallthrough]];
						case bzero_msk | has_dot | has_e | has_minus:							[[fallthrough]];
						case bzero_msk | has_dot | has_e | has_plus:							[[fallthrough]];
						// 1e1
						// 1e-1
						// 1e+1
						// 1.0
						// 1.0e1
						// 1.0e-1
						// 1.0e+1
						case bdigit_msk | has_e:												[[fallthrough]];
						case bdigit_msk | has_e | has_minus:									[[fallthrough]];
						case bdigit_msk | has_e | has_plus:										[[fallthrough]];
						case bdigit_msk | has_dot:												[[fallthrough]];
						case bdigit_msk | has_dot | has_e:										[[fallthrough]];
						case bdigit_msk | has_dot | has_e | has_minus:							[[fallthrough]];
						case bdigit_msk | has_dot | has_e | has_plus:							[[fallthrough]];
						// +1e1
						// +1.0
						// +1.0e1
						// +1.0e+1
						// +1.0e-1
						// -1.0e+1
						case begins_sign | has_digits | has_e | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_plus:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | signs_msk:			[[fallthrough]];
						// -1e1
						// -1e+1
						// +1e-1
						// -1.0
						// -1.0e1
						// -1.0e-1
						case begins_sign | has_digits | has_e | has_minus:						[[fallthrough]];
						case begins_sign | has_digits | has_e | signs_msk:						[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_minus:					[[fallthrough]];
						case begins_sign | has_digits | has_dot | has_e | has_minus:
							val = std::make_unique<value<double>>(parse_float());
							break;

						//=================== hexadecimal floats
						// 0x10p0
						// 0x10p-0
						// 0x10p+0
						case bzero_msk | has_x | has_p:											[[fallthrough]];
						case bzero_msk | has_x | has_p | has_minus:								[[fallthrough]];
						case bzero_msk | has_x | has_p | has_plus:								[[fallthrough]];
						// -0x10p0
						// -0x10p-0
						// +0x10p0
						// +0x10p+0
						// -0x10p+0
						// +0x10p-0
						case begins_sign | has_digits | has_x | has_p | has_minus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | has_plus:				[[fallthrough]];
						case begins_sign | has_digits | has_x | has_p | signs_msk:				[[fallthrough]];
						// 0x10.1p0
						// 0x10.1p-0
						// 0x10.1p+0
						case bzero_msk | has_x | has_dot | has_p:								[[fallthrough]];
						case bzero_msk | has_x | has_dot | has_p | has_minus:					[[fallthrough]];
						case bzero_msk | has_x | has_dot | has_p | has_plus:					[[fallthrough]];
						// -0x10.1p0
						// -0x10.1p-0
						// +0x10.1p0
						// +0x10.1p+0
						// -0x10.1p+0
						// +0x10.1p-0
						case begins_sign | has_digits | has_x | has_dot | has_p | has_minus:	[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | has_plus:		[[fallthrough]];
						case begins_sign | has_digits | has_x | has_dot | has_p | signs_msk:
							val = std::make_unique<value<double>>(parse_hex_float());
							break;

						//=================== times
						// HH:MM
						// HH:MM:SS
						// HH:MM:SS.FFFFFF
						case bzero_msk | has_colon:												[[fallthrough]];
						case bzero_msk | has_colon | has_dot:									[[fallthrough]];
						case bdigit_msk | has_colon:											[[fallthrough]];
						case bdigit_msk | has_colon | has_dot:
							val = std::make_unique<value<time>>(parse_time());
							break;

						//=================== local dates
						// YYYY-MM-DD
						case bzero_msk | has_minus:												[[fallthrough]];
						case bdigit_msk | has_minus:
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
						case bzero_msk | has_minus | has_colon | has_t:							[[fallthrough]];
						case bzero_msk | signs_msk | has_colon | has_t:							[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_t:						[[fallthrough]];
						case bdigit_msk | signs_msk | has_colon | has_t:						[[fallthrough]];
						// YYYY-MM-DDTHH:MM:SS.FFFFFF
						// YYYY-MM-DDTHH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DDTHH:MM:SS.FFFFFF+HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF
						// YYYY-MM-DD HH:MM:SS.FFFFFF-HH:MM
						// YYYY-MM-DD HH:MM:SS.FFFFFF+HH:MM
						case bzero_msk | has_minus | has_colon | has_dot | has_t:				[[fallthrough]];
						case bzero_msk | signs_msk | has_colon | has_dot | has_t:				[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_dot | has_t:				[[fallthrough]];
						case bdigit_msk | signs_msk | has_colon | has_dot | has_t:				[[fallthrough]];
						// YYYY-MM-DDTHH:MMZ
						// YYYY-MM-DD HH:MMZ
						// YYYY-MM-DDTHH:MM:SSZ
						// YYYY-MM-DD HH:MM:SSZ
						// YYYY-MM-DDTHH:MM:SS.FFFFFFZ
						// YYYY-MM-DD HH:MM:SS.FFFFFFZ
						case bzero_msk | has_minus | has_colon | has_z | has_t:					[[fallthrough]];
						case bzero_msk | has_minus | has_colon | has_dot | has_z | has_t:		[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_z | has_t:				[[fallthrough]];
						case bdigit_msk | has_minus | has_colon | has_dot | has_z | has_t:
							val = std::make_unique<value<date_time>>(parse_date_time());
							break;
					}

					#undef signs_msk
					#undef bzero_msk
					#undef bdigit_msk
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

			[[nodiscard]]
			parsed_key parse_key() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_bare_key_character(*cp) || is_string_delimiter(*cp));
				push_parse_scope("key"sv);

				parsed_key key;
				recording_whitespace = false;

				while (!is_error())
				{
					#if TOML_LANG_UNRELEASED // toml/issues/687 (unicode bare keys)
					if (is_unicode_combining_mark(*cp))
						set_error_and_return_default("bare keys may not begin with unicode combining marks"sv);
					else
					#endif

					// bare_key_segment
					if (is_bare_key_character(*cp))
						key.segments.emplace_back(parse_bare_key_segment());

					// "quoted key segment"
					else if (is_string_delimiter(*cp))
					{
						const auto begin_pos = cp->position;

						recording_whitespace = true;
						auto str = parse_string();
						recording_whitespace = false;
						return_if_error({});

						if (str.was_multi_line)
						{
							set_error_at(
								begin_pos,
								"multi-line strings are prohibited in "sv,
								key.segments.empty() ? ""sv : "dotted "sv,
								"keys"sv
							);
							return_after_error({});
						}
						else
							key.segments.emplace_back(std::move(str.value));
					}

					// ???
					else
						set_error_and_return_default(
							"expected bare key starting character or string delimiter, saw '"sv, *cp, '\''
						);
						
					// whitespace following the key segment
					consume_leading_whitespace();

					// eof or no more key to come
					if (is_eof() || *cp != U'.')
						break;

					// was a dotted key, so go around again to consume the next segment
					advance_and_return_if_error_or_eof({});
					consume_leading_whitespace();
					set_error_and_return_if_eof({});
				}
				return_if_error({});
				return key;
			}

			[[nodiscard]]
			parsed_key_value_pair parse_key_value_pair() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(is_string_delimiter(*cp) || is_bare_key_character(*cp));
				push_parse_scope("key-value pair"sv);

				// get the key
				start_recording();
				auto key = parse_key();
				stop_recording(1_sz);

				// skip past any whitespace that followed the key
				consume_leading_whitespace();
				set_error_and_return_if_eof({});

				// '='
				if (*cp != U'=')
					set_error_and_return_default("expected '=', saw '"sv, *cp, '\'');
				advance_and_return_if_error_or_eof({});

				// skip past any whitespace that followed the '='
				consume_leading_whitespace();
				return_if_error({});
				set_error_and_return_if_eof({});

				// get the value
				if (is_value_terminator(*cp))
					set_error_and_return_default("expected value, saw '"sv, *cp, '\'');
				return { std::move(key), parse_value() };
			}

			[[nodiscard]]
			table* parse_table_header() TOML_MAY_THROW
			{
				return_if_error({});
				assert_not_eof();
				assert_or_assume(*cp == U'[');
				push_parse_scope("table header"sv);

				const auto header_begin_pos = cp->position;
				source_position header_end_pos;
				parsed_key key;
				bool is_arr = false;

				//parse header
				{
					// skip first '['
					advance_and_return_if_error_or_eof({});

					// skip second '[' (if present)
					if (*cp == U'[')
					{
						is_arr = true;
						advance_and_return_if_error_or_eof({});
					}

					// skip past any whitespace that followed the '['
					consume_leading_whitespace();
					set_error_and_return_if_eof({});

					// get the actual key
					start_recording();
					key = parse_key();
					stop_recording(1_sz);
					return_if_error({});

					// skip past any whitespace that followed the key
					consume_leading_whitespace();
					return_if_error({});
					set_error_and_return_if_eof({});

					// consume the closing ']'
					if (*cp != U']')
						set_error_and_return_default("expected ']', saw '"sv, *cp, '\'');
					if (is_arr)
					{
						advance_and_return_if_error_or_eof({});
						if (*cp != U']')
							set_error_and_return_default("expected ']', saw '"sv, *cp, '\'');
					}
					advance_and_return_if_error({});
					header_end_pos = current_position(1);

					// handle the rest of the line after the header
					consume_leading_whitespace();
					if (!is_eof() && !consume_comment() && !consume_line_break())
						set_error_and_return_default("expected a comment or whitespace, saw '"sv, *cp, '\'');
				}
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
							set_error_and_return_default("cannot redefine existing table '"sv, recording_buffer, '\'');
						else
							set_error_and_return_default(
								"cannot redefine existing "sv, child->type(),
								" '"sv, recording_buffer,
								"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
							);
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
						set_error_and_return_default("cannot redefine existing table '"sv, recording_buffer, '\'');
					else
						set_error_and_return_default(
							"cannot redefine existing "sv, matching_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, is_arr ? "array-of-tables"sv : "table"sv
						);
				}
			}

			void parse_key_value_pair_and_insert(table* tab) TOML_MAY_THROW
			{
				return_if_error();
				assert_not_eof();
				push_parse_scope("key-value pair"sv);

				auto kvp = parse_key_value_pair();
				return_if_error();

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
							set_error("cannot redefine existing "sv, child->type(), " as dotted key-value pair"sv);
						else
							child->source_.end = kvp.value->source_.end;
						return_if_error();
						tab = &child->ref_cast<table>();
					}
				}

				if (auto conflicting_node = tab->get(kvp.key.segments.back()))
				{
					if (conflicting_node->type() == kvp.value->type())
						set_error(
							"cannot redefine existing "sv, conflicting_node->type(), " '"sv, recording_buffer, '\''
						);
					else
						set_error(
							"cannot redefine existing "sv, conflicting_node->type(),
							" '"sv, recording_buffer,
							"' as "sv, kvp.value->type()
						);
				}

				return_if_error();
				tab->values.emplace(
					std::move(kvp.key.segments.back()),
					std::move(kvp.value)
				);
			}

			void parse_document() TOML_MAY_THROW
			{
				assert_not_error();
				assert_not_eof();
				push_parse_scope("root table"sv);
					
				table* current_table = &root;

				do
				{
					return_if_error();

					// leading whitespace, line endings, comments
					if (consume_leading_whitespace()
						|| consume_line_break()
						|| consume_comment())
						continue;

					// [tables]
					// [[table array]]
					else if (*cp == U'[')
						current_table = parse_table_header();

					// bare_keys
					// dotted.keys
					// "quoted keys"
					else if (is_bare_key_character(*cp) || is_string_delimiter(*cp))
					{
						push_parse_scope("key-value pair"sv);

						parse_key_value_pair_and_insert(current_table);

						// handle the rest of the line after the kvp
						// (this is not done in parse_key_value_pair() because that function is also used for inline tables)
						consume_leading_whitespace();
						return_if_error();
						if (!is_eof() && !consume_comment() && !consume_line_break())
							set_error("expected a comment or whitespace, saw '"sv, *cp, '\'');
					}


					else // ??
						set_error("expected keys, tables, whitespace or comments, saw '"sv, *cp, '\'');

				}
				while (!is_eof());

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

	TOML_EXTERNAL_LINKAGE
	std::unique_ptr<toml::array> parser::parse_array() TOML_MAY_THROW
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'[');
		push_parse_scope("array"sv);

		// skip opening '['
		advance_and_return_if_error_or_eof({});

		auto arr = std::make_unique<array>();
		auto& vals = arr->values;
		enum parse_elem : int
		{
			none,
			comma,
			val
		};
		parse_elem prev = none;

		while (!is_error())
		{
			while (consume_leading_whitespace()
				|| consume_line_break()
				|| consume_comment())
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
					set_error_and_return_default("expected comma or closing ']', saw '"sv, *cp, '\'');
					continue;
				}
				prev = val;
				vals.push_back(parse_value());
			}
		}

		return_if_error({});
		return arr;
	}

	TOML_EXTERNAL_LINKAGE
	std::unique_ptr<toml::table> parser::parse_inline_table() TOML_MAY_THROW
	{
		return_if_error({});
		assert_not_eof();
		assert_or_assume(*cp == U'{');
		push_parse_scope("inline table"sv);

		// skip opening '{'
		advance_and_return_if_error_or_eof({});

		auto tab = std::make_unique<table>();
		tab->inline_ = true;
		enum parse_elem : int
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
					set_error_and_return_default("expected comma or closing '}', saw '"sv, *cp, '\'');
				else
				{
					prev = kvp;
					parse_key_value_pair_and_insert(tab.get());
				}
			}

			/// ???
			else
				set_error_and_return_default("expected key or closing '}', saw '"sv, *cp, '\'');
		}

		return_if_error({});
		return tab;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result do_parse(utf8_reader_interface&& reader) TOML_MAY_THROW
	{
		return impl::parser{ std::move(reader) };
	}

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	#undef push_parse_scope_2
	#undef push_parse_scope_1
	#undef push_parse_scope
	#undef TOML_RETURNS_BY_THROWING
	#undef is_eof
	#undef assert_not_eof
	#undef return_if_eof
	#undef is_error
	#undef return_after_error
	#undef assert_not_error
	#undef return_if_error
	#undef return_if_error_or_eof
	#undef set_error_and_return
	#undef set_error_and_return_default
	#undef set_error_and_return_if_eof
	#undef advance_and_return_if_error
	#undef advance_and_return_if_error_or_eof
}

namespace toml
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(parse_ex)
	#else
		TOML_ABI_NAMESPACE_START(parse_noex)
	#endif

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#ifdef __cpp_lib_char8_t

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string_view source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW
	{
		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	#endif // __cpp_lib_char8_t

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	inline namespace literals
	{
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(lit_ex)
		#else
			TOML_ABI_NAMESPACE_START(lit_noex)
		#endif

		TOML_API
		TOML_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::string_view{ str, len });
		}

		#ifdef __cpp_lib_char8_t

		TOML_API
		TOML_EXTERNAL_LINKAGE
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW
		{
			return parse(std::u8string_view{ str, len });
		}

		#endif // __cpp_lib_char8_t

		TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_PADDING_WARNINGS

//# {{
#endif // !TOML_DOXYGEN
//# }} 
