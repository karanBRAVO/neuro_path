#pragma once

#include "raylib.h"
#include "utils/types.hpp"

// Player class for a 3D game
class Player {
  const BoxSize3D dimensions = {0.3f, 1.0f, 0.3f};  // player dimensions
  Vector3 position;                                 // player's position in 3D space
  BoundingBox bbox;                                 // bounding box for collision detection

 public:
  Player(Vector3 initialPos);

  const Vector3& getPos() const { return position; }
  const BoundingBox& getBBox() const { return bbox; }

  void setPos(const Vector3& newPos);
  void draw3D() const;
};
