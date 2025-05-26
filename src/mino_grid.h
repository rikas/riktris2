#pragma once

#include "line_clear_animation.h"
#include "tetrimino.h"

// Number of squares in the playfield grid. There are 10x20 squares where minos can be placed.
#define GRID_WIDTH 10u
#define GRID_HEIGHT 20u

// TODO: we can take this from the texture
// Pixel width of a single mino in the grid (a square that forms the tetriminos)
#define MINO_W 25

class MinoGrid {
private:
  uint width = GRID_WIDTH;
  uint height = GRID_HEIGHT;

public:
  MinoGrid() {
    matrix[9][19] = 2;
    matrix[8][19] = 3;
    matrix[7][19] = 4;
    matrix[6][19] = 2;
    matrix[5][19] = 3;
    matrix[4][19] = 4;
    matrix[3][19] = 4;
    matrix[2][19] = 4;
    matrix[1][19] = 4;
  }
  ~MinoGrid() = default;
  int matrix[GRID_WIDTH][GRID_HEIGHT] = {{0}};
  void Draw(int offsetX, int offsetY);
  void drawAnimated(int offsetX, int offsetY, const LineClearAnimation &animation);
  void Update();

  // Functions that don't modify the grid matrix
  bool isRowComplete(int rowNumber) const;
  bool isValidRowNumber(int rowNumber) const;
  std::vector<int> getCompletedRows() const;

  // Functions that modify the grid matrix
  int removeCompletedRows();
  void addTetrimino(Tetrimino *tetrimino);
};
