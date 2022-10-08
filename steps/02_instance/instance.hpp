#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

namespace graphics
{
namespace vkinit
{
vk::raii::Instance make_instance (const std::string &appName)
{
#if !defined(NDEBUG)
    std::cout << "Making an vulkan instance..." << std::endl;
#endif

    vk::raii::Context context;

    uint32_t version = context.enumerateInstanceVersion ();
#if !defined(NDEBUG)
    std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT (version)
              << ", Major: " << VK_API_VERSION_MAJOR (version) << ", Minor: " << VK_API_VERSION_MINOR (version)
              << ", Patch: " << VK_API_VERSION_PATCH (version) << std::endl;
#endif
    vk::ApplicationInfo appInfo {appName.c_str (), version, "First engine", version, version};

    /*
     * Everything with Vulkan is "opt-in", so we need to query which extensions glfw needs
     * in order to interface with vulkan.
     */
    uint32_t glfw_ext_count = 0;
    const char **arr_glfw_extensions;
    arr_glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_ext_count);

    std::vector<const char *> glfw_extensions (arr_glfw_extensions, arr_glfw_extensions + glfw_ext_count);

#if !defined(NDEBUG)
    std::cout << "Extensions to be requated by GLFW:" << std::endl;
    for ( auto &extension : glfw_extensions )
        std::cout << "\t\"" << extension << "\"" << std::endl;
#endif

    vk::InstanceCreateInfo create_info {vk::InstanceCreateFlags {}, &appInfo, 0, nullptr, glfw_ext_count,
                                        glfw_extensions.data ()};

    try
    {
        return vk::raii::Instance {context, create_info};
    } catch ( vk::SystemError &err )
    {
#if !defined(NDEBUG)
        std::cout << "Failed to create instance!" << std::endl;
#endif
        return nullptr;
    }
}

}   // namespace vkinit
}   // namespace graphics