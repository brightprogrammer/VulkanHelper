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
Not yet, but it can give you approx every function to create a simple renderer (a noob one). Little help is highly appreciated.

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
    // get graphics and present queue index
    // the index getters return an integer value
    // index >= 0 means index is valid
    // index < 0  means index is invalid
    int graphicsQueueIdx = Vulkan::GetPhysicalDeviceQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
    // get queue index that support surface presentation
    int presentQueueIdx  = Vulkan::GetPhysicalDeviceSurfaceSupportQueueIndex(physicalDevice, surface);

    // assert macro is provided by Core.hpp
    // first parameter is the condition
    // second parameter is the message to be displayed
    ASSERT(graphicsQueueIdx > 0 && presentQueueIdx > 0, "\tInvalid queue indices returned");

    // queue create infos must contain unique indices
    // on most gpus graphics and surface support queues are same
    // but some of them might have it in different queues
    // this is a consquence of supporting multiple types of op in one queue
    std::set<uint32> uniqueQueueIndices{
        graphicsQueueIdx, presentQueueIdx
    };

    // queue priority
    float queuePriority{1.f};

    // queue create infos
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    // create the unique queue create infos
    for(const auto& idx : uniqueQueueIndices){
        queueCreateInfos.push_back(Vulkan::Init::DeviceQueueCreateInfo(idx, 1, &queuePriority));
    }

    // device extensions
    // device selection already checks for this extension so add it directly
    Names extensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // create device create info using initializers
    VkDeviceCreateInfo deviceCreateInfo 
        = Vulkan::Init::DeviceCreateInfo(extensions, queueCreateInfos);

    // create device
    device = Vulkan::CreateDevice(physicalDevice, deviceCreateInfo);

    // get graphics queue after creating logical device
    graphicsQueue = Vulkan::GetDeviceQueue(device, graphicsQueueIdx.value(), 0);
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

### CONTRIBUTING
Any type of contribution is appreciated. Do take a look at the code commenting style and other stuff to keep the as readable as possible.

Enjoy! :heart:
