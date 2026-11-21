// SPDX-License-Identifier: MIT
//
// Fuzz harness: toml_parse_sources_fuzzer targets TOML source parsing

#include <cstdio>
#include <cstdint>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <unistd.h>

#include <toml++/toml.hpp>

namespace
{
	constexpr std::size_t max_size = 4096;

	// per-process temp file so concurrent fuzzing processes cannot clash
	const std::string tmp_file_path = []()
	{
		std::string s = "/tmp/tomlpp_fuzz_source_";
		s += std::to_string(static_cast<long>(::getpid()));
		s += ".toml";
		return s;
	}();

	// guaranteed to be missing: the directory does not exist
	constexpr char missing_file_path[] = "/tmp/tomlpp_fuzz_missing_dir_xyz/config.toml";

	// serves the document byte-at-a-time; once fail_at bytes have been
	// served, either throws (the parser's io_error wall, exceptions builds)
	// or poisons the stream with badbit (non-throwing builds)
	class failing_streambuf final : public std::streambuf
	{
		std::string data_;
		std::size_t pos_ = {};
		std::size_t fail_at_;
		std::istream* stream_ = {};

	  public:
		failing_streambuf(std::string_view data, std::size_t fail_at) : data_{ data }, fail_at_{ fail_at }
		{}

		void attach(std::istream& stream)
		{
			stream_ = &stream;
		}

	  protected:
		int_type underflow() override
		{
			if (pos_ >= data_.size())
				return traits_type::eof();
			if (fail_at_ != static_cast<std::size_t>(-1) && pos_ >= fail_at_)
			{
#if TOML_EXCEPTIONS
				throw std::runtime_error{ "fuzz: induced read failure" };
#else
				if (stream_)
					stream_->setstate(std::ios::badbit);
				return traits_type::eof();
#endif
			}
			setg(&data_[pos_], &data_[pos_], &data_[pos_] + 1u);
			pos_++;
			return traits_type::to_int_type(*gptr());
		}
	};

	void introspect_error(const toml::parse_error& err, std::ostream& oss)
	{
		oss << err.description();

		const auto& src = err.source();
		oss << src.begin << src.end << src;
		(void)static_cast<bool>(src.begin);
		(void)(src.begin == src.end);
		(void)(src.begin != src.end);
		(void)(src.begin < src.end);
		(void)(src.begin <= src.end);
		(void)(src.begin > src.end);
		(void)(src.begin >= src.end);
		if (src.path)
			oss << *src.path;
	}

	void introspect_table(const toml::table& tbl, std::ostream& oss)
	{
		for (const auto& [key, node] : tbl)
		{
			(void)key;
			const auto& src = node.source();
			oss << src.begin << src.end << src;
			(void)static_cast<bool>(src.begin);
			(void)(src.begin != src.end);
		}
	}

#if TOML_EXCEPTIONS

	// exceptions builds: the parse functions return a table and throw on
	// error (the non-throwing source-path overloads throw too); the
	// parse_error path is exercised here
	template <typename ParseCall>
	void consume_parse(ParseCall&& call, std::ostream& oss)
	{
		try
		{
			toml::table tbl = call();
			introspect_table(tbl, oss);
		}
		catch (const toml::parse_error& err)
		{
			introspect_error(err, oss);
		}
	}

#else // !TOML_EXCEPTIONS

	// non-throwing builds: every parse returns a parse_result; exercise the
	// full result API (bool/failed/error/table, iteration, move plumbing)
	void introspect_result(const toml::parse_result& result, std::ostream& oss)
	{
		if (!result)
		{
			introspect_error(result.error(), oss);
		}
		else
		{
			introspect_table(result.table(), oss);
			for (const auto& [key, node] : result)
			{
				(void)key;
				oss << node.source().begin << node.source().end;
			}
		}
	}

#endif
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	if (size > max_size)
		return 0;

	const auto flags = size ? data[0] : std::uint8_t{};
	std::ostringstream oss;

	const std::string_view input{ reinterpret_cast<const char*>(data), size };
	const auto sep = input.find('\0');
	const std::string path_str{ input.substr(0u, sep == std::string_view::npos ? input.size() : sep) };
	const std::string_view doc_str = sep == std::string_view::npos ? std::string_view{} : input.substr(sep + 1u);

	// source-path-annotated in-memory parses (both path parameter flavours);
	// in non-throwing builds these instantiate the full parse_result API
#if TOML_EXCEPTIONS
	consume_parse([&] { return toml::parse(doc_str, std::string{ path_str }); }, oss);
	consume_parse([&] { return toml::parse(doc_str, path_str); }, oss);
#else
	toml::parse_result res = toml::parse(doc_str, std::string{ path_str });
	introspect_result(res, oss);
	{
		toml::parse_result moved{ std::move(res) }; // move constructor
		res = std::move(moved);						// move assignment
		introspect_result(res, oss);
		if (res)
			(void)std::move(res).table(); // rvalue table()
		else
			(void)std::move(res).error(); // rvalue error()
	}
	{
		toml::parse_result res2 = toml::parse(doc_str, path_str);
		introspect_result(res2, oss);
	}
#endif

	// istream parse with an induced mid-read failure (or a clean
	// byte-at-a-time read) plus a source path
	{
		const bool fail = (flags & 1u) != 0u && !doc_str.empty();
		failing_streambuf buf{ doc_str, fail ? doc_str.size() / 2u : static_cast<std::size_t>(-1) };
		std::istream stream{ &buf };
		buf.attach(stream);
#if TOML_EXCEPTIONS
		consume_parse([&] { return toml::parse(stream, path_str); }, oss);
#else
		toml::parse_result stream_res = toml::parse(stream, path_str);
		introspect_result(stream_res, oss);
#endif
	}

	// parse_file on a guaranteed-missing path (open failure branch)
	if ((flags & 2u) != 0u)
	{
#if TOML_EXCEPTIONS
		consume_parse([&] { return toml::parse_file(missing_file_path); }, oss);
#else
		toml::parse_result missing = toml::parse_file(missing_file_path);
		introspect_result(missing, oss);
#endif
	}

	// parse_file on a real file (full-read branch, <= 2 MiB threshold)
	{
		if (FILE* f = std::fopen(tmp_file_path.c_str(), "wb"))
		{
			if (!doc_str.empty())
				std::fwrite(doc_str.data(), 1u, doc_str.size(), f);
			std::fclose(f);
		}
#if TOML_EXCEPTIONS
		consume_parse([&] { return toml::parse_file(tmp_file_path); }, oss);
#else
		toml::parse_result file_res = toml::parse_file(tmp_file_path);
		introspect_result(file_res, oss);
#endif
	}

	return 0;
}
