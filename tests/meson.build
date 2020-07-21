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
	'windows_compat.cpp'
]

compiler_supports_cpp20 = compiler.links('''
	#include <version>
	#include <string>
	#include <iostream>

	int main()
	{
		std::string s = "kek";
		std::cout << s << std::endl;
		return 0;
	}
	''',
	name : 'supports C++20',
	args : [ '-std=c++2a' ]
)

compiler_supports_char8 = compiler_supports_cpp20 and compiler.links('''
	#include <version>
	#include <string_view>
	#include <string>
	#include <type_traits>
	using namespace std::string_view_literals;

	#ifndef __cpp_lib_char8_t
		#error oh noes
	#endif

	static_assert(!std::is_same_v<char, char8_t>);
	static_assert(!std::is_same_v<std::string, std::u8string>);

	std::u8string func()
	{
		return std::u8string{ u8"this is a test."sv };
	}

	int main()
	{
		return 0;
	}
	''',
	name : 'supports char8_t',
	args : [ '-std=c++2a', '-fchar8_t' ]
)

compiler_supports_consteval = compiler_supports_cpp20 and compiler.compiles('''

	consteval int kek() noexcept
	{
		return 42;
	}

	int main()
	{
		return kek();
	}
	''',
	name : 'supports consteval',
	args : [ '-std=c++2a' ]
)

compiler_supports_float16_args = []
if compiler.get_id() == 'gcc'
	compiler_supports_float16_args += '-mfp16-format=ieee'
endif
compiler_supports_float16 = compiler.links('''
	int main()
	{
		static_assert(sizeof(_Float16) == 2);
		_Float16 f = static_cast<_Float16>(1);
		return 0;
	}
	''',
	name : 'supports float16',
	args : compiler_supports_float16_args
)

compiler_supports_fast_math = compiler.links('''
	#include <cmath>
	#include <iostream>
	int main()
	{
		std::cout << std::exp2(2.0) << std::pow(2.0, 3.0) << std::endl;
		return 0;
	}
	''',
	name : 'supports fast-math',
	args : [ '-ffast-math', '-ffp-contract=fast' ]
)

fast_math_modes = [ false, true ]
exception_modes = [ true, false ]
unreleased_feature_modes = [ false, true ]
cpp20_modes = [ false, true ]
executables = []
counter = 0
foreach cpp20 : cpp20_modes
	if cpp20 and not compiler_supports_cpp20
		continue
	endif
	foreach unrel : unreleased_feature_modes
		foreach fast_math : fast_math_modes
			if fast_math and not compiler_supports_fast_math
				continue
			endif
			foreach exceptions : exception_modes

					name = ''
					overrides = []
					args = []

					if cpp20
						name = 'cpp20'
						overrides += 'cpp_std=none'
						args += '-std=c++2a'

						if compiler_supports_char8
							args += '-fchar8_t'
						endif
					else
						name = 'cpp17'
					endif

					if not exceptions
						name = name + '_noexcept'
						overrides += 'cpp_eh=none'
					endif

					if fast_math
						name = name + '_fastmath'
						if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
							args += '-ffast-math'
							args += '-ffp-contract=fast'
						endif
					endif

					if unrel
						name = name + '_unrel'
						args += '-DTOML_UNRELEASED_FEATURES=1'
					else
						args += '-DTOML_UNRELEASED_FEATURES=0'
					endif

					if counter % 6 == 3
						args += '-DTOML_ALL_INLINE=1'
					endif
					if counter % 2 == 1
						args += '-DUSE_SINGLE_HEADER=1'
					endif
					if counter % 4 == 2 and exceptions
						args += '-DUSE_TARTANLLAMA_OPTIONAL=1'
						name = name + '_tlopt'
					endif
					
					if compiler_supports_float16
						if compiler.get_id() == 'gcc'
							args += '-mfp16-format=ieee'
						endif
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

			endforeach # exceptions
		endforeach # fast_math
	endforeach # strict
endforeach # cpp20

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

foreach executable : executables
	foreach locale : locales
		test(executable[0] + ' (' + locale + ')', executable[1], env : ['LC_ALL=' + locale])
	endforeach
endforeach
