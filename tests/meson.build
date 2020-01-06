test_sources = [
	'lifetimes.cpp',
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
	'parsing_tables.cpp'
]

disable_exceptions = 'cpp_eh=none'
no_unreleased_features = '-DTOML_UNRELEASED_FEATURES=0'
define_toml_use_char8_strings = '-DTOML_CHAR_8_STRINGS=1'
manually_set_cpp_std = 'cpp_std=none'
cpp20 = '-std=c++2a'

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


strict_char = executable(
	'strict_char',
	test_sources,
	include_directories : inc,
	cpp_args : [ no_unreleased_features ]
)
test('strict_char', strict_char)


strict_char_noexcept = executable(
	'strict_char_noexcept',
	test_sources,
	include_directories : inc,
	override_options : [ disable_exceptions ],
	cpp_args : [ no_unreleased_features ]
)
test('strict_char_noexcept', strict_char_noexcept)


if compiler_supports_char8_strings

	char8 = executable(
		'char8',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, define_toml_use_char8_strings ]
	)
	test('char8', char8)

	char8_noexcept = executable(
		'char8_noexcept',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std, disable_exceptions ],
		cpp_args : [ cpp20, define_toml_use_char8_strings ]
	)
	test('char8_noexcept', char8_noexcept)
	
	strict_char8 = executable(
		'strict_char8',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std ],
		cpp_args : [ cpp20, define_toml_use_char8_strings, no_unreleased_features ]
	)
	test('strict_char8', strict_char8)

	strict_char8_noexcept = executable(
		'strict_char8_noexcept',
		test_sources,
		include_directories : inc,
		override_options : [ manually_set_cpp_std, disable_exceptions ],
		cpp_args : [ cpp20, define_toml_use_char8_strings, no_unreleased_features ]
	)
	test('strict_char8_noexcept', strict_char8_noexcept)

endif