#include "ct.hpp"
#include "util.hpp"
#include "back_projection.hpp"
using namespace ovx::util;
namespace ctlib = ::ct;

namespace ovx {
namespace ct {

// Number of params for back projection
constexpr int BACK_PROJECTION_PARAM_NUM = 2;

static vx_status map_and_convert(
	ctlib::Image* converted,
	vx_map_id *map,
	vx_imagepatch_addressing_t *addr,
	vx_image img) {
  vx_uint32 width = 0, height = 0, planes = 0;
  CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_WIDTH,  &width,  sizeof(width)));
  CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_HEIGHT, &height, sizeof(height)));
  CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_PLANES, &planes, sizeof(planes)));

  vx_rectangle_t rect = { 0, 0, width, height };
  void* ptr = nullptr;
  CHECK_VX_STATUS(vxMapImagePatch(img,  &rect, 0, map, addr,  &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X));
  converted->data = (uint8_t*)ptr;
  converted->width = width;
  converted->height = height;
  converted->channels = planes;
	return VX_SUCCESS;
}

vx_status VX_CALLBACK back_projection_host(
  vx_node node,
  const vx_reference* refs,
  vx_uint32 num) {
	if (num != BACK_PROJECTION_PARAM_NUM) {
	  return VX_ERROR_INVALID_PARAMETERS;
	}

	auto input = (vx_image)refs[0];
	auto output = (vx_image)refs[1];
	vx_map_id inmap, outmap;
	vx_imagepatch_addressing_t inaddr, outaddr;
	ctlib::Image inimg, outimg;
	CHECK_VX_STATUS(map_and_convert(&inimg, &inmap, &inaddr, input));
	CHECK_VX_STATUS(map_and_convert(&outimg, &outmap, &outaddr, output));
	auto res = ctlib::calc_back_projection_cpu(&outimg, inimg);
	auto result = VX_SUCCESS;
	if (res != ctlib::Result::OK) {
		result = VX_FAILURE;
		vxAddLogEntry(
			(vx_reference)node,
			result,
			"calc_back_projection_cpu failed: %d\n",
			res);
	}
	CHECK_VX_STATUS(vxUnmapImagePatch(input, inmap));
	CHECK_VX_STATUS(vxUnmapImagePatch(output, outmap));
	return result;
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
  auto res = VX_SUCCESS;
  vxAddLogEntry((vx_reference)ctx,
    res,
    "OK: registered user kernel %s\n",
    BACK_PROJECTION_NAME.c_str());
  return res;

}

vx_node back_projection_node(
  vx_graph graph,
  vx_image input,
  vx_image output) {
}

} // namespace ct
} // namespace ovx
