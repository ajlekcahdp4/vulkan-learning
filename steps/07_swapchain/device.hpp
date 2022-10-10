#pragma once

#include "logging.hpp"

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

struct queue_family_indices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool complete () { return graphics_family.has_value () && present_family.has_value (); }
};

struct swapchain_support_details
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;
};

struct swapchain_bundle
{
    vk::raii::SwapchainKHR swapchain;
    std::vector<vk::Image> images;
    vk::Format format;
    vk::Extent2D extent;
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

queue_family_indices find_queue_families (const vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface)
{
    queue_family_indices indices;

    std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties ();

#if !defined(NDEBUG)
    std::cout << "Our physical device can support " << queue_families.size () << " queue families" << std::endl;
#endif

    int i = 0;
    for ( auto &queue_family : queue_families )
    {
        if ( queue_family.queueFlags & vk::QueueFlagBits::eGraphics )
        {
            indices.graphics_family = i;
#if !defined(NDEBUG)
            std::cout << "Queue family #" << i << " is suitable for graphics" << std::endl;
#endif
        }

        if ( device.getSurfaceSupportKHR (i, *surface) )
        {
            indices.present_family = i;
#if !defined(NDEBUG)
            std::cout << "Queue family #" << i << " is suitable for presenting" << std::endl;
#endif
        }

        if ( indices.complete () )
            return indices;
        i++;
    }

    return indices;
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

std::vector<vk::raii::Queue> get_queue (vk::raii::PhysicalDevice &p_device, vk::raii::Device &l_device,
                                        vk::raii::SurfaceKHR &surface)
{
    queue_family_indices indices = find_queue_families (p_device, surface);
    return {l_device.getQueue (indices.graphics_family.value (), 0),
            l_device.getQueue (indices.present_family.value (), 0)};
}

swapchain_support_details query_swapchain_support (vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface)
{
    swapchain_support_details support;
    support.capabilities = device.getSurfaceCapabilitiesKHR (*surface);
    /*
    uint32_t                                          minImageCount           = {};
    uint32_t                                          maxImageCount           = {};
    VULKAN_HPP_NAMESPACE::Extent2D                    currentExtent           = {};
    VULKAN_HPP_NAMESPACE::Extent2D                    minImageExtent          = {};
    VULKAN_HPP_NAMESPACE::Extent2D                    maxImageExtent          = {};
    uint32_t                                          maxImageArrayLayers     = {};
    VULKAN_HPP_NAMESPACE::SurfaceTransformFlagsKHR    supportedTransforms     = {};
    VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR currentTransform        =
    VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR::eIdentity; VULKAN_HPP_NAMESPACE::CompositeAlphaFlagsKHR
    supportedCompositeAlpha = {}; VULKAN_HPP_NAMESPACE::ImageUsageFlags             supportedUsageFlags     = {};
    */

#if !defined(NDEBUG)
    std::cout << "Swapchain can support the following surface capabilities:\n";

    std::cout << "\tminimum image count: " << support.capabilities.minImageCount << std::endl;
    std::cout << "\tmaximum image count: " << support.capabilities.maxImageCount << std::endl;

    std::cout << "\tcurrent extent: \n";
    /*
    struct Extent2D {
            uint32_t    width;
            uint32_t    height;
    };
    */
    std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << std::endl;
    std::cout << "\t\theight: " << support.capabilities.currentExtent.height << std::endl;

    std::cout << "\tminimum supported extent: \n";
    std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << std::endl;
    std::cout << "\t\theight: " << support.capabilities.minImageExtent.height << std::endl;

    std::cout << "\tmaximum supported extent: \n";
    std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << std::endl;
    std::cout << "\t\theight: " << support.capabilities.maxImageExtent.height << std::endl;

    std::cout << "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << std::endl;

    std::cout << "\tsupported transforms:\n";
    std::vector<std::string> stringList = log_transform_bits (support.capabilities.supportedTransforms);
    for ( auto &line : stringList )
        std::cout << "\t\t" << line << std::endl;

    std::cout << "\tcurrent transform:\n";
    stringList = log_transform_bits (support.capabilities.currentTransform);
    for ( auto &line : stringList )
        std::cout << "\t\t" << line << std::endl;

    std::cout << "\tsupported alpha operations:\n";
    stringList = log_alpha_composite_bits (support.capabilities.supportedCompositeAlpha);
    for ( auto &line : stringList )
        std::cout << "\t\t" << line << std::endl;

    std::cout << "\tsupported image usage:\n";
    stringList = log_image_usage_bits (support.capabilities.supportedUsageFlags);
    for ( auto &line : stringList )
        std::cout << "\t\t" << line << std::endl;

#endif

    support.formats = device.getSurfaceFormatsKHR (*surface);

#if !defined(NDEBUG)

    for ( auto &supportedFormat : support.formats )
    {
        /*
        * typedef struct VkSurfaceFormatKHR {
                VkFormat           format;
                VkColorSpaceKHR    colorSpace;
        } VkSurfaceFormatKHR;
        */

        std::cout << "supported pixel format: " << vk::to_string (supportedFormat.format) << '\n';
        std::cout << "supported color space: " << vk::to_string (supportedFormat.colorSpace) << '\n';
    }
#endif

    support.present_modes = device.getSurfacePresentModesKHR (*surface);

#if !defined(NDEBUG)
    for ( auto &present_mode : support.present_modes )
    {
        std::cout << '\t' << log_present_mode (present_mode) << '\n';
    }
#endif

    return support;
}

vk::SurfaceFormatKHR choose_swapchain_surface_format (const std::vector<vk::SurfaceFormatKHR> &formats)
{
    for ( auto &format : formats )
        if ( format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
            return format;
    return formats[0];
}

vk::PresentModeKHR choose_swapchain_present_mode (const std::vector<vk::PresentModeKHR> &present_modes)
{
    for ( auto &mode : present_modes )
        if ( mode == vk::PresentModeKHR::eMailbox )   // The fastest one
            return mode;
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D choose_swapchain_extent (uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR &capabilities)
{
    // In some systems UINT32_MAX is a flag to say that the size has not been specified
    if ( capabilities.currentExtent.width != UINT32_MAX )
        return capabilities.currentExtent;
    else
    {
        vk::Extent2D extent;

        extent.width =
            std::min (capabilities.maxImageExtent.width, std::max (capabilities.minImageExtent.width, width));

        extent.height =
            std::min (capabilities.maxImageExtent.height, std::max (capabilities.minImageExtent.height, height));
        return extent;
    }
}

swapchain_bundle create_swapchain (vk::raii::Device &logical_device, vk::raii::PhysicalDevice &phys_device,
                                   vk::raii::SurfaceKHR &surface, uint32_t width, uint32_t height)
{
    swapchain_support_details support = query_swapchain_support (phys_device, surface);
    vk::SurfaceFormatKHR format       = choose_swapchain_surface_format (support.formats);
    vk::PresentModeKHR present_mode   = choose_swapchain_present_mode (support.present_modes);
    vk::Extent2D extent               = choose_swapchain_extent (width, height, support.capabilities);
    uint32_t image_count = std::max (support.capabilities.maxImageCount, support.capabilities.minImageCount + 1);

    /*
                * VULKAN_HPP_CONSTEXPR SwapchainCreateInfoKHR(
      VULKAN_HPP_NAMESPACE::SwapchainCreateFlagsKHR flags_         = {},
      VULKAN_HPP_NAMESPACE::SurfaceKHR              surface_       = {},
      uint32_t                                      minImageCount_ = {},
      VULKAN_HPP_NAMESPACE::Format                  imageFormat_   = VULKAN_HPP_NAMESPACE::Format::eUndefined,
      VULKAN_HPP_NAMESPACE::ColorSpaceKHR   imageColorSpace_  = VULKAN_HPP_NAMESPACE::ColorSpaceKHR::eSrgbNonlinear,
      VULKAN_HPP_NAMESPACE::Extent2D        imageExtent_      = {},
      uint32_t                              imageArrayLayers_ = {},
      VULKAN_HPP_NAMESPACE::ImageUsageFlags imageUsage_       = {},
      VULKAN_HPP_NAMESPACE::SharingMode     imageSharingMode_ = VULKAN_HPP_NAMESPACE::SharingMode::eExclusive,
      uint32_t                              queueFamilyIndexCount_ = {},
      const uint32_t *                      pQueueFamilyIndices_   = {},
      VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR preTransform_ =
        VULKAN_HPP_NAMESPACE::SurfaceTransformFlagBitsKHR::eIdentity,
      VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR compositeAlpha_ =
        VULKAN_HPP_NAMESPACE::CompositeAlphaFlagBitsKHR::eOpaque,
      VULKAN_HPP_NAMESPACE::PresentModeKHR presentMode_  = VULKAN_HPP_NAMESPACE::PresentModeKHR::eImmediate,
      VULKAN_HPP_NAMESPACE::Bool32         clipped_      = {},
      VULKAN_HPP_NAMESPACE::SwapchainKHR   oldSwapchain_ = {} ) VULKAN_HPP_NOEXCEPT
                */
    vk::SwapchainCreateInfoKHR create_info {
        vk::SwapchainCreateFlagsKHR (),          *surface, image_count, format.format, format.colorSpace, extent, 1,
        vk::ImageUsageFlagBits::eColorAttachment};
    queue_family_indices indices = find_queue_families (phys_device, surface);

    uint32_t queue_family_indices[] = {indices.graphics_family.value (), indices.present_family.value ()};

    if ( indices.graphics_family.value () != indices.present_family.value () )
    {
        create_info.imageSharingMode      = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queue_family_indices;
    }
    else
        create_info.imageSharingMode = vk::SharingMode::eExclusive;

    create_info.preTransform   = support.capabilities.currentTransform;
    create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;

    swapchain_bundle bundle {logical_device.createSwapchainKHR (create_info)};
    bundle.images = (*logical_device).getSwapchainImagesKHR (*bundle.swapchain);
    bundle.format = format.format;
    bundle.extent = extent;

    return bundle;
}

}   // namespace vkinit
}   // namespace graphics