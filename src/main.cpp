#include "camera3d/camera3d.hpp"
#include "maze-generator/maze-generator.hpp"
#include "player/player.hpp"
#include "raylib.h"

void play_sound(const Sound& sound) {
  if (!IsSoundPlaying(sound)) {
    PlaySound(sound);
  }
}

void stop_sound(const Sound& sound) {
  if (IsSoundPlaying(sound)) {
    StopSound(sound);
  }
}

int main() {
  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 400;
  const int FPS = 60;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NeuroPath");

  InitAudioDevice();  // Initialize audio device

  SetTargetFPS(FPS);  // Set target FPS (maximum)
  DisableCursor();    // Disable cursor (lock cursor)

  // maze generator
  MazeGenerator maze_generator(SCREEN_WIDTH, SCREEN_HEIGHT);

  // player
  Player player({0.0f, 10.0f, 0.0f});
  const float gravity = 9.81f;
  const float jumpStrength = 5.0f;
  const float moveSpeed = 1.2f;
  float jumpSpeed = 5.0f;
  bool isJumping = false;

  // camera
  neuro_path::Camera3D camera({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});

  // animation frame control
  int frame_count = 0;
  int frame_interval = FPS / 2;

  bool render3d = false;
  bool showInfo = true;

  // Load textures
  Texture2D wallTexture = LoadTexture("resources/wall_texture.jpg");
  Texture2D floorTexture = LoadTexture("resources/floor_texture.png");

  // Sound and Music effects
  Sound walkSound = LoadSound("resources/walk.mp3");
  Sound runSound = LoadSound("resources/running.mp3");
  Sound jumpLandingSound = LoadSound("resources/jump_land.mp3");
  Music bgMusic = LoadMusicStream("resources/bg.mp3");

  // Main game loop
  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    UpdateMusicStream(bgMusic);

    if (render3d) {
      if (!IsMusicStreamPlaying(bgMusic)) {
        PlayMusicStream(bgMusic);
      }
      UpdateMusicStream(bgMusic);
      camera.update();
      const Vector3 cameraDirection = camera.getDirection();

      // Update camera position and target
      const float shake = sin(GetTime() * 10.0f) * 0.02f;
      const Vector3 cameraOffset = {shake, 0.5f + shake,
                                    shake};  // Camera height offset from player
      camera.setPosition(Vector3Add(player.getPos(), cameraOffset));
      camera.setTarget(Vector3Add(camera.getPosition(), cameraDirection));

      // calculate forward and right vectors based on yaw
      Vector3 forward = camera.getForward();
      Vector3 right = camera.getRight();

      // last player position
      const Vector3 previousPlayerPosition = player.getPos();

      // Player movement
      if (IsKeyDown(KEY_LEFT_CONTROL)) {
        const Vector3 newPos =
            Vector3Add(player.getPos(), Vector3Scale(forward, (moveSpeed * 2) * deltaTime));
        player.setPos(newPos);
        play_sound(runSound);
      } else {
        if (IsKeyDown(KEY_W)) {
          const Vector3 newPos =
              Vector3Add(player.getPos(), Vector3Scale(forward, moveSpeed * deltaTime));
          player.setPos(newPos);
          play_sound(walkSound);
        }
        if (IsKeyDown(KEY_S)) {
          const Vector3 newPos =
              Vector3Subtract(player.getPos(), Vector3Scale(forward, moveSpeed * deltaTime));
          player.setPos(newPos);
          play_sound(walkSound);
        }
        if (IsKeyDown(KEY_A)) {
          const Vector3 newPos =
              Vector3Add(player.getPos(), Vector3Scale(right, moveSpeed * deltaTime));
          player.setPos(newPos);
          play_sound(walkSound);
        }
        if (IsKeyDown(KEY_D)) {
          const Vector3 newPos =
              Vector3Subtract(player.getPos(), Vector3Scale(right, moveSpeed * deltaTime));
          player.setPos(newPos);
          play_sound(walkSound);
        }
      }

      if (!IsKeyDown(KEY_LEFT_CONTROL) || isJumping) {
        stop_sound(runSound);
      }
      if ((!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) ||
          IsKeyDown(KEY_LEFT_CONTROL) || isJumping) {
        stop_sound(walkSound);
      }

      if (IsKeyPressed(KEY_SPACE) && !isJumping) {
        jumpSpeed = jumpStrength;
        isJumping = true;
      }

      // Apply gravity if jumping
      jumpSpeed -= gravity * deltaTime;
      Vector3 newPos = Vector3Add(player.getPos(), {0.0f, jumpSpeed * deltaTime, 0.0f});
      player.setPos(newPos);

      const bool wasJumping = isJumping;

      // Check for collision with the floor
      for (const auto& floor_bbox : maze_generator.getFloorBBoxes()) {
        if (CheckCollisionBoxes(player.getBBox(), floor_bbox)) {
          Vector3 newPos = player.getPos();
          newPos.y = floor_bbox.max.y;
          player.setPos(newPos);
          jumpSpeed = 0.0f;
          isJumping = false;
          break;
        }
      }

      if (wasJumping && !isJumping) {
        play_sound(jumpLandingSound);
      }

      // Check for collision with walls
      for (const auto& wall_bbox : maze_generator.getWallBBoxes()) {
        if (CheckCollisionBoxes(player.getBBox(), wall_bbox)) {
          player.setPos(previousPlayerPosition);
        }
      }
    } else {
      frame_count++;

      if (IsKeyPressed(KEY_I)) {
        showInfo = !showInfo;
      }
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

    if (!render3d) {
      // 2D rendering
      maze_generator.draw(frame_count, frame_interval);

      if (showInfo) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLUE, 0.8f));
        DrawText("Press O Key to start maze generation", 10, 10, 20, BLACK);
        DrawText("Press UP/DOWN Key to change maze generation speed", 10, 30, 20, BLACK);
        DrawText("Press I Key to toggle this info", 10, 50, 20, BLACK);
        if (maze_generator.getState() == IN_PROGRESS) {
          DrawText("Maze generation in progress...", 150, 170, 20, BLACK);
        } else if (maze_generator.getState() == COMPLETED) {
          DrawText("Maze generation completed!", 150, 170, 20, BLACK);
          DrawText("Press P Key to toggle 3D view", 150, 190, 20, BLACK);
        }
      }
    } else {
      // 3D rendering
      BeginMode3D(camera.getCamera());
      // player.draw3D();
      maze_generator.draw3D(false, wallTexture, floorTexture);
      // DrawGrid(10, 1.0f);  // Draw a grid for reference
      EndMode3D();
    }

    EndDrawing();
  }

  // cleanup
  UnloadTexture(wallTexture);
  UnloadTexture(floorTexture);

  UnloadSound(walkSound);
  UnloadSound(runSound);
  UnloadSound(jumpLandingSound);
  UnloadMusicStream(bgMusic);

  CloseAudioDevice();
  CloseWindow();
  return 0;
}