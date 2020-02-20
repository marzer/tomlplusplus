#include "tests.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_INIT_WARNINGS

TEST_CASE("parsing - TOML spec example")
{
	static constexpr auto toml_text =
S(R"(# This is a TOML document.

title = "TOML Example"

[owner]
name = "Tom Preston-Werner"
dob = 1979-05-27T07:32:00-08:00 # First class dates

[database]
server = "192.168.1.1"
ports = [ 8001, 8001, 8002 ]
connection_max = 5000
enabled = true

[servers]

  # Indentation (tabs and/or spaces) is allowed but not required
  [servers.alpha]
  ip = "10.0.0.1"
  dc = "eqdc10"

  [servers.beta]
  ip = "10.0.0.2"
  dc = "eqdc10"

[clients]
data = [ ["gamma", "delta"], [1, 2] ]

# Line breaks are OK when inside arrays
hosts = [
  "alpha",
  "omega"
])"sv);

	parsing_should_succeed(toml_text, [](table&& tbl) noexcept
	{
		CHECK(tbl.size() == 5);

		CHECK(tbl[S("title")] == S("TOML Example"sv));

		CHECK(tbl[S("owner")]);
		CHECK(tbl[S("owner")].as<table>());
		CHECK(tbl[S("owner")][S("name")] == S("Tom Preston-Werner"sv));
		const auto dob = date_time{ { 1979, 5, 27 }, { 7, 32 }, { -8, 0 } };
		CHECK(tbl[S("owner")][S("dob")] == dob);

		CHECK(tbl[S("database")].as<table>());
		CHECK(tbl[S("database")][S("server")] == S("192.168.1.1"sv));
		const auto ports = { 8001, 8001, 8002 };
		CHECK(tbl[S("database")][S("ports")] == ports);
		CHECK(tbl[S("database")][S("connection_max")] == 5000);
		CHECK(tbl[S("database")][S("enabled")] == true);

		CHECK(tbl[S("servers")].as<table>());
		CHECK(tbl[S("servers")][S("alpha")].as<table>());
		CHECK(tbl[S("servers")][S("alpha")][S("ip")] == S("10.0.0.1"sv));
		CHECK(tbl[S("servers")][S("alpha")][S("dc")] == S("eqdc10"sv));
		CHECK(tbl[S("servers")][S("beta")].as<table>());
		CHECK(tbl[S("servers")][S("beta")][S("ip")] == S("10.0.0.2"sv));
		CHECK(tbl[S("servers")][S("beta")][S("dc")] == S("eqdc10"sv));

		CHECK(tbl[S("clients")].as<table>());
		REQUIRE(tbl[S("clients")][S("data")].as<array>());
		CHECK(tbl[S("clients")][S("data")].as<array>()->size() == 2);
		REQUIRE(tbl[S("clients")][S("data")][0].as<array>());
		CHECK(tbl[S("clients")][S("data")][0].as<array>()->size() == 2);
		CHECK(tbl[S("clients")][S("data")][0][0] == S("gamma"sv));
		CHECK(tbl[S("clients")][S("data")][0][1] == S("delta"sv));
		REQUIRE(tbl[S("clients")][S("data")][1].as<array>());
		CHECK(tbl[S("clients")][S("data")][1].as<array>()->size() == 2);
		CHECK(tbl[S("clients")][S("data")][1][0] == 1);
		CHECK(tbl[S("clients")][S("data")][1][1] == 2);
		REQUIRE(tbl[S("clients")][S("hosts")].as<array>());
		CHECK(tbl[S("clients")][S("hosts")].as<array>()->size() == 2);
		CHECK(tbl[S("clients")][S("hosts")][0] == S("alpha"sv));
		CHECK(tbl[S("clients")][S("hosts")][1] == S("omega"sv));
	});
}

TOML_POP_WARNINGS
