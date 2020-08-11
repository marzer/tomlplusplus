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
	'user_feedback.cpp',
	'windows_compat.cpp'
]

# compiler argument references:
# msvc: https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=vs-2019
# intel and intel-cl: https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-oneapi-dev-guide-and-reference/top/compiler-reference/compiler-options/alphabetical-list-of-compiler-options.html
# gcc:
# clang:

#######################################################################################################################
# c++ 20 check
#######################################################################################################################

compiler_supports_cpp20_args = []
if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
	compiler_supports_cpp20_args += '-std=c++2a'
elif compiler.get_id() == 'intel-cl'
	compiler_supports_cpp20_args += '/Qstd=c++2a'
elif compiler.get_id() == 'msvc'
	compiler_supports_cpp20_args += '/std=c++latest'
endif
compiler_supports_cpp20 = compiler.links('''
	#include <version>
	#include <string>
	#include <iostream>

	int main()
	{
		std::string s = "kek";
		std::cout << s << "\n";
		return 0;
	}
	''',
	name : 'supports c++20',
	args : compiler_supports_cpp20_args
)

#######################################################################################################################
# char8_t check
#######################################################################################################################

compiler_supports_char8_args = []
compiler_supports_char8_args += compiler_supports_cpp20_args
if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
	compiler_supports_char8_args += '-fchar8_t'
endif
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
	args : compiler_supports_char8_args
)

#######################################################################################################################
# consteval check
#######################################################################################################################

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
	args : compiler_supports_cpp20_args
)

#######################################################################################################################
# __fp16 and _Float16 checks
#######################################################################################################################

float_16_preprocessor_single_check_template = '''
	#ifndef @0@
		#error @0@ wasn't defined!
	#else
		#pragma message("@0@: " MAKE_STRING(@0@))
	#endif
	#if @0@ != @1@
		#error @0@ was not @1@!
	#endif

'''
float_16_preprocessor_checks = '''
	#define MAKE_STRING(s)		MAKE_STRING_1(s)
	#define MAKE_STRING_1(s)	#s
	''' + float_16_preprocessor_single_check_template.format('__FLT_RADIX__', '2')						\
	+ float_16_preprocessor_single_check_template.format('__FLT16_MANT_DIG__', '11')					\
	+ float_16_preprocessor_single_check_template.format('__FLT16_DIG__', '3')							\
	+ float_16_preprocessor_single_check_template.format('__FLT16_MIN_EXP__', '-13')					\
	+ float_16_preprocessor_single_check_template.format('__FLT16_MIN_10_EXP__', '-4')					\
	+ float_16_preprocessor_single_check_template.format('__FLT16_MAX_EXP__', '16')						\
	+ float_16_preprocessor_single_check_template.format('__FLT16_MAX_10_EXP__', '4')

compiler_disable_optimizations = ''
if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
	compiler_disable_optimizations = '-O0'
elif compiler.get_id() == 'msvc' or compiler.get_id() == 'intel-cl'
	compiler_disable_optimizations = '/Od'
endif

compiler_supports_float16_args = [ compiler_disable_optimizations ]
if compiler.get_id() == 'gcc'
	compiler_supports_float16_args += '-mfp16-format=ieee'
	compiler_supports_float16_args += '-fmax-errors=1'
elif compiler.get_id() == 'clang'
	compiler_supports_float16_args += '-ferror-limit=1'
endif
compiler_supports_fp16 = compiler.links('''
	@0@

	int main()
	{
		static_assert(sizeof(__fp16) == 2);
		__fp16 f = static_cast<__fp16>(1);
		const auto f2 = static_cast<float>(f);
		const auto f3 = static_cast<__fp16>(0.2L);
		return 0;
	}
	'''.format(float_16_preprocessor_checks),
	name : 'supports __fp16',
	args : compiler_supports_float16_args
)
compiler_supports_float16 = compiler.links('''
	@0@

	int main()
	{
		static_assert(sizeof(_Float16) == 2);
		_Float16 f = static_cast<_Float16>(1);
		const auto f2 = static_cast<float>(f);
		const auto f3 = static_cast<_Float16>(0.2L);
		return 0;
	}
	'''.format(float_16_preprocessor_checks),
	name : 'supports _Float16',
	args : compiler_supports_float16_args
)

#######################################################################################################################
# int128 check
#######################################################################################################################

compiler_supports_int128 = compiler.links('''
	#ifndef __SIZEOF_INT128__
		#error __SIZEOF_INT128__ wasn't defined!
	#endif

	#include <cstdint>

	int main()
	{
		static_assert(__SIZEOF_INT128__ == 16);
		static_assert(sizeof(__int128_t) == 16);
		static_assert(sizeof(__uint128_t) == 16);
		__int128_t i = static_cast<__int128_t>(1);
		const auto i2 = static_cast<int64_t>(i);
		const auto i3 = static_cast<int32_t>(i);
		return 0;
	}
	''',
	name : 'supports __int128_t',
	args : [ compiler_disable_optimizations ]
)

#######################################################################################################################
# float128 check
#######################################################################################################################

compiler_supports_float128 = compiler.links('''
	#ifndef __SIZEOF_FLOAT128__
		#error __SIZEOF_FLOAT128__ wasn't defined!
	#endif
	#ifndef __FLT128_MANT_DIG__
		#error __FLT128_MANT_DIG__ wasn't defined!
	#endif
	#ifndef __LDBL_MANT_DIG__
		#error __LDBL_MANT_DIG__ wasn't defined!
	#endif
	#if __FLT128_MANT_DIG__ <= __LDBL_MANT_DIG__
		#error __FLT128_MANT_DIG__ was <= __LDBL_MANT_DIG__
	#endif
	int main()
	{
		static_assert(__SIZEOF_FLOAT128__ == 16);
		static_assert(sizeof(__float128) == 16);
		__float128 f = static_cast<__float128>(1);
		const auto f2 = static_cast<long double>(f);
		const auto f3 = static_cast<double>(f);
		return 0;
	}
	''',
	name : 'supports __float128',
	args : [ compiler_disable_optimizations ]
)

#######################################################################################################################
# fast math check
#######################################################################################################################

compiler_supports_fast_math_args = []
if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
	compiler_supports_fast_math_args += '-ffast-math'
	compiler_supports_fast_math_args += '-ffp-contract=fast'
elif compiler.get_id() == 'msvc' or compiler.get_id() == 'intel-cl'
	compiler_supports_fast_math_args += '/fp:fast'
endif
compiler_supports_fast_math = compiler.links('''
	#include <cmath>
	#include <iostream>
	int main()
	{
		std::cout << std::exp2(2.0) << std::pow(2.0, 3.0) << "\n";
		return 0;
	}
	''',
	name : 'supports fast-math',
	args : compiler_supports_fast_math_args
)

#######################################################################################################################
# do the thing!
#######################################################################################################################

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

					if exceptions
						args += '-DSHOULD_HAVE_EXCEPTIONS=1'
					else
						name = name + '_noexcept'
						overrides += 'cpp_eh=none'
						args += '-DSHOULD_HAVE_EXCEPTIONS=0'
					endif

					if fast_math
						name = name + '_fastmath'
						if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
							args += '-ffast-math'
							args += '-ffp-contract=fast'
						endif
					endif

					if compiler_supports_float16 or compiler_supports_fp16
						if compiler.get_id() == 'gcc'
							args += '-mfp16-format=ieee'
						endif
						if compiler_supports_fp16
							args += '-DSHOULD_HAVE_FP16=1'
						endif
						if compiler_supports_float16
							args += '-DSHOULD_HAVE_FLOAT16=1'
						endif
					endif

					if compiler_supports_int128
						args += '-DSHOULD_HAVE_INT128=1'
					endif
			
					if compiler_supports_float128
						args += '-DSHOULD_HAVE_FLOAT128=1'
					endif

					if compiler_supports_float16 or compiler_supports_int128 or compiler_supports_float128
						if compiler.get_id() == 'gcc'
							args += '-fext-numeric-literals'
						endif
					endif

					if unrel
						name = name + '_unrel'
						args += '-DTOML_UNRELEASED_FEATURES=1'
					else
						args += '-DTOML_UNRELEASED_FEATURES=0'
					endif

					if counter % 6 == 3
						args += '-DTOML_HEADER_ONLY=1'
					endif
					if counter % 2 == 1
						args += '-DUSE_SINGLE_HEADER=1'
					endif
					if counter % 4 == 2 and exceptions
						args += '-DUSE_TARTANLLAMA_OPTIONAL=1'
						name = name + '_tlopt'
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
		test(
			executable[0] + ' (' + locale + ')', executable[1],
			env : ['LC_ALL=' + locale],
			workdir : meson.source_root() / 'tests'
		)
	endforeach
endforeach
