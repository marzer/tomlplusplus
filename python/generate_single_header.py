#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

import sys
import re
import os
import os.path as path
import traceback




def get_script_folder():
	return path.dirname(path.realpath(sys.argv[0]))



def read_all_text_from_file(path):
	print("Reading {}".format(path))
	with open(path, 'r', encoding='utf-8') as file:
		text = file.read()
	return text



def make_divider(text = None, text_col = 40):
	if (text is None):
		return "//" + ('-' * 118)
	else:
		text = "//{}  {}  ".format('-' * (text_col - 2),text);
		if (len(text) < 120):
			return text + ('-' * (120 - len(text)))
		else:
			return text



class Preprocessor:

	def __init__(self):
		pass

	def preprocess(self, match):

		raw_incl = match if isinstance(match, str) else match.group(1)
		incl = raw_incl.strip().lower()
		if incl in self.processed_includes:
			return ''

		self.processed_includes.append(incl)
		text = read_all_text_from_file(path.join(get_script_folder(), '..', 'include', 'toml++', incl))
		text = re.sub(r'^\s*#\s*pragma\s+once\s*$', '', text, 0, re.I | re.M)
		text = re.sub(r'^\s*//\s*clang-format\s+(?:off|on)\s*$', '', text, 0, re.I | re.M)
		
		self.current_level += 1
		text = re.sub(r'^\s*#\s*include\s+"(.+?)"', lambda m : self.preprocess(m), text, 0, re.I | re.M)
		self.current_level -= 1

		if (self.current_level == 1):
			header_text = '↓ ' + raw_incl
			lpad = 28 + ((25 * (self.header_indent % 4)) - int((len(header_text) + 4) / 2))
			self.header_indent += 1
			return '\n{}\n#pragma region {}\n\n{}\n\n#pragma endregion {}\n{}'.format(
				make_divider(header_text, lpad), '', text, '', make_divider('↑ ' + raw_incl, lpad))
		else:
			return text

	def __call__(self, file):
		self.processed_includes = []
		self.header_indent = 0
		self.current_level = 0
		return self.preprocess(file)



def main():

	# preprocess header(s)
	source_text = Preprocessor()('toml.h')
	source_text = re.sub('\r\n', '\n', source_text, 0, re.I | re.M) # convert windows newlines
	source_text = re.sub('(?:(?:\n|^)[ \t]*//[/#!<]+[^\n]*)+\n', '\n', source_text, 0, re.I | re.M) # remove 'magic' comment blocks
	source_text = re.sub('(?:///[<].*?)\n', '\n', source_text, 0, re.I | re.M) # remove inline doxy briefs
	source_text = re.sub('\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', source_text, 0, re.I | re.M) # remove double newlines
	source_text = re.sub('([^ \t])[ \t]+\n', '\\1\n', source_text, 0, re.I | re.M) # remove trailing whitespace
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
	source_text = source_text.strip()

	# extract library version
	library_version = [0,0,0]
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_MAJOR\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version[0] = match.group(1)
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_MINOR\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version[1] = match.group(1)
	match = re.search(r'^\s*#\s*define\s+TOML_LIB_(?:REVISION|PATCH)\s+([0-9]+)\s*$', source_text, re.I | re.M)
	if match is not None:
		library_version[2] = match.group(1)

	# build the preamble (license etc)
	preamble = []
	preamble.append('''
toml++ v{}
https://github.com/marzer/tomlplusplus
SPDX-License-Identifier: MIT'''.format('.'.join(str(x) for x in library_version)))
	preamble.append('''
-         THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY            -

If you wish to submit a contribution to toml++, hooray and thanks! Before you crack on, please be aware that this
file was assembled from a number of smaller files by a python script, and code contributions should not be made
against it directly. You should instead make your changes in the relevant source file(s). The file names of the files
that contributed to this header can be found at the beginnings and ends of the corresponding sections of this file.''')
	preamble.append('''
TOML language specification:
Latest: https://github.com/toml-lang/toml/blob/master/README.md
v0.5.0: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md''')
	preamble.append(read_all_text_from_file(path.join(get_script_folder(), '..', 'LICENSE')))

	# write the output file
	output_file_path = path.join(get_script_folder(), '..', 'toml.hpp')
	print("Writing to {}".format(output_file_path))
	with open(output_file_path,'w', encoding='utf-8', newline='\n') as output_file:
		if (len(preamble) > 0):
			print(make_divider(), file=output_file)
		for pre in preamble:
			print('//', file=output_file)
			for line in pre.strip().splitlines():
				print('//', file=output_file, end = '')
				if (len(line) > 0):
					print(' ', file=output_file, end = '')
					print(line, file=output_file)
				else:
					print('\n', file=output_file, end = '')
			print('//', file=output_file)
			print(make_divider(), file=output_file)
		print('''// clang-format off
#pragma once
#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
''', file=output_file)
		print(source_text, file=output_file)
		print('''
#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif
// clang-format on''', file=output_file)



if __name__ == '__main__':
	try:
		main()
	except Exception as err:
		print(
			'Fatal error: [{}] {}'.format(
				type(err).__name__,
				str(err)
			),
			file=sys.stderr
		)
		traceback.print_exc(file=sys.stderr)
		sys.exit(1)
	sys.exit()
