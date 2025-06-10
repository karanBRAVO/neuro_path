#include "player.hpp"

Player::Player(Vector3 initialPos) : position(initialPos) {
  bbox.min = {position.x - dimensions.width / 2, position.y, position.z - dimensions.depth / 2};
  bbox.max = {position.x + dimensions.width / 2, position.y + dimensions.height,
              position.z + dimensions.depth / 2};
}

void Player::setPos(const Vector3& newPos) {
  position = newPos;
  bbox.min = {position.x - dimensions.width / 2, position.y, position.z - dimensions.depth / 2};
  bbox.max = {position.x + dimensions.width / 2, position.y + dimensions.height,
              position.z + dimensions.depth / 2};
}

void Player::draw3D() const {
  DrawCylinder(position, dimensions.width / 2, dimensions.width / 2, dimensions.height, 16, BLUE);
  DrawCylinderWires(position, dimensions.width / 2, dimensions.width / 2, dimensions.height, 16,
                    BLUE);
  DrawBoundingBox(bbox, RED);
}
