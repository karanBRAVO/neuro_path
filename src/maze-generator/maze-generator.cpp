#include "maze-generator.hpp"

MazeGenerator::MazeGenerator(const int& width, const int& height)
    : WIDTH(width), HEIGHT(height), COLS(width / CELL_SIZE), ROWS(height / CELL_SIZE) {
  // Initialize the grid with Node pointers
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      grid.push_back(std::make_unique<Node>(j, i));
    }
  }
}

void MazeGenerator::start_generation() {
  if (state != NOT_STARTED) return;

  state = IN_PROGRESS;  // Set the state to in progress

  Node* start_node = grid[0].get();  // Start from the first node
  stack.push(start_node);            // Start from the first node
  start_node->visited = true;        // Mark the starting node as visited

  generate();  // Start the depth-first search to generate the maze
}

void MazeGenerator::generate() {
  if (state != IN_PROGRESS) return;

  if (stack.empty()) {
    state = COMPLETED;
    calcPath();
    calcBoundingBoxes();
    return;
  }

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

void MazeGenerator::removeWall(Node* a, Node* b) {
  int dx = b->x - a->x;
  int dy = b->y - a->y;

  if (dx == 1) {          // b is to the right of a
    a->walls[1] = false;  // right wall
    b->walls[3] = false;  // left wall
  } else if (dx == -1) {  // b is to the left of a
    a->walls[3] = false;  // left wall
    b->walls[1] = false;  // right wall
  }
  if (dy == 1) {          // b is below a
    a->walls[2] = false;  // bottom wall
    b->walls[0] = false;  // top wall
  } else if (dy == -1) {  // b is above a
    a->walls[0] = false;  // top wall
    b->walls[2] = false;  // bottom wall
  }
}

void MazeGenerator::calcPath() {
  Node* current = grid[grid.size() - 1].get();  // Start from the end node (bottom-right corner)

  while (current) {
    path.push_back(current);
    current = current->parent;
  }

  std::reverse(path.begin(), path.end());  // Reverse the path to go from start to end
}

void MazeGenerator::draw(const int& frame_count, const int& fps) {
  // Draw the maze grid
  for (const auto& cell : grid) {
    int x = cell->x * CELL_SIZE;
    int y = cell->y * CELL_SIZE;

    if (state == IN_PROGRESS && cell->visited) {
      DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, Fade(LIGHTGRAY, 0.5f));
    }

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

  if (state == IN_PROGRESS) {
    if (!stack.empty()) {
      Node* current = stack.top();  // Get the current node from the stack
      int x = current->x * CELL_SIZE;
      int y = current->y * CELL_SIZE;
      DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, PURPLE);  // Highlight the current node
    }
    if (frame_count % fps == 0) {
      generate();
    }
  } else if (state == COMPLETED) {
    for (int i = 0; i < total_path_nodes; i++) {
      Node* node = path[i];
      int x = node->x * CELL_SIZE;
      int y = node->y * CELL_SIZE;
      int t = 5;
      DrawRectangle(x + t, y + t, CELL_SIZE - 2 * t, CELL_SIZE - 2 * t,
                    GREEN);  // Highlight the path
    }
    if (frame_count % fps == 0) {
      if (total_path_nodes < path.size()) {
        total_path_nodes++;
      }
    }
  }

  if (state != NOT_STARTED) {
    // start and end points
    DrawRectangle(0, 0, CELL_SIZE, CELL_SIZE, MAGENTA);
    DrawRectangle((COLS - 1) * CELL_SIZE, (ROWS - 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
  }
}

BoundingBox MazeGenerator::generateBBox(const Vector3& position, const BoxSize3D& dimensions) {
  return {{position.x - dimensions.width / 2, position.y - dimensions.height / 2,
           position.z - dimensions.depth / 2},
          {position.x + dimensions.width / 2, position.y + dimensions.height / 2,
           position.z + dimensions.depth / 2}};
}

void MazeGenerator::calcBoundingBoxes() {
  for (const auto& cell : grid) {
    // floor bbox
    const Vector3 floor_pos = {cell->x * floor_dimension.width, 0.0f,
                               cell->y * floor_dimension.depth};
    const BoundingBox floor_bbox = generateBBox(floor_pos, floor_dimension);
    floor_bboxes.push_back(floor_bbox);

    // wall bbox
    const BoxSize3D top_bottom_walls_dimensions = {floor_dimension.width, wall_height, wall_depth};
    const BoxSize3D right_left_walls_dimensions = {wall_depth, wall_height, floor_dimension.depth};

    if (cell->walls[0]) {
      const Vector3 top_wall_pos = {floor_pos.x, floor_pos.y + wall_height / 2,
                                    floor_pos.z - floor_dimension.depth / 2};

      const BoundingBox top_wall_bbox = generateBBox(top_wall_pos, top_bottom_walls_dimensions);
      wall_bboxes.push_back(top_wall_bbox);
    }
    if (cell->walls[1]) {
      const Vector3 right_wall_pos = {floor_pos.x + floor_dimension.width / 2,
                                      floor_pos.y + wall_height / 2, floor_pos.z};

      const BoundingBox right_wall_bbox = generateBBox(right_wall_pos, right_left_walls_dimensions);
      wall_bboxes.push_back(right_wall_bbox);
    }
    if (cell->walls[2]) {
      const Vector3 bottom_wall_pos = {floor_pos.x, floor_pos.y + wall_height / 2,
                                       floor_pos.z + floor_dimension.depth / 2};

      const BoundingBox bottom_wall_bbox =
          generateBBox(bottom_wall_pos, top_bottom_walls_dimensions);
      wall_bboxes.push_back(bottom_wall_bbox);
    }
    if (cell->walls[3]) {
      const Vector3 left_wall_pos = {floor_pos.x - floor_dimension.width / 2,
                                     floor_pos.y + wall_height / 2, floor_pos.z};

      const BoundingBox left_wall_bbox = generateBBox(left_wall_pos, right_left_walls_dimensions);
      wall_bboxes.push_back(left_wall_bbox);
    }
  }
}

void MazeGenerator::draw3D() {
  if (state != COMPLETED) return;

  for (const auto& cell : grid) {
    // floor tiles
    const Vector3 floor_pos = {cell->x * floor_dimension.width, 0.0f,
                               cell->y * floor_dimension.depth};
    Color floor_color = PURPLE;
    if (cell->x == 0 && cell->y == 0)
      floor_color = DARKPURPLE;
    else if (cell->x == ROWS - 1 && cell->y == COLS - 1)
      floor_color = RED;
    DrawCube(floor_pos, floor_dimension.width, floor_dimension.height, floor_dimension.depth,
             floor_color);
    // DrawBoundingBox(floor_bboxes[cell->y * COLS + cell->x], RED);

    // walls
    const BoxSize3D top_bottom_walls_dimensions = {floor_dimension.width, wall_height, wall_depth};
    const BoxSize3D right_left_walls_dimensions = {wall_depth, wall_height, floor_dimension.depth};
    if (cell->walls[0]) {  // top
      const Vector3 top_wall_pos = {floor_pos.x, floor_pos.y + wall_height / 2,
                                    floor_pos.z - floor_dimension.depth / 2};
      DrawCube(top_wall_pos, top_bottom_walls_dimensions.width, top_bottom_walls_dimensions.height,
               top_bottom_walls_dimensions.depth, BLACK);
    }
    if (cell->walls[1]) {  // right
      const Vector3 right_wall_pos = {floor_pos.x + floor_dimension.width / 2,
                                      floor_pos.y + wall_height / 2, floor_pos.z};
      DrawCube(right_wall_pos, right_left_walls_dimensions.width,
               right_left_walls_dimensions.height, right_left_walls_dimensions.depth, BLACK);
    }
    if (cell->walls[2]) {  // bottom
      const Vector3 bottom_wall_pos = {floor_pos.x, floor_pos.y + wall_height / 2,
                                       floor_pos.z + floor_dimension.depth / 2};
      DrawCube(bottom_wall_pos, top_bottom_walls_dimensions.width,
               top_bottom_walls_dimensions.height, top_bottom_walls_dimensions.depth, BLACK);
    }
    if (cell->walls[3]) {  // left
      const Vector3 left_wall_pos = {floor_pos.x - floor_dimension.width / 2,
                                     floor_pos.y + wall_height / 2, floor_pos.z};
      DrawCube(left_wall_pos, right_left_walls_dimensions.width, right_left_walls_dimensions.height,
               right_left_walls_dimensions.depth, BLACK);
    }
  }

  // for (const auto& wall_bbox : wall_bboxes) {
  //   DrawBoundingBox(wall_bbox, RED);
  // }

  // draw the path
  for (const auto& cell : path) {
    const BoxSize3D path_tile_dimension = {0.5f, 0.2f, 0.5f};
    const Vector3 path_cell_pos = {cell->x * floor_dimension.width, 0.1f,
                                   cell->y * floor_dimension.depth};
    DrawCube(path_cell_pos, path_tile_dimension.width, path_tile_dimension.height,
             path_tile_dimension.depth, GREEN);
  }
}
