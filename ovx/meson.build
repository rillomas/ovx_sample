sources = [
	'util.cpp',
	'ct.cpp'
]
ctlib_proj = subproject('ct')
ctlib_dep = ctlib_proj.get_variable('ctlib_dep')
ovxlib = static_library(
	'ovx',
	sources,
	dependencies:[
		vx,
		ctlib_dep,
		spdlog,
	])
