#pragma once
#include <spdlog/spdlog.h>
#include <VX/vx.h>

namespace vxutil {
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

}