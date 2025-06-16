#include "helper.hpp"

namespace helper {
void play_sound(const Sound& sound) {
  if (!IsSoundPlaying(sound)) PlaySound(sound);
}

void stop_sound(const Sound& sound) {
  if (IsSoundPlaying(sound)) StopSound(sound);
}
}  // namespace helper