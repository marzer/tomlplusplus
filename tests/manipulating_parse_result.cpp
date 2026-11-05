// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

#if !TOML_EXCEPTIONS

TEST_CASE("parse_result - good parse")
{
	auto result = "key = true"_toml;
	static_assert(std::is_same_v<decltype(result), parse_result>);
	static_assert(!std::is_same_v<decltype(result), table>);

	REQUIRE(result.succeeded());
	REQUIRE(!result.failed());
	REQUIRE(result);

	REQUIRE(!result.table().empty());
	REQUIRE(result.table().size() == 1);
	REQUIRE(!std::move(result).table().empty());
	REQUIRE(!static_cast<const parse_result&>(result).table().empty());

	REQUIRE(!static_cast<table&>(result).empty());
	REQUIRE(!static_cast<table&&>(result).empty());
	REQUIRE(!static_cast<const table&>(result).empty());

	auto& tbl = static_cast<table&>(result);
	CHECK(tbl["key"sv]);
	CHECK(result["key"sv]);
	CHECK(&result["key"sv].ref<bool>() == &tbl["key"sv].ref<bool>());
	CHECK(result.begin() == tbl.begin());
	CHECK(result.end() == tbl.end());
	CHECK(result.begin() != tbl.end());
	CHECK(result.cbegin() == tbl.cbegin());
	CHECK(result.cend() == tbl.cend());
	CHECK(result.cbegin() != tbl.cend());

	auto& cresult = static_cast<const parse_result&>(result);
	auto& ctbl	  = static_cast<const table&>(cresult);
	CHECK(cresult.begin() == ctbl.begin());
	CHECK(cresult.end() == ctbl.end());
	CHECK(cresult.begin() != ctbl.end());
	CHECK(cresult.cbegin() == ctbl.cbegin());
	CHECK(cresult.cend() == ctbl.cend());
	CHECK(cresult.cbegin() != ctbl.cend());
	CHECK(ctbl["key"sv]);
	CHECK(cresult["key"sv]);
	CHECK(&cresult["key"sv].ref<bool>() == &ctbl["key"sv].ref<bool>());

	size_t tbl_iterations{};
	for (auto&& [k, v] : tbl)
	{
		(void)k;
		(void)v;
		tbl_iterations++;
	}
	size_t result_iterations{};
	for (auto& [k, v] : result)
	{
		(void)k;
		(void)v;
		result_iterations++;
	}
	size_t cresult_iterations{};
	for (auto [k, v] : cresult)
	{
		(void)k;
		(void)v;
		cresult_iterations++;
	}
	CHECK(tbl_iterations == tbl.size());
	CHECK(tbl_iterations == result_iterations);
	CHECK(tbl_iterations == cresult_iterations);
}

TEST_CASE("parse_result - bad parse")
{
	auto result = "key = trUe"_toml;
	static_assert(std::is_same_v<decltype(result), parse_result>);
	static_assert(!std::is_same_v<decltype(result), table>);

	REQUIRE(!result.succeeded());
	REQUIRE(result.failed());
	REQUIRE(!result);

	CHECK(!result["key"sv]);
	CHECK(result.begin() == decltype(result.begin()){});
	CHECK(result.end() == decltype(result.end()){});
	CHECK(result.cbegin() == decltype(result.cbegin()){});
	CHECK(result.cend() == decltype(result.cend()){});

	auto& cresult = static_cast<const parse_result&>(result);
	CHECK(!cresult["key"sv]);
	CHECK(cresult.begin() == decltype(cresult.begin()){});
	CHECK(cresult.end() == decltype(cresult.end()){});
	CHECK(cresult.cbegin() == decltype(cresult.cbegin()){});
	CHECK(cresult.cend() == decltype(cresult.cend()){});

	for (auto [k, v] : result)
	{
		(void)k;
		(void)v;
		FAIL("This code should not run");
	}
	for (auto [k, v] : cresult)
	{
		(void)k;
		(void)v;
		FAIL("This code should not run");
	}
}

TEST_CASE("parse_result - iterator types")
{
	static_assert(std::is_same_v<parse_result::iterator, table_iterator>);
	static_assert(std::is_same_v<parse_result::const_iterator, const_table_iterator>);
	static_assert(std::is_same_v<decltype(std::declval<parse_result&>().begin()), table_iterator>);
	static_assert(std::is_same_v<decltype(std::declval<const parse_result&>().begin()), const_table_iterator>);
	static_assert(std::is_same_v<decltype(std::declval<parse_result&>().cbegin()), const_table_iterator>);
}

TEST_CASE("parse_result - default construction is a failure")
{
	const parse_result result;
	CHECK(result.failed());
	CHECK(!result.succeeded());
	CHECK(!result);
	CHECK(result.error().description().empty());
	CHECK(!result.error().source().begin);
	CHECK(!result.error().source().path);
	CHECK(result.begin() == result.end());
}

TEST_CASE("parse_result - construction from a table or an error")
{
	SECTION("table")
	{
		const parse_result result{ table{ { "a", 1 } } };
		REQUIRE(result.succeeded());
		CHECK(result["a"] == 1);
	}

	SECTION("error")
	{
		const parse_result result{ parse_error{ std::string{ "kaboom" }, source_position{ 2, 3 } } };
		REQUIRE(result.failed());
		CHECK(result.error().description() == "kaboom"sv);
		CHECK(result.error().source().begin == source_position{ 2, 3 });
	}
}

TEST_CASE("parse_result - move construction")
{
	SECTION("from a success")
	{
		auto source = "a = 1"_toml;
		REQUIRE(source);

		const parse_result result{ std::move(source) };
		REQUIRE(result.succeeded());
		CHECK(result["a"] == 1);
	}

	SECTION("from a failure")
	{
		auto source = "a = trUe"_toml;
		REQUIRE(!source);
		const std::string description{ source.error().description() };
		const auto begin = source.error().source().begin;

		const parse_result result{ std::move(source) };
		REQUIRE(result.failed());
		CHECK(result.error().description() == description);
		CHECK(result.error().source().begin == begin);
	}
}

TEST_CASE("parse_result - move assignment")
{
	SECTION("success over success")
	{
		auto result = "a = 1"_toml;
		REQUIRE(result);

		result = "b = 2"_toml;
		REQUIRE(result.succeeded());
		CHECK(!result["a"]);
		CHECK(result["b"] == 2);
	}

	SECTION("failure over success")
	{
		auto result = "a = 1"_toml;
		REQUIRE(result);
		auto failure = "a = trUe"_toml;
		REQUIRE(!failure);
		const std::string description{ failure.error().description() };

		result = std::move(failure);
		REQUIRE(result.failed());
		CHECK(result.error().description() == description);
	}

	SECTION("success over failure")
	{
		auto result = "a = trUe"_toml;
		REQUIRE(!result);

		result = "b = 2"_toml;
		REQUIRE(result.succeeded());
		CHECK(result["b"] == 2);
	}

	SECTION("failure over failure")
	{
		auto result = "a = trUe"_toml;
		REQUIRE(!result);
		auto failure = "b = falsE"_toml;
		REQUIRE(!failure);
		const std::string description{ failure.error().description() };
		const auto begin = failure.error().source().begin;
		REQUIRE(result.error().description() != description);
		REQUIRE(result.error().source().begin != begin);

		result = std::move(failure);
		REQUIRE(result.failed());
		CHECK(result.error().description() == description);
		CHECK(result.error().source().begin == begin);
	}
}

TEST_CASE("parse_result - error access")
{
	auto result = "a = trUe"_toml;
	REQUIRE(result.failed());
	const auto& cresult = std::as_const(result);

	static_assert(std::is_same_v<decltype(result.error()), parse_error&>);
	static_assert(std::is_same_v<decltype(std::move(result).error()), parse_error&&>);
	static_assert(std::is_same_v<decltype(cresult.error()), const parse_error&>);

	CHECK(&cresult.error() == &result.error());
	CHECK(&static_cast<parse_error&>(result) == &result.error());
	CHECK(&static_cast<const parse_error&>(cresult) == &result.error());

	const std::string description{ result.error().description() };
	CHECK(!description.empty());

	SECTION("moving out with error() &&")
	{
		const parse_error err{ std::move(result).error() };
		CHECK(err.description() == description);
	}

	SECTION("moving out with a conversion")
	{
		const parse_error err{ static_cast<parse_error&&>(result) };
		CHECK(err.description() == description);
	}
}

TEST_CASE("parse_result - node views")
{
	SECTION("successful parse")
	{
		auto result = toml::parse("a = 1\nb = [2, 3]\n[c]\nd = 4"sv);
		REQUIRE(result);
		const auto& cresult = std::as_const(result);
		auto& tbl			= result.table();

		static_assert(std::is_same_v<decltype(result.at_path("a"sv)), node_view<node>>);
		static_assert(std::is_same_v<decltype(cresult.at_path("a"sv)), node_view<const node>>);
		static_assert(std::is_same_v<decltype(result.at_path(toml::path{})), node_view<node>>);
		static_assert(std::is_same_v<decltype(cresult.at_path(toml::path{})), node_view<const node>>);
		static_assert(std::is_same_v<decltype(result[toml::path{}]), node_view<node>>);
		static_assert(std::is_same_v<decltype(cresult[toml::path{}]), node_view<const node>>);
		static_assert(std::is_same_v<decltype(result["a"sv]), node_view<node>>);
		static_assert(std::is_same_v<decltype(cresult["a"sv]), node_view<const node>>);

		CHECK(result.at_path("b[1]"sv) == 3);
		CHECK(result.at_path("b[1]"sv).node() == tbl["b"][1].node());
		CHECK(cresult.at_path("b[1]"sv).node() == tbl["b"][1].node());

		CHECK(result.at_path(toml::path{ "c.d" }) == 4);
		CHECK(result.at_path(toml::path{ "c.d" }).node() == tbl["c"]["d"].node());
		CHECK(cresult.at_path(toml::path{ "c.d" }).node() == tbl["c"]["d"].node());

		CHECK(result[toml::path{ "b[0]" }] == 2);
		CHECK(result[toml::path{ "b[0]" }].node() == tbl["b"][0].node());
		CHECK(cresult[toml::path{ "b[0]" }].node() == tbl["b"][0].node());

		CHECK(result["a"sv] == 1);
		CHECK(result["a"sv].node() == tbl["a"].node());
		CHECK(cresult["a"sv].node() == tbl["a"].node());

		CHECK(!result.at_path("z"sv));
		CHECK(!cresult[toml::path{ "b[2]" }]);
	}

	SECTION("failed parse")
	{
		auto result = toml::parse("a = 1\nb = trUe"sv);
		REQUIRE(!result);
		const auto& cresult = std::as_const(result);

		CHECK(!result.at_path("a"sv));
		CHECK(!cresult.at_path("a"sv));
		CHECK(!result.at_path(toml::path{ "a" }));
		CHECK(!cresult.at_path(toml::path{ "a" }));
		CHECK(!result[toml::path{ "a" }]);
		CHECK(!cresult[toml::path{ "a" }]);
		CHECK(!result["a"sv]);
		CHECK(!cresult["a"sv]);
	}
}

TEST_CASE("parse_result - stream output")
{
	const auto streamed = [](const auto& val)
	{
		std::ostringstream ss;
		ss << val;
		return std::move(ss).str();
	};

	SECTION("success prints the table")
	{
		const auto result = "a = 1\nb = 'two'"_toml;
		REQUIRE(result);
		CHECK(streamed(result) == "a = 1\nb = 'two'"sv);
	}

	SECTION("failure prints the error")
	{
		const auto result = toml::parse("a = trUe"sv, "bad.toml"sv);
		REQUIRE(!result);
		CHECK(
			streamed(result)
			== "Error while parsing boolean: expected 'true', saw 'trU'\n\t(error occurred at line 1, column 7 of 'bad.toml')"sv);
	}
}

#endif // !TOML_EXCEPTIONS
