/**
 * @file VulkanEnumStringifier.hpp
 * @author Siddharth Mishra (bshock665@gmail)
 * @brief 
 * @version 0.1
 * @date 2021-04-15
 * 
 */

 /**
  * @copyright Copyright 2021 Siddharth Mishra
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  * 
  *     http://www.apache.org/licenses/LICENSE-2.0
  * 
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  * 
  */

#ifndef VULKAN_HELPER_VULKAN_ENUM_STRINGIFIER_HPP
#define VULKAN_HELPER_VULKAN_ENUM_STRINGIFIER_HPP

#include <vulkan/vulkan.h>

namespace Vulkan{
    inline const char* ResultString(const VkResult& res){
        switch(res){
            case 0 : return "VK_SUCCESS";
            case 1 : return "VK_NOT_READY";
            case 2 : return "VK_TIMEOUT";
            case 3 : return "VK_EVENT_SET";
            case 4 : return "VK_EVENT_RESET";
            case 5 : return "VK_INCOMPLETE";
            case -1 : return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case -2 : return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case -3 : return "VK_ERROR_INITIALIZATION_FAILED";
            case -4 : return "VK_ERROR_DEVICE_LOST";
            case -5 : return "VK_ERROR_MEMORY_MAP_FAILED";
            case -6 : return "VK_ERROR_LAYER_NOT_PRESENT";
            case -7 : return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case -8 : return "VK_ERROR_FEATURE_NOT_PRESENT";
            case -9 : return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case -10 : return "VK_ERROR_TOO_MANY_OBJECTS";
            case -11 : return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case -12 : return "VK_ERROR_FRAGMENTED_POOL";
            case -13 : return "VK_ERROR_UNKNOWN";
            case -1000069000 : return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case -1000072003 : return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case -1000161000 : return "VK_ERROR_FRAGMENTATION";
            case -1000257000 : return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
            case -1000000000 : return "VK_ERROR_SURFACE_LOST_KHR";
            case -1000000001 : return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case 1000001003 : return "VK_SUBOPTIMAL_KHR";
            case -1000001004 : return "VK_ERROR_OUT_OF_DATE_KHR";
            case -1000003001 : return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case -1000011001 : return "VK_ERROR_VALIDATION_FAILED_EXT";
            case -1000012000 : return "VK_ERROR_INVALID_SHADER_NV";
            case -1000158000 : return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
            case -1000174001 : return "VK_ERROR_NOT_PERMITTED_EXT";
            case -1000255000 : return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
            case 1000268000 : return "VK_THREAD_IDLE_KHR";
            case 1000268001 : return "VK_THREAD_DONE_KHR";
            case 1000268002 : return "VK_OPERATION_DEFERRED_KHR";
            case 1000268003 : return "VK_OPERATION_NOT_DEFERRED_KHR";
            case 1000297000 : return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
            case 0x7FFFFFFF : return "VK_RESULT_MAX_ENUM";
            default : return "ERROR : Unknown VkResult value passed";
        }
    }
}

#endif//VULKAN_ENUM_STRINGIFIER_HPP