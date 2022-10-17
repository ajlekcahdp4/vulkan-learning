#pragma once

#include "device.hpp"
#include "frames.hpp"
#include "instance.hpp"
#include "logging.hpp"
#include "swapchain.hpp"

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

        std::cout << "Making a graphics engine..." << std::endl;

        build_glfw_window ();
        instance = vkinit::make_instance ("first instance");

        vkinit::make_debug_messenger (instance);

        phys_device    = vkinit::choose_phys_device (instance);
        surface        = std::make_unique<vk::raii::SurfaceKHR> (instance, vkinit::create_surface (instance, window));
        device         = vkinit::create_logical_device (phys_device, *surface);
        auto queues    = vkinit::get_queue (phys_device, device, *surface);
        graphics_queue = queues[0];
        present_queue  = queues[1];
        vkinit::query_swapchain_support (phys_device, *surface);
        vkinit::swapchain_bundle bundle = vkinit::create_swapchain (device, phys_device, *surface, width, height);
        swapchain_frames                = std::move (bundle.m_frames);
    }
    ~engine ()
    {

        std::cout << "Destroing graphics engine..." << std::endl;
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
    vkinit::swapchain_bundle swapchain;
    std::vector<vk_utils::swapchain_frame> swapchain_frames;

    // glfw setup
    void build_glfw_window ()
    {
        glfwInit ();

        glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);   // no default rendering client
        // resizing breaks the swapchain, we'll disable it for now
        glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);
        if ( window = glfwCreateWindow (width, height, "First window", nullptr, nullptr) )
        {

            std::cout << "Successfully made a GLFW window" << std::endl;
        }
        else
        {

            std::cout << "GLFW window creation failed" << std::endl;
        }
    }
};
}   // namespace graphics