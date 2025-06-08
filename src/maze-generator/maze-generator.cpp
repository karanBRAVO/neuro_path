#include "maze-generator.hpp"

MazeGenerator::MazeGenerator(int width, int height)
    : WIDTH(width), HEIGHT(height), COLS(width / CELL_SIZE), ROWS(height / CELL_SIZE) {
  // Initialize the grid with Node pointers
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      grid.push_back(std::make_unique<Node>(j, i));
    }
  }
}

void MazeGenerator::generate() {
  dfs();
}

void MazeGenerator::dfs() {
  Node* start_node = grid[0].get();  // Start from the first node
  stack.push(start_node);            // Start from the first node
  start_node->visited = true;        // Mark the starting node as visited

  // loop until the stack is empty
  while (!stack.empty()) {
    Node* current = stack.top();   // get the current node from the stack
    std::vector<Node*> neighbors;  // vector to hold unvisited neighbors

    // Check all four possible directions (top, right, bottom, left)
    for (auto& direction : directions) {
      int nx = current->x + direction.first;   // x-coordinate of the neighbor
      int ny = current->y + direction.second;  // y-coordinate of the neighbor
      // Check if the neighbor is within bounds
      if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS) {
        Node* neighbor = grid[ny * COLS + nx].get();  // get the neighbor node (2D to 1D index)
        // Check if the neighbor has not been visited
        if (!neighbor->visited) {
          neighbors.push_back(neighbor);
        }
      }
    }

    // If there are unvisited neighbors, choose one randomly
    if (!neighbors.empty()) {
      Node* next_node = helper::getRandomElement(neighbors).value();
      next_node->parent = current;     // Set the parent of the next node
      removeWall(current, next_node);  // Remove the wall between current and next_node
      next_node->visited = true;       // Mark the next node as visited
      stack.push(next_node);           // Push the next node onto the stack
    } else {
      stack.pop();  // Backtrack if no unvisited neighbors
    }
  }

  // remove entry and exit walls
  grid[0]->walls[3] = false;
  grid[grid.size() - 1]->walls[1] = false;
}

void MazeGenerator::removeWall(Node* a, Node* b) {
  int dx = b->x - a->x;
  int dy = b->y - a->y;

  if (dx == 1) {          // b is to the right of a
    a->walls[1] = false;  // Remove right wall of a
    b->walls[3] = false;  // Remove left wall of b
  } else if (dx == -1) {  // b is to the left of a
    a->walls[3] = false;  // Remove left wall of a
    b->walls[1] = false;  // Remove right wall of b
  }
  if (dy == 1) {          // b is below a
    a->walls[2] = false;  // Remove bottom wall of a
    b->walls[0] = false;  // Remove top wall of b
  } else if (dy == -1) {  // b is above a
    a->walls[0] = false;  // Remove top wall of a
    b->walls[2] = false;  // Remove bottom wall of b
  }
}

std::vector<Node*> MazeGenerator::getPath() const {
  std::vector<Node*> path;
  Node* current = grid[grid.size() - 1].get();  // Start from the end node (bottom-right corner)

  while (current) {
    path.push_back(current);
    current = current->parent;
  }

  std::reverse(path.begin(), path.end());  // Reverse the path to go from start to end
  return path;
}

void MazeGenerator::draw() const {
  // Draw the path from start to end
  std::vector<Node*> path = getPath();
  for (const auto& node : path) {
    int x = node->x * CELL_SIZE;
    int y = node->y * CELL_SIZE;
    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, GREEN);
  }

  // Draw the maze grid
  for (const auto& cell : grid) {
    int x = cell->x * CELL_SIZE;
    int y = cell->y * CELL_SIZE;

    // Draw walls based on the wall flags
    if (cell->walls[0]) {  // Top wall
      DrawLine(x, y, x + CELL_SIZE, y, BLACK);
    }
    if (cell->walls[1]) {  // Right wall
      DrawLine(x + CELL_SIZE, y, x + CELL_SIZE, y + CELL_SIZE, BLACK);
    }
    if (cell->walls[2]) {  // Bottom wall
      DrawLine(x, y + CELL_SIZE, x + CELL_SIZE, y + CELL_SIZE, BLACK);
    }
    if (cell->walls[3]) {  // Left wall
      DrawLine(x, y, x, y + CELL_SIZE, BLACK);
    }
  }

  // Draw the start and end points
  DrawRectangle(0, 0, CELL_SIZE, CELL_SIZE, BLUE);  // Start point
  DrawRectangle((COLS - 1) * CELL_SIZE, (ROWS - 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
}
