#!/usr/bin/env python3

import sys
import re
import os
import os.path
import traceback




def get_script_folder():
	return os.path.dirname(os.path.realpath(sys.argv[0]))



def read_all_text_from_file(path):
	print("Reading {}".format(path))
	file = open(path,'r')
	text = file.read()
	file.close()
	return text



def make_divider(text = None, text_col = 40):
	if (text is None):
		return "//" + ('-' * 98)
	else:
		text = "//{}  {}  ".format('-' * (text_col - 2),text);
		if (len(text) < 100):
			return text + ('-' * (100 - len(text)))
		else:
			return text



preprocessed_include_headers = 0
preprocessed_includes = []
preprocess_include_level = -1
def preprocess(match):
	global preprocessed_includes
	global preprocess_include_level
	global preprocessed_include_headers

	raw_incl = match if isinstance(match, str) else match.group(1)
	incl = raw_incl.strip().lower()
	if incl in preprocessed_includes:
		return ''

	preprocessed_includes.append(incl)
	text = read_all_text_from_file(os.path.join(get_script_folder(), r'..\include\toml++\{}'.format(incl)))

	if preprocess_include_level >= 0:
		text = re.sub(r'^\s*#\s*pragma\s*once\s*$', '', text, 0, re.I | re.M)

	preprocess_include_level += 1
	text = re.sub(r'^\s*#\s*include\s+"(.+?)"', preprocess, text, 0, re.I | re.M)
	preprocess_include_level -= 1

	if (preprocess_include_level == 0):
		lpad = 35 + (preprocessed_include_headers % 3) * 20
		preprocessed_include_headers += 1
		return '''
{}
#pragma region {}

{}

#pragma endregion {}
{}'''.format(make_divider('↓ ' + raw_incl, lpad), raw_incl, text, raw_incl, make_divider('↑ ' + raw_incl, lpad))
	else:
		return text



def main():

	# build the preamble (license etc)
	preamble = []
	preamble.append('''toml++
https://github.com/marzer/tomlplusplus''')
	preamble.append('''*** THIS HEADER WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT ***
If you wish to submit a change to toml++, ensure you're modifying the
relevant source file when you do so. Modifying this file directly will
just see your change lost the next time this file is regenerated.''')
	preamble.append('''TOML language documentation:
Latest: https://github.com/toml-lang/toml
v0.5.0: https://github.com/toml-lang/toml/tree/v0.5.0''')
	preamble.append(read_all_text_from_file(os.path.join(get_script_folder(), r'..\LICENSE')))

	# preprocess header(s)
	source_text = preprocess('toml.h')
	source_text = re.sub('\r\n', '\n', source_text, 0, re.I | re.M)
	source_text = re.sub(r'^[ \t]*//[/#!].+?$', '', source_text, 0, re.I | re.M)
	source_text = re.sub('\n([ \t]*\n[ \t]*)+\n', '\n\n', source_text, 0, re.I | re.M)
	source_text = re.sub('([^ \t])[ \t]+\n', '\\1\n', source_text, 0, re.I | re.M)
	source_text = source_text.strip()
	
	# write the output file
	output_file_path = os.path.join(get_script_folder(), r'..\toml.hpp')
	print("Writing to {}".format(output_file_path))
	output_file = open(output_file_path,'w', encoding='utf-8')
	if (len(preamble) > 0):
		print(make_divider(), file=output_file)
	for pre in preamble:
		print('//', file=output_file)
		for line in pre.splitlines():
			print('//', file=output_file, end = '')
			if (len(line) > 0):
				print(' ', file=output_file, end = '')
				print(line, file=output_file)
			else:
				print('\n', file=output_file, end = '')
		print('//', file=output_file)
		print(make_divider(), file=output_file)
	print(source_text, file=output_file)
	output_file.close()



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
