#include "mino_grid.h"
#include "line_clear_animation.h"
#include "tetrimino.h"
#include "texture_manager.h"
#include <iostream>
#include <ostream>

// When drawing the matrix we are drawing the minos already in place in the grid.
void MinoGrid::Draw(int offsetX, int offsetY) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (matrix[x][y]) {
        int minoType = matrix[x][y] - 1; // Adjust for zero-based index
        const Texture2D &minoTexture =
            TextureManager::getInstance().getTexture("mino_" + MINO_NAMES[minoType] + ".png");

        int posX = x * (minoTexture.width + 1);
        int posY = y * (minoTexture.height + 1);

        // Draw the mino by getting the right mino gfx. If we have 1 in the
        // matrix then the mino is zero, since TETRIMINO_TYPE starts at 0.
        DrawTexture(minoTexture, posX + offsetX, posY + offsetY, WHITE);
      }
    }
  }
}

void MinoGrid::drawAnimated(int offsetX, int offsetY, const LineClearAnimation &animation) {
  for (int row = 0; row < width; row++) {
    for (int col = 0; col < height; col++) {
      if (matrix[row][col]) {
        int minoType = matrix[row][col] - 1; // Adjust for zero-based index
        const Texture2D &minoTexture =
            TextureManager::getInstance().getTexture("mino_" + MINO_NAMES[minoType] + ".png");

        int posX = row * (minoTexture.width + 1);
        int posY = col * (minoTexture.height + 1);

        if (animation.isActive && animation.rowsToClear.size() > 0 &&
            std::find(animation.rowsToClear.begin(), animation.rowsToClear.end(), col) !=
                animation.rowsToClear.end()) {
          // If the animation is flashing, we might want to skip drawing this mino
          if (animation.flashCount % 2 == 0 && animation.state == AnimationState::FLASHING) {
            DrawTexture(minoTexture, posX + offsetX, posY + offsetY, WHITE);
          }
        } else {
          // Draw the mino by getting the right mino gfx. If we have 1 in the
          // matrix then the mino is zero, since TETRIMINO_TYPE starts at 0.
          DrawTexture(minoTexture, posX + offsetX, posY + offsetY, WHITE);
        }
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
        int gridX = tetCol + x;
        int gridY = tetRow + y;

        if (gridX >= 0 && gridX < width && gridY < height) {
          // Add the mino to the matrix. The number will be the mino type + 1 since we can't have it
          // as zero (if the type == MINO_T).
          matrix[tetCol + x][tetRow + y] = tetrimino->getShape() + 1;
        }
      }
    }
  }
}

bool MinoGrid::isValidRowNumber(int rowNumber) const {
  return rowNumber >= 0 && rowNumber < height;
}

bool MinoGrid::isRowComplete(int rowNumber) const {
  if (!isValidRowNumber(rowNumber)) {
    return false; // Invalid row index
  }

  for (int col = 0; col < width; col++) {
    if (!matrix[col][rowNumber]) {
      return false; // Found an empty square in the row
    }
  }

  return true;
}

// Returns a vector of completed rows, starting from the bottom of the grid. { 19, 18, 17, ... }
std::vector<int> MinoGrid::getCompletedRows() const {
  std::vector<int> completedRows;

  for (int row = height - 1; row >= 0; row--) {
    if (isRowComplete(row)) {
      completedRows.push_back(row);
    }
  }

  return completedRows;
}

int MinoGrid::removeCompletedRows() {
  std::vector<int> completedRows = getCompletedRows();
  int rowsCleared = completedRows.size();

  // No lines to clear
  if (rowsCleared == 0) {
    return 0;
  }

  // Create a new grid with completed rows removed
  for (int col = 0; col < width; ++col) {
    int writeRow = height - 1; // Start from bottom

    // Copy non-completed rows from bottom to top
    for (int readRow = height - 1; readRow >= 0; --readRow) {
      // Check if this row should be kept (not in completedRows)
      bool keepRow =
          std::find(completedRows.begin(), completedRows.end(), readRow) == completedRows.end();

      if (keepRow) {
        matrix[col][writeRow] = matrix[col][readRow];
        --writeRow;
      }
    }

    // Fill remaining top rows with zeros
    while (writeRow >= 0) {
      matrix[col][writeRow] = 0;
      --writeRow;
    }
  }

  return rowsCleared;
}
