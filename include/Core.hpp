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

#ifndef CORE_HPP
#define CORE_HPP

#include <cstdint>
#include <vector>

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
#define ASSERT(b, ...) \
    if(!(b)){ \
        printf("ASSERT::FAILURE [ %s ]\n", #b); \
        printf("ERROR RAISED FROM FILE[ %s ]@FUNCTION[ %s ]@LINE[ %i ]\n", __FILE__, __FUNCTION__, __LINE__); \
        printf(__VA_ARGS__); \
        printf("\n"); \
        exit(-1); \
    }


#endif//CORE_HPP