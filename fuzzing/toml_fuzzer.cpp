#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>

#include <toml++/toml.hpp>

using namespace std::string_view_literals;

namespace
{
	// printed output grows with the square of the document's size
	constexpr std::size_t max_document_size = 4096;

	struct chunked_streambuf : std::streambuf
	{
		std::string data;
		std::size_t position = {};
		std::size_t chunk_size;

		chunked_streambuf(std::string_view source, std::size_t chunk) : data{ source }, chunk_size{ chunk }
		{}

	  protected:
		int_type underflow() override
		{
			if (position >= data.size())
				return traits_type::eof();
			const auto remaining = data.size() - position;
			const auto count	 = remaining < chunk_size ? remaining : chunk_size;
			setg(&data[position], &data[position], &data[position] + count);
			position += count;
			return traits_type::to_int_type(*gptr());
		}
	};

	struct outcome
	{
		bool parsed = {};
		toml::table table;
		std::string error;
	};

	std::string describe(const toml::parse_error& err)
	{
		const auto& where = err.source();
		return std::to_string(where.begin.line) + ":" + std::to_string(where.begin.column) + " to "
			 + std::to_string(where.end.line) + ":" + std::to_string(where.end.column) + ": "
			 + std::string{ err.description() };
	}

	template <typename Source>
	outcome parse(Source&& source)
	{
#if TOML_EXCEPTIONS
		try
		{
			return { true, toml::parse(static_cast<Source&&>(source)), {} };
		}
		catch (const toml::parse_error& err)
		{
			return { false, {}, describe(err) };
		}
#else
		auto result = toml::parse(static_cast<Source&&>(source));
		if (!result)
			return { false, {}, describe(result.error()) };
		return { true, std::move(result).table(), {} };
#endif
	}

	bool same_position(const toml::source_region& lhs, const toml::source_region& rhs)
	{
		return lhs.begin == rhs.begin && lhs.end == rhs.end;
	}

	bool same(const toml::node& lhs, const toml::node& rhs, bool exact)
	{
		if (lhs.type() != rhs.type())
			return false;
		if (exact && !same_position(lhs.source(), rhs.source()))
			return false;

		if (const auto* tbl = lhs.as_table())
		{
			const auto& other = *rhs.as_table();
			if (tbl->size() != other.size() || (exact && tbl->is_inline() != other.is_inline()))
				return false;
			auto counterpart = other.begin();
			for (auto&& [key, value] : *tbl)
			{
				if (key != counterpart->first || !same(value, counterpart->second, exact))
					return false;
				if (exact && !same_position(key.source(), counterpart->first.source()))
					return false;
				++counterpart;
			}
			return true;
		}

		if (const auto* arr = lhs.as_array())
		{
			const auto& other = *rhs.as_array();
			if (arr->size() != other.size())
				return false;
			for (std::size_t i = 0; i < arr->size(); i++)
			{
				if (!same((*arr)[i], other[i], exact))
					return false;
			}
			return true;
		}

		if (const auto* number = lhs.as_floating_point())
		{
			const double a = number->get();
			const double b = rhs.as_floating_point()->get();
			return std::isnan(a) ? std::isnan(b) : (a == b && std::signbit(a) == std::signbit(b));
		}

		return toml::impl::node_deep_equality(&lhs, &rhs);
	}

	[[noreturn]] void fail(std::string_view what, std::string_view doc, std::string_view detail)
	{
		for (const auto text : { what, "\ninput:\n"sv, doc, "\ndetail:\n"sv, detail, "\n"sv })
		{
			if (!text.empty())
				std::fwrite(text.data(), 1, text.size(), stderr);
		}
		std::abort();
	}

	// each check is its own function so that a fuzzing service reports their failures separately

	[[gnu::noinline]] void check_streams(std::string_view doc, const outcome& from_string)
	{
		std::istringstream seekable{ std::string{ doc } };
		chunked_streambuf chunks{ doc, 1u + doc.size() % 37u };
		std::istream unseekable{ &chunks };
		for (std::istream* stream : { static_cast<std::istream*>(&seekable), &unseekable })
		{
			const auto from_stream = parse(*stream);
			if (from_stream.parsed != from_string.parsed)
				fail("string and stream disagree on whether the document parses"sv,
					 doc,
					 from_string.parsed ? from_stream.error : from_string.error);
			if (!from_string.parsed && from_stream.error != from_string.error)
				fail("string and stream report different errors"sv, doc, from_string.error + "\n" + from_stream.error);
			if (from_string.parsed && !same(from_string.table, from_stream.table, true))
				fail("string and stream give different tables"sv, doc, {});
		}
	}

	[[gnu::noinline]] void check_round_trip(std::string_view doc, const toml::table& tbl)
	{
		for (const auto flags :
			 { toml::toml_formatter::default_flags,
			   toml::format_flags::none,
			   toml::format_flags::allow_literal_strings | toml::format_flags::allow_multi_line_strings
				   | toml::format_flags::allow_real_tabs_in_strings | toml::format_flags::allow_unicode_strings,
			   toml::toml_formatter::default_flags | toml::format_flags::terse_key_value_pairs
				   | toml::format_flags::force_multi_line_arrays })
		{
			std::ostringstream stream;
			stream << toml::toml_formatter{ tbl, flags };
			const auto printed	= stream.str();
			const auto reparsed = parse(std::string_view{ printed });
			if (!reparsed.parsed)
				fail("printed TOML does not parse"sv, doc, printed + "\n" + reparsed.error);
			if (!same(tbl, reparsed.table, false))
				fail("printed TOML parses to a different table"sv, doc, printed);
		}
	}

	[[gnu::noinline]] void check_other_formats(const toml::table& tbl)
	{
		const auto print = [](auto&& formatter)
		{
			std::ostringstream stream;
			stream << formatter;
		};
		print(toml::json_formatter{ tbl });
		print(toml::json_formatter{ tbl, toml::format_flags::none });
		print(toml::yaml_formatter{ tbl });
		print(toml::yaml_formatter{ tbl, toml::format_flags::none });
	}
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	if (size > max_document_size)
		return 0;

	const std::string_view doc{ reinterpret_cast<const char*>(data), size };
	const auto from_string = parse(doc);
	check_streams(doc, from_string);
	if (!from_string.parsed)
		return 0;

	check_round_trip(doc, from_string.table);
	check_other_formats(from_string.table);
	return 0;
}
