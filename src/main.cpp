#include "maze-generator/maze-generator.hpp"
#include "raylib.h"

int main() {
  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 450;
  const int FPS = 60;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroPath - Maze");
  SetTargetFPS(FPS);
  DisableCursor();

  MazeGenerator maze_generator(SCREEN_WIDTH, SCREEN_HEIGHT);
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