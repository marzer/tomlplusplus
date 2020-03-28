project(
	'tomlplusplus',
	'cpp',
	version : '1.0.0',
	license : 'MIT',
	default_options : [
		'cpp_std=c++17',
		'warning_level=3',
		'werror=true',
		'cpp_eh=default'
	]
)

tomlplusplus_dep = declare_dependency(
	include_directories : include_directories('include'),
	version : meson.project_version(),
)

build_tests = false
if get_option('BUILD_TESTS').auto()
	build_tests = (not meson.is_subproject())
else
	build_tests = get_option('BUILD_TESTS').enabled()
endif

build_examples = false
if get_option('BUILD_EXAMPLES').auto()
	build_examples = (not meson.is_subproject())
else
	build_examples = get_option('BUILD_EXAMPLES').enabled()
endif

if build_tests or build_examples

	compiler = meson.get_compiler('cpp')
	message(['compiler ID: ', compiler.get_id()])
	message(['compiler version: ', compiler.version()])

	if compiler.get_id() == 'gcc'
		add_project_arguments([
				'-g0',
				'-fmax-errors=5',
				'-march=native',
				'-Wno-init-list-lifetime'
			],
			language : 'cpp'
		)
	endif

	if compiler.get_id() == 'clang'
		add_project_arguments([
				'-g0',
				'-ferror-limit=5',
				'-march=native',
				'-fchar8_t',
				# '-Weverything',
				'-Wno-c++98-compat',
				'-Wno-c++98-compat-pedantic',
				'-Wno-float-equal',
				'-Wno-switch-enum',
				'-Wno-documentation-unknown-command',
				'-Wno-padded',
				'-Wno-weak-vtables',
				'-Wno-double-promotion'
				#, '-ftime-trace'
			],
			language : 'cpp'
		)
	endif

	if compiler.get_id() == 'intel-cl'
		add_project_arguments([
				'/Qoption,cpp,--unicode_source_kind,UTF-8',
				'/std=c++latest',
				'/wd82', # storage class is not first
				'/wd280', # selector expression is constant (why the fuck is that a warning?)
				'/wd411', # class provides no constructor (duh, it's an aggregate)
				'/wd1011', # missing return statement (false negative)
				'/wd1628', # function marked [[noreturn]] returns (false positive)
				'/wd3280' # declaration hides member (triggered in Catch2)
			],
			language : 'cpp'
		)
	endif

	inc = include_directories('include', 'extern')

	if build_tests
		subdir('tests')
	else
		message('Not building tests')
	endif

	if build_examples
		subdir('examples')
	else
		message('Not building examples')
	endif

endif

