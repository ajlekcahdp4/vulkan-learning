/*
 * After selecting a physical device to use we need to set up a logical device to interface with it.
 */

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

const int WIDTH  = 800;
const int HEIGHT = 600;

const std::vector<const char *> Validation_layers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool Enable_validation_layers = false;
#else
const bool Enable_validation_layers = true;
#endif

class hello_triangle_application
{
  public:
    hello_triangle_application ()
    {
        init_window ();
        init_vulkan ();
    }

    ~hello_triangle_application ()
    {
        device.destroy ();
        instance.destroy ();

        glfwDestroyWindow (window);

        glfwTerminate ();
    }

    void run () { main_loop (); }

  private:
    void init_vulkan ()
    {
        create_vulkan_instance ();
        pick_physical_device ();
        create_logical_device ();
    }

    void pick_physical_device ()
    {
        physical_device = instance.enumeratePhysicalDevices ().front ();
    }

    void create_logical_device ()
    {
        // set device queue info
        float queue_priority = 0.0f;
        vk::DeviceQueueCreateInfo device_queue_info (
            vk::DeviceQueueCreateFlags (),
            static_cast<uint32_t> (find_graphics_queue_family_index ()), 1, &queue_priority);

        // set device features (for now set nothing)
        vk::PhysicalDeviceFeatures deviceFeatures {};

        // create logical device
        device = physical_device.createDevice (
            vk::DeviceCreateInfo (vk::DeviceCreateFlags (), device_queue_info));

        graphics_queue = device.getQueue (find_graphics_queue_family_index (), 0);
    }

    std::size_t find_graphics_queue_family_index ()
    {
        // get the QueueFamilyProperties of the first PhysicalDevice
        std::vector<vk::QueueFamilyProperties> queue_family_properties =
            physical_device.getQueueFamilyProperties ();

        // get the first index into queueFamiliyProperties which supports graphics
        auto property_iterator =
            std::find_if (queue_family_properties.begin (), queue_family_properties.end (),
                          [] (vk::QueueFamilyProperties const &qfp) {
                              return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
                          });

        std::size_t graphics_queue_family_index =
            std::distance (queue_family_properties.begin (), property_iterator);
        assert (graphics_queue_family_index < queue_family_properties.size ());
        return graphics_queue_family_index;
    }

    void create_vulkan_instance ()
    {
        if ( Enable_validation_layers && !check_VL_support () )
            throw std::runtime_error ("validation layers requested, but not available!");

        // OPTIONAL: provide some useful information to the driver in order to optimize our specific
        // application.
        vk::ApplicationInfo app_info ("Hello Triangle", 1, "No engine", 1, VK_API_VERSION_1_0);

        // tells the Vulkan driver which global extensions and validation layers we want to use.
        vk::InstanceCreateInfo create_info ({}, &app_info);

        // the next two layers specify the desired global extensions.
        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions  = glfwGetRequiredInstanceExtensions (&glfw_extension_count);

        create_info.enabledExtensionCount   = glfw_extension_count;
        create_info.ppEnabledExtensionNames = glfw_extensions;

        // determine the global validation layers to enable.
        if ( Enable_validation_layers )
        {
            create_info.enabledLayerCount   = static_cast<uint32_t> (Validation_layers.size ());
            create_info.ppEnabledLayerNames = Validation_layers.data ();
        }
        else
            create_info.enabledLayerCount = 0;

        // actually create an instance.
        instance = vk::createInstance (create_info);
    }

    void init_window ()
    {
        if ( !glfwInit () )
            throw (std::runtime_error ("Can't initialize glfw!"));

        // tell GLFW not to create OpenGL context.
        glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
        // tell GLFW to disable resizable windows.
        glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

        // actually create a window.
        window = glfwCreateWindow (WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    bool check_VL_support ()
    {
        // list all of the available layers.
        uint32_t layer_count;
        vk::enumerateInstanceLayerProperties (&layer_count, nullptr);

        std::vector<vk::LayerProperties> available_layers (layer_count);
        vk::enumerateInstanceLayerProperties (&layer_count, available_layers.data ());

        // check if all the validation layers exists.
        for ( const auto &layerName : Validation_layers )
        {
            bool layer_found = false;

            for ( const auto &layer_properties : available_layers )
            {
                if ( std::strcmp (layerName, layer_properties.layerName) == 0 )
                {
                    layer_found = true;
                    break;
                }
            }

            if ( !layer_found )
                return false;
        }

        return true;
    }

    void main_loop ()
    {
        // Event loop
        while ( !glfwWindowShouldClose (window) )
            glfwPollEvents ();
    }

    GLFWwindow *window = nullptr;
    vk::Instance instance;
    vk::PhysicalDevice physical_device = VK_NULL_HANDLE;
    vk::Device device;
    vk::Queue graphics_queue;
};

int main ()
{
    hello_triangle_application app;

    try
    {
        app.run ();
    } catch ( const std::exception &e )
    {
        std::cerr << e.what () << std::endl;
        return EXIT_FAILURE;
    }
}