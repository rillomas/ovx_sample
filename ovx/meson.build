sources = [
	'util.cpp',
	'ct.cpp'
]
ctlib_proj = subproject('ct')
ctlib_dep = ctlib_proj.get_variable('ctlib_dep')
abseil_str = declare_dependency(
	include_directories: join_paths(prefix, incdir),
	dependencies: cc.find_library(
		'libabsl_strings',
		dirs:[join_paths(prefix,libdir)
	]),
)
ovxlib = static_library(
	'ovx',
	sources,
	dependencies:[
		vx,
		ctlib_dep,
		spdlog,
		abseil_str,
	])
