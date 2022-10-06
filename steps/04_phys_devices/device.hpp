#pragma once

#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

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
    std::set<std::string> required_extensions (requested_extensions.begin (),
                                               requested_extensions.end ());

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
    std::cout << "Device can support all the requested extensions!" << std::endl;
    return true;
}

vk::raii::PhysicalDevice choose_phys_device (const vk::raii::Instance &instance)
{
#if !defined(NDEBUG)
    std::cout << "Choosing physical device..." << std::endl;
#endif

    std::vector<vk::raii::PhysicalDevice> available_devices = instance.enumeratePhysicalDevices ();
#if !defined(NDEBUG)
    std::cout << "There are " << available_devices.size ()
              << " available physical device(s) on this system" << std::endl;
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

}   // namespace vkinit
}   // namespace graphics