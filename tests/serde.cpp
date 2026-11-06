// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

// the whole file is inert unless the compiler provides C++26 reflection
#if TOML_ENABLE_REFLECTION

TOML_DISABLE_SPAM_WARNINGS;

namespace
{
	struct Point
	{
		int x;
		int y;
	};

	struct Endpoint
	{
		std::string host;
		int port;
	};

	struct Renamed
	{
		[[= toml::rename("listen-port")]] int port;
	};

	struct WithTransient
	{
		int kept;
		[[= toml::transient]] int skipped = 99;
	};

	struct Nested
	{
		std::string title;
		Endpoint primary;
	};

	struct MetadataBlock
	{
		std::string author;
		int revision;
	};

	struct Flattened
	{
		std::string title;
		[[= toml::flatten]] MetadataBlock meta;
	};

	struct Optionals
	{
		int required;
		std::optional<int> maybe;
	};

	class[[= toml::reflectable]] Account
	{
	  private:
		std::string name;
		int balance = 0;

	  public:
		Account() = default;
		Account(std::string name, int balance) : name{ std::move(name) }, balance{ balance }
		{}

		const std::string& getName() const
		{
			return name;
		}
		int getBalance() const
		{
			return balance;
		}
	};
}

TEST_CASE("serde - round-trip plain struct")
{
	const auto t = toml::serialize(Point{ 3, 4 });
	CHECK(t["x"].value<int>() == 3);
	CHECK(t["y"].value<int>() == 4);

	const auto p = t.deserialize<Point>();
	CHECK(p.x == 3);
	CHECK(p.y == 4);
}

TEST_CASE("serde - rename")
{
	const auto t = toml::serialize(Renamed{ 8080 });
	CHECK(t.contains("listen-port"));
	CHECK(!t.contains("port"));
	CHECK(t["listen-port"].value<int>() == 8080);

	CHECK(t.deserialize<Renamed>().port == 8080);
}

TEST_CASE("serde - transient members are skipped both directions")
{
	const auto t = toml::serialize(WithTransient{ 1, 7 });
	CHECK(t.contains("kept"));
	CHECK(!t.contains("skipped"));

	auto src	 = toml::table{ { "kept", 1 }, { "skipped", 123 } };
	const auto r = src.deserialize<WithTransient>();
	CHECK(r.kept == 1);
	CHECK(r.skipped == 99); // default kept, table value ignored
}

TEST_CASE("serde - nested struct becomes sub-table")
{
	const auto t = toml::serialize(Nested{ "svc", Endpoint{ "localhost", 5432 } });
	REQUIRE(t["primary"].is_table());
	CHECK(t["primary"]["host"].value<std::string>() == "localhost");
	CHECK(t["primary"]["port"].value<int>() == 5432);

	const auto n = t.deserialize<Nested>();
	CHECK(n.title == "svc");
	CHECK(n.primary.host == "localhost");
	CHECK(n.primary.port == 5432);
}

TEST_CASE("serde - flatten inlines sub-struct keys")
{
	const auto t = toml::serialize(Flattened{ "spec", meta_block{ "ada", 7 } });
	CHECK(t.contains("author")); // inlined, not nested under "meta"
	CHECK(t.contains("revision"));
	CHECK(!t.contains("meta"));

	const auto f = t.deserialize<Flattened>();
	CHECK(f.meta.author == "ada");
	CHECK(f.meta.revision == 7);
}

TEST_CASE("serde - optional members")
{
	SECTION("set optional round-trips")
	{
		const auto t = toml::serialize(Optionals{ 1, 42 });
		CHECK(t["maybe"].value<int>() == 42);
		CHECK(t.deserialize<Optionals>().maybe == 42);
	}

	SECTION("empty optional is omitted on serialize")
	{
		const auto t = toml::serialize(Optionals{ 1, std::nullopt });
		CHECK(!t.contains("maybe"));
	}

	SECTION("missing key yields nullopt, not an error")
	{
		const auto t = toml::table{ { "required", 1 } };
		const auto r = t.deserialize<Optionals>();
		CHECK(r.required == 1);
		CHECK(!r.maybe.has_value());
	}
}

TEST_CASE("serde - private members via reflectable annotation")
{
	const auto t = toml::serialize(Account{ "alice", 500 });
	CHECK(t["name"].value<std::string>() == "alice");
	CHECK(t["balance"].value<int>() == 500);

	const auto a = t.deserialize<Account>();
	CHECK(a.getName() == "alice");
	CHECK(a.getBalance() == 500);
}

#if TOML_EXCEPTIONS

TEST_CASE("serde - deserialize errors throw deserialization_error")
{
	SECTION("missing required key")
	{
		const auto t = toml::table{ { "x", 3 } }; // y missing
		CHECK_THROWS_AS((void)t.deserialize<Point>(), toml::deserialization_error);
	}

	SECTION("type mismatch reports the offending key")
	{
		const auto t = toml::table{ { "x", 3 }, { "y", "nope" } };
		try
		{
			(void)t.deserialize<Point>();
			FORCE_FAIL("expected deserialization_error");
		}
		catch (const toml::deserialization_error& e)
		{
			CHECK(e.key() == "y");
			CHECK(!e.description().empty());
		}
	}
}

#else

TEST_CASE("serde - deserialize errors surface through serde_result")
{
	SECTION("missing required key")
	{
		const auto t = toml::table{ { "x", 3 } };
		const auto r = t.deserialize<Point>();
		CHECK(!r);
		CHECK(!r.has_value());
		CHECK(r.error().key() == "y");
	}

	SECTION("happy path reports a value")
	{
		const auto t = toml::table{ { "x", 3 }, { "y", 4 } };
		const auto r = t.deserialize<Point>();
		REQUIRE(r);
		CHECK(r.value().x == 3);
		CHECK(r.value().y == 4);
	}
}

#endif // TOML_EXCEPTIONS

#endif // TOML_ENABLE_REFLECTION
