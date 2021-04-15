/**
 * @file VulkanTools.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
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

#ifndef VULKAN_HELPER_VULKAN_TOOLS_HPP
#define VULKAN_HELPER_VULKAN_TOOLS_HPP

#include "Core.hpp"
#include <vulkan/vulkan.h>
#include <fstream>
#include <SDL2/SDL.h>
#include "Vulkan.hpp"

// vulkan namespace
namespace Vulkan{
    
    // contains functions that help in selection, reading etc
    namespace Tools{

        /**
        * @brief get the physical device that best meets the requirements of vulkan renderer
        * 
        * @param instance
        * @param surface : recommended to pass surface handle for device selection
        * @return VkPhysicalDevice : selected physical device
        */
        [[nodiscard]] inline VkPhysicalDevice SelectBestPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface = VK_NULL_HANDLE){
            // check if a physical device has been bound to the VulkanState or not
            CHECK_VULKAN_HANDLE(instance)

            // device list
            auto physicalDeviceList = EnumeratePhysicalDevices(instance);
            
            // start with score = 0
            // if we find a score greater than this score then
            // select that as score and that as selected physical devie
            uint32 score = 0;

            // selected physical device
            VkPhysicalDevice selectedPhysicalDevice;

            for(const auto& physicalDevice : physicalDeviceList){
                uint32 newScore = RatePhysicalDevice(physicalDevice, surface);
                if(score < newScore){
                    score = newScore;
                    selectedPhysicalDevice = physicalDevice;
                }
            }

            // check for score
            ASSERT(score != 0, "No suitable Physical Device found on host\n");

            // print success
            VkPhysicalDeviceProperties properties = GetPhysicalDeviceProperties(selectedPhysicalDevice);
            printf("[SelectBestPhysicalDevice] : Selected Physical Device [%s]\n", properties.deviceName);

            return selectedPhysicalDevice;
        }

        /**
        * @brief Select the best present mode out of given present modes.
        *        Selects MAILBOX (if available) else returns FIFO.
        *        FIFO is guaranteed to be present.
        * 
        * @param presentModes : const reference std::vector<VkPresentModeKHR>
        * @return VkPresentModeKHR 
        */
        [[nodiscard]] inline VkPresentModeKHR SelectSwapchainSurfacePresentMode(const std::vector<VkPresentModeKHR>& presentModes){
            // check if mailbox is available : no tearing
            for(const auto& mode : presentModes)
                if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return mode;

            // by default is guaranteed to be available
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        /**
        * @brief Select the best surface format from given surface formats.
        *        Selects 8 bit B8G8R8A8 SRGB format and SRGB non linear colorspace
        *        if possible, else returns the first one available.
        * 
        * @param formats : const reference std::vector<VkSurfaceFormatKHR>
        * @return VkSurfaceFormatKHR 
        */
        [[nodiscard]] inline VkSurfaceFormatKHR SelectSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats){
            for(const auto& format : formats){
                if(format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                        return format;
                    }
            }
            // if we don't find a desired surface format then return the first one
            return formats[0];
        }

        /**
        * @brief Get the swapchain surface image extent.
        * 
        * @param width of window that contains the surface
        * @param height of window that contains the surface
        * @param capabilities is surface capabilities of surface
        * @return VkExtent2D containing extent of image
        */
        [[nodiscard]] inline VkExtent2D SelectSwapchainSurfaceImageExtent(SDL_Window* window, const VkSurfaceCapabilitiesKHR& capabilities){
            // in displays with low dpi
            if(capabilities.currentExtent.width != UINT32_MAX){
                printf("[SelectSwapchainSurfaceImageExtent] : w = %i | h = %i\n", capabilities.currentExtent.width, capabilities.currentExtent.height);
                return capabilities.currentExtent;
            }

            // in displays with high dpi
            int w, h;
            SDL_GetWindowSize(window, &w, &h);

            // extent
            VkExtent2D extent{static_cast<uint32>(w), static_cast<uint32>(h)};

            // clamp extent width and height
            extent.width = std::max(capabilities.minImageExtent.width, 
                std::min(capabilities.maxImageExtent.width, extent.width));
            extent.height = std::max(capabilities.minImageExtent.height, 
                std::min(capabilities.maxImageExtent.height, extent.height));

            printf("[SelectSwapchainSurfaceImageExtent] : w = %i | h = %i\n", extent.width, extent.height);

            return extent;
        }

        /**
        * @brief Get the shader spirv code
        * 
        * @param filename of file containing spirv code
        * @return std::vector<uint32> binary code
        */
        [[nodiscard]] inline static std::vector<char> LoadShaderCode(const char* filename){
            // open file in binary mode from the end
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            // check if file is open or not
            ASSERT(file.is_open(), "\tUnable to open shader code file %s", filename);

            // code size
            std::size_t codeSize = static_cast<std::size_t>(file.tellg());

            // code 
            std::vector<char> shaderCode(codeSize);

            // put file cursor at beginning
            file.seekg(0);

            // read file data
            file.read(shaderCode.data(), codeSize);

            // close file
            file.close();

            // return
            return shaderCode;
        }
    
    } // tools namespace

} // vulkan namespace

#endif//VULKAN_TOOLS_HPP