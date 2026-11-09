#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

#include <toml++/toml.hpp>

namespace
{
	[[noreturn]] void fail(const char* what, const std::string& actual, const std::string& expected)
	{
		std::fprintf(stderr, "%s: '%s', expected '%s'\n", what, actual.c_str(), expected.c_str());
		std::abort();
	}

	template <typename T>
	std::string reference(T val, bool relaxed)
	{
		std::string text = std::signbit(val) ? "-" : "";
		val				 = std::abs(val);
		if (val == T{})
			return text + "0.0";

		const auto scientific = [&](auto... precision)
		{
			char buf[64];
			const auto str = std::string{ buf, std::to_chars(buf, buf + sizeof(buf), val, precision...).ptr };
			const auto e   = str.find('e');
			return std::pair{ str.substr(0, 1) + (e > 1u ? str.substr(2u, e - 2u) : std::string{}),
							  std::stoi(str.substr(e + 1u)) };
		};

		auto [digits, exponent] = scientific(std::chars_format::scientific);
		if (relaxed && digits.length() > 6u)
			std::tie(digits, exponent) = scientific(std::chars_format::scientific, 5);
		while (digits.length() > 1u && digits.back() == '0')
			digits.pop_back();

		if (exponent < -4 || exponent >= (relaxed ? 6 : 16))
		{
			text += digits.substr(0, 1);
			if (digits.length() > 1u)
				text += "." + digits.substr(1);
			const auto magnitude = std::to_string(exponent < 0 ? -exponent : exponent);
			return text + (exponent < 0 ? "e-" : "e+") + (magnitude.length() < 2u ? "0" : "") + magnitude;
		}
		if (exponent < 0)
			return text + "0." + std::string(static_cast<std::size_t>(-exponent - 1), '0') + digits;
		const auto whole = static_cast<std::size_t>(exponent) + 1u;
		if (digits.length() <= whole)
			return text + digits + std::string(whole - digits.length(), '0') + ".0";
		return text + digits.substr(0, whole) + "." + digits.substr(whole);
	}

	std::string yaml_of(const std::string& text)
	{
		const auto e = text.find('e');
		if (e == std::string::npos || text.find('.') != std::string::npos)
			return text;
		return text.substr(0, e) + ".0" + text.substr(e);
	}

	template <typename T>
	std::string printed(T val, bool relaxed)
	{
		std::ostringstream stream;
		toml::impl::print_to_stream(stream, val, toml::value_flags::none, relaxed);
		return stream.str();
	}

	template <typename Formatter>
	std::string formatted(double val, bool relaxed)
	{
		std::ostringstream stream;
		stream << Formatter{ toml::value{ val },
							 Formatter::default_flags
								 | (relaxed ? toml::format_flags::relaxed_float_precision : toml::format_flags::none) };
		return stream.str();
	}

	template <typename T>
	bool same_bits(T lhs, T rhs)
	{
		return std::memcmp(&lhs, &rhs, sizeof(T)) == 0;
	}

	double parse_double(const std::string& text)
	{
		try
		{
			const auto tbl = toml::parse("v = " + text);
			if (const auto val = tbl["v"].value_exact<double>())
				return *val;
		}
		catch (const toml::parse_error& err)
		{
			fail("printed double does not parse", text, std::string{ err.description() });
		}
		fail("printed double parses as another type", text, "a float");
	}

	void check_double(double val)
	{
		if (!std::isfinite(val))
			return;

		const auto text		= printed(val, false);
		const auto expected = reference(val, false);
		if (text != expected)
			fail("double", text, expected);
		if (const auto toml_text = formatted<toml::toml_formatter>(val, false); toml_text != expected)
			fail("toml_formatter", toml_text, expected);
		if (const auto json_text = formatted<toml::json_formatter>(val, false); json_text != expected)
			fail("json_formatter", json_text, expected);
		if (const auto yaml_text = formatted<toml::yaml_formatter>(val, false); yaml_text != yaml_of(expected))
			fail("yaml_formatter", yaml_text, yaml_of(expected));

		const auto relaxed = reference(val, true);
		if (const auto relaxed_text = printed(val, true); relaxed_text != relaxed)
			fail("relaxed double", relaxed_text, relaxed);
		if (const auto yaml_text = formatted<toml::yaml_formatter>(val, true); yaml_text != yaml_of(relaxed))
			fail("relaxed yaml_formatter", yaml_text, yaml_of(relaxed));

		if (const auto back = parse_double(text); !same_bits(back, val))
			fail("double read back", printed(back, false), text);
	}

	void check_float(float val)
	{
		if (!std::isfinite(val))
			return;

		const auto text		= printed(val, false);
		const auto expected = reference(val, false);
		if (text != expected)
			fail("float", text, expected);
		if (const auto relaxed_text = printed(val, true); relaxed_text != reference(val, true))
			fail("relaxed float", relaxed_text, reference(val, true));

		if (const float back = std::strtof(text.c_str(), nullptr); !same_bits(back, val))
			fail("float read back", printed(back, false), text);
	}
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	if (size >= sizeof(double))
	{
		double val;
		std::memcpy(&val, data, sizeof(val));
		check_double(val);
	}
	if (size >= sizeof(float))
	{
		float val;
		std::memcpy(&val, data, sizeof(val));
		check_float(val);
	}

	std::string doc = "v = ";
	doc.append(reinterpret_cast<const char*>(data), size);
	try
	{
		const auto tbl = toml::parse(doc);
		if (const auto val = tbl["v"].value_exact<double>())
			check_double(*val);
	}
	catch (const toml::parse_error&)
	{}
	return 0;
}
