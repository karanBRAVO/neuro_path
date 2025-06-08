#define FPS 60

#include "maze-generator/maze-generator.hpp"
#include "raylib.h"

int main() {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "NeuroPath - Maze");
  SetTargetFPS(FPS);

  MazeGenerator maze_generator(screenWidth, screenHeight);
  maze_generator.generate();

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    maze_generator.draw();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}