#include "tests.h"
#include "unicode.h"
using namespace toml::impl;

TEST_CASE("unicode - is_ascii_letter")
{
	static constexpr auto fn = is_ascii_letter;
	REQUIRE(not_in(fn,	{	U'\0',		U'@'		}));
	REQUIRE(in_only(fn,	{	U'A',		U'Z'		}));
	REQUIRE(not_in(fn,	{	U'[',		U'`'		}));
	REQUIRE(in_only(fn,	{	U'a',		U'z'		}));
	REQUIRE(not_in(fn,	{	U'{',		unimax		}));
}

TEST_CASE("unicode - is_ascii_whitespace")
{
	static constexpr auto fn = is_ascii_whitespace;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0008'	}));
	REQUIRE(in_only(fn,		U'\t'					));
	REQUIRE(not_in(fn,	{	U'\n',		U'\u001F'	}));
	REQUIRE(in_only(fn,		U' '					));
	REQUIRE(not_in(fn,	{	U'!',		unimax		}));
}

TEST_CASE("unicode - is_ascii_line_break")
{
	static constexpr auto fn = is_ascii_line_break<true>;
	REQUIRE(not_in(fn,	{	U'\0',		U'\t'		}));
	REQUIRE(in_only(fn,	{	U'\n',		U'\r'		}));
	REQUIRE(not_in(fn,	{	U'\u000E',	unimax		}));
}

TEST_CASE("unicode - is_binary_digit")
{
	static constexpr auto fn = is_binary_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'1'		}));
	REQUIRE(not_in(fn,	{	U'2',		unimax		}));
}

TEST_CASE("unicode - is_octal_digit")
{
	static constexpr auto fn = is_octal_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'7'		}));
	REQUIRE(not_in(fn,	{	U'8',		unimax		}));
}

TEST_CASE("unicode - is_decimal_digit")
{
	static constexpr auto fn = is_decimal_digit;
	REQUIRE(not_in(fn,	{	U'\0',		U'/'		}));
	REQUIRE(in(fn,		{	U'0',		U'9'		}));
	REQUIRE(not_in(fn,	{	U':',		unimax		}));
}

TEST_CASE("unicode - hex_to_dec")
{
	REQUIRE(hex_to_dec(U'0') == 0x0u);
	REQUIRE(hex_to_dec(U'1') == 0x1u);
	REQUIRE(hex_to_dec(U'2') == 0x2u);
	REQUIRE(hex_to_dec(U'3') == 0x3u);
	REQUIRE(hex_to_dec(U'4') == 0x4u);
	REQUIRE(hex_to_dec(U'5') == 0x5u);
	REQUIRE(hex_to_dec(U'6') == 0x6u);
	REQUIRE(hex_to_dec(U'7') == 0x7u);
	REQUIRE(hex_to_dec(U'8') == 0x8u);
	REQUIRE(hex_to_dec(U'9') == 0x9u);
	REQUIRE(hex_to_dec(U'A') == 0xAu);
	REQUIRE(hex_to_dec(U'B') == 0xBu);
	REQUIRE(hex_to_dec(U'C') == 0xCu);
	REQUIRE(hex_to_dec(U'D') == 0xDu);
	REQUIRE(hex_to_dec(U'E') == 0xEu);
	REQUIRE(hex_to_dec(U'F') == 0xFu);
	REQUIRE(hex_to_dec(U'a') == 0xau);
	REQUIRE(hex_to_dec(U'b') == 0xbu);
	REQUIRE(hex_to_dec(U'c') == 0xcu);
	REQUIRE(hex_to_dec(U'd') == 0xdu);
	REQUIRE(hex_to_dec(U'e') == 0xeu);
	REQUIRE(hex_to_dec(U'f') == 0xfu);
}

TEST_CASE("unicode - is_string_delimiter")
{
	static constexpr auto fn = is_string_delimiter;
	REQUIRE(not_in(fn,	{	U'\0',		U'!'		}));
	REQUIRE(in_only(fn,		U'"'					));
	REQUIRE(not_in(fn,	{	U'#',		U'&'		}));
	REQUIRE(in_only(fn,		U'\''					));
	REQUIRE(not_in(fn,	{	U'(',		unimax		}));
}

TEST_CASE("unicode - is_unicode_whitespace")
{
	static constexpr auto fn = is_unicode_whitespace;
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
	static constexpr auto fn = is_unicode_line_break;
	REQUIRE(not_in(fn,	{	U'\0',		U'\u0084'	}));
	REQUIRE(in_only(fn,		U'\u0085'				));
	REQUIRE(not_in(fn,	{	U'\u0086',	U'\u2027'	}));
	REQUIRE(in_only(fn,	{	U'\u2028',	U'\u2029'	}));
	REQUIRE(not_in(fn,	{	U'\u202A',	unimax		}));
}

TEST_CASE("unicode - is_unicode_surrogate")
{
	static constexpr auto fn = is_unicode_surrogate;
	REQUIRE(not_in(fn,	{	U'\0',		0xD7FFu		}));
	REQUIRE(in_only(fn,	{	0xD800u,	0xDFFF		}));
	REQUIRE(not_in(fn,	{	0xE000,		unimax		}));
}
