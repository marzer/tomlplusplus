#include "tests.h"

TEST_CASE("parsing - comments")
{
	parsing_should_succeed(S(R"(
# This is a full-line comment
key = "value"  # This is a comment at the end of a line
another = "# This is not a comment"
)"sv),
		[](table&& tbl) noexcept
		{
			CHECK(tbl.size() == 2);
			CHECK(tbl[S("key")] == S("value"sv));
			CHECK(tbl[S("another")] == S("# This is not a comment"sv));
		}
	);
}
