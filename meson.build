project(
	'tomlplusplus',
	'cpp',
	version: '3.2.0',
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
include_dir = include_directories('include')
overrides = []
universal_args = [] # args used in tests, examples, lib, everything
devel_args = [] # args used in everything *but* the lib

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
	devel_args += '-march=native'
endif

# GCC
if is_gcc
	universal_args += [
		'-fmax-errors=5',
		'-Wno-init-list-lifetime',
	]
	if is_pedantic
		universal_args += [
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
		]
	endif
	if is_release and is_pedantic
		universal_args += [
			'-Wsuggest-attribute=const',
			'-Wsuggest-attribute=pure',
		]
	endif
endif

# Clang
if is_clang
	if is_pedantic
		universal_args += '-Weverything'
	endif
	universal_args += [
		'-ferror-limit=5',
		'-Wno-unused-command-line-argument',

		# flags from here down are disabling stupidly pedantic warnings that only appear with -Weverything
		'-Wno-c++98-compat',
		'-Wno-c++98-compat-pedantic',
		'-Wno-documentation',
		'-Wno-documentation-unknown-command',
		'-Wno-switch-enum',
		'-Wno-covered-switch-default',
	]
	if get_option('time_trace')
		universal_args += ['-ftime-trace']
	endif
endif

# MSVC or icc-cl
if is_msvc or is_icc_cl
	universal_args += [
		'/bigobj',
		'/fp:except-', # disable floating-point exceptions
		'/Gy', # function-level linking
		'/GF', # string pooling
		'/openmp-',
		'/permissive-',
		'/utf-8',
		'/Zc:inline'
	]
	if has_exceptions
		universal_args += '/Zc:throwingNew'
	endif
	if is_release
		universal_args += [
			'/GL', # whole program optimization
			'/Gw', # Optimize Global Data
			'/Ob3', # aggressive inlining
			'/Oy', # omit frame pointers
			'/Oi', # generate intrinsics
		]
		add_project_link_arguments('/ltcg', language: 'cpp')
	endif
	if is_pedantic
		universal_args += '/W4'
	endif
endif

# icc-cl
if is_icc_cl
	universal_args += [
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
	]
endif

# icc (any)
if is_icc
	universal_args += [
		'/Qdiag-error-limit:5',
		'/Qoption,cpp,--unicode_source_kind,UTF-8',
		'/D__builtin_bit_cast(T, v)=([&]()noexcept{ T val; memcpy(&val, &v, sizeof(T)); return val; })()', # __builtin_bit_cast workaround
	]
endif

# windows stuff
if is_windows
	universal_args += has_exceptions ? '-D_HAS_EXCEPTIONS=1' : '-D_HAS_EXCEPTIONS=0'
elif is_release
	overrides += 'strip=true'
endif

# LTO
if is_lld or is_debug or (is_windows and is_clang)
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
	args: [ compiler_supports_cpp20_args, compiler_supports_char8_args ]
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
# _Float16 checks
#######################################################################################################################

compiler_supports_float16_args = compiler.get_supported_arguments('-mfp16-format=ieee')
compiler_supports_float16 = get_option('float16') and compiler.links('''
	int main()
	{
		static_assert(sizeof(_Float16) == 2);
		_Float16 f = static_cast<_Float16>(1);
		const auto f2 = static_cast<float>(f);
		const auto f3 = static_cast<_Float16>(0.2L);
		return 0;
	}
	''',
	name: 'supports _Float16',
	args: compiler_supports_float16_args
)
if compiler_supports_float16
	devel_args += compiler_supports_float16_args
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

if compiler_supports_float16 or compiler_supports_float128 or compiler_supports_int128
	devel_args += compiler.get_supported_arguments('-fext-numeric-literals')
endif

#######################################################################################################################
# subdirectories
#######################################################################################################################

# Empty dependency that will be filled either in src/ or include/
tomlplusplus_dep = dependency('', required: false)

if get_option('compile_library')
	subdir('src')
else
	subdir('include')
endif

build_tests = get_option('build_tests') and not is_subproject
if build_tests
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
	install_subdir('include'/'toml++', install_dir: get_option('includedir'))
endif

# Allow subproject usage
meson.override_dependency(meson.project_name(), tomlplusplus_dep)
