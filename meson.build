project('ovx_sample', 'cpp',
	default_options:['cpp_std=c++11', 'warning_level=2'])
cc = meson.get_compiler('cpp')
INTEL_BASE_DIR = '/opt/intel/'
VTUNE_DIR = join_paths(INTEL_BASE_DIR, 'system_studio_2019/vtune_amplifier/')
OPENVINO_DIR = join_paths(INTEL_BASE_DIR, 'computer_vision_sdk')
OPENVX_DIR = join_paths(OPENVINO_DIR, 'openvx')
OPENCV_DIR = join_paths(OPENVINO_DIR, 'opencv')
SUB_PROJECT_DIR = 'subprojects'
prefix = get_option('prefix')
incdir = get_option('includedir')
libdir = get_option('libdir')
spdlog = declare_dependency(
	include_directories: join_paths(SUB_PROJECT_DIR, 'spdlog/include'),
)
cxxopts = declare_dependency(
	include_directories: join_paths(SUB_PROJECT_DIR, 'cxxopts/include'),
)
vx = declare_dependency(
	include_directories: join_paths(OPENVX_DIR,incdir),
	dependencies: cc.find_library(
		'libopenvx',
		dirs:[join_paths(OPENVX_DIR,libdir)
	]),
)
vt = declare_dependency(
	include_directories: join_paths(VTUNE_DIR, incdir),
	dependencies: cc.find_library(
		'libittnotify',
		dirs:[join_paths(VTUNE_DIR,'lib64')
	]),
)
cv = dependency('opencv')
dl = cc.find_library('libdl')
ovxlib_inc = include_directories(incdir)

subdir('src')
subdir('test')

sources = [
	'main.cpp',
]
executable(
	'ovx_sample',
	sources,
	cpp_pch:'pch/header.hpp',
	dependencies:[
		ovxlib_dep,
		cv,
		vx,
		vt,
		dl,
		spdlog,
		cxxopts,
	])
