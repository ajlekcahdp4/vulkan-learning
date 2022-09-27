// first use of GLFW library

#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    void init_vulkan () {}

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
        glfwDestroyWindow (window);

        glfwTerminate ();
    }

  private:
    GLFWwindow *window = nullptr;
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