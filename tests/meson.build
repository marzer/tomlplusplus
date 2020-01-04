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


test_char = executable(
	'test_char',
	test_sources,
	include_directories : inc
)
test('test_char', test_char)


test_char_noexcept = executable(
	'test_char_noexcept',
	test_sources,
	include_directories : inc,
	override_options : [ disable_exceptions ]
)
test('test_char_noexcept', test_char_noexcept)


test_strict_char = executable(
	'test_strict_char',
	test_sources,
	include_directories : inc,
	cpp_args : [ no_unreleased_features ]
)
test('test_strict_char', test_strict_char)


test_strict_char_noexcept = executable(
	'test_strict_char_noexcept',
	test_sources,
	include_directories : inc,
	override_options : [ disable_exceptions ],
	cpp_args : [ no_unreleased_features ]
)
test('test_strict_char_noexcept', test_strict_char_noexcept)



# char8_t is not supported until gcc9, yo.
# test_char8 = executable(
#	'test_char8',
# 	test_sources,
# 	include_directories : inc,
# 	cpp_args : ['-DTOML_CHAR_8_STRINGS=1', '-fchar8_t']
#)
# 					
# test_char8_noexcept = executable(
#	'test_char8_noexcept',
# 	test_sources,
# 	include_directories : inc,
# 	override_options : [ disable_exceptions ],
# 	cpp_args : ['-DTOML_CHAR_8_STRINGS=1', '-fchar8_t']
#)


