# minimum cmake version
cmake_minimum_required(VERSION 3.2)

# project settings
project(VulkanHelper VERSION 0.1 LANGUAGES CXX C)

# find required packages
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

# set c++ version
set(CMAKE_CXX_STANDARD 17) # c++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # yes this version is required
set(CMAKE_CXX_EXTENSIONS OFF) # no extensions

# interface library
add_library(vulkanhelper INTERFACE)
target_include_directories(vulkanhelper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)