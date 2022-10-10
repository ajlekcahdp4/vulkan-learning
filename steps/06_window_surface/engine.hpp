#pragma once

#include "device.hpp"
#include "instance.hpp"
#include "logging.hpp"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include <iostream>

namespace graphics
{

struct engine
{
  public:
    engine ()
    {
#if !defined(NDEBUG)
        std::cout << "Making a graphics engine..." << std::endl;
#endif
        build_glfw_window ();
        instance = vkinit::make_instance ("first instance");
#if !defined(NDEBUG)
        vkinit::make_debug_messenger (instance);
#endif
        phys_device    = vkinit::choose_phys_device (instance);
        surface        = std::make_unique<vk::raii::SurfaceKHR> (instance, vkinit::create_surface (instance, window));
        device         = vkinit::create_logical_device (phys_device, *surface);
        auto queues    = vkinit::get_queue (phys_device, device, *surface);
        graphics_queue = queues[0];
        present_queue  = queues[1];
    }

    ~engine ()
    {
#if !defined(NDEBUG)
        std::cout << "Destroing graphics engine..." << std::endl;
#endif
        glfwTerminate ();
    }

  private:
    uint32_t width              = 800;
    uint32_t height             = 600;
    GLFWwindow *window          = nullptr;
    vk::raii::Instance instance = nullptr;
    std::unique_ptr<vk::raii::SurfaceKHR> surface {nullptr};
    vk::raii::DebugUtilsMessengerEXT debug_messenger = nullptr;
    vk::raii::PhysicalDevice phys_device             = nullptr;
    vk::raii::Device device                          = nullptr;
    vk::raii::Queue graphics_queue                   = nullptr;
    vk::raii::Queue present_queue                    = nullptr;

    // glfw setup
    void build_glfw_window ()
    {
        glfwInit ();

        glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);   // no default rendering client
        // resizing breaks the swapchain, we'll disable it for now
        glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
        if ( window = glfwCreateWindow (width, height, "First window", nullptr, nullptr) )
        {
#if !defined(NDEBUG)
            std::cout << "Successfully made a GLFW window" << std::endl;
#endif
        }
        else
        {
#if !defined(NDEBUG)
            std::cout << "GLFW window creation failed" << std::endl;
#endif
        }
    }
};
}   // namespace graphics