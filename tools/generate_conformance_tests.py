#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import utils
import io
import re
import json
import yaml
import math
import dateutil.parser
from pathlib import Path
from datetime import datetime, date, time



def sanitize(s):
	s = re.sub(r'[ _:;\/-]+', '_', s, 0, re.I | re.M)
	if s in ('bool', 'float', 'int', 'double', 'auto', 'array', 'table'):
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
					s += ' ' + val.render(indent + '\t\t')
					s += '\n' + indent + '\t'
				else:
					s += '{}, {} '.format(python_value_to_tomlpp(str(key)), python_value_to_tomlpp(val))
				s += '},'
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
			elif val_type in ("datetime", "date", "time", "datetime-local", "date-local", "time-local"):
				dt_val = dateutil.parser.parse(val["value"])
				if val_type in ("date", "date-local"):
					return dt_val.date()
				elif val_type in ("time", "time-local"):
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

	def __init__(self, file_path, is_valid_case):
		self.__name = file_path.stem
		self.__identifier = sanitize(self.__name)
		self.__group = self.__identifier.strip('_').split('_')[0]
		self.__data = utils.read_all_text_from_file(file_path, logger=True).strip()
		self.__conditions = []
		if is_valid_case:
			self.__expected = True
			path_base = str(Path(file_path.parent, file_path.stem))
			yaml_file = Path(path_base + r'.yaml')
			if yaml_file.exists():
				self.__expected = python_to_tomlpp(yaml.load(
					utils.read_all_text_from_file(yaml_file, logger=True),
					Loader=yaml.FullLoader
				))
			else:
				json_file = Path(path_base + r'.json')
				if json_file.exists():
					self.__expected = python_to_tomlpp(json_to_python(json.loads(
						utils.read_all_text_from_file(json_file, logger=True),
					)))

		else:
			self.__expected = False

	def name(self):
		return self.__name

	def identifier(self):
		return self.__identifier

	def group(self):
		return self.__group

	def add_condition(self, cond):
		self.__conditions.append(cond)
		return self

	def condition(self):
		if not self.__conditions or not self.__conditions[0]:
			return ''
		if len(self.__conditions) == 1:
			return rf'{self.__conditions[0]}'
		return rf'{" && ".join([rf"{c}" for c in self.__conditions])}'

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
	source_folder = source_folder.resolve()
	utils.assert_existing_directory(source_folder)
	files = utils.get_all_files(source_folder, all="*.toml")
	if ignore_list:
		files_ = []
		for f in files:
			ignored = False
			for ignore in ignore_list:
				if isinstance(ignore, str):
					if f.stem == ignore:
						ignored = True
						break
				elif ignore.fullmatch(f.stem) is not None: # regex
					ignored = True
					break
			if not ignored:
				files_.append(f)
		files = files_
	tests = []
	for f in files:
		try:
			tests.append(TomlTest(f, is_valid_set))
		except Exception as e:
			print(rf'Error reading {f}, skipping...', file=sys.stderr)
	return tests



def add_condition(tests, condition, names):
	for test in tests:
		matched = False
		for name in names:
			if isinstance(name, str):
				if test.name() == name:
					matched = True
					break
			elif name.fullmatch(test.name()) is not None: # regex
				matched = True
				break
		if matched:
			test.add_condition(condition)



def load_valid_inputs(tests, extern_root):
	tests['valid']['burntsushi'] = load_tests(Path(extern_root, 'toml-test', 'tests', 'valid'), True, (
		# newline/escape handling tests. these get broken by I/O (I test them separately)
		'string-escapes',
		# causes MSVC to run out of heap space during compilation O_o
		'inline-table-key-dotted',
		# broken by the json reader
		'key-alphanum',
		# breaks clang:
		'multiline-string',
	))
	tests['valid']['iarna'] = load_tests(Path(extern_root, 'toml-spec-tests', 'values'), True, (
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
		re.compile(r'spec-newline-.*'),
		re.compile(r'spec-string-escape-.*'),
		# bugged: https://github.com/iarna/toml-spec-tests/issues/3
		'spec-date-time-6',
		'spec-date-time-local-2',
		'spec-time-2',
		# breaks gcc:
		'spec-string-basic-multiline-4',
	))



def load_invalid_inputs(tests, extern_root):
	tests['invalid']['burntsushi'] = load_tests(Path(extern_root, 'toml-test', 'tests', 'invalid'), False, (
		# these break IO/git/visual studio (i test them elsewhere)
		re.compile('.*(bom|control).*'),
		'encoding-utf16',
	))
	add_condition(tests['invalid']['burntsushi'], '!TOML_LANG_UNRELEASED', (
		'datetime-no-secs',
		re.compile(r'inline-table-linebreak-.*'),
		'inline-table-trailing-comma',
		'key-special-character',
		'multi-line-inline-table',
		'string-basic-byte-escapes',
	))

	tests['invalid']['iarna'] = load_tests(Path(extern_root, 'toml-spec-tests', 'errors'), False, (
		# these break IO/git/visual studio (i test them elsewhere)
		re.compile('.*(bom|control).*'),
	))
	add_condition(tests['invalid']['iarna'], '!TOML_LANG_UNRELEASED', (
		'inline-table-trailing-comma',
	))



def requires_unicode(s):
	for c in s:
		if ord(c) > 127:
			return True
	return False



def write_test_file(name, all_tests):

	for test in all_tests:
		unicode = requires_unicode(str(test))
		if not unicode and not isinstance(test.expected(), bool):
			unicode = requires_unicode(test.expected().render())
		if unicode:
			test.add_condition(r'UNICODE_LITERALS_OK')

	tests_by_group = {}
	for test in all_tests:
		if test.group() not in tests_by_group:
			tests_by_group[test.group()] = {}
		cond = test.condition()
		if cond not in tests_by_group[test.group()]:
			tests_by_group[test.group()][cond] = []
		tests_by_group[test.group()][cond].append(test)
	all_tests = tests_by_group

	test_file_path = Path(utils.entry_script_dir(), '..', 'tests', rf'conformance_{sanitize(name.strip())}.cpp').resolve()
	print(rf'Writing to {test_file_path}')
	with open(test_file_path, 'w', encoding='utf-8', newline='\n') as test_file:
		write = lambda txt,end='\n': print(txt, file=test_file, end=end)

		# preamble
		write('// This file is a part of toml++ and is subject to the the terms of the MIT license.')
		write('// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>')
		write('// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.')
		write('// SPDX-License-Identifier: MIT')
		write('//-----')
		write('// this file was generated by generate_conformance_tests.py - do not modify it directly')
		write('')
		write('#include "tests.h"')
		write('using namespace toml::impl;')
		write('')

		# test data
		write('TOML_DISABLE_WARNINGS; // unused variable spam')
		write('')
		write('namespace')
		write('{', end='')
		for group, conditions in all_tests.items():
			for condition, tests in conditions.items():
				write('')
				if condition != '':
					write(f'#if {condition}');
					write('')
				for test in tests:
					write(f'\t{test}')
				if condition != '':
					write('')
					write(f'#endif // {condition}');
		write('}')
		write('')
		write('TOML_ENABLE_WARNINGS;')
		write('')

		# tests
		write(f'TEST_CASE("conformance - {name}")')
		write('{', end='')
		for group, conditions in all_tests.items():
			for condition, tests in conditions.items():
				if condition != '':
					write('')
					write(f'#if {condition}');
				for test in tests:
					write('')
					expected = test.expected()
					if isinstance(expected, bool):
						if expected:
							write(f'\tparsing_should_succeed(FILE_LINE_ARGS, {test.identifier()});')
						else:
							write(f'\tparsing_should_fail(FILE_LINE_ARGS, {test.identifier()});')
					else:
						s = expected.render('\t\t')
						write(f'\tparsing_should_succeed(FILE_LINE_ARGS, {test.identifier()}, [](toml::table&& tbl)')
						write('\t{')
						write(f'\t\tconst auto expected = {s};')
						write('\t\tREQUIRE(tbl == expected);')
						write('\t});')
				if condition != '':
					write('')
					write(f'#endif // {condition}');
		write('}')
		write('')



def main():
	extern_root = Path(utils.entry_script_dir(), '..', 'external').resolve()
	utils.assert_existing_directory(extern_root)
	assert extern_root.exists()
	all_tests = { 'valid': dict(), 'invalid': dict() }
	load_valid_inputs(all_tests, extern_root)
	load_invalid_inputs(all_tests, extern_root)
	for validity, sources in all_tests.items():
		for source, tests in sources.items():
			write_test_file('{}/{}'.format(source, validity), tests )



if __name__ == '__main__':
	utils.run(main, verbose=True)
