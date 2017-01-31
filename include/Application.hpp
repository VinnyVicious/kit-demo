#pragma once

#include <Kit/Application.hpp>
#include <Kit/Renderer.hpp>
#include <Kit/Quad.hpp>

class HelicopterDemo : public kit::Application
{
  public:
    HelicopterDemo();
    ~HelicopterDemo();

    virtual void onResize(glm::uvec2 newsize);
    virtual void onInitialize();
    virtual void onRender();

  private:

};
