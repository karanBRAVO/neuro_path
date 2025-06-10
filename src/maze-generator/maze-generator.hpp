/*
Maze Generation - Graph-based Algorithm

This code implements a maze generation algorithm using a graph-based approach.
*/
#pragma once

#include <memory>
#include <stack>
#include <vector>

#include "raylib.h"
#include "utils/helper.hpp"
#include "utils/types.hpp"

// Enum to represent the state of the maze generation
enum GenerationState { NOT_STARTED, IN_PROGRESS, COMPLETED, FAILED };

// Structure representing a cell in the maze
struct Node {
  int x;          // x-coordinate of the node
  int y;          // y-coordinate of the node
  bool visited;   // flag to check if the node has been visited
  bool walls[4];  // walls around the node (top, right, bottom, left)
  Node* parent;   // pointer to the parent node (for pathfinding)

  Node(int x, int y) : x(x), y(y), visited(false), walls{true, true, true, true}, parent(nullptr) {}
};

// MazeGenerator class to generate a maze
class MazeGenerator {
 private:
  const int CELL_SIZE = 20;                              // size of each cell in the maze
  const int WIDTH;                                       // width of the maze
  const int HEIGHT;                                      // height of the maze
  const int COLS;                                        // number of columns in the maze
  const int ROWS;                                        // number of rows in the maze
  const BoxSize3D floor_dimension = {1.0f, 0.2f, 1.0f};  // dimensions of the floor tile in 3D space
  const float wall_height = 2.0f;                        // height of wall
  const float wall_depth = 0.2f;                         // depth of wall
  int total_path_nodes = 0;                              // total nodes in path at ith frame
  std::vector<std::unique_ptr<Node>> grid;               // 1D array of nodes representing the maze
  std::vector<std::pair<int, int>> directions = {
      {0, -1},  // top
      {1, 0},   // right
      {0, 1},   // bottom
      {-1, 0}   // left
  };
  GenerationState state = NOT_STARTED;    // current state of the maze generation
  std::stack<Node*> stack;                // stack for depth-first search
  std::vector<Node*> path;                // vector to hold the path from start to end
  std::vector<BoundingBox> floor_bboxes;  // vector to hold bounding boxes for floor in 3D
  std::vector<BoundingBox> wall_bboxes;   // vector to hold bounding boxes for walls in 3D

  // Depth-first search algorithm to generate the maze
  void generate();
  // Remove wall between two nodes
  void removeWall(Node* a, Node* b);
  // calculate the path from start to end
  void calcPath();
  // calculate the bounding boxes for the floor and walls in 3D
  void calcBoundingBoxes();
  // Generate a bounding box for a given position and dimensions
  BoundingBox generateBBox(const Vector3& position, const BoxSize3D& dimensions);

 public:
  MazeGenerator(const int& width, const int& height);

  GenerationState getState() const { return state; }
  const std::vector<BoundingBox>& getFloorBBoxes() const { return floor_bboxes; }
  const std::vector<BoundingBox>& getWallBBoxes() const { return wall_bboxes; }

  void start_generation();
  void draw(const int& frame_count, const int& fps);
  void draw3D();
};
