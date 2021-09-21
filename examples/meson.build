example_args = []
example_args += additional_arguments
if is_gcc or is_clang
	example_args += [ '-Wno-switch', '-Wno-switch-default', '-Wno-switch-enum' ]
endif

examples = [
	'simple_parser',
	'toml_to_json_transcoder',
	'toml_generator',
	'error_printer'
]

foreach example : examples
	executable(
		example,
		[ example+'.cpp' ],
		include_directories: include_dirs,
		cpp_args: example_args,
		override_options: overrides
	)
endforeach
