#include "ct.hpp"
#include "vx_util.hpp"
using namespace vxutil;

namespace ct {

constexpr int BACK_PROJECTION_PARAM_NUM = 2;

vx_status VX_CALLBACK back_projection_host(
  vx_node node,
  const vx_reference* refs,
  vx_uint32 num) {
	if (num != BACK_PROJECTION_PARAM_NUM) {
	  return VX_ERROR_INVALID_PARAMETERS;
	}

	auto input = (vx_image)refs[0];
	auto output = (vx_image)refs[1];

}

vx_status back_projection_validator(
  vx_node node,
  const vx_reference parameters[],
  vx_uint32 num,
  vx_meta_format metas[]) {
	return VX_SUCCESS;
}


vx_status register_user_kernel(vx_context ctx) {
  vx_kernel kernel = vxAddUserKernel(ctx,
    BACK_PROJECTION_NAME.c_str(),
    KernelID::BACK_PROJECTION,
    back_projection_host,
    BACK_PROJECTION_PARAM_NUM,
    back_projection_validator,
    nullptr,
    nullptr);
  CHECK_VX_OBJECT(kernel);
  vxAddLogEntry((vx_reference)ctx,
    VX_SUCCESS,
    "OK: registered user kernel %s\n",
    BACK_PROJECTION_NAME.c_str());
  return VX_SUCCESS;

}

vx_node back_projection_node(
  vx_graph graph,
  vx_image input,
  vx_image output) {
}

} // namespace ct