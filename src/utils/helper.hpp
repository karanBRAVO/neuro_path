#pragma once

#include <optional>
#include <random>
#include <vector>

namespace helper {
template <typename T>
std::optional<T> getRandomElement(const std::vector<T>& vec) {
  if (vec.empty()) return std::nullopt;

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, vec.size() - 1);
  return vec[distrib(gen)];
}
}  // namespace helper
