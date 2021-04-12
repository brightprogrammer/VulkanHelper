/**
 * @file VulkanTools.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief Contains functions that help in selecting Vulkan "things" or act as tools.
 * @version 0.1
 * @date 2021-04-08
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


#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP

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
        * @brief get the list of surface extension names that are available on
        *        host platform
        * 
        * @return Names : vector of const char* containing surfaces extension names
        */
        [[nodiscard]] inline Names GetSurfaceExtensions(){
            // names of extensions that will be returned
            Names extensions;
            // names of all extensions
            Names availableExtensions = EnumerateInstanceExtensionNames();
            // names of platform specifc surface extensions
            Names surfaceExtensions{
                "VK_KHR_surface"        ,        
                "VK_KHR_xcb_surface"    ,
                "VK_KHR_xlib_surface"   ,
                "VK_KHR_wayland_surface",
                "VK_KHR_win32_surface"  ,
                "VK_MVK_ios_surface"    ,
                "VK_MVK_macos_surface"  ,
                "VK_EXT_metal_surface"  ,
                "VK_KHR_android_surface"
            };
            
            // this will enable all surface extensions that are available
            for(const auto extension : surfaceExtensions){
                if(CheckAvailability(availableExtensions, extension))
                    extensions.push_back(extension);
            }

            // return names of available surface extensions
            return extensions;
        }

        /**
        * @brief the default physical device rating system
        * 
        * @param physicalDevice handle
        * @param surface optional surface handle. given surface improves device selection.
        *        on some platforms future calls on selected physical device may fail if
        *        if doesn't support surface presentation. It is highly recommended to pass
        *        a valid surface handle.
        * @return uint32 :score of this physical device
        */
        [[nodiscard]] inline uint32 RatePhysicalDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface = VK_NULL_HANDLE){
            // check physical device
            ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle passed");
            
            // device score
            uint32 score = 0;

            // get physical device information
            VkPhysicalDeviceProperties properties = GetPhysicalDeviceProperties(physicalDevice);
            VkPhysicalDeviceMemoryProperties memoryProperties = GetPhysicalDeviceMemoryProperties(physicalDevice);
            VkPhysicalDeviceFeatures features = GetPhysicalDeviceFeatures(physicalDevice);

            score += properties.limits.maxColorAttachments * 100;
            score += properties.limits.maxDescriptorSetInputAttachments * 100;
            score += properties.limits.maxImageDimension2D * 1000;
            score += properties.limits.maxImageArrayLayers * 10;
            score += properties.limits.maxViewports * 500;

            score += memoryProperties.memoryHeapCount * 1000;

            if(features.multiViewport == VK_TRUE)
                score += 500;

            // get queue properties
            std::vector<VkQueueFamilyProperties> queues = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
            
            // if device doesn't provide graphics queue then set score to 0
            for(const auto& queue : queues){
                // since graphics is most important queue
                if(queue.queueFlags & VK_QUEUE_GRAPHICS_BIT){
                    score += 100000; // 1L or 100 Th
                }

                // since compute is less important that graphics
                if(queue.queueFlags & VK_QUEUE_COMPUTE_BIT){
                    score += 50000; // 50 Th
                }
            }

            // rate physical device based on surface currently bound to VulkanState<id>
            // if surface is bound to VulkanState before device selection then that means
            // user wants to show images onto a window
            // for this we check if device provides a presentation queue for the given surface
            if(surface != VK_NULL_HANDLE){
                // if surface is bound to the given VulkanState
                // then check if device supports surface presentation or not
                uint i = 0;
                VkBool32 presentationSupported;
                for(const auto& queue : queues){
                    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupported);
                    if(presentationSupported) break; else i++;
                }

                // it may happen that we reached the end and we never got the queue
                // in that case we have to check it differently
                if(i+1 == queues.size() && !presentationSupported) score = 0;
                else score += 110000;
            }else{
                printf("WARNING : IT IS RECOMMENDED TO CREATE A SURFACE (if needed) BEFORE DEVICE SELECTION FOR BETTER DEVICE SELECTION\n");
            }

            // get device extension names
            Names extensions = EnumerateDeviceExtensionNames(physicalDevice);

            // check if swapchain extension is present or not
            bool swapchainExtensionAvailable = false;
            for(const auto& extension : extensions){
                if(strcmp(extension, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
                    swapchainExtensionAvailable = true;
            }

            // no swapchain means no multi-image rendering
            if(!swapchainExtensionAvailable)
                score = 0;

            // if surface handle is given
            // then check for availablity of present modes and formats
            if(surface != VK_NULL_HANDLE){
                // get surfacepresent modes
                uint32 count = 0;
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
                if(count == 0) score = 0; // set score 0 if no present modes are available

                // get surface formts
                count = 0;
                VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
                if(count == 0) score = 0;
            }

            return score;
        }

        /**
        * @brief get the physical device that best meets the requirements of vulkan renderer
        * 
        * @param instance
        * @param surface : recommended to pass surface handle for device selection
        * @return VkPhysicalDevice : selected physical device
        */
        [[nodiscard]] inline VkPhysicalDevice SelectBestPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface = VK_NULL_HANDLE){
            // check if a physical device has been bound to the VulkanState or not
            ASSERT(CheckValidHandle(instance),  "\t");

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

        /**
        * @brief get queue index of given queue family from list of queue families
        * 
        * @param queueFamilyProperties 
        * @param flag 
        * @return signed integer ( less than 0 means queue family not found )
        */
        [[nodiscard]] inline int GetPhysicalDeviceQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties, const VkQueueFlagBits& flag){
            // check for queue index
            int queueIdx = -1;
            for(uint32 i = 0; i<queueFamilyProperties.size();  i++){
                if(queueFamilyProperties[i].queueFlags & flag) queueIdx = i;
            }

            // return queue index
            return queueIdx;
        }

        /**
        * @brief get queue index of given queue family from list of queue families.
        *        use the other function if you already have queue properties enumerated.
        * 
        * @param queueFamilyProperties 
        * @param flag 
        * @return signed integer ( less than 0 means family not found )
        */
        [[nodiscard]] inline int GetPhysicalDeviceQueueFamilyIndex(const VkPhysicalDevice& physicalDevice, const VkQueueFlagBits& flag){
            // check valid physical device handle
            ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given");

            // get queue family properties
            std::vector<VkQueueFamilyProperties> queueFamilyProperties = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
            
            // check for queue index
            int queueIdx = -1;
            for(uint32 i = 0; i<queueFamilyProperties.size();  i++){
                if(queueFamilyProperties[i].queueFlags & flag) queueIdx = i;
            }

            // return queue index
            return queueIdx;
        }

        /**
        * @brief get queue index of queue family that supports surface presentation
        * 
        * @param physicalDevice handle
        * @param surface handle
        * @return signed integer ( less than 0 means family not found )
        */
        [[nodiscard]] inline int GetPhysicalDeviceSurfaceSupportQueueIndex(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface){
            // check valid physical device handle
            ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given");

            // check valid surface handle
            ASSERT(CheckValidHandle(surface), "\tInvalid Surface handle passed");

            // get physical device queues
            std::vector<VkQueueFamilyProperties> queues = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);

            // presentation queue index
            int presentationQueueIdx = -1;

            // check which queue supports presentation
            for(uint i=0; i<queues.size(); i++){
                VkBool32 presentationSupported;
                VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice , i, surface, &presentationSupported);
                if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfaceSupportQueueIndex] : %s\n", ResultString(res));
                if(presentationSupported){
                    presentationQueueIdx = i;
                    break;
                }
            }

            return presentationQueueIdx;
        }

    
    } // tools namespace

} // vulkan namespace

#endif//VULKAN_TOOLS_HPP