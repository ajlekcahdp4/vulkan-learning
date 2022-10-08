#pragma once

#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace graphics
{
namespace vkinit
{

struct queue_family_indeces
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool complete () { return graphics_family.has_value () && present_family.has_value (); }
};

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

queue_family_indeces find_queue_families (vk::raii::PhysicalDevice &device)
{
    queue_family_indeces indeces;

    std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties ();

#if !defined(NDEBUG)
    std::cout << "Our physical device can support " << queue_families.size () << " queue families" << std::endl;
#endif

    int i = 0;
    for ( auto &queue_family : queue_families )
    {
        if ( queue_family.queueFlags & vk::QueueFlagBits::eGraphics )
        {
            indeces.graphics_family = i;
            indeces.present_family  = i;
        }

#if !defined(NDEBUG)
        std::cout << "Queue family #" << i << " is suitable for graphics and presenting" << std::endl;
#endif

        if ( indeces.complete () )
            return indeces;
        i++;
    }

    return indeces;
}

vk::raii::Device create_logical_device (vk::raii::PhysicalDevice &p_device)
{
    queue_family_indeces indeces = find_queue_families (p_device);
    float queue_priority         = 1.0f;
    vk::DeviceQueueCreateInfo queue_create_info {vk::DeviceQueueCreateFlags {}, indeces.graphics_family.value (), 1,
                                                 &queue_priority};
    vk::PhysicalDeviceFeatures device_features {};   // default setup
    std::vector<const char *> enabled_layers;
#if !defined(NDEBUG)
    enabled_layers.push_back ("VK_LAYER_KHRONOS_validation");
#endif
    vk::DeviceCreateInfo device_create_info {vk::DeviceCreateFlags {},
                                             1,
                                             &queue_create_info,
                                             static_cast<uint32_t> (enabled_layers.size ()),
                                             enabled_layers.data (),
                                             0,
                                             nullptr,
                                             &device_features};
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

vk::raii::Queue get_queue (vk::raii::PhysicalDevice &p_device, vk::raii::Device &l_device)
{
    queue_family_indeces indeces = find_queue_families (p_device);
    return l_device.getQueue (indeces.graphics_family.value (), 0);
}

}   // namespace vkinit
}   // namespace graphics