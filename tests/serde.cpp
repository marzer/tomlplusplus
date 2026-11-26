// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"
#include <map>
#include <set>
#include <vector>

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
		Account(std::string name_, int balance_) : name{ std::move(name_) }, balance{ balance_ }
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

	class Stranger
	{
	  private:
		std::string codename;
		int clearance = 0;

	  public:
		int id = 0;

		Stranger() = default;
		Stranger(int id_, std::string codename_, int clearance_)
			: codename{ std::move(codename_) },
			  clearance{ clearance_ },
			  id{ id_ }
		{}

		const std::string& getCodename() const
		{
			return codename;
		}
		int getClearance() const
		{
			return clearance;
		}
	};

	struct HoldsStranger
	{
		int tag;
		Stranger inner;
	};

	enum class Element
	{
		fire,
		water,
		wind,
	};

	enum Legacy : unsigned char
	{
		legacy_off,
		legacy_on,
	};

	struct Spell
	{
		Element element;
		Legacy legacy;
		std::optional<Element> weakness;
		std::vector<Element> combo;
	};

	struct Containers
	{
		std::vector<int> ports;
		std::vector<Endpoint> mirrors;
		std::set<std::string> tags;
		std::map<std::string, int> limits;
		std::map<std::string, std::vector<Point>> shapes;
	};

	// unwraps the serde_result that deserialize() returns when exceptions are off
	template <typename T, typename... Tag>
	T deserialize(const toml::table& tbl, Tag... tag)
	{
#if TOML_EXCEPTIONS
		return tbl.deserialize<T>(tag...);
#else
		auto res = tbl.deserialize<T>(tag...);
		REQUIRE(res);
		return std::move(res.value());
#endif
	}

	template <typename T, typename... Tag>
	void deserialize_should_fail(const toml::table& tbl, std::string_view key, std::string_view description, Tag... tag)
	{
#if TOML_EXCEPTIONS
		try
		{
			(void)tbl.deserialize<T>(tag...);
			FORCE_FAIL("expected deserialization_error");
		}
		catch (const toml::deserialization_error& e)
		{
			CHECK(e.key() == key);
			CHECK(e.description() == description);
		}
#else
		const auto res = tbl.deserialize<T>(tag...);
		REQUIRE(!res);
		CHECK(!res.has_value());
		CHECK(res.error().key() == key);
		CHECK(res.error().description() == description);
#endif
	}
}

TEST_CASE("serde - round-trip plain struct")
{
	const auto t = toml::serialize(Point{ 3, 4 });
	CHECK(t["x"].value<int>() == 3);
	CHECK(t["y"].value<int>() == 4);

	const auto p = deserialize<Point>(t);
	CHECK(p.x == 3);
	CHECK(p.y == 4);
}

TEST_CASE("serde - rename")
{
	const auto t = toml::serialize(Renamed{ 8080 });
	CHECK(t.contains("listen-port"));
	CHECK(!t.contains("port"));
	CHECK(t["listen-port"].value<int>() == 8080);

	CHECK(deserialize<Renamed>(t).port == 8080);
}

TEST_CASE("serde - transient members are skipped both directions")
{
	const auto t = toml::serialize(WithTransient{ 1, 7 });
	CHECK(t.contains("kept"));
	CHECK(!t.contains("skipped"));

	auto src	 = toml::table{ { "kept", 1 }, { "skipped", 123 } };
	const auto r = deserialize<WithTransient>(src);
	CHECK(r.kept == 1);
	CHECK(r.skipped == 99); // default kept, table value ignored
}

TEST_CASE("serde - nested struct becomes sub-table")
{
	const auto t = toml::serialize(Nested{ "svc", Endpoint{ "localhost", 5432 } });
	REQUIRE(t["primary"].is_table());
	CHECK(t["primary"]["host"].value<std::string>() == "localhost");
	CHECK(t["primary"]["port"].value<int>() == 5432);

	const auto n = deserialize<Nested>(t);
	CHECK(n.title == "svc");
	CHECK(n.primary.host == "localhost");
	CHECK(n.primary.port == 5432);
}

TEST_CASE("serde - flatten inlines sub-struct keys")
{
	const auto t = toml::serialize(Flattened{ "spec", MetadataBlock{ "ada", 7 } });
	CHECK(t.contains("author")); // inlined, not nested under "meta"
	CHECK(t.contains("revision"));
	CHECK(!t.contains("meta"));

	const auto f = deserialize<Flattened>(t);
	CHECK(f.meta.author == "ada");
	CHECK(f.meta.revision == 7);
}

TEST_CASE("serde - optional members")
{
	SECTION("set optional round-trips")
	{
		const auto t = toml::serialize(Optionals{ 1, 42 });
		CHECK(t["maybe"].value<int>() == 42);
		CHECK(deserialize<Optionals>(t).maybe == 42);
	}

	SECTION("empty optional is omitted on serialize")
	{
		const auto t = toml::serialize(Optionals{ 1, std::nullopt });
		CHECK(!t.contains("maybe"));
	}

	SECTION("missing key yields nullopt, not an error")
	{
		const auto t = toml::table{ { "required", 1 } };
		const auto r = deserialize<Optionals>(t);
		CHECK(r.required == 1);
		CHECK(!r.maybe.has_value());
	}
}

TEST_CASE("serde - private members via reflectable annotation")
{
	const auto t = toml::serialize(Account{ "alice", 500 });
	CHECK(t["name"].value<std::string>() == "alice");
	CHECK(t["balance"].value<int>() == 500);

	const auto a = deserialize<Account>(t);
	CHECK(a.getName() == "alice");
	CHECK(a.getBalance() == 500);
}

TEST_CASE("serde - unannotated type exposes only public members")
{
	const auto t = toml::serialize(Stranger{ 7, "fox", 3 });
	CHECK(t.size() == 1);
	CHECK(t["id"].value<int>() == 7);

	const auto src	= toml::table{ { "id", 7 }, { "codename", "fox" }, { "clearance", 3 } };
	const auto back = deserialize<Stranger>(src);
	CHECK(back.id == 7);
	CHECK(back.getCodename().empty());
	CHECK(back.getClearance() == 0);
}

TEST_CASE("serde - allow_private reaches private members of an unannotated type")
{
	const auto t = toml::serialize(Stranger{ 7, "fox", 3 }, toml::allow_private);
	CHECK(t.size() == 3);
	CHECK(t["id"].value<int>() == 7);
	CHECK(t["codename"].value<std::string>() == "fox");
	CHECK(t["clearance"].value<int>() == 3);

	const auto back = deserialize<Stranger>(t, toml::allow_private);
	CHECK(back.id == 7);
	CHECK(back.getCodename() == "fox");
	CHECK(back.getClearance() == 3);
}

TEST_CASE("serde - allow_private does not extend to nested members")
{
	const auto t = toml::serialize(HoldsStranger{ 1, Stranger{ 7, "fox", 3 } }, toml::allow_private);
	REQUIRE(t["inner"].is_table());
	CHECK(t["inner"].as_table()->size() == 1);
	CHECK(t["inner"]["id"].value<int>() == 7);
}

TEST_CASE("serde - allow_private reports a missing private member")
{
	deserialize_should_fail<Stranger>(toml::table{ { "id", 7 }, { "codename", "fox" } },
									  "clearance",
									  "missing required key",
									  toml::allow_private);
}

TEST_CASE("serde - enums map to enumerator names")
{
	const auto t = toml::serialize(Spell{ Element::wind, legacy_on, Element::fire, { Element::water, Element::wind } });
	CHECK(t
		  == toml::table{
			  { "element", "wind" },
			  { "legacy", "legacy_on" },
			  { "weakness", "fire" },
			  { "combo", toml::array{ "water", "wind" } },
		  });

	const auto back = deserialize<Spell>(t);
	CHECK(back.element == Element::wind);
	CHECK(back.legacy == legacy_on);
	CHECK(back.weakness == Element::fire);
	CHECK(back.combo == std::vector{ Element::water, Element::wind });
}

TEST_CASE("serde - enum value with no enumerator is written as its number")
{
	const auto t = toml::serialize(Spell{ static_cast<Element>(42), static_cast<Legacy>(7), {}, {} });
	CHECK(t["element"].value<std::string>() == "42");
	CHECK(t["legacy"].value<std::string>() == "7");
}

TEST_CASE("serde - enum errors")
{
	auto t = toml::serialize(Spell{});

	SECTION("unknown name")
	{
		t.insert_or_assign("element", "earth");
		deserialize_should_fail<Spell>(t, "element", "unknown enumerator");
	}

	SECTION("names are case-sensitive")
	{
		t.insert_or_assign("element", "Fire");
		deserialize_should_fail<Spell>(t, "element", "unknown enumerator");
	}

	SECTION("integer instead of a name")
	{
		t.insert_or_assign("element", 1);
		deserialize_should_fail<Spell>(t, "element", "expected a string");
	}

	SECTION("unknown name in an array")
	{
		t.insert_or_assign("combo", toml::array{ "fire", "earth" });
		deserialize_should_fail<Spell>(t, "combo", "unknown enumerator");
	}
}

TEST_CASE("serde - containers")
{
	Containers c;
	c.ports	  = { 80, 443 };
	c.mirrors = { Endpoint{ "a", 1 }, Endpoint{ "b", 2 } };
	c.tags	  = { "x", "y" };
	c.limits  = { { "cpu", 4 }, { "mem", 16 } };
	c.shapes  = { { "line", { Point{ 0, 0 }, Point{ 1, 2 } } } };

	const auto t = toml::serialize(c);
	CHECK(t
		  == toml::table{
			  { "ports", toml::array{ 80, 443 } },
			  { "mirrors",
				toml::array{ toml::table{ { "host", "a" }, { "port", 1 } },
							 toml::table{ { "host", "b" }, { "port", 2 } } } },
			  { "tags", toml::array{ "x", "y" } },
			  { "limits", toml::table{ { "cpu", 4 }, { "mem", 16 } } },
			  { "shapes",
				toml::table{
					{ "line",
					  toml::array{ toml::table{ { "x", 0 }, { "y", 0 } }, toml::table{ { "x", 1 }, { "y", 2 } } } } } },
		  });

	const auto back = deserialize<Containers>(t);
	CHECK(back.ports == c.ports);
	REQUIRE(back.mirrors.size() == 2);
	CHECK(back.mirrors[1].host == "b");
	CHECK(back.mirrors[1].port == 2);
	CHECK(back.tags == c.tags);
	CHECK(back.limits == c.limits);
	REQUIRE(back.shapes.count("line") == 1);
	REQUIRE(back.shapes.at("line").size() == 2);
	CHECK(back.shapes.at("line")[1].y == 2);
}

TEST_CASE("serde - container errors")
{
	auto t = toml::serialize(Containers{});

	SECTION("array expected")
	{
		t.insert_or_assign("ports", 80);
		deserialize_should_fail<Containers>(t, "ports", "expected an array");
	}

	SECTION("element of the wrong type")
	{
		t.insert_or_assign("ports", toml::array{ 80, "https" });
		deserialize_should_fail<Containers>(t, "ports", "type mismatch");
	}

	SECTION("table expected")
	{
		t.insert_or_assign("limits", toml::array{ 1 });
		deserialize_should_fail<Containers>(t, "limits", "expected a table");
	}

	SECTION("map value of the wrong type")
	{
		t.insert_or_assign("limits", toml::table{ { "cpu", "many" } });
		deserialize_should_fail<Containers>(t, "cpu", "type mismatch");
	}
}

TEST_CASE("serde - deserialize errors")
{
	SECTION("missing required key")
	{
		deserialize_should_fail<Point>(toml::table{ { "x", 3 } }, "y", "missing required key");
	}

	SECTION("type mismatch")
	{
		deserialize_should_fail<Point>(toml::table{ { "x", 3 }, { "y", "nope" } }, "y", "type mismatch");
	}

	SECTION("table expected")
	{
		deserialize_should_fail<Nested>(toml::table{ { "title", "svc" }, { "primary", 1 } },
										"primary",
										"expected a table");
	}
}

#endif // TOML_ENABLE_REFLECTION
