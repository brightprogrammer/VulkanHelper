/**
 * @file VulkanInitializers.hpp
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

#ifndef VULKAN_HELPER_INITIALIZERS_HPP
#define VULKAN_HELPER_INITIALIZERS_HPP

#include <SDL2/SDL_video.h>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "Core.hpp"
#include "Vulkan.hpp"
#include "VulkanTools.hpp"

// Vulkan Namespace contains vulkan initializers, helpers etc
namespace Vulkan{

    // Init namespace contains initializers
    namespace Init{
        /**
        * @brief VkApplicationInfo Initializers
        * 
        * @param pApplicationName name of application (cstring)
        * @param applicationVersion version number of application (uint32).
        *        One can user VK_MAKE_VERSION(major, minor, patch) to make 
        *        a version number but any uint32 value will work.
        * @return VkApplicationInfo filled with basic information
        */
        [[nodiscard]] inline VkApplicationInfo ApplicationInfo(const char* pApplicationName, 
            const uint32& applicationVersion) noexcept{
            // intialize
            VkApplicationInfo applicationInfo = {};
            applicationInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pApplicationName    = pApplicationName;
            applicationInfo.applicationVersion  = applicationVersion; 
            applicationInfo.pEngineName         = "Bhayankar";
            applicationInfo.engineVersion       = VK_MAKE_VERSION(0, 0, 0);
            applicationInfo.apiVersion          = VK_API_VERSION_1_2;

            // return
            return applicationInfo;
        }

        /**
        * @brief VkInstanceCreateInfo Initializer
        * 
        * @param applicationInfo pointer to VkApplicationInfo
        * @param extensions vector of const char* containing names of extensions to be enabled
        * @param layers vector of const char* containing names of layers to be enabled
        * @return VkInstanceCreateInfo filled with basic information
        */
        [[nodiscard]] inline VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo* applicationInfo, 
            const std::vector<const char*>& extensions, const std::vector<const char*>& layers) noexcept{
            // intialize
            VkInstanceCreateInfo instanceCreateInfo = {};
            instanceCreateInfo.sType                        = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo             = applicationInfo;
            instanceCreateInfo.enabledExtensionCount        = static_cast<uint32>(extensions.size());
            instanceCreateInfo.ppEnabledExtensionNames      = extensions.data();
            instanceCreateInfo.enabledLayerCount            = static_cast<uint32>(layers.size());
            instanceCreateInfo.ppEnabledLayerNames          = layers.data();
            instanceCreateInfo.pNext                        = nullptr;

            // return
            return instanceCreateInfo;
        }

        [[nodiscard]] inline VkDeviceQueueCreateInfo DeviceQueueCreateInfo(const uint32& queueIdx,
            const uint32& queueCount, const float* queuePriorities) noexcept{
            // intialize
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueCount       = queueCount;
            queueCreateInfo.pQueuePriorities = queuePriorities;
            queueCreateInfo.queueFamilyIndex = queueIdx;

            // return
            return queueCreateInfo;
        }

        /**
        * @brief Get VkDeviceCreateInfo for device creation
        * 
        * @param extensions to be enabled
        * @param queueCreateInfos to be created
        * @return VkDeviceCreateInfo 
        */
        [[nodiscard]] inline VkDeviceCreateInfo DeviceCreateInfo(const std::vector<const char*>& extensions, 
            const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos) noexcept{
            // intialize
            VkDeviceCreateInfo deviceCreateInfo = {};
            deviceCreateInfo.sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.enabledExtensionCount      = static_cast<uint32>(extensions.size());
            deviceCreateInfo.ppEnabledExtensionNames    = extensions.data();
            deviceCreateInfo.queueCreateInfoCount       = static_cast<uint32>(queueCreateInfos.size());
            deviceCreateInfo.pQueueCreateInfos          = queueCreateInfos.data();

            // return
            return deviceCreateInfo;
        }

        /**
        * @brief swapchain create info initializer
        * 
        * @param physicalDevice 
        * @param surface 
        * @return VkSwapchainCreateInfoKHR 
        */
        [[nodiscard]] inline VkSwapchainCreateInfoKHR SwapchainCreateInfo(const VkPhysicalDevice& physicalDevice, 
            const VkSurfaceKHR& surface, SDL_Window* window) noexcept{
            // get surface information required for swapchain creation
            auto surfacePresentModes = Vulkan::GetPhysicalDeviceSurfacePresentModes(physicalDevice, surface);
            auto surfaceCapabilities = Vulkan::GetPhysicalDeviceSurfaceCapabilities(physicalDevice, surface);
            auto surfaceFormats      = Vulkan::GetPhysicalDeviceSurfaceFormats(physicalDevice, surface);

            // select best available surface format and surface present modes
            // that can be used with images to show on this surface
            auto surfaceFormat           = Vulkan::Tools::SelectSwapchainSurfaceFormat(surfaceFormats);
            auto surfacePresentMode      = Vulkan::Tools::SelectSwapchainSurfacePresentMode(surfacePresentModes);
            
            // store image extent
            auto swapchainImageExtent    = Vulkan::Tools::SelectSwapchainSurfaceImageExtent(window, surfaceCapabilities);
            
            // it is recommended to set minimum image count one more than given min count
            uint32 imageCount   = surfaceCapabilities.minImageCount + 1;

            // check if image count exceeded max image count
            // if max image count is equal to 0 then there is no maximum
            if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
                imageCount = surfaceCapabilities.maxImageCount;

            // first we need swapchain create info
            VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
            swapchainCreateInfo.sType               = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.oldSwapchain        = VK_NULL_HANDLE;
            swapchainCreateInfo.surface             = surface;
            swapchainCreateInfo.presentMode         = surfacePresentMode;
            swapchainCreateInfo.imageFormat         = surfaceFormat.format;
            swapchainCreateInfo.imageColorSpace     = surfaceFormat.colorSpace;
            // colors will be rendererd to these images
            swapchainCreateInfo.imageUsage          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            // what area to render to
            swapchainCreateInfo.imageExtent         = swapchainImageExtent;
            swapchainCreateInfo.minImageCount       = imageCount;
            // only one array layer (no mimpap)
            swapchainCreateInfo.imageArrayLayers    = 1;
            
            // get queue indices
            auto graphicsQueueIdx = GetPhysicalDeviceQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
            auto presentQueueIdx  = GetPhysicalDeviceSurfaceSupportQueueIndex(physicalDevice, surface);

            // check whehter the image will be used by multiple queues or not
            uint32 queueFamilyIndices[] = {graphicsQueueIdx.value(), presentQueueIdx.value()};
            
            // if it is to be used in different queus then
            // set to concurrent mode (many access at once (slow))
            // else set to exclusive mode (single access at a time (fast))
            //
            // note that vulkan believes you so, it wont be doing any checks at runtime
            // you might end up crashing program if you abuse these asked parameters
            if(graphicsQueueIdx.value() != presentQueueIdx.value()){
                swapchainCreateInfo.imageSharingMode        = VK_SHARING_MODE_CONCURRENT;
                swapchainCreateInfo.queueFamilyIndexCount   = 2;
                swapchainCreateInfo.pQueueFamilyIndices     = queueFamilyIndices;
            }else{
                swapchainCreateInfo.imageSharingMode        = VK_SHARING_MODE_EXCLUSIVE;
                swapchainCreateInfo.queueFamilyIndexCount   = 0;
                swapchainCreateInfo.pQueueFamilyIndices     = nullptr;
            }

            swapchainCreateInfo.preTransform        = surfaceCapabilities.currentTransform;
            swapchainCreateInfo.compositeAlpha      = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainCreateInfo.presentMode         = surfacePresentMode;
            swapchainCreateInfo.clipped             = VK_TRUE;

            // return 
            return swapchainCreateInfo;
        }

        /**
         * @brief image view create info
         * 
         * @param image 
         * @param imageFormat 
         * @return VkImageViewCreateInfo 
         */
        [[nodiscard]] inline VkImageViewCreateInfo ImageViewCreateInfo(const VkImage& image, const VkImageAspectFlags& aspectFlags, const VkFormat& imageFormat){
            // intialize
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image                           = image;
            // we want to view it as 2D image
            imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            // using swizzling you can change the view from rgba to argb, agbr, abgr etc...
            // or scale the components
            // here we swizzle to identity, i.e. we take components as it is
            // it is also useful when only rgb format is available and we want bgr
            // SWIZZLE_IDENTITIY is same is SWIZZLE_R to component.r and similary to others
            imageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.format                          = imageFormat;
            // what aspect of image do we want to see
            imageViewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
            // from what mipmap level to start viewing (think of it as a stack)
            imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            // number of layers after base mipmap level to view
            imageViewCreateInfo.subresourceRange.levelCount     = 1;
            // from what layer form an array of layers to start viewing (think of it as a stack)
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            // how much layers above base layer to view
            imageViewCreateInfo.subresourceRange.layerCount     = 1;

            // return
            return imageViewCreateInfo;
        }

        /**
         * @brief command pool create info initializer
         * 
         * @param queueFamilyIdx that the command buffer will submit commands to
         * @return VkCommandPoolCreateInfo 
         */
        [[nodiscard]] inline VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32 queueFamilyIdx, const VkCommandPoolCreateFlags& createFlag){
            // initialize
            VkCommandPoolCreateInfo commandPoolInfo = {};
            commandPoolInfo.sType               = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            // reset means that command buffers allocated from this pool must be able to reset anytime
            commandPoolInfo.flags               = createFlag;
            // queue family index that the command buffers allocated from this pool will submit commands to
            // this way command pool allocates command buffers that are able to submit commands to
            // any queue of that graphics family
            commandPoolInfo.queueFamilyIndex    = queueFamilyIdx;

            // return
            return commandPoolInfo;
        }

        /**
         * @brief command buffer allocate info initializer
         * 
         * @param commandPool 
         * @param commandBufferCount 
         * @return VkCommandBufferAllocateInfo 
         */
        [[nodiscard]] inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(const VkCommandPool& commandPool, uint32 commandBufferCount){
            // initialize
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool        = commandPool;
            commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = commandBufferCount;

            // return
            return commandBufferAllocateInfo;   
        }


        /**
         * @brief attachment description initializer
         * 
         * @param imageFormat 
         * @return VkAttachmentDescription 
         */
        [[nodiscard]] inline VkAttachmentDescription AttachmentDescription(const VkFormat& imageFormat){
            // initialize
            VkAttachmentDescription description = {};
            // attachment must have format needed by the swapchain
            description.format              = imageFormat;
            // no msaa : multi sampling anti aliasing
            description.samples             = VK_SAMPLE_COUNT_1_BIT;
            // load operation when this attachment is loaded
            // default to clear the previous values in this attachment
            // because we don't need previous image data
            description.loadOp              = VK_ATTACHMENT_LOAD_OP_CLEAR;
            // what to do when the renderpass ends
            // default to store the image
            description.storeOp             = VK_ATTACHMENT_STORE_OP_STORE;
            // for now no stencil
            // default to dont care whatever it has
            // if the image format is color then stencil op(s) are ignored
            description.stencilLoadOp       = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            // and dont care what it will do in future
            description.stencilStoreOp      = VK_ATTACHMENT_STORE_OP_STORE;

            // we don't know the initial layout of image
            description.initialLayout       = VK_IMAGE_LAYOUT_UNDEFINED;

            // but we want the image to be in a format that is suitable for
            // presentation to a surface : sourcing the presentation
            description.finalLayout         = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            // return
            return description;
        }

        /**
         * @brief attachment reference initializer
         * 
         * @param attachmentIndex of attachment description in renderpass pAttachments array 
         * @return VkAttachmentReference 
         */
        [[nodiscard]] inline VkAttachmentReference AttachmentReference(const uint32& attachmentIndex){
            // initialize
            VkAttachmentReference attchamentRef = {};
            // the attachment number is the index of the attachment that this reference will refer to
            // in the array of attachments (pAttachments) in the parent renderpass
            attchamentRef.attachment  = attachmentIndex;
            attchamentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // return
            return attchamentRef;
        }

        /**
         * @brief subpass description initializer
         * 
         * @param colorAttachments
         * @param depthStencilAttachments
         * @return VkSubpassDescription 
         */
        [[nodiscard]] inline VkSubpassDescription SubpassDescription(const std::vector<VkAttachmentReference>& colorAttachments, const VkAttachmentReference& depthStencilAttachment){
            // initialize
            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount    = colorAttachments.size();
            subpass.pColorAttachments       = colorAttachments.data();
            subpass.pDepthStencilAttachment = &depthStencilAttachment;
            subpass.inputAttachmentCount    = 0;
            subpass.preserveAttachmentCount = 0;

            // return
            return subpass;
        }

        /**
         * @brief renderpass create info initializer
         * 
         * @param attachments 
         * @param subpasses 
         * @return VkRenderPassCreateInfo 
         */
        [[nodiscard]] inline VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachments, const std::vector<VkSubpassDescription>& subpasses){
            // initialize
            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType              = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassCreateInfo.attachmentCount    = attachments.size();
            renderPassCreateInfo.pAttachments       = attachments.data();
            renderPassCreateInfo.subpassCount       = subpasses.size();
            renderPassCreateInfo.pSubpasses         = subpasses.data();

            // return
            return renderPassCreateInfo;
        }


        /**
         * @brief framebuffer create info initializer
         * 
         * @param renderpass 
         * @param imageViews 
         * @param imageExtent 
         * @return VkFramebufferCreateInfo 
         */
        [[nodiscard]] inline VkFramebufferCreateInfo FramebufferCreateInfo(const VkRenderPass& renderpass, const std::vector<VkImageView>& imageViews, const VkExtent2D& imageExtent){
            // initialze
            VkFramebufferCreateInfo fbCreateInfo = {};
            fbCreateInfo.sType              = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbCreateInfo.renderPass         = renderpass;
            fbCreateInfo.attachmentCount    = imageViews.size();
            fbCreateInfo.pAttachments       = imageViews.data();
            fbCreateInfo.layers             = 1;
            fbCreateInfo.width              = imageExtent.width;
            fbCreateInfo.height             = imageExtent.height;

            // return
            return fbCreateInfo;
        }

        /**
         * @brief fence create info inializer
         * 
         * @param flags what type of fence is this ? signaled?, wait?, what?
         * @return VkFenceCreateInfo 
         */
        [[nodiscard]] inline VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlagBits& flags){
            // initialize
            VkFenceCreateInfo fenceInfo = {};
            fenceInfo.sType         = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags         = flags;

            // return
            return fenceInfo;
        }

        /**
         * @brief semaphore create info initializer 
         * 
         * @return VkSemaphoreCreateInfo 
         */
        [[nodiscard]] inline VkSemaphoreCreateInfo SemaphoreCreateInfo(){
            // initialize
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            // for semaphore we dont need any flags

            // return
            return semaphoreInfo;
        }

        /**
         * @brief Shader Module Create Info initializer
         * 
         * @param code std::vector<uint32> containing spir-v code
         * @return VkShaderModuleCreateInfo 
         */
        [[nodiscard]] inline VkShaderModuleCreateInfo ShaderModuleCreateInfo(const std::vector<char>& code){
            // intialize
            VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
            shaderModuleCreateInfo.sType        = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCreateInfo.codeSize     = code.size();
            shaderModuleCreateInfo.pCode        = reinterpret_cast<const uint32*>(code.data());

            // return
            return shaderModuleCreateInfo;
        }

        /**
         * @brief command buffer begin info initializer
         * 
         * @param usageFlags 
         * @return VkCommandBufferBeginInfo 
         */
        [[nodiscard]] inline VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlagBits& usageFlags){
            // initialize
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags         = usageFlags;

            // return
            return beginInfo;
        }

        /**
         * @brief render pass begin info initializer
         * 
         * @param renderpass 
         * @param windowExtent 
         * @param framebuffer 
         * @param clearColors 
         * @return VkRenderPassBeginInfo 
         */
        [[nodiscard]] inline VkRenderPassBeginInfo RenderPassBeginInfo(const VkRenderPass& renderpass, const VkExtent2D& windowExtent, const VkFramebuffer& framebuffer, const std::vector<VkClearValue>& clearColors){
            // initialize
            VkRenderPassBeginInfo rpInfo = {};
            rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpInfo.renderPass = renderpass;
            rpInfo.renderArea.offset.x = 0;
            rpInfo.renderArea.offset.y = 0;
            rpInfo.renderArea.extent = windowExtent;
            rpInfo.framebuffer = framebuffer;	
            rpInfo.clearValueCount = clearColors.size();
            rpInfo.pClearValues = clearColors.data();

            // return
            return rpInfo;
        }

        /**
         * @brief submit info initializer
         * 
         * @param cmdBuffers 
         * @param waitSemaphores 
         * @param signalSemaphores 
         * @return VkSubmitInfo 
         */
        [[nodiscard]] inline VkSubmitInfo SubmitInfo(const std::vector<VkCommandBuffer>& cmdBuffers,const VkPipelineStageFlags& waitStage, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores){
            // initialize
            VkSubmitInfo submit = {};
            submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit.pNext = nullptr;
            submit.pWaitDstStageMask        = &waitStage;
            submit.waitSemaphoreCount       = waitSemaphores.size();
            submit.pWaitSemaphores          = waitSemaphores.data();
            submit.signalSemaphoreCount     = signalSemaphores.size();
            submit.pSignalSemaphores        = signalSemaphores.data();
            submit.commandBufferCount       = cmdBuffers.size();
            submit.pCommandBuffers          = cmdBuffers.data();

            // return
            return submit;
        }

        /**
         * @brief present info initializer
         * 
         * @param swapchains 
         * @param waitSemaphores 
         * @param imageIndices 
         * @return VkPresentInfoKHR 
         */
        [[nodiscard]] inline VkPresentInfoKHR PresentInfo(const std::vector<VkSwapchainKHR>& swapchains, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<uint32>& imageIndices){
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.pNext = nullptr;
            presentInfo.pSwapchains = swapchains.data();
            presentInfo.swapchainCount = swapchains.size();
            presentInfo.pWaitSemaphores = waitSemaphores.data();
            presentInfo.waitSemaphoreCount = waitSemaphores.size();
            presentInfo.pImageIndices = imageIndices.data();

            return presentInfo;
        }

        /**
         * @brief Shader stage create info initializer
         * 
         * @param shaderStage what shader stage to plug this module ? vertex/fragment/geometry etc...
         * @param module valid shader module
         * @return VkPipelineShaderStageCreateInfo 
         */
        [[nodiscard]] inline VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(const VkShaderStageFlagBits& shaderStage, const VkShaderModule& module){
            // intialize
            VkPipelineShaderStageCreateInfo shaderStageInfo = {};
            shaderStageInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            // vertex/fragment/compute/geometry etc... stage
            shaderStageInfo.stage           = shaderStage;
            shaderStageInfo.module          = module;
            // this is where you set the entry point in shader
            shaderStageInfo.pName           = "main";

            // return
            return shaderStageInfo;
        }

        /**
         * @brief vertex input state create info initializer
         * 
         * @return VkPipelineVertexInputStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(){
            // initialize
            VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            // no vertex attribute or descriptors
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.vertexBindingDescriptionCount = 0;

            // return
            return vertexInputInfo;
        }

        /**
         * @brief Pipeline input assembly state create info initializer
         * 
         * @param topology what primitive topology to use at input assembly stage ? POINT_LIST, TRIANGLE_LIST, LINE_LIST
         * @return VkPipelineInputAssemblyStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology& topology){
            // initialize
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
            inputAssemblyInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyInfo.primitiveRestartEnable    = VK_FALSE;
            inputAssemblyInfo.topology                  = topology; 

            // return 
            return inputAssemblyInfo;
        }

    
        /**
         * @brief Rasterization state create info initializer
         * 
         * @param polygonMode what polygon mode to use in rasterizer
         * @return VkPipelineRasterizationStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(const VkPolygonMode& polygonMode){
            // intialize
            VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
            rasterizerInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizerInfo.cullMode                 = VK_CULL_MODE_NONE;
            rasterizerInfo.polygonMode              = polygonMode;
            rasterizerInfo.depthClampEnable         = VK_FALSE; 
            // discards all primitives before rasterization state if set to VK_TRUE
            rasterizerInfo.rasterizerDiscardEnable  = VK_FALSE;
            rasterizerInfo.lineWidth                = 1.f;
            rasterizerInfo.frontFace                = VK_FRONT_FACE_CLOCKWISE;
            rasterizerInfo.depthBiasEnable          = VK_FALSE;
            rasterizerInfo.depthBiasConstantFactor  = 0.f;
            rasterizerInfo.depthBiasClamp           = 0.f;
            rasterizerInfo.depthBiasSlopeFactor     = 0.f;

            // return
            return rasterizerInfo;
        }

        /**
         * @brief multisample state create info
         * 
         * @return VkPipelineMultisampleStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(){
            // intialize
            VkPipelineMultisampleStateCreateInfo mutlisamplingInfo = {};
            mutlisamplingInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            mutlisamplingInfo.sampleShadingEnable   = VK_FALSE;
            // default to no multisampling
            mutlisamplingInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            mutlisamplingInfo.minSampleShading      = 1.f;
            mutlisamplingInfo.pSampleMask           = nullptr;
            mutlisamplingInfo.alphaToCoverageEnable = VK_FALSE;
            mutlisamplingInfo.alphaToOneEnable      = VK_FALSE;

            // return
            return mutlisamplingInfo;
        }

        /**
         * @brief color blend attachment state initializer
         * 
         * @return VkPipelineColorBlendAttachmentState 
         */
        [[nodiscard]] inline VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(){
            // initialize
            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask =   VK_COLOR_COMPONENT_R_BIT |
                                                    VK_COLOR_COMPONENT_G_BIT |
                                                    VK_COLOR_COMPONENT_B_BIT |
                                                    VK_COLOR_COMPONENT_A_BIT ;
            colorBlendAttachment.blendEnable    = VK_FALSE;

            // return
            return colorBlendAttachment;
        }

        /**
         * @brief color blend state create info initializer
         * 
         * @return VkPipelineColorBlendStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& colorBlendAttachmentStates){
            // initialize
            VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
            colorBlendInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlendInfo.attachmentCount  = colorBlendAttachmentStates.size();
            colorBlendInfo.pAttachments     = colorBlendAttachmentStates.data();
            colorBlendInfo.logicOp          = VK_LOGIC_OP_COPY;
            colorBlendInfo.logicOpEnable    = VK_FALSE;

            // return
            return colorBlendInfo;
        }

        /**
         * @brief viewport state create info initializer
         * 
         * @param viewport viewport
         * @param scissors 
         * @return VkPipelineViewportStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(const std::vector<VkViewport>& viewports, const std::vector<VkRect2D>& scissors){
            // initialize
            VkPipelineViewportStateCreateInfo viewportInfo = {};
            viewportInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportInfo.viewportCount          = viewports.size();
            viewportInfo.pViewports             = viewports.data();
            viewportInfo.scissorCount           = scissors.size();
            viewportInfo.pScissors              = scissors.data();

            // return
            return viewportInfo;
        }

        /**
         * @brief returns empty graphics pipeline create info
         * 
         * @return VkGraphicsPipelineCreateInfo 
         */
        [[nodiscard]] inline VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(){
            // initialize
            VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {};
            graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

            // return
            return graphicsPipelineInfo;
        }

        /**
         * @brief pipeline create info initializer
         * 
         * @return VkPipelineLayoutCreateInfo 
         */
        [[nodiscard]] inline VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(){
            // initialize
            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount           = 0;
            pipelineLayoutInfo.pushConstantRangeCount   = 0;

            // return
            return pipelineLayoutInfo;
        }

        /**
         * @brief buffer create info initializer
         * 
         * @param size of memory to allocate for buffer
         * @param usageFlags is how do you want to use this buffer?
         * @return VkBufferCreateInfo 
         */
        [[nodiscard]] inline VkBufferCreateInfo BufferCreateInfo(const uint32 size, const VkBufferUsageFlags& usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT){
            // initialize
            VkBufferCreateInfo bufferCreateInfo = {};
            bufferCreateInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size               = size;
            bufferCreateInfo.usage              = usageFlags;

            // return
            return bufferCreateInfo;
        }

        /**
         * @brief descriptor set layout create info
         * 
         * @param bindings 
         * @return VkDescriptorSetLayoutCreateInfo 
         */
        [[nodiscard]] inline VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings){
            // initialize
            VkDescriptorSetLayoutCreateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setInfo.flags = 0; // no flags;
            setInfo.bindingCount = static_cast<uint32>(bindings.size());
            setInfo.pBindings = bindings.data();

            // return
            return setInfo;
        }

        /**
         * @brief VkDescriptorPoolCreateInfo initializer
         * 
         * @param poolSizes 
         * @return VkDescriptorPoolCreateInfo 
         */
        [[nodiscard]] inline VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& poolSizes){
            // initialize
            VkDescriptorPoolCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            createInfo.pPoolSizes = poolSizes.data();
            createInfo.poolSizeCount = static_cast<uint32>(poolSizes.size());
            createInfo.maxSets = 10;

            // return
            return createInfo;
        }

        /**
         * @brief image create info initializer
         * 
         * @param format 
         * @param usageFlags 
         * @param extent 
         * @return VkImageCreateInfo 
         */
        [[nodiscard]] inline VkImageCreateInfo ImageCreateInfo(const VkFormat& format, const VkImageUsageFlags& usageFlags, const VkExtent3D& extent){
            // initialize
            VkImageCreateInfo imageCreateInfo = {};
            imageCreateInfo.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageCreateInfo.arrayLayers             = 1;
            imageCreateInfo.extent                  = extent;
            imageCreateInfo.format                  = format;
            imageCreateInfo.mipLevels               = 1;
            imageCreateInfo.samples                 = VK_SAMPLE_COUNT_1_BIT;
            imageCreateInfo.tiling                  = VK_IMAGE_TILING_OPTIMAL;
            imageCreateInfo.usage                   = usageFlags;
            imageCreateInfo.imageType               = VK_IMAGE_TYPE_2D;

            // return
            return imageCreateInfo;
        }

        /**
         * @brief pipeline depth stencil state create info
         * 
         * @param bDepthTest 
         * @param bDepthWrite 
         * @param compareOp 
         * @return VkPipelineDepthStencilStateCreateInfo 
         */
        [[nodiscard]] inline VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(const bool& bDepthTest, const bool& bDepthWrite, const VkCompareOp& compareOp){
            // initializer
            VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
            depthStencilStateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateInfo.depthTestEnable           = bDepthTest ? VK_TRUE : VK_FALSE;
            depthStencilStateInfo.depthWriteEnable          = bDepthWrite ? VK_TRUE : VK_FALSE;
            depthStencilStateInfo.depthCompareOp            = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
            depthStencilStateInfo.depthBoundsTestEnable     = VK_FALSE;
            depthStencilStateInfo.stencilTestEnable         = VK_FALSE;

            // return
            return depthStencilStateInfo;
        }

        /**
         * @brief descriptor set layout binding initializer
         *
         * @param binding is binding number in shader
         * @return VkDescriptorSetLayoutBinding
         */
        [[nodiscard]] inline VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(const uint32& binding, const VkDescriptorType& descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, const VkShaderStageFlags& shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT){
            // initialize
            VkDescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding          = binding;
            layoutBinding.descriptorCount  = 1;
            layoutBinding.descriptorType   = descriptorType;
            layoutBinding.stageFlags       = shaderStageFlags;

            // return
            return layoutBinding;
        }

        /** 
         * @brief write descriptor set initializer
         * 
         * @param binding is binding number in shader to write to
         * @param dstSet is what set to write to
         * @param descriptorType is uniform/sampler/storage... ?
         * @param bufferInfo is information of buffer that will be written
         * 
         * @return 
         */
        [[nodiscard]] inline VkWriteDescriptorSet WriteDescriptorSet(const uint32& binding, const VkDescriptorSet& dstSet, const VkDescriptorType& descriptorType, const VkDescriptorBufferInfo& bufferInfo){
            // initialize
            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstBinding = binding;
            write.dstSet = dstSet;
            write.descriptorCount = 1;
            write.descriptorType = descriptorType;
            write.pBufferInfo = &bufferInfo;

            // return
            return write;
        }

    } // namespace Init

} // namespace Vulkan

#endif//INITIALIZERS_HPP
