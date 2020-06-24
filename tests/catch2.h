#pragma once
#include "settings.h"

#if __has_include(<Catch2/single_include/catch2/catch.hpp>)
	#include <Catch2/single_include/catch2/catch.hpp>
#else
	#error Catch2 is missing! You probably need to fetch submodules ("git submodule update --init extern/Catch2")
#endif
