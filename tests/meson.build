test_sources = [
	'at_path.cpp',
	'conformance_burntsushi_invalid.cpp',
	'conformance_burntsushi_valid.cpp',
	'conformance_iarna_invalid.cpp',
	'conformance_iarna_valid.cpp',
	'formatters.cpp',
	'for_each.cpp',
	'impl_toml.cpp',
	'main.cpp',
	'manipulating_arrays.cpp',
	'manipulating_parse_result.cpp',
	'manipulating_tables.cpp',
	'manipulating_values.cpp',
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
	'path.cpp',
	'tests.cpp',
	'user_feedback.cpp',
	'using_iterators.cpp',
	'visit.cpp',
	'windows_compat.cpp'
]

test_deps = [ tomlplusplus_dep ]
if not get_option('use_vendored_libs')
	test_deps += dependency('catch2')
endif

#######################################################################################################################
# fast math check
#######################################################################################################################

compiler_supports_fast_math_args = compiler.get_supported_arguments('-ffast-math', '-ffp-contract=fast', '/fp:fast')
compiler_supports_fast_math = compiler.links('''
	#include <cmath>
	#include <iostream>
	int main()
	{
		std::cout << std::exp2(2.0) << std::pow(2.0, 3.0) << "\n";
		return 0;
	}
	''',
	name: 'supports fast-math',
	args: compiler_supports_fast_math_args
)
#######################################################################################################################
# do the thing!
#######################################################################################################################

fast_math_modes = [ false, true ]
exception_modes = [ true, false ]
unreleased_feature_modes = [ false, true ]
cpp20_modes = [ false, true ]
test_executables = []
test_counter = 0
test_base_args = []
test_base_args += universal_args
test_base_args += devel_args

# there are a number of skips here for builds that choose to precompile the library for their tests;
# that's 'fine', it just means that the tests for the compiled library only test settings exactly compatible with
# the way the library itself was compiled (otherwise we risk ABI errors, link errors, etc.)
#
# regular header-only mode tests all the things like normal.

foreach cpp20 : cpp20_modes

	# skip this target if we're testing c++20 mode and precompiling the library
	if cpp20 and (not compiler_supports_cpp20 or get_option('compile_library'))
		continue
	endif

	foreach unrel : unreleased_feature_modes

		# skip this target if it's using 'unreleased features' and precompiling the library
		if unrel and get_option('compile_library')
			continue
		endif

		foreach fast_math : fast_math_modes

			# skip this target if it's testing fast math and the compiler doesn't support a known fast-math mode
			# (or if we're precompiling the library)
			if fast_math and (not compiler_supports_fast_math or get_option('compile_library'))
				continue
			endif

			foreach exceptions : exception_modes

				# skip this target if exceptions are disabled and precompiling the library;
				if not exceptions and get_option('compile_library')
					continue
				endif

				# skip this target if we've disabled exceptions and enabled fast math;
				# both are already tested separately and don't interact in any way meaningful enough to warrant
				# a combined permutation.
				if not exceptions and fast_math
					continue
				endif

				# ... same for 'unreleased features' and fast math
				if unrel and fast_math
					continue
				endif

				test_name = ''
				test_overrides = []
				test_overrides += overrides
				test_args = []
				test_args += test_base_args

				# use the single-header version for some tests to ensure it is the same (no header generation failures)
				single_header = (
					test_counter % 2 == 0
					and not get_option('compile_library')
				)

				# use asan in the tests
				address_sanitizer = (
					get_option('asan_tests')
					and is_clang
				)

				# use ubsan in the tests
				ub_sanitizer = (
					get_option('ubsan_tests')
					and is_clang
				)

				if cpp20
					test_name = 'cpp20'
					test_overrides += 'cpp_std=none'
					test_args += compiler_supports_cpp20_args
					if compiler_supports_char8
						test_args += compiler_supports_char8_args
					endif
				else
					test_name = 'cpp17'
				endif

				if exceptions
					test_overrides += 'cpp_eh=default'
					test_args += '-DSHOULD_HAVE_EXCEPTIONS=1'
					if is_windows
						test_args += '-D_HAS_EXCEPTIONS=1'
					endif
				else
					test_name = test_name + '_noexcept'
					test_overrides += 'cpp_eh=none'
					test_args += '-DSHOULD_HAVE_EXCEPTIONS=0'
					if is_windows
						test_args += '-D_HAS_EXCEPTIONS=0'
					endif
				endif

				if address_sanitizer or ub_sanitizer
					test_args += [ '-g3' ]
					if address_sanitizer and ub_sanitizer
						test_overrides += 'b_sanitize=address,undefined'
					elif address_sanitizer
						test_overrides += 'b_sanitize=address'
					else
						test_overrides += 'b_sanitize=undefined'
					endif
				endif

				if fast_math
					test_name = test_name + '_fastmath'
					test_args += compiler_supports_fast_math_args
				endif

				if compiler_supports_float16
					test_args += '-DSHOULD_HAVE_FLOAT16=1'
				endif
				if compiler_supports_int128
					test_args += '-DSHOULD_HAVE_INT128=1'
				endif
				if compiler_supports_float128
					test_args += '-DSHOULD_HAVE_FLOAT128=1'
				endif

				if unrel
					test_name = test_name + '_unrel'
					test_args += '-DTOML_ENABLE_UNRELEASED_FEATURES=1'
				else
					test_args += '-DTOML_ENABLE_UNRELEASED_FEATURES=0'
				endif

				if single_header
					test_args += '-DUSE_SINGLE_HEADER=1'
				endif

				if not get_option('use_vendored_libs')
					test_args += '-DUSE_VENDORED_LIBS=0'
				endif

				test_executables += [[
					test_name,
					executable(
						test_name,
						test_sources,
						cpp_args: test_args,
						dependencies: test_deps,
						override_options: test_overrides
					)
				]]

				test_counter = test_counter + 1

			endforeach # exceptions
		endforeach # fast_math
	endforeach # strict
endforeach # cpp20

test_locales = [
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

foreach executable : test_executables
	foreach locale : test_locales
		test(
			executable[0] + ' (' + locale + ')', # name
			executable[1], # executable object
			env: ['LC_ALL=' + locale],
			workdir: meson.source_root()/'tests'
		)
	endforeach
endforeach


# add the ODR-test build project
executable(
	'odr_test',
	[ 'odr_test_1.cpp', 'odr_test_2.cpp' ],
	cpp_args: test_base_args,
	dependencies: tomlplusplus_dep,
	override_options: overrides
)
