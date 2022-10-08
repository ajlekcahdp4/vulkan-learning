#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

namespace graphics
{
namespace vkinit
{

bool is_supported (std::vector<const char *> &extensions, std::vector<const char *> &layers,
                   const vk::raii::Context &context)
{
    // check extension support
    std::vector<vk::ExtensionProperties> supported_extensions = context.enumerateInstanceExtensionProperties ();
#if !defined(NDEBUG)
    std::cout << "Device can support the folowwing extensions:" << std::endl;
    for ( auto &s_extension : supported_extensions )
        std::cout << "\t\"" << s_extension.extensionName << "\"" << std::endl;
#endif

    bool found;
    for ( const char *extension : extensions )
    {
        found = false;
        for ( auto &s_extension : supported_extensions )
        {
            if ( strcmp (extension, s_extension.extensionName) == 0 )
            {
                found = true;
#if !defined(NDEBUG)
                std::cout << "Extension \"" << extension << "\" is supported!\n";
#endif
            }
        }
        if ( !found )
        {
#if !defined(NDEBUG)
            std::cout << "Extension \"" << extension << "\" is not supported!\n";
#endif
            return false;
        }
    }

    // check layer support
    std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties ();

#if !defined(NDEBUG)
    std::cout << "Device can support the following layers:\n";
    for ( vk::LayerProperties s_layer : supported_layers )
    {
        std::cout << '\t' << s_layer.layerName << '\n';
    }
#endif

    for ( const char *layer : layers )
    {
        found = false;
        for ( vk::LayerProperties s_layer : supported_layers )
        {
            if ( strcmp (layer, s_layer.layerName) == 0 )
            {
                found = true;
#if !defined(NDEBUG)
                std::cout << "Layer \"" << layer << "\" is supported!\n";
#endif
            }
        }
        if ( !found )
        {
#if !defined(NDEBUG)
            std::cout << "Layer \"" << layer << "\" is not supported!\n";
#endif
            return false;
        }
    }

    return true;
}

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
    glfw_extensions.push_back ("VK_EXT_debug_utils");
#endif

#if !defined(NDEBUG)
    std::cout << "Extensions to be requated by GLFW:" << std::endl;
    for ( auto &extension : glfw_extensions )
        std::cout << "\t\"" << extension << "\"" << std::endl;
#endif

    std::vector<const char *> layers;

#if !defined(NDEBUG)
    layers.push_back ("VK_LAYER_KHRONOS_validation");
#endif

    if ( !is_supported (glfw_extensions, layers, context) )
    {
        return nullptr;
    }

    vk::InstanceCreateInfo create_info {vk::InstanceCreateFlags {},
                                        &appInfo,
                                        static_cast<uint32_t> (layers.size ()),
                                        layers.data (),
                                        static_cast<uint32_t> (glfw_extensions.size ()),
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