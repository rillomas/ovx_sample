sources = [
	'back_projection.cpp',
]
LOCAL_DIR = '/home/masato/.local'
HALIDE_BIN_DIR = join_paths(LOCAL_DIR, 'bin')
HALIDE_INC_DIR = join_paths(LOCAL_DIR, 'include')
curses = cc.find_library('libcurses')
halide = cc.find_library('libHalide', dirs:[HALIDE_BIN_DIR])
ctlib = static_library(
	'ct',
	sources,
	include_directories:[
		HALIDE_INC_DIR,
	],
	dependencies:[
		halide,
		curses
	])
