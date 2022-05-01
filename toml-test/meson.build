tt_deps = [ tomlplusplus_dep ]
if not get_option('use_vendored_libs')
	tt_deps += dependency('nlohmann_json', fallback: ['json', 'nlohmann_json_dep'])
endif

tt_args = []
tt_args += devel_args
if not get_option('use_vendored_libs')
	tt_args += '-DUSE_VENDORED_LIBS=0'
endif

if get_option('build_tt_encoder')
	executable(
		'tt_encoder',
		'tt_encoder.cpp',
		cpp_args: tt_args,
		dependencies: tt_deps
	)
endif

if get_option('build_tt_decoder')
	executable(
		'tt_decoder',
		'tt_decoder.cpp',
		cpp_args: tt_args,
		dependencies: tt_deps
	)
endif
