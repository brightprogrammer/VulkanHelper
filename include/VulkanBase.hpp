/**
 * @file VulkanBase.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief vulkan base is a struct that holds basic vulkan handles to give a jumpstart to your application.
 * @version 0.1
 * @date 2021-04-19
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

#ifndef VULKAN_HELPER_VULKAN_BASE_HPP
#define VULKAN_HELPER_VULKAN_BASE_HPP

#include "VulkanInitializers.hpp"
#include "VulkanTools.hpp"
#include <Vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace Vulkan{
    namespace Tools{
        /**
        * @brief VulkanBase is a basic struct that can be used to initialize
        *        basic Vulkan handles like VkInstance, VkSurfaceKHR, VkDevice,
        *        VkSwapchainKHR and VkImageView to initialize Vulkan in your
        *        application in just a few calls. You can initialize all these
        *        things using VulkanBase and then store them in your application.
        *      
        */
        struct VulkanBase{
            /**
            * @brief Construct a new Vulkan Base object.
            * 
            */
            VulkanBase() {
                availableInstanceExtensions = Vulkan::EnumerateInstanceExtensionNames();
                availableInstanceLayers = Vulkan::EnumerateInstanceLayerNames();
            };

            /// list of all available instance extensions
            Names availableInstanceExtensions;

            /// extensions for instance creation
            Names instanceExtensions;

            /// list of all available instance layers
            Names availableInstanceLayers;

            /// layers for instance creation
            Names instanceLayers;

            /// application name
            const char* applicationName = "application";

            /// application version
            uint32 applicationVersion = VK_MAKE_VERSION(0, 0, 0);

            /// created vulkan instance handle
            VkInstance instance = VK_NULL_HANDLE;

            /// given sdl window handle, if set to nullptr then surface won't be created
            SDL_Window* window = nullptr;

            /// created vulkan surface handle i.e created for given sdl window
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            
            /// selected physical device handle
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

            /// list of all available device extensions
            Names availableDeviceExtensions;

            /// extensions for device creation
            Names deviceExtensions;

            /// graphics family index
            std::optional<uint32> graphicsIdx;

            /// presentation family index, it has value only when a surface is created
            std::optional<uint32> presentIdx; 

            /// created logical device handle
            VkDevice device = VK_NULL_HANDLE;

            /// device graphics queue handle
            VkQueue graphicsQueue;

            /// device presentation queue handle (queue that supports surface presentation)
            VkQueue presentQueue;

            /// created swapchain handle
            VkSwapchainKHR swapchain = VK_NULL_HANDLE;

            /// swapchain image extent
            VkExtent2D imageExtent;

            /// swapchain image format
            VkFormat imageFormat;

            /// created swapchain images handle
            std::vector<VkImage> images;

            /// number of images in swapchain
            uint32 numberOfImagesInSwapchain;

            /// created image views for swapchain images
            std::vector<VkImageView> imageViews;

            /**
             * @brief Enable an instance extension.
             *        If extension is already enabled in nothing will happen (true will be returned)
             *
             * @param extensionName
             * @return true if extension was found
             * @return false otherwise
             */
            inline bool EnableInstanceExtension(const char* extensionName){
                if(CheckAvailability(availableInstanceExtensions, extensionName)){
                    // add extension name only when it is not already present
                    if(CheckAvailability(instanceExtensions, extensionName) == false){
                        instanceExtensions.push_back(extensionName);
                    }
                    return true;
                }
                return false;
            }

            /**
             * @brief Enable an instance layer.
             *        If layer is already enabled in nothing will happen (true will be returned)
             *
             * @param layerName
             * @return true if layer was found
             * @return false otherwise
             */
            inline bool EnableInstanceLayer(const char* layerName){
                if(CheckAvailability(availableInstanceLayers, layerName)){
                    // add layer name only when it is not already present
                    if(CheckAvailability(instanceLayers, layerName) == false){
                        instanceExtensions.push_back(layerName);
                    }
                    return true;
                }
                return false;
            }

            /**
            * @brief Creates a Vulkan instance and stores it in VulkanBase::instance.
            *        Surface extensions are already requested if window is set to some
            *        valid pointer. If not set then user must request instances extensions 
            *        using VulkanBase::EnableInstanceExtension(extensionName) call.
            *        Similarly user can request instance layers using EnableInstanceLayer(...)
            *
            */
            inline void CreateInstance(){
                VkApplicationInfo appInfo = Vulkan::Init::ApplicationInfo(applicationName, applicationVersion);
                VkInstanceCreateInfo instanceCreateInfo = Vulkan::Init::InstanceCreateInfo(appInfo, instanceExtensions, instanceLayers);
                instance = Vulkan::CreateInstance(instanceCreateInfo);
            }

            /// create vulkan surface, if window is nullptr in constructor then surface will be VK_NULL_HANDLE
            inline void CreateSurface(SDL_Window* window){
                surface = Vulkan::CreateSurface(instance, window);
            }

            /// select vulkan capable physical device
            inline void SelectPhysicalDevice(){
                physicalDevice = Vulkan::Tools::SelectBestPhysicalDevice(instance, surface);
                availableDeviceExtensions = Vulkan::EnumerateDeviceExtensionNames(physicalDevice);
            }

            /**
             * @brief Enable a device extension.
             *
             * @warning a physical device must be selected before using this function
             *
             * @param extensionName 
             * @return true 
             * @return false 
             */
            inline bool EnableDeviceExtension(const char* extensionName){
                if(CheckAvailability(availableDeviceExtensions, extensionName)){
                    // add extension name only when it is not already present
                    if(CheckAvailability(deviceExtensions, extensionName) == false){
                        deviceExtensions.push_back(extensionName);
                    }
                    return true;
                }
                return false;
            }


            /// create a logical device
            inline void CreateDevice(){
                // queue create infos
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
                // get graphics family indices
                graphicsIdx = Vulkan::GetPhysicalDeviceQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
                ASSERT(graphicsIdx.has_value(), "NO GRAPHICS QUEUE FAMILY PRESENT ON SELECTED DEVICE");
                
                // get presentation family index
                if(surface != VK_NULL_HANDLE){
                    presentIdx  = Vulkan::GetPhysicalDeviceSurfaceSupportQueueIndex(physicalDevice, surface); 
                    ASSERT(presentIdx.has_value(), "SELECTED DEVICE DOESN'T SUPPORT SURFACE PRESENTATION");
                }

                // unique queue indices
                std::set<uint32> uniqueQueueIndices = {graphicsIdx.value(), presentIdx.value()};

                // queue priorities : since we are creating only one queue, only one value in vector
                std::vector<float> queuePriorities = {float{1.f}};
                
                // add unique queues to queue create infos vector
                for(const auto& queueIdx : uniqueQueueIndices)
                    queueCreateInfos.push_back(Vulkan::Init::DeviceQueueCreateInfo(queueIdx, queuePriorities));
                
                // device create info
                VkDeviceCreateInfo deviceCreateInfo = Vulkan::Init::DeviceCreateInfo(deviceExtensions, queueCreateInfos);

                // create device
                device = Vulkan::CreateDevice(physicalDevice, deviceCreateInfo);

                // get created device queue
                graphicsQueue = Vulkan::GetDeviceQueue(device, graphicsIdx.value(), 0);
                if(surface!=VK_NULL_HANDLE){
                    if(presentIdx != graphicsIdx){
                        presentQueue = Vulkan::GetDeviceQueue(device, presentIdx.value(), 0);
                    }else presentQueue = graphicsQueue;
                }
            }

            /**
             * @brief Create a Vulkan swapchain.
             *        This function also gets the handles of images that are in swapchain
             * 
             * @warning VulkanBase::surface must be a valid handle
             *
             */
            inline void CreateSwapchain(){
                VkSwapchainCreateInfoKHR swapchainCreateInfo = Vulkan::Init::SwapchainCreateInfo(physicalDevice, surface, window);
                imageExtent = swapchainCreateInfo.imageExtent;
                imageFormat = swapchainCreateInfo.imageFormat;
                swapchain = Vulkan::CreateSwapchain(device, swapchainCreateInfo);
                
                // get images
                images = Vulkan::GetSwapchainImages(device, swapchain);
                numberOfImagesInSwapchain = images.size();
            }

            /// create image views for swapchain images
            inline void CreateImageViews(){
                // resize image views vector
                imageViews.resize(numberOfImagesInSwapchain);
                // create image views
                for(uint i = 0; i<numberOfImagesInSwapchain; i++){
                    VkImageViewCreateInfo imageViewCreateInfo = Vulkan::Init::ImageViewCreateInfo(images[i], VK_IMAGE_ASPECT_COLOR_BIT, imageFormat);
                    imageViews[i] = Vulkan::CreateImageView(device, imageViewCreateInfo);
                }
            }

        }; // VulkanBase
    } // tools namespace
} // vulkan namespace

#endif//VULKAN_HELPER_VULKAN_BASE_HPP