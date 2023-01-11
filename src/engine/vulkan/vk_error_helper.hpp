#ifndef __VULKAN_ENGINE_ERROR_HELPER__
#define __VULKAN_ENGINE_ERROR_HELPER__

#include "vulkan/vulkan.h"
#include <string>

namespace TutoVulkan
{
namespace Engine
{
    class VkErrorHelper final
    {
    public:
        static std::string stringify(const VkResult p_vkerror)
        {
            switch (p_vkerror)
            {
            case 0:
                return "VK_SUCCESS";
            case 1:
                return "VK_NOT_READY";
            case 2:
                return "VK_TIMEOUT";
            case 3:
                return "VK_EVENT_SET";
            case 4:
                return "VK_EVENT_RESET";
            case 5:
                return "VK_INCOMPLETE";
            case -1:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case -2:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case -3:
                return "VK_ERROR_INITIALIZATION_FAILED";
            case -4:
                return "VK_ERROR_DEVICE_LOST";
            case -5:
                return "VK_ERROR_MEMORY_MAP_FAILED";
            case -6:
                return "VK_ERROR_LAYER_NOT_PRESENT";
            case -7:
                return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case -8:
                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case -9:
                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case -10:
                return "VK_ERROR_TOO_MANY_OBJECTS";
            case -11:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case -12:
                return "VK_ERROR_FRAGMENTED_POOL";
            case -13:
                return "VK_ERROR_UNKNOWN";
            case -1000069000:
                return "VK_ERROR_OUT_OF_POOL_MEMORY"; // Provided by VK_VERSION_1_1
            case -1000072003:
                return "VK_ERROR_INVALID_EXTERNAL_HANDLE"; // Provided by VK_VERSION_1_1
            case -1000161000:
                return "VK_ERROR_FRAGMENTATION"; // Provided by VK_VERSION_1_2
            case -1000257000:
                return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"; // Provided by VK_VERSION_1_2
            case 1000297000:
                return "VK_PIPELINE_COMPILE_REQUIRED"; // Provided by VK_VERSION_1_3
            case -1000000000:
                return "VK_ERROR_SURFACE_LOST_KHR"; // Provided by VK_KHR_surface
            case -1000000001:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; // Provided by VK_KHR_surface
            case 1000001003:
                return "VK_SUBOPTIMAL_KHR"; // Provided by VK_KHR_swapchain
            case -1000001004:
                return "VK_ERROR_OUT_OF_DATE_KHR"; // Provided by VK_KHR_swapchain
            case -1000003001:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; // Provided by VK_KHR_display_swapchain
            case -1000011001:
                return "VK_ERROR_VALIDATION_FAILED_EXT"; // Provided by VK_EXT_debug_report
            case -1000012000:
                return "VK_ERROR_INVALID_SHADER_NV"; // Provided by VK_NV_glsl_shader
            case -1000158000:
                return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; // Provided by VK_EXT_image_drm_format_modifier
            case -1000174001:
                return "VK_ERROR_NOT_PERMITTED_KHR"; // Provided by VK_KHR_global_priority
            case -1000255000:
                return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; // Provided by VK_EXT_full_screen_exclusive
            case 1000268000:
                return "VK_THREAD_IDLE_KHR"; // Provided by VK_KHR_deferred_host_operations
            case 1000268001:
                return "VK_THREAD_DONE_KHR"; // Provided by VK_KHR_deferred_host_operations
            case 1000268002:
                return "VK_OPERATION_DEFERRED_KHR"; // Provided by VK_KHR_deferred_host_operations
            case 1000268003:
                return "VK_OPERATION_NOT_DEFERRED_KHR"; // Provided by VK_KHR_deferred_host_operations
            default:
                return "ERROR_UNKNOWN";
            }
        }
    };
} // namespace Engine
} // namespace TutoVulkan
#endif