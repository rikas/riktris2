#include "tetrimino_bag.h"

void TetriminoBag::refillBag() {
  currentBag = {TETRIMINO_I, TETRIMINO_O, TETRIMINO_T, TETRIMINO_S,
                TETRIMINO_Z, TETRIMINO_J, TETRIMINO_L};

  // Shuffle the bag
  std::shuffle(currentBag.begin(), currentBag.end(), rng);
}

TETRIMINO_SHAPE TetriminoBag::getNextShape() {
  if (currentBag.empty()) {
    refillBag();
  }

  TETRIMINO_SHAPE nextShape = currentBag.back();
  currentBag.pop_back();
  return nextShape;
}

// Preview next N pieces without consuming them
std::vector<TETRIMINO_SHAPE> TetriminoBag::preview(int count) {
  std::vector<TETRIMINO_SHAPE> result;
  std::vector<TETRIMINO_SHAPE> tempBag = currentBag;

  for (int i = 0; i < count; i++) {
    if (tempBag.empty()) {
      // Simulate refilling bag for preview
      tempBag = {TETRIMINO_I, TETRIMINO_O, TETRIMINO_T, TETRIMINO_S,
                 TETRIMINO_Z, TETRIMINO_J, TETRIMINO_L};
      std::shuffle(tempBag.begin(), tempBag.end(), rng);
    }

    result.push_back(tempBag.back());
    tempBag.pop_back();
  }

  return result;
}

// Get remaining pieces in current bag (for debugging)
int TetriminoBag::remainingInBag() const { return currentBag.size(); }
