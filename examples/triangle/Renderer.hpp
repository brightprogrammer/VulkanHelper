/**
 * @file Renderer.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief Triangle Renderer Example
 * @version 0.1
 * @date 2021-04-16
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

#ifndef VULKAN_HELPER_EXAMPLES_TRIANGLE_RENDERER_HPP
#define VULKAN_HELPER_EXAMPLES_TRIANGLE_RENDERER_HPP

#include <VulkanHelper.hpp>

// simple renderer to render a triangle
struct Renderer{
    // vulkan handles
    VkInstance instance;

    // helper functions
    void CreateInstance();
};

#endif//VULKAN_HELPER_EXAMPLES_TRIANGLE_RENDERER_HPP