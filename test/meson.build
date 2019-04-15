sources = [
	'ovx_test.cpp',
]
gtest = dependency('gtest_main')
executable('ovx_test',
	sources,
	dependencies:[
		vx,
		ovxlib_dep,
		gtest,
		dl,
		spdlog,
		cv
	])
