#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import os
import sys
import urllib.request
from pathlib import Path

VENDOR_DIR = Path(__file__).resolve().parents[1] / 'vendor'
CATCH2_URL = 'https://raw.githubusercontent.com/catchorg/Catch2/v3.16.1/extras'
JSON_URL = 'https://raw.githubusercontent.com/nlohmann/json/v3.12.0/single_include/nlohmann'
FILES = {
    'catch_amalgamated.hpp': rf'{CATCH2_URL}/catch_amalgamated.hpp',
    'catch_amalgamated.cpp': rf'{CATCH2_URL}/catch_amalgamated.cpp',
    'json.hpp': rf'{JSON_URL}/json.hpp',
}


def main():
    downloads = {}
    for name, url in FILES.items():
        print(rf'Downloading {url}')
        with urllib.request.urlopen(url, timeout=60) as response:
            downloads[name] = response.read()

    for name, content in downloads.items():
        path = VENDOR_DIR / name
        temp = path.with_name(rf'{name}.{os.getpid()}.tmp')
        temp.write_bytes(content)
        os.replace(temp, path)
    return 0


if __name__ == '__main__':
    sys.exit(main())
