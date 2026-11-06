// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include <filesystem>
#include <fstream>
#include <optional>
#include <random>
#include <stdexcept>

namespace
{
	struct unseekable_buf : std::streambuf
	{
		std::string data;
		size_t pos					 = {};
		size_t fail_at				 = std::string::npos;
		bool fail_with_std_exception = true;

		explicit unseekable_buf(std::string d,
								size_t fail_at_pos			  = std::string::npos,
								bool std_exception_on_failure = true)
			: data{ std::move(d) },
			  fail_at{ fail_at_pos },
			  fail_with_std_exception{ std_exception_on_failure }
		{}

	  protected:
		int_type underflow() override
		{
#if TOML_EXCEPTIONS
			if (pos == fail_at)
			{
				if (fail_with_std_exception)
					throw std::runtime_error{ "the pipe broke" };
				throw 42;
			}
#endif
			if (pos >= data.size())
				return traits_type::eof();

			setg(&data[pos], &data[pos], &data[pos] + 1);
			pos++;
			return traits_type::to_int_type(*gptr());
		}
	};

	// never has a get area, like std::cin synced with stdio
	struct unbuffered_buf : std::streambuf
	{
		std::string data;
		size_t pos	   = {};
		size_t fail_at = std::string::npos;

		explicit unbuffered_buf(std::string d, size_t fail_at_pos = std::string::npos)
			: data{ std::move(d) },
			  fail_at{ fail_at_pos }
		{}

	  protected:
		int_type underflow() override
		{
#if TOML_EXCEPTIONS
			if (pos == fail_at)
				throw std::runtime_error{ "the pipe broke" };
#endif
			if (pos >= data.size())
				return traits_type::eof();
			return traits_type::to_int_type(data[pos]);
		}

		int_type uflow() override
		{
			const auto c = underflow();
			if (!traits_type::eq_int_type(c, traits_type::eof()))
				pos++;
			return c;
		}
	};

	template <typename Buf>
	parse_result parse_through(std::string doc)
	{
		Buf buf{ std::move(doc) };
		std::istream stream{ &buf };
		return toml::parse(stream);
	}

	class temp_file
	{
		std::filesystem::path path_;

	  public:
		explicit temp_file(std::string_view contents)
			: path_{ std::filesystem::temp_directory_path()
					 / ("tomlplusplus_test_" + std::to_string(std::random_device{}()) + ".toml") }
		{
			std::ofstream file{ path_, std::ios::binary };
			file.write(contents.data(), static_cast<std::streamsize>(contents.length()));
		}

		~temp_file()
		{
			std::error_code ec;
			std::filesystem::remove(path_, ec);
		}

		temp_file(const temp_file&)			   = delete;
		temp_file& operator=(const temp_file&) = delete;

		std::string str() const
		{
			return path_.string();
		}
	};

	template <typename Func>
	std::optional<parse_error> parse_failure(Func&& parse)
	{
#if TOML_EXCEPTIONS
		try
		{
			static_cast<void>(parse());
		}
		catch (const parse_error& err)
		{
			return err;
		}
		return {};
#else
		auto result = parse();
		if (result)
			return {};
		return std::move(result).error();
#endif
	}

#if TOML_EXCEPTIONS

	// in_avail() calls showmanyc() once the first fail_at bytes are used up
	struct showmanyc_buf : std::streambuf
	{
		std::string data;

		showmanyc_buf(std::string d, size_t fail_at) : data{ std::move(d) }
		{
			setg(data.data(), data.data(), data.data() + fail_at);
		}

	  protected:
		std::streamsize showmanyc() override
		{
			if (gptr() == egptr())
				throw std::runtime_error{ "the pipe broke" };
			return egptr() - gptr();
		}
	};

	template <typename Buf>
	void check_failure(
		const std::string& doc,
		size_t fail_at,
		source_position expected,
		std::string_view description		  = "An I/O error occurred while reading from the underlying stream"sv,
		std::string_view rethrown_description = "the pipe broke"sv)
	{
		INFO("failing at byte " << fail_at);

		Buf buf{ doc, fail_at };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == description);
		CHECK(err->source().begin == expected);

		Buf rethrowing_buf{ doc, fail_at };
		std::istream rethrowing_stream{ &rethrowing_buf };
		rethrowing_stream.exceptions(std::ios::badbit);
		const auto rethrown = parse_failure([&]() { return toml::parse(rethrowing_stream); });
		REQUIRE(rethrown);
		CHECK(rethrown->description() == rethrown_description);
		CHECK(rethrown->source().begin == expected);
	}

#endif
}

TEST_CASE("parsing - streams that can't seek")
{
	SECTION("a document")
	{
		unseekable_buf buf{ "a = 1\nb = 'two'\n" };
		std::istream stream{ &buf };
		auto tbl = toml::parse(stream);
		CHECK(tbl["a"] == 1);
		CHECK(tbl["b"] == "two"sv);
	}

	SECTION("a document as short as a byte order mark")
	{
		unseekable_buf buf{ "a=1" };
		std::istream stream{ &buf };
		auto tbl = toml::parse(stream);
		CHECK(tbl["a"] == 1);
	}

	SECTION("a document shorter than a byte order mark")
	{
		unseekable_buf buf{ "a" };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Error while parsing key-value pair: encountered end-of-file"sv);
		CHECK(err->source().begin == source_position{ 1, 2 });
	}

	SECTION("a byte order mark")
	{
		unseekable_buf buf{ BOM_PREFIX "a = 1" };
		std::istream stream{ &buf };
		auto tbl = toml::parse(stream);
		CHECK(tbl["a"] == 1);
	}

	SECTION("only a byte order mark")
	{
		unseekable_buf buf{ BOM_PREFIX };
		std::istream stream{ &buf };
		auto tbl = toml::parse(stream);
		CHECK(tbl.begin() == tbl.end());
	}

	SECTION("multi-byte code points across reads")
	{
		unseekable_buf buf{ std::string(122, ' ') + "a = '\xE2\x82\xAC\xF0\x9F\x98\x82'" };
		std::istream stream{ &buf };
		auto tbl = toml::parse(stream);
		CHECK(tbl["a"] == "\xE2\x82\xAC\xF0\x9F\x98\x82"sv);
	}
}

TEST_CASE("parsing - unbuffered streams")
{
	SECTION("a document")
	{
		auto tbl = parse_through<unbuffered_buf>("a = 1\nb = 'two'\n");
		CHECK(tbl["a"] == 1);
		CHECK(tbl["b"] == "two"sv);
	}

	SECTION("a byte order mark")
	{
		auto tbl = parse_through<unbuffered_buf>(BOM_PREFIX "a = 1");
		CHECK(tbl["a"] == 1);
	}

	SECTION("a document shorter than a byte order mark")
	{
		const auto err = parse_failure([&]() { return parse_through<unbuffered_buf>("a"); });
		REQUIRE(err);
		CHECK(err->description() == "Error while parsing key-value pair: encountered end-of-file"sv);
		CHECK(err->source().begin == source_position{ 1, 2 });
	}

	SECTION("multi-byte code points across reads")
	{
		std::string value;
		for (size_t i = 0; i < 200u; i++)
			value += "\xE2\x82\xAC\xF0\x9F\x98\x82";
		auto tbl = parse_through<unbuffered_buf>("a = '" + value + "'");
		CHECK(tbl["a"] == std::string_view{ value });
		REQUIRE(tbl["a"].node());
		CHECK(tbl["a"].node()->source().begin == source_position{ 1, 5 });
		CHECK(tbl["a"].node()->source().end == source_position{ 1, 407 });
	}
}

#if TOML_EXCEPTIONS

TEST_CASE("parsing - streams that fail while reading")
{
	SECTION("the stream sets badbit")
	{
		unseekable_buf buf{ "a = 1\nb = 2\n", 3u };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream, "pipe.toml"sv); });
		REQUIRE(err);
		CHECK(err->description() == "An I/O error occurred while reading from the underlying stream"sv);
		CHECK(err->source().begin == source_position{ 1, 4 });
		REQUIRE(err->source().path);
		CHECK(*err->source().path == "pipe.toml"sv);
	}

	SECTION("the stream rethrows")
	{
		unseekable_buf buf{ "a = 1\nb = 2\n", 3u };
		std::istream stream{ &buf };
		stream.exceptions(std::ios::badbit);
		const auto err = parse_failure([&]() { return toml::parse(stream, "pipe.toml"sv); });
		REQUIRE(err);
		CHECK(err->description() == "the pipe broke"sv);
		CHECK(err->source().begin == source_position{ 1, 4 });
		REQUIRE(err->source().path);
		CHECK(*err->source().path == "pipe.toml"sv);
	}

	SECTION("the stream rethrows something that isn't a std::exception")
	{
		unseekable_buf buf{ "a = 1\nb = 2\n", 3u, false };
		std::istream stream{ &buf };
		stream.exceptions(std::ios::badbit);
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "An unspecified error occurred"sv);
		CHECK(err->source().begin == source_position{ 1, 4 });
	}

	SECTION("the stream fails on its first read")
	{
		unseekable_buf buf{ "a = 1\nb = 2\n", 0u };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Reading from the underlying stream failed - zero bytes read"sv);
		CHECK(err->source().begin == source_position{ 1, 1 });
	}

	SECTION("the stream fails straight after a byte order mark")
	{
		unseekable_buf buf{ BOM_PREFIX "a = 1\n", 3u };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Reading from the underlying stream failed - zero bytes read"sv);
		CHECK(err->source().begin == source_position{ 1, 1 });
	}

	SECTION("the stream rethrows on its first read")
	{
		unseekable_buf buf{ "a = 1\nb = 2\n", 0u };
		std::istream stream{ &buf };
		stream.exceptions(std::ios::badbit);
		const auto err = parse_failure([&]() { return toml::parse(stream, "pipe.toml"sv); });
		REQUIRE(err);
		CHECK(err->description() == "the pipe broke"sv);
		CHECK(err->source().begin == source_position{ 1, 1 });
		REQUIRE(err->source().path);
		CHECK(*err->source().path == "pipe.toml"sv);
	}

	SECTION("the stream fails right after a whole block")
	{
		const auto doc = "#" + std::string(40u, 'x');

		unseekable_buf buf{ doc, 32u };
		std::istream stream{ &buf };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "An I/O error occurred while reading from the underlying stream"sv);
		CHECK(err->source().begin == source_position{ 1, 33 });

		unseekable_buf rethrowing_buf{ doc, 32u };
		std::istream rethrowing_stream{ &rethrowing_buf };
		rethrowing_stream.exceptions(std::ios::badbit);
		const auto rethrown = parse_failure([&]() { return toml::parse(rethrowing_stream); });
		REQUIRE(rethrown);
		CHECK(rethrown->description() == "the pipe broke"sv);
		CHECK(rethrown->source().begin == source_position{ 1, 33 });
	}

	SECTION("the stream fails partway through a value")
	{
		check_failure<unseekable_buf>("a = 07:32:00\n", 6u, { 1, 7 });
	}

	SECTION("the streambuf throws from showmanyc")
	{
		check_failure<showmanyc_buf>("a = 1\nb = 2\n", 0u, { 1, 1 }, "the pipe broke"sv);
		check_failure<showmanyc_buf>("a = 1\nb = 2\n", 6u, { 2, 1 }, "the pipe broke"sv);
	}
}

TEST_CASE("parsing - streams that fail mid-sequence or right after a line break")
{
	const auto check = [](const std::string& doc, size_t fail_at, source_position expected)
	{
		check_failure<unseekable_buf>(doc, fail_at, expected);
		check_failure<unbuffered_buf>(doc, fail_at, expected);
	};

	SECTION("mid-sequence, before the first code point is complete")
	{
		check("\xF0\x9F\x98\x80 = 1\n", 3u, { 1, 1 });
		for (const auto fail_at : { 4_sz, 9_sz })
			check_failure<unbuffered_buf>("\xF0\x9F\x98\x80 = 1\n", fail_at, { 1, 1 });
	}

	SECTION("mid-sequence, after a full read")
	{
		check("a = '" + std::string(26u, 'x') + "\xE2\x82\xAC'", 32u, { 1, 32 });
		check("a = '" + std::string(25u, 'x') + "\xE2\x82\xAC'", 32u, { 1, 31 });
		check("a = '" + std::string(58u, 'x') + "\xF0\x9F\x98\x82'", 64u, { 1, 64 });
	}

	SECTION("right after a line break, after a full read")
	{
		check("#" + std::string(30u, 'x') + "\nb = 2\n", 32u, { 2, 1 });
		check("#" + std::string(29u, 'x') + "\r\nb = 2\n", 32u, { 2, 1 });
		check("#" + std::string(30u, 'x') + "\r\nb = 2\n", 32u, { 1, 33 });
	}

	SECTION("partway through a read")
	{
		check_failure<unseekable_buf>("a = '\xE2\x82\xAC'", 6u, { 1, 6 });
		check_failure<unseekable_buf>("a = 1\nb = 2\n", 6u, { 2, 1 });
		check_failure<unseekable_buf>("#" + std::string(60u, 'x') + "\nb = 2\n", 50u, { 1, 51 });
	}

	SECTION("partway through an unbuffered read, losing what it read")
	{
		check_failure<unbuffered_buf>("a = '\xE2\x82\xAC'", 6u, { 1, 4 });
		check_failure<unbuffered_buf>("a = 1\nb = 2\n", 6u, { 1, 4 });
		check_failure<unbuffered_buf>("#" + std::string(60u, 'x') + "\nb = 2\n", 50u, { 1, 33 });
	}
}

TEST_CASE("parsing - streams that fail after an error")
{
	constexpr auto no_type	   = "Error while parsing value: could not determine value type"sv;
	constexpr auto underscores = "Error while parsing decimal integer: underscores may only follow digits"sv;
	const auto doc			   = "a = ;" + std::string(20u, ' ') + "b = 1\n";

	SECTION("before the failing byte is read")
	{
		check_failure<unseekable_buf>(doc, 15u, { 1, 5 }, no_type, no_type);
		check_failure<unseekable_buf>("a = ;" + std::string(27u, ' ') + "b = 1\n", 32u, { 1, 5 }, no_type, no_type);
	}

	SECTION("found by looking ahead into the failure")
	{
		check_failure<unseekable_buf>("a = 1__2\n", 8u, { 1, 7 }, underscores, underscores);
		check_failure<unbuffered_buf>("a = 1__" + std::string(40u, '2') + "\n",
									  40u,
									  { 1, 7 },
									  underscores,
									  underscores);
		check_failure<showmanyc_buf>("a = 1__2\n", 8u, { 1, 7 }, underscores, underscores);
	}

	SECTION("partway through an unbuffered read, losing the error")
	{
		check_failure<unbuffered_buf>(doc, 15u, { 1, 4 });
	}

	SECTION("before the error")
	{
		check_failure<unseekable_buf>("a = 1\nb = ;\n", 3u, { 1, 4 });
	}
}

TEST_CASE("parsing - streams with failbit or eofbit in their exception mask")
{
	for (const std::ios::iostate mask :
		 { std::ios::failbit, std::ios::eofbit, std::ios::failbit | std::ios::eofbit | std::ios::badbit })
	{
		CAPTURE(static_cast<int>(mask));

		for (const size_t length : { 5u, 31u, 32u, 33u, 64u })
		{
			CAPTURE(length);
			std::string doc = "a = 1";
			doc.resize(length, ' ');
			std::istringstream stream{ doc };
			stream.exceptions(mask);
			auto tbl = toml::parse(stream);
			CHECK(tbl["a"] == 1);
			CHECK(stream.exceptions() == mask);
		}

		std::istringstream with_bom{ BOM_PREFIX "a = 1" };
		with_bom.exceptions(mask);
		auto tbl = toml::parse(with_bom);
		CHECK(tbl["a"] == 1);

		std::istringstream empty;
		empty.exceptions(mask);
		auto empty_tbl = toml::parse(empty);
		CHECK(empty_tbl.begin() == empty_tbl.end());

		std::istringstream incomplete{ "a = " };
		incomplete.exceptions(mask);
		const auto err		= parse_failure([&]() { return toml::parse(incomplete); });
		const auto expected = parse_failure([]() { return toml::parse("a = "sv); });
		REQUIRE(err);
		REQUIRE(expected);
		CHECK(err->description() == expected->description());
		CHECK(err->source().begin == expected->source().begin);
	}
}

#endif

TEST_CASE("parsing - stream edge cases")
{
	SECTION("a stream already at its end")
	{
		std::istringstream stream;
		stream.setstate(std::ios::eofbit);
		auto tbl = toml::parse(stream);
		CHECK(tbl.begin() == tbl.end());
	}

	SECTION("a stream that has already failed")
	{
		std::istringstream stream{ "a = 1" };
		stream.setstate(std::ios::failbit);
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Reading from the underlying stream failed - zero bytes read"sv);
		CHECK(err->source().begin == source_position{ 1, 1 });
	}

	SECTION("a stream with badbit set")
	{
		std::istringstream stream{ "a = 1" };
		stream.setstate(std::ios::badbit);
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Reading from the underlying stream failed - zero bytes read"sv);
		CHECK(err->source().begin == source_position{ 1, 1 });
	}

	SECTION("a code point cut short at the end of a full block")
	{
		std::istringstream stream{ "# " + std::string(29u, ' ') + "\xE2" };
		const auto err = parse_failure([&]() { return toml::parse(stream); });
		REQUIRE(err);
		CHECK(err->description() == "Encountered EOF during incomplete utf-8 code point sequence"sv);
		CHECK(err->source().begin == source_position{ 1, 32 });
	}

	SECTION("a look-ahead that outgrows the initial window")
	{
		std::string doc = "a = 1";
		for (size_t i = 0; i < 130u; i++)
			doc += "\xE2\x82\xAC";
		parsing_should_fail(
			FILE_LINE_ARGS,
			doc,
			"1:5: Error while parsing value: numeric value too long to identify type - cannot exceed 126 characters"sv);
	}
}

TEST_CASE("parsing - code points split across reads")
{
	const auto check_splits = [](size_t boundary, std::string_view bom, const auto& parse)
	{
		for (const auto cp : { "\xC3\xA9"sv, "\xE2\x82\xAC"sv, "\xF0\x9F\x98\x82"sv })
		{
			for (size_t split = 1; split < cp.length(); split++)
			{
				INFO((bom.empty() ? "" : "after a byte order mark, ")
					 << cp.length() << "-byte code point split " << split << '|' << (cp.length() - split)
					 << " across byte " << boundary);

				const auto filler = std::string(boundary - split - 5u, 'x');
				const auto head	  = std::string{ bom } + "a = '" + filler;
				const auto lead	  = source_position{ 1, static_cast<source_index>(boundary - split + 1u) };
				const auto value  = filler + std::string{ cp };

				auto tbl = parse(head + std::string{ cp } + "'");
				CHECK(tbl["a"] == std::string_view{ value });
				REQUIRE(tbl["a"].node());
				CHECK(tbl["a"].node()->source().end == source_position{ 1, lead.column + 2u });

				const auto fails_at_lead = [&](const std::string& doc, std::string_view description)
				{
					const auto err = parse_failure([&]() { return parse(doc); });
					REQUIRE(err);
					CHECK(err->description() == description);
					CHECK(err->source().begin == lead);
				};
				fails_at_lead(head + std::string{ cp.substr(0u, split) } + "x'",
							  "Encountered invalid utf-8 sequence"sv);
				fails_at_lead(head + std::string{ cp.substr(0u, split) },
							  "Encountered EOF during incomplete utf-8 code point sequence"sv);
			}
		}
	};

	SECTION("unbuffered streams, read in 32-byte blocks")
	{
		check_splits(32u, ""sv, parse_through<unbuffered_buf>);
		check_splits(32u, BOM_PREFIX ""sv, parse_through<unbuffered_buf>);
	}

	SECTION("streams read a byte at a time")
	{
		check_splits(32u, ""sv, parse_through<unseekable_buf>);
		check_splits(32u, BOM_PREFIX ""sv, parse_through<unseekable_buf>);
	}

	SECTION("strings, validated in 4 KiB chunks")
	{
		check_splits(4096u, ""sv, [](const std::string& doc) { return toml::parse(doc); });
	}
}

TEST_CASE("parsing - long stretches across a read or validation boundary")
{
	const auto column_after = [](std::string_view text)
	{
		size_t code_points = {};
		for (const auto c : text)
			code_points += (static_cast<unsigned char>(c) & 0xC0u) != 0x80u;
		return "1:" + std::to_string(code_points + 1u);
	};

	const auto stretch = [](std::string_view prefix, std::string_view cp, size_t end)
	{
		std::string text{ prefix };
		text.append((end - text.size()) % cp.size(), 'x');
		while (text.size() < end)
			text.append(cp);
		return text;
	};

	for (const size_t boundary : { 259_sz, 515_sz, 4096_sz })
	{
		for (size_t end = boundary - 4u; end <= boundary + 4u; end++)
		{
			for (const auto cp : { "x"sv, "\xC3\xA9"sv, "\xE2\x82\xAC"sv, "\xF0\x9F\x98\x82"sv })
			{
				INFO(cp.length() << "-byte code points up to byte " << end);

				const auto comment = stretch("#", cp, end);
				parsing_should_fail(FILE_LINE_ARGS,
									comment + "\x01",
									column_after(comment)
										+ ": Error while parsing comment: control characters other than TAB (U+0009) "
										  "are explicitly prohibited in comments");

				const auto basic = stretch("a = \"", cp, end);
				parsing_should_fail(FILE_LINE_ARGS,
									basic + "\x01\"",
									column_after(basic)
										+ ": Error while parsing string: unescaped control characters other than TAB "
										  "(U+0009) are explicitly prohibited");

				const auto literal = stretch("a = '", cp, end);
				parsing_should_fail(FILE_LINE_ARGS,
									literal + "\x01'",
									column_after(literal)
										+ ": Error while parsing literal string: control characters other than TAB "
										  "(U+0009) are explicitly prohibited");
			}

			INFO("ascii up to byte " << end);
			for (const auto blank : { " "sv, "\t"sv })
			{
				const auto spaced = stretch("a", blank, end);
				parsing_should_fail(FILE_LINE_ARGS,
									spaced + "\x01",
									column_after(spaced)
										+ R"(: Error while parsing key-value pair: expected '=', saw '\u0001')");
			}

			const auto key = stretch("", "k", end);
			parsing_should_fail(FILE_LINE_ARGS,
								key + "\x01",
								column_after(key)
									+ R"(: Error while parsing key-value pair: expected '=', saw '\u0001')");
		}
	}
}

TEST_CASE("parsing - source paths")
{
	SECTION("string_view")
	{
		auto tbl = toml::parse("a = 1"sv, "a.toml"sv);
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

	SECTION("std::string rvalue")
	{
		auto tbl = toml::parse("a = 1"sv, std::string{ "a.toml" });
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

	SECTION("stream with a std::string rvalue")
	{
		std::istringstream ss{ "a = 1" };
		auto tbl = toml::parse(ss, std::string{ "a.toml" });
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

	SECTION("string literal")
	{
		auto tbl = toml::parse("a = 1"sv, "a.toml");
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

	SECTION("stream with a string literal")
	{
		std::istringstream ss{ "a = 1" };
		auto tbl = toml::parse(ss, "a.toml");
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

#if TOML_HAS_CHAR8

	SECTION("char8_t document with a string literal")
	{
		auto tbl = toml::parse(u8"a = 1"sv, "a.toml");
		REQUIRE(tbl["a"]);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "a.toml"sv);
	}

#endif

	SECTION("null pointer")
	{
		const char* path = nullptr;
		auto tbl		 = toml::parse("a = 1"sv, path);
		REQUIRE(tbl["a"]);
		CHECK(!tbl["a"].node()->source().path);
	}

	SECTION("errors carry the path")
	{
		const auto err = parse_failure([]() { return toml::parse("a = trUe"sv, std::string{ "bad.toml" }); });
		REQUIRE(err);
		CHECK(err->description() == "Error while parsing boolean: expected 'true', saw 'trU'"sv);
		CHECK(err->source().begin == source_position{ 1, 7 });
		REQUIRE(err->source().path);
		CHECK(*err->source().path == "bad.toml"sv);
	}
}

TEST_CASE("parsing - files")
{
	SECTION("a file")
	{
		const temp_file file{ "a = 1\n[b]\nc = 'two'\n" };
		auto tbl = toml::parse_file(file.str());
		CHECK(tbl["a"] == 1);
		CHECK(tbl["b"]["c"] == "two"sv);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == file.str());
	}

	SECTION("a file with an error")
	{
		const temp_file file{ "a = 1\nb = trUe\n" };
		const auto err = parse_failure([&]() { return toml::parse_file(file.str()); });
		REQUIRE(err);
		CHECK(err->description() == "Error while parsing boolean: expected 'true', saw 'trU'"sv);
		CHECK(err->source().begin == source_position{ 2, 7 });
		REQUIRE(err->source().path);
		CHECK(*err->source().path == file.str());
	}

	SECTION("a file too large to read in one go")
	{
		const std::string big(1024u * 1024u * 3u, 'x');
		const temp_file file{ "big = '" + big + "'\nsmall = 1\n" };
		auto tbl = toml::parse_file(file.str());
		CHECK((tbl["big"].value_or(""sv) == big));
		CHECK(tbl["small"] == 1);
	}

	SECTION("a missing file")
	{
		const auto path = (std::filesystem::temp_directory_path() / "tomlplusplus_test_does_not_exist.toml").string();
		const auto err	= parse_failure([&]() { return toml::parse_file(path); });
		REQUIRE(err);
		CHECK(err->description() == "File could not be opened for reading"sv);
		CHECK(err->source().begin == source_position{});
		REQUIRE(err->source().path);
		CHECK(*err->source().path == path);
	}

	SECTION("a directory")
	{
		const auto path = std::filesystem::temp_directory_path().string();
		CHECK(parse_failure([&]() { return toml::parse_file(path); }).has_value());
	}
}

#if TOML_HAS_CHAR8

TEST_CASE("parsing - char8_t")
{
	SECTION("a document")
	{
		auto tbl = toml::parse(u8"a = 'kek'"sv);
		CHECK(tbl["a"] == "kek"sv);
	}

	SECTION("a literal")
	{
		auto tbl = u8"a = 'kek'"_toml;
		CHECK(tbl["a"] == "kek"sv);
	}

	SECTION("with a source path")
	{
		auto tbl = toml::parse(u8"a = 1"sv, "u8.toml"sv);
		REQUIRE(tbl["a"].node()->source().path);
		CHECK(*tbl["a"].node()->source().path == "u8.toml"sv);

		auto tbl2 = toml::parse(u8"a = 1"sv, std::string{ "u8.toml" });
		REQUIRE(tbl2["a"].node()->source().path);
		CHECK(*tbl2["a"].node()->source().path == "u8.toml"sv);
	}

	SECTION("a file")
	{
		const temp_file file{ "a = 1\n" };
		const auto path = file.str();
		auto tbl = toml::parse_file(std::u8string_view{ reinterpret_cast<const char8_t*>(path.data()), path.length() });
		CHECK(tbl["a"] == 1);
	}
}

#endif
