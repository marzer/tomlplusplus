test_sources = [
	'main.cpp',
	'parsing_arrays.cpp',
	'parsing_booleans.cpp',
	'parsing_comments.cpp',
	'parsing_dates_and_times.cpp',
	'parsing_floats.cpp',
	'parsing_integers.cpp',
	'parsing_key_value_pairs.cpp',
	'parsing_spec_example.cpp',
	'parsing_strings.cpp',
	'parsing_tables.cpp',
	'manipulating_arrays.cpp',
	'manipulating_tables.cpp',
	'tests.cpp'
]

disable_exceptions = 'cpp_eh=none'
no_unreleased_features = '-DTOML_UNRELEASED_FEATURES=0'
toml_char8_strings = '-DTOML_CHAR_8_STRINGS=1'
manually_set_cpp_std = 'cpp_std=none'
cpp20 = '-std=c++2a'
use_tloptional = '-DTARTANLLAMA_OPTIONAL'
compiler_supports_char8_strings = compiler.compiles('''
	#include <string_view>
	#include <string>
	using namespace std::string_view_literals;
	std::u8string func()
	{
		return std::u8string{ u8"this is a test."sv };
	}
	''',
	name : 'char8 string check',
	args : [ '-std=c++2a' ]
)

############################################################################
### char
############################################################################


char = executable(
	'char',
	test_sources,
	include_directories : inc
)
test('char', char)


char_noexcept = executable(
	'char_noexcept',
	test_sources,
	include_directories : inc,
	override_options : [ disable_exceptions ]
)
test('char_noexcept', char_noexcept)


char_strict = executable(
	'char_strict',
	test_sources,
	include_directories : inc,
	cpp_args : [ no_unreleased_features ]
)
test('char_strict', char_strict)


char_strict_noexcept = executable(
	'char_strict_noexcept',
	test_sources,
	include_directories : inc,
	override_options : [ disable_exceptions ],
	cpp_args : [ no_unreleased_features ]
)
test('char_strict_noexcept', char_strict_noexcept)


############################################################################
### char w/ tl::optional
############################################################################


char_tlopt = executable(
	'char_tlopt',
	test_sources,
	include_directories : inc,
	cpp_args : [ use_tloptional ]
)
test('char_tlopt', char_tlopt)


char_tlopt_strict = executable(
	'char_tlopt_strict',
	test_sources,
	include_directories : inc,
	cpp_args : [ no_unreleased_features, use_tloptional ]
)
test('char_tlopt_strict', char_tlopt_strict)


if compiler_supports_char8_strings


	############################################################################
	### char8
	############################################################################


	char8 = executable(
		'char8',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, toml_char8_strings ]
	)
	test('char8', char8)

	char8_noexcept = executable(
		'char8_noexcept',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std, disable_exceptions ],
		cpp_args : [ cpp20, toml_char8_strings ]
	)
	test('char8_noexcept', char8_noexcept)
	
	char8_strict = executable(
		'char8_strict',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, toml_char8_strings, no_unreleased_features ]
	)
	test('char8_strict', char8_strict)

	char8_strict_noexcept = executable(
		'char8_strict_noexcept',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std, disable_exceptions ],
		cpp_args : [ cpp20, toml_char8_strings, no_unreleased_features ]
	)
	test('char8_strict_noexcept', char8_strict_noexcept)


	############################################################################
	### char8 w/ tl::optional
	############################################################################


	char8_tlopt = executable(
		'char8_tlopt',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, toml_char8_strings, use_tloptional ]
	)
	test('char8_tlopt', char8_tlopt)

	char8_tlopt_strict = executable(
		'char8_tlopt_strict',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, toml_char8_strings, no_unreleased_features, use_tloptional ]
	)
	test('char8_tlopt_strict', char8_tlopt_strict)


endif
