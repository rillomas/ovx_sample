#pragma once
#include <string>
#include <VX/vx.h>
namespace ovx {
namespace ct {

// This library's ID
constexpr int LIBRARY_ID = 1;

enum class KernelID {
	BACK_PROJECTION_GENERIC = VX_KERNEL_BASE(VX_ID_DEFAULT, LIBRARY_ID) + 0x0,
	BACK_PROJECTION_CPU = VX_KERNEL_BASE(VX_ID_DEFAULT, LIBRARY_ID) + 0x1,
};

// This does not work with vxSetNodeTarget.
// We get VX_ERROR_INVALID_PARAMETERS error.
// enum class BackendTarget {
// 	GENERIC = VX_ENUM_BASE(VX_ID_DEFAULT, VX_ENUM_TARGET) + 0x0,
// 	CPU_INTEL = VX_ENUM_BASE(VX_ID_DEFAULT, VX_ENUM_TARGET) + 0x1,
// 	GPU_INTEL = VX_ENUM_BASE(VX_ID_DEFAULT, VX_ENUM_TARGET) + 0x2,
// };

// const std::string TARGET_GENERIC = "app.ct.back_projection.generic";
// const std::string TARGET_CPU_INTEL = "app.ct.back_projection.cpu.intel";
// const std::string TARGET_GPU_INTEL = "app.ct.back_projection.gpu.intel";

const std::string BACK_PROJECTION_GENERIC_NAME = "app.ct.back_projection.generic";
const std::string BACK_PROJECTION_CPU_NAME = "app.ct.back_projection.generic";

// Registers all user kernels for this library.
vx_status register_user_kernel(vx_context ctx);

vx_node back_projection_generic_node(
	vx_graph graph,
	vx_image input,
	vx_image output);

vx_node back_projection_cpu_node(
	vx_graph graph,
	vx_image input,
	vx_image output);


} // namespace ct
} // namespace ovx
