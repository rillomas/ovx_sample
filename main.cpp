#include <chrono>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>
#include <VX/vx.h>
#include <VX/vxu.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ittnotify.h>
#include "util.hpp"
#include "ct.hpp"
using namespace spdlog;
using namespace ovx::util;
using namespace ovx::ct;

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
		return ReturnValue::OK;
	}
	if (!result.count("i")) {
		error("Input data path is required");
		return ReturnValue::ERROR;
	}
	if (!result.count("o")) {
		error("Output data path is required");
		return ReturnValue::ERROR;
	}
	auto in_path = result["i"].as<std::string>();
	auto out_path = result["o"].as<std::string>();
	info("Initializing resources");
	info("Reading input file: {}", in_path);
	// We read as grayscale since most of the OpenVX
	// nodes only support VX_DF_IMAGE_U8 input/output
	cv::Mat input = cv::imread(in_path, cv::IMREAD_GRAYSCALE);
	if (input.empty()) {
		error("Failed to read {}", in_path);
		return ReturnValue::ERROR;
	}

	vx_context context = vxCreateContext();
	vx_graph graph = vxCreateGraph(context);
	vx_image input_img = create_image_from_mat(context, input);
	if (input_img == nullptr) {
		error("Failed to convert input cv::Mat to vx_image");
		return ReturnValue::ERROR;
	}
	cv::Mat output(input.size(), input.type());
	auto format = get_format(output);
	vx_image output_img = vxCreateImageFromHandle(
			context,
			mat_type_to_image_format(context, output.type()),
			&format,
			(void**)&output.data,
			VX_MEMORY_TYPE_HOST);
  CHECK_VX_OBJECT(context, output_img);
	// Construct graph and execute
	vxGaussian3x3Node(graph, input_img, output_img);
	CHECK_VX_STATUS(context, vxVerifyGraph(graph));

	const std::string name = "Process";
	auto event = __itt_event_create(name.c_str(),name.length());
	constexpr int ITERATIONS = 30;
	std::vector<std::chrono::microseconds> time;
	for (int i=0; i<ITERATIONS; i++) {
		__itt_event_start(event);
		auto start = std::chrono::high_resolution_clock::now();
		CHECK_VX_STATUS(context, vxProcessGraph(graph));
		auto end = std::chrono::high_resolution_clock::now();
		__itt_event_end(event);
		time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-start));
	}

	for (const auto& t : time) {
		info("time: {} usec", t.count());
	}
	// Skip the first run when calculating average
	// because the first run has more overhead.
	constexpr int SKIP_COUNT = 1;
	auto sum = std::accumulate(
			std::next(std::begin(time), SKIP_COUNT),
			std::end(time),
			std::chrono::microseconds(0)).count();
	float average =  sum / (float)(ITERATIONS-SKIP_COUNT);
	info("time average: {} usec", average);

	// Transfer ownership to cv::Mat and write output image to file
	vx_rectangle_t output_rect = {0, 0, (uint32_t)output.cols, (uint32_t)output.rows};
	void* p = nullptr;
	const uint32_t plane_index = 0;
	vx_map_id output_map;
	vx_imagepatch_addressing_t addr;
	CHECK_VX_STATUS(context, vxMapImagePatch(
			output_img,
			&output_rect,
			plane_index,
			&output_map,
			&addr,
			&p,
			VX_READ_ONLY,
			VX_MEMORY_TYPE_HOST,
			0));
	bool ok = cv::imwrite(out_path, output);
	if (!ok) {
		error("Output to {} failed", out_path);
		return ReturnValue::ERROR;
	}
	info("Output to {}", out_path);
	// Relase ownership
	vxUnmapImagePatch(output_img, output_map);

	info("Releasing resources");
	CHECK_VX_STATUS(context, vxReleaseImage(&input_img));
	CHECK_VX_STATUS(context, vxReleaseImage(&output_img));
	CHECK_VX_STATUS(context, vxReleaseGraph(&graph));
	vxReleaseContext(&context);
	return ReturnValue::OK;
}
