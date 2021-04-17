#include "Renderer.hpp"
#include "Vulkan.hpp"
#include "VulkanInitializers.hpp"
#include <vulkan/vulkan_core.h>

// create vulkan instance
void Renderer::CreateInstance(){
    // for now we need only surface extensions
    Names extensions = Vulkan::Tools::GetSurfaceExtensions();

    // and only validation layer
    Names layers = {"VK_LAYER_KRONOS_validation"};

    // create application info
    // no using initializers here just to show how it's done without them
    VkApplicationInfo appInfo = {};
    appInfo.sType                   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName        = "Triangle Example";
    appInfo.applicationVersion      = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName             = "Vulkan Helper";
    appInfo.engineVersion           = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion              = VK_API_VERSION_1_2;

    /**
     * using initializers : 
     * auto appInfo = Vulkan::Init::ApplicationInfo("Triangle Example", VK_MAKE_VERSION(0, 1, 0));
     */

    // create instance create info
    // this is how it's done with initializer
    auto instanceCreateInfo = Vulkan::Init::InstanceCreateInfo(appInfo, extensions, layers);

    // you can set extra things after initializing
    // for example, you can pass instance extension related stuff in VkInstanceCreateInfo::pNext
    // initializers only take minimum parameters to get you going

    // create instance
    // one line, all checks performed for you by create instance
    instance = Vulkan::CreateInstance(instanceCreateInfo);
}

int main(){
    return 0;
}