#include "mino_grid.h"
#include "tetrimino.h"
#include "texture_manager.h"

// When drawing the matrix we are drawing the minos already in place in the grid.
void MinoGrid::Draw(int offsetX, int offsetY) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (matrix[x][y]) {
        int minoType = matrix[x][y] - 1; // Adjust for zero-based index
        const Texture2D &minoTexture = TextureManager::getInstance().getTexture(
            "data/gfx/mino_" + MINO_NAMES[minoType] + ".png");

        int posX = x * (minoTexture.width + 1);
        int posY = y * (minoTexture.height + 1);

        // Draw the mino by getting the right mino gfx. If we have 1 in the
        // matrix then the mino is zero, since TETRIMINO_TYPE starts at 0.
        DrawTexture(minoTexture, posX + offsetX, posY + offsetY, WHITE);
      }
    }
  }
}

void MinoGrid::addTetrimino(Tetrimino *tetrimino) {
  int tetCol = tetrimino->getCol();
  int tetRow = tetrimino->getRow();

  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      if (tetrimino->isFilled(x, y)) {
        // Add the mino to the matrix. The number will be the mino type + 1 since we can't have it
        // as zero (if the type == MINO_T).
        matrix[tetCol + x][tetRow + y] = tetrimino->getShape() + 1;
      }
    }
  }
}

bool MinoGrid::isValidRowNumber(int rowNumber) const {
  return rowNumber >= 0 && rowNumber < GRID_HEIGHT;
}

bool MinoGrid::isRowComplete(int rowNumber) const {
  if (!isValidRowNumber(rowNumber)) {
    return false; // Invalid row index
  }

  for (int col = 0; col < GRID_WIDTH; col++) {
    if (!matrix[col][rowNumber]) {
      return false; // Found an empty square in the row
    }
  }

  return true;
}

std::vector<int> MinoGrid::getCompletedRows() const {
  std::vector<int> completedRows;

  for (int row = GRID_HEIGHT - 1; row >= 0; row--) {
    if (isRowComplete(row)) {
      completedRows.push_back(row);
    }
  }

  return completedRows;
}

int MinoGrid::removeCompletedRows() {
  std::vector<int> completedRows = getCompletedRows();
  int rowsCleared = completedRows.size();

  if (rowsCleared == 0) {
    return 0; // No lines to clear
  }

  // Remove rows from bottom to top to avoid index shifting issues.
  // Since completedRows is already sorted from bottom to top, we can iterate through it directly.
  for (int row : completedRows) {
    removeRow(row);
  }

  return rowsCleared;
}

void MinoGrid::removeRow(int rowNumber) {
  if (!isValidRowNumber(rowNumber)) {
    return; // Invalid row index
  }

  for (int col = 0; col < GRID_WIDTH; col++) {
    matrix[col][rowNumber] = 0; // Clear the row
  }

  // Move all rows above down
  for (int row = rowNumber - 1; row >= 0; row--) {
    for (int col = 0; col < GRID_WIDTH; col++) {
      matrix[col][row + 1] = matrix[col][row];
    }
  }
}
