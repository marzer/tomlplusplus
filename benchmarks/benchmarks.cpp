// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include <toml++/toml.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

namespace
{
	constexpr std::string_view words[] = { "apple",	  "brave",	"cloud",  "delta",	"ember",   "fable",	 "grove",
										   "harbour", "island", "jungle", "kettle", "lantern", "meadow", "nectar",
										   "orbit",	  "pebble", "quartz", "river",	"saddle",  "timber", "umber",
										   "valley",  "willow", "xenon",  "yonder", "zephyr" };

	constexpr char32_t non_ascii_ranges[][2] = { { 0x00C0, 0x00D6 },  { 0x00D8, 0x00F6 }, { 0x0391, 0x03A1 },
												 { 0x03B1, 0x03C9 },  { 0x0410, 0x044F }, { 0x05D0, 0x05EA },
												 { 0x3041, 0x3096 },  { 0x4E00, 0x9FFF }, { 0xAC00, 0xD7A3 },
												 { 0x1F600, 0x1F64F } };

	enum class content
	{
		mixed,
		strings,
		numbers
	};

	class generator
	{
		std::string out_;
		uint64_t state_;
		size_t counter_ = {};
		bool unicode_	= {};

		uint64_t next() noexcept
		{
			auto z = (state_ += 0x9E3779B97F4A7C15u);
			z	   = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9u;
			z	   = (z ^ (z >> 27)) * 0x94D049BB133111EBu;
			return z ^ (z >> 31);
		}

		size_t below(size_t n) noexcept
		{
			return next() % n;
		}

		bool one_in(size_t n) noexcept
		{
			return below(n) == 0u;
		}

		void append_utf8(char32_t cp)
		{
			if (cp < 0x800u)
			{
				out_ += static_cast<char>(0xC0u | (cp >> 6));
				out_ += static_cast<char>(0x80u | (cp & 0x3Fu));
				return;
			}
			if (cp < 0x10000u)
			{
				out_ += static_cast<char>(0xE0u | (cp >> 12));
				out_ += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
				out_ += static_cast<char>(0x80u | (cp & 0x3Fu));
				return;
			}
			out_ += static_cast<char>(0xF0u | (cp >> 18));
			out_ += static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu));
			out_ += static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu));
			out_ += static_cast<char>(0x80u | (cp & 0x3Fu));
		}

		void word()
		{
			if (!unicode_ || one_in(2u))
			{
				out_ += words[below(std::size(words))];
				return;
			}
			const auto& range  = non_ascii_ranges[below(std::size(non_ascii_ranges))];
			const size_t first = range[0];
			const size_t last  = range[1];
			for (auto i = 2u + below(5u); i > 0u; i--)
				append_utf8(static_cast<char32_t>(first + below(last - first + 1u)));
		}

		void sentence(size_t count)
		{
			for (size_t i = 0; i < count; i++)
			{
				if (i)
					out_ += ' ';
				word();
			}
		}

		void comment()
		{
			out_ += "# "sv;
			sentence(2u + below(10u));
		}

		void key()
		{
			counter_++;
			if (one_in(10u))
			{
				out_ += '"';
				word();
				out_ += ' ';
				out_ += std::to_string(counter_);
				out_ += '"';
				return;
			}
			out_ += words[below(std::size(words))];
			out_ += '-';
			out_ += std::to_string(counter_);
		}

		std::string new_key()
		{
			std::string text;
			std::swap(text, out_);
			key();
			std::swap(text, out_);
			return text;
		}

		void digits(uint64_t value, uint64_t base)
		{
			char buffer[64] = {};
			auto pos		= std::size(buffer);
			do
			{
				buffer[--pos] = "0123456789abcdef"[value % base];
				value /= base;
			}
			while (value);
			out_.append(buffer + pos, std::size(buffer) - pos);
		}

		void random_digits(size_t count)
		{
			for (; count > 0u; count--)
				out_ += static_cast<char>('0' + below(10u));
		}

		void padded(uint64_t value, size_t width)
		{
			const auto text = std::to_string(value);
			if (text.size() < width)
				out_.append(width - text.size(), '0');
			out_ += text;
		}

		void integer()
		{
			const auto bits	 = next();
			const auto value = bits >> (1u + below(62u));
			switch (below(10u))
			{
				case 0:
					out_ += "0x"sv;
					digits(value, 16u);
					return;

				case 1:
					out_ += "0o"sv;
					digits(value, 8u);
					return;

				case 2:
					out_ += "0b"sv;
					digits(value >> 32, 2u);
					return;

				case 3:
					out_ += '-';
					digits(value, 10u);
					return;

				case 4:
				{
					const auto text = std::to_string(value);
					for (size_t i = 0; i < text.size(); i++)
					{
						if (i && (text.size() - i) % 3u == 0u)
							out_ += '_';
						out_ += text[i];
					}
					return;
				}

				default: digits(value, 10u); return;
			}
		}

		void floating_point()
		{
			constexpr std::string_view specials[] = { "inf"sv, "-inf"sv, "+inf"sv, "nan"sv, "-nan"sv, "+nan"sv };
			if (one_in(50u))
			{
				out_ += specials[below(std::size(specials))];
				return;
			}

			if (one_in(4u))
				out_ += '-';
			digits(below(1000000u), 10u);

			const auto fraction = !one_in(5u);
			if (fraction)
			{
				out_ += '.';
				random_digits(1u + below(8u));
			}

			if (fraction && !one_in(4u))
				return;
			out_ += one_in(2u) ? 'e' : 'E';
			if (one_in(3u))
				out_ += one_in(2u) ? '-' : '+';
			digits(below(300u), 10u);
		}

		void date()
		{
			padded(1900u + below(200u), 4u);
			out_ += '-';
			padded(1u + below(12u), 2u);
			out_ += '-';
			padded(1u + below(28u), 2u);
		}

		void time()
		{
			padded(below(24u), 2u);
			out_ += ':';
			padded(below(60u), 2u);
			out_ += ':';
			padded(below(60u), 2u);
			if (!one_in(3u))
				return;
			out_ += '.';
			random_digits(1u + below(9u));
		}

		void date_time()
		{
			date();
			out_ += one_in(4u) ? ' ' : 'T';
			time();
			switch (below(4u))
			{
				case 0: return;

				case 1: out_ += 'Z'; return;

				default:
					out_ += one_in(2u) ? '+' : '-';
					padded(below(24u), 2u);
					out_ += ':';
					padded(below(60u), 2u);
					return;
			}
		}

		void basic_string(size_t length)
		{
			constexpr std::string_view escapes[] = {
				"\\n"sv, "\\t"sv, "\\\""sv, "\\\\"sv, "\\u00E9"sv, "\\U0001F600"sv
			};
			out_ += '"';
			for (size_t i = 0; i < length; i++)
			{
				if (i)
					out_ += ' ';
				if (one_in(8u))
					out_ += escapes[below(std::size(escapes))];
				else
					word();
			}
			out_ += '"';
		}

		void literal_string(size_t length)
		{
			out_ += '\'';
			sentence(length);
			out_ += '\'';
		}

		void multi_line_string(size_t lines, bool literal)
		{
			const auto delimiter = literal ? "'''"sv : R"(""")"sv;
			out_ += delimiter;
			out_ += '\n';
			for (; lines > 0u; lines--)
			{
				sentence(2u + below(10u));
				out_ += '\n';
			}
			out_ += delimiter;
		}

		void number_array()
		{
			const auto floats = one_in(2u);
			out_ += '[';
			for (auto i = 4u + below(29u); i > 0u; i--)
			{
				if (floats)
					floating_point();
				else
					integer();
				if (i > 1u)
					out_ += ", "sv;
			}
			out_ += ']';
		}

		void array(size_t depth)
		{
			const auto count	  = below(8u);
			const auto multi_line = !depth && count > 4u && one_in(2u);
			out_ += '[';
			for (size_t i = 0; i < count; i++)
			{
				if (multi_line)
					out_ += "\n\t"sv;
				else if (i)
					out_ += ' ';
				value(content::mixed, depth + 1u);
				if (multi_line || i + 1u < count)
					out_ += ',';
			}
			if (multi_line)
				out_ += '\n';
			out_ += ']';
		}

		void inline_table(size_t depth)
		{
			const auto count = below(5u);
			if (!count)
			{
				out_ += "{}"sv;
				return;
			}
			out_ += "{ "sv;
			for (size_t i = 0; i < count; i++)
			{
				if (i)
					out_ += ", "sv;
				key();
				out_ += " = "sv;
				value(content::mixed, depth + 1u);
			}
			out_ += " }"sv;
		}

		void value(content kind, size_t depth)
		{
			if (kind == content::strings)
			{
				switch (below(4u))
				{
					case 0: basic_string(4u + below(36u)); return;
					case 1: literal_string(4u + below(36u)); return;
					case 2: multi_line_string(2u + below(8u), false); return;
					default: multi_line_string(2u + below(8u), true); return;
				}
			}

			if (kind == content::numbers)
			{
				switch (below(10u))
				{
					case 0:
					case 1:
					case 2: integer(); return;
					case 3:
					case 4:
					case 5: floating_point(); return;
					case 6: date(); return;
					case 7: date_time(); return;
					default: number_array(); return;
				}
			}

			switch (below(depth < 3u ? 20u : 16u))
			{
				case 0:
				case 1:
				case 2: basic_string(1u + below(10u)); return;
				case 3:
				case 4: literal_string(1u + below(10u)); return;
				case 5:
				case 6:
				case 7: integer(); return;
				case 8:
				case 9: floating_point(); return;
				case 10: out_ += one_in(2u) ? "true"sv : "false"sv; return;
				case 11: date(); return;
				case 12: time(); return;
				case 13:
				case 14: date_time(); return;
				case 15:
				{
					if (depth)
					{
						basic_string(1u + below(10u));
						return;
					}
					const auto lines = 2u + below(6u);
					multi_line_string(lines, one_in(2u));
					return;
				}
				case 16:
				case 17:
				case 18: array(depth); return;
				default: inline_table(depth); return;
			}
		}

		void key_value(content kind, std::string_view indent)
		{
			out_ += indent;
			key();
			if (one_in(8u))
			{
				out_ += '.';
				key();
			}
			out_ += " = "sv;
			value(kind, 0u);
			if (one_in(6u))
			{
				out_ += ' ';
				comment();
			}
			out_ += '\n';
		}

		void key_values(content kind, size_t min, size_t max, std::string_view indent)
		{
			for (auto i = min + below(max - min + 1u); i > 0u; i--)
				key_value(kind, indent);
		}

	  public:
		generator(uint64_t seed, bool unicode) noexcept //
			: state_{ seed },
			  unicode_{ unicode }
		{}

		std::string sections(size_t size, content kind)
		{
			while (out_.size() < size)
			{
				if (one_in(4u))
				{
					comment();
					out_ += '\n';
				}

				const auto name = new_key();
				out_ += '[';
				out_ += name;
				out_ += "]\n"sv;
				key_values(kind, 4u, 16u, {});

				for (auto i = below(3u); i > 0u; i--)
				{
					out_ += "\n["sv;
					out_ += name;
					out_ += '.';
					key();
					out_ += "]\n"sv;
					key_values(kind, 2u, 8u, "\t"sv);
				}

				if (one_in(3u))
				{
					std::string header{ "\n[["sv };
					header += name;
					header += '.';
					header += new_key();
					header += "]]\n"sv;
					for (auto i = 2u + below(5u); i > 0u; i--)
					{
						out_ += header;
						key_values(kind, 1u, 6u, "\t"sv);
					}
				}

				out_ += '\n';
			}
			return std::move(out_);
		}

		std::string nested(size_t size)
		{
			while (out_.size() < size)
			{
				out_ += '[';
				key();
				out_ += "]\n"sv;
				for (auto i = 8u + below(16u); i > 0u; i--)
				{
					key();
					switch (below(4u))
					{
						case 0:
						{
							const auto depth = 1u + below(100u);
							out_ += " = "sv;
							out_.append(depth, '[');
							integer();
							out_.append(depth, ']');
							break;
						}

						case 1:
						{
							const auto depth = 1u + below(100u);
							out_ += " = "sv;
							for (auto d = depth; d > 0u; d--)
							{
								out_ += "{ "sv;
								key();
								out_ += " = "sv;
							}
							integer();
							for (auto d = depth; d > 0u; d--)
								out_ += " }"sv;
							break;
						}

						case 2:
						{
							const auto depth = 1u + below(50u);
							out_ += " = "sv;
							for (auto d = depth; d > 0u; d--)
							{
								out_ += "[{ "sv;
								key();
								out_ += " = "sv;
							}
							integer();
							for (auto d = depth; d > 0u; d--)
								out_ += " }]"sv;
							break;
						}

						default:
							for (auto d = 1u + below(40u); d > 0u; d--)
							{
								out_ += '.';
								key();
							}
							out_ += " = "sv;
							integer();
							break;
					}
					out_ += '\n';
				}
				out_ += '\n';
			}
			return std::move(out_);
		}

		std::string tables(size_t size)
		{
			while (out_.size() < size)
			{
				out_ += '[';
				key();
				for (auto i = below(4u); i > 0u; i--)
				{
					out_ += '.';
					key();
				}
				out_ += "]\n"sv;
				key_values(content::mixed, 1u, 3u, {});

				if (one_in(4u))
				{
					std::string header{ "[["sv };
					header += new_key();
					header += "]]\n"sv;
					for (auto i = 4u + below(16u); i > 0u; i--)
					{
						out_ += header;
						key_values(content::mixed, 1u, 3u, {});
					}
				}

				out_ += '\n';
			}
			return std::move(out_);
		}
	};

	struct document
	{
		std::string_view name;
		std::string_view text;
	};

	class string_buffer : public std::streambuf
	{
	  public:
		explicit string_buffer(std::string& text) noexcept
		{
			setg(text.data(), text.data(), text.data() + text.size());
		}
	};

	std::string read_file(const char* path)
	{
		const std::ifstream file{ path, std::ios::binary };
		if (!file)
			return {};
		std::ostringstream text;
		text << file.rdbuf();
		return text.str();
	}

	bool write_file(const std::filesystem::path& path, std::string_view text)
	{
		std::ofstream file{ path, std::ios::binary };
		file.write(text.data(), static_cast<std::streamsize>(text.size()));
		file.close();
		return !file.fail();
	}

	template <typename Parse>
	std::optional<toml::table> parse_table(std::string_view name, Parse&& parse)
	{
#if TOML_EXCEPTIONS
		try
		{
			return parse();
		}
		catch (const toml::parse_error& err)
		{
			std::cerr << name << ": " << err << "\n";
			return {};
		}
#else
		auto result = parse();
		if (result)
			return std::move(result).table();
		std::cerr << name << ": " << result.error() << "\n";
		return {};
#endif
	}

	template <typename Func>
	double best_seconds(Func&& func)
	{
		using clock = std::chrono::steady_clock;

		size_t runs = 1;
		for (;; runs *= 2u)
		{
			const auto start = clock::now();
			for (size_t i = 0; i < runs; i++)
				static_cast<void>(func());
			if (std::chrono::duration<double>(clock::now() - start).count() >= 0.1)
				break;
		}

		auto best = std::numeric_limits<double>::infinity();
		for (int i = 0; i < 5; i++)
		{
			const auto start = clock::now();
			for (size_t r = 0; r < runs; r++)
				static_cast<void>(func());
			best =
				std::min(best, std::chrono::duration<double>(clock::now() - start).count() / static_cast<double>(runs));
		}
		return best;
	}

	void report(std::string_view name, size_t bytes, double seconds)
	{
		const auto megabytes = static_cast<double>(bytes) / 1e6;
		std::cout << std::left << std::setw(24) << name << std::right << std::fixed << std::setprecision(2)
				  << std::setw(9) << megabytes << " MB" << std::setprecision(1) << std::setw(10) << megabytes / seconds
				  << " MB/s\n"
				  << std::flush;
	}

	template <typename Parse>
	bool time_parse(std::string_view name, size_t bytes, Parse&& parse)
	{
		if (!parse_table(name, parse))
			return false;
		report(name, bytes, best_seconds(parse));
		return true;
	}

	void print_configuration()
	{
		std::cout << "toml++ " << TOML_LIB_MAJOR << '.' << TOML_LIB_MINOR << '.' << TOML_LIB_PATCH;
#if TOML_HEADER_ONLY
		std::cout << ", header-only";
#else
		std::cout << ", compiled library";
#endif
#if TOML_EXCEPTIONS
		std::cout << ", exceptions";
#else
		std::cout << ", no exceptions";
#endif
#if TOML_ENABLE_UNRELEASED_FEATURES
		std::cout << ", unreleased features";
#endif
#if defined(__clang__)
		std::cout << ", clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif defined(__GNUC__)
		std::cout << ", gcc " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_FULL_VER)
		std::cout << ", msvc " << _MSC_FULL_VER;
#endif
		std::cout << ", __cplusplus " << __cplusplus;
#ifndef NDEBUG
		std::cout << ", assertions on";
#endif
		std::cout << "\n\n";
	}
}

int main(int argc, char** argv)
{
	std::vector<std::string_view> filters;
	for (int i = 1; i < argc; i++)
		filters.emplace_back(argv[i]);
	const auto selected = [&](std::string_view name) noexcept
	{
		if (filters.empty())
			return true;
		for (auto filter : filters)
			if (name.find(filter) != std::string_view::npos)
				return true;
		return false;
	};

	print_configuration();

	const auto bundled = read_file(TOMLPLUSPLUS_BENCHMARK_DATA);
	const auto mixed   = generator{ 1u, false }.sections(8000000u, content::mixed);
	const auto strings = generator{ 2u, false }.sections(4000000u, content::strings);
	const auto numbers = generator{ 3u, false }.sections(4000000u, content::numbers);
	const auto unicode = generator{ 4u, true }.sections(4000000u, content::mixed);
	const auto nested  = generator{ 5u, false }.nested(2000000u);
	const auto tables  = generator{ 6u, false }.tables(4000000u);

	const document documents[] = { { "parse/bundled"sv, bundled }, { "parse/mixed"sv, mixed },
								   { "parse/strings"sv, strings }, { "parse/numbers"sv, numbers },
								   { "parse/unicode"sv, unicode }, { "parse/nested"sv, nested },
								   { "parse/tables"sv, tables } };

	for (const auto& doc : documents)
	{
		if (doc.text.empty() || !selected(doc.name))
			continue;
		if (!time_parse(doc.name, doc.text.size(), [&] { return toml::parse(doc.text); }))
			return 1;
	}

	if (selected("parse_stream/mixed"sv))
	{
		auto text		 = mixed;
		const auto parse = [&]
		{
			string_buffer buffer{ text };
			std::istream stream{ &buffer };
			return toml::parse(stream);
		};
		if (!time_parse("parse_stream/mixed"sv, text.size(), parse))
			return 1;
	}

	if (!bundled.empty() && selected("parse_file/bundled"sv))
	{
		if (!time_parse("parse_file/bundled"sv,
						bundled.size(),
						[] { return toml::parse_file(TOMLPLUSPLUS_BENCHMARK_DATA); }))
			return 1;
	}

	if (selected("parse_file/mixed"sv))
	{
		std::error_code ec;
		const auto path = std::filesystem::temp_directory_path(ec) / "tomlplusplus_benchmark.toml";
		if (ec || !write_file(path, mixed))
		{
			std::cerr << "parse_file/mixed: could not write a temporary file\n";
			return 1;
		}
#if TOML_ENABLE_WINDOWS_COMPAT
		const auto native_path = path.wstring();
#else
		const auto native_path = path.string();
#endif
		const auto timed =
			time_parse("parse_file/mixed"sv, mixed.size(), [&] { return toml::parse_file(native_path); });
		std::filesystem::remove(path, ec);
		if (!timed)
			return 1;
	}

#if TOML_ENABLE_FORMATTERS
	const auto format = [&](std::string_view name, auto&& print)
	{
		if (!selected(name))
			return;
		std::ostringstream probe;
		print(probe);
		report(name,
			   probe.str().size(),
			   best_seconds(
				   [&]
				   {
					   std::ostringstream out;
					   print(out);
				   }));
	};
	const auto table = parse_table("mixed"sv, [&] { return toml::parse(mixed); });
	if (!table)
		return 1;
	format("toml_formatter/mixed"sv, [&](std::ostream& out) { out << toml::toml_formatter{ *table }; });
	format("json_formatter/mixed"sv, [&](std::ostream& out) { out << toml::json_formatter{ *table }; });
	format("yaml_formatter/mixed"sv, [&](std::ostream& out) { out << toml::yaml_formatter{ *table }; });
#endif

	return 0;
}
