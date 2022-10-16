#pragma once

#include "logging.hpp"
#include "queues.hpp"

#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

namespace graphics
{
namespace vkinit
{

bool device_support_extensions (const vk::raii::PhysicalDevice &device,
                                const std::vector<const char *> &requested_extensions)
{
    std::set<std::string> required_extensions (requested_extensions.begin (), requested_extensions.end ());

    std::cout << "Device can support the folowing extensions:" << std::endl;

    for ( auto &extension : device.enumerateDeviceExtensionProperties () )
    {

        std::cout << "\t\"" << extension.extensionName << "\"" << std::endl;

        required_extensions.erase (extension.extensionName);
    }
    return required_extensions.empty ();
}

bool is_suitable (const vk::raii::PhysicalDevice &device)
{

    std::cout << "Checking if device is suitable..." << std::endl;

    const std::vector<const char *> requested_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    std::cout << "We are requesting the folowwing device extensions:" << std::endl;
    for ( auto &extension : requested_extensions )
        std::cout << "\t\"" << extension << "\"" << std::endl;

    if ( !device_support_extensions (device, requested_extensions) )
    {

        std::cout << "Device can't support all the requested extensions!" << std::endl;

        return false;
    }

    std::cout << "Device can support all the requested extensions!" << std::endl;

    return true;
}

vk::raii::PhysicalDevice choose_phys_device (const vk::raii::Instance &instance)
{

    std::cout << "Choosing physical device..." << std::endl;

    std::vector<vk::raii::PhysicalDevice> available_devices = instance.enumeratePhysicalDevices ();

    std::cout << "There are " << available_devices.size () << " available physical device(s) on this system"
              << std::endl;

    for ( auto &device : available_devices )
    {

        log_device_properties (device);

        if ( is_suitable (device) )
            return device;
    }

    return nullptr;
}

vk::raii::Device create_logical_device (vk::raii::PhysicalDevice &p_device, vk::raii::SurfaceKHR &surface)
{
    queue_family_indices indices = find_queue_families (p_device, surface);

    std::vector<uint32_t> unique_indices;
    unique_indices.push_back (indices.graphics_family.value ());
    if ( indices.graphics_family.value () != indices.present_family.value () )
        unique_indices.push_back (indices.present_family.value ());

    float queue_priority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
    for ( auto queue_family_index : unique_indices )
        queue_create_info.push_back (
            vk::DeviceQueueCreateInfo {vk::DeviceQueueCreateFlags {}, queue_family_index, 1, &queue_priority});

    std::vector<const char *> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    vk::PhysicalDeviceFeatures device_features {};   // default setup
    std::vector<const char *> enabled_layers;

    enabled_layers.push_back ("VK_LAYER_KHRONOS_validation");

    // clang-format off
    vk::DeviceCreateInfo device_create_info {
        vk::DeviceCreateFlags {},
        static_cast<uint32_t> (queue_create_info.size ()),                                    
        queue_create_info.data (),
        static_cast<uint32_t> (enabled_layers.size ()),
        enabled_layers.data (),
        static_cast<uint32_t>(device_extensions.size()),
        device_extensions.data(),
        &device_features};
    // clang-format on
    try
    {
        vk::raii::Device device = p_device.createDevice (device_create_info);

        std::cout << "GPU has been successfully abstracted!" << std::endl;

        return device;
    } catch ( vk::SystemError &err )
    {

        std::cout << "Device creation failed!" << std::endl;

        return nullptr;
    }
}

vk::SurfaceKHR create_surface (vk::raii::Instance &instance, GLFWwindow *window)
{
    VkSurfaceKHR c_style_surface;
    int res = glfwCreateWindowSurface (*instance, window, nullptr, &c_style_surface);
    if ( res != VK_SUCCESS )
    {

        std::cout << "Failed to abstract the glfw surface for Vulkan! ERRCODE: " << res << std::endl;

        throw std::runtime_error ("Failed to abstract the glfw surface for Vulkan!");
    }
    else
    {

        std::cout << "Successfully abstracted the glfw surface for Vulkan!" << std::endl;
    }
    return c_style_surface;
}

}   // namespace vkinit
}   // namespace graphics