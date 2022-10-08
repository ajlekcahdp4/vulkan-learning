#pragma once

#include <iostream>
#include <vulkan/vulkan_raii.hpp>

namespace graphics
{
namespace vkinit
{
VKAPI_ATTR VkBool32 VKAPI_CALL debug_call_back (VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

vk::raii::DebugUtilsMessengerEXT make_debug_messenger (vk::raii::Instance &instance)
{
    vk::DebugUtilsMessengerCreateInfoEXT createInfo {
        vk::DebugUtilsMessengerCreateFlagsEXT (),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        debug_call_back, nullptr};
    return instance.createDebugUtilsMessengerEXT (createInfo);
}
}   // namespace vkinit
}   // namespace graphics