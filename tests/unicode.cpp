#include "tests.h"
#include "unicode.h"
using namespace toml::impl;

TEST_CASE("unicode - is_ascii_letter")
{
	constexpr auto fn = is_ascii_letter;
	REQUIRE(not_in(fn,	{	U'\0',		U'@'		}));
	REQUIRE(in_only(fn,	{	U'A',		U'Z'		}));
	REQUIRE(not_in(fn,	{	U'[',		U'`'		}));
	REQUIRE(in_only(fn,	{	U'a',		U'z'		}));
	REQUIRE(not_in(fn,	{	U'{',		unimax		}));
}

TEST_CASE("unicode - is_ascii_whitespace")
{
	constexpr auto fn = is_ascii_whitespace;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0008'	}));
	REQUIRE(in_only(fn,		U'\t'					));
	REQUIRE(not_in(fn,	{	U'\n',		U'\u001F'	}));
	REQUIRE(in_only(fn,		U' '					));
	REQUIRE(not_in(fn,	{	U'!',		unimax		}));
}

TEST_CASE("unicode - is_ascii_line_break")
{
	constexpr auto fn = is_ascii_line_break<true>;
	REQUIRE(not_in(fn,	{	U'\0',		U'\t'		}));
	REQUIRE(in_only(fn,	{	U'\n',		U'\r'		}));
	REQUIRE(not_in(fn,	{	U'\u000E',	unimax		}));
}

TEST_CASE("unicode - is_decimal_digit")
{
	constexpr auto fn = is_decimal_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in_only(fn,	{	U'0',		U'9'		}));
	REQUIRE(not_in(fn,	{	U':',		unimax		}));
}

TEST_CASE("unicode - is_string_delimiter")
{
	constexpr auto fn = is_string_delimiter;
	REQUIRE(not_in(fn,	{	U'\0',		U'!'		}));
	REQUIRE(in_only(fn,		U'"'					));
	REQUIRE(not_in(fn,	{	U'#',		U'&'		}));
	REQUIRE(in_only(fn,		U'\''					));
	REQUIRE(not_in(fn,	{	U'(',		unimax		}));
}

TEST_CASE("unicode - is_unicode_whitespace")
{
	constexpr auto fn = is_unicode_whitespace;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u009F'	}));
	REQUIRE(in_only(fn,		U'\u00A0'				));
	REQUIRE(not_in(fn,	{	U'\u00A1',	U'\u167F'	}));
	REQUIRE(in_only(fn,		U'\u1680'				));
	REQUIRE(not_in(fn,	{	U'\u1681',	U'\u1FFF'	}));
	REQUIRE(in_only(fn,	{	U'\u2000',	U'\u200A'	}));
	REQUIRE(not_in(fn,	{	U'\u200B',	U'\u202E'	}));
	REQUIRE(in_only(fn,		U'\u202F'				));
	REQUIRE(not_in(fn,	{	U'\u2030',	U'\u205E'	}));
	REQUIRE(in_only(fn,		U'\u205F'				));
	REQUIRE(not_in(fn,	{	U'\u2060',	U'\u2FFF'	}));
	REQUIRE(in_only(fn,		U'\u3000'				));
	REQUIRE(not_in(fn,	{	U'\u3001',	unimax		}));
}


TEST_CASE("unicode - is_unicode_line_break")
{
	constexpr auto fn = is_unicode_line_break;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0084'	}));
	REQUIRE(in_only(fn,		U'\u0085'				));
	REQUIRE(not_in(fn,	{	U'\u0086',	U'\u2027'	}));
	REQUIRE(in_only(fn,	{	U'\u2028',	U'\u2029'	}));
	REQUIRE(not_in(fn,	{	U'\u202A',	unimax		}));
}

TEST_CASE("unicode - is_unicode_surrogate")
{
	constexpr auto fn = is_unicode_surrogate;
	REQUIRE(not_in(fn,	{	U'\0',		0xD7FFu		}));
	REQUIRE(in_only(fn,	{	0xD800u,	0xDFFF		}));
	REQUIRE(not_in(fn,	{	0xE000,		unimax		}));
}
