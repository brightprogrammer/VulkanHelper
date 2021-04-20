# VULKAN HELPER
 
## LICENSE 
#### VulkanHelper uses Apache License 2.0

>Copyright 2021 Siddharth Mishra
>
>Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
>
>    http://www.apache.org/licenses/LICENSE-2.0
>
>Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

 ### WHAT IS VULKAN HELPER ?

**Vulkan Helper** is a simple, close to Vulkan (C API) interface that connects the Vulkan C API and C++ containers. Using the library might cut down your C code to half. Does not provides Vulkan handle wrappers but instead only Vulkan function wrappers with extra checks done automatically and provides an easy human readable message when something wrong happens.

### IS IT COMPLETE ?
Not yet, but it can give you approx every function to create a simple renderer (a noob one). Little help is highly appreciated. The project in developed as I learn Vulkan.  
I am working on my own Vulkan Renderer and I keep adding Vulkan wrapper functions as I go.
So in theory the library won't be complete ever but instead it will be close to completion.

### HOW DO I USE IT ?
There are three namespaces `Vulkan`, `Vulkan::Tools` and `Vulkan::Init`.  
`Vulkan` namespace contains simple functions that wrap Vulkan C functions and `Vulkan::Tools` namespace contains functions like VkPhysicalDevice selection, selection of best VkSurface format, present modes, getting queue family index, getting VkResult string etc...  
The `Vulkan::Init` namespace contains Vulkan struct initializers. You can use these functions to initialize large structs within one or two lines.  

To use the library, copy headers to your local include directoty and then include `VulkanHelper.hpp` in your project to include it all.


#### VULKAN INSTANCE CREATION : 
```c++
void CreateInstance(){
    // Names is just a vector of const char
    Names extensions = Vulkan::Tools::GetSurfaceExtensions();
    // enable debug
    #ifdef DEBUG
        Names layers = {"VK_LAYER_KHRONOS_validation"};
    #else
        Names layers;
    #endif
    
    // create app info using initializer
    VkApplicationInfo applicationInfo = Vulkan::Init::ApplicationInfo("VulkanHelper", 0);

    // create instace create info using initializer
    VkInstanceCreateInfo instanceCreateInfo = Vulkan::Init::InstanceCreateInfo(&applicationInfo, extensions, layers);

    // create and store instance
    // all functions that create something return the handle
    // and not take handle as a parameter (unlike C API)
    instance = Vulkan::CreateInstance(instanceCreateInfo);  
}
```

#### CREATE VULKAN SURFACE : 
```c++
// create surface
void Renderer::CreateSurface(){
    surface = Vulkan::CreateSurface(instance, window);
}
```

#### QUICK PHYSICAL DEVICE SELECTION : 
```c++
// select physical device
void Renderer::SelectPhysicalDevice(){
    physicalDevice = Vulkan::SelectBestPhysicalDevice(instance, surface);
}
```

#### LOGICAL DEVICE (VkDevice) CREATION : 
```c++
// create logical device
void Renderer::CreateDevice(){
    // device extensions
    Names extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // queue create infos
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    // get queue indices
    graphicsIdx = Vulkan::GetPhysicalDeviceQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
    // be sure to check if queue is present on selected device before creation
    ASSERT(graphicsIdx.has_value(), "NO GRAPHICS QUEUE FAMILY PRESENT ON SELECTED DEVICE");
    presentIdx  = Vulkan::GetPhysicalDeviceSurfaceSupportQueueIndex(physicalDevice, surface); 
    ASSERT(presentIdx.has_value(), "SELECTED DEVICE DOESN'T SUPPORT SURFACE PRESENTATION");

    // unique queue indices
    std::set<uint32> uniqueQueueIndices = {graphicsIdx.value(), presentIdx.value()};

    // queue priorities : since we are creating only one queue, only one value in vector
    std::vector<float> queuePriorities = {float{1.f}};
    
    // add unique queues to queue create infos vector
    for(const auto& queueIdx : uniqueQueueIndices)
        queueCreateInfos.push_back(Vulkan::Init::DeviceQueueCreateInfo(queueIdx, queuePriorities));
    
    // device create info
    VkDeviceCreateInfo deviceCreateInfo = Vulkan::Init::DeviceCreateInfo(extensions, queueCreateInfos);

    // create device
    device = Vulkan::CreateDevice(physicalDevice, deviceCreateInfo);

    // get created device queue
    graphicsQueue = Vulkan::GetDeviceQueue(device, graphicsIdx.value(), 0);
}
```

Similarly other Vulkan handles can be created. Notice that you don't have to perform any checks and all as that is already provided by **Vulkan Helper**

#### COMMAND BUFFER RECORDING
```c++
// to begin recording
VkCommandBuffer& cmd; // this is a reference to the current command buffer in use
auto beginInfo = Vulkan::Init::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
Vulkan::BeginCommandBuffer(cmd, beginInfo);
.
.
.
// record commands
Vulkan::CmdBeginRenderPass(cmd, ...);
Vulkan::CmdBindPipeline(cmd, ...);
Vulkan::CmdDraw(cmd, ...);
Vulkan::CmdEndRenderPass(cmd, ...);
.
.
.
//end command buffer recording
Vulkan::EndCommandBuffer(cmd);
```

Functions that take an array of objects in C API take a `std::vector` in **VulkanHelper** API  
For example while updating descriptor sets
```c++
// descriptor writes
VkWriteDescriptorSet writeToSet = {};
.
.
.
// make a vector
std::vector<VkWriteDescriptorSet> descriptorWrites = {writeToSet}; // one write
std::vector<VkCopyDescriptorSet> descriptorCopies = {}; // no copies
Vulkan::UpdateDescriptorSets(device, descriptorWrites, descriptorCopies); // update
```
In the above function the the third parameter is defaulted to empty vector but just to  
improve code readability, make a descriptorCopies vector and pass it as parameter.

### `Vulkan::Tools::VulkanBase` HEADSTART
Although you can initialize Vulkan on your self by creating everything (instance, device, surface, swapchain,...), you can also use VulkanBase to jumpstart your application in just one call.
VulkanBase will quickly create a VkInstance, VkSurfaceKHR, VkDevice, VkSwapchainKHR and VkImageView (s) for swapchain images. All of this can either be done in a single call or you can do them in multiple function calls. It also stores necessary data like, device queues, image extent, image format etc...
```c++
// include all headers in one shot
#include <VulkanHelper.hpp>
.
.
.
Vulkan::Tools::VulkanBase vulkan; // contians all handles
vulkan.Initialize(window); // one call initialize
.
.
.
Vulkan::Tools::VulkanBase vulkan;
// or you can initialize them one by one
vulkan.EnableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
vulkan.EnableSurfaceExtensions();
vulkan.CreateInstance();
vulkan.CreateSurface();
vulkan.SelectPhysicalDevice();
vulkan.EnableDeviceExtension(VK_EXT_SWAPCHAIN_EXTENSION_NAME);
vulkan.CreateDevice();
vulkan.CreateSwapcahain();
vulkan.CreateImageViews();
.
.
.
// you can store the handles like
instance = vulkan.instance
// or you can use them directly
Vulkan::CreateSemaphore(vulkan.device, semaphoreCreateInfo);
.
.
.
```

### CONTRIBUTING
Any type of contribution is appreciated. Do take a look at the code commenting style and other stuff to keep the as readable as possible.

Enjoy! :heart:
