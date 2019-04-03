sources = [
	'util.cpp',
	'ct.cpp'
]
ovxlib = static_library(
	'ovx',
	sources,
	link_with: [
		ctlib,
	],
	include_directories:[
		vx_incdir,
		spdlog,
	],
	dependencies:[
		vx,
	])
