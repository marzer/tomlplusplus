#include <toml++/toml.h>
#include <iostream>

using namespace std::literals::string_view_literals;

inline constexpr auto toml_document =
R"(# This is a TOML document.

title = "TOML Example"

# plain signed integers
int1 = -9223372036854775808
int2 =  9223372036854775807

# floats
flt1 = 0.00000000001
flt2 = 1e-11
flt3 = 11.0

# hexadecimal with prefix `0x`
hex1 = 0xDEADBEEF
hex2 = 0xdeadbeef
hex3 = 0xdead_beef

# octal with prefix `0o`
oct1 = 0o01234567
oct2 = 0o755 # useful for Unix file permissions

# binary with prefix `0b`
bin1 = 0b11010110 # 214

# hexfloats
hexf = 0x1.2p3 # 9.0 in hex

# local dates and times
tim1 = 07:32:00
tim2 = 00:32:00.100000000
dat1 = 1979-05-27

# offset datetimes 
odt1 = 1979-05-27T07:32:00Z
odt2 = 1979-05-27T00:32:00-07:00
odt3 = 1979-05-27T00:32:00.999999-07:00

[owner]
name = "Mark Gillard"
dob = 1987-03-16 10:20:00+09:30

	[[owner.pets]]
	name = "Brian"
	species = "cat"

	[[owner.pets]]
	name = "Skippy"
	species = "kangaroo"

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
	try
	{
		const auto table = toml::parse(toml_document);

		std::cout << *table << std::endl;
	}
	catch (const toml::parse_error& err)
	{
		std::cerr
			<< "Parse error on line "sv << err.where().begin.line
			<< ", column "sv << err.where().begin.column
			<< ":\n"sv << err.what()
			<< std::endl;

		return 1;
	}

	return 0;
}
