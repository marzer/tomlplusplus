#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

import sys
import os
import os.path as path
import utils
import re
import traceback
import subprocess
import random
import concurrent.futures as futures
import html
import bs4 as soup
import json


#=== CONFIG ============================================================================================================



repository = 'marzer/tomlplusplus'
inline_namespaces = [
	"toml::literals",
]
inline_namespace_explainer = 'All members of this namespace are automatically members of the parent namespace. '	\
	+ 'It does not require an explicit \'using\' statement.'
type_names = [
	#------ standard types
	'size_t',
	'uint8_t',
	'uint16_t',
	'uint32_t',
	'uint64_t',
	'int8_t',
	'int16_t',
	'int32_t',
	'int64_t',
	'ptrdiff_t',
	'intptr_t',
	'uintptr_t',
	'exception',
	'iterator',
	'const_iterator',
	'void',
	'char',
	'wchar_t',
	'int',
	'long',
	'short',
	'signed',
	'unsigned',
	'float',
	'double',
	'bool',
	'pair',
	'tuple',
	'istream',
	'ostream',
	'ifstream',
	'ofstream',
	'stringstream',
	'istringstream',
	'ostringstream',
	'string_view',
	'string',
	'byte',
	'optional',
	#------ toml++ types
	'node',
	'table',
	'array',
	'value',
	'date',
	'time',
	'date_time',
	'time_offset',
	'parse_result',
	'parse_error',
	'json_formatter',
	'default_formatter',
	'format_flags',
	'inserter',
	'node_type',
]
all_namespaces = [
	'std',
	'toml',
	'literals',
	'impl'
]
string_literals = [
	's',
	'sv',
	'_toml'
]
external_links = [
	(r'static_cast','https://en.cppreference.com/w/cpp/language/static_cast'),
	(r'const_cast','https://en.cppreference.com/w/cpp/language/const_cast'),
	(r'dynamic_cast','https://en.cppreference.com/w/cpp/language/dynamic_cast'),
	(r'reinterpret_cast','https://en.cppreference.com/w/cpp/language/reinterpret_cast'),
	('std::assume_aligned(?:\(\))?', 'https://en.cppreference.com/w/cpp/memory/assume_aligned'),
	(r'(?:std::)?nullptr_t', 'https://en.cppreference.com/w/cpp/types/nullptr_t'),
	(r'(?:std::)?size_t', 'https://en.cppreference.com/w/cpp/types/size_t'),
	(r'(?:std::)?u?int(_fast|_least)?(?:8|16|32|64)_ts?', 'https://en.cppreference.com/w/cpp/types/integer'),
	(r'(?:wchar|char(?:8|16|32))_ts?', 'https://en.cppreference.com/w/cpp/language/types#Character_types'),
	(r'\s(?:<|&lt;)fstream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/fstream'),
	(r'\s(?:<|&lt;)iosfwd(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/iosfwd'),
	(r'\s(?:<|&lt;)iostream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/iostream'),
	(r'\s(?:<|&lt;)sstream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/sstream'),
	(r'\s(?:<|&lt;)string(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/string'),
	(r'\s(?:<|&lt;)string_view(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/string_view'),
	(r'std::(?:basic_|w)?fstreams?', 'https://en.cppreference.com/w/cpp/io/basic_fstream'),
	(r'std::(?:basic_|w)?ifstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ifstream'),
	(r'std::(?:basic_|w)?iostreams?', 'https://en.cppreference.com/w/cpp/io/basic_iostream'),
	(r'std::(?:basic_|w)?istreams?', 'https://en.cppreference.com/w/cpp/io/basic_istream'),
	(r'std::(?:basic_|w)?istringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_istringstream'),
	(r'std::(?:basic_|w)?ofstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ofstream'),
	(r'std::(?:basic_|w)?ostreams?', 'https://en.cppreference.com/w/cpp/io/basic_ostream'),
	(r'std::(?:basic_|w)?ostringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ostringstream'),
	(r'std::(?:basic_|w)?stringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_stringstream'),
	(r'std::(?:basic_|w|u(?:8|16|32))?string_views?', 'https://en.cppreference.com/w/cpp/string/basic_string_view'),
	(r'std::(?:basic_|w|u(?:8|16|32))?strings?', 'https://en.cppreference.com/w/cpp/string/basic_string'),
	(r'std::add_[lr]value_reference(?:_t)?', 'https://en.cppreference.com/w/cpp/types/add_reference'),
	(r'std::allocators?', 'https://en.cppreference.com/w/cpp/memory/allocator'),
	(r'std::arrays?', 'https://en.cppreference.com/w/cpp/container/array'),
	(r'std::as_(writable_)?bytes(?:\(\))?', 'https://en.cppreference.com/w/cpp/container/span/as_bytes'),
	(r'std::bit_cast(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/bit_cast'),
	(r'std::bit_ceil(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/bit_ceil'),
	(r'std::bit_floor(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/bit_floor'),
	(r'std::bit_width(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/bit_width'),
	(r'std::bytes?', 'https://en.cppreference.com/w/cpp/types/byte'),
	(r'std::char_traits', 'https://en.cppreference.com/w/cpp/string/char_traits'),
	(r'std::chrono::durations?', 'https://en.cppreference.com/w/cpp/chrono/duration'),
	(r'std::clamp(?:\(\))?', 'https://en.cppreference.com/w/cpp/algorithm/clamp'),
	(r'std::conditional(?:_t)?', 'https://en.cppreference.com/w/cpp/types/conditional'),
	(r'std::countl_one(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/countl_one'),
	(r'std::countl_zero(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/countl_zero'),
	(r'std::countr_one(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/countr_one'),
	(r'std::countr_zero(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/countr_zero'),
	(r'std::enable_if(?:_t)?', 'https://en.cppreference.com/w/cpp/types/enable_if'),
	(r'std::exceptions?', 'https://en.cppreference.com/w/cpp/error/exception'),
	(r'std::has_single_bit(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/has_single_bit'),
	(r'std::initializer_lists?', 'https://en.cppreference.com/w/cpp/utility/initializer_list'),
	(r'std::integral_constants?', 'https://en.cppreference.com/w/cpp/types/integral_constant'),
	(r'std::is_(?:nothrow_)?convertible(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_convertible'),
	(r'std::is_(?:nothrow_)?invocable(?:_r)?', 'https://en.cppreference.com/w/cpp/types/is_invocable'),
	(r'std::is_base_of(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_base_of'),
	(r'std::is_constant_evaluated(?:\(\))?', 'https://en.cppreference.com/w/cpp/types/is_constant_evaluated'),
	(r'std::is_enum(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_enum'),
	(r'std::is_floating_point(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_floating_point'),
	(r'std::is_integral(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_integral'),
	(r'std::is_pointer(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_pointer'),
	(r'std::is_reference(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_reference'),
	(r'std::is_same(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_same'),
	(r'std::is_signed(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_signed'),
	(r'std::is_unsigned(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_unsigned'),
	(r'std::is_void(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_void'),
	(r'std::launder(?:\(\))?', 'https://en.cppreference.com/w/cpp/utility/launder'),
	(r'std::lerp(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/lerp'),
	(r'std::maps?', 'https://en.cppreference.com/w/cpp/container/map'),
	(r'std::max(?:\(\))?', 'https://en.cppreference.com/w/cpp/algorithm/max'),
	(r'std::min(?:\(\))?', 'https://en.cppreference.com/w/cpp/algorithm/min'),
	(r'std::numeric_limits', 'https://en.cppreference.com/w/cpp/types/numeric_limits'),
	(r'std::optionals?', 'https://en.cppreference.com/w/cpp/utility/optional'),
	(r'std::pairs?', 'https://en.cppreference.com/w/cpp/utility/pair'),
	(r'std::popcount(?:\(\))?', 'https://en.cppreference.com/w/cpp/numeric/popcount'),
	(r'std::remove_cv(?:_t)?', 'https://en.cppreference.com/w/cpp/types/remove_cv'),
	(r'std::remove_reference(?:_t)?', 'https://en.cppreference.com/w/cpp/types/remove_reference'),
	(r'std::reverse_iterator', 'https://en.cppreference.com/w/cpp/iterator/reverse_iterator'),
	(r'std::runtime_errors?', 'https://en.cppreference.com/w/cpp/error/runtime_error'),
	(r'std::sets?', 'https://en.cppreference.com/w/cpp/container/set'),
	(r'std::shared_ptrs?', 'https://en.cppreference.com/w/cpp/memory/shared_ptr'),
	(r'std::spans?', 'https://en.cppreference.com/w/cpp/container/span'),
	(r'std::to_address(?:\(\))?', 'https://en.cppreference.com/w/cpp/memory/to_address'),
	(r'std::tuples?', 'https://en.cppreference.com/w/cpp/utility/tuple'),
	(r'std::type_identity(?:_t)?', 'https://en.cppreference.com/w/cpp/types/type_identity'),
	(r'std::underlying_type(?:_t)?', 'https://en.cppreference.com/w/cpp/types/underlying_type'),
	(r'std::unique_ptrs?', 'https://en.cppreference.com/w/cpp/memory/unique_ptr'),
	(r'std::unordered_maps?', 'https://en.cppreference.com/w/cpp/container/unordered_map'),
	(r'std::unordered_sets?', 'https://en.cppreference.com/w/cpp/container/unordered_set'),
	(r'std::vectors?', 'https://en.cppreference.com/w/cpp/container/vector'),
	(
		r'std::atomic(?:_(?:'
			+ r'bool|[su]?char(?:8_t|16_t|32_t)?|u?short'
			+ r'|u?int(?:8_t|16_t|32_t|64_t)?|u?l?long'
			+ r'))?',
		'https://en.cppreference.com/w/cpp/atomic/atomic'
	),
	(
		r'(?:L?P)?(?:'
			+ r'D?WORD(?:32|64|_PTR)?|HANDLE|HMODULE|BOOL(?:EAN)?'
			+ r'|U?SHORT|U?LONG|U?INT(?:8|16|32|64)?'
			+ r'|BYTE|VOID|C[WT]?STR'
			+ r')',
		'https://docs.microsoft.com/en-us/windows/desktop/winprog/windows-data-types'
	),
	(
		r'(?:__INTELLISENSE__|_MSC_FULL_VER|_MSC_VER|_MSVC_LANG|_WIN32|_WIN64)',
		'https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019'
	),
	(r'IUnknowns?', 'https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nn-unknwn-iunknown'),
	(r'(?:IUnknown::)?QueryInterface?', 'https://docs.microsoft.com/en-us/windows/win32/api/unknwn/nf-unknwn-iunknown-queryinterface(q)'),
	(r'(?:Legacy)?InputIterators?', 'https://en.cppreference.com/w/cpp/named_req/InputIterator'),
	(r'(?:Legacy)?OutputIterators?', 'https://en.cppreference.com/w/cpp/named_req/OutputIterator'),
	(r'(?:Legacy)?ForwardIterators?', 'https://en.cppreference.com/w/cpp/named_req/ForwardIterator'),
	(r'(?:Legacy)?BidirectionalIterators?', 'https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator'),
	(r'(?:Legacy)?RandomAccessIterators?', 'https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator'),
	(r'(?:Legacy)?ContiguousIterators?', 'https://en.cppreference.com/w/cpp/named_req/ContiguousIterator'),
	(
		r'(?:'
			+ r'__cplusplus|__STDC_HOSTED__'
			+ r'|__FILE__|__LINE__'
			+ r'|__DATE__|__TIME__'
			+ r'|__STDCPP_DEFAULT_NEW_ALIGNMENT__'
			+ r')',
		'https://en.cppreference.com/w/cpp/preprocessor/replace'
	),
	# toml-specific
	(r'toml::values?', 'classtoml_1_1value.html'),
	(r'(toml::)?date_times?', 'structtoml_1_1date__time.html'),
	(r'(toml::)?time', 'structtoml_1_1time.html'),
	(r'(toml::)?dates?', 'structtoml_1_1date.html')
]
header_overrides = [

]
badges = [
	(
		'Releases',
		'https://img.shields.io/github/v/release/marzer/tomlplusplus?style=flat-square',
		'https://github.com/marzer/tomlplusplus/releases'
	),
	(
		'C++17',
		'badge-C++17.svg',
		'https://en.cppreference.com/w/cpp/compiler_support'
	),
	(
		'C++20',
		'badge-C++20.svg',
		'https://en.cppreference.com/w/cpp/compiler_support'
	),
	(
		'TOML v1.0.0-rc.2',
		'badge-TOML.svg',
		'https://toml.io/en/v1.0.0-rc.2'
	),
	(None, None, None), # <br>
	(
		'MIT License',
		'badge-license-MIT.svg',
		'https://github.com/marzer/tomlplusplus/blob/master/LICENSE'
	),
	(
		'CircleCI',
		'https://img.shields.io/circleci/build/github/marzer/tomlplusplus'
			+ '?label=circle%20ci&logo=circleci&logoColor=white&style=flat-square',
		'https://circleci.com/gh/marzer/tomlplusplus'
	),
	(
		'Mentioned in Awesome C++',
		'badge-awesome.svg',
		'https://github.com/fffaraz/awesome-cpp'
	)
]


#=== HTML DOCUMENT =====================================================================================================



class HTMLDocument(object):

	def __init__(self, path):
		self.__path = path
		with open(path, 'r', encoding='utf-8') as f:
			self.__doc = soup.BeautifulSoup(f, 'html5lib', from_encoding='utf-8')
		self.head = self.__doc.head
		self.body = self.__doc.body
		self.table_of_contents = None
		toc_candidates = self.body.main.article.div.div.div('div', class_='m-block m-default', recursive=False)
		for div in toc_candidates:
			if div.h3 and div.h3.string == 'Contents':
				self.table_of_contents = div
				break

	def flush(self):
		with open(self.__path, 'w', encoding='utf-8', newline='\n') as f:
			f.write(str(self.__doc))

	def new_tag(self, name, parent=None, string=None, class_=None, index=None, before=None, after=None, **kwargs):
		tag = self.__doc.new_tag(name, **kwargs)
		if (string is not None):
			if (tag.string is not None):
				tag.string.replace_with(string)
			else:
				tag.string = soup.NavigableString(string)
		if (class_ is not None):
			tag['class'] = class_
		if (before is not None):
			before.insert_before(tag)
		elif (after is not None):
			after.insert_after(tag)
		elif (parent is not None):
			if (index is None or index < 0):
				parent.append(tag)
			else:
				parent.insert(index, tag)

		return tag

	def find_all_from_sections(self, name=None, select=None, section=None, include_toc=False, **kwargs):
		tags = []
		sectionArgs = { }
		if (section is not None):
			sectionArgs['id'] = section
		sections = self.body.main.article.div.div.div('section', recursive=False, **sectionArgs)
		if include_toc and self.table_of_contents is not None:
			sections = [self.table_of_contents, *sections]
		for sect in sections:
			matches = sect(name, **kwargs) if name is not None else [ sect ]
			if (select is not None):
				newMatches = []
				for match in matches:
					newMatches += match.select(select)
				matches = newMatches
			tags += matches
		return tags



def html_find_parent(tag, names, cutoff=None):
	if not utils.is_collection(names):
		names = [ names ]
	parent = tag.parent
	while (parent is not None):
		if (cutoff is not None and parent is cutoff):
			return None
		if parent.name in names:
			return parent;
		parent = parent.parent
	return parent



def html_replace_tag(tag,str):
	doc = soup.BeautifulSoup(str, 'html5lib')
	newTags = None
	if (len(doc.body.contents) > 0):
		newTags = [f for f in doc.body.contents]
		newTags = [f.extract() for f in newTags]
		prev = tag
		for newTag in newTags:
			prev.insert_after(newTag)
			prev = newTag
	else:
		newTags = []

	if (isinstance(tag, soup.NavigableString)):
		tag.extract()
	else:
		tag.decompose()
	return newTags



def html_shallow_search(starting_tag, names, filter = None):
	if isinstance(starting_tag, soup.NavigableString):
		return []

	if not utils.is_collection(names):
		names = [ names ]

	if starting_tag.name in names:
		if filter is None or filter(starting_tag):
			return [ starting_tag ]

	results = []
	for tag in starting_tag.children:
		if isinstance(tag, soup.NavigableString):
			continue
		if tag.name in names:
			if filter is None or filter(tag):
				results.append(tag)
		else:
			results = results + html_shallow_search(tag, names, filter)
	return results



def html_string_descendants(starting_tag, filter = None):
	if isinstance(starting_tag, soup.NavigableString):
		if filter is None or filter(starting_tag):
			return [ starting_tag ]

	results = []
	for tag in starting_tag.children:
		if isinstance(tag, soup.NavigableString):
			if filter is None or filter(tag):
				results.append(tag)
		else:
			results = results + html_string_descendants(tag, filter)
	return results



def html_append_class(tag, classes):
	appended = False
	if 'class' not in tag.attrs:
		tag['class'] = []
	if not utils.is_collection(classes):
		classes = (classes,)
	for class_ in classes:
		if class_ not in tag['class']:
			tag['class'].append(class_)
			appended = True
	return appended



class RegexReplacer(object):

	def __substitute(self, m):
		self.__result = True
		self.__groups = [str(m.group(0))]
		self.__groups += [str(g) for g in m.groups()]
		return self.__handler(m)

	def __init__(self, expression, handler, value):
		self.__handler = handler
		self.__result = False
		self.__value = expression.sub(lambda m: self.__substitute(m), value)
		if (not self.__result):
			self.__groups = []

	def __str__(self):
		return self.__value

	def __bool__(self):
		return self.__result

	def __getitem__(self, key):
		return self.__groups[key]

	def __len__(self):
		return len(self.__groups)



#=======================================================================================================================



# allows the injection of custom tags using square-bracketed proxies.
class CustomTagsFix(object):
	__double_tags = re.compile(r"\[\s*(span|div|code|pre|h1|h2|h3|h4|h5|h6)(.*?)\s*\](.*?)\[\s*/\s*\1\s*\]", re.I)
	__single_tags = re.compile(r"\[\s*(/?(?:span|div|code|pre|emoji|br|li|ul|ol))(\s.*?)?\s*\]", re.I)
	__allowed_parents = ['dd', 'p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'li']
	__emojis = None
	__emoji_uri = re.compile(r".+unicode/([0-9a-fA-F]+)[.]png.*", re.I)

	@classmethod
	def __double_tags_substitute(cls, m):
		return '<{}{}>{}</{}>'.format(
			m.group(1),
			html.unescape(m.group(2)),
			m.group(3),
			m.group(1)
		)

	@classmethod
	def __single_tags_substitute(cls, m):
		if (str(m.group(1)).lower() == 'emoji'):
			emoji = str(m.group(2)).strip().lower()
			if emoji == '':
				return ''
			if cls.__emojis is None:
				cls.__emojis = json.loads(utils.read_all_text_from_file('emojis.json', 'https://api.github.com/emojis'))
				if '__processed' not in cls.__emojis:
					emojis = {}
					for key, uri in cls.__emojis.items():
						m2 = cls.__emoji_uri.fullmatch(uri)
						if m2:
							emojis[key] = [ str(m2.group(1)).upper(), uri ]
					aliases = [('sundae', 'ice_cream')]
					for alias, key in aliases:
						emojis[alias] = emojis[key]
					emojis['__processed'] = True
					with open('emojis.json', 'w', encoding='utf-8', newline='\n') as f:
						f.write(json.dumps(emojis, sort_keys=True, indent=4))
					cls.__emojis = emojis
			if emoji not in cls.__emojis:
				return ''
			return '&#x{}'.format(cls.__emojis[emoji][0])

		else:
			return '<{}{}>'.format(
				m.group(1),
				(' ' + str(m.group(2)).strip()) if m.group(2) else ''
			)

	def __call__(self, file, doc):
		changed = False
		for name in self.__allowed_parents:
			tags = doc.find_all_from_sections(name, include_toc=True)
			for tag in tags:
				if (len(tag.contents) == 0 or html_find_parent(tag, 'a', doc.body) is not None):
					continue

				replacer = RegexReplacer(self.__double_tags, self.__double_tags_substitute, str(tag))
				if (replacer):
					changed = True
					html_replace_tag(tag, str(replacer))
					continue

				replacer = RegexReplacer(self.__single_tags, self.__single_tags_substitute, str(tag))
				if (replacer):
					changed = True
					html_replace_tag(tag, str(replacer))
					continue

		return changed



#=======================================================================================================================



# adds custom links to the navbar.
class NavBarFix(object):

	def __call__(self, file, doc):
		global repository
		__links = [
			('Report an issue', f'https://github.com/{repository}/issues'),
			('Github', f'https://github.com/{repository}/')
		]
		list = doc.body.header.nav.div.div.select_one('#m-navbar-collapse').div.ol
		if (list.select_one('.muu-injected') is None):
			for label, url in self.__links:
				doc.new_tag('a',
					parent=doc.new_tag('li', parent=list, class_='muu-injected muu-external-navbar', index=0),
					string=label,
					href=url,
					target='_blank'
				)
			return True
		return False



#=======================================================================================================================



# base type for modifier parsing fixers.
class ModifiersFixBase(object):
	_modifierRegex = "defaulted|noexcept|constexpr|(?:pure )?virtual|protected|__(?:(?:vector|std|fast)call|cdecl)"
	_modifierClasses = {
		"defaulted" : "m-info",
		"noexcept" : "m-success",
		"constexpr" : "m-primary",
		"pure virtual" : "m-warning",
		"virtual" : "m-warning",
		"protected" : "m-warning",
		"__vectorcall" : "m-info",
		"__stdcall" : "m-info",
		"__fastcall" : "m-info",
		"__cdecl" : "m-info"
	}



#=======================================================================================================================



# fixes improperly-parsed modifiers on function signatures in the various 'detail view' sections.
class ModifiersFix1(ModifiersFixBase):

	__expression = re.compile(r'(\s+)({})(\s+)'.format(ModifiersFixBase._modifierRegex))
	__sections = ['pub-static-methods', 'pub-methods', 'friends', 'func-members']

	@classmethod
	def __substitute(cls, m):
		return '{}<span class="muu-injected m-label m-flat {}">{}</span>{}'.format(
			m.group(1),
			cls._modifierClasses[m.group(2)],
			m.group(2),
			m.group(3)
		)

	def __call__(self, file, doc):
		changed = False
		for sect in self.__sections:
			tags = doc.find_all_from_sections('dt', select='span.m-doc-wrap', section=sect)
			for tag in tags:
				replacer = RegexReplacer(self.__expression, self.__substitute, str(tag))
				if (replacer):
					changed = True
					html_replace_tag(tag, str(replacer))
		return changed



#=======================================================================================================================



# fixes improperly-parsed modifiers on function signatures in the 'Function documentation' section.
class ModifiersFix2(ModifiersFixBase):

	__expression = re.compile(r'\s+({})\s+'.format(ModifiersFixBase._modifierRegex))

	@classmethod
	def __substitute(cls, m, matches):
		matches.append(m.group(1))
		return ' '

	def __call__(self, file, doc):
		changed = False
		sections = doc.find_all_from_sections(section=False) # all sections without an id
		section = None
		for s in sections:
			if (str(s.h2.string) == 'Function documentation'):
				section = s
				break
		if (section is not None):
			funcs = section(id=True)
			funcs = [f.find('h3') for f in funcs]
			for f in funcs:
				bumper = f.select_one('span.m-doc-wrap-bumper')
				end = f.select_one('span.m-doc-wrap').contents
				end = end[len(end)-1]
				matches = []
				bumperContent = self.__expression.sub(lambda m: self.__substitute(m, matches), str(bumper))
				if (matches):
					changed = True
					bumper = html_replace_tag(bumper, bumperContent)
					lastInserted = end.find('span')
					for match in matches:
						lastInserted = doc.new_tag('span',
							parent=end,
							string=match,
							class_='muu-injected m-label {}'.format(self._modifierClasses[match]),
							before=lastInserted
						)
						lastInserted.insert_after(' ')
		return changed



#=======================================================================================================================



# applies some basic fixes to index.html
class IndexPageFix(object):

	def __call__(self, file, doc):
		global badges
		if file != 'index.html':
			return False
		parent = doc.body.main.article.div.div.div
		banner = parent('img')[0].extract()
		parent('h1')[0].replace_with(banner)
		parent = doc.new_tag('div', class_='gh-badges', after=banner)
		for (alt, src, href) in badges:
			if alt is None and src is None and href is None:
				doc.new_tag('br', parent=parent)
			else:
				anchor = doc.new_tag('a', parent=parent, href=href, target='_blank')
				doc.new_tag('img', parent=anchor, src=src, alt=alt)
		return True



#=======================================================================================================================



# base type for applying inline namespace annotations.
class InlineNamespaceFixBase(object):
	_namespaceFiles = ['namespace{}.html'.format(ns.lower().replace('::','_1_1')) for ns in inline_namespaces]



#=======================================================================================================================



# adds inline namespace annotations in class and namespace trees.
class InlineNamespaceFix1(InlineNamespaceFixBase):
	__allowedFiles = ['annotated.html', 'namespaces.html']

	def __call__(self, file, doc):
		global inline_namespace_explainer
		changed = False
		if (file in self.__allowedFiles):
			anchors = []
			for f in self._namespaceFiles:
				anchors += doc.body.find_all("a", href=f)
			for anchor in anchors:
				next = anchor.next_sibling
				while (next is not None and isinstance(next, soup.NavigableString)):
					next = next.next_sibling
				if (next is not None and next.get('class') is not None and 'muu-injected' in next.get('class')):
					continue
				doc.new_tag('span',
					after=anchor,
					string='inline',
					title=inline_namespace_explainer,
					class_='m-label m-info m-flat muu-injected muu-inline-namespace'
				)
				anchor.insert_after(' ')
				changed = True
		return changed



#=======================================================================================================================



# adds inline namespace annotations to the h1 element of inline namespace pages.
class InlineNamespaceFix2(InlineNamespaceFixBase):

	def __call__(self, file, doc):
		global inline_namespace_explainer
		changed = False
		if (file in self._namespaceFiles):
			h1 = doc.body.find('h1')
			tag = h1.select_one('span.muu-injected')
			if (tag is None):
				tag = doc.new_tag('span',
					parent=h1,
					string='inline',
					title=inline_namespace_explainer,
					class_='m-label m-info muu-injected muu-inline-namespace'
				)
				tag.insert_before(' ')
				changed = True
		return changed



#=======================================================================================================================



# adds inline namespace annotations to sections with id=namespaces.
class InlineNamespaceFix3(InlineNamespaceFixBase):

	def __call__(self, file, doc):
		global inline_namespace_explainer
		anchors = doc.find_all_from_sections('a', section='namespaces')
		changed = False
		for anchor in anchors:
			if (anchor.get('href') not in self._namespaceFiles):
				continue
			next = anchor.next_sibling
			while (next is not None and isinstance(next, soup.NavigableString)):
				next = next.next_sibling
			if (next is not None and next.get('class') is not None and 'muu-injected' in next.get('class')):
				continue
			doc.new_tag('span',
				after=anchor,
				string='inline',
				title=inline_namespace_explainer,
				class_='m-label m-info m-flat muu-injected muu-inline-namespace'
			)
			anchor.insert_after(' ')
			changed = True
		return changed



#=======================================================================================================================



# adds some additional colouring to the syntax highlighting in code blocks.
class SyntaxHighlightingFix(object):

	__keywords = [
		'constexpr',
		'if',
		'else',
		'true',
		'false',
		'const',
		'noexcept',
		'template',
		'typename'
	]

	def __call__(self, file, doc):
		global type_names
		global all_namespaces
		global string_literals

		code_blocks = doc.body('pre', class_='m-code')
		changed = False
		for code_block in code_blocks:

			# namespaces
			names = code_block('span', class_='n')
			names_ = []
			for name in names:
				if (name.string is None or name.string not in all_namespaces):
					names_.append(name)
					continue
				next = name.next_sibling
				if (next is None or next.string is None or next.string != '::'):
					names_.append(name)
					continue

				next.extract()
				name.string.replace_with(name.string + '::')
				name['class'] = 'ns'
				changed = True

			# string literals
			names = names_
			names_ = []
			for name in names:
				if (name.string is None or name.string not in string_literals):
					names_.append(name)
					continue
				prev = name.previous_sibling
				if (prev is None or prev['class'] is None or 's' not in prev['class']):
					names_.append(name)
					continue
				name['class'] = 'sa'
				changed = True

			# types and typedefs
			names = names_ + code_block('span', class_='kt')
			for name in names:
				if (name.string is not None and name.string in type_names):
					name['class'] = 'ut'
					changed = True

			# misidentifed keywords
			for keywordClass in ['nf', 'nb']:
				kws = code_block('span', class_=keywordClass)
				for kw in kws:
					if (kw.string is not None and kw.string in self.__keywords):
						kw['class'] = 'k'
						changed = True

		return changed



#=======================================================================================================================



# adds links to external sources where appropriate
class ExtDocLinksFix(object):

	__allowedNames = ['dd', 'p', 'dt', 'h3', 'td', 'div']

	def __init__(self):
		global external_links
		self.__expressions = []
		for type, uri in external_links:
			self.__expressions.append((re.compile('(?<![a-zA-Z_])'+type+'(?![a-zA-Z_])'), uri))

	@classmethod
	def __substitute(cls, m, uri):
		external = uri.startswith('http')
		return r'<a href="{}" class="m-doc muu-injected{}"{}>{}</a>'.format(
			uri,
			' muu-external' if external else '',
			' target="_blank"' if external else '',
			m.group(0),
		)

	def __call__(self, file, doc):
		changed = False
		root = doc.body.main.article.div.div
		tags = html_shallow_search(root, self.__allowedNames, lambda t: html_find_parent(t, 'a', root) is None)
		strings = []
		for tag in tags:
			strings = strings + html_string_descendants(tag, lambda t: html_find_parent(t, 'a', tag) is None)
		for expr, uri in self.__expressions:
			for string in strings:
				if string.parent is None:
					continue
				replacer = RegexReplacer(expr, lambda m: self.__substitute(m, uri), html.escape(str(string), quote=False))
				if replacer:
					repl_str = str(replacer)
					begins_with_ws = len(repl_str) > 0 and repl_str[:1].isspace()
					new_tag = html_replace_tag(string, repl_str)[0]
					if (begins_with_ws and new_tag.string is not None and not new_tag.string[:1].isspace()):
						new_tag.insert_before(' ')
					changed = True
		return changed



#=======================================================================================================================



# collapses std::enable_if in template headers to reduce verbosity.
class EnableIfFix(object):

	__expression = re.compile(
		# group 1: everything left of and including "std::enable_if<" (or it's aliases)
		r'^(.+?(?:template&lt;.+&gt;\s*)?template\s*&lt;.+?(?:typename|class)\s*(?:=\s*)?(?:<a.+?>\s*)?(?:std::enable_if(?:_t)?)\s*(?:</a>\s*)?&lt;)\s*'
		# group 2: the SFINAE parameters we actually want from inside the std::enable_if
		+ r'(.+?)'
		# group 3: the rest of the template declaration on the right
		+ r'\s*(&gt;\s*&gt;.+?)$',
		re.S
	)

	__spacingFix1 = re.compile(r'(_v|>::value)(&&|\|\|)')

	@classmethod
	def __substitute(cls, m):
		return r'{}<span class="muu-injected muu-enable-if"><a href="#" onclick="ToggleEnableIf(this);return false;">...</a><span>{}</span></span>{}'.format(
			m.group(1),
			m.group(2),
			m.group(3)
		)

	def __call__(self, file, doc):
		changed = False
		for template in doc.body('div', class_='m-doc-template'):
			replacer = RegexReplacer(self.__expression, lambda m: self.__substitute(m), str(template))
			if replacer:
				injected = html_replace_tag(template, str(replacer))[0].select_one(".muu-enable-if")
				anchor = injected.a
				content = injected.span
				tweaks = []
				for tag in content.descendants:
					if (isinstance(tag, soup.NavigableString)):
						val = str(tag)
						replacer = RegexReplacer(self.__spacingFix1, lambda m: '{} {}'.format(m[1], m[2]), val)
						if replacer:
							tweaks.append((tag,str(replacer)))
				for tag, sub in tweaks:
					tag.replace_with(sub)
				anchor['title'] = content.get_text().strip().replace('"', '&quot;')
				changed = True
		return changed



#=======================================================================================================================



# makes sure links to certain external sources are correctly marked as such.
class ExternalLinksFix(object):

	__href = re.compile(r'^\s*(?:https?|s?ftp|mailto)[:]', re.I)
	__godbolt = re.compile(r'^\s*https[:]//godbolt.org/z/.+?$', re.I)

	def __call__(self, file, doc):
		changed = False
		for anchor in doc.body('a', recursive=True):
			if self.__href.search(anchor['href']):
				if 'target' not in anchor.attrs or anchor['target'] != '_blank':
					anchor['target'] = '_blank'
					changed = True
				changed = html_append_class(anchor, 'muu-external') or changed

				# do magic with godbolt.org links
				if self.__godbolt.fullmatch(anchor['href']):
					changed = html_append_class(anchor, 'godbolt') or changed
					if anchor.parent.name == 'p' and len(anchor.parent.contents) == 1:
						changed = html_append_class(anchor.parent, ('m-note', 'm-success', 'godbolt')) or changed
						if anchor.parent.next_sibling is not None and anchor.parent.next_sibling.name == 'pre':
							code_block = anchor.parent.next_sibling
							code_block.insert(0, anchor.parent.extract())



		return changed



#=======================================================================================================================



# overrides <path/to/header.h> links
class HeaderOverridesFix(object):

	def __init__(self):
		global header_overrides
		self.__expressions = []
		for header, repl, html_file in header_overrides:
			self.__expressions.append((
				re.compile('(?:<|")'+header+'(?:>|")'),
				'<' + repl + '>',
				html_file
			))

	@classmethod
	def __substitute(cls, m, repl):
		return repl

	def __call__(self, file, doc):
		changed = False
		root = doc.body.main.article.div.div
		tags = root.find_all('a', string=True)
		for tag in tags:
			skip = False
			for expr, repl, html_file in self.__expressions:
				replacer = RegexReplacer(expr, lambda m: self.__substitute(m, repl), tag.string)
				if replacer:
					tag.string = str(replacer)
					tag["href"] = html_file
					changed = True
					skip = True
				if skip:
					continue
		return changed



#=======================================================================================================================



_threadError = False



def postprocess_file(dir, file, fixes):

	global _threadError
	if (_threadError):
		return False
	print("Post-processing {}".format(file))
	changed = False

	try:
		doc = HTMLDocument(path.join(dir, file))
		file = file.lower()
		for fix in fixes:
			if (fix(file, doc)):
				changed = True
		if (changed):
			doc.flush()

	except Exception as err:
		print(
			'Error: [{}] {}'.format(
				type(err).__name__,
				str(err)
			),
			file=sys.stderr
		)
		traceback.print_exc(file=sys.stderr)
		_threadError = True

	return changed



def preprocess_xml(xml_dir):
	pass



def main():
	global _threadError

	num_threads = os.cpu_count() * 2
	root_dir = path.join(utils.get_script_folder(), '..')
	docs_dir = path.join(root_dir, 'docs')
	xml_dir = path.join(docs_dir, 'xml')
	html_dir = path.join(docs_dir, 'html')
	mcss_dir = path.join(root_dir, 'extern', 'mcss')
	doxygen = path.join(mcss_dir, 'documentation', 'doxygen.py')

	# delete any previously generated html and xml
	utils.delete_directory(xml_dir)
	utils.delete_directory(html_dir)

	# run doxygen
	subprocess.check_call( ['doxygen', 'Doxyfile'], shell=True, cwd=docs_dir )

	# fix some shit that's broken in the xml
	preprocess_xml(xml_dir)

	# run doxygen.py (m.css)
	utils.run_python_script(doxygen, path.join(docs_dir, 'Doxyfile-mcss'), '--no-doxygen')

	# delete xml
	utils.delete_directory(xml_dir)

	# post-process html files
	fixes = [
		CustomTagsFix()
		, SyntaxHighlightingFix()
		#, NavBarFix()
		, IndexPageFix()
		, ModifiersFix1()
		, ModifiersFix2()
		, InlineNamespaceFix1()
		, InlineNamespaceFix2()
		, InlineNamespaceFix3()
		, ExtDocLinksFix()
		, EnableIfFix()
		, ExternalLinksFix()
		, HeaderOverridesFix()
	]
	files = [path.split(f) for f in utils.get_all_files(html_dir, any=('*.html', '*.htm'))]
	if files:
		with futures.ThreadPoolExecutor(max_workers=min(len(files), num_threads)) as executor:
			jobs = { executor.submit(postprocess_file, dir, file, fixes) : file for dir, file in files }
			for job in futures.as_completed(jobs):
				if _threadError:
					executor.shutdown(False)
					break
				else:
					file = jobs[job]
					print('Finished processing {}.'.format(file))
		if _threadError:
			return 1


if __name__ == '__main__':
	utils.run(main)
