#pragma once
#include <optional>
#include <random>
#include <vector>

#include "raylib.h"

namespace helper {
template <typename T>
std::optional<T> getRandomElement(const std::vector<T>& vec,
                                  std::optional<int> seed = std::nullopt) {
  if (vec.empty()) return std::nullopt;

  static std::mt19937 gen(std::random_device{}());

  if (seed.has_value()) {
    gen.seed(seed.value());
  }

  std::uniform_int_distribution<> distrib(0, vec.size() - 1);
  return vec[distrib(gen)];
}

void play_sound(const Sound& sound);

void stop_sound(const Sound& sound);
}  // namespace helper
