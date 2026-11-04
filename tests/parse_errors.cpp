// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	template <typename... Args>
	parse_error make_error(const char* desc, Args&&... args)
	{
#if TOML_EXCEPTIONS
		return parse_error{ desc, std::forward<Args>(args)... };
#else
		return parse_error{ std::string{ desc }, std::forward<Args>(args)... };
#endif
	}

	std::optional<parse_error> parse_failure(std::string_view doc, std::string_view source_path)
	{
#if TOML_EXCEPTIONS
		try
		{
			static_cast<void>(toml::parse(doc, source_path));
		}
		catch (const parse_error& err)
		{
			return err;
		}
		return {};
#else
		auto result = toml::parse(doc, source_path);
		if (result)
			return {};
		return std::move(result).error();
#endif
	}

	std::string streamed(const parse_error& err)
	{
		std::ostringstream ss;
		ss << err;
		return std::move(ss).str();
	}
}

TEST_CASE("parse errors - construction")
{
	const source_region region{ { 3, 7 }, { 3, 9 }, std::make_shared<const std::string>("bad.toml") };

	SECTION("description and region")
	{
		const auto err = make_error("kaboom", source_region{ region });
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == region.begin);
		CHECK(err.source().end == region.end);
		CHECK(err.source().path == region.path);
	}

	SECTION("description and a copied region")
	{
		const auto err = make_error("kaboom", region);
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == region.begin);
		CHECK(err.source().end == region.end);
		CHECK(err.source().path == region.path);
	}

	SECTION("description and position")
	{
		const auto err = make_error("kaboom", source_position{ 3, 7 });
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == source_position{ 3, 7 });
		CHECK(err.source().end == source_position{ 3, 7 });
		CHECK(!err.source().path);
	}

	SECTION("description, position and path")
	{
		const auto err = make_error("kaboom", source_position{ 3, 7 }, region.path);
		CHECK(err.description() == "kaboom"sv);
		CHECK(err.source().begin == source_position{ 3, 7 });
		CHECK(err.source().end == source_position{ 3, 7 });
		CHECK(err.source().path == region.path);
	}
}

TEST_CASE("parse errors - description is null-terminated")
{
	const auto err	= make_error("kaboom", source_position{ 1, 1 });
	const auto desc = err.description();
	CHECK(desc == "kaboom"sv);
	CHECK(desc.data()[desc.length()] == '\0');
}

#if TOML_EXCEPTIONS

TEST_CASE("parse errors - are std::runtime_errors when exceptions are enabled")
{
	static_assert(std::is_base_of_v<std::runtime_error, parse_error>);

	const auto err = make_error("kaboom", source_position{ 1, 1 });
	CHECK(std::string_view{ err.what() } == "kaboom"sv);
}

#endif

TEST_CASE("parse errors - stream output")
{
	const auto path = std::make_shared<const std::string>("bad.toml");

	CHECK(streamed(make_error("kaboom", source_position{ 3, 7 })) == "kaboom\n\t(error occurred at line 3, column 7)");
	CHECK(streamed(make_error("kaboom", source_position{ 3, 7 }, path))
		  == "kaboom\n\t(error occurred at line 3, column 7 of 'bad.toml')");
	CHECK(streamed(make_error("kaboom", source_region{ { 3, 7 }, { 3, 9 }, path }))
		  == "kaboom\n\t(error occurred at line 3, column 7 to line 3, column 9 of 'bad.toml')");
}

TEST_CASE("parse errors - from the parser")
{
	const auto err = parse_failure("enabled = trUe"sv, "bad.toml"sv);
	REQUIRE(err);
	CHECK(err->description() == "Error while parsing boolean: expected 'true', saw 'trU'"sv);
	CHECK(err->source().begin == source_position{ 1, 13 });
	CHECK(err->source().end == source_position{ 1, 13 });
	REQUIRE(err->source().path);
	CHECK(*err->source().path == "bad.toml"sv);
	CHECK(streamed(*err)
		  == "Error while parsing boolean: expected 'true', saw 'trU'\n\t(error occurred at line 1, column 13 of "
			 "'bad.toml')");
}

TEST_CASE("parse errors - long descriptions are truncated")
{
	const std::string key(600u, 'k');
	const auto err = parse_failure(key + " = 1\n" + key + " = 2\n", "long.toml"sv);
	REQUIRE(err);
	const auto full = "Error while parsing key-value pair: cannot redefine existing integer '" + key + "'";
	const auto desc = err->description();
	CHECK(desc.length() < full.length());
	CHECK(std::string_view{ full }.substr(0u, desc.length()) == desc);
}
