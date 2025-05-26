#pragma once

#include "tetrimino.h"
#include <random>
#include <vector>

class TetriminoBag {
private:
  std::vector<TETRIMINO_SHAPE> currentBag;
  std::mt19937 rng;

  void refillBag();

public:
  TetriminoBag() : rng(std::random_device{}()) { refillBag(); }

  TETRIMINO_SHAPE getNextShape();

  // Preview next N pieces without consuming them
  std::vector<TETRIMINO_SHAPE> preview(int count);

  // Get remaining pieces in current bag (for debugging)
  int remainingInBag() const;
};
