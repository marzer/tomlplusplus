// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	template <typename L, typename R>
	void check_ordering(const L& lhs, const R& rhs, int expected)
	{
		INFO("'" << lhs << "' vs '" << rhs << "'");
		CHECK((lhs == rhs) == (expected == 0));
		CHECK((lhs != rhs) == (expected != 0));
		CHECK((lhs < rhs) == (expected < 0));
		CHECK((lhs <= rhs) == (expected <= 0));
		CHECK((lhs > rhs) == (expected > 0));
		CHECK((lhs >= rhs) == (expected >= 0));
		CHECK((rhs == lhs) == (expected == 0));
		CHECK((rhs != lhs) == (expected != 0));
		CHECK((rhs < lhs) == (expected > 0));
		CHECK((rhs <= lhs) == (expected >= 0));
		CHECK((rhs > lhs) == (expected < 0));
		CHECK((rhs >= lhs) == (expected <= 0));
	}

	void check_source(const toml::key& k, const source_region& expected)
	{
		CHECK(k.source().begin == expected.begin);
		CHECK(k.source().end == expected.end);
		CHECK(k.source().path == expected.path);
	}

	source_region make_test_region()
	{
		return { { 2, 3 }, { 2, 6 }, std::make_shared<const std::string>("keys.toml") };
	}
}

TEST_CASE("keys - construction")
{
	const auto test_region = make_test_region();

	SECTION("default")
	{
		alignas(toml::key) unsigned char storage[sizeof(toml::key)];
		for (auto& byte : storage)
			byte = 0xFF;

		const auto k = ::new (static_cast<void*>(storage)) toml::key;
		CHECK(k->str().empty());
		check_source(*k, source_region{});
		k->~key();
	}

	SECTION("string_view")
	{
		CHECK(toml::key{ "kek"sv }.str() == "kek"sv);
		check_source(toml::key{ "kek"sv }, source_region{});

		const toml::key moved_region{ "kek"sv, source_region{ test_region } };
		CHECK(moved_region.str() == "kek"sv);
		check_source(moved_region, test_region);

		const toml::key copied_region{ "kek"sv, test_region };
		CHECK(copied_region.str() == "kek"sv);
		check_source(copied_region, test_region);
	}

	SECTION("std::string")
	{
		CHECK(toml::key{ std::string{ "kek" } }.str() == "kek"sv);
		check_source(toml::key{ std::string{ "kek" } }, source_region{});

		const toml::key moved_region{ std::string{ "kek" }, source_region{ test_region } };
		CHECK(moved_region.str() == "kek"sv);
		check_source(moved_region, test_region);

		const toml::key copied_region{ std::string{ "kek" }, test_region };
		CHECK(copied_region.str() == "kek"sv);
		check_source(copied_region, test_region);
	}

	SECTION("c-string")
	{
		CHECK(toml::key{ "kek" }.str() == "kek"sv);
		check_source(toml::key{ "kek" }, source_region{});

		const toml::key moved_region{ "kek", source_region{ test_region } };
		CHECK(moved_region.str() == "kek"sv);
		check_source(moved_region, test_region);

		const toml::key copied_region{ "kek", test_region };
		CHECK(copied_region.str() == "kek"sv);
		check_source(copied_region, test_region);
	}

	SECTION("embedded nulls survive string_view and std::string but not c-strings")
	{
		CHECK(toml::key{ "a\0b"sv }.length() == 3u);
		CHECK(toml::key{ std::string{ "a\0b"sv } }.length() == 3u);
		CHECK(toml::key{ "a\0b" }.length() == 1u);
	}

	SECTION("copy and move")
	{
		const toml::key original{ "kek"sv, test_region };

		toml::key copied{ original };
		CHECK(copied.str() == "kek"sv);
		check_source(copied, test_region);

		const toml::key moved{ std::move(copied) };
		CHECK(moved.str() == "kek"sv);
		check_source(moved, test_region);

		toml::key assigned;
		assigned = original;
		CHECK(assigned.str() == "kek"sv);
		check_source(assigned, test_region);
	}
}

TEST_CASE("keys - string operations")
{
	const toml::key k{ "kek"sv };
	const toml::key empty_key{ ""sv };

	SECTION("str")
	{
		CHECK(k.str() == "kek"sv);
		CHECK(empty_key.str() == ""sv);
	}

	SECTION("implicit conversion to std::string_view")
	{
		const std::string_view sv = k;
		CHECK(sv == "kek"sv);
		CHECK(sv.data() == k.str().data());
	}

	SECTION("empty")
	{
		CHECK(!k.empty());
		CHECK(empty_key.empty());
		CHECK(toml::key{}.empty());
	}

	SECTION("data and length")
	{
		CHECK(k.data() == k.str().data());
		CHECK(k.length() == 3u);
		CHECK(std::string_view{ k.data(), k.length() } == "kek"sv);
		CHECK(empty_key.length() == 0u);
	}
}

TEST_CASE("keys - comparison")
{
	SECTION("key with key")
	{
		check_ordering(toml::key{ "a" }, toml::key{ "a" }, 0);
		check_ordering(toml::key{ "a" }, toml::key{ "b" }, -1);
		check_ordering(toml::key{ "a" }, toml::key{ "ab" }, -1);
		check_ordering(toml::key{ "" }, toml::key{ "a" }, -1);
		check_ordering(toml::key{ "B" }, toml::key{ "a" }, -1);
		check_ordering(toml::key{ "kek2" }, toml::key{ "kek10" }, 1);
	}

	SECTION("key with std::string_view")
	{
		check_ordering(toml::key{ "a" }, "a"sv, 0);
		check_ordering(toml::key{ "a" }, "b"sv, -1);
		check_ordering(toml::key{ "a" }, "ab"sv, -1);
		check_ordering(toml::key{ "" }, "a"sv, -1);
		check_ordering(toml::key{ "a" }, ""sv, 1);
		check_ordering(toml::key{ "kek2" }, "kek10"sv, 1);
	}

	SECTION("source regions are ignored")
	{
		const auto test_region = make_test_region();
		const toml::key with_source{ "a"sv, test_region };
		check_ordering(with_source, toml::key{ "a" }, 0);
		check_ordering(with_source, toml::key{ "b"sv, test_region }, -1);
	}
}

TEST_CASE("keys - iteration")
{
	static_assert(std::is_same_v<toml::key::const_iterator, const char*>);
	static_assert(std::is_same_v<toml::key::iterator, toml::key::const_iterator>);

	const toml::key k{ "kek"sv };
	CHECK(k.begin() == k.data());
	CHECK(k.end() == k.data() + k.length());
	CHECK(std::string(k.begin(), k.end()) == "kek");

	std::string chars;
	for (char c : k)
		chars += c;
	CHECK(chars == "kek");

	const toml::key empty_key{};
	CHECK(empty_key.begin() == empty_key.end());
}

TEST_CASE("keys - stream output")
{
	const auto [str, expected] = GENERATE(Catch::Generators::table<std::string_view, std::string_view>({
		{ "kek"sv, "kek"sv },
		{ ""sv, ""sv },
		{ "a.b c"sv, "a.b c"sv },
		{ "\"quoted\""sv, "\"quoted\""sv },
	}));

	std::ostringstream ss;
	ss << toml::key{ str };
	CHECK(ss.str() == expected);
}

TEST_CASE("keys - source regions from parsing")
{
	auto tbl = toml::parse("a = 1\n  b = 2\n    c = 3"sv, "keys.toml"sv);

	std::vector<toml::key> keys;
	for (auto&& kvp : tbl)
		keys.push_back(kvp.first);
	REQUIRE(keys.size() == 3u);

	const auto check_key = [](const toml::key& k, std::string_view name, source_position begin)
	{
		INFO(name);
		CHECK(k == name);
		CHECK(k.source().begin == begin);
		CHECK(k.source().end == source_position{ begin.line, begin.column + 1u });
		REQUIRE(k.source().path);
		CHECK(*k.source().path == "keys.toml"sv);
	};

	check_key(keys[0], "a"sv, { 1, 1 });
	check_key(keys[1], "b"sv, { 2, 3 });
	check_key(keys[2], "c"sv, { 3, 5 });
}

TEST_CASE("keys - traits")
{
	static_assert(is_key<toml::key>);
	static_assert(is_key<const toml::key&>);
	static_assert(is_key<toml::key&&>);
	static_assert(!is_key<std::string>);
	static_assert(!is_key<std::string_view>);

	static_assert(is_key_or_convertible<toml::key>);
	static_assert(is_key_or_convertible<std::string>);
	static_assert(is_key_or_convertible<std::string_view>);
	static_assert(is_key_or_convertible<const char*>);
	static_assert(!is_key_or_convertible<int>);
	static_assert(!is_key_or_convertible<toml::table>);
}
