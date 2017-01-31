#include "Application.hpp"

#include <Kit/Window.hpp>
#include <Kit/Quad.hpp>
#include <Kit/Renderer.hpp>
#include <Kit/Text.hpp>

HelicopterDemo::HelicopterDemo() : kit::Application ()
{
  std::cout << "Created demo!" << std::endl;
}

HelicopterDemo::~HelicopterDemo()
{
  std::cout << "Deleted demo!" << std::endl;
}

void HelicopterDemo::onInitialize()
{
  kit::Application::onInitialize();

  this->getWindow()->setVSync(false);   // Disable vsync

  this->setRenderRate(1000.0 / 60.0);   // Set target FPS to 60
  this->setUpdateRate(1000.0 / 300.0);  // Set target UPS to 300
}

void HelicopterDemo::onRender()
{
  kit::Application::onRender();
}

void HelicopterDemo::onResize(glm::uvec2 newsize)
{
  kit::Application::onResize(newsize);
}
