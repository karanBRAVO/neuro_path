#include "camera3d.hpp"

namespace neuro_path {

Camera3D::Camera3D(Vector3 pos, Vector3 target) {
  camera.position = pos;
  camera.target = target;
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
}

Vector3 Camera3D::getDirection() {
  return {cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw)};
}

Vector3 Camera3D::getForward() {
  return {sinf(yaw), 0.0f, cosf(yaw)};
}

Vector3 Camera3D::getRight() {
  return {sinf(yaw + PI / 2), 0.0f, cosf(yaw + PI / 2)};
}

void Camera3D::setPosition(const Vector3& pos) {
  camera.position = pos;
}

void Camera3D::setTarget(const Vector3& target) {
  camera.target = target;
}

void Camera3D::update() {
  Vector2 mouseDelta = GetMouseDelta();
  yaw -= mouseDelta.x * mouseSensitivity;
  pitch -= mouseDelta.y * mouseSensitivity;

  // Clamp pitch to prevent over-rotation
  if (pitch > maxPitch) pitch = maxPitch;
  if (pitch < -maxPitch) pitch = -maxPitch;
}

}  // namespace neuro_path