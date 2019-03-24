#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <VX/vx.h>
#include <VX/vxu.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace spdlog;

enum ReturnValue {
	OK,
	ERROR
};

const char* vxStatusToStr (vx_status e) {
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

// Check COMMAND to VX_SUCCESS and if it is not, print error message and
// exits the application.
#define CHECK_VX_STATUS(COMMAND) \
  { \
		vx_status __local_status = COMMAND; \
		if(__local_status != VX_SUCCESS) \
		{ \
			error("VX API call failed with {}" \
					" file: {}" \
					" line: {}" \
					, vxStatusToStr(__local_status) \
					, __FILE__ \
					, __LINE__); \
			std::exit(1); \
		} \
  }

vx_df_image mat_type_to_image_format(int mat_type)
{
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
			error("Unsupported format: {}", mat_type);
			return 0;
	}
}

vx_image create_image_from_mat(
		vx_context ctx,
		const cv::Mat& mat) {
	vx_imagepatch_addressing_t format;
	format.dim_x = mat.cols;
	format.dim_y = mat.rows;
	format.stride_x = mat.elemSize();
	format.stride_y = mat.step;
	format.scale_x = VX_SCALE_UNITY;
	format.scale_y = VX_SCALE_UNITY;
	format.step_x = 1;
	format.step_y = 1;
	return vxCreateImageFromHandle(
			ctx,
			mat_type_to_image_format(mat.type()),
			&format,
			(void**)&mat.data,
			VX_MEMORY_TYPE_HOST);
}

int main(int argc, char** argv) {
	cxxopts::Options options("ovx_sample", "Sample for OpenVX");
	options
		.add_options()
		("i,input", "Path for input image", cxxopts::value<std::string>())
		("o,output", "Path for output image", cxxopts::value<std::string>())
		("h,help", "Print help");
	auto result = options.parse(argc, argv);
	if (result.count("help")) {
		std::cout << options.help({""}) << std::endl;
		return OK;
	}
	if (!result.count("i")) {
		error("Input data path is required");
		return ERROR;
	}
	auto in_path = result["i"].as<std::string>();
	info("Initializing resources");
	info("Reading input file: {}", in_path);
	cv::Mat input = cv::imread(in_path);
	if (input.empty()) {
		error("Failed to read {}", in_path);
		return ERROR;
	}
	cv::Mat output(input.size(), input.type());

	vx_context context = vxCreateContext();
	vx_graph graph = vxCreateGraph(context);
	vx_image input_img = create_image_from_mat(context, input);
	if (input_img == nullptr) {
		error("Failed to convert input cv::Mat to vx_image");
		return ERROR;
	}
	info("Releasing resources");
	CHECK_VX_STATUS(vxReleaseImage(&input_img));
	CHECK_VX_STATUS(vxReleaseGraph(&graph));
	CHECK_VX_STATUS(vxReleaseContext(&context));
	return OK;
}
