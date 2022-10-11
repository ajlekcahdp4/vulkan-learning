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

void log_device_properties (const vk::raii::PhysicalDevice &device)
{
    vk::PhysicalDeviceProperties properties = device.getProperties ();
    std::cout << "Device name: " << properties.deviceName << std::endl;
    std::cout << "Device type: ";

    switch ( properties.deviceType )
    {
    case vk::PhysicalDeviceType::eCpu:
        std::cout << "CPU" << std::endl;
        break;
    case vk::PhysicalDeviceType::eDiscreteGpu:
        std::cout << "Discrete GPU" << std::endl;
        break;
    case vk::PhysicalDeviceType::eIntegratedGpu:
        std::cout << "Integrated GPU" << std::endl;
        break;
    case vk::PhysicalDeviceType::eVirtualGpu:
        std::cout << "Virtual GPU" << std::endl;
        break;
    default:
        std::cout << "Other" << std::endl;
    }
}

bool device_support_extensions (const vk::raii::PhysicalDevice &device,
                                const std::vector<const char *> &requested_extensions)
{
    std::set<std::string> required_extensions (requested_extensions.begin (), requested_extensions.end ());

#if !defined(NDEBUG)
    std::cout << "Device can support the folowing extensions:" << std::endl;
#endif
    for ( auto &extension : device.enumerateDeviceExtensionProperties () )
    {
#if !defined(NDEBUG)
        std::cout << "\t\"" << extension.extensionName << "\"" << std::endl;
#endif
        required_extensions.erase (extension.extensionName);
    }
    return required_extensions.empty ();
}

bool is_suitable (const vk::raii::PhysicalDevice &device)
{

#if !defined(NDEBUG)
    std::cout << "Checking if device is suitable..." << std::endl;
#endif

    const std::vector<const char *> requested_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#if !defined(NDEBUG)
    std::cout << "We are requesting the folowwing device extensions:" << std::endl;
    for ( auto &extension : requested_extensions )
        std::cout << "\t\"" << extension << "\"" << std::endl;
#endif

    if ( !device_support_extensions (device, requested_extensions) )
    {
#if !defined(NDEBUG)
        std::cout << "Device can't support all the requested extensions!" << std::endl;
#endif
        return false;
    }
#if !defined(NDEBUG)
    std::cout << "Device can support all the requested extensions!" << std::endl;
#endif
    return true;
}

vk::raii::PhysicalDevice choose_phys_device (const vk::raii::Instance &instance)
{
#if !defined(NDEBUG)
    std::cout << "Choosing physical device..." << std::endl;
#endif

    std::vector<vk::raii::PhysicalDevice> available_devices = instance.enumeratePhysicalDevices ();
#if !defined(NDEBUG)
    std::cout << "There are " << available_devices.size () << " available physical device(s) on this system"
              << std::endl;
#endif

    for ( auto &device : available_devices )
    {
#if !defined(NDEBUG)
        log_device_properties (device);
#endif
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
#if !defined(NDEBUG)
    enabled_layers.push_back ("VK_LAYER_KHRONOS_validation");
#endif

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

#if !defined(NDEBUG)
        std::cout << "GPU has been successfully abstracted!" << std::endl;
#endif
        return device;
    } catch ( vk::SystemError &err )
    {
#if !defined(NDEBUG)
        std::cout << "Device creation failed!" << std::endl;
#endif
        return nullptr;
    }
}

vk::SurfaceKHR create_surface (vk::raii::Instance &instance, GLFWwindow *window)
{
    VkSurfaceKHR c_style_surface;
    int res = glfwCreateWindowSurface (*instance, window, nullptr, &c_style_surface);
    if ( res != VK_SUCCESS )
    {
#if !defined(NDEBUG)
        std::cout << "Failed to abstract the glfw surface for Vulkan! ERRCODE: " << res << std::endl;
#endif
        throw std::runtime_error ("Failed to abstract the glfw surface for Vulkan!");
    }
    else
    {
#if !defined(NDEBUG)
        std::cout << "Successfully abstracted the glfw surface for Vulkan!" << std::endl;
#endif
    }
    return c_style_surface;
}

}   // namespace vkinit
}   // namespace graphics