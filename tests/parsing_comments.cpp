#include "tests.h"

TEST_CASE("parsing - comments")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(# This is a full-line comment
			key = "value"  # This is a comment at the end of a line
			another = "# This is not a comment"
		)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 2);
			CHECK(tbl[S("key")] == S("value"sv));
			CHECK(tbl[S("another")] == S("# This is not a comment"sv));
		}
	);

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(# this = "looks like a KVP but is commented out)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 0);
		}
	);
	
	#if TOML_LANG_AT_LEAST(1, 0, 0)
	{
		// toml/issues/567 (disallow non-TAB control characters in comments)
		// 00 - 08
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0000 some trailing garbage"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0001"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0002"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0003"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0004"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0005"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0006"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0007"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0008"sv));
		
		// skip tab and line breaks (real and otherwise)
		// \u0009 is \t
		// \u000A is \n
		// \u000B is \v (vertical tab)
		// \u000C is \f (form feed)
		// \u000D is \r
		
		// 0E - 1F
		parsing_should_fail(FILE_LINE_ARGS, S("# \u000E"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u000F"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0010"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0011"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0012"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0013"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0014"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0015"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0016"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0017"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0018"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u0019"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001A"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001B"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001C"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001D"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001E"sv));
		parsing_should_fail(FILE_LINE_ARGS, S("# \u001F"sv));
		// 7F
		parsing_should_fail(FILE_LINE_ARGS, S("# \u007F"sv));
	}
	#else
	{
		parsing_should_succeed(FILE_LINE_ARGS, S(
			"## 00 - 08"
			"# \u0000  "
			"# \u0001  "
			"# \u0002  "
			"# \u0003  "
			"# \u0004  "
			"# \u0005  "
			"# \u0006  "
			"# \u0007  "
			"# \u0008  "
			"## 0A - 1F"
			"# \u000A  "
			"# \u000B  "
			"# \u000C  "
			"# \u000D  "
			"# \u000E  "
			"# \u000F  "
			"# \u0010  "
			"# \u0011  "
			"# \u0012  "
			"# \u0013  "
			"# \u0014  "
			"# \u0015  "
			"# \u0016  "
			"# \u0017  "
			"# \u0018  "
			"# \u0019  "
			"# \u001A  "
			"# \u001B  "
			"# \u001C  "
			"# \u001D  "
			"# \u001E  "
			"# \u001F  "
			"## 7F	   "
			"# \u007F  "sv
		));
	}
	#endif
}
