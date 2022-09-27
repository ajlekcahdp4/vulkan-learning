// No need to compile this file, it does nothing. Just an example of the code structure.

#include <cstdlib>
#include <iostream>
#include <stdexcept>

class hello_triangle_application
{
  public:
    void run ()
    {
        init_vulkan ();
        main_loop ();
        cleanup ();
    }

  private:
    void init_vulkan () {}

    void main_loop () {}

    void cleanup () {}
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