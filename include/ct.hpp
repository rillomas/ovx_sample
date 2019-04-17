#pragma once
#include <string>
#include <VX/vx.h>
namespace ovx {
namespace ct {

// This library's ID
constexpr int LIBRARY_ID = 1;

enum class KernelID {
	BACK_PROJECTION = VX_KERNEL_BASE(VX_ID_DEFAULT, LIBRARY_ID) + 0x001,
};

enum class BackendTarget {
	GENERIC = VX_ENUM_BASE(VX_ID_DEFAULT, VX_ENUM_TARGET) + 0x001,
	CPU_INTEL,
	GPU_INTEL,
};

const std::string BACK_PROJECTION_NAME = "app.ct.back_projection";

// Registers all user kernels for this library.
vx_status register_user_kernel(vx_context ctx);

vx_node back_projection_node(
	vx_graph graph,
	vx_image input,
	vx_image output);

} // namespace ct
} // namespace ovx
