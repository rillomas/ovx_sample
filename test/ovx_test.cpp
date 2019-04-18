#include <gtest/gtest.h>
#include <opencv2/highgui/highgui.hpp>
#include <VX/vx.h>
#include <chrono>
#include "util.hpp"
#include "ct.hpp"

#define GET_STATUS(obj) vxGetStatus((vx_reference)obj)

static void VX_CALLBACK log_callback(
	vx_context ctx,
	vx_reference ref,
	vx_status status,
	const vx_char* string) {
	UNUSED(ctx);
	UNUSED(ref);
	printf("LOG[status: %s] %s", ovx::util::status_to_str(status), string);
}

static vx_status write_to_file(
	vx_context ctx,
	vx_image output_img,
	const cv::Mat& output,
	const std::string& out_path) {
	// Transfer ownership to cv::Mat and write output image to file
	vx_rectangle_t output_rect = {0, 0, (uint32_t)output.cols, (uint32_t)output.rows};
	void* p = nullptr;
	const uint32_t plane_index = 0;
	vx_map_id output_map;
	vx_imagepatch_addressing_t addr;
	CHECK_VX_STATUS(ctx, vxMapImagePatch(
			output_img,
			&output_rect,
			plane_index,
			&output_map,
			&addr,
			&p,
			VX_READ_ONLY,
			VX_MEMORY_TYPE_HOST,
			VX_NOGAP_X));
	bool ok = cv::imwrite(out_path, output);
	if (!ok) {
		return VX_FAILURE;
	}
	// Release ownership
	return vxUnmapImagePatch(output_img, output_map);
}

class OVXTestFixture : public ::testing::Test {
protected:
	virtual void SetUp() {
		ctx_ = vxCreateContext();
		ASSERT_EQ(VX_SUCCESS, vxGetStatus((vx_reference)ctx_));
		vxRegisterLogCallback(ctx_, log_callback, vx_false_e);
		ASSERT_EQ(VX_SUCCESS, ovx::ct::register_user_kernel(ctx_));
		graph_ = vxCreateGraph(ctx_);
		ASSERT_NE(nullptr, graph_);
	}

	virtual void TearDown() {
		ASSERT_EQ(VX_SUCCESS, vxReleaseGraph(&graph_));
		ASSERT_EQ(VX_SUCCESS, vxReleaseContext(&ctx_));
	}

	vx_context ctx_;
	vx_graph graph_;
};

TEST_F(OVXTestFixture, RunDefaultBackProjection) {
	auto input = cv::imread("../test/data/lena.png");
	EXPECT_FALSE(input.empty());
	auto input_img = ovx::util::create_image_from_mat(ctx_, input);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(input_img));
	cv::Mat output(input.size(), input.type());
	auto format = ovx::util::get_format(output);
	vx_image output_img = vxCreateImageFromHandle(
			ctx_,
			ovx::util::mat_type_to_image_format(ctx_, output.type()),
			&format,
			(void**)&output.data,
			VX_MEMORY_TYPE_HOST);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(output_img));
	auto n = ovx::ct::back_projection_generic_node(graph_, input_img, output_img);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(n));
	EXPECT_EQ(VX_SUCCESS, vxReleaseNode(&n));
	EXPECT_EQ(VX_SUCCESS, vxVerifyGraph(graph_));
	auto start = std::chrono::high_resolution_clock::now();
	EXPECT_EQ(VX_SUCCESS, vxProcessGraph(graph_));
	auto end = std::chrono::high_resolution_clock::now();
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
	std::cout << usec << " usec" << std::endl;
	EXPECT_EQ(VX_SUCCESS, write_to_file(ctx_, output_img, output, "lena_bright_generic.png"));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&input_img));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&output_img));
}

TEST_F(OVXTestFixture, RunCPUBackProjection) {
	auto input = cv::imread("../test/data/lena.png");
	EXPECT_FALSE(input.empty());
	auto input_img = ovx::util::create_image_from_mat(ctx_, input);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(input_img));
	cv::Mat output(input.size(), input.type());
	auto format = ovx::util::get_format(output);
	vx_image output_img = vxCreateImageFromHandle(
			ctx_,
			ovx::util::mat_type_to_image_format(ctx_, output.type()),
			&format,
			(void**)&output.data,
			VX_MEMORY_TYPE_HOST);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(output_img));
	auto n = ovx::ct::back_projection_cpu_node(graph_, input_img, output_img);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(n));
	EXPECT_EQ(VX_SUCCESS, vxReleaseNode(&n));
	EXPECT_EQ(VX_SUCCESS, vxVerifyGraph(graph_));
	auto start = std::chrono::high_resolution_clock::now();
	EXPECT_EQ(VX_SUCCESS, vxProcessGraph(graph_));
	auto end = std::chrono::high_resolution_clock::now();
	auto usec = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
	std::cout << usec << " usec" << std::endl;
	EXPECT_EQ(VX_SUCCESS, write_to_file(ctx_, output_img, output, "lena_bright_cpu.png"));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&input_img));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&output_img));
}
