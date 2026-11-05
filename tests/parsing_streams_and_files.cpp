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

	SECTION("strings and streams agree on a code point cut short at the end of a full block")
	{
		parsing_should_fail(FILE_LINE_ARGS,
							"a = ;" + std::string(26u, ' ') + "\xE2",
							"1:32: Encountered EOF during incomplete utf-8 code point sequence"sv);
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
