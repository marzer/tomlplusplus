#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import utils
import re
from pathlib import Path
from io import StringIO

class Preprocessor:

	__re_strip_blocks = re.compile(r'//[#!]\s*[{][{].*?//[#!]\s*[}][}]*?\n', re.I | re.S)
	__re_includes = re.compile(r'^\s*#\s*include\s+"(.+?)"', re.I | re.M)

	def __preprocess(self, match):

		raw_incl = match if isinstance(match, str) else match.group(1)
		incl = raw_incl.strip().lower()
		if incl in self.__processed_includes:
			return ''

		self.__processed_includes.append(incl)
		text = utils.read_all_text_from_file(Path(utils.entry_script_dir(), '..', 'include', 'toml++', incl).resolve(), logger=True).strip() + '\n'
		text = text.replace('\r\n', '\n') # convert windows newlines
		text = self.__re_strip_blocks.sub('', text, 0) # strip {{ }} blocks
		self.__current_level += 1
		text = self.__re_includes.sub(lambda m : self.__preprocess(m), text, 0)
		self.__current_level -= 1

		if (self.__current_level == 1):
			header_text = '↓ ' + raw_incl
			lpad = 20 + ((25 * (self.__header_indent % 4)) - int((len(header_text) + 4) / 2))
			self.__header_indent += 1
			text = '#if 1  {}\n{}\n\n#endif {}\n'.format(
				utils.make_divider(header_text, lpad, line_length=113),
				text,
				utils.make_divider('↑ ' + raw_incl, lpad, line_length=113)
			)

		return '\n\n' + text + '\n\n' # will get merged later

	def __init__(self, file):
		self.__processed_includes = []
		self.__header_indent = 0
		self.__current_level = 0
		self.__string = self.__preprocess(file)

	def __str__(self):
		return self.__string



def main():

	# preprocess header(s)
	source_text = str(Preprocessor('toml.h'))

	# strip various things:
	# 'pragma once'
	source_text = re.sub(r'^\s*#\s*pragma\s+once\s*$', '', source_text, 0, re.I | re.M)
	# clang-format directives
	source_text = re.sub(r'^\s*//\s*clang-format\s+.+?$', '', source_text, 0, re.I | re.M)
	# spdx license identifiers
	source_text = re.sub(r'^\s*//\s*SPDX-License-Identifier:.+?$', '', source_text, 0, re.I | re.M)
	# 'magic' comment blocks (incl. doxygen)
	source_text = re.sub('(?:(?:\n|^)[ \t]*//[/#!<]+[^\n]*)+\n', '\n', source_text, 0, re.I | re.M)
	# 'magic' comments (incl. doxygen)
	source_text = re.sub('(?://[/#!<].*?)\n', '\n', source_text, 0, re.I | re.M)
	# remove trailing whitespace
	source_text = re.sub('([^ \t])[ \t]+\n', '\\1\n', source_text, 0, re.I | re.M)
	# bookended namespace blocks
	source_text = re.sub('}\n+TOML_NAMESPACE_END\n+TOML_NAMESPACE_START\n+{\n+', '\n', source_text, 0, re.I | re.M)
	source_text = re.sub('}\n+TOML_IMPL_NAMESPACE_END\n+TOML_IMPL_NAMESPACE_START\n+{\n+', '\n', source_text, 0, re.I | re.M)
	# blank lines before some preprocessor directives
	#source_text = re.sub('\n+\n(\s*)#\s*(elif|else|endif)(.*?)\n', '\n\\1#\\2\\3\n', source_text, 0, re.I | re.M)
	# blank lines after some preprocessor directives
	#source_text = re.sub('#\s*(if|ifn?def|elif|else)(.*?)\n\n+', '#\\1\\2\n', source_text, 0, re.I | re.M)
	# blank lines after opening braces
	source_text = re.sub('[{]\s*\n\s*\n+', '{\n', source_text, 0, re.I | re.M)
	# double newlines
	source_text = re.sub('\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', source_text, 0, re.I | re.M)

	# source_text = re.sub(  # blank lines between various preprocessor directives
	# 	'[#](endif(?:\s*//[^\n]*)?)\n{2,}[#](ifn?(?:def)?|define)',
	# 	'#\\1\n#\\2',
	# 	source_text, 0, re.I | re.M
	# )
	return_type_pattern														\
		= r'(?:'															\
		+ r'(?:\[\[nodiscard\]\]\s*)?'										\
		+ r'(?:(?:friend|explicit|virtual|inline|const|operator)\s+)*'		\
		+ r'(?:'															\
			+ r'bool|int64_t|(?:const_)?iterator|double|void'				\
			+ r'|node(?:_(?:view|of)<.+?>|)?|table|array|value(?:<.+?>)?'	\
			+ r'|T|U|parse_(?:error|result)'								\
		+ r')'																\
		+ r'(?:\s*[&*]+)?'													\
		+ r'(?:\s*[(]\s*[)])?'												\
		+ r'\s+'															\
		+ r')'
	blank_lines_between_returns_pattern = '({}[^\n]+)\n\n([ \t]*{})'.format(return_type_pattern, return_type_pattern)
	for i in range(0, 5): # remove blank lines between simple one-liner definitions
		source_text = re.sub('(using .+?;)\n\n([ \t]*using)', '\\1\n\\2', source_text, 0, re.I | re.M)
		source_text = re.sub(
				'([a-zA-Z_][a-zA-Z0-9_]*[ \t]+[a-zA-Z_][a-zA-Z0-9_]*[ \t]*;)'	\
				+ '\n\n([ \t]*[a-zA-Z_][a-zA-Z0-9_]*[ \t]+[a-zA-Z_][a-zA-Z0-9_]*[ \t]*;)', '\\1\n\\2',
				source_text, 0, re.I | re.M)
		source_text = re.sub(blank_lines_between_returns_pattern, '\\1\n\\2', source_text, 0, re.I | re.M)
	source_text = source_text.strip() + '\n'

	# change TOML_LIB_SINGLE_HEADER to 1
	source_text = re.sub(
		'#\s*define\s+TOML_LIB_SINGLE_HEADER\s+[0-9]+',
		'#define	TOML_LIB_SINGLE_HEADER 1',
		source_text, 0, re.I
	)

	# extract library version
	library_version = {
		'major': 0,
		'minor': 0,
		'patch': 0
	}
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_MAJOR\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version['major'] = match.group(1)
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_MINOR\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version['minor'] = match.group(1)
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_(?:REVISION|PATCH)\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version['patch'] = match.group(1)

	# build the preamble (license etc)
	preamble = []
	preamble.append('''
// toml++ v{major}.{minor}.{patch}
// https://github.com/marzer/tomlplusplus
// SPDX-License-Identifier: MIT'''.format(**library_version))
	preamble.append('''
// -         THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY            -
//
// If you wish to submit a contribution to toml++, hooray and thanks! Before you crack on, please be aware that this
// file was assembled from a number of smaller files by a python script, and code contributions should not be made
// against it directly. You should instead make your changes in the relevant source file(s). The file names of the files
// that contributed to this header can be found at the beginnings and ends of the corresponding sections of this file.''')
	preamble.append('''
// TOML Language Specifications:
// latest:      https://github.com/toml-lang/toml/blob/master/README.md
// v1.0.0:      https://toml.io/en/v1.0.0
// v0.5.0:      https://toml.io/en/v0.5.0
// changelog:   https://github.com/toml-lang/toml/blob/master/CHANGELOG.md''')
	preamble.append(utils.read_all_text_from_file(Path(utils.entry_script_dir(), '..', 'LICENSE').resolve(), logger=True))

	# write the output
	with StringIO(newline='\n') as output:

		# build in a string buffer
		write = lambda txt, end='\n': print(txt, file=output, end=end)
		if (len(preamble) > 0):
			write(utils.make_divider())
		for pre in preamble:
			write('//')
			for line in pre.strip().splitlines():
				if len(line) == 0:
					write('//')
					continue
				if not line.startswith('//'):
					write('// ', end = '')
				write(line)
			write('//')
			write(utils.make_divider())
		write(source_text)
		write('')

		output_str = output.getvalue().strip()

		# analyze the output to find any potentially missing #undefs
		re_define = re.compile(r'^\s*#\s*define\s+([a-zA-Z0-9_]+)(?:$|\s|\()')
		re_undef = re.compile(r'^\s*#\s*undef\s+([a-zA-Z0-9_]+)(?:$|\s|//)')
		defines = dict()
		for output_line in output_str.splitlines():
			defined = True
			m = re_define.match(output_line)
			if not m:
				defined = False
				m = re_undef.match(output_line)
			if m:
				defines[m.group(1)] = defined
		ignore_list = ( # macros that are meant to stay public (user configs etc)
			r'INCLUDE_TOMLPLUSPLUS_H',
			r'TOML_API',
			r'TOML_UNRELEASED_FEATURES',
			r'TOML_LARGE_FILES',
			r'TOML_PARSER',
			r'TOML_WINDOWS_COMPAT',
			r'TOML_EXCEPTIONS',
			r'TOML_LIB_SINGLE_HEADER',
			r'TOML_LIB_MAJOR',
			r'TOML_LIB_MINOR',
			r'TOML_LIB_PATCH',
			r'TOML_LANG_MAJOR',
			r'TOML_LANG_MINOR',
			r'TOML_LANG_PATCH',
			r'TOML_UNDEF_MACROS',
			r'TOML_HEADER_ONLY',
			r'TOML_ALL_INLINE',
			r'TOML_MAX_NESTED_VALUES'
		)
		set_defines = []
		for define, currently_set in defines.items():
			if currently_set and define not in ignore_list:
				set_defines.append(define)
		if len(set_defines) > 0:
			set_defines.sort()
			print(f"Potentially missing #undefs:")
			for define in set_defines:
				print(f"\t#undef {define}")

		# write the output file
		output_file_path = Path(utils.entry_script_dir(), '..', 'toml.hpp').resolve()
		print("Writing to {}".format(output_file_path))
		with open(output_file_path,'w', encoding='utf-8', newline='\n') as output_file:
			print(output_str, file=output_file)


if __name__ == '__main__':
	utils.run(main, verbose=True)
