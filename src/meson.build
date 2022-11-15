# Compiled library

# These are the arguments needed to compile and consume the library, and
# are exposed to users with  the `compile_args` kwarg of declare_dependency()
lib_args = ['-DTOML_HEADER_ONLY=0']
if get_option('default_library') != 'static'
	lib_args += '-DTOML_SHARED_LIB=1'
endif
if compiler_supports_float16
	lib_args += compiler_supports_float16_args
	lib_args += '-DTOML_ENABLE_FLOAT16=1'
else
	lib_args += '-DTOML_ENABLE_FLOAT16=0'
endif
if compiler_supports_float16 or compiler_supports_float128 or compiler_supports_int128
	lib_args += compiler.get_supported_arguments('-fext-numeric-literals')
endif

lib = library(
	meson.project_name(),
	'toml++'/'toml.cpp',
	cpp_args: [universal_args, lib_args],
	gnu_symbol_visibility: 'hidden',
	include_directories: include_dir,
	install: not is_subproject,
	version: meson.project_version(),
	override_options: overrides
)

tomlplusplus_dep = declare_dependency(
	compile_args: lib_args,
	include_directories: include_dir,
	link_with: lib
)

if not is_subproject
	import('pkgconfig').generate(
		lib,
		description: 'TOML config file parser and serializer for C++',
		extra_cflags: lib_args,
		url: 'https://marzer.github.io/tomlplusplus'
	)
endif

# cmake
if get_option('generate_cmake_config') and not is_subproject
	cmake = import('cmake')
	cmake.write_basic_package_version_file(
		name: meson.project_name(),
		version: meson.project_version(),
	)

	# This gets the full path of the library, then considers just the last
	# component (i.e. the actual file name), and finally removes the
	# version suffix from it, because users _should_ link against the .so
	# file, as opposed to the .so.x.y.z one. This last step is only doable
	# with str.replace(), introduced in Meson 0.58.0, but it isn't essential
	lib_name = lib.full_path().split('/')[-1]
	if meson.version().version_compare('>=0.58.0')
		lib_name = lib_name.replace('.' + meson.project_version(), '')
	endif

	# CMake needs space-separated values since it doesn't have types
	cmake_compile_options = ''
	foreach arg : lib_args
		cmake_compile_options += arg + ' '
	endforeach
	cmake_compile_options = cmake_compile_options.strip()

	cmake.configure_package_config_file(
		name: meson.project_name(),
		input: '..'/'cmake'/'tomlplusplusConfig.cmake.meson.in',
		configuration: configuration_data({
			'compile_library': true,
			'compile_options': cmake_compile_options,
			'includedir': get_option('includedir'),
			'libdir': get_option('libdir'),
			'lib_name': lib_name
		})
	)
endif
