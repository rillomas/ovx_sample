#include <gtest/gtest.h>
#include <VX/vx.h>
#include "ct.hpp"

static void VX_CALLBACK log_callback(
	vx_context ctx,
	vx_reference ref,
	vx_status status,
	const vx_char* string) {
	printf("LOG[status: %d] %s\n", status, string);
}

class OVXTestFixture : public ::testing::Test {
protected:
	virtual void SetUp() {
		ctx_ = vxCreateContext();
		ASSERT_EQ(VX_SUCCESS, vxGetStatus((vx_reference)ctx_));
		vxRegisterLogCallback(ctx_, log_callback, vx_false_e);
		ASSERT_EQ(VX_SUCCESS, ovx::ct::register_user_kernel(ctx_));
	}

	virtual void TearDown() {
		vxReleaseContext(&ctx_);
	}

	vx_context ctx_;
};

TEST_F(OVXTestFixture, ValidateBackProjection) {

}
