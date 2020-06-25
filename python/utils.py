#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import os
import os.path as path
import subprocess
import traceback
import shutil
import fnmatch



def is_tool(name):
	return shutil.which(name) is not None



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



def repeat_pattern(pattern, count):
	if len(pattern) == 1:
		return pattern * count

	text = ''
	for i in range(0, count):
		text = text + pattern[i % len(pattern)]
	return text



def delete_directory(dir_path):
	if (path.exists(dir_path)):
		print('Deleting {}'.format(dir_path))
		shutil.rmtree(dir_path)



def is_collection(val):
	if isinstance(val, (list, tuple, dict, set, range)):
		return True
	return False



def get_all_files(dir, all=None, any=None):
	files = [f for f in [path.join(dir, f) for f in os.listdir(dir)] if path.isfile(f)]
	if (files and all is not None):
		if (not is_collection(all)):
			all = (all,)
		all = [f for f in all if f is not None]
		for fil in all:
			files = fnmatch.filter(files, fil)
				
	if (files and any is not None):
		if (not is_collection(any)):
			any = (any,)
		any = [f for f in any if f is not None]
		if any:
			results = set()
			for fil in any:
				results.update(fnmatch.filter(files, fil))
			files = [f for f in results]
	return files



def run(main_func):
	try:
		result = main_func()
		if result is None:
			sys.exit(0)
		else:
			sys.exit(int(result))
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
