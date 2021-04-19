/**
 * @file Core.hpp
 * @author Siddharth Mishra (bshock665@gmail.com)
 * @brief Contains core typedefs and functions that almost all files need
 * @version 0.1
 * @date 2021-04-11
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

  
#ifndef VULKAN_HELPER_CORE_HPP
#define VULKAN_HELPER_CORE_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <string>
#include <cinttypes>

// convinience typedefs
typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef std::vector<const char*> Names;

/**
 * @brief Convinient assert macro definition. Pass in the
 *        condition as first paramter and then the printf style
 *        formatted string that will be printend on screen as
 *        debug message
 * 
 */
#ifndef SETTING_DONT_ASSERT
#define ASSERT(b, ...) \
    if(!(b)){ \
        printf("\nASSERT::FAILURE [ %s ]\n", #b); \
        printf("ERROR RAISED FROM FILE[ %s ]@FUNCTION[ %s ]@LINE[ %i ]\n", __FILE__, __FUNCTION__, __LINE__); \
        printf("\t"); \
        printf(__VA_ARGS__); \
        printf("\n\n"); \
        exit(-1); \
    }
#endif//SETTING_DONT_ASSERT

// if produce logs is defined then logging macro will be defined else it will be empty
#ifndef SETTING_DONT_GENERATE_LOG
    // simple log macro
    #define LOG(severity, ...) { \
        printf("\n[%s] : GENERATED FROM FILE[ %s ]@FUNCTION[ %s ]@LINE[ %i ]\n", #severity, __FILE__, __FUNCTION__, __LINE__); \
        printf("\t"); \
        printf(__VA_ARGS__); \
        printf("\n\n"); \
    }
#else
    #define LOG(...)
#endif//SETTING_DONT_GENERATE_LOG

#ifndef SETTING_DONT_CHECK_VULKAN_HANDLES
    #define CHECK_VULKAN_HANDLE(VulkanHandle) \
        ASSERT(VulkanHandle != VK_NULL_HANDLE, "Invalid Vulkan handle passed as parameter [ parameter name : %s ]", #VulkanHandle)
#endif

// define layer name macros
#define VK_LAYER_KHRONOS_VALIDATION_NAME        "VK_LAYER_KHRONOS_validation"
#define VK_LAYER_LUNARG_API_DUMP_NAME           "VK_LAYER_LUNARG_api_dump"       
#define VK_LAYER_LUNARG_DEVICE_SIMULATION_NAME  "VK_LAYER_LUNARG_device_simulation" 
#define VK_LAYER_LUNARG_MONITOR_NAME            "VK_LAYER_LUNARG_monitor"
#define VK_LAYER_LUNARG_SCREENSHOT_NAME         "VK_LAYER_LUNARG_screenshot"

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

#endif//VULKAN_HELPER_CORE_HPP
