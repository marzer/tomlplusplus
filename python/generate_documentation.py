#!/usr/bin/env python3
# This file is a part of toml++ and is subject to the the terms of the MIT license.
# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.

import sys
import re
import os
import os.path as path
import traceback
import datetime
import subprocess
import random
import concurrent.futures
import shutil
import html
import fnmatch
import bs4 as soup



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
	#------ toml++ types
	'table',
	'array',
	'value',
	'date',
	'time',
	'date_time',
	'time_offset',
	'string_char',
	'parse_result',
	'parse_error',
	'json_formatter',
	'default_formatter',
	'format_flags'
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

def is_tool(name):
	return shutil.which(name) is not None



def is_collection(val):
	if isinstance(val, (list, tuple, dict, set)):
		return True
	return False



def read_all_text_from_file(path):
	print("Reading {}".format(path))
	with open(path, 'r', encoding='utf-8') as file:
		text = file.read()
	return text



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



class HTMLDocument(object):

	def __init__(self, path):
		self.__path = path
		with open(path, 'r', encoding='utf-8') as f:
			self.__doc = soup.BeautifulSoup(f, 'html5lib', from_encoding='utf-8')
		self.head = self.__doc.head
		self.body = self.__doc.body

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
		

	def find_all_from_sections(self, name=None, select=None, section=None, **kwargs):
		tags = []
		sectionArgs = { }
		if (section is not None):
			sectionArgs['id'] = section
		sections = self.body.main.article.div.div.div('section', recursive=False, **sectionArgs)
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
	if not is_collection(names):
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
	
	if not is_collection(names):
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



# allows the injection of <div> and <span> tags using [div] and [span] proxies.
class CustomTagsFix(object): 
	__expression = re.compile(r"\[\s*(span|div)(.*?)\s*\](.*?)\[\s*/\s*\1\s*\]", re.I)
	__allowedNames = ['dd', 'p']
	
	@classmethod
	def __substitute(cls, m):
		return '<{}{}>{}</{}>'.format(
			m.group(1),
			html.unescape(m.group(2)),
			m.group(3),
			m.group(1)
		)
		
	def __call__(self, file, doc):
		changed = False
		for name in self.__allowedNames:
			tags = doc.find_all_from_sections(name)
			for tag in tags:
				if (len(tag.contents) == 0 or html_find_parent(tag, 'a', doc.body) is not None):
					continue
				replacer = RegexReplacer(self.__expression, self.__substitute, str(tag))
				if (replacer):
					changed = True
					html_replace_tag(tag, str(replacer))
		return changed



#=======================================================================================================================



# adds custom links to the navbar.
class NavBarFix(object): 
	
	__links = [
		('Report an issue', 'https://github.com/marzer/tomlplusplus/issues'),
		('Github', 'https://github.com/marzer/tomlplusplus/')
	]

	def __call__(self, file, doc):
		list = doc.body.header.nav.div.div.select_one('#m-navbar-collapse').div.ol
		if (list.select_one('.tpp-injected') is None):
			for label, url in self.__links:
				doc.new_tag('a',
					parent=doc.new_tag('li', parent=list, class_='tpp-injected tpp-external-navbar', index=0),
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
		return '{}<span class="tpp-injected m-label m-flat {}">{}</span>{}'.format(
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
							class_='tpp-injected m-label {}'.format(self._modifierClasses[match]),
							before=lastInserted
						)
						lastInserted.insert_after(' ')
		return changed



#=======================================================================================================================



# applies some basic fixes to index.html
class IndexPageFix(object): 

	__badges = [
		('C++', 'https://img.shields.io/badge/c%2B%2B-17%2C%2020-informational', 'https://en.cppreference.com/w/cpp/compiler_support'),
		('TOML', 'https://img.shields.io/badge/TOML-v0.5.0-informational', 'https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md'),
		('MIT License', 'https://img.shields.io/badge/license-MIT-blue.svg', 'https://github.com/marzer/tomlplusplus/blob/master/LICENSE'),
		('Releases', 'https://img.shields.io/github/release/marzer/tomlplusplus.svg', 'https://github.com/marzer/tomlplusplus/releases'),
		('Mentioned in Awesome C++', 'https://awesome.re/mentioned-badge.svg', 'https://github.com/fffaraz/awesome-cpp'),
		('CircleCI', 'https://circleci.com/gh/marzer/tomlplusplus.svg?style=shield', 'https://circleci.com/gh/marzer/tomlplusplus')
	]

	def __call__(self, file, doc):
		if file != 'index.html':
			return False
		parent = doc.body.main.article.div.div.div
		banner = parent('img')[0].extract()
		parent('h1')[0].replace_with(banner)
		parent = doc.new_tag('div', class_='gh-badges', after=banner)
		for (alt, src, href) in self.__badges:
			anchor = doc.new_tag('a', parent=parent, href=href, target='_blank')
			doc.new_tag('img', parent=anchor, src=src, alt='caption')
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
				if (next is not None and next.get('class') is not None and 'tpp-injected' in next.get('class')):
					continue
				doc.new_tag('span',
					after=anchor,
					string='inline',
					title=inline_namespace_explainer,
					class_='m-label m-info m-flat tpp-injected tpp-inline-namespace'
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
			tag = h1.select_one('span.tpp-injected')
			if (tag is None):
				tag = doc.new_tag('span',
					parent=h1,
					string='inline',
					title=inline_namespace_explainer,
					class_='m-label m-info tpp-injected tpp-inline-namespace'
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
			if (next is not None and next.get('class') is not None and 'tpp-injected' in next.get('class')):
				continue
			doc.new_tag('span',
				after=anchor,
				string='inline',
				title=inline_namespace_explainer,
				class_='m-label m-info m-flat tpp-injected tpp-inline-namespace'
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
	__types = [
		(r'(?:std::)?size_t', 'https://en.cppreference.com/w/cpp/types/size_t'),
		(r'(?:std::)?u?int(?:8|16|32|64)(fast|least)?_ts?', 'https://en.cppreference.com/w/cpp/types/integer'),
		(r'std::pairs?', 'https://en.cppreference.com/w/cpp/utility/pair'),
		(r'std::bytes?', 'https://en.cppreference.com/w/cpp/types/byte'),
		(r'std::optionals?', 'https://en.cppreference.com/w/cpp/utility/optional'),
		(r'std::tuples?', 'https://en.cppreference.com/w/cpp/utility/tuple'),
		(r'std::integral_constants?', 'https://en.cppreference.com/w/cpp/types/integral_constant'),
		(r'std::char_traits', 'https://en.cppreference.com/w/cpp/string/char_traits'),
		(r'std::allocators?', 'https://en.cppreference.com/w/cpp/memory/allocator'),
		(r'std::enable_if(?:_t)?', 'https://en.cppreference.com/w/cpp/types/enable_if'),
		(r'std::conditional(?:_t)?', 'https://en.cppreference.com/w/cpp/types/conditional'),
		(r'std::unordered_maps?', 'https://en.cppreference.com/w/cpp/container/unordered_map'),
		(r'std::unordered_sets?', 'https://en.cppreference.com/w/cpp/container/unordered_set'),
		(r'std::maps?', 'https://en.cppreference.com/w/cpp/container/map'),
		(r'std::sets?', 'https://en.cppreference.com/w/cpp/container/set'),
		(r'std::vectors?', 'https://en.cppreference.com/w/cpp/container/vector'),
		(r'std::arrays?', 'https://en.cppreference.com/w/cpp/container/array'),
		(r'std::chrono::durations?', 'https://en.cppreference.com/w/cpp/chrono/duration'),
		(
			r'std::atomic(?:_(?:'
				+ r'bool|[su]?char(?:8_t|16_t|32_t)?|u?short'
				+ r'|u?int(?:8_t|16_t|32_t|64_t)?|u?l?long'
				+ r'))?',
			'https://en.cppreference.com/w/cpp/atomic/atomic'
		),
		(r'std::unique_ptrs?', 'https://en.cppreference.com/w/cpp/memory/unique_ptr'),
		(r'std::shared_ptrs?', 'https://en.cppreference.com/w/cpp/memory/shared_ptr'),
		(r'(?:std::)?nullptr_t', 'https://en.cppreference.com/w/cpp/types/nullptr_t'),
		(r'std::reverse_iterator', 'https://en.cppreference.com/w/cpp/iterator/reverse_iterator'),
		(r'std::(?:basic_|w)?istreams?', 'https://en.cppreference.com/w/cpp/io/basic_istream'),
		(r'std::(?:basic_|w)?ostreams?', 'https://en.cppreference.com/w/cpp/io/basic_ostream'),
		(r'std::(?:basic_|w)?iostreams?', 'https://en.cppreference.com/w/cpp/io/basic_iostream'),
		(r'std::(?:basic_|w)?ifstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ifstream'),
		(r'std::(?:basic_|w)?ofstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ofstream'),
		(r'std::(?:basic_|w)?fstreams?', 'https://en.cppreference.com/w/cpp/io/basic_fstream'),
		(r'std::(?:basic_|w)?istringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_istringstream'),
		(r'std::(?:basic_|w)?ostringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_ostringstream'),
		(r'std::(?:basic_|w)?stringstreams?', 'https://en.cppreference.com/w/cpp/io/basic_stringstream'),
		(r'std::(?:basic_|w|u8)?string_views?', 'https://en.cppreference.com/w/cpp/string/basic_string_view'),	
		(r'std::(?:basic_|w|u8)?strings?', 'https://en.cppreference.com/w/cpp/string/basic_string'),	
		(r'\s(?:<|&lt;)fstream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/fstream'),
		(r'\s(?:<|&lt;)sstream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/sstream'),
		(r'\s(?:<|&lt;)iostream(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/iostream'),
		(r'\s(?:<|&lt;)iosfwd(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/iosfwd'),
		(r'\s(?:<|&lt;)string(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/string'),
		(r'\s(?:<|&lt;)string_view(?:>|&gt;)', 'https://en.cppreference.com/w/cpp/header/string_view'),
		(r'char(?:8|16|32)_ts?', 'https://en.cppreference.com/w/cpp/language/types'),
		(r'std::is_(?:nothrow_)?convertible(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_convertible'),
		(r'std::is_same(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_same'),
		(r'std::is_base_of(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_base_of'),
		(r'std::is_enum(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_enum'),
		(r'std::is_floating_point(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_floating_point'),
		(r'std::is_integral(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_integral'),
		(r'std::is_pointer(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_pointer'),
		(r'std::is_reference(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_reference'),
		(r'std::is_signed(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_signed'),
		(r'std::is_unsigned(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_unsigned'),
		(r'std::is_void(?:_v)?', 'https://en.cppreference.com/w/cpp/types/is_void'),
		(r'std::is_(?:nothrow_)?invocable(?:_r)?', 'https://en.cppreference.com/w/cpp/types/is_invocable'),
		(r'std::add_[lr]value_reference(?:_t)?', 'https://en.cppreference.com/w/cpp/types/add_reference'),
		(r'std::remove_reference(?:_t)?', 'https://en.cppreference.com/w/cpp/types/remove_reference'),
		(r'std::remove_cv(?:_t)?', 'https://en.cppreference.com/w/cpp/types/remove_cv'),
		(r'std::underlying_type(?:_t)?', 'https://en.cppreference.com/w/cpp/types/underlying_type'),
		(r'std::exceptions?', 'https://en.cppreference.com/w/cpp/error/exception'),
		(r'std::runtime_errors?', 'https://en.cppreference.com/w/cpp/error/runtime_error'),
		(r'std::is_constant_evaluated', 'https://en.cppreference.com/w/cpp/types/is_constant_evaluated'),
		(r'std::launder', 'https://en.cppreference.com/w/cpp/utility/launder'),
		(r'std::bit_width', 'https://en.cppreference.com/w/cpp/numeric/bit_width'),
		(r'std::bit_ceil', 'https://en.cppreference.com/w/cpp/numeric/bit_ceil'),
		(r'std::bit_floor', 'https://en.cppreference.com/w/cpp/numeric/bit_floor'),
		(r'std::bit_cast', 'https://en.cppreference.com/w/cpp/numeric/bit_cast'),
		(r'std::countl_zero', 'https://en.cppreference.com/w/cpp/numeric/countl_zero'),
		(r'std::countr_zero', 'https://en.cppreference.com/w/cpp/numeric/countr_zero'),
		(r'std::countl_one', 'https://en.cppreference.com/w/cpp/numeric/countl_one'),
		(r'std::countr_one', 'https://en.cppreference.com/w/cpp/numeric/countr_one'),
		(r'std::has_single_bit', 'https://en.cppreference.com/w/cpp/numeric/has_single_bit'),
		(r'std::min', 'https://en.cppreference.com/w/cpp/algorithm/min'),
		(r'std::max', 'https://en.cppreference.com/w/cpp/algorithm/max'),
		(r'std::clamp', 'https://en.cppreference.com/w/cpp/algorithm/clamp'),
		(r'std::numeric_limits', 'https://en.cppreference.com/w/cpp/types/numeric_limits'),
		(r'std::initializer_lists?', 'https://en.cppreference.com/w/cpp/utility/initializer_list'),
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
	__allowedNames = ['dd', 'p', 'dt', 'h3', 'td', 'div']
	
	def __init__(self):
		self.__expressions = []
		for type, uri in self.__types:
			self.__expressions.append((re.compile('(?<![a-zA-Z_])'+type+'(?![a-zA-Z_])'), uri))
	
	@classmethod
	def __substitute(cls, m, uri):
		external = uri.startswith('http')
		return r'<a href="{}" class="m-doc tpp-injected{}"{}>{}</a>'.format(
			uri,
			' tpp-external' if external else '',
			' target="_blank"' if external else '',
			m.group(0),
		)

	def __call__(self, file, doc):
		changed = False
		root = doc.body.main.article.div.div
		
		tags = tags = html_shallow_search(root, self.__allowedNames, lambda t: html_find_parent(t, 'a', root) is None)

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
		return r'{}<span class="tpp-injected tpp-enable-if"><a href="#" onclick="ToggleEnableIf(this);return false;">...</a><span>{}</span></span>{}'.format(
			m.group(1),
			m.group(2),
			m.group(3)
		)
	
	def __call__(self, file, doc):
		changed = False
		for template in doc.body('div', class_='m-doc-template'):
			replacer = RegexReplacer(self.__expression, lambda m: self.__substitute(m), str(template))
			if replacer:
				injected = html_replace_tag(template, str(replacer))[0].select_one(".tpp-enable-if")
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



def delete_directory(dir_path):
	if (path.exists(dir_path)):
		print('Deleting {}'.format(dir_path))
		shutil.rmtree(dir_path)



def get_script_folder():
	return path.dirname(path.realpath(sys.argv[0]))



def run_python_script(script_path, *args):
	subprocess.check_call(
		['py' if is_tool('py') else 'python3', script_path] + [arg for arg in args]
	)



def preprocess_xml(xml_dir):
	pass



def main():
	global _threadError
	
	num_threads = os.cpu_count() * 2
	root_dir = path.join(get_script_folder(), '..')
	docs_dir = path.join(root_dir, 'docs')
	xml_dir = path.join(docs_dir, 'xml')
	html_dir = path.join(docs_dir, 'html')
	mcss_dir = path.join(root_dir, 'extern', 'mcss')
	doxygen = path.join(mcss_dir, 'documentation', 'doxygen.py')

	# delete any previously generated html and xml
	delete_directory(xml_dir)
	delete_directory(html_dir)

	# run doxygen
	subprocess.check_call( ['doxygen', 'Doxyfile'], shell=True, cwd=docs_dir )

	# fix some shit that's broken in the xml
	preprocess_xml(xml_dir)

	# run doxygen.py (m.css)
	run_python_script(doxygen, path.join(docs_dir, 'Doxyfile-mcss'), '--no-doxygen')

	# delete xml
	delete_directory(xml_dir)
		
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
	]
	files = [path.split(f) for f in get_all_files(html_dir, any=('*.html', '*.htm'))]
	if files:
		with concurrent.futures.ThreadPoolExecutor(max_workers=min(len(files), num_threads)) as executor:
			jobs = { executor.submit(postprocess_file, dir, file, fixes) : file for dir, file in files }
			for job in concurrent.futures.as_completed(jobs):
				if _threadError:
					executor.shutdown(False)
					break
				else:
					file = jobs[job]
					print('Finished processing {}.'.format(file))
		if _threadError:
			sys.exit(1)


if __name__ == '__main__':
	try:
		main()
	except Exception as err:
		print(
			'Error: [{}] {}'.format(
				type(err).__name__,
				str(err)
			),
			file=sys.stderr
		)
		traceback.print_exc(file=sys.stderr)
		sys.exit(1)
	sys.exit()
