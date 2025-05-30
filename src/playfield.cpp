#include "playfield.h"
#include "line_clear_animation.h"
#include "mino_grid.h"
#include "tetrimino.h"
#include <iostream>
#include <raylib.h>

void Playfield::Update() {
  if (!clearAnimation.isActive)
    return;

  clearAnimation.timer += GetFrameTime();

  switch (clearAnimation.state) {
  case AnimationState::FLASHING: {
    float flashInterval = clearAnimation.flashDuration / clearAnimation.maxFlashes;

    if (clearAnimation.timer >= flashInterval) {
      clearAnimation.flashCount++;
      clearAnimation.timer = 0.0f;

      if (clearAnimation.flashCount >= clearAnimation.maxFlashes) {
        // Move to clearing phase
        clearAnimation.state = AnimationState::CLEARING;
        clearAnimation.timer = 0.0f;

        // TODO: to remove after other animations are implemented
        clearAnimation.isActive = false;
        clearAnimation.state = AnimationState::NONE;
        executeLineClear();
      }
    }
    break;
  }
  case AnimationState::CLEARING: {
    // if (clearAnimation.timer >= clearAnimation.clearDuration) {
    //   // Actually remove the lines from the grid
    //   executeLineClear();
    //
    //   // Move to dropping phase
    //   clearAnimation.state = AnimationState::DROPPING;
    //   clearAnimation.timer = 0.0f;
    //
    //   // Calculate how far each row needs to drop
    //   int linesCleared = clearAnimation.rowsToClear.size();
    //   for (int row = 0; row < GRID_HEIGHT; row++) {
    //     int dropDistance = 0;
    //     for (int clearedRow : clearAnimation.rowsToClear) {
    //       if (clearedRow < row) {
    //         dropDistance++;
    //       }
    //     }
    //     rowDropOffsets[row] = -dropDistance * (MINO_W + 1); // Negative = up
    //   }
    // }
    break;
  }

  case AnimationState::DROPPING: {
    // // Animate rows dropping down smoothly
    // float progress = clearAnimation.timer / clearAnimation.dropDuration;
    // progress = std::min(1.0f, progress);
    //
    // float easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress); // Ease out
    //
    // // Update drop offsets
    // int linesCleared = clearAnimation.rowsToClear.size();
    // for (int row = 0; row < GRID_HEIGHT; row++) {
    //   int dropDistance = 0;
    //   for (int clearedRow : clearAnimation.rowsToClear) {
    //     if (row < clearedRow) {
    //       dropDistance++;
    //     }
    //   }
    //
    //   if (dropDistance > 0) {
    //     float targetOffset = dropDistance * (MINO_W + 1);
    //     rowDropOffsets[row] = -targetOffset + (targetOffset * easedProgress);
    //   }
    // }
    //
    // if (progress >= 1.0f) {
    //   // Animation complete
    //   clearAnimation.isActive = false;
    //   clearAnimation.state = AnimationState::NONE;
    //   std::fill(rowDropOffsets.begin(), rowDropOffsets.end(), 0.0f);
    // }
    break;
  }

  case AnimationState::NONE:
  default:
    break;
  }
}

void Playfield::Draw() {
  DrawTexture(playfieldTexture, position.x, position.y, WHITE);

  grid.drawAnimated(drawStart.x, drawStart.y, clearAnimation);
  // if (clearAnimation.isActive) {
  // } else {
  //   grid.Draw(drawStart.x, drawStart.y);
  // }
}

// void Playfield::drawWithAnimation() {
//
//   const auto &rowsToClear = clearAnimation.rowsToClear;
//
//   switch (clearAnimation.state) {
//   case AnimationState::FLASHING: {
//     // Alternate the visibility of the clearing rows
//     for (int row = 0; row < GRID_HEIGHT; ++row) {
//       bool isClearing = std::find(rowsToClear.begin(), rowsToClear.end(), row) !=
//       rowsToClear.end(); if (isClearing) {
//         // Flash: show/hide based on even/odd flashCount
//         if (clearAnimation.flashCount % 2 == 0) {
//           grid.DrawRow(row, drawStart.x, drawStart.y + row * (MINO_W + 1));
//         }
//       } else {
//         grid.DrawRow(row, drawStart.x, drawStart.y + row * (MINO_W + 1));
//       }
//     }
//     break;
//   }
//   case AnimationState::CLEARING: {
//     // Draw all rows normally (or optionally fade out the clearing rows)
//     for (int row = 0; row < GRID_HEIGHT; ++row) {
//       grid.DrawRow(row, drawStart.x, drawStart.y + row * (MINO_W + 1));
//     }
//     break;
//   }
//   case AnimationState::DROPPING: {
//     // Shift rows visually by rowDropOffsets[row]
//     for (int row = 0; row < GRID_HEIGHT; ++row) {
//       float offset = rowDropOffsets[row];
//       grid.DrawRow(row, drawStart.x, drawStart.y + row * (MINO_W + 1) + offset);
//     }
//     break;
//   }
//   default:
//     // Fallback: draw grid as normal
//     grid.Draw(drawStart.x, drawStart.y);
//     break;
//   }
// }

void Playfield::executeLineClear() { grid.removeCompletedRows(); }

bool Playfield::TetriminoOverlapping(Tetrimino *tetrimino) const {
  int tetCol = tetrimino->getCol();
  int tetRow = tetrimino->getRow();

  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      if (tetrimino->isFilled(x, y)) {
        // The square is overlapping an existing square in the playfield.
        if (grid.matrix[tetCol + x][tetRow + y]) {
          return true;
        }
        // The square is outside of the playfield!
        if ((tetCol + x) > ((int)GRID_WIDTH - 1) || (tetRow + y) > ((int)GRID_HEIGHT - 1) ||
            (tetCol + x) < 0) {
          return true;
        }
      }
    }
  }

  return false;
}

// Check if the tetrimino is touching the left margin of the playfield or exiting squares in the
// grid matrix. If it is, return true.
bool Playfield::isTouchingLeft(Tetrimino *tetrimino) const {
  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      if (tetrimino->isFilled(x, y)) {
        // Touching the left wall
        if (tetrimino->getCol() + x <= 0) {
          return true;
        }

        // Touching existing squares in the field matrix
        if (grid.matrix[tetrimino->getCol() + x - 1][tetrimino->getRow() + y]) {
          return true;
        }
      }
    }
  }
  return false;
}

// Check if the tetrimino is touching the right margin of the playfield or exiting squares in the
// grid matrix. If it is, return true.
bool Playfield::isTouchingRight(Tetrimino *tetrimino) const {
  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      if (tetrimino->isFilled(x, y)) {
        // Touching the right wall
        if (tetrimino->getCol() + x >= GRID_WIDTH - 1) {
          return true;
        }

        // Touching existing squares in the field matrix
        if (grid.matrix[tetrimino->getCol() + x + 1][tetrimino->getRow() + y]) {
          return true;
        }
      }
    }
  }
  return false;
}

// Check if the tetrimino is touching the bottom margin of the playfield or exiting squares in the
// grid matrix. If it is, return true.
bool Playfield::isTouchingDown(Tetrimino *tetrimino) const {
  for (int x = 0; x < NUMBER_OF_ROTATIONS; x++) {
    for (int y = 0; y < NUMBER_OF_ROTATIONS; y++) {
      if (tetrimino->isFilled(x, y)) {
        // Touching the bottom wall
        if (tetrimino->getRow() + y >= GRID_HEIGHT - 1) {
          return true;
        }

        // Touching existing squares in the field matrix
        if (grid.matrix[tetrimino->getCol() + x][tetrimino->getRow() + y + 1]) {
          return true;
        }
      }
    }
  }
  return false;
}

void Playfield::startLineClearAnimation(const std::vector<int> &rows) {
  if (rows.empty()) {
    return;
  }

  clearAnimation.rowsToClear = rows;
  clearAnimation.state = AnimationState::FLASHING;
  clearAnimation.timer = 0.0f;
  clearAnimation.flashCount = 0;
  clearAnimation.isActive = true;

  // Backup current grid state to tempGrid
  tempGrid = grid;

  // Initialize row drop offsets
  rowDropOffsets.clear();
  rowDropOffsets.resize(GRID_HEIGHT, 0.0f);
}
