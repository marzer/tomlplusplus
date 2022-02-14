tt_deps = [
	tomlplusplus_dep,
	dependency('nlohmann_json', fallback: ['json', 'nlohmann_json_dep'])
]

if get_option('build_tt_encoder')
	executable(
		'tt_encoder',
		'tt_encoder.cpp',
		cpp_args: devel_args,
		dependencies: tt_deps
	)
endif

if get_option('build_tt_decoder')
	executable(
		'tt_decoder',
		'tt_decoder.cpp',
		cpp_args: devel_args,
		dependencies: tt_deps
	)
endif
