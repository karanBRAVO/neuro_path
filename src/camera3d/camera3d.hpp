#pragma once

#include "raylib.h"
#include "raymath.h"

namespace neuro_path {

class Camera3D {
  Camera camera = {0};                     // camera object
  float mouseX = 0.0f;                     // horizontal mouse movement
  float mouseY = 0.0f;                     // vertical mouse movement
  float yaw = 0.0f;                        // Horizontal rotation
  float pitch = 0.0f;                      // Vertical rotation
  float mouseSensitivity = 0.003f;         // Adjust for sensitivity
  const float maxPitch = 89.0f * DEG2RAD;  // Limit vertical look

 public:
  Camera3D(Vector3 pos, Vector3 target);

  const Camera& getCamera() const { return camera; }
  const float& getYaw() const { return yaw; }
  const float& getPitch() const { return pitch; }
  const Vector3& getPosition() const { return camera.position; }
  Vector3 getDirection();
  Vector3 getForward();  // Forward vector is based on yaw
  Vector3 getRight();    // right vector is perpendicular to forward

  void setPosition(const Vector3& pos);
  void setTarget(const Vector3& target);
  void update();
};

}  // namespace neuro_path