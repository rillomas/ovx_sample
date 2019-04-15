#include <gtest/gtest.h>
#include <opencv2/highgui/highgui.hpp>
#include <VX/vx.h>
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
	printf("LOG[status: %d] %s\n", status, string);
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

TEST_F(OVXTestFixture, RunBackProjection) {
	auto input = cv::imread("../test/data/lena.png", cv::IMREAD_GRAYSCALE);
	EXPECT_FALSE(input.empty());
	auto input_img = ovx::util::create_image_from_mat(ctx_, input);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(input_img));
	cv::Mat output(input.size(), input.type());
	auto format = ovx::util::get_format(output);
	vx_image output_img = vxCreateImageFromHandle(
			ctx_,
			ovx::util::mat_type_to_image_format(output.type()),
			&format,
			(void**)&output.data,
			VX_MEMORY_TYPE_HOST);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(output_img));
	auto n = ovx::ct::back_projection_node(graph_, input_img, output_img);
	EXPECT_EQ(VX_SUCCESS, GET_STATUS(n));
	EXPECT_EQ(VX_SUCCESS, vxReleaseNode(&n));
	EXPECT_EQ(VX_SUCCESS, vxVerifyGraph(graph_));

	EXPECT_EQ(VX_SUCCESS, vxProcessGraph(graph_));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&input_img));
	EXPECT_EQ(VX_SUCCESS, vxReleaseImage(&output_img));
}
