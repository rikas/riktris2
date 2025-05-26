#pragma once

#include "tetrimino.h"
#include <raylib.h>
#include <string>
#include <vector>

class TetriminoManager {
private:
  std::vector<Texture2D> minoTextures;
  std::vector<Texture2D> ghostTextures;

public:
  TetriminoManager() = default;
  ~TetriminoManager() = default;
  Tetrimino *generate(TETRIMINO_SHAPE type, int speed);
};
