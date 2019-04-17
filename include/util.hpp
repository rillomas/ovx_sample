#pragma once
#include <VX/vx.h>

namespace cv {
class Mat;
} // namespace cv

namespace ovx {
namespace util {

// Macro to suppress unused parameter warnings
#define UNUSED(x) (void)(x)

// Convert given status to a string for loggint	
const char* status_to_str(vx_status e);

#define LOG_INFO(...) printf(__VA_ARGS__)

#define LOG_VX(ctx, stat, ...) \
	vxAddLogEntry( \
		(vx_reference)ctx, \
		stat, \
		__VA_ARGS__)


// Check command to VX_SUCCESS and if it is not, log error
#define CHECK_VX_STATUS(ctx, cmd) \
  { \
		vx_status stat = cmd; \
		if(stat != VX_SUCCESS) { \
			vxAddLogEntry( \
				(vx_reference)ctx, \
				stat, \
				"VX API call failed: %s file: %s line: %d\n", \
				ovx::util::status_to_str(stat), \
				__FILE__, \
				__LINE__); \
		} \
  }

#define CHECK_VX_OBJECT(ctx, obj) \
  { \
		vx_status stat = vxGetStatus((vx_reference)obj); \
		if(stat != VX_SUCCESS) { \
			vxAddLogEntry( \
				(vx_reference)ctx, \
				stat, \
				"Got invalid VX object: %s file: %s line: %d\n", \
				ovx::util::status_to_str(stat), \
				__FILE__, \
				__LINE__); \
		} \
  }

vx_df_image mat_type_to_image_format(vx_context ctx, int mat_type);
vx_imagepatch_addressing_t get_format(const cv::Mat& mat);
vx_image create_image_from_mat(
		vx_context ctx,
		const cv::Mat& mat);

} // nameapce util
} // namespace ovx
