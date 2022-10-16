#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace vk_utils
{

struct swapchain_frame
{
    vk::Image image {nullptr};
    vk::raii::ImageView image_view {nullptr};
};

}   // namespace vk_utils