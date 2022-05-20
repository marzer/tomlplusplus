example_args = []
example_args += universal_args
example_args += devel_args
example_overrides = []
example_overrides += overrides
if is_gcc or is_clang
	example_args += [ '-Wno-switch', '-Wno-switch-default', '-Wno-switch-enum' ]
endif
if is_clang and (get_option('asan_examples') or get_option('ubsan_examples'))
	example_args += [ '-g3' ]
	if get_option('asan_examples') and get_option('ubsan_examples')
		example_overrides += 'b_sanitize=address,undefined'
	elif get_option('asan_examples')
		example_overrides += 'b_sanitize=address'
	else
		example_overrides += 'b_sanitize=undefined'
	endif
endif

examples = [
	'simple_parser',
	'toml_to_json_transcoder',
	'toml_generator',
	'error_printer',
	'parse_benchmark',
	'toml_merger',
]

example_executables = []
foreach example : examples
	example_executables += [[
		example,
		executable(
			example,
			[ example+'.cpp' ],
			cpp_args: example_args,
			dependencies: tomlplusplus_dep,
			override_options: example_overrides
		)
	]]
endforeach

if is_clang and (get_option('asan_examples') or get_option('ubsan_examples'))
	foreach executable : example_executables

		# skip the transcoder (it waits on stdin) and the benchmark (might be slow and cause CI to fail)
		if executable[0].contains('transcoder') or executable[0].contains('benchmark')
			continue
		endif

		test(
			executable[0], # name
			executable[1], # executable object
			workdir: meson.source_root()/'examples'
		)

	endforeach
endif
