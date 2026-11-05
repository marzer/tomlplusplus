#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import os
import re
import sys
from argparse import ArgumentParser
from datetime import date
from io import StringIO
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION_FILE = ROOT / 'VERSION.txt'
TRIPLET = r'[0-9]+(?:[.][0-9]+){2}'
UNDEF_BLOCK = re.compile(r'(?m)^(#if TOML_UNDEF_MACROS\n)(?:#undef [A-Za-z0-9_]+\n)*(#endif)$')
PUBLIC_MACROS = (
    r'INCLUDE_TOMLPLUSPLUS_H',
    r'POXY_IMPLEMENTATION_DETAIL',
    r'TOML_ALL_INLINE',
    r'TOML_API',
    r'TOML_CALLCONV',
    r'TOML_CONCAT',
    r'TOML_CONCAT_1',
    r'TOML_CONFIG_HEADER',
    r'TOML_CUDA',
    r'TOML_ENABLE_FORMATTERS',
    r'TOML_ENABLE_PARSER',
    r'TOML_ENABLE_SIMD',
    r'TOML_ENABLE_UNRELEASED_FEATURES',
    r'TOML_ENABLE_WINDOWS_COMPAT',
    r'TOML_ENABLE_FLOAT16',
    r'TOML_EXCEPTIONS',
    r'TOML_EXPORTED_CLASS',
    r'TOML_EXPORTED_FREE_FUNCTION',
    r'TOML_EXPORTED_MEMBER_FUNCTION',
    r'TOML_EXPORTED_STATIC_FUNCTION',
    r'TOML_HEADER_ONLY',
    r'TOML_LANG_MAJOR',
    r'TOML_LANG_MINOR',
    r'TOML_LANG_PATCH',
    r'TOML_LIB_MAJOR',
    r'TOML_LIB_MINOR',
    r'TOML_LIB_PATCH',
    r'TOML_LIB_SINGLE_HEADER',
    r'TOML_MAX_NESTED_VALUES',
    r'TOML_MAX_DOTTED_KEYS_DEPTH',
    r'TOML_NAMESPACE_END',
    r'TOML_NAMESPACE_START',
    r'TOML_OPTIONAL_TYPE',
    r'TOML_SMALL_FLOAT_TYPE',
    r'TOML_SMALL_INT_TYPE',
    r'TOML_UNDEF_MACROS',
    r'TOMLPLUSPLUS_H',
    r'TOMLPLUSPLUS_HPP',
    r'TOML_SHARED_LIB',
    r'TOML_DISABLE_CONDITIONAL_NOEXCEPT_LAMBDA',
    r'TOML_DISABLE_NOEXCEPT_NOEXCEPT',
)


def read_text_file(path):
    with open(path, encoding='utf-8', newline='') as f:
        return f.read()


def write_text_file(path, text):
    print(rf'Regenerating {path.relative_to(ROOT)}')
    temp = path.with_name(rf'{path.name}.{os.getpid()}.tmp')
    with open(temp, 'w', encoding='utf-8', newline='') as f:
        f.write(text)
    os.replace(temp, path)


def repeat_pattern(pattern, count):
    if len(pattern) == 1:
        return pattern * count
    text = ''
    for i in range(0, count):
        text = text + pattern[i % len(pattern)]
    return text


def make_divider(text=None, text_col=40, pattern='-', line_length=120):
    if text is None:
        return "//" + repeat_pattern(pattern, line_length - 2)
    else:
        text = f"//{repeat_pattern(pattern, text_col - 2)}  {text}  "
        if len(text) < line_length:
            return text + repeat_pattern(pattern, line_length - len(text))
        else:
            return text


class Preprocessor:
    __re_includes = re.compile(r'^\s*#\s*include\s+"(.+?)".*?$', re.I | re.M)
    __re_pragma_once = re.compile(r'^\s*#\s*pragma\s+once\s*$', re.M)

    def __init__(self, file, read_file):
        self.__read_file = read_file
        self.__processed_files = set()
        self.__once_only = set()
        self.__directory_stack = [Path.cwd()]
        self.__include_stack = []
        self.__entry_root = ''
        self.__string = self.__preprocess(file)

    def __preprocess(self, incl):

        if not isinstance(incl, (Path, str)):  # a regex match object
            incl = incl.group(1).strip()
        if isinstance(incl, str):
            incl = Path(incl.strip().replace('\\', r'/'))
        if not incl.is_absolute():
            incl = Path(self.__directory_stack[-1], incl).resolve()
        self.__processed_files.add(incl)
        if incl in self.__once_only:
            return ''
        self.__include_stack.append(incl)

        text = self.__read_file(incl).strip() + '\n'
        text = text.replace('\r\n', '\n')  # convert windows newlines

        self.__directory_stack.append(incl.parent)
        if self.__re_pragma_once.search(text):
            self.__once_only.add(incl)
        if len(self.__include_stack) == 1 and self.__entry_root == '':
            self.__entry_root = str(incl.parent).replace('\\', r'/')
        if len(self.__include_stack) > 1:
            text = self.__re_pragma_once.sub('', text)

        text = self.__re_includes.sub(lambda m: self.__preprocess(m), text, 0)

        incl_normalized = str(incl).replace('\\', r'/')
        if incl_normalized.startswith(self.__entry_root):
            incl_normalized = incl_normalized[len(self.__entry_root) :].strip('/')

        if len(self.__include_stack) > 1 and incl_normalized not in (r'impl/header_start.hpp', r'impl/header_end.hpp'):
            header = make_divider(incl_normalized, 10, pattern=r'*')
            footer = ''
            if len(self.__include_stack) > 2:
                footer = str(self.__include_stack[-2]).replace('\\', r'/')
                if footer.startswith(self.__entry_root):
                    footer = footer[len(self.__entry_root) :].strip('/')
                footer = make_divider(footer, 10, pattern=r'*')

            text = f'\n\n{header}\n\n{text}\n\n{footer}'.rstrip()

        self.__include_stack.pop()
        self.__directory_stack.pop()
        return '\n\n' + text + '\n\n'

    def __str__(self):
        return self.__string

    def processed_files(self):
        out = list(self.__processed_files)
        out.sort()
        return out


def generate_single_header(version, read_file):

    # preprocess header(s)
    toml_h = str(Preprocessor(ROOT / 'include' / 'toml++' / 'toml.hpp', read_file))

    # strip various things:
    if 1:
        for i in range(3):
            # trailing whitespace
            toml_h = re.sub('([^ \t])[ \t]+\n', r'\1\n', toml_h)
            # explicit 'strip this' blocks
            toml_h = re.sub(r'(?:\n[ \t]*)?//[#!][ \t]*[{][{].*?//[#!][ \t]*[}][}].*?\n', '\n', toml_h, flags=re.S)
            # spdx license identifiers
            toml_h = re.sub(r'^\s*//\s*SPDX-License-Identifier:.+?$', '', toml_h, flags=re.I | re.M)
            # double blank lines
            toml_h = re.sub('\n(?:[ \t]*\n[ \t]*)+\n', '\n\n', toml_h)
            # magic comments
            blank_line = r'(?:[ \t]*\n)'
            comment_line = r'(?:[ \t]*//(?:[/#!<]| ?(?:---|===|\^\^\^|vvv))[^\n]*\n)'
            toml_h = re.sub(rf'\n{comment_line}{blank_line}+{comment_line}', '\n', toml_h)
            toml_h = re.sub(rf'([{{,])\s*\n(?:{comment_line}|{blank_line})+', r'\1\n', toml_h)
            toml_h = re.sub(rf'{comment_line}+', '\n', toml_h)
            # consecutive header separators
            header_separator = r'(?://\*\*\*\**[ \t]+[a-zA-Z0-9_/.-]+[ \t]+\*\*\*\*+\n)'
            toml_h = re.sub(rf'(?:{header_separator}{blank_line}*)+({header_separator})', r'\1', toml_h)
            # weird spacing edge case between } and pp directives
            toml_h = re.sub('\n[}]\n#', r'\n}\n\n#', toml_h)
            # enable warnings -> disable warnings
            toml_h = re.sub('(TOML_ENABLE_WARNINGS;)\n[ \t\n]*\n(TOML_DISABLE_WARNINGS;)', r'', toml_h)
            # blank lines between consecutive TOML_XXXXX_WARNINGS statements
            toml_h = re.sub('(TOML_[A-Z_]+?_WARNINGS;)\n[ \t\n]*\n(TOML_[A-Z_]+?_WARNINGS;)', r'\1\n\2', toml_h)
            # blank lines between consecutive #includes
            toml_h = re.sub(
                r'[#]\s*include\s*<(.+?)>\n[ \t\n]*\n[#]\s*include\s*<(.+?)>', r'#include <\1>\n#include <\2>', toml_h
            )
            # blank lines following opening brackets or a comma
            toml_h = re.sub(r'([^@][({,])\n\n', r'\1\n', toml_h)
            # blank lines preceeding closing brackets
            toml_h = re.sub(r'\n\n([ \t]*[})])', r'\n\1', toml_h)
            # IWYU pragmas
            toml_h = re.sub(r'\n// IWYU pragma: [^\n]+\n', '\n', toml_h)
        # ensure only one trailing newline
        toml_h = toml_h.strip() + '\n'

    # change TOML_LIB_SINGLE_HEADER to 1
    toml_h = re.sub(
        r'#\s*define\s+TOML_LIB_SINGLE_HEADER\s+[0-9]+', '#define	TOML_LIB_SINGLE_HEADER 1', toml_h, flags=re.I
    )

    # build the preamble (license etc)
    preamble = []
    preamble.append(rf'''
// toml++ v{version}
// https://github.com/marzer/tomlplusplus
// SPDX-License-Identifier: MIT''')
    preamble.append(r'''
// -         THIS FILE WAS ASSEMBLED FROM MULTIPLE HEADER FILES BY A SCRIPT - PLEASE DON'T EDIT IT DIRECTLY            -
//
// If you wish to submit a contribution to toml++, hooray and thanks! Before you crack on, please be aware that this
// file was assembled from a number of smaller files by a python script, and code contributions should not be made
// against it directly. You should instead make your changes in the relevant source file(s). The file names of the files
// that contributed to this header can be found at the beginnings and ends of the corresponding sections of this file.''')
    preamble.append(r'''
// TOML Language Specifications:
// latest:      https://github.com/toml-lang/toml/blob/master/README.md
// v1.0.0:      https://toml.io/en/v1.0.0
// v0.5.0:      https://toml.io/en/v0.5.0
// changelog:   https://github.com/toml-lang/toml/blob/master/CHANGELOG.md''')
    preamble.append(read_file(ROOT / 'LICENSE'))

    # write the output
    with StringIO(newline='\n') as output:
        # build in a string buffer
        write = lambda txt, end='\n': print(txt, file=output, end=end)
        if len(preamble) > 0:
            write(make_divider())
        for pre in preamble:
            write('//')
            for line in pre.strip().splitlines():
                if len(line) == 0:
                    write('//')
                    continue
                if not line.startswith('//'):
                    write('// ', end='')
                write(line)
            write('//')
            write(make_divider())
        write(toml_h)
        write('')

        return output.getvalue().strip() + '\n'


def with_generated_undefs(text, single_header):
    defined = dict()
    for match in re.finditer(r'(?m)^\s*#\s*(define|undef)\s+([A-Za-z0-9_]+)', UNDEF_BLOCK.sub('', single_header)):
        defined[match.group(2)] = match.group(1) == r'define'
    names = sorted(
        (n for n, d in defined.items() if d and n not in PUBLIC_MACROS), key=lambda n: n.replace('_', ' ').lower()
    )
    undefs = ''.join(rf'#undef {name}' + '\n' for name in names)
    text, count = UNDEF_BLOCK.subn(lambda m: m.group(1) + undefs + m.group(2), text)
    if count != 1:
        sys.exit(r'error: include/toml++/toml.hpp: no "#if TOML_UNDEF_MACROS" block of #undefs found')
    return text


def parse_version():
    text = read_text_file(VERSION_FILE).strip()
    match = re.fullmatch(r'(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)', text)
    if not match:
        sys.exit(rf"error: {VERSION_FILE.name}: '{text}' is not a major.minor.patch version")
    return (match[1], match[2], match[3])


def version_rules(major, minor, patch):
    version = rf'{major}.{minor}.{patch}'
    snippets = [
        (rf'(?i)(toml(?:plusplus|\+\+|pp)\s*[/:^]\s*){TRIPLET}', rf'\g<1>{version}'),
        (rf'(?i)(GIT_TAG\s+)(?:v\s*)?{TRIPLET}', rf'\g<1>v{version}'),
    ]
    return {
        'include/toml++/impl/version.hpp': [
            (r'(#\s*define\s+TOML_LIB_MAJOR)\s+[0-9]+', rf'\g<1> {major}'),
            (r'(#\s*define\s+TOML_LIB_MINOR)\s+[0-9]+', rf'\g<1> {minor}'),
            (r'(#\s*define\s+TOML_LIB_PATCH)\s+[0-9]+', rf'\g<1> {patch}'),
        ],
        'README.md': snippets,
        'docs/pages/main_page.md': snippets,
        'toml++.natvis': [(r'toml::v[0-9]+::', rf'toml::v{major}::')],
    }


def apply_rules(path, rules):
    text = read_text_file(ROOT / path)
    for pattern, replacement in rules:
        text, count = re.subn(pattern, replacement, text)
        if not count:
            sys.exit(rf"error: {path}: nothing matched '{pattern}'")
    return text


def updated_changelog(version):
    text = read_text_file(ROOT / 'CHANGELOG.md')
    if re.search(rf'(?m)^##\s+v{re.escape(version)}(?:\s|$)', text):
        return text
    released = rf'[Released](https://github.com/marzer/tomlplusplus/releases/tag/v{version}) {date.today()}'
    text, count = re.subn(r'(?m)^## Unreleased[ \t]*$', rf'## Unreleased\n\n## v{version}\n\n{released}', text, count=1)
    if not count:
        sys.exit(rf'error: CHANGELOG.md has neither a "## v{version}" heading nor an "## Unreleased" one')
    return text


def main():
    parser = ArgumentParser(description=r'Regenerates everything derived from VERSION.txt and include/toml++.')
    parser.add_argument(r'--check', action='store_true', help=r'report anything out of date, changing nothing')
    parser.add_argument(r'--inputs', action='store_true', help=r'list the files regeneration reads, changing nothing')
    args = parser.parse_args()

    if args.inputs:
        inputs = [VERSION_FILE, Path(__file__).resolve(), ROOT / 'toml.hpp']
        inputs += sorted(path for path in (ROOT / 'include').rglob('*') if path.is_file())
        for path in inputs:
            print(path.relative_to(ROOT).as_posix())
        return 0

    major, minor, patch = parse_version()
    version = rf'{major}.{minor}.{patch}'

    outputs = {ROOT / path: apply_rules(path, rules) for path, rules in version_rules(major, minor, patch).items()}
    outputs[ROOT / 'CHANGELOG.md'] = updated_changelog(version)

    def read(path):
        return outputs[path] if path in outputs else read_text_file(path)

    toml_hpp = ROOT / 'include' / 'toml++' / 'toml.hpp'
    outputs[toml_hpp] = with_generated_undefs(read(toml_hpp), generate_single_header(version, read))
    outputs[ROOT / 'toml.hpp'] = generate_single_header(version, read)

    stale = [path for path, text in outputs.items() if not path.exists() or read_text_file(path) != text]

    if args.check:
        for path in stale:
            print(rf'error: {path.relative_to(ROOT)} is out of date', file=sys.stderr)
        if stale:
            print(r'build the project, or run tools/regenerate.py', file=sys.stderr)
        return 1 if stale else 0

    for path in stale:
        write_text_file(path, outputs[path])
    return 0


if __name__ == '__main__':
    sys.exit(main())
