/**
 * @file Vulkan.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief Vulkan Helper Functions. Contains Vulkan C functions wrappers that return/use C++ containers.
 * @version 0.1
 * @date 2021-04-05
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


#ifndef VULKAN_HELPER_VULKAN_HPP
#define VULKAN_HELPER_VULKAN_HPP

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
        ASSERT(res == VK_SUCCESS, "FAILED -> returned : %s", ResultString(res));
    }

    // /**
    // * @brief Checks whether a Vulkan handle is valid or not
    // * 
    // * @tparam VulkanHandleType type of Vulkan handle : automatically deduced
    // * @param handle : const reference to Vulkan handle
    // * @return true  : valid handle
    // * @return false : invalid handle
    // */
    // template<typename VulkanHandleType>
    // [[nodiscard]] inline bool CheckValidHandle(const VulkanHandleType& handle){
    //     return handle != VK_NULL_HANDLE;
    // }

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
    * @brief Destroy instnace
    * 
    * @param instance const reference of handle to Vulkan instance (VkInstance)
    * @param allocator
    */
    inline void DestroyInstance(const VkInstance& instance, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check for errors
        CHECK_VULKAN_HANDLE(instance);

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
            "Vulkan Instance creation failed -> returned : %s", ResultString(resInstanceCreation));

        // print success
        LOG(success, "[CreateInstance] : Vulkan Instance created");

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
        CHECK_VULKAN_HANDLE(instance)

        // get device count
        uint32 count;
        VkResult res = vkEnumeratePhysicalDevices(instance, &count, nullptr);
        if(res != VK_SUCCESS) LOG(error, "[EnumeratePhysicalDevices] : %s", ResultString(res));

        // check physical device count
        ASSERT(count > 0, "No Vulkan capable Physical Devices found on host");

        // get devices
        std::vector<VkPhysicalDevice> devices(count);
        res = vkEnumeratePhysicalDevices(instance, &count, devices.data());
        if(res != VK_SUCCESS) LOG(error, "[EnumeratePhysicalDevices] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
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
        CHECK_VULKAN_HANDLE(physicalDevice)

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
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
        CHECK_VULKAN_HANDLE(instance)

        // check for valid surface handle
        CHECK_VULKAN_HANDLE(surface)

        // destroy surface
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    /**
    * @brief create a Vulkan surface for given instance and window
    * 
    * @param instance const ref to valid VkInstance handle
    * @param window ref to valid window
    * @return VkSurfaceKHR 
    */
    [[nodiscard]] inline VkSurfaceKHR CreateSurface(const VkInstance& instance, SDL_Window* window) noexcept{
        // check for valid instance handle
        CHECK_VULKAN_HANDLE(instance)

        // check for valid window pointer
        ASSERT(window != nullptr, "[CreateSurface] : Invalid SDL_Window pointer passed");

        // surface handle
        VkSurfaceKHR surface;

        // create surface
        SDL_bool resSurfaceCreated = SDL_Vulkan_CreateSurface(window, instance, &surface);

        // check success
        ASSERT(resSurfaceCreated, "Surface creation failed\n\t\tERROR MESSAGE : %s", SDL_GetError());

        // print success
        LOG(success, "[CreateSurface] : Created Surface for Window[%s]", SDL_GetWindowTitle(window));

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
        // get extension count
        uint32 count;
        VkResult res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
        if(res != VK_SUCCESS) LOG(error, "[EnumerateDeviceExtensionNames] : %s", ResultString(res));

        // get extension properties
        std::vector<VkExtensionProperties> extensions(count);
        res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data());
        if(res != VK_SUCCESS) LOG(error, "[EnumerateDeviceExtensionNames] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
        // get queue family properties
        uint32 count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queues(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queues.data());

        // return
        return queues;
    }

    /**
    * @brief get queue index of given queue family from list of queue families
    * 
    * @param queueFamilyProperties 
    * @param flag 
    * @return std::optional<uint32> 
    */
    [[nodiscard]] inline std::optional<uint32> GetPhysicalDeviceQueueFamilyIndex(const std::vector<VkQueueFamilyProperties>& queueFamilyProperties, const VkQueueFlagBits& flag){
        // check for queue index
        std::optional<uint32> queueIdx;
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
    * @return std::optional<uint32> 
    */
    [[nodiscard]] inline std::optional<uint32> GetPhysicalDeviceQueueFamilyIndex(const VkPhysicalDevice& physicalDevice, const VkQueueFlagBits& flag){
        // check valid physical device handle
        CHECK_VULKAN_HANDLE(physicalDevice)

        // get queue family properties
        std::vector<VkQueueFamilyProperties> queueFamilyProperties = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);
        
        // check for queue index
        std::optional<uint32> queueIdx;
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
    * @return std::optional<uint32> : optional value meaning, the queue may or may not be present
    *         so be sure to check 
    */
    [[nodiscard]] inline std::optional<uint32> GetPhysicalDeviceSurfaceSupportQueueIndex(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface){
        // check valid physical device handle
        CHECK_VULKAN_HANDLE(physicalDevice)

        // check valid surface handle
        CHECK_VULKAN_HANDLE(surface)

        // get physical device queues
        std::vector<VkQueueFamilyProperties> queues = GetPhysicalDeviceQueueFamilyProperties(physicalDevice);

        // presentation queue index
        std::optional<uint32> presentationQueueIdx;

        // check which queue supports presentation
        for(uint i=0; i<queues.size(); i++){
            VkBool32 presentationSupported;
            VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice , i, surface, &presentationSupported);
            if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfaceSupportQueueIndex] : %s", ResultString(res));
            if(presentationSupported){
                presentationQueueIdx = i;
                break;
            }
        }

        return presentationQueueIdx;
    }

    /**
     * @brief Destroy given device
     * 
     * @param device 
     * @param allocator
     */
    inline void DestroyDevice(const VkDevice& device, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check for valid device handle
        CHECK_VULKAN_HANDLE(device)

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
        CHECK_VULKAN_HANDLE(physicalDevice)

        // logical device handle
        VkDevice device;

        // create device
        VkResult resLogicalDeviceCreation 
            = vkCreateDevice(physicalDevice, &deviceCreateInfo, allocator, &device);

        // check if device creation is successful
        ASSERT( resLogicalDeviceCreation == VK_SUCCESS, 
            "Logical Device creation failed -> returned : %s", ResultString(resLogicalDeviceCreation));

        // print success message
        LOG(success, "[CreateDevice] : Logical Device creation successful");

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
        CHECK_VULKAN_HANDLE(device)

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
        // check surface handle
        CHECK_VULKAN_HANDLE(surface)
        
        // get capabilities
        VkSurfaceCapabilitiesKHR capabilities;
        VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfaceCapabilities] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(physicalDevice)

        // check surface handle
        CHECK_VULKAN_HANDLE(surface)

        // get mode count
        uint32 modeCount;
        VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr);
        if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfacePresentModes] : %s", ResultString(res));

        // get modes
        std::vector<VkPresentModeKHR> presentModes(modeCount);
        res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, presentModes.data());
        if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfacePresentModes] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(physicalDevice)
        
        // check surface handle
        CHECK_VULKAN_HANDLE(surface)

        // get format count
        uint32 formatCount;
        VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfaceFormats] : %s", ResultString(res));

        // get formats
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());    
        if(res != VK_SUCCESS) LOG(error, "[GetPhysicalDeviceSurfaceFormats] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(device)

        // check for valid swapchain handle
        CHECK_VULKAN_HANDLE(swapchain)
    
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
        CHECK_VULKAN_HANDLE(device)
        
        // swpachain handle
        VkSwapchainKHR swapchain;

        // create swapchain
        VkResult resSwapchainCreated = 
            vkCreateSwapchainKHR(device, &swapchainCreateInfo, allocator, &swapchain);

        // check for success
        ASSERT(resSwapchainCreated == VK_SUCCESS, 
            "Failed to create Swapchain -> returned : %s", ResultString(resSwapchainCreated));

        // print success message
        LOG(success, "[CreateSwapchain] : Swapchain creation successful");

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
        CHECK_VULKAN_HANDLE(swapchain)

        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // get image count
        uint32 count;
        VkResult res = vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
        if(res != VK_SUCCESS) LOG(error, "[GetSwapchainImages] : %s", ResultString(res));
        
        // get images
        std::vector<VkImage> images(count);
        res = vkGetSwapchainImagesKHR(device, swapchain, &count, images.data());
        if(res != VK_SUCCESS) LOG(error, "[GetSwapchainImages] : %s", ResultString(res));

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
        CHECK_VULKAN_HANDLE(device)

        // check image view handle
        CHECK_VULKAN_HANDLE(imageView)
        
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
        CHECK_VULKAN_HANDLE(device)

        // image view
        VkImageView imageView;

        // create image view
        VkResult resImageViewCreateInfo = vkCreateImageView(device, &ivCreateInfo, allocator, &imageView);
        
        // check success
        ASSERT(resImageViewCreateInfo == VK_SUCCESS, 
            "[CreateImageView] : Image View creation failed -> returned %s", ResultString(resImageViewCreateInfo));

        // print success
        LOG(success, "[CreateImageView] : Image View creation successful");

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
        CHECK_VULKAN_HANDLE(device)

        // check command pool handle
        CHECK_VULKAN_HANDLE(commandPool)

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
        CHECK_VULKAN_HANDLE(device)

        // command pool
        VkCommandPool commandPool;

        // create
        VkResult resCreateCommandPool = vkCreateCommandPool(device, &commandPoolInfo, allocator, &commandPool);
    
        // check success
        ASSERT(resCreateCommandPool == VK_SUCCESS, "Command Pool creation failed -> returned %s", ResultString(resCreateCommandPool));
    
        // print success
        LOG(success, "[CreateCommandPool] : Command Pool creation successful");

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
        CHECK_VULKAN_HANDLE(device)

        // command buffers
        std::vector<VkCommandBuffer> commandBuffers(cmdBufAllocInfo.commandBufferCount);

        // create
        VkResult resAllocateCommandBuffers = vkAllocateCommandBuffers(device, &cmdBufAllocInfo, commandBuffers.data());
    
        // check success
        ASSERT(resAllocateCommandBuffers == VK_SUCCESS, "Command Buffer allocation failed -> returned : %s", ResultString(resAllocateCommandBuffers));
    
        // print success
        LOG(success, "[AllocateCommandBuffers] : %i Command Buffers allocated successfully", static_cast<uint>(commandBuffers.size()));

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
        CHECK_VULKAN_HANDLE(device)

        // check valid renderpass handle
        CHECK_VULKAN_HANDLE(renderpass)
    
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
        CHECK_VULKAN_HANDLE(device)

        // renderpass
        VkRenderPass renderpass;

        // create
        VkResult resCreateRenderPass = vkCreateRenderPass(device, &createInfo, allocator, &renderpass);

        // check success
        ASSERT(resCreateRenderPass == VK_SUCCESS, "RenderPass creation failed -> return : %s", ResultString(resCreateRenderPass));

        // print success
        LOG(success, "[CreateRenderPass] : RenderPass creation successful");

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
        CHECK_VULKAN_HANDLE(device)
    
        // check valid framebuffer handle
        CHECK_VULKAN_HANDLE(framebuffer)

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
        CHECK_VULKAN_HANDLE(device)

        // framebuffer handle
        VkFramebuffer framebuffer;

        // create
        VkResult resCreateFramebuffer = vkCreateFramebuffer(device, &framebufferCreateInfo, allocator, &framebuffer);

        // check success
        ASSERT(resCreateFramebuffer == VK_SUCCESS, "Framebuffer creation failed -> returned : %s", ResultString(resCreateFramebuffer));

        // print success
        LOG(success, "[CreateFramebuffer] : Framebuffer creation successful");

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
        CHECK_VULKAN_HANDLE(device)

        // check valid fence handle
        CHECK_VULKAN_HANDLE(fence)

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
        CHECK_VULKAN_HANDLE(device)

        // fence
        VkFence fence;

        // create
        VkResult resCreateFence = vkCreateFence(device, &fenceCreateInfo, allocator, &fence);

        // check success
        ASSERT(resCreateFence == VK_SUCCESS, "Fence creation failed -> returned : %s", ResultString(resCreateFence));

        // print success
        LOG(success, "[CreateFence] : Fence creation successful");

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
        CHECK_VULKAN_HANDLE(device)

        // wait
        VkResult resWaitFences = vkWaitForFences(device, fences.size(), fences.data(), waitAll, timeout);

        // check success
        ASSERT(resWaitFences == VK_SUCCESS, "Something wrong happened while waiting for Fence(s) -> returned %s", ResultString(resWaitFences));
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
        CHECK_VULKAN_HANDLE(device)

        // wait
        VkResult resWaitFence = vkWaitForFences(device, 1, &fence, VK_TRUE, timeout);

        // check success
        ASSERT(resWaitFence == VK_SUCCESS, "Something wrong happened while waiting for a Fence -> returned %s", ResultString(resWaitFence));
    }

    /**
     * @brief reset multiple fences(s) at once
     * 
     * @param device 
     * @param fences 
     */
    inline void ResetFences(const VkDevice& device, const std::vector<VkFence>& fences){
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        // reset
        VkResult resResetFences = vkResetFences(device, fences.size(), fences.data());

        // check success
        ASSERT(resResetFences == VK_SUCCESS, "Reset Fence(s) failed -> returned : %s", ResultString(resResetFences));
    }

    /**
     * @brief reset only one fence
     * 
     * @param device 
     * @param fence
     */
    inline void ResetFence(const VkDevice& device, const VkFence& fence){
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        // reset
        VkResult resResetFence = vkResetFences(device, 1, &fence);

        // check success
        ASSERT(resResetFence == VK_SUCCESS, "Reset Fence failed -> returned : %s", ResultString(resResetFence));
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
        CHECK_VULKAN_HANDLE(device)

        // check valid fence handle
        CHECK_VULKAN_HANDLE(semaphore)

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
        CHECK_VULKAN_HANDLE(device)

        // fence
        VkSemaphore semaphore;

        // create
        VkResult resCreateSemaphore = vkCreateSemaphore(device, &semaphoreCreateInfo, allocator, &semaphore);

        // check success
        ASSERT(resCreateSemaphore == VK_SUCCESS, "Semaphore creation failed -> returned : %s", ResultString(resCreateSemaphore));

        // print success
        LOG(success, "[CreateSemaphore] : Semaphore creation successful");

        // return
        return semaphore;
    }

    [[nodiscard]] inline uint32 AcquireNextImage(const VkDevice& device, const VkSwapchainKHR& swapchain, const uint64& timeout, const VkSemaphore& semaphore, const VkFence& fence){
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        // check valid swapchain handle
        CHECK_VULKAN_HANDLE(swapchain)

        // image index
        uint32 idx;

        // get index
        VkResult resAcquireNextImage = vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, &idx);

        // check success
        ASSERT(resAcquireNextImage == VK_SUCCESS, "Failed to acquire next image index -> returned : %s", ResultString(resAcquireNextImage));

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
        CHECK_VULKAN_HANDLE(cmdBuffer)

        // reset
        VkResult resResetCommandBuffer = vkResetCommandBuffer(cmdBuffer, flags);

        // check success
        ASSERT(resResetCommandBuffer == VK_SUCCESS, "Reset Command Buffer failed -> returned : %s", ResultString(resResetCommandBuffer));
    }

    /**
     * @brief begin command buffer recording
     * 
     * @param cmdBuffer 
     * @param beginInfo 
     */
    inline void BeginCommandBuffer(const VkCommandBuffer& cmdBuffer, const VkCommandBufferBeginInfo& beginInfo){
        // check valid command buffer handle
        CHECK_VULKAN_HANDLE(cmdBuffer)

        // begin
        VkResult resBeginCommandBuffer = vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        // check success
        ASSERT(resBeginCommandBuffer == VK_SUCCESS, "Failed to begin Command Buffer recording -> returned : %s", ResultString(resBeginCommandBuffer));
    }

    /**
     * @brief end command buffer recording
     * 
     * @param cmdBuffer 
     */
    inline void EndCommandBuffer(const VkCommandBuffer& cmdBuffer){
        // check valid command buffer handle
        CHECK_VULKAN_HANDLE(cmdBuffer)

        // begin
        VkResult resEndCommandBuffer = vkEndCommandBuffer(cmdBuffer);

        // check success
        ASSERT(resEndCommandBuffer == VK_SUCCESS, "Failed to end Command Buffer recording -> returned : %s", ResultString(resEndCommandBuffer));
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
        CHECK_VULKAN_HANDLE(cmdBuffer)

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
        CHECK_VULKAN_HANDLE(cmdBuffer)

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
        CHECK_VULKAN_HANDLE(queue)

        // submit
        VkResult resQueueSubmit = vkQueueSubmit(queue, submitInfos.size(), submitInfos.data(), fence);

        // check success
        ASSERT(resQueueSubmit == VK_SUCCESS, "Queue submit failed -> returned : %s", ResultString(resQueueSubmit));
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
        CHECK_VULKAN_HANDLE(cmdBuffer)

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
        CHECK_VULKAN_HANDLE(cmdBuffer)

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
        CHECK_VULKAN_HANDLE(cmdBuffer)
    
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
        CHECK_VULKAN_HANDLE(cmdBuffer)

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
        CHECK_VULKAN_HANDLE(device)
    
        // wait
        VkResult resDeviceWaitIdle = vkDeviceWaitIdle(device);

        // check success
        ASSERT(resDeviceWaitIdle == VK_SUCCESS, "Device waid idle failed -> returned : %s", ResultString(resDeviceWaitIdle));
    }

    /**
     * @brief Create a Descriptor Set Layout
     * 
     * @param device 
     * @param createInfo 
     * @param pAllocator 
     * @return VkDescriptorSetLayout 
     */
    inline VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo& createInfo, const VkAllocationCallbacks *pAllocator = nullptr){
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        // set layout
        VkDescriptorSetLayout layout;

        // create
        VkResult resCreateDescriptorSetLayout = vkCreateDescriptorSetLayout(device, &createInfo, pAllocator, &layout);
    
        // check success
        ASSERT(resCreateDescriptorSetLayout == VK_SUCCESS, "Descriptor Set Layout creation failed -> returned : %s", ResultString(resCreateDescriptorSetLayout));
    
        // print success
        LOG(success, "[CreateDescriptorSetLayout] : Descriptor Set Layout creation successful");

        // return
        return layout;
    }

     /**
     * @brief Create a Descriptor Pool
     * 
     * @param device 
     * @param createInfo 
     * @param pAllocator 
     * @return VkDescriptorSetLayout 
     */
    inline VkDescriptorPool CreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo& createInfo, const VkAllocationCallbacks *pAllocator = nullptr){
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        VkDescriptorPool pool;

        // create
        VkResult resCreateDescriptorPool = vkCreateDescriptorPool(device, &createInfo, pAllocator, &pool);
    
        // check success
        ASSERT(resCreateDescriptorPool == VK_SUCCESS, "Descriptor Pool creation failed -> returned : %s", ResultString(resCreateDescriptorPool));
    
        // print success
        LOG(success, "[CreateDescriptorPool] : Descriptor Pool creation successful");

        // return
        return pool;
    }


    /**
     * @brief submit to present queue
     * 
     * @param queue 
     * @param presentInfo 
     */
    inline void QueuePresent(const VkQueue& queue, const VkPresentInfoKHR& presentInfo){
        // check valid queue handle
        CHECK_VULKAN_HANDLE(queue)

        // present
        VkResult resQueuePresent = vkQueuePresentKHR(queue, &presentInfo);

        // check success
        ASSERT(resQueuePresent == VK_SUCCESS, "Queue present failed -> returned : %s", ResultString(resQueuePresent));
    }

    /**
     * @brief destroy shader module
     * 
     * @param device 
     * @param shader 
     * @param allocator
     */
    inline void DestroyShaderModule(const VkDevice& device, const VkShaderModule& shaderModule, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check valid device handle
        CHECK_VULKAN_HANDLE(device)

        // check valid shader handle
        CHECK_VULKAN_HANDLE(shaderModule)

        // destroy
        vkDestroyShaderModule(device, shaderModule, allocator);
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
        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // shader module handle
        VkShaderModule shader;

        // create and check
        VkResult resCreateShaderModule = vkCreateShaderModule(device, &smCreateInfo, allocator, &shader);
        ASSERT(resCreateShaderModule == VK_SUCCESS, "Failed to create Shader Module -> returned : %s", ResultString(resCreateShaderModule));

        // print success
        LOG(success, "[CreateShaderModule] : Shader Module creation successful");

        return shader;
    }

    /**
     * @brief destroy pipeline layout
     * 
     * @param device 
     * @param layout 
     * @param allocator
     */
    inline void DestroyPipelineLayout(const VkDevice& device, const VkPipelineLayout& pipelineLayout, const VkAllocationCallbacks* allocator = nullptr) noexcept{
        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // check pipeline layout handle
        CHECK_VULKAN_HANDLE(pipelineLayout)
    
        // destroy
        vkDestroyPipelineLayout(device, pipelineLayout, allocator);
    }

    /**
     * @brief allocate descriptor sets
     * 
     * @param device 
     * @param allocateInfo 
     * @return std::vector<VkDescriptorSet> 
     */
    [[nodiscard]] inline std::vector<VkDescriptorSet> AllocateDescriptorSets(const VkDevice& device, const VkDescriptorSetAllocateInfo& allocateInfo){
        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // descriptor sets
        std::vector<VkDescriptorSet> sets(allocateInfo.descriptorSetCount);

        // allocate
        VkResult resAllocateDescriptorSets = vkAllocateDescriptorSets(device, &allocateInfo, sets.data());

        // check success
        ASSERT(resAllocateDescriptorSets == VK_SUCCESS, "Descriptor Set allocation failed -> returned : %s", ResultString(resAllocateDescriptorSets));

        // print success
        LOG(success, "[AllocateDescriptorSets] : Succesfully allocated %i Descriptor Sets", static_cast<uint32>(sets.size()));

        // return
        return sets;
    }

    /**
     * @brief update descriptor sets
     * 
     * @param device 
     * @param descriptorWrites 
     * @param descriptorCopies 
     */
    inline void UpdateDescriptorSets(const VkDevice& device, const std::vector<VkWriteDescriptorSet>& descriptorWrites, const std::vector<VkCopyDescriptorSet>& descriptorCopies = std::vector<VkCopyDescriptorSet>(0)){
        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // update
        vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), descriptorCopies.size(), descriptorCopies.data());
    }

    /**
     * @brief bind descriptor sets
     * 
     * @param commandBuffer 
     * @param pipelineBindPoint 
     * @param layout 
     * @param firstSet 
     * @param descriptorSets 
     * @param dynamicOffsets 
     */
    inline void CmdBindDescriptorSets(const VkCommandBuffer& commandBuffer, const VkPipelineBindPoint& pipelineBindPoint, const VkPipelineLayout& pipelineLayout, const uint32& firstSet, const std::vector<VkDescriptorSet>& descriptorSets, const std::vector<uint32>& dynamicOffsets = std::vector<uint32>(0)){
        // check valid command buffer handle
        CHECK_VULKAN_HANDLE(commandBuffer)
    
        // check pipeline layout
        CHECK_VULKAN_HANDLE(pipelineLayout)

        // bind sets
        vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, firstSet, descriptorSets.size(), descriptorSets.data(), dynamicOffsets.size(), dynamicOffsets.data());
    }

    /**
     * @brief destroy descriptor pool
     * 
     * @param device 
     * @param descriptorPool 
     * @param allocator 
     */
    inline void DestroyDescriptorPool(const VkDevice& device, const VkDescriptorPool& descriptorPool, const VkAllocationCallbacks *allocator = nullptr){
        // check device handle
        CHECK_VULKAN_HANDLE(device)
        
        // descriptor pool
        CHECK_VULKAN_HANDLE(descriptorPool)

        // destroy
        vkDestroyDescriptorPool(device, descriptorPool, allocator);
    }

    /**
     * @brief destroy descriptor set layout
     * 
     * @param device 
     * @param descriptorSetLayout 
     * @param allocator 
     */
    inline void DestroyDescriptorSetLayout(const VkDevice& device, const VkDescriptorSetLayout& descriptorSetLayout, const VkAllocationCallbacks *allocator = nullptr){
        // check device handle
        CHECK_VULKAN_HANDLE(device)

        // check set layout handle
        CHECK_VULKAN_HANDLE(descriptorSetLayout)
    
        // destroy
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, allocator);
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
        CHECK_VULKAN_HANDLE(device)

        // pipeline layout
        VkPipelineLayout layout;

        // create
        VkResult resCreatePipelineLayout = vkCreatePipelineLayout(device, &pipelineLayoutInfo, allocator, &layout);

        // check success
        ASSERT(resCreatePipelineLayout == VK_SUCCESS, "Pipeline Layout creation failed -> returned %s", ResultString(resCreatePipelineLayout));

        // print success
        LOG(success, "[CreatePipelineLayout] : Pipeline Layout creation successful");

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
        CHECK_VULKAN_HANDLE(device)

        // check pipeline handle
        CHECK_VULKAN_HANDLE(pipeline)

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
        CHECK_VULKAN_HANDLE(device)

        // pipelines vector with same size as that of create infos
        std::vector<VkPipeline> pipelines(createInfos.size());

        // create
        VkResult resCreateGraphicsPipelines = vkCreateGraphicsPipelines(device, pipelineCache, createInfos.size(), createInfos.data(), allocator, pipelines.data());
        
        // check success
        ASSERT(resCreateGraphicsPipelines == VK_SUCCESS, "Failed to create graphics pipeline -> returned %s", ResultString(resCreateGraphicsPipelines));

        // print success
        LOG(success, "[CreateGraphicsPipelines] : %i Graphics Pipeline(s) created successfully", static_cast<uint>(pipelines.size()));

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
        CHECK_VULKAN_HANDLE(device)

        // pipelines vector with same size as that of create infos
        VkPipeline pipeline;

        // create
        VkResult resCreateGraphicsPipeline = vkCreateGraphicsPipelines(device, pipelineCache, 1, &createInfo, allocator, &pipeline);
        
        // check success
        ASSERT(resCreateGraphicsPipeline == VK_SUCCESS, "Failed to create graphics pipeline -> returned %s", ResultString(resCreateGraphicsPipeline));

        // print success
        LOG(success, "[CreateGraphicsPipeline] : Graphics Pipeline created successfully");

        // return
        return pipeline;
    }

} // namespace Vulkan


#endif//BHAYANKAR_VULKAN_HPP