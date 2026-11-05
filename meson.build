# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

project(
	'tomlplusplus',
	'cpp',
	license: 'MIT',
	version: files('VERSION.txt')[0],
	meson_version: '>=0.61.0',
	default_options: [
		# https://mesonbuild.com/Builtin-options.html
		# core options
		'buildtype=release',
		'default_library=shared',
		# base options
		'b_ndebug=if-release',
		# compiler options
		'cpp_std=c++17'
	]
)

#-----------------------------------------------------------------------------------------------------------------------
# global vars + imports
#-----------------------------------------------------------------------------------------------------------------------

fs = import('fs')

is_devel = get_option('devel')
is_debug = get_option('debug')
is_release = not is_debug
is_pedantic = get_option('pedantic') or is_devel
is_permissive = get_option('permissive')
is_subproject = meson.is_subproject()

cpp = meson.get_compiler('cpp')
is_gcc = cpp.get_id() == 'gcc'
is_clang = cpp.get_id() == 'clang'
has_exceptions = get_option('cpp_eh') != 'none'
unreleased_features = get_option('unreleased_features')

build_tests = (get_option('build_tests') or is_devel) and not is_subproject
build_examples = (get_option('build_examples') or is_devel) and not is_subproject
build_tt = (get_option('build_tt') or is_devel) and not is_subproject and has_exceptions
build_lib = get_option('build_lib') or get_option('compile_library') or build_tests or build_examples or build_tt

#-----------------------------------------------------------------------------------------------------------------------
# global_args
#
# these are the arguments common to everything in the project
# *** they are not forwarded to dependents when using this as a submodule. ***
#-----------------------------------------------------------------------------------------------------------------------

global_args = cpp.get_supported_arguments(
	# clang/gcc
	'-ferror-limit=5',
	'-fmax-errors=5',
	'-Wno-unused-command-line-argument',
	'-Wno-reserved-macro-identifier',
	'-Wno-init-list-lifetime',
	'-fchar8_t',
	# msvc
	'/bigobj',
	'/Gy', # function-level linking
	'/GF', # string pooling
	'/openmp-',
	'/utf-8',
	'/volatile:iso',
	'/Zc:__cplusplus',
	'/Zc:inline',
	'/Zc:externConstexpr',
	'/Zc:preprocessor'
)
if has_exceptions
	global_args += cpp.get_supported_arguments('/Zc:throwingNew')
	global_args += '-D_HAS_EXCEPTIONS=1'
else
	global_args += '-D_HAS_EXCEPTIONS=0'
endif
if is_permissive
	global_args += cpp.get_supported_arguments('/permissive')
else
	global_args += cpp.get_supported_arguments('/permissive-')
endif
if is_pedantic
	global_args += cpp.get_supported_arguments('-Weverything')
	if is_gcc
		global_args += cpp.get_supported_arguments(
			'-Wcast-align',
			'-Wcast-qual',
			'-Wctor-dtor-privacy',
			'-Wdisabled-optimization',
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
			'-Wpointer-arith',
			'-Wredundant-decls',
			'-Wshadow',
			'-Wsign-conversion',
			'-Wsign-promo',
			'-Wstack-protector',
			'-Wstrict-null-sentinel',
			'-Wswitch-default',
			'-Wundef',
			'-Wunreachable-code',
			'-Wunused',
			'-Wunused-parameter',
			'-Wuseless-cast',
			'-Wvariadic-macros',
			'-Wwrite-strings'
		)
	endif
endif
# unnecessary pedantry:
global_args += cpp.get_supported_arguments(
	'-Wno-c++98-compat',
	'-Wno-c++98-compat-pedantic',
	'-Wno-switch-enum',
	'-Wno-covered-switch-default',
	'-Wno-padded',
	'-Wno-float-equal',
	'-Wno-c2y-extensions',
	'-Wno-unsafe-buffer-usage',
	'-Wno-nrvo',
	'-Wno-poison-system-directories'
)
if is_clang
	global_args += cpp.get_supported_arguments('-Wno-switch-default')
endif
if get_option('time_trace')
	global_args += cpp.get_supported_arguments('-ftime-trace')
endif

#-----------------------------------------------------------------------------------------------------------------------
# global_overrides
#
# these are the meson overrides common to everything in the project
# *** they are not forwarded to dependents when using this as a submodule. ***
#-----------------------------------------------------------------------------------------------------------------------

global_overrides = [ ]
if is_pedantic
	global_overrides += [
		'warning_level=3',
		'werror=true',
		]
endif

#-----------------------------------------------------------------------------------------------------------------------
# generated files
#-----------------------------------------------------------------------------------------------------------------------

regenerate = get_option('regenerate') and not is_subproject and fs.exists('.git')

if regenerate
	python = import('python').find_installation()
	regenerate_script = files('tools/regenerate.py')
	run_command(python, regenerate_script, check: true)
	foreach input : run_command(python, regenerate_script, '--inputs', check: true).stdout().split()
		fs.read(input)
	endforeach
endif

#-----------------------------------------------------------------------------------------------------------------------
# subdirectories + files
#-----------------------------------------------------------------------------------------------------------------------

# Empty dependency that will be filled either in src/ or include/
tomlplusplus_dep = dependency('', required: false)

subdir('include')

if build_lib
	subdir('src')
endif

if build_tests
	subdir('tests')
endif

if build_examples
	subdir('examples')
endif

if build_tt
	subdir('toml-test')
endif

# Allow subproject usage
meson.override_dependency(meson.project_name(), tomlplusplus_dep)

summary(
	{
		'Library': build_lib ? get_option('default_library') : 'header-only',
		'Tests': build_tests,
		'Examples': build_examples,
		'toml-test': build_tt,
		'Pedantic': is_pedantic,
		'Unreleased features': unreleased_features,
		'Regenerate': regenerate,
	},
	bool_yn: true
)
