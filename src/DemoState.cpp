#include "DemoState.hpp"

#include "Application.hpp"
#include "Helicopter.hpp"

#include <Kit/Window.hpp>
#include <Kit/Text.hpp>
#include <Kit/Types.hpp>
#include <Kit/Console.hpp>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DemoState::DemoState()
{
  std::cout << "Created Demo state!" << std::endl;
}

DemoState::~DemoState()
{
  std::cout << "Deleted Demo state!" << std::endl;
}

/* --- Default overrides --- */

void DemoState::allocate()
{
  std::cout << "Allocating Demo state!" << std::endl;
  m_renderer = new kit::Renderer(glm::uvec2(640, 480));
  ////m_renderer->setFXAA(false);
  //m_renderer->setBloom(false);
  //m_renderer->setInternalResolution(0.75f);
  //m_renderer->setShadows(false);
  m_renderer->setGPUMetrics(false);
  m_renderer->setSceneFringe(true);
  
  m_renderPayload = new kit::RenderPayload();
  m_renderer->registerPayload(m_renderPayload);
  
  m_camera = new kit::Camera(75.0f, 1.0f, glm::vec2(0.1f, 5000.0f));
  //m_camera->setScale(glm::vec3(1.0f, -1.0f, 1.0f));
  m_camera->translate(glm::vec3(0.0f, 0.0f, 10.0f));
  m_renderer->setActiveCamera(m_camera);
  
  m_cameraFollower = new kit::Transformable();
  
  m_renderQuad = new kit::Quad();
  m_renderQuad->setTexture(m_renderer->getBuffer());

  m_skybox = new kit::Skybox("sky3");
  m_renderer->setSkybox(m_skybox);
  
  m_envLight = new kit::Light(kit::Light::IBL);
  m_envLight->setEnvironment("sky3");
  m_envLight->setColor(glm::vec3(1.0f, 1.0f, 1.0f) * 1.0f);
  m_renderPayload->addLight(m_envLight);
  
  m_sunLight = new kit::Light(kit::Light::Directional, glm::uvec2(2048, 2048));
  m_sunLight->setDirection(glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)));
  m_sunLight->setMaxShadowDistance(100.0f);
  m_renderPayload->addLight(m_sunLight);
 
  m_sphere = new kit::Model("Sphere.mesh");
  m_renderPayload->addRenderable(m_sphere);
  
  m_renderPayload->addRenderable(new kit::GridFloor());
  
  m_track = new kit::Model("Destroyer.mesh");
  m_renderPayload->addRenderable(m_track);
  
  std::vector<glm::mat4> shipTransforms;
  for(int x = 0; x < 3; x++)
  {
    glm::mat4 t;
    t = glm::translate(t, glm::vec3(x*100.0f, 0.0f, x*100.0f));
    t = glm::rotate(t, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shipTransforms.push_back(t);
  }
  
  m_track->setInstancing(true, shipTransforms);
  
  m_track->setPosition(glm::vec3(400.0f, -10.0f, 400.0f));
  
  
  for(int x = 0; x < 256; x++)
  {
    float scale = kit::randomFloat(2.0f, 16.0f);
    glm::mat4 t;
    t = glm::translate(t, glm::vec3(kit::randomFloat(-100.0f, 100.0f), kit::randomFloat(-199.0f, 1.0f), kit::randomFloat(-100.0f, 100.0f)));
    t = glm::rotate(t, glm::degrees(kit::randomFloat(0.0f, 360.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    t = glm::rotate(t, glm::degrees(kit::randomFloat(0.0f, 360.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    t = glm::scale(t, glm::vec3(1.0f, 1.0f, 1.0f) * scale);

    m_sphereTransforms.push_back(t);
  }
  m_sphere->setInstancing(true, m_sphereTransforms);
  
  
  m_helicopter = new Helicopter(this);
  m_helicopter->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
  
  m_water = new kit::Water();
  m_water->setScale(glm::vec3(2000.0f, 1.0f, 2000.0f));
  m_renderPayload->addRenderable(m_water);
  
  resizeBuffers(getApplication()->getWindow()->getFramebufferSize());
  
  getApplication()->getScriptEngine().add_global(chaiscript::var(m_renderer), "renderer");
}

void DemoState::release()
{
  std::cout << "Releasing demo state!" << std::endl;
  
  
  if(m_helicopter) delete m_helicopter;
  m_helicopter = nullptr;
  
  m_renderPayload->removeRenderable(m_track);
  if(m_track) delete m_track;
  m_track = nullptr;
  
  m_renderPayload->removeRenderable(m_water);
  if(m_water) delete m_water;
  m_water = nullptr;
  
  m_renderPayload->removeRenderable(m_sphere);
  if(m_sphere) delete m_sphere;
  m_sphere = nullptr;
  
  m_renderPayload->removeLight(m_envLight);
  if(m_envLight) delete m_envLight;
  m_envLight = nullptr;

  m_renderPayload->removeLight(m_sunLight);
  if(m_sunLight) delete m_sunLight;
  m_sunLight = nullptr;
  
  m_renderer->setSkybox(nullptr);
  if(m_skybox) delete m_skybox;
  m_skybox = nullptr;
  
  if(m_renderQuad) delete m_renderQuad;
  m_renderQuad = nullptr;
  
  if(m_cameraFollower) delete m_cameraFollower;
  m_cameraFollower = nullptr;
  
  m_renderer->setActiveCamera(nullptr);
  if(m_camera) delete m_camera;
  m_camera = nullptr;
  
  m_renderer->unregisterPayload(m_renderPayload);
  if(m_renderPayload) delete m_renderPayload;
  m_renderPayload = nullptr;
  
  if(m_renderer) delete m_renderer;
  m_renderer = nullptr;
  
}

void DemoState::handleEvent(const double& mstime, const kit::WindowEvent& evt)
{
  
}

void DemoState::update(const double& ms)
{
  if(getApplication()->getConsole()->isActive())
  {
    return;
  }
  
  static glm::vec2 oldMouse = glm::vec2(0.0f, 0.0f);
  glm::vec2 newMouse = getApplication()->getWindow()->getMousePosition();
  glm::vec2 mouseDelta = newMouse - oldMouse;
  
  const static float mouseSpeed = 0.05f;

  m_water->update(ms);
  //m_camera->setRotation(kit::rotationTo(m_camera->getPosition(), m_helicopter->getPosition()));
  
  static double time = 0;
  time += ms;

  if(getApplication()->getWindow()->isKeyDown(kit::Left))
  {
    m_helicopter->inputRoll(-1.0f);
  }
  
  if(getApplication()->getWindow()->isKeyDown(kit::Right))
  {
    m_helicopter->inputRoll(1.0f);
  }
  
  if(getApplication()->getWindow()->isKeyDown(kit::Up))
  {
    m_helicopter->inputPitch(1.0f);
  }

  if(getApplication()->getWindow()->isKeyDown(kit::Down))
  {
    m_helicopter->inputPitch(-1.0f);
  }
  
  if(getApplication()->getWindow()->isKeyDown(kit::W))
  {
    m_helicopter->inputThrottle(1.0f);
  }

  if(getApplication()->getWindow()->isKeyDown(kit::S))
  {
    m_helicopter->inputThrottle(-1.0f);
  }
  
  if(getApplication()->getWindow()->isKeyDown(kit::A))
  {
    m_helicopter->inputYaw(1.0f);
  }

  if(getApplication()->getWindow()->isKeyDown(kit::D))
  {
    m_helicopter->inputYaw(-1.0f);
  }
  
  m_helicopter->update(ms);
  
  m_cameraFollower->setPosition(m_helicopter->getWorldPosition() + (m_helicopter->getWorldForward() * -15.0f));
  m_cameraFollower->setRotation(m_helicopter->getWorldRotation());
  
  static bool firstPerson = false;
  
  if(getApplication()->getWindow()->isKeyDown(kit::Tab))
  {
    firstPerson = !firstPerson;
  }
  
  if(!firstPerson)
  {
    m_camera->attachTo(nullptr);
    m_camera->setPosition(glm::mix(m_camera->getWorldPosition(), m_cameraFollower->getWorldPosition(), glm::clamp(0.01f * float(ms), 0.0f, 1.0f)));
    m_camera->setRotation(glm::lerp(m_camera->getWorldRotation(), m_cameraFollower->getWorldRotation(), glm::clamp(0.01f * float(ms), 0.0f, 1.0f)));
  }
  else
  {
    m_camera->attachTo(m_helicopter);
    m_camera->setPosition(glm::vec3(0.0f, -0.45f, -2.7f));
    m_camera->setRotation(glm::quat());
  }
  
  oldMouse = newMouse;
}

void DemoState::render()
{
  m_renderer->renderReflections(0.0f);
  m_renderer->renderFrame();
  
  getApplication()->getWindow()->bind();
  
  m_renderQuad->setTexture(m_renderer->getBuffer());
  m_renderQuad->render();
  
  m_renderer->getMetricsText()->render(glm::ivec2(m_renderer->getResolution().x, m_renderer->getResolution().y));
  

  /*m_renderQuad->setTexture(m_renderer->getReflectionMap());
  m_renderQuad->setSize(glm::vec2(0.25f, 0.25f));
  m_renderQuad->render();
  m_renderQuad->setSize(glm::vec2(1.0f, 1.0f));*/
}

void DemoState::onConsoleActive()
{
  kit::ApplicationState::onConsoleActive();
}

void DemoState::onConsoleInactive()
{
  kit::ApplicationState::onConsoleInactive();
}

void DemoState::onActive()
{
  
}

void DemoState::onInactive()
{
}

void DemoState::onResize(glm::uvec2 v)
{
  if(v.x > 0 && v.y > 0)
  {
    resizeBuffers(v);
  }
}

void DemoState::resizeBuffers(glm::uvec2 resolution)
{
  m_renderer->setResolution(resolution);
  m_camera->setAspectRatio((float)resolution.x/(float)resolution.y);
}


kit::RenderPayload * DemoState::getRenderPayload()
{
  return m_renderPayload;
}
