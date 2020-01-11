#!/usr/bin/env python3

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



_inlineNamespaces = [
	"toml::literals",
]
_inlineNamespaceExplainer = 'All members of this namespace are automatically members of the parent namespace. '	\
	+ 'It does not require an explicit \'using\' statement.'



def is_tool(name):
	return shutil.which(name) is not None


def is_collection(val):
	if isinstance(val, (list, tuple, dict, set)):
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



def print_value(name, val):
	print('{}:\n    {}'.format(name, val))



def error_message(err, prefix="Error"):
	if (isinstance(err, Exception) or (is_collection(err) and isinstance(err[0], Exception))):
		exc = err[0] if is_collection(err) else err
		trace = err[1] if (is_collection(err) and len(err) > 1) else traceback.format_exc(err)
		print('{}: [{}] {}\n{}'.format(
				prefix,
				type(err).__name__,
				str(err),
				traceback.format_exc(err)
			),
			file=sys.stderr
		)
	else:
		print("{}: {}".format(prefix, err), file=sys.stderr)



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



def html_find_parent(tag,name,cutoff=None):
	parent = tag.parent
	while (parent is not None):
		if (cutoff is not None and parent is cutoff):
			return None
		if (parent.name == name):
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



# changes any links to index.html to link to namespaces.html instead (index.html is blank/unused)
class IndexHrefFix(object): 

	def __call__(self, file, doc):
		links = doc.body('a', href='index.html')
		if (len(links) > 0):
			for link in links:
				link['href'] = 'namespaces.html'
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



# base type for applying inline namespace annotations.
class InlineNamespaceFixBase(object):
	_namespaceFiles = ['namespace{}.html'.format(ns.lower().replace('::','_1_1')) for ns in _inlineNamespaces]



#=======================================================================================================================



# adds inline namespace annotations in class and namespace trees.
class InlineNamespaceFix1(InlineNamespaceFixBase):
	__allowedFiles = ['annotated.html', 'namespaces.html']

	def __call__(self, file, doc):
		global _inlineNamespaceExplainer
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
					title=_inlineNamespaceExplainer,
					class_='m-label m-info m-flat tpp-injected tpp-inline-namespace'
				)
				anchor.insert_after(' ')
				changed = True
		return changed



#=======================================================================================================================



# adds inline namespace annotations to the h1 element of inline namespace pages.
class InlineNamespaceFix2(InlineNamespaceFixBase):

	def __call__(self, file, doc):
		global _inlineNamespaceExplainer
		changed = False
		if (file in self._namespaceFiles):
			h1 = doc.body.find('h1')
			tag = h1.select_one('span.tpp-injected')
			if (tag is None):
				tag = doc.new_tag('span',
					parent=h1,
					string='inline',
					title=_inlineNamespaceExplainer,
					class_='m-label m-info tpp-injected tpp-inline-namespace'
				)
				tag.insert_before(' ')
				changed = True
		return changed



#=======================================================================================================================



# adds inline namespace annotations to sections with id=namespaces.
class InlineNamespaceFix3(InlineNamespaceFixBase):

	def __call__(self, file, doc):
		global _inlineNamespaceExplainer
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
				title=_inlineNamespaceExplainer,
				class_='m-label m-info m-flat tpp-injected tpp-inline-namespace'
			)
			anchor.insert_after(' ')
			changed = True
		return changed



#=======================================================================================================================



# adds a custom footer to the main index pages.
class FooterFix(object):
	__replacement = '<div id="tpp-custom-footer tpp-injected">Documentation generated {} using '	\
		+ '<a href="https://mcss.mosra.cz/">m.css</a></div>'.format(
			datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")
		)

	def __call__(self, file, doc):
		footer = doc.body.find(id='tpp-custom-footer')
		if (footer is None):
			return False
		html_replace_tag(footer, self.__replacement)
		return True



#=======================================================================================================================



# adds links to external sources where appropriate
class ExtDocLinksFix(object): 
	__types = [
		(r'(?:std::)?size_t', 'https://en.cppreference.com/w/cpp/types/size_t'),
		(r'(?:std::)?u?int(?:8|16|32|64)_ts?', 'https://en.cppreference.com/w/cpp/types/integer'),
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
		)
	]
	__allowedNames = ['dd', 'p', 'dt', 'h3', 'td']
	
	def __init__(self):
		self.__expressions = []
		for type, uri in self.__types:
			self.__expressions.append((re.compile(type+'(?!</a>)'), uri))
	
	@classmethod
	def __substitute(cls, m, uri):
		return r'<a href="{}" class="m-doc tpp-injected tpp-external" target="_blank">{}</a>'.format(
			uri,
			m.group(0)
		)
			
	def __process_tag(self, tag):
		for expr, uri in self.__expressions:
			for descendant in tag.descendants:
				if (not isinstance(descendant, soup.NavigableString) or html_find_parent(descendant, 'a', tag) is not None):
					continue
				replacer = RegexReplacer(expr, lambda m: self.__substitute(m, uri), html.escape(str(descendant), quote=False))
				if (replacer):
					html_replace_tag(descendant, str(replacer))
					return True
		return False
			

	def __call__(self, file, doc):
		changed = False
		for name in self.__allowedNames:
			for tag in doc.body.main.article.div.div(name):
				if (len(tag.contents) == 0 or html_find_parent(tag, 'a', doc.body) is not None):
					continue
				while (self.__process_tag(tag)):
					changed = True
					continue
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



_threadError = None



def postprocess_file(dir, file, fixes):
	global _threadError
	if (_threadError is not None):
		return False
	print("Post-processing {}".format(file))
	doc = HTMLDocument(path.join(dir, file))
	file = file.lower()
	changed = False
	for fix in fixes:
		if (fix(file, doc)):
			changed = True
	if (changed):
		doc.flush()
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



def main():
	global _threadError
	
	num_threads = os.cpu_count() * 2
	root_dir = path.join(get_script_folder(), '..')
	docs_dir = path.join(root_dir, 'docs')
	xml_dir = path.join(docs_dir, 'xml')
	html_dir = path.join(docs_dir, 'html')
	mcss_dir = path.join(root_dir, 'extern', 'mcss')
	doxygen = path.join(mcss_dir, 'documentation', 'doxygen.py')

	print_value('doc', docs_dir)
	print_value('xml', xml_dir)
	print_value('html', html_dir)
	print_value('m.css', mcss_dir)
	print_value('doxygen', doxygen)

	# delete any previously generated html and xml
	delete_directory(xml_dir)
	delete_directory(html_dir)

	# run doxygen (via m.css)
	run_python_script(doxygen, path.join(docs_dir, 'Doxyfile-mcss'))

	# clean up xml and tmp files
	delete_directory(xml_dir)
	#for file in get_all_files(cwd, '*.tmp'):
	#	try:
	#		print('Deleting {}'.format(file))
	#		os.remove(file)
	#	except Exception as e:
	#		fatal_error(e)
		
	# post-process html files
	fixes = [
		CustomTagsFix()
		, NavBarFix()
		, IndexHrefFix()
		, ModifiersFix1()
		, ModifiersFix2()
		, InlineNamespaceFix1()
		, InlineNamespaceFix2()
		, InlineNamespaceFix3()
		, FooterFix()
		, ExtDocLinksFix()
		, EnableIfFix()
	]
	files = [path.split(f) for f in get_all_files(html_dir, any=('*.html', '*.htm'))]
	print_value("Files", files)
	if files:
		with concurrent.futures.ThreadPoolExecutor(max_workers=min(len(files), num_threads)) as executor:
			jobs = { executor.submit(postprocess_file, dir, file, fixes) : file for dir, file in files }
			for job in concurrent.futures.as_completed(jobs):
				file = jobs[job]
				try:
					print('Finished processing {}.'.format(file))
				except Exception as e:
					_threadError = (e, traceback.format_exc(e))
					executor.shutdown(False)
					break
		if (_threadError is not None):
			error_message(_threadError, prefix="Fatal error")
			sys.exit(-1)



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
