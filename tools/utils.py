#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import subprocess
import sys
import traceback
from pathlib import Path


def entry_script_dir():
    return Path(sys.argv[0]).resolve().parent


def apply_clang_format(text, cwd=None):
    return subprocess.run(
        'clang-format --style=file'.split(),
        check=True,
        capture_output=True,
        cwd=str(Path.cwd() if cwd is None else cwd),
        encoding='utf-8',
        input=text,
    ).stdout


def run(main_func, verbose=False):
    try:
        result = main_func()
        if result is None:
            sys.exit(0)
        else:
            sys.exit(int(result))
    except Exception as err:
        if verbose:
            traceback.print_exc()
        else:
            print(err, file=sys.stderr)
        sys.exit(-1)
