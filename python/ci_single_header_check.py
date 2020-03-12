#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

import sys
import os
import os.path as path
import traceback
import hashlib
import subprocess
from shutil import which



def is_tool(name):
	return which(name) is not None



def get_script_folder():
	return path.dirname(path.realpath(sys.argv[0]))



def read_all_text_from_file(path):
	print("Reading {}".format(path))
	with open(path, 'r', encoding='utf-8') as file:
		text = file.read()
	return text



def run_python_script(script_path, *args):
	subprocess.check_call(
		['py' if is_tool('py') else 'python3', script_path] + [arg for arg in args]
	)



def main():
	hpp_path = path.join(get_script_folder(), '..', 'toml.hpp')
	hash1 = hashlib.sha1(read_all_text_from_file(hpp_path).encode('utf-8')).hexdigest()
	print("Hash 1: {}".format(hash1))
	run_python_script('generate_single_header.py')
	hash2 = hashlib.sha1(read_all_text_from_file(hpp_path).encode('utf-8')).hexdigest()
	print("Hash 2: {}".format(hash2))
	if (hash1 != hash2):
		print(
			"toml.hpp wasn't up-to-date!\nRun generate_single_header.py before your commit to prevent this error.",
			file=sys.stderr
		)
		return 1
	print("toml.hpp was up-to-date")
	return 0



if __name__ == '__main__':
	try:
		sys.exit(main())
	except Exception as err:
		print(
			'Fatal error: [{}] {}'.format(
				type(err).__name__,
				str(err)
			),
			file=sys.stderr
		)
		traceback.print_exc(file=sys.stderr)
		sys.exit(-1)
