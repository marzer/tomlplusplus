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
 

compiler = meson.get_compiler('cpp')

if compiler.get_id() == 'gcc'
	add_project_arguments(['-fmax-errors=5', '-Wno-attributes', '-Wno-init-list-lifetime' ], language : 'cpp')
endif

if compiler.get_id() == 'clang'
	add_project_arguments(['-ferror-limit=5', '-fchar8_t'], language : 'cpp')
endif

compiler_supports_char8_strings = compiler.compiles('''
	#include <string_view>
	#include <string>
	using namespace std::string_view_literals;
	std::u8string func()
	{
		return std::u8string{ u8"this is a test."sv };
	}
	''',
	name : 'char8 string check',
	args : [ '-std=c++2a' ]
)

#pymod = import('python')
#python = pymod.find_installation('python3')

inc = include_directories(['include', 'extern'])

subdir('tests')
# subdir('examples')
