#include <iostream>
#include <vector>

#include "maze-generator/maze-generator.hpp"
#include "raylib.h"
#include "raymath.h"
#include "utils/types.hpp"

int main() {
  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 400;
  const int FPS = 60;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroPath - Maze");
  SetTargetFPS(FPS);
  DisableCursor();

  Camera camera = {0};
  camera.position = {-0.1f, 0.95f, -0.1f};  // x, y, z coordinates
  camera.target = {0.0f, 0.0f, 0.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  MazeGenerator maze_generator(SCREEN_WIDTH, SCREEN_HEIGHT);

  int frame_count = 0;
  int frame_interval = FPS / 2;

  float mouseX = 0.0f;
  float mouseY = 0.0f;
  float yaw = 0.0f;                        // Horizontal rotation
  float pitch = 0.0f;                      // Vertical rotation
  float mouseSensitivity = 0.003f;         // Adjust for sensitivity
  const float maxPitch = 89.0f * DEG2RAD;  // Limit vertical look

  // player
  Vector3 playerPosition = {0.0f, 10.0f, 0.0f};
  const BoxSize3D playerDimensions = {0.3f, 1.0f, 0.3f};  // width, height, depth
  Vector2 velocity = {1.2f, 0.0f};                        // xz, y
  Vector3 acceleration = {0.0f, 9.8f, 0.0f};
  float jump_strength = 5.0f;
  bool isJumping = false;
  BoundingBox playerBox = {
      {playerPosition.x - playerDimensions.width / 2, playerPosition.y,
       playerPosition.z - playerDimensions.depth / 2},
      {playerPosition.x + playerDimensions.width / 2, playerPosition.y + playerDimensions.height,
       playerPosition.z + playerDimensions.depth / 2}};

  bool render3d = false;  // Flag to toggle 3D rendering

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    // if (IsKeyPressed(KEY_I)) {
    //   render3d = !render3d;
    // }

    if (render3d) {
      // UpdateCamera(&camera, CAMERA_FREE);

      Vector2 mouseDelta = GetMouseDelta();
      yaw -= mouseDelta.x * mouseSensitivity;
      pitch -= mouseDelta.y * mouseSensitivity;

      // Clamp pitch to prevent over-rotation
      if (pitch > maxPitch) pitch = maxPitch;
      if (pitch < -maxPitch) pitch = -maxPitch;

      // Calculate camera direction based on yaw and pitch
      Vector3 direction = {cosf(pitch) * sinf(yaw), sinf(pitch), cosf(pitch) * cosf(yaw)};

      // Update camera position and target
      Vector3 cameraOffset = {0.0f, 0.5f, 0.0f};  // Camera height offset from player
      camera.position = Vector3Add(playerPosition, cameraOffset);
      camera.target = Vector3Add(camera.position, direction);

      // calculate forward and right vectors based on yaw
      // Forward vector is based on yaw, right vector is perpendicular to forward
      Vector3 forward = {sinf(yaw), 0.0f, cosf(yaw)};
      Vector3 right = {sinf(yaw + PI / 2), 0.0f, cosf(yaw + PI / 2)};

      // last player position
      Vector3 previousPlayerPosition = playerPosition;

      // Player movement
      if (IsKeyDown(KEY_W)) {
        playerPosition = Vector3Add(playerPosition, Vector3Scale(forward, velocity.x * deltaTime));
      }
      if (IsKeyDown(KEY_S)) {
        playerPosition =
            Vector3Subtract(playerPosition, Vector3Scale(forward, velocity.x * deltaTime));
      }
      if (IsKeyDown(KEY_A)) {
        playerPosition = Vector3Add(playerPosition, Vector3Scale(right, velocity.x * deltaTime));
      }
      if (IsKeyDown(KEY_D)) {
        playerPosition =
            Vector3Subtract(playerPosition, Vector3Scale(right, velocity.x * deltaTime));
      }
      if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        velocity.y = jump_strength;
        isJumping = true;
      }

      velocity.y -= acceleration.y * deltaTime;
      playerPosition.y += velocity.y * deltaTime;

      playerBox.min = {playerPosition.x - playerDimensions.width / 2, playerPosition.y,
                       playerPosition.z - playerDimensions.depth / 2};
      playerBox.max = {playerPosition.x + playerDimensions.width / 2,
                       playerPosition.y + playerDimensions.height,
                       playerPosition.z + playerDimensions.depth / 2};

      // Check for collision with the floor
      for (const auto& floor_bbox : maze_generator.getFloorBBoxes()) {
        if (CheckCollisionBoxes(playerBox, floor_bbox)) {
          playerPosition.y = floor_bbox.max.y;
          velocity.y = 0.0f;
          isJumping = false;

          playerBox.min = {playerPosition.x - playerDimensions.width / 2, playerPosition.y,
                           playerPosition.z - playerDimensions.depth / 2};
          playerBox.max = {playerPosition.x + playerDimensions.width / 2,
                           playerPosition.y + playerDimensions.height,
                           playerPosition.z + playerDimensions.depth / 2};
          break;
        }
      }

      // Check for collision with walls
      for (const auto& wall_bbox : maze_generator.getWallBBoxes()) {
        if (CheckCollisionBoxes(playerBox, wall_bbox)) {
          playerPosition = previousPlayerPosition;  // revert to last position
          playerBox.min = {playerPosition.x - playerDimensions.width / 2, playerPosition.y,
                           playerPosition.z - playerDimensions.depth / 2};
          playerBox.max = {playerPosition.x + playerDimensions.width / 2,
                           playerPosition.y + playerDimensions.height / 2,
                           playerPosition.z + playerDimensions.depth / 2};
        }
      }
    } else {
      frame_count++;

      if (IsKeyPressed(KEY_O)) {
        maze_generator.start_generation();
        frame_count = 0;
      }
      if (IsKeyPressed(KEY_P) && maze_generator.getState() == COMPLETED && !render3d) {
        render3d = true;
      }
      if (IsKeyPressed(KEY_UP)) {
        frame_interval = std::max(1, frame_interval - 1);
      }
      if (IsKeyPressed(KEY_DOWN)) {
        frame_interval = std::min(FPS, frame_interval + 1);
      }

      // reset frame count
      if (frame_count % frame_interval == 0) {
        frame_count = 0;
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (maze_generator.getState() == NOT_STARTED) {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLUE, 0.8f));
      DrawText("Press O Key to start maze generation", 190, 200, 20, BLACK);
    } else if (maze_generator.getState() == COMPLETED && !render3d) {
      DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLUE, 0.8f));
      DrawText("Press P Key to toggle 3D view", 190, 200, 20, BLACK);
    }

    if (!render3d) {
      maze_generator.draw(frame_count, frame_interval);
    } else {
      // 3D rendering
      BeginMode3D(camera);

      // player
      // DrawCylinder(playerPosition, playerDimensions.width / 2, playerDimensions.width / 2,
      //              playerDimensions.height, 16, BLUE);
      // DrawCylinderWires(playerPosition, playerDimensions.width / 2, playerDimensions.width / 2,
      //                   playerDimensions.height, 16, BLUE);
      // DrawBoundingBox(playerBox, RED);

      maze_generator.draw3D();

      // DrawGrid(10, 1.0f);  // Draw a grid for reference

      EndMode3D();
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}