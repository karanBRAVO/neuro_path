#pragma once

// Structure representing a 3D size
// (width, height, depth)
struct BoxSize3D {
  float width;
  float height;
  float depth;
  BoxSize3D(float w, float h, float d) : width(w), height(h), depth(d) {}
};
