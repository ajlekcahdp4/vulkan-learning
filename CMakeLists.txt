cmake_minimum_required (VERSION 3.11)

project (vulkan_training)

find_package(glfw3 REQUIRED)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

if (NOT VulkanFOUND)
    find_library(Vulkan_LIBRARY REQUIRED)
    if (Vulkan_LIBRARY)
        set (Vulkan_FOUND ON)
    endif()
endif()

if (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
endif()   

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
add_subdirectory(steps)