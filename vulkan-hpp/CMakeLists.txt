cmake_minimum_required (VERSION 3.11)

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