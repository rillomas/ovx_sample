sources = [
	'util.cpp',
	'ct.cpp'
]
ctlib_proj = subproject('ct')
ctlib_dep = ctlib_proj.get_variable('ctlib_dep')
halide_incdir = join_paths(prefix, get_option('includedir'))
ovxlib = static_library(
	'ovx',
	sources,
	# cpp_args is currently used to suppress Halide's
	# compiler warnings.
	cpp_args:[
		'-Wno-missing-field-initializers',
	],
	include_directories: [
		ovxlib_inc,
		halide_incdir
	],
	dependencies:[
		vx,
		ctlib_dep,
		spdlog,
	])

ovxlib_dep = declare_dependency(
	include_directories: ovxlib_inc,
	link_with: ovxlib)
