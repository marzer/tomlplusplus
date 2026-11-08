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
#if TOML_ENABLE_FORMATTERS

#include "formatter.hpp"
#include "print_to_stream.hpp"
#include "value.hpp"
#include "table.hpp"
#include "array.hpp"
#include "unicode.hpp"
#include "parse_result.hpp"
TOML_DISABLE_WARNINGS;
#include <ostream>
#include <streambuf>
TOML_ENABLE_WARNINGS;
#include "header_start.hpp"

TOML_IMPL_NAMESPACE_START
{
	class formatter_buffer
	{
	  private:
		class chunks final : public std::streambuf
		{
		  private:
			std::ostream* target_;
			char chars_[4096];

		  public:
			explicit chunks(std::ostream& target) //
				: target_{ &target }
			{
				setp(chars_, chars_ + sizeof(chars_));
			}

			void flush()
			{
				const auto len = pptr() - pbase();
				if (!len)
					return;
				setp(chars_, chars_ + sizeof(chars_));
				target_->write(chars_, len);
			}

		  protected:
			int_type overflow(int_type ch) override
			{
				flush();
				if (traits_type::eq_int_type(ch, traits_type::eof()))
					return traits_type::not_eof(ch);
				*pptr() = traits_type::to_char_type(ch);
				pbump(1);
				return ch;
			}

			std::streamsize xsputn(const char* str, std::streamsize len) override
			{
				if (len > epptr() - pptr())
				{
					flush();
					if (len > epptr() - pptr())
					{
						target_->write(str, len);
						return len;
					}
				}
				traits_type::copy(pptr(), str, static_cast<size_t>(len));
				pbump(static_cast<int>(len));
				return len;
			}
		};

		formatter& formatter_;
		std::ostream* target_;
		chunks chunks_;
		std::ostream stream_;

	  public:
		explicit formatter_buffer(formatter& f) //
			: formatter_{ f },
			  target_{ f.stream_ },
			  chunks_{ *f.stream_ },
			  stream_{ &chunks_ }
		{
			stream_.exceptions(std::ios::badbit);
			formatter_.stream_ = &stream_;
		}

		~formatter_buffer() noexcept
		{
			formatter_.stream_ = target_;
		}

		void flush()
		{
			chunks_.flush();
		}
	};

	enum class TOML_CLOSED_FLAGS_ENUM formatted_string_traits : unsigned
	{
		none		  = 0,
		line_breaks	  = 1u << 0, // \n
		tabs		  = 1u << 1, // \t
		control_chars = 1u << 2, // also includes the non-ascii code points that are always escaped
		single_quotes = 1u << 3,
		non_bare	  = 1u << 4, // anything not satisfying "is bare key character"
		non_ascii	  = 1u << 5, // any codepoint >= 128

		all = (non_ascii << 1u) - 1u
	};
	TOML_MAKE_FLAGS(formatted_string_traits);
}
TOML_IMPL_NAMESPACE_END;

TOML_ANON_NAMESPACE_START
{
	TOML_INTERNAL_LINKAGE
	char* write_unicode_escape(char* out, char prefix, uint_least32_t value, int digits) noexcept
	{
		*out++ = '\\';
		*out++ = prefix;
		while (digits-- > 0)
			*out++ = "0123456789ABCDEF"[(value >> (digits * 4)) & 0xFu];
		return out;
	}
}
TOML_ANON_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	formatter::formatter(const node* source_node,
						 const parse_result* source_pr,
						 const formatter_constants& constants,
						 const formatter_config& config) noexcept //
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		: source_{ source_pr && *source_pr ? &source_pr->table() : source_node },
		  result_{ source_pr },
#else
		: source_{ source_pr ? source_pr : source_node },
#endif
		  constants_{ &constants },
		  config_{ config }
	{
#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS
		TOML_ASSERT_ASSUME(source_ || result_);
#else
		TOML_ASSERT_ASSUME(source_);
#endif

		config_.flags = (config_.flags | constants_->mandatory_flags) & ~constants_->ignored_flags;

		indent_columns_ = {};
		for (auto c : config_.indent)
			indent_columns_ += c == '\t' ? 4u : 1u;

		int_format_mask_ = config_.flags
						 & (format_flags::allow_binary_integers | format_flags::allow_octal_integers
							| format_flags::allow_hexadecimal_integers);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::attach(std::ostream & stream) noexcept
	{
		indent_		   = {};
		naked_newline_ = true;
		stream_		   = &stream;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::detach() noexcept
	{
		stream_ = nullptr;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_newline(bool force)
	{
		if (!naked_newline_ || force)
		{
			print_to_stream(*stream_, '\n');
			naked_newline_ = true;
		}
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_indent()
	{
		if (indent_ <= 0)
			return;

		char buf[128];
		TOML_ASSERT(config_.indent.length() <= sizeof(buf));
		size_t len = {};
		for (int i = 0; i < indent_; i++)
		{
			if (sizeof(buf) - len < config_.indent.length())
			{
				print_to_stream(*stream_, buf, len);
				len = {};
			}
			std::memcpy(buf + len, config_.indent.data(), config_.indent.length());
			len += config_.indent.length();
		}
		print_to_stream(*stream_, buf, len);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(char c)
	{
		print_to_stream(*stream_, c);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_unformatted(std::string_view str)
	{
		print_to_stream(*stream_, str);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str,
								 bool allow_multi_line,
								 bool allow_bare,
								 bool allow_literal_whitespace)
	{
		print_string(str, allow_multi_line, allow_bare, allow_literal_whitespace, string_dialect::toml);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_string(std::string_view str,
								 bool allow_multi_line,
								 bool allow_bare,
								 bool allow_literal_whitespace,
								 string_dialect dialect)
	{
		if (str.empty())
		{
			print_unformatted(literal_strings_allowed() ? "''"sv : "\"\""sv);
			return;
		}

		// pre-scan the string to determine how we should output it
		formatted_string_traits traits = {};

		if (!allow_bare)
			traits |= formatted_string_traits::non_bare;
		bool unicode_allowed = unicode_strings_allowed();

		// ascii fast path
		if (is_ascii(str.data(), str.length()))
		{
			for (auto c : str)
			{
				switch (c)
				{
					case '\n': traits |= formatted_string_traits::line_breaks; break;
					case '\t': traits |= formatted_string_traits::tabs; break;
					case '\'': traits |= formatted_string_traits::single_quotes; break;
					default:
					{
						if TOML_UNLIKELY(is_control_character(c))
							traits |= formatted_string_traits::control_chars;

						if (!is_ascii_bare_key_character(static_cast<char32_t>(c)))
							traits |= formatted_string_traits::non_bare;
						break;
					}
				}

				static constexpr auto all_ascii_traits =
					formatted_string_traits::all & ~formatted_string_traits::non_ascii;
				if (traits == all_ascii_traits)
					break;
			}
		}

		// unicode slow path
		else
		{
			traits |= formatted_string_traits::non_ascii;
			utf8_decoder decoder;

			// if the unicode is malformed just treat the string as a single-line non-literal and
			// escape all non-ascii characters (to ensure round-tripping and help with diagnostics)
			const auto bad_unicode = [&]() noexcept
			{
				traits &= ~formatted_string_traits::line_breaks;
				traits |= formatted_string_traits::control_chars | formatted_string_traits::non_bare;
				unicode_allowed = false;
			};

			for (auto c : str)
			{
				decoder(c);

				if TOML_UNLIKELY(decoder.error())
				{
					bad_unicode();
					break;
				}

				if (!decoder.has_code_point())
					continue;

				switch (decoder.codepoint)
				{
					case U'\n': traits |= formatted_string_traits::line_breaks; break;
					case U'\t': traits |= formatted_string_traits::tabs; break;
					case U'\'': traits |= formatted_string_traits::single_quotes; break;
					default:
					{
						if TOML_UNLIKELY(is_control_character(decoder.codepoint)
							|| is_non_ascii_unprintable(decoder.codepoint, dialect))
							traits |= formatted_string_traits::control_chars;

						// U+FEFF can't be bare: at the start of a document it reads back as a BOM
						if (!is_bare_key_character(decoder.codepoint)
							|| is_non_ascii_horizontal_whitespace(decoder.codepoint) || decoder.codepoint == U'\uFEFF')
							traits |= formatted_string_traits::non_bare;
						break;
					}
				}
			}

			if (decoder.needs_more_input())
				bad_unicode();
		}

		// strings with line breaks, tabs, single-quotes or anything that gets escaped can't be bare
		if (!!(traits
			   & (formatted_string_traits::line_breaks | formatted_string_traits::tabs
				  | formatted_string_traits::single_quotes | formatted_string_traits::control_chars)))
			traits |= formatted_string_traits::non_bare;

		// if the string meets the requirements of being 'bare' we can emit a bare string
		// (bare strings are composed of letters and numbers; no whitespace, control chars, quotes, etc)
		if (!(traits & formatted_string_traits::non_bare)
			&& (!(traits & formatted_string_traits::non_ascii) || unicode_allowed))
		{
			print_unformatted(str);
			return;
		}
		const auto real_tabs_allowed = allow_literal_whitespace && real_tabs_in_strings_allowed();

		// determine if this should be a multi-line string (triple-quotes)
		const auto multi_line = allow_literal_whitespace	 //
							 && allow_multi_line			 //
							 && multi_line_strings_allowed() //
							 && !!(traits & formatted_string_traits::line_breaks);

		// the parser trims a newline immediately following the opening delimiter of a multi-line string
		const auto leading_newline = multi_line && str[0] == '\n';

		// determine if this should be a literal string (single-quotes with no escaping)
		// (three or more consecutive single-quotes can't appear in a multi-line literal string)
		const auto literal = literal_strings_allowed()										  //
						  && !(traits & formatted_string_traits::control_chars)				  //
						  && (!(traits & formatted_string_traits::single_quotes)			  //
							  || (multi_line && str.find("'''"sv) == std::string_view::npos)) //
						  && (!(traits & formatted_string_traits::tabs) || real_tabs_allowed) //
						  && (!(traits & formatted_string_traits::line_breaks) || multi_line) //
						  && (!(traits & formatted_string_traits::non_ascii) || unicode_allowed);

		// literal strings (single quotes, no escape codes)
		if (literal)
		{
			const auto quot = multi_line ? R"(''')"sv : R"(')"sv;
			print_unformatted(quot);
			if (leading_newline)
				print_unformatted('\n');
			print_unformatted(str);
			print_unformatted(quot);
			return;
		}

		// anything from here down is a non-literal string, so requires iteration and escaping.
		print_unformatted(multi_line ? R"(""")"sv : R"(")"sv);
		if (leading_newline)
			print_unformatted('\n');

		// ascii fast path
		if (!(traits & formatted_string_traits::non_ascii))
		{
			const char* run		  = str.data();
			const char* const end = str.data() + str.length();
			for (auto it = run; it != end; it++)
			{
				std::string_view escape;
				switch (*it)
				{
					case '"': escape = R"(\")"sv; break;
					case '\\': escape = R"(\\)"sv; break;
					case '\x7F': escape = R"(\u007F)"sv; break;
					case '\t':
						if (real_tabs_allowed)
							continue;
						escape = R"(\t)"sv;
						break;
					case '\n':
						if (multi_line)
							continue;
						escape = R"(\n)"sv;
						break;
					default:
						if TOML_LIKELY(*it > '\x1F')
							continue;
						escape = control_char_escapes[static_cast<unsigned char>(*it)];
				}
				if (run != it)
					print_to_stream(*stream_, run, static_cast<size_t>(it - run));
				print_to_stream(*stream_, escape);
				run = it + 1;
			}
			if (run != end)
				print_to_stream(*stream_, run, static_cast<size_t>(end - run));
		}

		// unicode slow path
		else
		{
			utf8_decoder decoder;
			const char* run		 = str.data();
			const char* cp_start = str.data();
			const char* cp_end	 = cp_start;
			const auto print_run = [&]()
			{
				if (run != cp_start)
					print_to_stream(*stream_, run, static_cast<size_t>(cp_start - run));
				run = cp_end;
			};
			const auto escape_raw_bytes = [&]()
			{
				print_run();
				for (; cp_start != cp_end; cp_start++)
				{
					char escape[6];
					TOML_ANON_NAMESPACE::write_unicode_escape(escape, 'u', static_cast<uint8_t>(*cp_start), 4);
					print_to_stream(*stream_, escape, sizeof(escape));
				}
			};
			for (auto c : str)
			{
				decoder(c);
				cp_end++;

				// if the decoder encounters malformed unicode just emit raw bytes and
				if (decoder.error())
				{
					escape_raw_bytes();
					decoder.reset();
					continue;
				}

				if (!decoder.has_code_point())
					continue;

				std::string_view escape;
				char unicode_escape[12];
				switch (decoder.codepoint)
				{
					case U'"': escape = R"(\")"sv; break;
					case U'\\': escape = R"(\\)"sv; break;
					case U'\x7F': escape = R"(\u007F)"sv; break;
					case U'\t':
						if (!real_tabs_allowed)
							escape = R"(\t)"sv;
						break;
					case U'\n':
						if (!multi_line)
							escape = R"(\n)"sv;
						break;
					default:
					{
						const auto cp = static_cast<uint_least32_t>(decoder.codepoint);

						// control characters from lookup table
						if TOML_UNLIKELY(cp <= 0x1Fu)
							escape = control_char_escapes[cp];

						// escaped unicode characters
						else if (cp > 0x7Fu
								 && (!unicode_allowed || is_non_ascii_unprintable(decoder.codepoint, dialect)))
						{
							using TOML_ANON_NAMESPACE::write_unicode_escape;
							char* escape_end = unicode_escape;
							if (cp > 0xFFFFu && dialect == string_dialect::json)
							{
								const auto offset = cp - 0x10000u;
								escape_end		  = write_unicode_escape(escape_end, 'u', 0xD800u + (offset >> 10), 4);
								escape_end = write_unicode_escape(escape_end, 'u', 0xDC00u + (offset & 0x3FFu), 4);
							}
							else if (cp > 0xFFFFu)
								escape_end = write_unicode_escape(escape_end, 'U', cp, 8);
							else
								escape_end = write_unicode_escape(escape_end, 'u', cp, 4);
							escape = { unicode_escape, static_cast<size_t>(escape_end - unicode_escape) };
						}
					}
				}

				if (!escape.empty())
				{
					print_run();
					print_to_stream(*stream_, escape);
				}
				cp_start = cp_end;
			}
			escape_raw_bytes();
		}

		print_unformatted(multi_line ? R"(""")"sv : R"(")"sv);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<std::string>& val)
	{
		print_string(val.get());
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<int64_t>& val)
	{
		naked_newline_ = false;

		if (*val >= 0 && !!int_format_mask_)
		{
			static constexpr auto value_flags_mask =
				value_flags::format_as_binary | value_flags::format_as_octal | value_flags::format_as_hexadecimal;

			const auto fmt = val.flags() & value_flags_mask;
			switch (fmt)
			{
				case value_flags::format_as_binary:
					if (!!(int_format_mask_ & format_flags::allow_binary_integers))
					{
						print_to_stream(*stream_, "0b"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_octal:
					if (!!(int_format_mask_ & format_flags::allow_octal_integers))
					{
						print_to_stream(*stream_, "0o"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				case value_flags::format_as_hexadecimal:
					if (!!(int_format_mask_ & format_flags::allow_hexadecimal_integers))
					{
						print_to_stream(*stream_, "0x"sv);
						print_to_stream(*stream_, *val, fmt);
						return;
					}
					break;

				default: break;
			}
		}

		// fallback to decimal
		print_to_stream(*stream_, *val);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<double>& val)
	{
		const std::string_view* inf_nan = nullptr;
		switch (fpclassify(*val))
		{
			case fp_class::neg_inf: inf_nan = &constants_->float_neg_inf; break;
			case fp_class::pos_inf: inf_nan = &constants_->float_pos_inf; break;
			case fp_class::nan: inf_nan = &constants_->float_nan; break;
			case fp_class::ok: print_to_stream(*stream_, *val, value_flags::none, relaxed_float_precision()); break;
			default: TOML_UNREACHABLE;
		}

		if (inf_nan)
		{
			if (!!(config_.flags & format_flags::quote_infinities_and_nans))
				print_to_stream_bookended(*stream_, *inf_nan, '"');
			else
				print_to_stream(*stream_, *inf_nan);
		}

		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<bool>& val)
	{
		print_unformatted(*val ? constants_->bool_true : constants_->bool_false);
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print(const value<date_time>& val)
	{
		if (!!(config_.flags & format_flags::quote_dates_and_times))
			print_to_stream_bookended(*stream_, *val, literal_strings_allowed() ? '\'' : '"');
		else
			print_to_stream(*stream_, *val);
		naked_newline_ = false;
	}

	TOML_EXTERNAL_LINKAGE
	void formatter::print_value(const node& val_node, node_type type)
	{
		TOML_ASSUME(type > node_type::array);
		switch (type)
		{
			case node_type::string: print(*static_cast<const value<std::string>*>(&val_node)); break;
			case node_type::integer: print(*static_cast<const value<int64_t>*>(&val_node)); break;
			case node_type::floating_point: print(*static_cast<const value<double>*>(&val_node)); break;
			case node_type::boolean: print(*static_cast<const value<bool>*>(&val_node)); break;
			case node_type::date: print(*static_cast<const value<date>*>(&val_node)); break;
			case node_type::time: print(*static_cast<const value<time>*>(&val_node)); break;
			case node_type::date_time: print(*static_cast<const value<date_time>*>(&val_node)); break;
			default: TOML_UNREACHABLE;
		}
	}

#if TOML_ENABLE_PARSER && !TOML_EXCEPTIONS

	TOML_EXTERNAL_LINKAGE
	bool formatter::dump_failed_parse_result()
	{
		if (result_ && !(*result_))
		{
			stream() << result_->error();
			return true;
		}
		return false;
	}

#else

	TOML_EXTERNAL_LINKAGE
	TOML_ATTR(const)
	bool formatter::dump_failed_parse_result()
	{
		return false;
	}

#endif
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_FORMATTERS
