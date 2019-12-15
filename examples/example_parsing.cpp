#include <toml++/toml.h>

using namespace std::literals;

inline constexpr auto toml_document =
R"(# This is a TOML document.

title = "TOML Example"

# plain signed integers
int1 = -9223372036854775808
int2 =  9223372036854775807

# hexadecimal with prefix `0x`
hex1 = 0xDEADBEEF
hex2 = 0xdeadbeef
hex3 = 0xdead_beef

# octal with prefix `0o`
oct1 = 0o01234567
oct2 = 0o755 # useful for Unix file permissions

# binary with prefix `0b`
bin1 = 0b11010110

[owner]
name = "Mark Gillard"
dob = 1987-03-16T10:20:00+09:30 # First class dates

[database]
server = "192.168.1.1"
ports = [ -20.0, 8001, 8002 ]
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
])"sv;

int main(int /*argc*/, char** /*argv*/)
{
	const auto table = toml::parse(toml_document);

	return 0;
}
