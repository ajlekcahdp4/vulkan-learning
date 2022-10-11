#pragma once

#include <optional>

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

static queue_family_indices find_queue_families (const vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface)
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

static std::vector<vk::raii::Queue> get_queue (vk::raii::PhysicalDevice &p_device, vk::raii::Device &l_device,
                                               vk::raii::SurfaceKHR &surface)
{
    queue_family_indices indices = find_queue_families (p_device, surface);
    return {l_device.getQueue (indices.graphics_family.value (), 0),
            l_device.getQueue (indices.present_family.value (), 0)};
}

}   // namespace vkinit
}   // namespace graphics