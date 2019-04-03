#pragma once
#include <spdlog/spdlog.h>
#include <VX/vx.h>

namespace ovx {
namespace util {

// Convert given status to a string for loggint	
const char* status_to_str(vx_status e);

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
					, status_to_str(__local_status) \
					, __FILE__ \
					, __LINE__); \
			std::exit(1); \
		} \
  }

#define CHECK_VX_OBJECT(obj) \
  { \
		vx_status __local_status = vxGetStatus((vx_reference)obj); \
		if(__local_status != VX_SUCCESS) \
		{ \
			spdlog::error("Got invalid VX object at {}" \
					" file: {}" \
					" line: {}" \
					, status_to_str(__local_status) \
					, __FILE__ \
					, __LINE__); \
			std::exit(1); \
		} \
  }

} // nameapce util
} // namespace ovx