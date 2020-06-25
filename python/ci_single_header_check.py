#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import os.path as path
import utils
import hashlib



def main():
	hpp_path = path.join(utils.get_script_folder(), '..', 'toml.hpp')
	hash1 = hashlib.sha1(utils.read_all_text_from_file(hpp_path).encode('utf-8')).hexdigest()
	print("Hash 1: {}".format(hash1))
	utils.run_python_script('generate_single_header.py')
	hash2 = hashlib.sha1(utils.read_all_text_from_file(hpp_path).encode('utf-8')).hexdigest()
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
	utils.run(main)
