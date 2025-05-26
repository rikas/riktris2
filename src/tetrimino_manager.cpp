#include "tetrimino_manager.h"

Tetrimino *TetriminoManager::generate(TETRIMINO_SHAPE type, int speed) {
  Tetrimino *tetrimino = new Tetrimino(type, speed);
  return tetrimino;
}
