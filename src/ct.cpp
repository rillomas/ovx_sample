#include "ct.hpp"
#include "util.hpp"
#include "HalideBuffer.h"
#include "back_projection_basic.h"
using namespace ovx::util;

namespace ovx {
namespace ct {

// Number of params for back projection
constexpr int BACK_PROJECTION_PARAM_NUM = 2;

static Halide::Runtime::Buffer<uint8_t> map_and_convert(
	vx_image img,
	vx_map_id *map,
	vx_imagepatch_addressing_t *addr){
	vx_uint32 width = 0, height = 0, planes = 0;
	CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_WIDTH,  &width,  sizeof(width)));
	CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_HEIGHT, &height, sizeof(height)));
	CHECK_VX_STATUS(vxQueryImage(img, VX_IMAGE_PLANES, &planes, sizeof(planes)));

	vx_rectangle_t rect = { 0, 0, width, height };
	void* ptr = nullptr;
	CHECK_VX_STATUS(vxMapImagePatch(img,  &rect, 0, map, addr,  &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X));
	Halide::Runtime::Buffer<uint8_t> converted((uint8_t*)ptr, width, height, planes);
	return converted;
}

vx_status VX_CALLBACK back_projection_host(
	vx_node node,
	const vx_reference* refs,
	vx_uint32 num) {
	UNUSED(num);
	auto input = (vx_image)refs[0];
	auto output = (vx_image)refs[1];
	vx_map_id inmap, outmap;
	vx_imagepatch_addressing_t inaddr, outaddr;
	auto inimg = map_and_convert(input, &inmap, &inaddr);
	auto outimg = map_and_convert(output, &outmap, &outaddr);
	auto res = back_projection_basic(inimg, outimg);
	auto result = VX_SUCCESS;
	if (res != 0) {
		result = VX_FAILURE;
		vxAddLogEntry(
			(vx_reference)node,
			result,
			"back_projection_basic failed: %d\n",
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
	UNUSED(node);
	UNUSED(metas);
	if (num != BACK_PROJECTION_PARAM_NUM) {
		return VX_ERROR_INVALID_PARAMETERS;
	}
	vx_df_image format = VX_DF_IMAGE_VIRT;
	CHECK_VX_STATUS(vxQueryImage(
		(vx_image)parameters[0],
		VX_IMAGE_FORMAT,
		&format,
		sizeof(vx_df_image)));
	if (format != VX_DF_IMAGE_U8) {
		return VX_ERROR_INVALID_FORMAT;
	}

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
	CHECK_VX_STATUS(vxAddParameterToKernel(kernel, 0, VX_INPUT,  VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED));
	CHECK_VX_STATUS(vxAddParameterToKernel(kernel, 1, VX_OUTPUT, VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED));
	CHECK_VX_STATUS(vxFinalizeKernel(kernel));
	CHECK_VX_STATUS(vxReleaseKernel(&kernel));
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
	auto context = vxGetContext((vx_reference)graph);
	auto kernel = vxGetKernelByEnum(context, ovx::ct::KernelID::BACK_PROJECTION);
	CHECK_VX_OBJECT(kernel);
	auto node = vxCreateGenericNode(graph, kernel);
	CHECK_VX_OBJECT(node);
	CHECK_VX_STATUS(vxSetParameterByIndex(node, 0, (vx_reference)input));
	CHECK_VX_STATUS(vxSetParameterByIndex(node, 1, (vx_reference)output));
	CHECK_VX_STATUS(vxReleaseKernel(&kernel));
	return node;
}

} // namespace ct
} // namespace ovx
