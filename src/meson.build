sources = [
	'util.cpp',
	'ct.cpp'
]
ctlib_proj = subproject('ct')
ctlib_dep = ctlib_proj.get_variable('ctlib_dep')
ovxlib = static_library(
	'ovx',
	sources,
	include_directories: ovxlib_inc,
	dependencies:[
		vx,
		ctlib_dep,
		spdlog,
	])

ovxlib_dep = declare_dependency(
	include_directories: ovxlib_inc,
	link_with: ovxlib)
