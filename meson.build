project(
	'tomlplusplus',
	'cpp',
	version : '2.1.0',
	license : 'MIT',
	default_options : [
		'cpp_std=c++17',
		'warning_level=3',
		'werror=true',
		'cpp_eh=default',
		'b_ndebug=if-release'
	]
)

tomlplusplus_dep = declare_dependency(
	include_directories : include_directories('include'),
	version : meson.project_version(),
)

build_tests = get_option('BUILD_TESTS').enabled() or (get_option('BUILD_TESTS').auto() and not meson.is_subproject())
build_examples = get_option('BUILD_EXAMPLES').enabled() or (get_option('BUILD_EXAMPLES').auto() and not meson.is_subproject())

if build_tests or build_examples

	compiler = meson.get_compiler('cpp')
	message(['compiler ID: ', compiler.get_id()])
	message(['compiler version: ', compiler.version()])

	# intel-cl  __builtin_bit_cast workaround
	if compiler.get_id() == 'intel-cl'
		add_project_arguments([
				'/D__builtin_bit_cast(x,y)=x{}'
			],
			language : 'cpp'
		)	
	endif
	
	# GCC or Clang
	if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
		add_project_arguments([
				'-march=native',
				'-fno-rtti',
			],
			language : 'cpp'
		)	
		if get_option('SMALL_BINARIES')
			add_project_arguments([ '-fdata-sections', '-ffunction-sections', '-Wl,--gc-sections', '-Wl,-s' ], language : 'cpp')
		endif
	endif

	# GCC
	if compiler.get_id() == 'gcc'
		add_project_arguments([
				'-fmax-errors=5',
				'-Wno-init-list-lifetime'
			],
			language : 'cpp'
		)
		if get_option('ALL_WARNINGS')
			add_project_arguments([
					'-Wcast-align',
					'-Wcast-qual',
					'-Wctor-dtor-privacy',
					'-Wdisabled-optimization',
					'-Wextra',
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
					'-Wpedantic',
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
					'-Wvariadic-macros',
					'-Wwrite-strings',
					'-Wmissing-noreturn',
					'-Wsuggest-attribute=const',
					'-Wsuggest-attribute=pure'
				],
				language : 'cpp'
			)
		endif
		if get_option('SMALL_BINARIES')
			add_project_arguments([ '-fmerge-constants' ], language : 'cpp')
		endif
	endif

	# Clang
	if compiler.get_id() == 'clang'
	
		if get_option('ALL_WARNINGS')
			add_project_arguments([ '-Weverything' ], language : 'cpp')
		endif
		add_project_arguments([
				'-ferror-limit=5',
				'-Wno-unused-command-line-argument',

				# flags from here down are disabling stupidly pedantic warnings that only appear with -Weverything
				'-Wno-c++98-compat',
				'-Wno-c++98-compat-pedantic',
				'-Wno-documentation',
				'-Wno-documentation-unknown-command',
				'-Wno-switch-enum',
				'-Wno-covered-switch-default'
			],
			language : 'cpp'
		)
		if get_option('TIME_TRACE')
			add_project_arguments([ '-ftime-trace' ], language : 'cpp')
		endif
		if get_option('SMALL_BINARIES')
			add_project_arguments([ '-Oz', '-fmerge-all-constants', '-Wl,--build-id' ], language : 'cpp')
		endif
	endif

	# msvc
	if compiler.get_id() == 'msvc'
		add_project_arguments([
				'/utf-8'
			],
			language : 'cpp'
		)
	endif

	# ICC's visual studio frontend, ICL
	if compiler.get_id() == 'intel-cl'
		add_project_arguments([
				'/Qoption,cpp,--unicode_source_kind,UTF-8',
				'/wd82', # storage class is not first
				'/wd280', # selector expression is constant (why the fuck is that a warning?)
				'/wd411', # class provides no constructor (duh, it's an aggregate)
				'/wd1011', # missing return statement (false negative)
				'/wd1628', # function marked [[noreturn]] returns (false positive)
				'/wd3280' # declaration hides member (triggered in Catch2)
			],
			language : 'cpp'
		)
	endif

	inc = include_directories('include', 'extern')

	if build_tests
		subdir('tests')
	else
		message('Not building tests')
	endif

	if build_examples
		subdir('examples')
	else
		message('Not building examples')
	endif

endif

install_subdir('include/toml++/',
	strip_directory: true,
	install_dir: 'include/toml++'
)

pkgc = import('pkgconfig')
pkgc.generate (
	name: meson.project_name(),
	version: meson.project_version(),
	description: 'Header-only TOML config file parser and serializer for modern C++'
)

# meson cmake stuff requires at least 0.50
if meson.version().version_compare('>= 0.50')
	if get_option('GENERATE_CMAKE_CONFIG').enabled() or (get_option('GENERATE_CMAKE_CONFIG').auto() and not meson.is_subproject())
		cmake = import('cmake')
		cmake.write_basic_package_version_file(
			name: meson.project_name(),
			version: meson.project_version()
		)
		
		cmake_conf = configuration_data()
		cmake.configure_package_config_file(
			name: meson.project_name(),
			input: 'cmake/tomlplusplus.cmake.in',
			configuration: cmake_conf,
		)
	endif
endif
