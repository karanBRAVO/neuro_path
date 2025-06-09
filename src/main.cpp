#include "maze-generator/maze-generator.hpp"
#include "raylib.h"

int main() {
  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 400;
  const int FPS = 60;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroPath - Maze");
  SetTargetFPS(FPS);
  DisableCursor();

  MazeGenerator maze_generator(SCREEN_WIDTH, SCREEN_HEIGHT);

  int frame_count = 0;
  int frame_interval = FPS / 2;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    frame_count++;

    if (IsKeyPressed(KEY_SPACE)) {
      maze_generator.start_generation();
      frame_count = 0;
    } else if (IsKeyPressed(KEY_UP)) {
      frame_interval = std::max(1, frame_interval - 1);
    } else if (IsKeyPressed(KEY_DOWN)) {
      frame_interval = std::min(FPS, frame_interval + 1);
    }

    if (frame_count % frame_interval == 0) {
      frame_count = 0;
    }

    maze_generator.draw(frame_count, frame_interval);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}