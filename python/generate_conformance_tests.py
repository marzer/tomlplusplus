#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import os.path as path
import utils
import io
import re



def open_test_file(name):
	test_file_path = path.join(utils.get_script_folder(), '..', 'tests', name)
	print("Writing to {}".format(test_file_path))
	return open(test_file_path, 'w', encoding='utf-8', newline='\n')



def emit_preamble(test_file):
	write = lambda txt: print(txt, file=test_file)
	write('#include "tests.h"')
	write('using namespace toml::impl;')
	write('')
	write('#if !TOML_UNRELEASED_FEATURES // todo: improve conformance script to remove this')
	write('')



def sanitize(s):
	return re.sub(r'[ -]+', '_', s, 0, re.I | re.M)



def emit_invalid_tests(test_file, name, source_folder, skip_list=None):
	constants_buf = io.StringIO('', newline='\n')
	constants = lambda txt: print(txt, file=constants_buf)
	tests_buf = io.StringIO('', newline='\n')
	tests = lambda txt: print(txt, file=tests_buf)

	constants('namespace // invalid test data for {}'.format(name))
	constants('{')
	
	tests('TEST_CASE("conformance - invalid inputs from {}")'.format(name))
	tests('{')

	#files = [path.splitext(path.split(f)[1])[0] for f in utils.get_all_files(source_folder,all="*.toml")]
	files = [path.split(f) for f in utils.get_all_files(source_folder, all="*.toml")]
	files = [(f[0], *path.splitext(f[1])) for f in files]
	for dir,file,ext in files:
		if skip_list and file in skip_list:
			continue
		identifier = sanitize(file)
		constants('\t static constexpr auto {} = S(R"({})"sv);'.format(
			identifier,
			utils.read_all_text_from_file(path.join(dir, file + ext)).strip()
		))
		
		tests('\tparsing_should_fail(FILE_LINE_ARGS, {});'.format(identifier))

	constants('}')
	tests('}')

	write = lambda txt: print(txt, file=test_file)
	write(constants_buf.getvalue())
	write(tests_buf.getvalue())

def emit_appendix(test_file):
	write = lambda txt: print(txt, file=test_file)
	write('')
	write('#endif // !TOML_UNRELEASED_FEATURES')



def main():
	extern_root = path.join(utils.get_script_folder(), '..', 'extern')
	with open_test_file('conformance.cpp') as test_file:
		emit_preamble(test_file)
		emit_invalid_tests(
			test_file,
			'BurntSushi/toml-test',
			path.join(extern_root, 'toml-test', 'tests', 'invalid'),
			(
				# false negatives after TOML 0.4.0
				'array-mixed-types-arrays-and-ints',
				'array-mixed-types-ints-and-floats',
				'array-mixed-types-strings-and-ints'
			)
		)
		emit_invalid_tests(
			test_file,
			'iarna/toml-spec-tests',
			path.join(extern_root, 'toml-spec-tests', 'errors'),
			(
				# I handle these internally, they get broken by I/O
				'comment-control-1',
				'comment-control-2',
				'comment-control-3',
				'comment-control-4',
				'string-basic-control-1',
				'string-basic-control-2',
				'string-basic-control-3',
				'string-basic-control-4',
				'string-basic-multiline-control-1',
				'string-basic-multiline-control-2',
				'string-basic-multiline-control-3',
				'string-basic-multiline-control-4',
				'string-literal-control-1',
				'string-literal-control-2',
				'string-literal-control-3',
				'string-literal-control-4',
				'string-literal-multiline-control-1',
				'string-literal-multiline-control-2',
				'string-literal-multiline-control-3',
				'string-literal-multiline-control-4',

			)
		)
		emit_appendix(test_file)



if __name__ == '__main__':
	utils.run(main)
