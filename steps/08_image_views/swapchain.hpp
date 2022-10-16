#pragma once

#include <vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "frames.hpp"
#include "queues.hpp"

namespace graphics
{
namespace vkinit
{

struct swapchain_support_details
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;
};

struct swapchain_bundle
{
    swapchain_bundle () {}
    swapchain_bundle (vk::raii::Device &l_device, vk::SwapchainCreateInfoKHR &create_info)
        : m_impl {l_device.createSwapchainKHR (create_info)}, m_format {create_info.imageFormat},
          m_extent {create_info.imageExtent}
    {
        std::vector<vk::Image> images {(*l_device).getSwapchainImagesKHR (*m_impl)};
        m_frames.reserve (images.size ());

        for ( auto &image : images )
        {
            vk::ImageViewCreateInfo create_info         = {};
            create_info.image                           = image;
            create_info.viewType                        = vk::ImageViewType::e2D;
            create_info.components.r                    = vk::ComponentSwizzle::eIdentity;
            create_info.components.g                    = vk::ComponentSwizzle::eIdentity;
            create_info.components.b                    = vk::ComponentSwizzle::eIdentity;
            create_info.components.a                    = vk::ComponentSwizzle::eIdentity;
            create_info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            create_info.subresourceRange.baseMipLevel   = 0;
            create_info.subresourceRange.levelCount     = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount     = 1;
            create_info.format                          = m_format;

            m_frames.push_back (vk_utils::swapchain_frame {image, l_device.createImageView (create_info)});
        }
    }

    vk::raii::SwapchainKHR m_impl = nullptr;
    std::vector<vk_utils::swapchain_frame> m_frames;
    vk::Format m_format;
    vk::Extent2D m_extent;
};

static vk::SurfaceFormatKHR choose_swapchain_surface_format (const std::vector<vk::SurfaceFormatKHR> &formats)
{
    for ( auto &format : formats )
        if ( format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
            return format;
    return formats[0];
}

static vk::PresentModeKHR choose_swapchain_present_mode (const std::vector<vk::PresentModeKHR> &present_modes)
{
    for ( auto &mode : present_modes )
        if ( mode == vk::PresentModeKHR::eMailbox )   // The fastest one
            return mode;
    return vk::PresentModeKHR::eFifo;
}

static vk::Extent2D choose_swapchain_extent (uint32_t width, uint32_t height,
                                             const vk::SurfaceCapabilitiesKHR &capabilities)
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

static swapchain_support_details query_swapchain_support (vk::raii::PhysicalDevice &p_device,
                                                          vk::raii::SurfaceKHR &surface)
{
    swapchain_support_details support;
    support.capabilities = p_device.getSurfaceCapabilitiesKHR (*surface);
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

    support.formats = p_device.getSurfaceFormatsKHR (*surface);

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

    support.present_modes = p_device.getSurfacePresentModesKHR (*surface);

    for ( auto &present_mode : support.present_modes )
    {
        std::cout << '\t' << log_present_mode (present_mode) << '\n';
    }

    return support;
}

static swapchain_bundle create_swapchain (vk::raii::Device &logical_device, vk::raii::PhysicalDevice &phys_device,
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

    swapchain_bundle bundle {logical_device, create_info};
    return bundle;
}

}   // namespace vkinit
}   // namespace graphics