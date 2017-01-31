#include "Helicopter.hpp"

#include "DemoState.hpp"

#include <Kit/Renderer.hpp>
#include <Kit/Model.hpp>
#include <Kit/Mesh.hpp>

#define MAX_ACCELXZ 0.0001f
#define MAX_ACCELY 0.00005f

#define MAX_VELOCITY 0.05f
#define YAW_SPEED 0.160f

#define THROTTLE_ACCEL 0.01f

#define PITCH_ACCEL 0.0005f
#define ROLL_ACCEL 0.0005f
#define YAW_ACCEL 0.001f

#define ROLL_RECOVER 0.001f
#define PITCH_RECOVER 0.001f

#define RP_RECOVER 0.01f
#define Y_RECOVER 0.01f

#define Y_DECCEL 0.01f

Helicopter::Helicopter(DemoState * state) : kit::Transformable()
{
  m_state = state;
  
  m_base = new kit::Model("HelicopterBase.mesh");
  m_base->attachTo(this);
  m_state->getRenderPayload()->addRenderable(m_base);
  
  m_mainRotor = new kit::Model("MainRotorBlurred.mesh");
  m_mainRotor->attachTo(this);
  m_mainRotor->setPosition(glm::vec3(0.0f, 0.69807f, -0.0219f));
  m_state->getRenderPayload()->addRenderable(m_mainRotor);
  
  m_tailRotor = new kit::Model("TailRotorBlurred.mesh");
  m_tailRotor->attachTo(this);
  m_tailRotor->setPosition(glm::vec3(0.31641f, 1.07261f, 8.10181f));
  m_state->getRenderPayload()->addRenderable(m_tailRotor);
  
  m_pilotHead = new kit::Transformable();
  m_pilotHead->attachTo(this);
  m_pilotHead->setPosition(glm::vec3(0.0f, -0.45f, -3.0f));
  
}

Helicopter::~Helicopter()
{
  if(m_pilotHead) delete m_pilotHead;
  m_pilotHead = nullptr;
  
  m_state->getRenderPayload()->removeRenderable(m_base);
  delete m_base;
  
  m_state->getRenderPayload()->removeRenderable(m_mainRotor);
  delete m_mainRotor;
  
  m_state->getRenderPayload()->removeRenderable(m_tailRotor);
  delete m_tailRotor;
}

void Helicopter::inputThrottle(float delta)
{
  m_targetThrottle = glm::clamp(delta, -1.0f, 1.0f);
}

void Helicopter::inputPitch(float delta)
{
  m_targetPitchDelta = glm::clamp(delta, -1.0f, 1.0f);
}

void Helicopter::inputRoll(float delta)
{
  m_targetRollDelta = glm::clamp(delta, -1.0f, 1.0f);
}

void Helicopter::inputYaw(float delta)
{
  m_targetYawDelta = glm::clamp(delta, -1.0f, 1.0f);
}

void Helicopter::update(const double& ms)
{
  float time = (float)ms;
 
  m_mainRotor->rotateY(glm::degrees(time*0.004f));
  m_tailRotor->rotateX(glm::degrees(time*-0.016f));
  
  m_throttle = glm::mix(m_throttle, m_targetThrottle, glm::clamp(THROTTLE_ACCEL*time, 0.0f, 1.0f));
  m_pitchDelta = glm::mix(m_pitchDelta, m_targetPitchDelta, glm::clamp(PITCH_ACCEL*time, 0.0f, 1.0f));
  m_rollDelta = glm::mix(m_rollDelta, m_targetRollDelta, glm::clamp(ROLL_ACCEL*time, 0.0f, 1.0f));
  m_yawDelta = glm::mix(m_yawDelta, m_targetYawDelta, glm::clamp(YAW_ACCEL*time, 0.0f, 1.0f));
  m_yawAngle += m_yawDelta * YAW_SPEED * time;
  
  
  // Rotate
  glm::quat targetYaw = glm::rotate(glm::quat(), glm::radians(m_yawAngle), glm::vec3(0.0f, 1.0f, 0.0f));
  
  glm::quat targetRollPitch;
  glm::vec3 o = (glm::vec3(-m_pitchDelta, 0.0f, -m_rollDelta));
  if(glm::length(o) > 0.0f)
  {
    targetRollPitch = glm::rotate(targetRollPitch, glm::radians(55.0f * glm::length(o)), glm::normalize(o));
  }
  
  setRotation(targetYaw * targetRollPitch);
  
  // Set current acceleration
  glm::vec3 heliForward = targetYaw * glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 heliRight = targetYaw * glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 heliUp = glm::vec3(0.0f, 1.0f, 0.0f);
  
  glm::vec3 forceVector = glm::vec3(0.0f, 0.0f, 0.0f);
  forceVector += heliForward * m_pitchDelta;
  forceVector += heliRight * m_rollDelta;
  //forceVector = glm::normalize(forceVector);
  
  m_acceleration = forceVector * (0.33f + (0.66f * glm::clamp(m_throttle, 0.0f, 1.0f))) * MAX_ACCELXZ;
  
  
  float upwards = glm::clamp(glm::dot(glm::normalize(glm::vec3(m_pitchDelta, 0.1f, m_rollDelta)), heliUp), 0.0f, 1.0f);

  forceVector = glm::vec3(0.0f, 0.0f, 0.0f);
  forceVector += heliUp *  upwards;
  
  if(m_throttle > 0.0f)
  {
    m_acceleration += forceVector * m_throttle * MAX_ACCELY;
  }
  else 
  {
    m_acceleration += heliUp * m_throttle * MAX_ACCELY;
  }
   
  // Integrate acceleration into velocity
  m_velocity += m_acceleration * time;
  m_velocity = glm::clamp(m_velocity, glm::vec3(1.0f) * -MAX_VELOCITY, glm::vec3(1.0f) * MAX_VELOCITY);

  m_velocity.y -= 0.000002f * time;
  
  
  // Integrate velocity into position
  translate(m_velocity * time);
  
  m_velocity.x = glm::mix(m_velocity.x, 0.0f, 0.001f*time);
  m_velocity.z = glm::mix(m_velocity.z, 0.0f, 0.001f*time);
  
  m_targetPitchDelta = 0.0f;
  m_targetRollDelta = 0.0f;
  m_targetYawDelta = 0.0f;
  m_targetThrottle = 0.0f;

  //m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

kit::Transformable * Helicopter::getPilotHead()
{
  return m_pilotHead;
}
