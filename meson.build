project(
	'tomlplusplus',
	'cpp',
	version: '3.0.0',
	meson_version: '>=0.54.0',
	license: 'MIT',
	default_options: [ # https://mesonbuild.com/Builtin-options.html
		# core options
		'buildtype=release',
		'warning_level=3',
		'werror=true',

		# base options
		'b_lto=true',
		'b_ndebug=if-release',

		# compiler options
		'cpp_std=c++17'
	]
)

#######################################################################################################################
# compiler management
#######################################################################################################################

compiler = meson.get_compiler('cpp')
message('target cpu_family: @0@'.format(host_machine.cpu_family()))
message('target cpu: @0@'.format(host_machine.cpu()))
message('target system: @0@'.format(host_machine.system()))
message('target endian: @0@'.format(host_machine.endian()))

is_gcc = compiler.get_id() == 'gcc'
is_clang = compiler.get_id() == 'clang'
is_msvc = compiler.get_id() == 'msvc'
is_icc_cl = compiler.get_id() == 'intel-cl'
is_icc = is_icc_cl or compiler.get_id() == 'intel'
is_lld = compiler.get_linker_id() == 'ld.lld'
is_debug = get_option('debug')
is_release = not is_debug
is_pedantic = get_option('pedantic')
is_windows = host_machine.system() == 'windows'
is_x64 = host_machine.cpu_family() == 'x86_64'
is_subproject = meson.is_subproject()
has_exceptions = get_option('cpp_eh') != 'none'
include_dirs = include_directories('include', 'external')
overrides = []
additional_arguments = []

message('is_release: @0@'.format(is_release))
message('is_windows: @0@'.format(is_windows))
message('is_x64: @0@'.format(is_x64))
message('has_exceptions: @0@'.format(has_exceptions))

# compiler argument references:
# msvc: https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=vs-2019
# intel and intel-cl: https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-oneapi-dev-guide-and-reference/top/compiler-reference/compiler-options/alphabetical-list-of-compiler-options.html
# gcc:
# clang:

# GCC or Clang
if is_gcc or is_clang
	add_project_arguments(
		'-march=native',
		'-fvisibility=hidden',
		'-fvisibility-inlines-hidden',
		language: 'cpp'
	)
	if is_release
		add_project_arguments(
			'-fdata-sections',
			'-ffunction-sections',
			'-Wl,--gc-sections',
			'-Wl,-s',
			'-mfma',
			language: 'cpp'
		)
	endif
endif


# GCC
if is_gcc
	add_project_arguments(
		'-fmax-errors=5',
		'-Wno-init-list-lifetime',
		language: 'cpp'
	)
	if is_pedantic
		add_project_arguments(
			'-Wcast-align',
			'-Wcast-qual',
			'-Wctor-dtor-privacy',
			'-Wdisabled-optimization',
			'-Wfloat-equal',
			'-Wimport',
			'-Winit-self',
			'-Wlogical-op',
			'-Wmissing-declarations',
			'-Wmissing-field-initializers',
			'-Wmissing-format-attribute',
			'-Wmissing-include-dirs',
			'-Wmissing-noreturn',
			'-Wnoexcept',
			'-Wold-style-cast',
			'-Woverloaded-virtual',
			'-Wpacked',
			'-Wpadded',
			'-Wpointer-arith',
			'-Wredundant-decls',
			'-Wshadow',
			'-Wsign-conversion',
			'-Wsign-promo',
			'-Wstack-protector',
			'-Wstrict-null-sentinel',
			'-Wswitch-default',
			'-Wswitch-enum',
			'-Wundef',
			'-Wunreachable-code',
			'-Wunused',
			'-Wunused-parameter',
			'-Wuseless-cast',
			'-Wvariadic-macros',
			'-Wwrite-strings',
			'-Wmissing-noreturn',
			language: 'cpp'
		)
	endif
	if is_release
		add_project_arguments(
			'-fmerge-constants',
			'-Wsuggest-attribute=const',
			'-Wsuggest-attribute=pure',
			language: 'cpp'
		)
	endif
endif

# Clang
if is_clang
	if is_pedantic
		add_project_arguments('-Weverything', language: 'cpp')
	endif
	add_project_arguments(
		'-ferror-limit=5',
		'-Wno-unused-command-line-argument',

		# flags from here down are disabling stupidly pedantic warnings that only appear with -Weverything
		'-Wno-c++98-compat',
		'-Wno-c++98-compat-pedantic',
		'-Wno-documentation',
		'-Wno-documentation-unknown-command',
		'-Wno-switch-enum',
		'-Wno-covered-switch-default',
		language: 'cpp'
	)
	if get_option('time_trace')
		add_project_arguments('-ftime-trace', language: 'cpp')
	endif
endif

# MSVC or icc-cl
if is_msvc or is_icc_cl
	add_project_arguments(
		'/bigobj',
		'/fp:except-', # disable floating-point exceptions
		'/Gy', # function-level linking
		'/GF', # string pooling
		'/openmp-',
		'/permissive-',
		'/sdl-',
		'/utf-8',
		'/volatile:iso',
		'/Zc:__cplusplus',
		'/Zc:inline',
		'/Zc:throwingNew',
		'/Zi', # generate debug info (doesn't affect optimization)
		language: 'cpp'
	)
	add_project_link_arguments('/DEBUG', language: 'cpp') # generate PDB (doesn't affect optimization)
	if is_release
		add_project_arguments(
			'/GL', # whole program optimization
			'/Gw', # Optimize Global Data
			'/Ob3', # aggressive inlining
			'/Oy', # omit frame pointers
			'/Oi', # generate intrinsics
			language: 'cpp'
		)
		add_project_link_arguments('/ltcg', language: 'cpp')
	endif
	if is_pedantic
		add_project_arguments('/W4', language: 'cpp')
	endif
	if is_x64
		add_project_arguments('/arch:AVX', language: 'cpp')
	endif
endif

# icc-cl
if is_icc_cl
	add_project_arguments(
		'/wd82', # storage class is not first
		'/wd177', # unreferenced var
		'/wd280', # selector expression is constant (why the fuck is that a warning?)
		'/wd411', # class provides no constructor (duh, it's an aggregate)
		'/wd869', # parameter "blah" was never referenced
		'/wd1011', # missing return statement (false negative)
		'/wd1628', # function marked [[noreturn]] returns (false positive)
		'/wd2261', # assume with side effects discarded
		'/wd2557', # mismatched sign compare
		'/wd3280', # declaration hides member (triggered in Catch2)
		language: 'cpp'
	)
endif

# icc (any)
if is_icc
	add_project_arguments(
		'/Qdiag-error-limit:5',
		'/Qoption,cpp,--unicode_source_kind,UTF-8',
		'/D__builtin_bit_cast(T, v)=([&]()noexcept{ T val; memcpy(&val, &v, sizeof(T)); return val; })()', # __builtin_bit_cast workaround
		language: 'cpp'
	)
endif

# windows stuff
if is_windows
	add_project_arguments(
		'-D_ITERATOR_DEBUG_LEVEL=0',
		'-D_WINSOCK_DEPRECATED_NO_WARNINGS',
		'-D_SCL_SECURE_NO_WARNINGS',
		'-D_CRT_SECURE_NO_WARNINGS',
		language: 'cpp'
	)
	add_project_arguments(has_exceptions ? '-D_HAS_EXCEPTIONS=1' : '-D_HAS_EXCEPTIONS=0', language: 'cpp')
elif is_release
	overrides += 'strip=true'
endif


# LTO
if is_lld or is_debug
	overrides += 'b_lto=false'
endif

#######################################################################################################################
# c++ 20 check
#######################################################################################################################

compiler_supports_cpp20_args = []
if is_gcc or is_clang
	compiler_supports_cpp20_args += '-std=c++2a'
elif is_icc
	compiler_supports_cpp20_args += '/Qstd=c++2a'
elif is_msvc
	compiler_supports_cpp20_args += '/std:c++latest'
endif
compiler_supports_cpp20 = compiler_supports_cpp20_args.length() > 0 and compiler.links('''
	#include <version>
	#include <string>
	#include <iostream>
	#include <cstdint>
	#include <cstddef>
	#include <cstring>
	#include <cfloat>
	#include <climits>
	#include <cmath>
	#include <limits>
	#include <memory>
	#include <iosfwd>
	#include <type_traits>

	int main()
	{
		std::string s = "kek";
		std::cout << s << std::endl;
		return 0;
	}
	''',
	name: 'supports c++20',
	args: compiler_supports_cpp20_args
)

#######################################################################################################################
# char8_t check
#######################################################################################################################

compiler_supports_char8_args = []
if is_gcc or is_clang
	compiler_supports_char8_args += '-fchar8_t'
endif
compiler_supports_char8_args_private = []
compiler_supports_char8_args_private += compiler_supports_cpp20_args
compiler_supports_char8_args_private += compiler_supports_char8_args
compiler_supports_char8 = compiler_supports_cpp20 and compiler.links('''
	#include <version>
	#include <string_view>
	#include <string>
	#include <type_traits>
	using namespace std::string_view_literals;

	#if !defined(__cpp_char8_t)	|| __cpp_char8_t < 201811 || !defined(__cpp_lib_char8_t) || __cpp_lib_char8_t < 201907
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
	name: 'supports char8_t',
	args: compiler_supports_char8_args_private
)

#######################################################################################################################
# consteval check
# (this doesn't inform the build in any way; it's just so i can see who supports it properly)
#######################################################################################################################

compiler_supports_consteval = compiler_supports_cpp20 and compiler.compiles('''

	consteval int test() noexcept
	{
		return 42;
	}

	int main()
	{
		constexpr auto val = test(); // test() should be compiletime-callable
		return val;
	}
	''',
	name: 'supports consteval keyword',
	args: compiler_supports_cpp20_args
)

compiler_supports_consteval_properly = compiler_supports_consteval and not compiler.compiles('''

	consteval int test(int i) noexcept
	{
		return 42 + i;
	}

	int get_value() noexcept;

	int main()
	{
		return test(get_value()); // test() should not be runtime-callable
	}
	''',
	name: 'consteval is just renamed constexpr',
	args: compiler_supports_cpp20_args
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

compiler_supports_float16_args = []
if is_gcc
	compiler_supports_float16_args += '-mfp16-format=ieee'
endif
compiler_supports_fp16 = compiler.links('''
	int main()
	{
		static_assert(sizeof(__fp16) == 2);
		__fp16 f = static_cast<__fp16>(1);
		const auto f2 = static_cast<float>(f);
		const auto f3 = static_cast<__fp16>(0.2L);
		return 0;
	}
	''',
	name: 'supports __fp16',
	args: compiler_supports_float16_args
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
	name: 'supports _Float16',
	args: compiler_supports_float16_args
)
if compiler_supports_fp16 or compiler_supports_float16
	additional_arguments += compiler_supports_float16_args
endif

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
	name: 'supports __int128_t'
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
	name: 'supports __float128'
)

if compiler_supports_float128 and is_gcc and not is_subproject
	add_global_arguments('-fext-numeric-literals', language: 'cpp')
endif

#######################################################################################################################
# subdirectories
#######################################################################################################################

build_tests = get_option('build_tests') and not is_subproject
if build_tests
	run_command('git', 'submodule', 'update', '--init', '--depth', '1', 'external/Catch2')
	run_command('git', 'submodule', 'update', '--init', '--depth', '1', 'external/tloptional')
	subdir('tests')
endif

build_examples = get_option('build_examples') and not is_subproject
if build_examples
	subdir('examples')
endif

build_tt = (get_option('build_tt_encoder') or get_option('build_tt_encoder')) and not is_subproject
if build_tt
	subdir('toml-test')
endif

if not is_subproject
	install_subdir('include'/'toml++',
		strip_directory: true,
		install_dir: get_option('includedir')/'toml++'
	)
endif

#######################################################################################################################
# dependencies, cmake etc.
#######################################################################################################################

tomlplusplus_dep = declare_dependency(
	include_directories: include_directories('include'),
	version: meson.project_version(),
)

meson.override_dependency('tomlplusplus', tomlplusplus_dep)

if not is_subproject
	import('pkgconfig').generate(
		name: meson.project_name(),
		version: meson.project_version(),
		description: 'Header-only TOML config file parser and serializer for C++',
		install_dir: get_option('datadir')/'pkgconfig',
	)
endif

# cmake
if get_option('generate_cmake_config') and not is_subproject
	cmake = import('cmake')
	cmake.write_basic_package_version_file(
		name: meson.project_name(),
		version: meson.project_version(),
		install_dir: 'lib'/'cmake'/meson.project_name(),
	)

	cmake_conf = configuration_data()
	cmake.configure_package_config_file(
		name: meson.project_name(),
		input: 'cmake'/'tomlplusplus.cmake.in',
		configuration: cmake_conf,
		install_dir: 'lib'/'cmake'/meson.project_name(),
	)
endif
