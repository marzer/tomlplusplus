#include "tests.h"

TEST_CASE("parsing - booleans")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
			bool1 = true
			bool2 = false
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("bool1")] == true);
			CHECK(tbl[S("bool2")] == false);
		}
	);

	// "Always lowercase."
	parsing_should_fail(FILE_LINE_ARGS, S("bool = True"sv));
	parsing_should_fail(FILE_LINE_ARGS, S("bool = TRUE"sv));
	parsing_should_fail(FILE_LINE_ARGS, S("bool = tRUE"sv));
	parsing_should_fail(FILE_LINE_ARGS, S("bool = False"sv));
	parsing_should_fail(FILE_LINE_ARGS, S("bool = FALSE"sv));
	parsing_should_fail(FILE_LINE_ARGS, S("bool = fALSE"sv));

	// value tests
	parse_expected_value(FILE_LINE_ARGS, " true",  true);
	parse_expected_value(FILE_LINE_ARGS, "false", false);
}
