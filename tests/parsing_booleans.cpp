#include "tests.h"

TEST_CASE("parsing - booleans")
{
	parsing_should_succeed(S(R"(
bool1 = true
bool2 = false
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl[S("bool1")] == true);
			CHECK(tbl[S("bool2")] == false);
		}
	);

	// "Always lowercase."
	parsing_should_fail(S("bool = True"sv));
	parsing_should_fail(S("bool = TRUE"sv));
	parsing_should_fail(S("bool = tRUE"sv));
	parsing_should_fail(S("bool = False"sv));
	parsing_should_fail(S("bool = FALSE"sv));
	parsing_should_fail(S("bool = fALSE"sv));

	// value tests
	parse_expected_value(" true",  true);
	parse_expected_value("false", false);
}
