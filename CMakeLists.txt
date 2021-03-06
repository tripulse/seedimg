#[[ 
seedimg - module based image manipulation library written in modern C++ 
Copyright (C) 2020 telugu-boy 
 
This program is free software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version. 
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU Lesser General Public License for more details. 
 
You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
]]
cmake_minimum_required(VERSION 3.5)

get_cmake_property(vars CACHE_VARIABLES)
foreach(var ${vars})
  get_property(currentHelpString CACHE "${var}" PROPERTY HELPSTRING)
    if("${currentHelpString}" MATCHES "No help, variable specified on the command line." OR "${currentHelpString}" STREQUAL "")
        list(APPEND CL_ARGS "-D${var}=${${var}}")
    endif()
endforeach()

project(seedimg LANGUAGES CXX VERSION 1.0 DESCRIPTION "seedimg module based image manipulation library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(ExternalProject)
include(CheckCXXCompilerFlag)

set(AUTODETECT_ENABLED ON CACHE BOOL "Enable automatic detection of images. Requires ALL format modules.")

if(NOT AUTODETECT_ENABLED)
    set(PNG_ENABLED ON CACHE BOOL "Enable PNG")
    set(JPEG_ENABLED ON CACHE BOOL "Enable JPEG")
    set(WEBP_ENABLED ON CACHE BOOL "Enable WebP")
    set(TIFF_ENABLED ON CACHE BOOL "Enable TIFF")
    set(FARBFELD_ENABLED ON CACHE BOOL "Enable Farbfeld")
    set(IRDUMP_ENABLED OFF CACHE BOOL "Enable IR dumping")
else()
    set(PNG_ENABLED ON)
    set(JPEG_ENABLED ON)
    set(WEBP_ENABLED ON)
    set(TIFF_ENABLED ON)
    set(FARBFELD_ENABLED ON)
    set(IRDUMP_ENABLED ON)
endif()

set(FILTERS_ENABLED ON CACHE BOOL "Enable image operations")
set(CCONV_ENABLED ON CACHE BOOL "Enable colourspace conversion")

if(FILTERS_ENABLED AND NOT CCONV_ENABLED)
    message("CCONV_ENABLED must be set to ON for image operations.")
endif()

set(OPENCL_ENABLED OFF CACHE BOOL "Enable OpenCL filters")

set(EXTRAS_ENABLED OFF CACHE BOOL "Enable extras (like histogram)")



option(BUILD_TESTING "" OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

set(LIB_TYPE SHARED CACHE STRING "Build and link as STATIC or SHARED")

message("Building all seedimg libs as ${LIB_TYPE}.")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_THREAD_LIBS_INIT "-pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(WIN32)
  set(prefix "")
  set(suffix ".lib")
elseif(APPLE)
  set(prefix "lib")
  set(suffix ".a")
else()
  set(prefix "lib")
  set(suffix ".a")
endif()

add_subdirectory(src)

CHECK_CXX_COMPILER_FLAG(-Wextra COMPILER_SUPPORTS_WEXTRA)
if(COMPILER_SUPPORTS_WEXTRA)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")

# tests
if (${BUILD_TESTING})
    add_subdirectory(tests)
endif()

# dependent project
add_subdirectory(example)
