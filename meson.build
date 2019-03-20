project('ovx_sample', 'cpp',
  default_options:['cpp_std=c++11', 'warning_level=2'])
cc = meson.get_compiler('cpp')
OPENVINO_DIR = '/opt/intel/computer_vision_sdk'
OPENVX_DIR = join_paths(OPENVINO_DIR, 'openvx')
OPENCV_DIR = join_paths(OPENVINO_DIR, 'opencv')
EXTERNAL_DIR = 'external'
vx_incdir = include_directories(join_paths(OPENVX_DIR,'include'))
cv_incdir = include_directories(join_paths(OPENCV_DIR,'include'))
spdlog = include_directories(join_paths(EXTERNAL_DIR, 'spdlog/include'))
cxxopts = include_directories(join_paths(EXTERNAL_DIR, 'cxxopts/include'))
vx_libdir = join_paths(OPENVX_DIR,'lib')
cv_libdir = join_paths(OPENCV_DIR,'lib')
cv_core = cc.find_library('libopencv_core', dirs:[cv_libdir])
cv_imgcodecs = cc.find_library('libopencv_imgcodecs', dirs:[cv_libdir])
vx = cc.find_library('libopenvx', dirs:[vx_libdir])

sources = [
  'main.cpp',
]
executable(
  'ovx_sample',
  sources,
  include_directories:[
    vx_incdir,
    cv_incdir,
    spdlog,
    cxxopts
  ],
  cpp_pch:'pch/header.hpp',
  dependencies:[cv_core, cv_imgcodecs, vx])
