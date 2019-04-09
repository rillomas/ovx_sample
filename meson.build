project('ovx_sample', 'cpp',
	default_options:['cpp_std=c++11', 'warning_level=2'])
cc = meson.get_compiler('cpp')
INTEL_BASE_DIR = '/opt/intel/'
VTUNE_DIR = join_paths(INTEL_BASE_DIR, 'system_studio_2019/vtune_amplifier/')
OPENVINO_DIR = join_paths(INTEL_BASE_DIR, 'computer_vision_sdk')
OPENVX_DIR = join_paths(OPENVINO_DIR, 'openvx')
OPENCV_DIR = join_paths(OPENVINO_DIR, 'opencv')
SUB_PROJECT_DIR = 'subprojects'
vx_incdir = include_directories(join_paths(OPENVX_DIR,'include'))
vt_incdir = include_directories(join_paths(VTUNE_DIR, 'include'))
spdlog = include_directories(join_paths(SUB_PROJECT_DIR, 'spdlog/include'))
cxxopts = include_directories(join_paths(SUB_PROJECT_DIR, 'cxxopts/include'))
vx_libdir = join_paths(OPENVX_DIR,'lib')
vt_libdir = join_paths(VTUNE_DIR,'lib64')
cv = dependency('opencv')
vx = cc.find_library('libopenvx', dirs:[vx_libdir])
vt = cc.find_library('libittnotify', dirs:[vt_libdir])
dl = cc.find_library('libdl')

subdir('ovx')

sources = [
	'main.cpp',
]
executable(
	'ovx_sample',
	sources,
	include_directories:[
		vx_incdir,
		vt_incdir,
		spdlog,
		cxxopts
	],
	cpp_pch:'pch/header.hpp',
	link_with: [ovxlib],
	dependencies:[
		cv,
		vx,
		vt,
		dl,
	])
