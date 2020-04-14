args = []

if compiler.get_id() == 'gcc' or compiler.get_id() == 'clang'
	args += [ '-Wno-switch', '-Wno-switch-default', '-Wno-switch-enum' ]
endif

simple_parser = executable(
	'simple_parser',
	[ 'simple_parser.cpp' ],
	include_directories : inc,
	cpp_args : args
)

toml_to_json_transcoder = executable(
	'toml_to_json_transcoder',
	[ 'toml_to_json_transcoder.cpp' ],
	include_directories : inc,
	cpp_args : args
)

toml_generator = executable(
	'toml_generator',
	[ 'toml_generator.cpp' ],
	include_directories : inc,
	cpp_args : args
)

error_printer = executable(
	'error_printer',
	[ 'error_printer.cpp' ],
	include_directories : inc,
	cpp_args : args
)
