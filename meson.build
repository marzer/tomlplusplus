project(
	'tomlplusplus', 'cpp',
	version : '0.1.0',
	license : 'MIT',
	default_options : [
		'cpp_std=c++17',
		'warning_level=3',
		'cpp_eh=default'
	]
)
 

if meson.get_compiler('cpp').get_id() == 'gcc'
	add_project_arguments(['-fmax-errors=5', '-Wno-attributes'], language : 'cpp')
endif

if meson.get_compiler('cpp').get_id() == 'clang'
	add_project_arguments('-ferror-limit=5', language : 'cpp')
endif

#pymod = import('python')
#python = pymod.find_installation('python3')

inc = include_directories('include')

subdir('tests')
#subdir('examples')

