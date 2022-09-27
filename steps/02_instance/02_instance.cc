#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

const int WIDTH  = 800;
const int HEIGHT = 600;

class hello_triangle_application
{
  public:
    void run ()
    {
        init_window ();
        init_vulkan ();
        main_loop ();
        cleanup ();
    }

  private:
    void init_vulkan () { create_vulkan_instance (); }

    void create_vulkan_instance ()
    {
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
        create_info.enabledLayerCount = 0;

        // actually create an instance.]
        instance = vk::createInstance (create_info);
    }

    void init_window ()
    {
        if ( !glfwInit () )
            throw (std::runtime_error ("Can't initialize glfw!"));

        // tell GLFW not to create OpenGL context
        glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
        // tell GLFW to disable resizable windows
        glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

        // actually create a window
        window = glfwCreateWindow (WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void main_loop ()
    {
        // Event loop
        while ( !glfwWindowShouldClose (window) )
            glfwPollEvents ();
    }

    void cleanup ()
    {
        instance.destroy ();

        glfwDestroyWindow (window);

        glfwTerminate ();
    }

  private:
    GLFWwindow *window = nullptr;
    vk::Instance instance;
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