#include <toml17/toml.h>

using namespace std::literals;

//example from the TOML spec document
inline constexpr auto toml_document =
u8R"(# This is a TOML document.

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
])"sv;

int main(int /*argc*/, char** /*argv*/)
{

	const auto table = toml::parse(toml_document);

 //   if (argc < 2)
 //   {
 //       std::cout << "Usage: " << argv[0] << " filename" << std::endl;
 //       return 1;
 //   }

 //   try
 //   {
 //       std::shared_ptr<cpptoml::table> g = cpptoml::parse_file(argv[1]);
 //       std::cout << (*g) << std::endl;
 //   }
 //   catch (const cpptoml::parse_exception& e)
 //   {
 //       std::cerr << "Failed to parse " << argv[1] << ": " << e.what() << std::endl;
 //       return 1;
 //   }
 //   catch (const std::invalid_argument& e)
	//{
	//	std::cerr << "Failed to parse " << argv[1] << ": " << e.what() << std::endl;
	//	return 1;
	//}

    return 0;
}
