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
import json
import yaml # pip install pyyaml
import math
import dateutil.parser # pip install python-dateutil
from datetime import datetime, date, time



def sanitize(s):
	s = re.sub(r'[ _:;\/-]+', '_', s, 0, re.I | re.M)
	if s in ('bool', 'float', 'int', 'double', 'auto'):
		s = s + '_'
	return s



def python_value_to_tomlpp(val):
	if isinstance(val, str):
		if re.fullmatch(r'^[+-]?[0-9]+[eE][+-]?[0-9]+$', val, re.M):
			return str(float(val))
		else:
			return 'R"({})"sv'.format(val)
	elif isinstance(val, bool):
		return 'true' if val else 'false'
	elif isinstance(val, float):
		if math.isinf(val):
			return f'{"-" if val < 0.0 else ""}std::numeric_limits<double>::infinity()'
		elif math.isnan(val):
			return 'std::numeric_limits<double>::quiet_NaN()'
		else:
			return str(val)
	elif isinstance(val, int):
		if val == 9223372036854775807:
			return 'INT64_MAX'
		elif val == -9223372036854775808:
			return 'INT64_MIN'
		else:
			return str(val)
	elif isinstance(val, (TomlPPArray, TomlPPTable)):
		return str(val)
	elif isinstance(val, datetime):
		offset = None
		if val.tzinfo is not None:
			offset = val.tzinfo.utcoffset(val)
			mins = offset.total_seconds() / 60
			offset = (int(mins / 60), int(mins % 60))
		return 'toml::date_time{{ {{ {}, {}, {} }}, {{ {}, {}, {}, {}u }}{} }}'.format(
			val.year,
			val.month,
			val.day,
			val.hour,
			val.minute,
			val.second,
			val.microsecond*1000,
			'' if offset is None else ', {{ {}, {} }}'.format(offset[0], offset[1])
		)
	elif isinstance(val, date):
		return 'toml::date{{ {}, {}, {} }}'.format(
			val.year,
			val.month,
			val.day
		)
	elif isinstance(val, time):
		return 'toml::time{{ {}, {}, {}, {} }}'.format(
			val.hour,
			val.minute,
			val.second,
			val.microsecond*1000
		)
	else:
		raise ValueError(str(type(val)))



class TomlPPArray:

	def __init__(self, init_data=None):
		self.values = init_data if init_data else list()

	def render(self, indent = '', indent_declaration = False):
		s = ''
		if indent_declaration:
			s += indent
		if len(self.values) == 0:
			s += 'toml::array{}'
		else:
			s += 'toml::array{'
			for val in self.values:
				s += '\n' + indent + '\t'
				if isinstance(val, TomlPPArray) and len(self.values) == 1:
					s += 'toml::inserter{'
				if isinstance(val, (TomlPPTable, TomlPPArray)) and len(val) > 0:
					s += val.render(indent + '\t')
				else:
					s += python_value_to_tomlpp(val)
				if isinstance(val, TomlPPArray) and len(self.values) == 1:
					s += '}'
				s += ','
			s += '\n' + indent + '}'
		return s

	def __str__(self):
		return self.render()

	def __len__(self):
		return len(self.values)



class TomlPPTable:

	def __init__(self, init_data=None):
		self.values = init_data if init_data else dict()

	def render(self, indent = '', indent_declaration = False):
		s = ''
		if indent_declaration:
			s += indent
		if len(self.values) == 0:
			s += 'toml::table{}'
		else:
			s += 'toml::table{{'
			for key, val in self.values.items():
				s += '\n' + indent + '\t{ '
				if isinstance(val, (TomlPPTable, TomlPPArray)) and len(val) > 0:
					s += '\n' + indent + '\t\t{},'.format(python_value_to_tomlpp(str(key)))
					#s += '\n' + val.render(indent + '\t\t')
					s += ' ' + val.render(indent + '\t\t')
					s += '\n' + indent + '\t'
				else:
					s += '{}, {} '.format(python_value_to_tomlpp(str(key)), python_value_to_tomlpp(val))
				s += '},'
				#else:

				#s += '}\n'
			s += '\n' + indent + '}}'
		return s

	def __str__(self):
		return self.render()

	def __len__(self):
		return len(self.values)



def json_to_python(val):

	if isinstance(val, dict):
		if len(val) == 2 and "type" in val and "value" in val:
			val_type = val["type"]
			if val_type == "integer":
				return int(val["value"])
			elif val_type == "float":
				return float(val["value"])
			elif val_type == "string":
				return str(val["value"])
			elif val_type == "bool":
				return True if val["value"].lower() == "true" else False
			elif val_type == "array":
				return json_to_python(val["value"])
			elif val_type in ("datetime", "date", "time", "datetime-local"):
				dt_val = dateutil.parser.parse(val["value"])
				if val_type == "date":
					return dt_val.date()
				elif val_type == "time":
					return dt_val.time()
				else:
					return dt_val
			else:
				raise ValueError(val_type)
		else:
			vals = dict()
			for k,v in val.items():
				vals[k] = json_to_python(v)
			return vals

	elif isinstance(val, list):
		vals = list()
		for v in val:
			vals.append(json_to_python(v))
		return vals

	else:
		raise ValueError(str(type(val)))

def python_to_tomlpp(node):
	if isinstance(node, dict):
		table = TomlPPTable()
		for key, val in node.items():
			table.values[key] = python_to_tomlpp(val)
		return table
	elif isinstance(node, (set, list, tuple)):
		array = TomlPPArray()
		for val in node:
			array.values.append(python_to_tomlpp(val))
		return array
	else:
		return node


class TomlTest:

	def __init__(self, file_path, name, is_valid_case):
		self.__name = name
		self.__identifier = sanitize(name)
		self.__data = utils.read_all_text_from_file(file_path).strip()
		self.condition = ''
		self.requires_unicode = False
		if is_valid_case:
			self.__expected = True
			path_base = path.splitext(file_path)[0]
			yaml_file = path_base + '.yaml'
			if path.exists(yaml_file):
				self.__expected = python_to_tomlpp(yaml.load(
					utils.read_all_text_from_file(yaml_file),
					Loader=yaml.FullLoader
				))
			else:
				json_file = path_base + '.json'
				if path.exists(json_file):
					self.__expected = python_to_tomlpp(json_to_python(json.loads(
						utils.read_all_text_from_file(json_file),
					)))

		else:
			self.__expected = False

	def name(self):
		return self.__name

	def identifier(self):
		return self.__identifier

	def data(self):
		return self.__data

	def expected(self):
		return self.__expected

	def __str__(self):
		return 'static constexpr auto {} = R"({})"sv;'.format(
			self.__identifier,
			self.__data,
		)



def load_tests(source_folder, is_valid_set, ignore_list):
	tests = []
	files = [(fp, path.splitext(path.split(fp)[1])[0]) for fp in utils.get_all_files(source_folder, all="*.toml")]
	for file_path,name in files:
		if ignore_list and name in ignore_list:
			continue
		tests.append(TomlTest(file_path, name, is_valid_set))
	return tests



def set_condition(tests, condition, names):
	for test in tests:
		if test.name() in names:
			test.condition = condition



def load_valid_inputs(tests, extern_root):
	tests['valid']['burntsushi'] = load_tests(path.join(extern_root, 'toml-test', 'tests', 'valid'), True, (
		# newline/escape handling tests. these get broken by I/O (I test them separately)
		'string-escapes',
		# bugged: https://github.com/BurntSushi/toml-test/issues/58
		'datetime'
	))
	tests['valid']['iarna'] = load_tests(path.join(extern_root, 'toml-spec-tests', 'values'), True, (
		# these are stress-tests for 'large' datasets. I test these separately. Having them inline in C++ code is insane.
		'qa-array-inline-1000',
		'qa-array-inline-nested-1000',
		'qa-key-literal-40kb',
		'qa-key-string-40kb',
		'qa-scalar-literal-40kb',
		'qa-scalar-literal-multiline-40kb',
		'qa-scalar-string-40kb',
		'qa-scalar-string-multiline-40kb',
		'qa-table-inline-1000',
		'qa-table-inline-nested-1000',
		# newline/escape handling tests. these get broken by I/O (I test them separately)
		'spec-newline-1',
		'spec-newline-2',
		'spec-newline-3',
		'spec-string-escape-1',
		'spec-string-escape-2',
		'spec-string-escape-3',
		'spec-string-escape-4',
		'spec-string-escape-5',
		'spec-string-escape-6',
		'spec-string-escape-7',
		'spec-string-escape-8',
		'spec-string-escape-9',
		# bugged: https://github.com/iarna/toml-spec-tests/issues/3
		'spec-date-time-6',
		'spec-date-time-local-2',
		'spec-time-2',
		# breaks gcc:
		'spec-string-basic-multiline-4',
	))



def load_invalid_inputs(tests, extern_root):
	tests['invalid']['burntsushi'] = load_tests(path.join(extern_root, 'toml-test', 'tests', 'invalid'), False, (
		# false negatives after TOML 0.4.0
		'array-mixed-types-arrays-and-ints',
		'array-mixed-types-ints-and-floats',
		'array-mixed-types-strings-and-ints'
	))
	set_condition(tests['invalid']['burntsushi'], '!TOML_LANG_UNRELEASED', (
		'datetime-malformed-no-secs',
		'inline-table-linebreak',
		'multi-line-inline-table',
		'string-byte-escapes'
	))

	tests['invalid']['iarna'] = load_tests(path.join(extern_root, 'toml-spec-tests', 'errors'), False, (
		# I test these explicitly in the other test files (they get broken by I/O)
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
		'string-literal-multiline-control-4'
	))
	set_condition(tests['invalid']['iarna'], '!TOML_LANG_UNRELEASED', (
		'inline-table-trailing-comma',
	))



def requires_unicode(s):
	for c in s:
		if ord(c) > 127:
			return True
	return False



def write_test_file(name, test_cases):

	conditions = set()
	for test in test_cases:
		conditions.add(test.condition)

	test_file_path = path.join(utils.get_script_folder(), '..', 'tests', 'conformance_{}.cpp'.format(sanitize(name.strip())))
	print("Writing to {}".format(test_file_path))
	with open(test_file_path, 'w', encoding='utf-8', newline='\n') as test_file:
		write = lambda txt: print(txt, file=test_file)

		# preamble
		write('// This file is a part of toml++ and is subject to the the terms of the MIT license.')
		write('// Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>')
		write('// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.')
		write('// SPDX-License-Identifier: MIT')
		write('//-----')
		write('// this file was generated by generate_conformance_tests.py - do not modify it directly')
		write('')
		write('#include "tests.h"')
		write('using namespace toml::impl;')
		write('')

		# test data
		write('TOML_DISABLE_WARNINGS // unused variable spam')
		write('')
		write('namespace')
		write('{')
		for test in test_cases:
			s = '\t{}'.format(test)
			test.requires_unicode = requires_unicode(s)
			if test.requires_unicode:
				write('\t#if UNICODE_LITERALS_OK')
				write(s)
				write('\t#endif // UNICODE_LITERALS_OK')
			else:
				write(s)
		write('}')
		write('')
		write('TOML_ENABLE_WARNINGS')
		write('')

		# tests
		write('TEST_CASE("conformance - {}")'.format(name))
		write('{')
		for condition in conditions:
			if condition != '':
				write('')
				write('\t#if {}'.format(condition));
			for test in test_cases:
				if test.condition != condition:
					continue
				expected = test.expected()
				if isinstance(expected, bool):
					if expected:
						write('\tparsing_should_succeed(FILE_LINE_ARGS, {});'.format(test.identifier()))
					else:
						write('\tparsing_should_fail(FILE_LINE_ARGS, {});'.format(test.identifier()))
				else:
					s = expected.render('\t\t')
					if not test.requires_unicode:
						test.requires_unicode = requires_unicode(s)
					if test.requires_unicode:
						write('\t#if UNICODE_LITERALS_OK')
					write('\tparsing_should_succeed(FILE_LINE_ARGS, {}, [](toml::table&& tbl)'.format(test.identifier()))
					write('\t{')
					write('\t\tauto expected = {};'.format(s))
					write('\t\tREQUIRE(tbl == expected);')
					write('\t});')
					if test.requires_unicode:
						write('\t#endif // UNICODE_LITERALS_OK')
					write('')
			if condition != '':
				write('\t#endif // {}'.format(condition));
		write('}')
		write('')


def main():
	extern_root = path.join(utils.get_script_folder(), '..', 'extern')

	tests = { 'valid': dict(), 'invalid': dict() }
	load_valid_inputs(tests, extern_root)
	load_invalid_inputs(tests, extern_root)
	for test_type, test_groups in tests.items():
		for test_group, test_cases in test_groups.items():
			write_test_file('{}/{}'.format(test_group, test_type), test_cases )


if __name__ == '__main__':
	utils.run(main)
