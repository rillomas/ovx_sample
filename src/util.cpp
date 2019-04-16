#include <opencv2/core/core.hpp>
#include "util.hpp"

namespace ovx {
namespace util {

vx_df_image mat_type_to_image_format(vx_context ctx, int mat_type) {
	switch (mat_type) {
	case CV_8UC1:
		return VX_DF_IMAGE_U8;
	case CV_16SC1:
		return VX_DF_IMAGE_S16;
	case CV_8UC3:
		return VX_DF_IMAGE_RGB;
	case CV_8UC4:
		return VX_DF_IMAGE_RGBX;
	default:
		vxAddLogEntry(
			(vx_reference)ctx,
			VX_ERROR_INVALID_FORMAT,
			"Unsupported format: %d",
			mat_type);
		return 0;
	}
}

vx_imagepatch_addressing_t get_format(const cv::Mat& mat) {
	vx_imagepatch_addressing_t format;
	format.dim_x = mat.cols;
	format.dim_y = mat.rows;
	format.stride_x = mat.elemSize();
	format.stride_y = mat.step;
	format.scale_x = VX_SCALE_UNITY;
	format.scale_y = VX_SCALE_UNITY;
	format.step_x = 1;
	format.step_y = 1;
	return format;
}

vx_image create_image_from_mat(
		vx_context ctx,
		const cv::Mat& mat) {
	auto format = get_format(mat);
	return vxCreateImageFromHandle(
			ctx,
			mat_type_to_image_format(ctx, mat.type()),
			&format,
			(void**)&mat.data,
			VX_MEMORY_TYPE_HOST);
}


const char* status_to_str (vx_status e) {
#define VX_STATUS_TO_STR_ENTRY(E) case E: return #E;
	switch(e)
	{
		VX_STATUS_TO_STR_ENTRY(VX_STATUS_MIN)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_REFERENCE_NONZERO)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_MULTIPLE_WRITERS)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_GRAPH_ABANDONED)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_GRAPH_SCHEDULED)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_SCOPE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_NODE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_GRAPH)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_TYPE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_VALUE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_DIMENSION)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_FORMAT)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_LINK)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_REFERENCE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_MODULE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_INVALID_PARAMETERS)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_OPTIMIZED_AWAY)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NO_MEMORY)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NO_RESOURCES)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NOT_COMPATIBLE)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NOT_ALLOCATED)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NOT_SUFFICIENT)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NOT_SUPPORTED)
		VX_STATUS_TO_STR_ENTRY(VX_ERROR_NOT_IMPLEMENTED)
		VX_STATUS_TO_STR_ENTRY(VX_FAILURE)
		VX_STATUS_TO_STR_ENTRY(VX_SUCCESS)
		default: return "UNKNOWN VX ERROR CODE";
	}
#undef VX_STATUS_TO_STR_ENTRY
}

} // namespace util
} // namespace ovx
