#pragma once

#include <Kit/ApplicationState.hpp>
#include <Kit/Camera.hpp>
#include <Kit/Light.hpp>
#include <Kit/GridFloor.hpp>
#include <Kit/Skybox.hpp>
#include <Kit/Renderer.hpp>
#include <Kit/Model.hpp>
#include <Kit/Quad.hpp>
#include <Kit/Renderer.hpp>
#include <Kit/Camera.hpp>
#include <Kit/Water.hpp>

class Helicopter;

class DemoState : public kit::ApplicationState
{
  public:

    DemoState();
    virtual ~DemoState();
    
    virtual void allocate() override;
    virtual void release() override;
    
    virtual void onActive() override;
    virtual void onInactive() override;
    
    virtual void handleEvent(const double & mstime, const kit::WindowEvent& evt) override;
    virtual void render() override;
    virtual void update(const double& ms) override;
    
    virtual void onConsoleInactive() override; //< Called when the console is inactivated/hidden
    virtual void onConsoleActive() override;   //< Called when the console is activated/shown
    virtual void onResize(glm::uvec2 v) override;
    
    
    kit::RenderPayload* getRenderPayload();
    
    
  private:
    
    bool m_active = false;
    
    void resizeBuffers(glm::uvec2 resolution);
    
    kit::Renderer       * m_renderer = nullptr;
    kit::RenderPayload  * m_renderPayload = nullptr;
    kit::Quad           * m_renderQuad = nullptr;
    kit::Camera         * m_camera = nullptr;
    kit::Transformable  * m_cameraFollower = nullptr;
        
    kit::Skybox         * m_skybox = nullptr;
	Helicopter          * m_helicopter = nullptr;
    
    kit::Light          * m_envLight = nullptr;
    kit::Light          * m_sunLight = nullptr;
    
    kit::Water          * m_water = nullptr;
    kit::Model * m_track = nullptr;
    
    std::vector<glm::mat4> m_sphereTransforms;
    kit::Model          * m_sphere;
};
