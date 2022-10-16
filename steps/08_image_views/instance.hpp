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

    std::cout << "Device can support the folowwing extensions:" << std::endl;
    for ( auto &s_extension : supported_extensions )
        std::cout << "\t\"" << s_extension.extensionName << "\"" << std::endl;

    bool found;
    for ( const char *extension : extensions )
    {
        found = false;
        for ( auto &s_extension : supported_extensions )
        {
            if ( strcmp (extension, s_extension.extensionName) == 0 )
            {
                found = true;

                std::cout << "Extension \"" << extension << "\" is supported!\n";
            }
        }
        if ( !found )
        {

            std::cout << "Extension \"" << extension << "\" is not supported!\n";

            return false;
        }
    }

    // check layer support
    std::vector<vk::LayerProperties> supported_layers = vk::enumerateInstanceLayerProperties ();

    std::cout << "Device can support the following layers:\n";
    for ( vk::LayerProperties s_layer : supported_layers )
    {
        std::cout << '\t' << s_layer.layerName << '\n';
    }

    for ( const char *layer : layers )
    {
        found = false;
        for ( vk::LayerProperties s_layer : supported_layers )
        {
            if ( strcmp (layer, s_layer.layerName) == 0 )
            {
                found = true;

                std::cout << "Layer \"" << layer << "\" is supported!\n";
            }
        }
        if ( !found )
        {

            std::cout << "Layer \"" << layer << "\" is not supported!\n";

            return false;
        }
    }

    return true;
}

vk::raii::Instance make_instance (const std::string &appName)
{

    std::cout << "Making an vulkan instance..." << std::endl;

    vk::raii::Context context;

    uint32_t version = context.enumerateInstanceVersion ();

    std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT (version)
              << ", Major: " << VK_API_VERSION_MAJOR (version) << ", Minor: " << VK_API_VERSION_MINOR (version)
              << ", Patch: " << VK_API_VERSION_PATCH (version) << std::endl;

    vk::ApplicationInfo appInfo {appName.c_str (), version, "First engine", version, version};

    /*
     * Everything with Vulkan is "opt-in", so we need to query which extensions glfw needs
     * in order to interface with vulkan.
     */
    uint32_t glfw_ext_count = 0;
    const char **arr_glfw_extensions;
    arr_glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_ext_count);

    std::vector<const char *> glfw_extensions (arr_glfw_extensions, arr_glfw_extensions + glfw_ext_count);

    glfw_extensions.push_back ("VK_EXT_debug_utils");

    std::cout << "Extensions to be requated by GLFW:" << std::endl;
    for ( auto &extension : glfw_extensions )
        std::cout << "\t\"" << extension << "\"" << std::endl;

    std::vector<const char *> layers;

    layers.push_back ("VK_LAYER_KHRONOS_validation");

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

    return vk::raii::Instance {context, create_info};
}

}   // namespace vkinit
}   // namespace graphics