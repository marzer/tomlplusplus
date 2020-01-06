#include <toml++/toml.h>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
using namespace std::string_view_literals;

int main(int argc, char** argv)
{
	#ifdef _WIN32
	SetConsoleOutputCP(65001); //UTF-8 console output
	#endif

	auto path = std::string{ argc > 1 ? argv[1] : "example.toml" };
	auto file = std::ifstream{ path };
	if (!file)
	{
		std::cerr << "The file '"sv << path << "' could not be opened for reading."sv << std::endl;
		return 1;
	}
	try
	{
		//const auto table = toml::parse(file); //works but parse_errors would not include the source path

		const auto table = toml::parse(file, std::move(path));

		std::cout << table << std::endl;
	}
	catch (const toml::parse_error& err)
	{
		std::cerr
			<< "Error parsing file '"sv << *err.where().path
			<< "':\n"sv << err.what()
			<< "\n  ("sv << err.where().begin << ")"sv
			<< std::endl;

		return 1;
	}

	return 0;
}
