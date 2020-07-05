test_sources = [
	'conformance_burntsushi_invalid.cpp',
	'conformance_burntsushi_valid.cpp',
	'conformance_iarna_invalid.cpp',
	'conformance_iarna_valid.cpp',
	'impl_toml.cpp',
	'impl_catch2.cpp',
	'tests.cpp',
	'parsing_floats.cpp',
	'parsing_arrays.cpp',
	'parsing_booleans.cpp',
	'parsing_comments.cpp',
	'parsing_dates_and_times.cpp',
	'parsing_integers.cpp',
	'parsing_key_value_pairs.cpp',
	'parsing_spec_example.cpp',
	'parsing_strings.cpp',
	'parsing_tables.cpp',
	'manipulating_arrays.cpp',
	'manipulating_tables.cpp',
	'manipulating_values.cpp',
	'unicode.cpp',
	'unicode_generated.cpp',
]

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

character_types = ['char', 'char8']
exception_modes = [ true, false ]
unreleased_feature_modes = [ true, false ]
tloptional_modes = [ true, false ]
executables = []
counter = 0
foreach character_type : character_types
	if character_type == 'char8' and not compiler_supports_char8_strings
		continue
	endif
	foreach exceptions : exception_modes
		foreach unreleased_features : unreleased_feature_modes
			foreach tloptional : tloptional_modes

				if tloptional and not (character_type =='char' and unreleased_features and exceptions)
					continue
				endif

				name = character_type
				overrides = []
				args = []

				if character_type =='char8'
					overrides += 'cpp_std=none'
					args += '-DTOML_CHAR_8_STRINGS=1'
					args += '-std=c++2a'
				endif

				if unreleased_features
					args += '-DTOML_UNRELEASED_FEATURES=1'
				else
					name = name + '_strict'
					args += '-DTOML_UNRELEASED_FEATURES=0'
				endif

				if not exceptions
					name = name + '_noexcept'
					overrides += 'cpp_eh=none'
				endif

				if tloptional
					name = name + '_tlopt'
					args += '-DUSE_TARTANLLAMA_OPTIONAL=1'
				endif

				args += '-DTEST_BUILD_ID=@0@'.format(counter)

				if compiler.get_id() == 'gcc'
					args += '-Wno-padded'
					args += '-Wno-float-equal'
				endif

				if compiler.get_id() == 'clang'
					args += '-Wno-padded'
					args += '-Wno-float-equal'
					args += '-Wno-double-promotion'
				endif

				executables += [[
					name,
					executable(
						name,
						test_sources,
						include_directories : inc,
						cpp_args : args,
						override_options : overrides
					)
				]]

				counter = counter + 1

			endforeach # tloptional_modes
		endforeach # unreleased_feature_modes
	endforeach # exception_modes
endforeach # character_type

locales = [
	'C',
	'en_US.utf8',
	'ja_JP.utf8',
	'it_IT.utf8',
	'tr_TR.utf8',
	'fi_FI.utf8',
	'fr_FR.utf8',
	'zh_CN.utf8',
	'de_DE.utf8'
]

foreach locale : locales
	foreach executable : executables
		test(locale + '_' + executable[0], executable[1], env : ['LC_ALL=' + locale])
	endforeach
endforeach
