/*
Maze Generation - Graph-based Algorithm

This code implements a maze generation algorithm using a graph-based approach.
*/
#ifndef MAZE_GENERATOR_HPP
#define MAZE_GENERATOR_HPP
#pragma once

#include <memory>
#include <stack>
#include <vector>

#include "raylib.h"
#include "utils/helper.hpp"

// Structure representing a cell in the maze
struct Node {
  int x;          // x-coordinate of the node
  int y;          // y-coordinate of the node
  bool visited;   // flag to check if the node has been visited
  bool walls[4];  // walls around the node (top, right, bottom, left)
  Node* parent;   // pointer to the parent node (for pathfinding)

  Node(int x, int y) : x(x), y(y), visited(false), walls{true, true, true, true}, parent(nullptr) {}
  ~Node() = default;
};

// MazeGenerator class to generate a maze
class MazeGenerator {
 private:
  const int CELL_SIZE = 20;                 // size of each cell in the maze
  const int WIDTH;                          // width of the maze
  const int HEIGHT;                         // height of the maze
  const int COLS;                           // number of columns in the maze
  const int ROWS;                           // number of rows in the maze
  std::vector<std::unique_ptr<Node>> grid;  // 1D array of nodes representing the maze
  std::vector<std::pair<int, int>> directions = {
      {0, -1},  // top
      {1, 0},   // right
      {0, 1},   // bottom
      {-1, 0}   // left
  };
  std::stack<Node*> stack;  // stack for depth-first search

  // Depth-first search algorithm to generate the maze
  void dfs();
  // Remove wall between two nodes
  void removeWall(Node* a, Node* b);
  // returns the path from the start to the end of the maze
  std::vector<Node*> getPath() const;

 public:
  MazeGenerator(int width, int height);

  // generate the maze using depth-first search
  void generate();
  // draw the maze on the screen
  void draw() const;
};

#endif
