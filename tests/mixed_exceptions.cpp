// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
	struct pipe_broke : std::runtime_error
	{
		pipe_broke() : std::runtime_error{ "the pipe broke" }
		{}
	};

	class failing_buf : public std::streambuf
	{
		std::string data_;
		size_t pos_ = {};
		size_t fail_at_;
		bool throw_int_;

	  public:
		failing_buf(std::string data, size_t fail_at, bool throw_int)
			: data_{ std::move(data) },
			  fail_at_{ fail_at },
			  throw_int_{ throw_int }
		{}

	  protected:
		int_type underflow() override
		{
			if (pos_ == fail_at_)
			{
				if (throw_int_)
					throw 42;
				throw pipe_broke{};
			}
			if (pos_ >= data_.size())
				return traits_type::eof();

			setg(&data_[pos_], &data_[pos_], &data_[pos_] + 1);
			pos_++;
			return traits_type::to_int_type(*gptr());
		}
	};

	// never has a get area, so it's read in 32-byte blocks
	class unbuffered_buf : public std::streambuf
	{
		std::string data_;
		size_t pos_ = {};
		size_t fail_at_;

	  public:
		unbuffered_buf(std::string data, size_t fail_at) : data_{ std::move(data) }, fail_at_{ fail_at }
		{}

	  protected:
		int_type underflow() override
		{
			if (pos_ == fail_at_)
				throw pipe_broke{};
			if (pos_ >= data_.size())
				return traits_type::eof();
			return traits_type::to_int_type(data_[pos_]);
		}

		int_type uflow() override
		{
			const auto c = underflow();
			if (!traits_type::eq_int_type(c, traits_type::eof()))
				pos_++;
			return c;
		}
	};

	// in_avail() calls showmanyc() once the first fail_at bytes are used up
	class showmanyc_buf : public std::streambuf
	{
		std::string data_;

	  public:
		showmanyc_buf(std::string data, size_t fail_at) : data_{ std::move(data) }
		{
			setg(data_.data(), data_.data(), data_.data() + fail_at);
		}

	  protected:
		std::streamsize showmanyc() override
		{
			throw pipe_broke{};
		}
	};

	std::string describe_parse(std::streambuf& buf, std::ios::iostate mask)
	{
		std::istream stream{ &buf };
		stream.exceptions(mask);
		try
		{
			const auto result = toml::parse(stream);
			if (result)
				return "parsed";
			const auto& err = result.error();
			return std::to_string(err.source().begin.line) + ":" + std::to_string(err.source().begin.column) + ": "
				 + std::string{ err.description() };
		}
		catch (const pipe_broke& exc)
		{
			return "rethrown: " + std::string{ exc.what() };
		}
		catch (int exc)
		{
			return "rethrown: " + std::to_string(exc);
		}
		catch (...)
		{
			return "rethrown: something else";
		}
	}

	std::string parse_failing_stream(const std::string& doc,
									 size_t fail_at,
									 std::ios::iostate mask,
									 bool throw_int = false)
	{
		failing_buf buf{ doc, fail_at, throw_int };
		return describe_parse(buf, mask);
	}
}

int main()
{
	int failures = {};
	const auto expect =
		[&](const std::string& doc, size_t fail_at, const std::string& actual, std::string_view expected)
	{
		if (actual == expected)
			return;
		std::cerr << "failing at byte " << fail_at << " of:\n"
				  << doc << "\nexpected: " << expected << "\n  actual: " << actual << "\n\n";
		failures++;
	};

	const std::string doc = "a = 1\nb = 'two'\n";
	expect(doc, 0u, parse_failing_stream(doc, 0u, std::ios::badbit), "rethrown: the pipe broke");
	expect(doc, 3u, parse_failing_stream(doc, 3u, std::ios::badbit), "rethrown: the pipe broke");
	expect(doc, 3u, parse_failing_stream(doc, 3u, std::ios::badbit, true), "rethrown: 42");
	expect(doc,
		   0u,
		   parse_failing_stream(doc, 0u, std::ios::goodbit),
		   "1:1: Reading from the underlying stream failed - zero bytes read");
	expect(doc,
		   3u,
		   parse_failing_stream(doc, 3u, std::ios::goodbit),
		   "1:4: An I/O error occurred while reading from the underlying stream");

	const std::string cut_short = "\xF0\x9F\x98\x80 = 1\n";
	expect(cut_short, 3u, parse_failing_stream(cut_short, 3u, std::ios::badbit), "rethrown: the pipe broke");
	expect(cut_short,
		   3u,
		   parse_failing_stream(cut_short, 3u, std::ios::goodbit),
		   "1:1: An I/O error occurred while reading from the underlying stream");

	const std::string local_time = "a = 07:32:00\n";
	expect(local_time, 6u, parse_failing_stream(local_time, 6u, std::ios::badbit), "rethrown: the pipe broke");
	expect(local_time,
		   6u,
		   parse_failing_stream(local_time, 6u, std::ios::goodbit),
		   "1:7: An I/O error occurred while reading from the underlying stream");

	const std::string after_error = "a = 1__2\n";
	constexpr std::string_view underscores =
		"1:7: Error while parsing decimal integer: underscores may only follow digits";
	for (const auto mask : { std::ios::badbit, std::ios::goodbit })
		expect(after_error, 8u, parse_failing_stream(after_error, 8u, mask), underscores);

	{
		showmanyc_buf buf{ doc, 0u };
		expect(doc, 0u, describe_parse(buf, std::ios::goodbit), "rethrown: the pipe broke");
	}
	{
		showmanyc_buf buf{ after_error, 8u };
		expect(after_error, 8u, describe_parse(buf, std::ios::goodbit), underscores);
	}

	const std::string body = "a = 1\n"
							 "b = '\xE2\x82\xAC'\n"
							 "c = [ 1.5, { d = 1979-05-27T07:32:00Z }, 0x1_F ]\n"
							 "[e]\n"
							 "f.\"g\" = \"\"\"h\"\"\" # i\n";
	for (size_t padding = 0; padding < 32u; padding++)
	{
		const auto padded = std::string(padding, ' ') + body;
		for (size_t fail_at = 0; fail_at <= padded.size() + 1u; fail_at++)
		{
			unbuffered_buf buf{ padded, fail_at };
			expect(padded,
				   fail_at,
				   describe_parse(buf, std::ios::badbit),
				   fail_at <= padded.size() ? "rethrown: the pipe broke" : "parsed");
		}
	}

	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
