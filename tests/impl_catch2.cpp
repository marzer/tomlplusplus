#include "../include/toml++/toml_preprocessor.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_ALL_WARNINGS
#define CATCH_CONFIG_RUNNER
#include "catch2.h"
#include <clocale>
TOML_POP_WARNINGS

int main(int argc, char* argv[])
{
	#ifdef _WIN32
		SetConsoleOutputCP(65001);
	#endif
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	return Catch::Session().run(argc, argv);
}
