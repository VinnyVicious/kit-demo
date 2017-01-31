#pragma once

#include <Kit/Model.hpp>

class DemoState;

class Helicopter : public kit::Transformable
{
public:
  Helicopter(DemoState * state);
  ~Helicopter();
  
  // 1.0 is positive, -1.0 is negative
  void inputThrottle(float delta);
  
  // Leans left/right
  // 1.0 is right
  // -1.0 is left
  void inputRoll(float delta);
  
  // Leans forward/backward
  // 1.0 is forward
  // -1.0 is backward
  void inputPitch(float delta);
  
  // Yaws left/right
  // 1.0 is right
  // -1.0 is left
  void inputYaw(float delta);
  
  void update(double const & ms);
  
  kit::Transformable * getPilotHead();
  
protected:
  float stackme[256];
  // If above 0.0, rises
  // If below 0.0, falls
  // If 0.0, hovers
  // throttle always goes towards 0.0 if no input
  float m_throttle = 0.0f;
  float m_targetThrottle = 0.0f;
  
  // How much we are currently rolling/pitching/yawing, 0.0f - 1.0f
  float m_rollDelta = 0.0f;
  float m_targetRollDelta = 0.0f;
  
  float m_pitchDelta = 0.0f;
  float m_targetPitchDelta = 0.0f;
  
  float m_yawDelta = 0.0f;
  float m_targetYawDelta = 0.0f;
  float m_yawAngle = 0.0f;
  
  // How we are currently moving
  glm::vec3 m_velocity;
  
  // How we are currently accelerating
  glm::vec3 m_acceleration;
  
  glm::quat m_currentRotation;
  
  
private:
  
  
  
  DemoState * m_state = nullptr;
  kit::Model    * m_base = nullptr;
  kit::Model    * m_mainRotor = nullptr;
  kit::Model    * m_tailRotor = nullptr;
  kit::Transformable * m_pilotHead = nullptr;
};
