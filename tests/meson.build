# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

assert(build_tests)

test_sources = files(fs.read('sources.txt').strip().split('\n'))

test_deps = [ tomlplusplus_dep ]
if not get_option('use_vendored_libs')
	test_deps += dependency('catch2', version: '>=3.0.0')
endif

#-----------------------------------------------------------------------------------------------------------------------
# tests.exe
#-----------------------------------------------------------------------------------------------------------------------

test_args = []
test_args += global_args
if has_exceptions
	test_args += '-DSHOULD_HAVE_EXCEPTIONS=1'
else
	test_args += '-DSHOULD_HAVE_EXCEPTIONS=0'
endif
if get_option('use_vendored_libs')
	test_args += '-DUSE_VENDORED_LIBS=1'
else
	test_args += '-DUSE_VENDORED_LIBS=0'
endif


test_exe = executable(
	'tomlplusplus_tests',
	test_sources,
	cpp_args: test_args,
	dependencies: test_deps,
	override_options: global_overrides
)

#-----------------------------------------------------------------------------------------------------------------------
# per-locale invocations
#-----------------------------------------------------------------------------------------------------------------------

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

foreach locale : test_locales
	test(
		'tests - ' + locale,
		test_exe,
		env: ['LC_ALL=' + locale],
		workdir: meson.project_source_root()/'tests'
	)
endforeach

#-----------------------------------------------------------------------------------------------------------------------
# ODR build test
#-----------------------------------------------------------------------------------------------------------------------

# add the ODR-test build project
executable(
	'tomlplusplus_odr_test',
	files( 'odr_test_1.cpp', 'odr_test_2.cpp' ),
	cpp_args: test_args,
	dependencies: test_deps
)
