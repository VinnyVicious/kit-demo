#include "Application.hpp"
#include "DemoState.hpp"

#include <Kit/Material.hpp>
#include <Kit/Texture.hpp>

int main(int argc, char* argv[])
{
  try 
  {
    HelicopterDemo * demo = new HelicopterDemo();
    DemoState * state = new DemoState();
    demo->run(state);
    delete state;
    delete demo;
    
  }
  catch(kit::Exception & e)
  {
    std::cout << "Exception caught: " << e.what() << std::endl << "In file \"" << e.file() << "\" on line " << e.line() << " (in method " << e.method() << ")" << std::endl << std::endl;
  }

  return 0;
}
// Loading texture from file "./data/ch_ahe_z-10w_d.tga"
