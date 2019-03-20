project('ovx_sample', 'cpp',
    default_options:['cpp_std=c++11', 'warning_level=2'])
cc = meson.get_compiler('cpp')
OPENVINO_DIR = '/opt/intel/computer_vision_sdk'
OPENVX_DIR = join_paths(OPENVINO_DIR, 'openvx')
EXTERNAL_DIR = 'external'
vx_incdir = include_directories(join_paths(OPENVX_DIR,'include'))
spdlog = include_directories(join_paths(EXTERNAL_DIR, 'spdlog/include'))
vx_libdir = join_paths(OPENVX_DIR,'lib')
vx = cc.find_library('libopenvx', dirs:[vx_libdir])

sources = [
  'main.cpp',
]
executable(
    'ovx_sample',
    sources,
    include_directories:[vx_incdir, spdlog],
    cpp_pch:'pch/header.hpp',
    dependencies:[vx])
