#pragma once
#include <VX/vx.h>
namespace ct {

// ReturnValue is a general return value for this namespace
enum ReturnValue {
	OK,
	ERROR
};

enum LibraryID {
	CT = 1,
};

enum KernelID {
	BACK_PROJECTION = VX_KERNEL_BASE(VX_ID_DEFAULT, LibraryID::CT) + 0x001,
};

const std::string BACK_PROJECTION_NAME = "app.ct.back_projection";

// Registers all user kernels for this library.
vx_status register_user_kernel(vx_context ctx);

vx_node back_projection_node(
  vx_graph graph,
  vx_image input,
  vx_image output);

} // namespace ct
