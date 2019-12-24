#include <toml++/toml.h>
#include <iostream>
#include <fstream>

using namespace std::literals::string_view_literals;

int main(int /*argc*/, char** /*argv*/)
{
	try
	{
		const auto table = toml::parse(std::ifstream{ "example.toml" });

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
