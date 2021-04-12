/**
 * @file Vulkan.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief Vulkan Helper Functions. Contains Vulkan C functions wrappers that return/use C++ containers.
 * @version 0.1
 * @date 2021-04-05
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

#ifndef BHAYANKAR_VULKAN_HPP
#define BHAYANKAR_VULKAN_HPP

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <string>
#include <set>
#include <fstream>
#include <optional>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>
#include "Core.hpp"
#include "VulkanEnumStringifier.hpp"

/// Vulkan namespace contains helpers
namespace Vulkan{

    /**
     * @brief check return result of a vulkan function
     * 
     * @param res of a function
     */
    inline void CheckResult(VkResult res){
        ASSERT(res == VK_SUCCESS, "\tFAILED -> returned : %s", ResultString(res));
    }

    /**
    * @brief Checks whether a Vulkan handle is valid or not
    * 
    * @tparam VulkanHandleType type of Vulkan handle : automatically deduced
    * @param handle : const reference to Vulkan handle
    * @return true  : valid handle
    * @return false : invalid handle
    */
    template<typename VulkanHandleType>
    [[nodiscard]] inline bool CheckValidHandle(const VulkanHandleType& handle){
        return handle != VK_NULL_HANDLE;
    }

    /**
    * @brief get the list of instance extensions available on host
    * 
    * @return std::vector<const char*> 
    */
    [[nodiscard]] inline std::vector<const char*> EnumerateInstanceExtensionNames(){
        // get instance extensions
        uint32 count;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());

        //the names vector
        std::vector<const char*> extensionNames(count);

        // store them in the names vector
        for(uint32 i = 0; i<count; i++)
            extensionNames[i] = extensions[i].extensionName;

        return extensionNames;
    }

    /**
    * @brief get the list of instance layers on host
    * 
    * @return std::vector<const char*> 
    */
    [[nodiscard]] inline std::vector<const char*> EnumerateInstanceLayerNames(){    
        // get instance layers
        uint count = 0;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());

        // resize the names vector
        std::vector<const char*> layerNames(count);

        // store them in the names vector
        for(uint32 i = 0; i<count; i++)
            layerNames[i] = layers[i].layerName;

        return layerNames;
    }

    /**
    * @brief C string specialization
    * 
    * @param container list of names
    * @param object name
    * @return true found
    * @return false not found
    */
    [[nodiscard]] inline bool CheckAvailability(const Names& container, const char* object){
        // check if object is present or not
        for(const auto& obj : container){
            if(!strcmp(obj, object)){
                return true;
            }
        }

        // if not present then return false
        return false;
    }

    /**
    * @brief Check if an object is available in a list of objects
    * 
    * @tparam T type of object
    * @param container list of objects
    * @param object specifc object
    * @return true found
    * @return false not found
    */
    template<typename T>
    [[nodiscard]] inline bool CheckAvailability(const std::vector<T>& container, const T& object){
        // check if object is present or not
        for(const auto& obj : container){
            if(obj == object){
                return true;
            }
        }

        // if not present then return false
        return false;
    }

    /**
    * @brief C++ string specialization
    * 
    * @tparam empty
    * @param container vector of std::string 
    * @param object std::string name
    * @return true found
    * @return false not found
    */
    template<>
    [[nodiscard]] inline bool CheckAvailability<std::string>(const std::vector<std::string>& container, const std::string& object){
        // check if object is present or not
        for(const auto& obj : container){
            if(strcmp(obj.c_str(), object.c_str()) == 0){
                return true;
            }
        }

        // if not present then return false
        return false;
    }

    /**
    * @brief Destroy instnace
    * 
    * @param instance const reference of handle to Vulkan instance (VkInstance)
    * @param allocator
    */
    inline void DestroyInstance(const VkInstance& instance, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check for errors
        ASSERT(CheckValidHandle(instance), "\tInvalid instance given\n");

        // destroy instance
        vkDestroyInstance(instance, allocator);
    }

    /**
     * @brief create vulkan instance
     * 
     * @param instanceCreateInfo 
     * @param allocator 
     * @return VkInstance 
     */
    [[nodiscard]] inline VkInstance CreateInstance(const VkInstanceCreateInfo& instanceCreateInfo, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // instance handle
        VkInstance instance;

        // create new instance
        VkResult resInstanceCreation = vkCreateInstance(&instanceCreateInfo, allocator, &instance);
        
        // check sucess
        ASSERT( resInstanceCreation == VK_SUCCESS, 
            "\tVulkan Instance creation failed -> returned : %s\n", ResultString(resInstanceCreation));

        // print success
        printf("[CreateInstance] : Vulkan Instance created\n");

        // return instance handle
        return instance;
    }

    /**
    * @brief get physical devices present on host
    * 
    * @param instance : const referencet to Vulkan instance hanlde (VkInstance)
    * @return std::vector<VkPhysicalDevice> is list of physical devices
    */
    [[nodiscard]] inline std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(const VkInstance& instance) noexcept{
        // check for valid instance handle
        ASSERT(CheckValidHandle(instance), "\tInvalid Vulkan instnace handle\n");

        // get device count
        uint32 count;
        VkResult res = vkEnumeratePhysicalDevices(instance, &count, nullptr);
        if(res != VK_SUCCESS) printf("[EnumeratePhysicalDevices] : %s\n", ResultString(res));

        // check physical device count
        ASSERT(count > 0, "\tNo Vulkan capable Physical Devices found on host\t");

        // get devices
        std::vector<VkPhysicalDevice> devices(count);
        res = vkEnumeratePhysicalDevices(instance, &count, devices.data());
        if(res != VK_SUCCESS) printf("[EnumeratePhysicalDevices] : %s\n", ResultString(res));

        // return device list
        return devices;
    }

    /**
    * @brief get properties of given physical device
    * 
    * @param physicalDevice handle
    * @return VkPhysicalDeviceProperties 
    */
    [[nodiscard]] inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice) noexcept{
        // check for valid handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device given\n");
        
        // get and return properties
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        return properties;
    }

    /**
    * @brief get memory properties of given physical device
    * 
    * @param physicalDevice handle
    * @return VkPhysicalDeviceMemoryProperties 
    */
    [[nodiscard]] inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(const VkPhysicalDevice& physicalDevice) noexcept{
        // check for valid handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device given\n");

        // get properties
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        return memoryProperties;
    }

    /**
    * @brief get features of given physical device
    * 
    * @param physicalDevice handle
    * @return VkPhysicalDeviceFeatures 
    */
    [[nodiscard]] inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice) noexcept{
        // check for valid handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device given\n");
        
        // get and return properties
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        return features;
    }

    /**
     * @brief destroy vulkan surface
     * 
     * @param instance 
     * @param surface 
     */
    inline void DestroySurface(const VkInstance& instance, const VkSurfaceKHR& surface) noexcept{
        // check for valid instance handle
        ASSERT(CheckValidHandle(instance), "\tInvalid Vulkan instance handle given\n");

        // check for valid surface handle
        ASSERT(CheckValidHandle(surface), "\tInvalid Surface handle given\n");

        // destroy surface
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    /**
    * @brief create a Vulkan surface for given instance and window
    * 
    * @param instance const ref to valid VkInstance handle
    * @param window pointer to valid sdl valid window
    * @return VkSurfaceKHR 
    */
    [[nodiscard]] inline VkSurfaceKHR CreateSurface(const VkInstance& instance, SDL_Window* window) noexcept{
        // check for valid instance handle
        ASSERT(CheckValidHandle(instance), "\tInvalid Vulkan instance handle given\n");

        // surface handle
        VkSurfaceKHR surface;

        // create surface
        SDL_bool resSurfaceCreated = SDL_Vulkan_CreateSurface(window, instance, &surface);

        // check success
        ASSERT(resSurfaceCreated, "\tSurface creation failed\n\t\tERROR MESSAGE : %s", SDL_GetError());

        // print success
        printf("[CreateSurface] : Created Surface for Window[%s]\n", SDL_GetWindowTitle(window));

        // return surface handle
        return surface;
    }

    /**
     * @brief get device extension names
     * 
     * @param physicalDevice 
     * @return Names 
     */
    [[nodiscard]] inline Names EnumerateDeviceExtensionNames(const VkPhysicalDevice& physicalDevice){
        // check valid handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given\n");

        // get extension count
        uint32 count;
        VkResult res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
        if(res != VK_SUCCESS) printf("[EnumerateDeviceExtensionNames] : %s\n", ResultString(res));

        // get extension properties
        std::vector<VkExtensionProperties> extensions(count);
        res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data());
        if(res != VK_SUCCESS) printf("[EnumerateDeviceExtensionNames] : %s\n", ResultString(res));

        // create a vector and add the names to it
        std::vector<const char*> extensionNames(count);
        for(uint i=0; i<count; i++){
            extensionNames[i] = extensions[i].extensionName;
        }

        // return the names
        return extensionNames;
    }

    /**
    * @brief get physical device queues
    * 
    * @param physicalDevice handle
    * @return std::vector<VkQueueFamilyProperties> 
    */
    [[nodiscard]] inline std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice& physicalDevice) noexcept{
        // check for valid physical device handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given\n");
        
        // get queue family properties
        uint32 count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queues(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues.data());

        // return
        return queues;
    }

    /**
     * @brief Destroy given device
     * 
     * @param device 
     * @param allocator
     */
    inline void DestroyDevice(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check for valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle given\n");

        // destroy logical device
        vkDestroyDevice(device, allocator);
    }

    /**
    * @brief create logical device
    * 
    * @param physicalDevice 
    * @param deviceCreateInfo 
    * @param allocator
    * @return VkDevice 
    */
    [[nodiscard]] inline VkDevice CreateDevice(const VkPhysicalDevice& physicalDevice, const VkDeviceCreateInfo& deviceCreateInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check if a physical device is bound to VulkanState
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle passed");

        // logical device handle
        VkDevice device;

        // create device
        VkResult resLogicalDeviceCreation 
            = vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device);

        // check if device creation is successful
        ASSERT( resLogicalDeviceCreation == VK_SUCCESS, 
            "\tLogical Device creation failed -> returned : %s\n", ResultString(resLogicalDeviceCreation));

        // print success message
        printf("[CreateDevice] : Logical Device creation successful\n");

        return device;
    }

    /**
    * @brief get device queue after creating data
    * 
    * @param device 
    * @param queueFamilyIdx 
    * @param queueIdx 
    * @return VkQueue 
    */
    [[nodiscard]] inline VkQueue GetDeviceQueue(const VkDevice& device, const uint32& queueFamilyIdx, const uint32& queueIdx){
        // check if device is bound or not
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // get queue
        VkQueue queue;
        vkGetDeviceQueue(device, queueFamilyIdx, queueIdx, &queue);

        // return queue
        return queue;
    }

    /**
    * @brief get surface capabilities of given physical device and surface
    * 
    * @param physicalDevice 
    * @param surface 
    * @return VkSurfaceCapabilitiesKHR 
    */
    [[nodiscard]] inline VkSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) noexcept{
        // check physical device handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle passed");
        
        // check surface handle
        ASSERT(CheckValidHandle(surface), "\tInvalid Surface handle passed");
        
        // get capabilities
        VkSurfaceCapabilitiesKHR capabilities;
        VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfaceCapabilities] : %s\n", ResultString(res));

        return capabilities;
    }

    /**
    * @brief get surface present modes that the given physical device supports for the given surface
    * 
    * @param physicalDevice handle
    * @param surface handle
    * @return std::vector<VkPresentModeKHR> 
    */
    [[nodiscard]] inline std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModes(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface){
        // check physical device handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given");

        // check surface handle
        ASSERT(CheckValidHandle(surface), "\tInvalid surface handle given");

        // get mode count
        uint32 modeCount;
        VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr);
        if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfacePresentModes] : %s\n", ResultString(res));

        // get modes
        std::vector<VkPresentModeKHR> presentModes(modeCount);
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, presentModes.data());
        if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfacePresentModes] : %s\n", ResultString(res));

        return presentModes;
    }

    /**
    * @brief get physical device surface formats for given physical device and surface
    * 
    * @param physicalDevice handle
    * @param surface handle
    * @return std::vector<VkSurfaceFormatKHR> 
    */
    [[nodiscard]] inline std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormats(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface){
    // check physical device handle
        ASSERT(CheckValidHandle(physicalDevice), "\tInvalid Physical Device handle given");

        // check surface handle
        ASSERT(CheckValidHandle(surface), "\tInvalid surface handle given");

        // get format count
        uint32 formatCount;
        VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfaceFormats] : %s\n", ResultString(res));

        // get formats
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());    
        if(res != VK_SUCCESS) printf("[GetPhysicalDeviceSurfaceFormats] : %s\n", ResultString(res));

        return formats;
    }

    /**
     * @brief destroy swapchain
     * 
     * @param device 
     * @param swapchain 
     * @param allocator
     */
    inline void DestroySwapchain(const VkDevice& device, const VkSwapchainKHR& swapchain,  const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check for valid instance handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle given\n");

        // check for valid surface handle
        ASSERT(CheckValidHandle(swapchain), "\tInvalid Swapchain handle given\n");
    
        // destroy swapchain
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }

    /**
    * @brief create and get a Vulkan surface
    * 
    * @param device handle
    * @param swapchainCreateInfo
    * @param allocator
    * @return VkSwapchainKHR 
    */
    [[nodiscard]] inline VkSwapchainKHR CreateSwapchain(const VkDevice& device, const VkSwapchainCreateInfoKHR& swapchainCreateInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check for valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Physical Device handle given");
        
        // swpachain handle
        VkSwapchainKHR swapchain;

        // create swapchain
        VkResult resSwapchainCreated = 
            vkCreateSwapchainKHR(device, &swapchainCreateInfo, allocator, &swapchain);

        // check for success
        ASSERT(resSwapchainCreated == VK_SUCCESS, 
            "\tFailed to create Swapchain -> returned : %s", ResultString(resSwapchainCreated));

        // print success message
        printf("[CreateSwapchain] : Swapchain creation successful\n");

        return swapchain;
    }

    /**
     * @brief get swapchain images
     * 
     * @param device 
     * @param swapchain 
     * @return std::vector<VkImage> 
     */
    [[nodiscard]] inline std::vector<VkImage> GetSwapchainImages(const VkDevice& device, const VkSwapchainKHR& swapchain) noexcept{
        // check swapchain handle
        ASSERT(CheckValidHandle(swapchain), "\tInvalid Swapchain handle passed");

        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // get image count
        uint32 count;
        VkResult res = vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
        if(res != VK_SUCCESS) printf("[GetSwapchainImages] : %s\n", ResultString(res));
        
        // get images
        std::vector<VkImage> images(count);
        res = vkGetSwapchainImagesKHR(device, swapchain, &count, images.data());
        if(res != VK_SUCCESS) printf("[GetSwapchainImages] : %s\n", ResultString(res));

        return images;
    }

    /**
     * @brief destroy image view
     * 
     * @param device 
     * @param imageView 
     * @param allocator
     */
    inline void DestroyImageView(const VkDevice& device, const VkImageView& imageView, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check valid logical device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");
        
        // destroy image view
        vkDestroyImageView(device, imageView, allocator);
    }

    /**
     * @brief create image view for given device
     * 
     * @param device 
     * @param ivCreateInfo Image View create info
     * @param allocator
     * @return VkImageView 
     */
    [[nodiscard]] inline VkImageView CreateImageView(const VkDevice& device, const VkImageViewCreateInfo& ivCreateInfo, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check valid handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // image view
        VkImageView imageView;

        // create image view
        VkResult resImageViewCreateInfo = vkCreateImageView(device, &ivCreateInfo, allocator, &imageView);
        
        // check success
        ASSERT(resImageViewCreateInfo == VK_SUCCESS, 
            "\t[CreateImageView] : Image View creation failed -> returned %s", ResultString(resImageViewCreateInfo));

        // print success
        printf("[CreateImageView] : Image View creation successful\n");

        // return
        return imageView;
    }

    /**
     * @brief destroy command pool
     * 
     * @param device 
     * @param commandPool 
     * @param allocator 
     */
    inline void DestroyCommandPool(const VkDevice& device, const VkCommandPool& commandPool, const VkAllocationCallbacks* allocator = nullptr){
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical sDevice handle passed");

        // check command pool handle
        ASSERT(CheckValidHandle(commandPool), "\tInvalid Command Pool handle passed");
    
        // destroy
        vkDestroyCommandPool(device, commandPool, allocator);
    }

    /**
     * @brief create command pool
     * 
     * @param device 
     * @param commandPoolInfo 
     * @param allocator 
     * @return VkCommandPool 
     */
    [[nodiscard]] inline VkCommandPool CreateCommandPool(const VkDevice& device, const VkCommandPoolCreateInfo& commandPoolInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical sDevice handle passed");

        // command pool
        VkCommandPool commandPool;

        // create
        VkResult resCreateCommandPool = vkCreateCommandPool(device, &commandPoolInfo, allocator, &commandPool);
    
        // check success
        ASSERT(resCreateCommandPool == VK_SUCCESS, "\tCommand Pool creation failed -> returned %s", ResultString(resCreateCommandPool));
    
        // print success
        printf("[CreateCommandPool] : Command Pool creation successful\n");

        // return
        return commandPool;
    }

    /**
     * @brief allocate multiple command buffer(s)
     * 
     * @param device 
     * @param cmdBufAllocInfo 
     * @return std::vector<VkCommandBuffer> 
     */
    [[nodiscard]] inline std::vector<VkCommandBuffer> AllocateCommandBuffers(const VkDevice& device, const VkCommandBufferAllocateInfo& cmdBufAllocInfo){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // command buffers
        std::vector<VkCommandBuffer> commandBuffers(cmdBufAllocInfo.commandBufferCount);

        // create
        VkResult resAllocateCommandBuffers = vkAllocateCommandBuffers(device, &cmdBufAllocInfo, commandBuffers.data());
    
        // check success
        ASSERT(resAllocateCommandBuffers == VK_SUCCESS, "\tCommand Buffer allocation failed -> returned : %s", ResultString(resAllocateCommandBuffers));
    
        // print success
        printf("[AllocateCommandBuffers] : %i Command Buffers allocated successfully\n", static_cast<uint>(commandBuffers.size()));

        // return
        return commandBuffers;
    }

    /**
     * @brief destroy renderpass
     * 
     * @param device 
     * @param renderpass 
     * @param allocator 
     */
    inline void DestroyRenderPass(const VkDevice& device, const VkRenderPass& renderpass, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // check valid renderpass handle
        ASSERT(CheckValidHandle(renderpass), "\tInvalid RenderPass handle passed");
    
        // destroy
        vkDestroyRenderPass(device, renderpass, allocator);
    }

    /**
     * @brief create a renderpass
     * 
     * @param device 
     * @param createInfo 
     * @param allocator 
     * @return VkRenderPass 
     */
    [[nodiscard]] inline VkRenderPass CreateRenderPass(const VkDevice& device, const VkRenderPassCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // renderpass
        VkRenderPass renderpass;

        // create
        VkResult resCreateRenderPass = vkCreateRenderPass(device, &createInfo, allocator, &renderpass);

        // check success
        ASSERT(resCreateRenderPass == VK_SUCCESS, "\tRenderPass creation failed -> return : %s", ResultString(resCreateRenderPass));

        // print success
        printf("[CreateRenderPass] : RenderPass creation successful\n");

        // return
        return renderpass;
    }

    /**
     * @brief destroy framebuffer
     * 
     * @param device 
     * @param framebuffer 
     * @param allocator 
     */
    inline void DestroyFramebuffer(const VkDevice& device, const VkFramebuffer& framebuffer, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");
    
        // check valid framebuffer handle
        ASSERT(CheckValidHandle(framebuffer), "\tInvalid Framebuffer handle passed");

        // destroy
        vkDestroyFramebuffer(device, framebuffer, allocator);
    }

    /**
     * @brief create framebuffer
     * 
     * @param device 
     * @param framebufferCreateInfo 
     * @param allocator 
     * @return VkFramebuffer 
     */
    [[nodiscard]] inline VkFramebuffer CreateFramebuffer(const VkDevice& device, const VkFramebufferCreateInfo& framebufferCreateInfo, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // framebuffer handle
        VkFramebuffer framebuffer;

        // create
        VkResult resCreateFramebuffer = vkCreateFramebuffer(device, &framebufferCreateInfo, allocator, &framebuffer);

        // check success
        ASSERT(resCreateFramebuffer == VK_SUCCESS, "\tFramebuffer creation failed -> returned : %s", ResultString(resCreateFramebuffer));

        // print success
        printf("[CreateFramebuffer] : Framebuffer creation successful\n");

        // return
        return framebuffer;
    }

    /**
     * @brief destroy fence
     * 
     * @param device 
     * @param fence 
     * @param allocator 
     */
    inline void DestroyFence(const VkDevice& device, const VkFence& fence, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed"); 

        // check valid fence handle
        ASSERT(CheckValidHandle(fence), "\tInvalid Fence handle passed");

        // destroy 
        vkDestroyFence(device, fence, allocator);
    }

    /**
     * @brief create a fence
     * 
     * @param device 
     * @param fenceCreateInfo 
     * @param allocator 
     * @return VkFence 
     */
    [[nodiscard]] inline VkFence CreateFence(const VkDevice& device, const VkFenceCreateInfo& fenceCreateInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed"); 

        // fence
        VkFence fence;

        // create
        VkResult resCreateFence = vkCreateFence(device, &fenceCreateInfo, allocator, &fence);

        // check success
        ASSERT(resCreateFence == VK_SUCCESS, "\tFence creation failed -> returned : %s", ResultString(resCreateFence));

        // print success
        printf("[CreateFence] : Fence creation successful\n");

        // return
        return fence;
    }

    /**
     * @brief wait for multiple fence(s) at a time
     * 
     * @param device 
     * @param fences all fences
     * @param waitAll do we have to wait for all fences
     * @param timeout time in nanoseconds
     */
    inline void WaitForFences(const VkDevice& device, const std::vector<VkFence>& fences, VkBool32 waitAll, uint64 timeout){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // wait
        VkResult resWaitFences = vkWaitForFences(device, fences.size(), fences.data(), waitAll, timeout);

        // check success
        ASSERT(resWaitFences == VK_SUCCESS, "\tSomething wrong happened while waiting for Fence(s) -> returned %s", ResultString(resWaitFences));
    }

    /**
     * @brief wait for only a single fence
     * 
     * @param device 
     * @param fence 
     * @param timeout 
     */
    inline void WaitForFence(const VkDevice& device, const VkFence& fence, const uint64& timeout){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // wait
        VkResult resWaitFence = vkWaitForFences(device, 1, &fence, VK_TRUE, timeout);

        // check success
        ASSERT(resWaitFence == VK_SUCCESS, "\tSomething wrong happened while waiting for a Fence -> returned %s", ResultString(resWaitFence));
    }

    /**
     * @brief reset multiple fences(s) at once
     * 
     * @param device 
     * @param fences 
     */
    inline void ResetFences(const VkDevice& device, const std::vector<VkFence>& fences){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // reset
        VkResult resResetFences = vkResetFences(device, fences.size(), fences.data());

        // check success
        ASSERT(resResetFences == VK_SUCCESS, "\tReset Fence(s) failed -> returned : %s", ResultString(resResetFences));
    }

    /**
     * @brief reset only one fence
     * 
     * @param device 
     * @param fence
     */
    inline void ResetFence(const VkDevice& device, const VkFence& fence){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // reset
        VkResult resResetFence = vkResetFences(device, 1, &fence);

        // check success
        ASSERT(resResetFence == VK_SUCCESS, "\tReset Fence failed -> returned : %s", ResultString(resResetFence));
    }

    /**
     * @brief destroy semaphore
     * 
     * @param device 
     * @param semaphore
     * @param allocator 
     */
    inline void DestroySemaphore(const VkDevice& device, const VkSemaphore& semaphore, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed"); 

        // check valid fence handle
        ASSERT(CheckValidHandle(semaphore), "\tInvalid Semaphore handle passed");

        // destroy 
        vkDestroySemaphore(device, semaphore, allocator);
    }

    /**
     * @brief create a semaphore
     * 
     * @param device 
     * @param semaphoreCreateInfo 
     * @param allocator 
     * @return VkFence 
     */
    [[nodiscard]] inline VkSemaphore CreateSemaphore(const VkDevice& device, const VkSemaphoreCreateInfo& semaphoreCreateInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed"); 

        // fence
        VkSemaphore semaphore;

        // create
        VkResult resCreateSemaphore = vkCreateSemaphore(device, &semaphoreCreateInfo, allocator, &semaphore);

        // check success
        ASSERT(resCreateSemaphore == VK_SUCCESS, "\tSemaphore creation failed -> returned : %s", ResultString(resCreateSemaphore));

        // print success
        printf("[CreateSemaphore] : Semaphore creation successful\n");

        // return
        return semaphore;
    }

    [[nodiscard]] inline uint32 AcquireNextImage(const VkDevice& device, const VkSwapchainKHR& swapchain, const uint64& timeout, const VkSemaphore& semaphore, const VkFence& fence){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed"); 

        // check valid device handle
        ASSERT(CheckValidHandle(swapchain), "\tInvalid Swapchain handle passed"); 

        // check that semaphore and fence are not invalid at same time
        ASSERT(CheckValidHandle(semaphore) || CheckValidHandle(fence), 
            "\tBoth semaphore and fence must not be NULL handle at the same time");

        // image index
        uint32 idx;

        // get index
        VkResult resAcquireNextImage = vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &idx);

        // check success
        ASSERT(resAcquireNextImage == VK_SUCCESS, "\tFailed to acquire next image index -> returned : %s", ResultString(resAcquireNextImage));

        // return index
        return idx;
    }

    /**
     * @brief reset command buffers
     * 
     * @param cmdBuffer 
     * @param flags 
     */
    inline void ResetCommandBuffer(const VkCommandBuffer& cmdBuffer, const VkCommandBufferResetFlags& flags){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // reset
        VkResult resResetCommandBuffer = vkResetCommandBuffer(cmdBuffer, flags);

        // check success
        ASSERT(resResetCommandBuffer == VK_SUCCESS, "\tReset Command Buffer failed -> returned : %s", ResultString(resResetCommandBuffer));
    }

    /**
     * @brief begin command buffer recording
     * 
     * @param cmdBuffer 
     * @param beginInfo 
     */
    inline void BeginCommandBuffer(const VkCommandBuffer& cmdBuffer, const VkCommandBufferBeginInfo& beginInfo){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // begin
        VkResult resBeginCommandBuffer = vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        // check success
        ASSERT(resBeginCommandBuffer == VK_SUCCESS, "\tFailed to begin Command Buffer recording -> returned : %s", ResultString(resBeginCommandBuffer));
    }

    /**
     * @brief end command buffer recording
     * 
     * @param cmdBuffer 
     */
    inline void EndCommandBuffer(const VkCommandBuffer& cmdBuffer){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // begin
        VkResult resEndCommandBuffer = vkEndCommandBuffer(cmdBuffer);

        // check success
        ASSERT(resEndCommandBuffer == VK_SUCCESS, "\tFailed to end Command Buffer recording -> returned : %s", ResultString(resEndCommandBuffer));
    }

    /**
     * @brief bind vertex buffer
     * 
     * @param cmdBuffer 
     * @param firstBinding 
     * @param bindingCount 
     * @param buffers vector of buffers that needs to be bound
     * @param offsets vector of offsets for each buffer (each buffer may be a part of single large chunk of data)
     */
    inline void CmdBindVertexBuffers(const VkCommandBuffer& cmdBuffer, const uint32& firstBinding, const uint32& bindingCount, const std::vector<VkBuffer>& buffers, const std::vector<VkDeviceSize>& offsets){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // bind
        vkCmdBindVertexBuffers(cmdBuffer, firstBinding, bindingCount, buffers.data(), offsets.data());
    }

    /**
     * @brief push constants to shader stages
     * 
     * @param cmdBuffer 
     * @param pipelineLayout 
     * @param stageFlags 
     * @param offset 
     * @param size 
     * @param pValues 
     */
    inline void CmdPushConstants(const VkCommandBuffer& cmdBuffer, const VkPipelineLayout& pipelineLayout, const VkShaderStageFlags& stageFlags, const uint32_t& offset, const uint32_t& size, const void *pValues){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // push
        vkCmdPushConstants(cmdBuffer, pipelineLayout, stageFlags, offset, size, pValues);
    }

    /**
     * @brief submit to queue
     * 
     * @param queue 
     * @param submitInfos 
     * @param fence 
     */
    inline void QueueSumbit(const VkQueue& queue, const std::vector<VkSubmitInfo>& submitInfos, const VkFence& fence){
        // check valid queue handle
        ASSERT(CheckValidHandle(queue), "\tInvalid Queue handle passed");

        // submit
        VkResult resQueueSubmit = vkQueueSubmit(queue, submitInfos.size(), submitInfos.data(), fence);

        // check success
        ASSERT(resQueueSubmit == VK_SUCCESS, "\tQueue submit failed -> returned : %s", ResultString(resQueueSubmit));
    }

    /**
     * @brief begin render pass
     * 
     * @param cmdBuffer 
     * @param renderPassBeginInfo 
     * @param subpassContents 
     */
    inline void CmdBeginRenderPass(const VkCommandBuffer& cmdBuffer, const VkRenderPassBeginInfo& renderPassBeginInfo, const VkSubpassContents& subpassContents){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // begin
        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, subpassContents);
    }

    /**
     * @brief end render pass
     * 
     * @param cmdBuffer 
     */
    inline void CmdEndRenderPass(const VkCommandBuffer& cmdBuffer){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // begin
        vkCmdEndRenderPass(cmdBuffer);
    }

    /**
     * @brief bind a pipeline
     * 
     * @param cmdBuffer 
     * @param bindPoint 
     * @param pipeline 
     */
    inline void CmdBindPipeline(const VkCommandBuffer& cmdBuffer, const VkPipelineBindPoint& bindPoint, const VkPipeline& pipeline){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");
    
        // bind
        vkCmdBindPipeline(cmdBuffer, bindPoint, pipeline);
    }

    /**
     * @brief command buffer draw call
     * 
     * @param cmdBuffer 
     * @param vertexCount 
     * @param instanceCount 
     * @param firstVertex 
     * @param firstInstance 
     */
    inline void CmdDraw(const VkCommandBuffer& cmdBuffer, const uint32& vertexCount, const uint32& instanceCount, const uint32& firstVertex, const uint32 firstInstance){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // draw
        vkCmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    /**
     * @brief what for device until it becomes idle
     * 
     * @param device 
     */
    inline void DeviceWaitIdle(const VkDevice& device){
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");
    
        // wait
        VkResult resDeviceWaitIdle = vkDeviceWaitIdle(device);

        // check success
        ASSERT(resDeviceWaitIdle == VK_SUCCESS, "\tDevice waid idle failed -> returned : %s", ResultString(resDeviceWaitIdle));
    }

    /**
     * @brief end render pass
     * 
     * @param cmdBuffer 
     */
    inline void CmdBeginRenderPass(const VkCommandBuffer& cmdBuffer){
        // check valid command buffer handle
        ASSERT(CheckValidHandle(cmdBuffer), "\tInvalid Command Buffer handle passed");

        // end
        vkCmdEndRenderPass(cmdBuffer);
    }

    /**
     * @brief submit to present queue
     * 
     * @param queue 
     * @param presentInfo 
     */
    inline void QueuePresent(const VkQueue& queue, const VkPresentInfoKHR& presentInfo){
        // check valid queue handle
        ASSERT(CheckValidHandle(queue), "\tInvalid Queue handle passed");

        // present
        VkResult resQueuePresent = vkQueuePresentKHR(queue, &presentInfo);

        // check success
        ASSERT(resQueuePresent == VK_SUCCESS, "\tQueue present failed -> returned : %s", ResultString(resQueuePresent));
    }

    /**
     * @brief destroy shader module
     * 
     * @param device 
     * @param shader 
     * @param allocator
     */
    inline void DestroyShaderModule(const VkDevice& device, const VkShaderModule& shader, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check valid device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // check valid shader handle
        ASSERT(CheckValidHandle(shader), "\tInvalid Shader Module handle passed");

        // destroy
        vkDestroyShaderModule(device, shader, allocator);
    }

    /**
     * @brief Create a Shader Module object
     * 
     * @param device VkDevice handle
     * @param smCreateInfo shader module create info
     * @param allocator
     * @return VkShaderModule created shader module
     */
    [[nodiscard]] inline VkShaderModule CreateShaderModule(const VkDevice& device, const VkShaderModuleCreateInfo& smCreateInfo, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // shader module handle
        VkShaderModule shader;

        // create and check
        VkResult resCreateShaderModule = vkCreateShaderModule(device, &smCreateInfo, allocator, &shader);
        ASSERT(resCreateShaderModule == VK_SUCCESS, "\tFailed to create Shader Module -> returned : %s", ResultString(resCreateShaderModule));

        // print success
        printf("[CreateShaderModule] : Shader Module creation successful\n");

        return shader;
    }

    /**
     * @brief destroy pipeline layout
     * 
     * @param device 
     * @param layout 
     * @param allocator
     */
    inline void DestroyPipelineLayout(const VkDevice& device, const VkPipelineLayout& layout, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // check pipeline layout handle
        ASSERT(CheckValidHandle(layout), "\tInvalid Pipeline Layou handle passed");
    
        // destroy
        vkDestroyPipelineLayout(device, layout, allocator);
    }

    /**
     * @brief create pipeline layouts
     * 
     * @param device 
     * @param pipelineLayoutInfo 
     * @param allocator 
     * @return VkPipelineLayout 
     */
    [[nodiscard]] inline VkPipelineLayout CreatePipelineLayout(const VkDevice& device, const VkPipelineLayoutCreateInfo& pipelineLayoutInfo, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // pipeline layout
        VkPipelineLayout layout;

        // create
        VkResult resCreatePipelineLayout = vkCreatePipelineLayout(device, &pipelineLayoutInfo, allocator, &layout);

        // check success
        ASSERT(resCreatePipelineLayout == VK_SUCCESS, "\tPipeline Layout creation failed -> returned %s", ResultString(resCreatePipelineLayout));

        // print success
        printf("[CreatePipelineLayout] : Pipeline Layout creation successful\n");

        // return 
        return layout;
    }

    /**
     * @brief destroy a pipeline
     * 
     * @param device 
     * @param pipeline 
     * @param allocator 
     */
    inline void DestroyPipeline(const VkDevice& device, const VkPipeline& pipeline, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // check pipeline handle
        ASSERT(CheckValidHandle(pipeline), "\tInvalid Pipeline handle passed");

        // destroy
        vkDestroyPipeline(device, pipeline, allocator);
    }

    /**
     * @brief create multiple graphics pipeline(s)
     * 
     * @param device 
     * @param pipelineCache 
     * @param createInfos 
     * @param allocator
     * @return std::vector<VkPipeline> 
     */
    [[nodiscard]] inline std::vector<VkPipeline> CreateGraphicsPipelines(const VkDevice& device, const VkPipelineCache& pipelineCache, const std::vector<VkGraphicsPipelineCreateInfo>& createInfos, const VkAllocationCallbacks* allocator = nullptr){
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // pipelines vector with same size as that of create infos
        std::vector<VkPipeline> pipelines(createInfos.size());

        // create
        VkResult resCreateGraphicsPipelines = vkCreateGraphicsPipelines(device, pipelineCache, createInfos.size(), createInfos.data(), allocator, pipelines.data());
        
        // check success
        ASSERT(resCreateGraphicsPipelines == VK_SUCCESS, "\tFailed to create graphics pipeline -> returned %s", ResultString(resCreateGraphicsPipelines));

        // print success
        printf("[CreateGraphicsPipelines] : %i Graphics Pipeline(s) created successfully\n", static_cast<uint>(pipelines.size()));

        // return
        return pipelines;
    }

    /**
     * @brief create multiple graphics pipelines
     * 
     * @param device 
     * @param pipelineCache 
     * @param createInfos 
     * @param allocator
     * @return std::vector<VkPipeline> 
     */
    [[nodiscard]] inline VkPipeline CreateGraphicsPipeline(const VkDevice& device, const VkPipelineCache& pipelineCache, const VkGraphicsPipelineCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr){
        // check device handle
        ASSERT(CheckValidHandle(device), "\tInvalid Logical Device handle passed");

        // pipelines vector with same size as that of create infos
        VkPipeline pipeline;

        // create
        VkResult resCreateGraphicsPipeline = vkCreateGraphicsPipelines(device, pipelineCache, 1, &createInfo, allocator, &pipeline);
        
        // check success
        ASSERT(resCreateGraphicsPipeline == VK_SUCCESS, "\tFailed to create graphics pipeline -> returned %s", ResultString(resCreateGraphicsPipeline));

        // print success
        printf("[CreateGraphicsPipeline] : Graphics Pipeline created successfully\n");

        // return
        return pipeline;
    }

} // namespace Vulkan


#endif//BHAYANKAR_VULKAN_HPP