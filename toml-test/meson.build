if get_option('build_tt_encoder')
	executable(
		'tt_encoder',
		'tt_encoder.cpp',
		cpp_args: devel_args,
		include_directories: include_dirs
	)
endif

if get_option('build_tt_decoder')
	executable(
		'tt_decoder',
		'tt_decoder.cpp',
		cpp_args: devel_args,
		include_directories: include_dirs
	)
endif
