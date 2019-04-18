#include "ct.hpp"
#include "util.hpp"
#include "HalideBuffer.h"
#include "back_projection_generic.h"
using namespace ovx::util;

namespace ovx {
namespace ct {

// Number of params for back projection
constexpr int BACK_PROJECTION_PARAM_NUM = 2;

static vx_size format_to_channel_num(vx_node node, vx_df_image format) {
	switch (format) {
		case VX_DF_IMAGE_RGB:
			return 3;
		case VX_DF_IMAGE_U8:
		case VX_DF_IMAGE_U16:
		case VX_DF_IMAGE_U32:
		case VX_DF_IMAGE_S16:
		case VX_DF_IMAGE_S32:
			return 1;
		default:
			LOG_VX(node, VX_FAILURE, "Unsupported image format: %d\n", format);
			return 0;
	}
}

static Halide::Runtime::Buffer<uint8_t> map_and_convert(
	vx_node node,
	vx_image img,
	vx_map_id *map,
	vx_imagepatch_addressing_t *addr){
	vx_uint32 width = 0, height = 0;
	vx_df_image format = VX_DF_IMAGE_VIRT;
	CHECK_VX_STATUS(node, vxQueryImage(
		img,
		VX_IMAGE_FORMAT,
		&format,
		sizeof(vx_df_image)));
	CHECK_VX_STATUS(node, vxQueryImage(img, VX_IMAGE_WIDTH,  &width,  sizeof(width)));
	CHECK_VX_STATUS(node, vxQueryImage(img, VX_IMAGE_HEIGHT, &height, sizeof(height)));
	auto channels = format_to_channel_num(node, format);
	vx_rectangle_t rect = { 0, 0, width, height };
	void* ptr = nullptr;
	CHECK_VX_STATUS(node, vxMapImagePatch(img,  &rect, 0, map, addr,  &ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X));
	Halide::Runtime::Buffer<uint8_t> converted((uint8_t*)ptr, width, height, channels);
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
	auto inimg = map_and_convert(node, input, &inmap, &inaddr);
	auto outimg = map_and_convert(node, output, &outmap, &outaddr);
	auto res = back_projection_generic(inimg, outimg);
	auto result = VX_SUCCESS;
	if (res != 0) {
		result = VX_FAILURE;
		LOG_VX(node, result, "back_projection_basic failed: %d\n", res);
	}
	CHECK_VX_STATUS(node, vxUnmapImagePatch(input, inmap));
	CHECK_VX_STATUS(node, vxUnmapImagePatch(output, outmap));
	return result;
}

vx_status back_projection_validator(
	vx_node node,
	const vx_reference parameters[],
	vx_uint32 num,
	vx_meta_format metas[]) {
	UNUSED(node);
	if (num != BACK_PROJECTION_PARAM_NUM) {
		return VX_ERROR_INVALID_PARAMETERS;
	}
	auto in_img = (vx_image)parameters[0];
	vx_df_image in_format = VX_DF_IMAGE_VIRT;
	CHECK_VX_STATUS(node, vxQueryImage(
		in_img,
		VX_IMAGE_FORMAT,
		&in_format,
		sizeof(vx_df_image)));
	vx_uint32 width, height;
	CHECK_VX_STATUS(node, vxQueryImage(in_img, VX_IMAGE_WIDTH, &width, sizeof(width)));
	CHECK_VX_STATUS(node, vxQueryImage(in_img, VX_IMAGE_HEIGHT, &height, sizeof(height)));
	auto out_meta = metas[1];
	CHECK_VX_STATUS(node, vxSetMetaFormatAttribute(out_meta, VX_IMAGE_FORMAT, &in_format, sizeof(in_format)));
	CHECK_VX_STATUS(node, vxSetMetaFormatAttribute(out_meta, VX_IMAGE_WIDTH, &width, sizeof(width)));
	CHECK_VX_STATUS(node, vxSetMetaFormatAttribute(out_meta, VX_IMAGE_HEIGHT, &height, sizeof(height)));
	return VX_SUCCESS;
}


vx_status register_user_kernel(vx_context ctx) {
	vx_kernel kernel = vxAddUserKernel(ctx,
		BACK_PROJECTION_NAME.c_str(),
		(vx_enum)KernelID::BACK_PROJECTION,
		back_projection_host,
		BACK_PROJECTION_PARAM_NUM,
		back_projection_validator,
		nullptr,
		nullptr);
	CHECK_VX_OBJECT(ctx, kernel);
	CHECK_VX_STATUS(ctx, vxAddParameterToKernel(kernel, 0, VX_INPUT,  VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED));
	CHECK_VX_STATUS(ctx, vxAddParameterToKernel(kernel, 1, VX_OUTPUT, VX_TYPE_IMAGE,  VX_PARAMETER_STATE_REQUIRED));
	CHECK_VX_STATUS(ctx, vxFinalizeKernel(kernel));
	CHECK_VX_STATUS(ctx, vxReleaseKernel(&kernel));
	LOG_INFO("OK: registered user kernel %s\n", BACK_PROJECTION_NAME.c_str());
	return VX_SUCCESS;

}

vx_node back_projection_node(
	vx_graph graph,
	vx_image input,
	vx_image output) {
	auto ctx = vxGetContext((vx_reference)graph);
	auto kernel = vxGetKernelByEnum(ctx, (vx_enum)ovx::ct::KernelID::BACK_PROJECTION);
	CHECK_VX_OBJECT(ctx, kernel);
	auto node = vxCreateGenericNode(graph, kernel);
	CHECK_VX_OBJECT(ctx, node);
	CHECK_VX_STATUS(ctx, vxSetParameterByIndex(node, 0, (vx_reference)input));
	CHECK_VX_STATUS(ctx, vxSetParameterByIndex(node, 1, (vx_reference)output));
	CHECK_VX_STATUS(ctx, vxReleaseKernel(&kernel));
	return node;
}

} // namespace ct
} // namespace ovx
