sources = [
	'util.cpp',
	'ct.cpp'
]
ovxlib = static_library(
	'ovx',
	sources,
	include_directories:[
		vx_incdir,
		spdlog,
	],
	dependencies:[
		vx,
		ctlib_dep
	])
